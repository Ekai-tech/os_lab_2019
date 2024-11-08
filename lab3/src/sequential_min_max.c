#include <stdio.h> 
#include <stdlib.h> 
#include "find_min_max.h" 

int main(int argc, char **argv) { 
    // Проверка количества аргументов командной строки
    if (argc != 3) { 
        printf("Использование: %s seed arraysize\n", argv[0]); // Печать инструкции по использованию
        return 1; // Завершение программы с кодом ошибки 1
    } 

    // Преобразование первого аргумента в целое число для семени
    int seed = atoi(argv[1]); 
    if (seed <= 0) { 
        printf("seed должен быть положительным числом\n"); // Проверка, что семя положительное
        return 1; // Завершение программы с кодом ошибки 1
    } 

    // Преобразование второго аргумента в целое число для размера массива
    int array_size = atoi(argv[2]); 
    if (array_size <= 0) { 
        printf("array_size должен быть положительным числом\n"); // Проверка, что размер массива положительный
        return 1; // Завершение программы с кодом ошибки 1
    } 

    // Выделение памяти для массива целых чисел
    int *array = malloc(array_size * sizeof(int)); 
    if (array == NULL) { // Проверка успешного выделения памяти
        printf("Ошибка выделения памяти\n"); // Сообщение об ошибке при выделении памяти
        return 1; // Завершение программы с кодом ошибки 1
    } 

    // Генерация массива случайных чисел с использованием заданного семени
    GenerateArray(array, array_size, seed); 
    
    // Нахождение минимального и максимального значений в массиве
    struct MinMax min_max = GetMinMax(array, 0, array_size); 
    
    free(array); // Освобождение выделенной памяти после использования 

    // Вывод минимального и максимального значений на экран
    printf("min: %d\n", min_max.min); 
    printf("max: %d\n", min_max.max); 

    return 0; // Успешное завершение программы
}