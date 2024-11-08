#include "find_min_max.h" 
#include <limits.h> 
#include <stdlib.h> 

// Функция для нахождения минимального и максимального значений в массиве
struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) { 
    struct MinMax min_max; 
    min_max.min = INT_MAX; // Инициализируем минимальное значение максимальным возможным целым числом 
    min_max.max = INT_MIN; // Инициализируем максимальное значение минимальным возможным целым числом 

    // Проходим по элементам массива от индекса begin до end
    for (unsigned int i = begin; i < end; i++) { 
        // Если текущий элемент меньше найденного минимума, обновляем минимум
        if (array[i] < min_max.min) { 
            min_max.min = array[i]; // Обновляем минимальное значение
        } 
        // Если текущий элемент больше найденного максимума, обновляем максимум
        if (array[i] > min_max.max) { 
            min_max.max = array[i]; // Обновляем максимальное значение
        } 
    } 
    
    // Возвращаем структуру с найденными минимальным и максимальным значениями
    return min_max; // Возврат результата, содержащего минимум и максимум 
}