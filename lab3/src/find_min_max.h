#ifndef FIND_MIN_MAX_H 
#define FIND_MIN_MAX_H 
 
struct MinMax { 
    int min; 
    int max; 
}; 
 
// Change this line to match your implementation 
struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end); 
void GenerateArray(int *array, unsigned int size, int seed); 
 
#endif // FIND_MIN_MAX_H