#include <ctype.h> 
#include <limits.h> 
#include <stdbool.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/time.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <getopt.h> 
 
#include "find_min_max.h" 
#include "utils.h" 
 
#define FILENAME_TEMPLATE "result_%d.txt" 
 
int main(int argc, char **argv) { 
    int seed = -1; 
    int array_size = -1; 
    int pnum = -1; 
    bool with_files = false; 
 
    // Argument parsing 
    while (true) { 
        int current_optind = optind ? optind : 1; 
 
        static struct option options[] = { 
            {"seed", required_argument, 0, 0}, 
            {"array_size", required_argument, 0, 0}, 
            {"pnum", required_argument, 0, 0}, 
            {"by_files", no_argument, 0, 'f'}, 
            {0, 0, 0, 0} 
        }; 
 
        int option_index = 0; 
        int c = getopt_long(argc, argv, "f", options, &option_index); 
 
        if (c == -1) break; 
 
        switch (c) { 
            case 0: 
                switch (option_index) { 
                    case 0: 
                        seed = atoi(optarg); 
                        break; 
                    case 1: 
                        array_size = atoi(optarg); 
                        break; 
                    case 2: 
                        pnum = atoi(optarg); 
                        break; 
                    case 3: 
                        with_files = true; 
                        break; 
 
                    default: 
                        printf("Index %d is out of options\n", option_index); 
                } 
                break; 
            case 'f': 
                with_files = true; 
                break; 
 
            case '?': 
                break; 
 
            default: 
                printf("getopt returned character code 0%o?\n", c); 
        } 
    } 
 
    if (optind < argc) { 
        printf("Has at least one no option argument\n"); 
        return 1; 
    } 
 
    if (seed == -1  array_size == -1  pnum == -1) { 
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\"\n", argv[0]); 
        return 1; 
    } 
 
    int *array = malloc(sizeof(int) * array_size); 
    GenerateArray(array, array_size, seed); 
    int active_child_processes = 0; 
 
    struct timeval start_time; 
    gettimeofday(&start_time, NULL); 
 
    // Pipes for IPC 
    int pipe_fd[2]; 
    if (!with_files) { 
        if (pipe(pipe_fd) == -1) { 
            perror("pipe"); 
            return 1; 
        } 
    } 
 
    for (int i = 0; i < pnum; i++) { 
        pid_t child_pid = fork(); 
        if (child_pid >= 0) { 
            active_child_processes += 1; 
            if (child_pid == 0) { // Child process 
                // Calculate range for this child 
                int start_index = i * (array_size / pnum); 
                int end_index = (i + 1) * (array_size / pnum); 
                if (i == pnum - 1) { // Last child takes remaining elements 
                    end_index = array_size; 
                } 
 
                // Find min and max in this range 
                struct MinMax local_min_max = GetMinMax(array, start_index, end_index); 
 
                if (with_files) { 
                    // Write results to a file 
                    char filename[256]; 
                    snprintf(filename, sizeof(filename), FILENAME_TEMPLATE, i); 
                    FILE *file = fopen(filename, "w"); 
                    if (file) { 
                        fprintf(file, "%d %d\n", local_min_max.min, local_min_max.max); 
                        fclose(file); 
                    } else { 
                        perror("fopen"); 
                    } 
                } else { 
                    // Write results to pipe 
                    close(pipe_fd[0]); // Close reading end 
                    write(pipe_fd[1], &local_min_max, sizeof(local_min_max)); 
                    close(pipe_fd[1]); // Close writing end 
                } 
                 
                free(array); // Free

memory in child 
                return 0; // Exit child process 
            } 
        } else { 
            perror("Fork failed!"); 
            return 1; 
        } 
    } 
 
    while (active_child_processes > 0) { 
        wait(NULL); // Wait for all children to finish 
        active_child_processes -= 1; 
    } 
 
    struct MinMax min_max; 
    min_max.min = INT_MAX; 
    min_max.max = INT_MIN; 
 
    for (int i = 0; i < pnum; i++) { 
        if (with_files) { 
            // Read from files 
            char filename[256]; 
            snprintf(filename, sizeof(filename), FILENAME_TEMPLATE, i); 
            FILE *file = fopen(filename, "r"); 
            if (file) { 
                struct MinMax local_min_max; 
                fscanf(file, "%d %d", &local_min_max.min, &local_min_max.max); 
                fclose(file); 
 
                if (local_min_max.min < min_max.min) min_max.min = local_min_max.min; 
                if (local_min_max.max > min_max.max) min_max.max = local_min_max.max; 
 
                remove(filename); // Optionally remove the file after reading 
            } else { 
                perror("fopen"); 
            } 
        } else { 
            // Read from pipes 
            struct MinMax local_min_max; 
            read(pipe_fd[0], &local_min_max, sizeof(local_min_max)); 
 
            if (local_min_max.min < min_max.min) min_max.min = local_min_max.min; 
            if (local_min_max.max > min_max.max) min_max.max = local_min_max.max; 
        } 
    } 
 
    struct timeval finish_time; 
    gettimeofday(&finish_time, NULL); 
 
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0; 
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0; 
 
    free(array); // Free allocated memory 
 
    printf("Min: %d\n", min_max.min); 
    printf("Max: %d\n", min_max.max); 
    printf("Elapsed time: %fms\n", elapsed_time); 
 
    return 0; 
}