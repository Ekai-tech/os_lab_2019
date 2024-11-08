#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1; // Первый мьютекс
pthread_mutex_t mutex2; // Второй мьютекс

void *thread_function1(void *arg) {
    // Блокируем первый мьютекс
    pthread_mutex_lock(&mutex1);
    printf("Thread 1: acquired mutex1\n");
    
    // Задержка для имитации работы
    sleep(1);
    
    // Пытаемся заблокировать второй мьютекс
    printf("Thread 1: trying to acquire mutex2\n");
    pthread_mutex_lock(&mutex2);
    
    printf("Thread 1: acquired mutex2\n");

    // Освобождаем мьютексы
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

void *thread_function2(void *arg) {
    // Блокируем второй мьютекс
    pthread_mutex_lock(&mutex2);
    printf("Thread 2: acquired mutex2\n");
    
    // Задержка для имитации работы
    sleep(1);
    
    // Пытаемся заблокировать первый мьютекс
    printf("Thread 2: trying to acquire mutex1\n");
    pthread_mutex_lock(&mutex1);
    
    printf("Thread 2: acquired mutex1\n");

    // Освобождаем мьютексы
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Инициализация мьютексов
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    // Создание потоков
    pthread_create(&thread1, NULL, thread_function1, NULL);
    pthread_create(&thread2, NULL, thread_function2, NULL);

    // Ожидание завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Уничтожение мьютексов
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    return 0;
}