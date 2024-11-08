#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h> // Для использования сигнала SIGKILL

#include "find_min_max.h"
#include "utils.h"

#define FILENAME_TEMPLATE "result_%d.txt"

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;
    int timeout = -1; // Таймаут по умолчанию

    // Парсинг аргументов
    while (true) {
        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 't'}, // Новая опция для таймаута
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f:t:", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        break;
                    case 3:
                        with_files = true;
                        break;
                    case 4:
                        timeout = atoi(optarg); // Получение значения таймаута
                        break;
                }
                break;
            case 'f':
                with_files = true;
                break;
            case 't':
                timeout = atoi(optarg); // Получение значения таймаута
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Pipes for IPC
    int pipe_fd[2];
    if (!with_files) {
        if (pipe(pipe_fd) == -1) {
            perror("pipe");
            return 1;
        }
    }

    pid_t child_pids[pnum]; // Массив для хранения PID дочерних процессов

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            child_pids[i] = child_pid; // Сохраняем PID дочернего процесса
            active_child_processes += 1;

            if (child_pid == 0) { // Дочерний процесс
                // Вычисление диапазона для этого дочернего процесса
                int start_index = i * (array_size / pnum);
                int end_index = (i + 1) * (array_size / pnum);
                if (i == pnum - 1) { // Последний дочерний процесс берет оставшиеся элементы
                    end_index = array_size;
                }

                // Нахождение минимума и максимума в этом диапазоне
                struct MinMax local_min_max = GetMinMax(array, start_index, end_index);

                if (with_files) {
                    // Запись результатов в файл
                    char filename[256];
                    snprintf(filename, sizeof(filename), FILENAME_TEMPLATE, i);
                    FILE *file = fopen(filename, "w");
                    if (file) {
                        fprintf(file, "%d %d\n", local_min_max.min, local_min_max.max);
                        fclose(file);
                    } else {
                        perror("fopen");
                    }
                } else {
                    // Запись результатов в канал
                    close(pipe_fd[0]); // Закрытие конца чтения
                    write(pipe_fd[1], &local_min_max, sizeof(local_min_max));
                    close(pipe_fd[1]); // Закрытие конца записи
                }

                free(array); // Освобождение памяти в дочернем процессе
                return 0; // Успешный выход из дочернего процесса
            }
        } else {
            perror("Fork failed!");
            return 1;
        }
    }

    // Ожидание с таймаутом
    if (timeout > 0) {
        sleep(timeout); // Ожидание заданного времени

        // Отправка SIGKILL всем дочерним процессам
        for (int i = 0; i < pnum; i++) {
            kill(child_pids[i], SIGKILL); // Убиваем каждый дочерний процесс по его PID
        }
    }

    while (active_child_processes > 0) {
        wait(NULL); // Ожидание завершения всех дочерних процессов
        active_child_processes -= 1; 
    }

    struct MinMax min_max;
    min_max.min = INT_MAX; 
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        if (with_files) {
            char filename[256];
            snprintf(filename, sizeof(filename), FILENAME_TEMPLATE, i);
            FILE *file = fopen(filename, "r");
            if (file) {
                struct MinMax local_min_max;
                fscanf(file, "%d %d", &local_min_max.min, &local_min_max.max);
                fclose(file);

                if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;
                if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;

                remove(filename); // Удаление файла после чтения
            } else {
                perror("fopen");
            }
        } else {
            struct MinMax local_min_max;
            read(pipe_fd[0], &local_min_max, sizeof(local_min_max));

            if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;
            if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;
        }
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0; 
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array); 

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);

    return 0; 
}
//./parallel_min_max --seed 12345 --array_size 100 --pnum 4