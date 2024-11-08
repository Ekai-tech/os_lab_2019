#include "utils.h" 

#include <stdio.h> 
#include <stdlib.h> 

// Функция для генерации массива случайных целых чисел
void GenerateArray(int *array, unsigned int array_size, unsigned int seed) { 
    srand(seed); // Инициализация генератора случайных чисел с заданным семенем

    // Цикл для заполнения массива случайными числами
    for (int i = 0; i < array_size; i++) { 
        array[i] = rand(); // Генерация случайного числа и его сохранение в текущем элементе массива
    } 
}