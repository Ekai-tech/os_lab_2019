#include "utils.h" 

#include <stdio.h> 
#include <stdlib.h> 

// Функция для генерации массива случайных целых чисел
void GenerateArray(int *array, unsigned int size, unsigned int seed) { 
    srand(seed); // Инициализация генератора случайных чисел с заданным семенем

    // Цикл для заполнения массива случайными числами
    for (int i = 0; i < size; i++) { 
        array[i] = rand() % 101; // Генерация случайного числа и его сохранение в текущем элементе массива
    } 
}