/*****************************************************************************/
/* mutex_lock.c								     */
/* Author: Michelle Joslin Christian					     */
/*									     */
/*									     */
/*****************************************************************************/							

#include <pthread.h>							      	//posix API library
#include <stdio.h>							        //standard library
#include <sched.h>								//contains sched_params struct 
#include <time.h>								//contains timespec struct
#include <stdlib.h>								
#include <unistd.h>								//UNIX standard library
#include <math.h>								//for math functions like cos, sin 

#define MY_CLOCK CLOCK_REALTIME							//clock realtime
#define NUM_THREADS 2								//clock realtime
#define THREAD_1 1								//thread index for write thread
#define THREAD_2 2								//thread index for read thread
#define TOTAL_TIME 180								//total time for the application

//thread structure with index as the only element
typedef struct
{
        int thread_idx;
}threadParams_t;

//thread initialization for array of 2 threads
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

//sched_param 
struct sched_param rt_param;

//the below structure contains timespec structure and 6 variables that are updated in the write thread
//thus structure is common between two threads i.e. read and write
typedef struct
{
	struct timespec update;							//timespec structure
	double Lat;								//latitude
	double Long;								//longitude
	double alt;								//altitude
	double roll;								//roll
	double pitch;								//pitch
	double yaw;								//yaw
}values;

pthread_mutex_t shared_sem=PTHREAD_MUTEX_INITIALIZER;				//pthread_mutex_initialized
values write_value_struct;							//value structure that is common between two threads

int t_count=1, t_wcount=1, t_rcount=1;						//variables for time count

void* write_thread()
{
	while(t_count<=180)							//continue the process until 180 seconds
	{
	printf("Writing values in THREAD1 write_no: %d\n",t_wcount);			
	if(t_wcount==10)
	{
		t_wcount=0;
	}	
	pthread_mutex_lock(&shared_sem);					//provide shared semaphore to write thread
	double time=0.00;							//time variable to store the current time value
	clock_gettime(MY_CLOCK, &write_value_struct.update);			//clock_gettime provides the current time update
	time=write_value_struct.update.tv_sec;					//time variable stores the current time
	write_value_struct.Lat=0.01*time;					//update latitude
	write_value_struct.Long=0.2*time;					//update longitude
	write_value_struct.alt=0.25*time;					//update altitude
	write_value_struct.roll=sin(time);					//update roll
	write_value_struct.pitch=cos(time*time);				//update pitch
	write_value_struct.yaw=cos(time);					//update yaw
	printf("timestamp: %d sec %d nsec\n",write_value_struct.update.tv_sec, write_value_struct.update.tv_nsec);
        printf("Written values:\n Latitude: %0.2f Longitude: %0.2f Altitude: %0.2f\n Roll:%0.2f Pitch:%0.2f Yaw: %0.2f\n",
			write_value_struct.Lat, write_value_struct.Long, write_value_struct.alt, 
			write_value_struct.roll, write_value_struct.pitch, write_value_struct.yaw);
	pthread_mutex_unlock(&shared_sem);					//unlock or give the shared semaphore
        sleep(1);								//wait for 1 second
        t_count++;								//increment the total count variable
	t_wcount++;								//increment the wcount variable so that once it reaches
										//10 reset
	}
	if(t_count==180){
		exit(-1);
	}	
}

void* read_thread()
{
	struct timespec timed;
	clock_gettime(MY_CLOCK,&timed);
	timed.tv_sec+=10;
	timed.tv_nsec+=0;
	int rc=0;
	while(t_count<180)							//continue the process until 180 seconds
	{
	sleep(10);
	rc=pthread_mutex_timedlock(&shared_sem,&timed);				//implement timedlock instead of lock to avoid deadlock condition.
										//it tries to lock the sempahore and if not acquired waits for 10 seconds for it to unlock
										//even after the its not satisfied then back off
	if(rc)
	{
			printf("No new data available at time %lf sec %lf nsec\n", write_value_struct.update.tv_sec, write_value_struct.update.tv_nsec);
	}
	else{
	printf("*********************************************************READ ALERT*************************************************************\n");
	printf("Reading values in THREAD2 read_number= %d\n", t_rcount);
	t_rcount++;
	printf("timestamp: %d sec %d nsec\n", write_value_struct.update.tv_sec, write_value_struct.update.tv_nsec);
	printf("Read values:\n Latitude: %0.2f Longitude: %0.2f Altitude: %0.2f\n Roll:%0.2f Pitch:%0.2f Yaw: %0.2f\n",
                        write_value_struct.Lat, write_value_struct.Long, write_value_struct.alt, 
                        write_value_struct.roll, write_value_struct.pitch, write_value_struct.yaw);			
	pthread_mutex_unlock(&shared_sem);					//unlock the shared semaphore
	printf("*********************************************************READ ALERT**************************************************************\n");
	}
	}
}

int main (int argc, char *argv[])
{
	int rc=0;
	double tstart,tend,tdiff;

	struct timespec start_time;
	clock_gettime(MY_CLOCK,&start_time);
	printf("creating write thread\n");
        threadParams[THREAD_1].thread_idx=THREAD_1;				//give thread index as 1
	rc=pthread_create(&threads[0],NULL,write_thread, (void*)&threadParams[THREAD_1]);	//create thread for write
	if(rc){printf("ERROR; pthread_create() rc is %d\n", rc);perror(NULL);exit(-1);}		//print if error
	printf("Write thread spawned\n");	

        printf("creating read thread\n");
        threadParams[THREAD_2].thread_idx=THREAD_2;				//give thread index as 2
        rc=pthread_create(&threads[1],NULL,read_thread, (void*)&threadParams[THREAD_2]);	//create thread for read
        if(rc){printf("ERROR; pthread_create() rc is %d\n", rc);perror(NULL);exit(-1);} 	//print if error
        printf("Read thread spawned\n");       

       if(pthread_join(threads[0],NULL) == 0)					//wait until write thread executed
       	printf("Write Thread %x done\n", (unsigned int)threads[0]);
       else 
	perror("Write Thread");

       if(pthread_join(threads[1],NULL) == 0)					//wait until read thread executed
        printf("Read Thread %x done\n", (unsigned int)threads[1]);
       else
        perror("Read Thread");


	tstart=(double)start_time.tv_sec+((double)start_time.tv_nsec/1000000000);
	tend=(double)write_value_struct.update.tv_sec+((double)write_value_struct.update.tv_nsec)/1000000000;

	tdiff=tend-tstart;
	printf("total time:%lf\n",tdiff);
		
       if(pthread_mutex_destroy(&shared_sem)!=0)				//destroy the mutex
		perror("sharedSem destroy");

	printf("DONE\n");

	exit(0);
}
