#ifndef FIND_MIN_MAX_H 
#define FIND_MIN_MAX_H 

// Структура для хранения минимального и максимального значений
struct MinMax { 
    int min; // Минимальное значение
    int max; // Максимальное значение
}; 

// Объявление функции для нахождения минимума и максимума в массиве
// Параметры:
// - int *array: указатель на массив целых чисел, в котором нужно найти минимум и максимум
// - unsigned int begin: начальный индекс для поиска (включительно)
// - unsigned int end: конечный индекс для поиска (исключительно)
struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end); 

// Объявление функции для генерации массива целых чисел
// Параметры:
// - int *array: указатель на массив, который будет заполнен случайными числами
// - unsigned int size: размер массива (количество элементов)
// - int seed: начальное значение для генератора случайных чисел
void GenerateArray(int *array, unsigned int size, int seed); 

#endif // FIND_MIN_MAX_H 