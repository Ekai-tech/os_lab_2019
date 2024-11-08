#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

typedef struct {
    int start;
    int end;
    int mod;
    long long result;
} ThreadData;

pthread_mutex_t mutex;

void *factorial_part(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    long long partial_result = 1;

    for (int i = data->start; i <= data->end; i++) {
        partial_result = (partial_result * i) % data->mod;
    }

    // Защита доступа к общей переменной result
    pthread_mutex_lock(&mutex);
    data->result = (data->result * partial_result) % data->mod;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    int k = 0, pnum = 1, mod = 1;

    // Парсинг аргументов командной строки
    int option;
    while ((option = getopt(argc, argv, "k:p:m:")) != -1) {
        switch (option) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> -p <threads> -m <mod>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Invalid input values.\n");
        return EXIT_FAILURE;
    }

    pthread_t threads[pnum];
    ThreadData thread_data[pnum];
    
    // Инициализация мьютекса
    pthread_mutex_init(&mutex, NULL);

    // Разделение работы между потоками
    int range_per_thread = k / pnum;
    
    for (int i = 0; i < pnum; i++) {
        thread_data[i].start = i * range_per_thread + 1;
        thread_data[i].end = (i == pnum - 1) ? k : (i + 1) * range_per_thread; // последний поток обрабатывает остаток
        thread_data[i].mod = mod;
        thread_data[i].result = 1; // начальное значение для каждого потока

        pthread_create(&threads[i], NULL, factorial_part, &thread_data[i]);
    }

    // Ожидание завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    long long final_result = 1;

    // Суммируем результаты всех потоков
    for (int i = 0; i < pnum; i++) {
        final_result = (final_result * thread_data[i].result) % mod;
    }

    printf("Factorial of %d modulo %d is: %lld\n", k, mod, final_result);

    // Освобождение ресурсов
    pthread_mutex_destroy(&mutex);
    
    return EXIT_SUCCESS;
}