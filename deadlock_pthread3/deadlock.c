///////////////////////////////////////////////////////////////////////////////////////////////////
//												//
//												//
//Edited by Michelle Christian									//
//												//
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 2								//thread 1 and thread 2
#define THREAD_1 1
#define THREAD_2 2

typedef struct
{
    int threadIdx;
} threadParams_t;


pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

struct sched_param nrt_param;

// On the Raspberry Pi, the MUTEX semaphores must be statically initialized
//
// This works on all Linux platforms, but dynamic initialization does not work
// on the R-Pi in particular as of June 2020.
//
pthread_mutex_t rsrcA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rsrcB = PTHREAD_MUTEX_INITIALIZER;

//rsrcACount increments every time any thread grabs resource 1
//rsrcBCount increments every time any thread grabs resource 2
volatile int rsrcACnt=0, rsrcBCnt=0, noWait=0;

//function: grabRsrcs
//params: void* threadp 
//brief: function grabs the resources for respecitve threads based on the threadIDX
void *grabRsrcs(void *threadp)
{
   threadParams_t *threadParams = (threadParams_t *)threadp;
   int threadIdx = threadParams->threadIdx;

//if threadindex is THREAD_! then grab resources for thread 1
   if(threadIdx == THREAD_1)
   {
     printf("THREAD 1 grabbing resources\n");
     pthread_mutex_lock(&rsrcA);				//attempt to lock resource A
     rsrcACnt++;
     if(!noWait) sleep(1);					//check if safe argument given then sleep for 1 second
     printf("THREAD 1 got A, trying for B\n");
     pthread_mutex_lock(&rsrcB);				//attempt to lock resource B
     rsrcBCnt++;
     printf("THREAD 1 got A and B\n");				//thread successfully executed
     pthread_mutex_unlock(&rsrcB);
     pthread_mutex_unlock(&rsrcA);
     printf("THREAD 1 done\n");
   }
//else attempt to grab resources for thread 2
   else
   {
     printf("THREAD 2 grabbing resources\n");
     pthread_mutex_lock(&rsrcB);				//attempt to lock resource B
     rsrcBCnt++;
     if(!noWait) sleep(1);					//check if safe argument given the sleep for 1 second
     printf("THREAD 2 got B, trying for A\n");
     pthread_mutex_lock(&rsrcA);				//atempt to lock resource A
     rsrcACnt++;
     printf("THREAD 2 got B and A\n");				//thread succesfully executed
     pthread_mutex_unlock(&rsrcA);
     pthread_mutex_unlock(&rsrcB);
     printf("THREAD 2 done\n");
   }
   pthread_exit(NULL);
}


int main (int argc, char *argv[])
{
   int rc, safe=0;

   rsrcACnt=0, rsrcBCnt=0, noWait=0;

   if(argc < 2)							//no argument passed after filename
   {
     printf("Will set up unsafe deadlock scenario\n");
   }
   else if(argc == 2)						//argument passed after filename
   {
     if(strncmp("safe", argv[1], 4) == 0)			//check the string argument if safe
       safe=1;
     else if(strncmp("race", argv[1], 4) == 0)			//check the argument if race
       noWait=1;
     else
       printf("Will set up unsafe deadlock scenario\n");
   }
   else
   {
     printf("Usage: deadlock [safe|race|unsafe]\n");
   }

//crete thread 1 and check if safe argument is passed. If so join thread 1 before creating thread 2 
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

   //safe condition not satisfied and hence join both the threads simultaenously to implement race contition
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