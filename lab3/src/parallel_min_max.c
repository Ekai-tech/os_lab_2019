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

#include "find_min_max.h" 
#include "utils.h" 

#define FILENAME_TEMPLATE "result_%d.txt" // Шаблон для имен файлов для хранения результатов

int main(int argc, char **argv) { 
    int seed = -1; // Семя для генерации случайных чисел
    int array_size = -1; // Размер массива
    int pnum = -1; // Количество дочерних процессов
    bool with_files = false; // Флаг, указывающий, нужно ли записывать результаты в файлы

    // Парсинг аргументов
    while (true) { 
        int current_optind = optind ? optind : 1; 

        static struct option options[] = { 
            {"seed", required_argument, 0, 0}, // Опция для семени
            {"array_size", required_argument, 0, 0}, // Опция для размера массива
            {"pnum", required_argument, 0, 0}, // Опция для количества процессов
            {"by_files", no_argument, 0, 'f'}, // Опция для указания вывода в файлы
            {0, 0, 0, 0} 
        }; 

        int option_index = 0; 
        int c = getopt_long(argc, argv, "f", options, &option_index); 

        if (c == -1) break; // Выход из цикла, когда больше нет опций 

        switch (c) { 
            case 0: 
                switch (option_index) { 
                    case 0: 
                        seed = atoi(optarg); // Преобразование аргумента семени в целое число
                        break; 
                    case 1: 
                        array_size = atoi(optarg); // Преобразование аргумента размера массива в целое число
                        break; 
                    case 2: 
                        pnum = atoi(optarg); // Преобразование аргумента количества процессов в целое число
                        break; 
                    case 3: 
                        with_files = true; // Установка флага в true, если используются файлы
                        break; 

                    default: 
                        printf("Индекс %d выходит за пределы опций\n", option_index); // Обработка неожиданного индекса
                } 
                break; 
            case 'f': 
                with_files = true; // Установка флага в true, если используются файлы
                break; 

            case '?': 
                break; // Обработка неизвестных опций

            default: 
                printf("getopt вернул код символа 0%o?\n", c); // Обработка неожиданных кодов возврата
        } 
    } 

    if (optind < argc) { 
        printf("Есть хотя бы один неопциональный аргумент\n"); // Проверка на неожиданные аргументы
        return 1; 
    } 

    if (seed == -1 || array_size == -1 || pnum == -1) { // Проверка на установленные все обязательные параметры
        printf("Использование: %s --seed \"num\" --array_size \"num\" --pnum \"num\"\n", argv[0]); // Печать инструкций по использованию
        return 1; 
    } 

    int *array = malloc(sizeof(int) * array_size); // Выделение памяти для массива
    GenerateArray(array, array_size, seed); // Заполнение массива случайными значениями на основе семени
    int active_child_processes = 0; // Счетчик активных дочерних процессов

    struct timeval start_time; 
    gettimeofday(&start_time, NULL); // Получение времени начала для измерения производительности

    // Каналы для IPC (межпроцессного взаимодействия)
    int pipe_fd[2]; 
    if (!with_files) { 
        if (pipe(pipe_fd) == -1) { // Создание канала для связи между процессами
            perror("pipe"); // Печать ошибки, если создание канала не удалось
            return 1; 
        } 
    } 

    for (int i = 0; i < pnum; i++) { // Цикл для создания дочерних процессов
        pid_t child_pid = fork(); // Создание нового процесса
        if (child_pid >= 0) { 
            active_child_processes += 1; // Увеличение счетчика активных дочерних процессов
            if (child_pid == 0) { // Блок кода дочернего процесса
                // Вычисление диапазона для этого дочернего процесса на основе его индекса
                int start_index = i * (array_size / pnum); // Начальный индекс для этого дочернего процесса
                int end_index = (i + 1) * (array_size / pnum); // Конечный индекс для этого дочернего процесса
                if (i == pnum - 1) { // Последний дочерний процесс берет оставшиеся элементы массива
                    end_index = array_size; 
                } 

                // Нахождение минимума и максимума в этом диапазоне с помощью функции GetMinMax
                struct MinMax local_min_max = GetMinMax(array, start_index, end_index); 

                if (with_files) { 
                    // Запись результатов в файл, если это указано пользователем
                    char filename[256]; 
                    snprintf(filename, sizeof(filename), FILENAME_TEMPLATE, i); // Создание имени файла на основе индекса
                    FILE *file = fopen(filename, "w"); // Открытие файла для записи результатов
                    if (file) { 
                        fprintf(file, "%d %d\n", local_min_max.min, local_min_max.max); // Запись минимального и максимального значений в файл
                        fclose(file); // Закрытие файла после записи
                    } else { 
                        perror("fopen"); // Печать ошибки при открытии файла
                    } 
                } else { 
                    // Запись результатов в канал, если вывод в файлы не указан
                    close(pipe_fd[0]); // Закрытие конца чтения канала в дочернем процессе
                    write(pipe_fd[1], &local_min_max, sizeof(local_min_max)); // Отправка минимальных и максимальных значений через канал
                    close(pipe_fd[1]); // Закрытие конца записи канала в дочернем процессе
                } 

                free(array); // Освобождение выделенной памяти в дочернем процессе перед выходом
                return 0; // Успешный выход из дочернего процесса
            } 
        } else { 
            perror("Ошибка при создании процесса!"); // Печать ошибки при неудаче fork()
            return 1; 
        } 
    } 

    while (active_child_processes > 0) { 
        wait(NULL); // Ожидание завершения всех дочерних процессов
        active_child_processes -= 1; // Уменьшение счетчика активных дочерних процессов после каждого ожидания
    } 

    struct MinMax min_max; 
    min_max.min = INT_MAX; // Инициализация структуры min_max крайними значениями для целей сравнения
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) { 
        if (with_files) { 
            // Чтение результатов из файлов, созданных дочерними процессами
            char filename[256]; 
            snprintf(filename, sizeof(filename), FILENAME_TEMPLATE, i); // Создание имени файла на основе индекса
            
            FILE *file = fopen(filename, "r"); // Открытие файла для чтения результатов
            
            if (file) {  
                struct MinMax local_min_max;  
                fscanf(file, "%d %d", &local_min_max.min, &local_min_max.max); // Чтение минимальных и максимальных значений из файла
                
                fclose(file); 

                if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;  
                if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;

                remove(filename); // Опционально удалить файл после его чтения для освобождения ресурсов  
            } else {  
                perror("fopen");  
            }  
        } else {  
            struct MinMax local_min_max;
            read(pipe_fd[0], &local_min_max, sizeof(local_min_max));   // Чтение минимальных и максимальных значений из канала
            
            if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;
            if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;
        }
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);   // Получение времени окончания для измерения производительности

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);   // Освобождение выделенной памяти перед выходом

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);

    return 0;
    
}
//./parallel_min_max --seed 42 --array_size 100 --pnum 4 --by_files
//./parallel_min_max --seed 42 --array_size 100 --pnum 4