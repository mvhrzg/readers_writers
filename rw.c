/*
 * File:   ReadersWriters.c
 * Author: MHerzog
 *
 * Created on April 8, 2015, 5:44 PM
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_OF_READERS 5
#define NUM_READS 200
#define NUM_WRITES 200

sem_t readerCountMutex; //the mutual exclusion semaphore for the readerCount
int readerCount; //number of threads reading of wanting to read

sem_t bufferMutex; // mutual exclusion semaphore for the "shared buffer"
int sharedBuffer; //the "shared buffer"is an int starting at 0.
                  //readers read its value and print out.
                  //write increments the value by 1.

int up(sem_t* thisMutex){
    return sem_post(thisMutex);
}

int down(sem_t* thisMutex){
    return sem_wait(thisMutex);
}

void* readerCode(void* reader){
    int i;
    int r = *((int *) reader);
    printf("reader %d created.\n", r);
    for (i = 0; i < NUM_READS; i++){
//	down(&readerCountMutex);
	sem_wait(&readerCountMutex);
	readerCount = readerCount + 1;

	if(readerCount == 1){
//	    down(&bufferMutex);
	    sem_wait(&bufferMutex);
	}//if

//	up(&readerCountMutex);
	sem_post(&readerCountMutex);
	printf("reader %d on iteration %d. sharedBuffer is %d.\n", r, i, sharedBuffer);

//	down(&readerCountMutex);
	sem_wait(&readerCountMutex);
	readerCount = readerCount - 1;
//	printf(" readerCount is %d.\n", readerCount);

	if(readerCount == 0){
//	    up(&bufferMutex);
	    sem_post(&bufferMutex);
	}

//	up(&readerCountMutex);
	sem_post(&readerCountMutex);
    }//for
    printf("READER %d DONE.\n", r);
    pthread_exit(0);
}

void* writerCode(void* writer){
    int i;
    for(i = 0; i < NUM_WRITES; i++){
//	down(&bufferMutex);
	sem_wait(&bufferMutex);
	sharedBuffer = sharedBuffer + 1;
	printf("writer on iteration %d. sharedBuffer is %d.\n", i, sharedBuffer);
//	up(&bufferMutex);
	sem_post(&bufferMutex);
    }//for
    pthread_exit(0);
}

int main(int argc, char** argv) {
    int i;
    readerCount = 0;
    sharedBuffer = 0;
    sem_init(&readerCountMutex, 0, 1);
    sem_init(&bufferMutex, 0, 1);

    //create writer thread
    pthread_t writerThread;
    int *temp2 = (int *)malloc(sizeof(int *));
    pthread_create(&writerThread, 0, writerCode, (void *) temp2);

    //create reader threads
    pthread_t reads[NUM_OF_READERS];
    for(i = 0; i < NUM_OF_READERS; i++){
	int *temp = (int *)malloc(sizeof(int *));
	*temp = i;
	pthread_create(&reads[i], 0, readerCode, (void *) temp);
    }

    pthread_join(writerThread, 0);
    printf("writer_join.\n");

    for(i = 0; i < NUM_OF_READERS; i++){
	pthread_join(reads[i], 0);
	printf("reader %d join.\n", i);
    }


    sem_close(&readerCountMutex);
    sem_close(&bufferMutex);

    return (EXIT_SUCCESS);
}


