#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 10
#define NUM_READERS 10

int sharedArray[ARRAY_SIZE];
int writeCounter = 0;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int dataReady = 0;

void* writerThread(void* arg) {
	while (1) {
		pthread_mutex_lock(&myMutex);

		sharedArray[writeCounter % ARRAY_SIZE] = writeCounter;
		writeCounter++;

		dataReady = 1;
		pthread_cond_broadcast(&cond);

		pthread_mutex_unlock(&myMutex);

		sleep(2);
	}

	return NULL;
}

void* readerThread(void* arg) {
	long tid = (long)arg;

	while (1) {
		pthread_mutex_lock(&myMutex);

		while (!dataReady) {
			pthread_cond_wait(&cond, &myMutex);
		}

		printf("Reader %ld, tid: %lx, array: [", tid, pthread_self());
		for (int i = 0; i < ARRAY_SIZE; i++) {
			printf("%d ", sharedArray[i]);
		}
		printf("]\n");

		dataReady = 0;

		pthread_mutex_unlock(&myMutex);
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

	pthread_mutex_destroy(&myMutex);
	pthread_cond_destroy(&cond);

	return 0;
}
