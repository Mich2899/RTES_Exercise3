//deadlock_updated.c 
//edited by Michelle Christian
#include <pthread.h>					//POSIX thread library for stamdard thread API
#include <stdio.h>					//standard IO library
#include <sched.h>					//defines the sched_param structure
#include <time.h>					//contains time and date functions
#include <stdlib.h>					//standard library
#include <string.h>					//string functions
#include <unistd.h>					//unix standard library

#define NUM_THREADS 2					//number of threads
#define THREAD_1 1					//Thread 1
#define THREAD_2 2					//Thread 2

//structure to define thread parameters
typedef struct
{
    int threadIdx;
} threadParams_t;


pthread_t threads[NUM_THREADS];			//array for threads
threadParams_t threadParams[NUM_THREADS];		//thread params array for thread parameters

struct sched_param nrt_param;				//sched_param structure to define scheduling paramters

// On the Raspberry Pi, the MUTEX semaphores must be statically initialized
//
// This works on all Linux platforms, but dynamic initialization does not work
// on the R-Pi in particular as of June 2020.
//
pthread_mutex_t rsrcA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rsrcB = PTHREAD_MUTEX_INITIALIZER;

volatile int rsrcACnt=0, rsrcBCnt=0, noWait=0;	//resource count variales for parameters A and B

//function that defines when a thread grabs resources
void *grabRsrcs(void *threadp)
{
   threadParams_t *threadParams = (threadParams_t *)threadp;		
   int threadIdx = threadParams->threadIdx;
   int rc1=0, dlck_cnt=0;

   if(threadIdx == THREAD_1)
   {
     printf("THREAD 1 grabbing resources\n");
     pthread_mutex_lock(&rsrcA);
     rsrcACnt++;
     if(!noWait) sleep(1);
     printf("THREAD 1 got A, trying for B\n");
     rc1=pthread_mutex_trylock(&rsrcB);
	     if(rc1){
		printf("!!!!!!!!!!!!!!!!!!!!!!!!DEADLOCK ENCOUNTERED!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	     	printf("Thread 2 has grabbed B hence Thread 1 giving up A\n");
		pthread_mutex_unlock(&rsrcA);
		dlck_cnt++;
		goto thread2;
	     }
     rsrcBCnt++;
     printf("THREAD 1 got A and B\n");
     pthread_mutex_unlock(&rsrcB);
     pthread_mutex_unlock(&rsrcA);
     printf("THREAD 1 done\n");
   }
   else
   {
thread2:     printf("THREAD 2 grabbing resources\n");
     pthread_mutex_lock(&rsrcB);
     rsrcBCnt++;
     if(!noWait) sleep(1);
     printf("THREAD 2 got B, trying for A\n");
     pthread_mutex_lock(&rsrcA); 
     rsrcACnt++;
     printf("THREAD 2 got B and A\n");
     pthread_mutex_unlock(&rsrcA);
     pthread_mutex_unlock(&rsrcB);
     printf("THREAD 2 done\n");
     if(dlck_cnt)
     {
     	     pthread_mutex_lock(&rsrcA);
	     rsrcACnt++;
	     if(!noWait) sleep(1);
	     printf("THREAD 1 got A, trying for B\n");
	     pthread_mutex_lock(&rsrcB);
	     rsrcBCnt++;
	     printf("THREAD 1 got A and B\n");
             pthread_mutex_unlock(&rsrcB);
	     pthread_mutex_unlock(&rsrcA);
	     printf("THREAD 1 done\n");     
     }
   }
   pthread_exit(NULL);
}


int main (int argc, char *argv[])
{
//safe variable to see if the scenario is safe
   int rc, safe=0;

//safe variable to see if the scenario is race condition and resource count
   rsrcACnt=0, rsrcBCnt=0, noWait=0;

//if argument is less than 2
   if(argc < 2)
   {
     printf("Will set up unsafe deadlock scenario\n");
   }
   else if(argc == 2)
   {
     if(strncmp("safe", argv[1], 4) == 0)
       safe=1;
     else if(strncmp("race", argv[1], 4) == 0)
       noWait=1;
     else
       printf("Will set up unsafe deadlock scenario\n");
   }
   else
   {
     printf("Usage: deadlock [safe|race|unsafe]\n");
   }


   printf("Creating thread %d\n", THREAD_1);
   threadParams[THREAD_1].threadIdx=THREAD_1;
   rc = pthread_create(&threads[0], NULL, grabRsrcs, (void *)&threadParams[THREAD_1]);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}
   printf("Thread 1 spawned\n");

   if(safe) // Make sure Thread 1 finishes with both resources first
   {
     if(pthread_join(threads[0], NULL) == 0)
       printf("Thread 1: %x done\n", (unsigned int)threads[0]);
     else
       perror("Thread 1");
   }

   printf("Creating thread %d\n", THREAD_2);
   threadParams[THREAD_2].threadIdx=THREAD_2;
   rc = pthread_create(&threads[1], NULL, grabRsrcs, (void *)&threadParams[THREAD_2]);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}
   printf("Thread 2 spawned\n");

   printf("rsrcACnt=%d, rsrcBCnt=%d\n", rsrcACnt, rsrcBCnt);
   printf("will try to join CS threads unless they deadlock\n");

   if(!safe)
   {
     if(pthread_join(threads[0], NULL) == 0)
       printf("Thread 1: %x done\n", (unsigned int)threads[0]);
     else
       perror("Thread 1");
   }

   if(pthread_join(threads[1], NULL) == 0)
     printf("Thread 2: %x done\n", (unsigned int)threads[1]);
   else
     perror("Thread 2");

   if(pthread_mutex_destroy(&rsrcA) != 0)
     perror("mutex A destroy");

   if(pthread_mutex_destroy(&rsrcB) != 0)
     perror("mutex B destroy");

   printf("All done\n");

   exit(0);
}
