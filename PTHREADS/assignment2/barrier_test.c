//Mpourlh Stulianh
//AM:  2774

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_OF_THREADS 2					//number of threads 
#define REPETITIONS 5						//repetitions of function

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct barrier						//struct barrier_t
{
	int counter;						//check, in barrier_wait, how many threads are in barrier
	int state;						//check if threads are first threads or not
	int flag;						//check if last thread arrives
	int flag2;						//check if last thread arrives
	int threads; 						//number of threads to synchronize
	pthread_mutex_t m;					//one thread at a time increase the counter in barrier_wait
	pthread_cond_t c;					//if last thread arrives, it sends a signal to the rest
}barrier_t;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void barrier_init(barrier_t *b, int n);				//prototypes
void barrier_wait(barrier_t *b);
void barrier_destroy(barrier_t *b);
void *func();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;			//initializations
barrier_t b;
int s = 0;							//global variable

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void barrier_init(barrier_t *b,int n)				//function barrier_init
{
	b->counter = 0;						//initialize counter to zero
	b->state = 0;						//initialize state to zero
	b->flag = 0;						//initialize flag to zero
	b->flag2 = 0;						//initialize flag to zero
	b->threads = n; 					//initialize number of threads to synchronize
	pthread_mutex_init(&b->m, NULL);			//initialize mutex
	pthread_cond_init(&b->c, NULL);				//initialize condition variable
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//1os tropos
//----------

void barrier_wait(barrier_t *b)					//function barrier_wait
{
	pthread_mutex_lock(&b->m);				//mutex lock
	b->counter += 1;					//increase counter of threads
	if(b->state == 0)					//if this is one of the first threads
	{
		if(b->counter == b->threads)			//if last thread arrives
		{
			b->state = 1;				//modify state to 1, if another thread arrives while there are threads in barrier 
								//go to else state 
			b->counter = 0;				//initialize counter
			b->flag = 1;				//modify flag to unlock the threads
			b->flag2 = 0;				//modify flag2 to zero
			pthread_cond_broadcast(&b->c);		//send signal to the rest threads
		}
		while(b->flag == 0)				//while thread is not the last thread
		{	
			pthread_cond_wait(&b->c,&b->m);		//wait until last thread arrives
		}
	}
	else							//if some of the first threads have not left the barrier yet and a thread arrives 
	{
		if(b->counter == b->threads)			//if last thread arrives
		{
			b->state = 0;				//modify state to 0, if another thread arrives while there are threads in barrier 
								//go to if state 
			b->counter = 0;				//initialize counter
			b->flag2 = 1;				//modify flag2 to unlock the threads
			b->flag = 0;				//modify flag to zero
			pthread_cond_broadcast(&b->c);		//send signal to the rest threads
		}
		while(b->flag2 == 0)				//while thread is not the last thread
		{	
			pthread_cond_wait(&b->c,&b->m);		//wait until last thread arrives
		}
	}
	pthread_mutex_unlock(&b->m);				//mutex unlock
}

/*
//2os tropos
//-----------
void barrier_wait(barrier_t *b)					//function barrier_wait
{
	pthread_mutex_lock(&b->m);				//mutex lock
	b->counter += 1;					//increase counter 
	if(b->counter == b->threads)				//if last thread arrives
	{
		b->counter = 0;					//initialize counter to zero 
		pthread_cond_broadcast(&b->c);			//send signal to the rest
	}
	else							//if this is not the last thread
	{	
		pthread_cond_wait(&b->c,&b->m);			//wait until last thread arrives
	}
	pthread_mutex_unlock(&b->m);				//mutex unlock
}
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void barrier_destroy(barrier_t *b)				//function barrier_destroy
{
	if(b->counter == 0)					//if there is no thread use barrier
	{
		pthread_cond_destroy(&b->c);			//destroy condition variable
		pthread_mutex_destroy(&b->m);			//destroy mutex
	}
	else							//if there are threads in barrier
	{
		printf("Barrier is in use!\n");			//print message
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void *func()							//function for threads
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
			
		barrier_wait(&b);				//else threads must synchronize
		work_done = 0;					//initialize conition work_done
		continue;					//and continue
	}
        pthread_mutex_lock(&m2);				//mutex lock
        s += 1;							//modify global variable
        pthread_mutex_unlock(&m2);				//mutex unlock
	barrier_wait(&b);					// Wait for all threads to finish modifying global variable
        
	if(s > 0)						//check a condition
	{
		printf("Result: %d\n",s);			//printf global variable
	}
	repeat += 1;						//increase the number of repetitions
	work_done = 1;						//thread do work, so modify variable work_done to 1
    }
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
	int i;
	pthread_t my_threads[NUM_OF_THREADS];			//initialize threads
	barrier_init(&b, NUM_OF_THREADS);			//call barrier_init
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
