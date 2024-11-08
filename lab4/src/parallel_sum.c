#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <sys/time.h>
#include <stdbool.h> 
#include "utils.h" 


struct SumArgs {
    int *array;
    int begin;
    int end;
};

int Sum(const struct SumArgs *args) {
    int sum = 0;
    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i];
    }
    return sum;
}

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    // Парсинг аргументов командной строки
    while (true) {
        static struct option options[] = {
            {"threads_num", required_argument, 0, 0},
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                if (option_index == 0) threads_num = atoi(optarg);
                else if (option_index == 1) seed = atoi(optarg);
                else if (option_index == 2) array_size = atoi(optarg);
                break;
            case '?':
                break;
            default:
                printf("Unknown option\n");
                return 1;
        }
    }

    if (threads_num == 0 || array_size == 0 || seed == 0) {
        printf("Usage: %s --threads_num \"num\" --seed \"num\" --array_size \"num\"\n", argv[0]);
        return 1;
    }

    // Генерация массива
    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed); // Убедитесь, что эта функция объявлена в utils.h

    struct SumArgs args[threads_num];
    pthread_t threads[threads_num];

    // Измерение времени начала
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Создание потоков
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * (array_size / threads_num);
        args[i].end = (i + 1) * (array_size / threads_num);
        if (i == threads_num - 1) args[i].end = array_size; // Последний поток берет остаток

        pthread_create(&threads[i], NULL, ThreadSum, &args[i]);
    }

    int total_sum = 0;

    // Ожидание завершения потоков и суммирование результатов
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }

    // Измерение времени окончания
    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);
    
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0 + 
                          (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    
    printf("Total: %d\n", total_sum);
    printf("Elapsed time: %f ms\n", elapsed_time);

    return 0;
}
//./parallel_sum --threads_num "4" --seed "12345" --array_size "100000"