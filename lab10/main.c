#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_SIZE 256
#define NUM_READERS 10

char shared_array[ARRAY_SIZE];
int record_counter = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int updated = 0;

void* writer_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        snprintf(shared_array, ARRAY_SIZE, "Record %d", record_counter++);
        updated = 1;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(1); 
    }
    return NULL;
}

void* reader_thread(void* arg) {
    long tid = (long)arg;

    while (1) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        printf("Reader %ld: %s\n", tid, shared_array);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writer;
    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0) {
        perror("Failed to create writer thread");
        return EXIT_FAILURE;
    }
    for (long i = 0; i < NUM_READERS; i++) {
        if (pthread_create(&readers[i], NULL, reader_thread, (void*)i) != 0) {
            perror("Failed to create reader thread");
            return EXIT_FAILURE;
        }
    }

    pthread_join(writer, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
	
    return EXIT_SUCCESS;
}