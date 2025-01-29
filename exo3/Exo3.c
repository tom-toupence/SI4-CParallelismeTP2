#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define NBCELLS 200000000

int N, seqOutput;
char *array;
char target;

typedef struct {
    int from, to;
    char target;
    int result;
} Task;

int myCeil(float x) {
    int ix = (int)x;
    if (x > ix) {
        return ix + 1;
    } else {
        return ix;
    }
}

void *nbOccursSlice(void *arg) {
    Task *task = (Task *)arg;
    task->result = 0;
    for (int i = task->from; i < task->to; i++) {
        if (array[i] == task->target) task->result++;
    }
    return NULL;
}

int nbOccurs(char *array, int size, char c) {
    Task task = {0, size, c, 0};
    nbOccursSlice(&task);
    return task.result;
}

int nbOccursParallelN(char *array, int size, char c, int numThreads) {
    pthread_t threads[numThreads];
    Task tasks[numThreads];
    int chunkSize = myCeil((float)size / numThreads);
    
    for (int i = 0; i < numThreads; i++) {
        tasks[i].from = i * chunkSize;
        tasks[i].to = (i + 1) * chunkSize < size ? (i + 1) * chunkSize : size;
        tasks[i].target = c;
        tasks[i].result = 0;
        pthread_create(&threads[i], NULL, nbOccursSlice, &tasks[i]);
    }
    
    int result = 0;
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        result += tasks[i].result;
    }
    return result;
}

char randomChar() {
    return (char)(rand() % 256);
}

void randomCharArray() {
    array = malloc(NBCELLS * sizeof(char));
    for (int i = 0; i < NBCELLS; i++) array[i] = randomChar();
}

double getTime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    N = (argc > 1) ? atoi(argv[1]) : sysconf(_SC_NPROCESSORS_ONLN);
    printf("Au maximum, le nb de core utilisés sera : %d\n", N);
    
    srand(time(NULL));
    randomCharArray();
    target = randomChar();
    
    double start, end;
    
    start = getTime();
    seqOutput = nbOccurs(array, NBCELLS, target);
    end = getTime();
    double seqTime = end - start;
    printf("Résultat avec nbOccurs : %d\n", seqOutput);
    printf("Temps avec nbOccurs: %.2f sec\n", seqTime);
    
    // Collect data for different numbers of processors
    printf("Processors\tSpeedup\n");
    for (int numThreads = 1; numThreads <= N; numThreads++) {
        start = getTime();
        int parNOutput = nbOccursParallelN(array, NBCELLS, target, numThreads);
        end = getTime();
        double parTime = end - start;
        double speedup = seqTime / parTime;
        printf("%d\t%.2f\n", numThreads, speedup);
    }
    
    free(array);
    return 0;
}