#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_RUNS (2)
#define DATA_SIZE ((long long)100000)
#define NUM_THREADS (10) 

typedef struct {
    unsigned int seed;
    double duration;
    long double value1;
    long double value2;
    volatile long double *array1; 
    volatile long double *array2; 
} ThreadData;

void* random_memory_access(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    clock_t start_time = clock();
    
    for (size_t i = 0; i < DATA_SIZE; i++) {
        
        if (rand_r(&data->seed) % 2 == 0) { 
            data->value1 = data->array1[rand_r(&data->seed) % DATA_SIZE];
        } else {
            data->value2 = data->array2[rand_r(&data->seed) % DATA_SIZE];
        }
    }

    clock_t end_time = clock();
    data->duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    unsigned int seed_base = 50;
    double total_time = 0.0;

    volatile long double *array1 = (long double *)malloc((size_t)DATA_SIZE * sizeof(long double));
    volatile long double *array2 = (long double *)malloc((size_t)DATA_SIZE * sizeof(long double));
    
    if (array1 == NULL || array2 == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    srand(seed_base);

    for (size_t i = 0; i < DATA_SIZE; i++) {
        array1[i] = (long double)rand() / RAND_MAX;
        array2[i] = (long double)rand() / RAND_MAX;
    }

    for (int run = 0; run < NUM_RUNS; run++) {
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_data[i].seed = seed_base + i;
            thread_data[i].array1 = array1; 
            thread_data[i].array2 = array2; 
            pthread_create(&threads[i], NULL, random_memory_access, (void*)&thread_data[i]);
        }

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
            printf("Run %d, Thread %d: Time taken: %.10f seconds\n", run + 1, i + 1, thread_data[i].duration);
            total_time += thread_data[i].duration;
        }

        seed_base += NUM_THREADS;
    }

    free(array1);
    free(array2);

    printf("Average time over %d runs and %d threads: %.10f seconds\n", NUM_RUNS, NUM_THREADS, total_time / (NUM_RUNS * NUM_THREADS));

    return 0;
}
