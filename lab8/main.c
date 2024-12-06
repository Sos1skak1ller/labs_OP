#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define ARRAY_SIZE 10
#define NUM_READERS 10
#define NUM_ITERATIONS 20

struct SharedData {
    int array[ARRAY_SIZE];
    pthread_mutex_t mutex;
    int counter;
};

void* writer(void* arg) {
    struct SharedData* data = (struct SharedData*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&data->mutex);
        data->counter++;
        for (int j = 0; j < ARRAY_SIZE; j++) {
            data->array[j] = data->counter;
        }
        pthread_mutex_unlock(&data->mutex);
        usleep(100000); // 100ms delay
    }
    return NULL;
}

void* reader(void* arg) {
    struct SharedData* data = (struct SharedData*)arg;
    pthread_t tid = pthread_self();
    
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&data->mutex);
        printf("Thread %lu reading: [", tid);
        for (int j = 0; j < ARRAY_SIZE; j++) {
            printf("%d", data->array[j]);
            if (j < ARRAY_SIZE - 1) printf(", ");
        }
        printf("]\n");
        pthread_mutex_unlock(&data->mutex);
        usleep(200000); // 200ms delay
    }
    return NULL;
}

int main() {
    struct SharedData data = {.counter = 0};
    pthread_t readers[NUM_READERS], writer_thread;
    
    pthread_mutex_init(&data.mutex, NULL);

    // Create writer thread
    pthread_create(&writer_thread, NULL, writer, &data);
    
    // Create reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, &data);
    }
    
    // Join threads
    pthread_join(writer_thread, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    
    pthread_mutex_destroy(&data.mutex);
    return 0;
}