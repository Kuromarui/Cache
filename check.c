#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define DATA_SIZE (1000000) 
#define NUM_THREADS (2)      
#define STRIDE (128)     
#define NUM_RUNS (100)        

typedef struct {
    double *data;
    long start;
    long end;
} ThreadArgs;


void* Cache_access(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    double* data = threadArgs->data;
    long start = threadArgs->start;
    long end = threadArgs->end;

    for (long i = start; i < end; i += STRIDE) {
        double temp = data[i] * 2.0;
    }
    return NULL;
}

double run_multithreaded_cache(double* data) {
    pthread_t threads[NUM_THREADS];
    ThreadArgs threadArgs[NUM_THREADS];
    long chunk_size = DATA_SIZE / NUM_THREADS;

    clock_t start_time = clock();

    for (long i = 0; i < NUM_THREADS; i++) {
        threadArgs[i].data = data;
        threadArgs[i].start = i * chunk_size;
        threadArgs[i].end = (i == NUM_THREADS - 1) ? DATA_SIZE : (i + 1) * chunk_size;
        pthread_create(&threads[i], NULL, Cache_access, (void*)&threadArgs[i]);
    }

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    return (double)(end_time - start_time) / CLOCKS_PER_SEC;
}

int main() {
    double* data = (double*)malloc(DATA_SIZE * sizeof(double));
    if (data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    srand(40);

    for (long i = 0; i < DATA_SIZE; i++) {
        data[i] = (double)(rand() % 100000); 
    }

    double total_time = 0.0;

    for (int run = 0; run < NUM_RUNS; run++) {
        double time_taken = run_multithreaded_cache(data);
        printf("Run %d: Time taken with access: %.10f seconds\n", run + 1, time_taken);
        total_time += time_taken;
    }

    printf("Average time: %d runs: %.10f seconds\n", NUM_RUNS, total_time / NUM_RUNS);

    free(data);
    return 0;
}