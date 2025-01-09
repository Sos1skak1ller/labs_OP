#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 10
#define NUM_READERS 10

int sharedArray[ARRAY_SIZE];
int writeCounter = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* writerThread(void* arg) {
    while (1) {
        pthread_rwlock_wrlock(&rwlock);

        sharedArray[writeCounter % ARRAY_SIZE] = writeCounter;
        writeCounter++;

        pthread_rwlock_unlock(&rwlock);

        sleep(1);
    }

    return NULL;
}

void* readerThread(void* arg) {
    long tid = (long)arg;

    while (1) {
        pthread_rwlock_rdlock(&rwlock);

        printf("Reader %ld, tid: %lx, array: [", tid, pthread_self());
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", sharedArray[i]);
        }
        printf("]\n");

        pthread_rwlock_unlock(&rwlock);
    }

    return NULL;
}

int main() {
    pthread_t writer, readers[NUM_READERS];

    pthread_create(&writer, NULL, writerThread, NULL);

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, readerThread, (void*)i);
    }

    pthread_join(writer, NULL);

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    return 0;
}