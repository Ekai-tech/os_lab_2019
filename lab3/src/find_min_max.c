#include "find_min_max.h" 
#include <limits.h> 
#include <stdlib.h> 
 
struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) { 
    struct MinMax min_max; 
    min_max.min = INT_MAX; // Initialize min to maximum possible integer 
    min_max.max = INT_MIN; // Initialize max to minimum possible integer 
 
    for (unsigned int i = begin; i < end; i++) { 
        if (array[i] < min_max.min) { 
            min_max.min = array[i]; // Update min if current element is smaller 
        } 
        if (array[i] > min_max.max) { 
            min_max.max = array[i]; // Update max if current element is larger 
        } 
    } 
     
    return min_max; // Return the result containing min and max 
}