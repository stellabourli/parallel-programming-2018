//Mpourlh Stulianh
//AM:  2774

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_OF_THREADS 10				//number of threads 
#define REPETITIONS 20					//repetitions of function

pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;		//initializations
pthread_barrier_t b;
int s = 0;

void *func()						//function for threads
{
    int work_done = 0;						//for every time all threads do their work, before repeat it
    int repeat = 0;						//counter for repetitions

    while(1) {
	if(work_done)						//every time thread do their work
	{
		if (repeat == REPETITIONS)			//if repetitions are over
		{
			break;					//break the loop
		}
			
		pthread_barrier_wait(&b);			//else threads must synchronize
		work_done = 0;					//initialize conition work_done
		continue;					//and continue
	}
        pthread_mutex_lock(&m2);				//mutex lock
        s += 1;							//modify global variable
        pthread_mutex_unlock(&m2);				//mutex unlock
	pthread_barrier_wait(&b);				// Wait for all threads to finish modifying global variable
        
	if(s > 0)						//check a condition
	{
		printf("Result: %d\n",s);			//printf global variable
	}
	repeat += 1;						//increase the number of repetitions
	work_done = 1;						//thread do work, so modify variable work_done to 1
    }
    return 0;
}

int main()
{
	int i;
	pthread_t my_threads[NUM_OF_THREADS];			//initialize threads
	pthread_barrier_init(&b ,NULL, NUM_OF_THREADS);		//call barrier_init
	struct timeval tv1,tv2;						   
	double t;
	
	gettimeofday(&tv1,NULL);
	
	for(i=0; i<NUM_OF_THREADS; i++)				//create threads
	{
		pthread_create(&my_threads[i], NULL, func, NULL);
	}
	
	
	for(i=0; i<NUM_OF_THREADS; i++)				//join threads
	{
		pthread_join(my_threads[i], NULL);
	}
	
	gettimeofday(&tv2,NULL);
	
								//runtime calculation
	t = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)*1.0E-6;
	printf("time: %lf sec.\n",t);
	
	return 0;
}
