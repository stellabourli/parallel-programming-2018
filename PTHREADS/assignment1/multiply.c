//Mpourlh Stulianh
//AM:  2774

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#include <string.h>

#define NUM_OF_THREADS 1         		//number of threads (1,2,3,4,8,16)
#define SIZE_A 1024                		//size of A array
#define SIZE_B SIZE_A	           		//size of B array
#define SIZE_C SIZE_A              		//size of C array, result of multiplication

#define N SIZE_C                   		//N is the size of array C
#define M 16		               		//blocks (16,256,1024)
#define S N/M                      		//S is the size of the block
#define NTASK M*M		           	//NTASK is the number of blocks 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void taskexecute(int wid);         		//prototypes
void *thrfunc(void *arg);

						//mutex initialization
pthread_mutex_t tlock = PTHREAD_MUTEX_INITIALIZER;
int taskid = 0; 		           	//counter for tasks 


int A[SIZE_A][SIZE_A] = {0};       		//definition and initialization of arrays
int B[SIZE_B][SIZE_B] = {0};
int C[SIZE_C][SIZE_C] = {0};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void taskexecute(int wid)          		//function-task, multiply blocks
{
	int i, j, k, x, y;
	int sum;
	x = wid / M;
	y = wid % M;
	for (i = x*S; i < (x+1)*S; i++)
	{
		for (j = y*S; j < (y+1)*S; j++)
		{
			for (k = 0, sum = 0; k < N; k++)
			{
				sum += A[i][k]*B[k][j];
			}
			C[i][j] = sum;
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                   
void *thrfunc(void *arg)           		//the function of threads
{
	int t;

	while (1) 				//loop while all tasks done
	{ 
		pthread_mutex_lock(&tlock);
		t = taskid++; 
		pthread_mutex_unlock(&tlock);
		if (t >= NTASK)
		{
			break; 
		}
		taskexecute(t);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				   
void create_Arrays()               		//function creates arrays from files
{
	char fileA[15];
	char fileB[15];
	int number;
	FILE *fp;
	int x = 0,y = 0;
						//ask for files
	//printf("Give name of file, array A:\n");
	//scanf("%s",fileA);
	//printf("Give name of file, array B:\n");
	//scanf("%s",fileB);
	strcpy(fileA,"Amat1024");
	strcpy(fileB,"Bmat1024");
						//open first file and fill array A
	if((fp = fopen(fileA,"r")) == NULL)
	{
		printf("error in open file\n");
		exit(1);
	}
	while (fscanf(fp, "%d", &number) != EOF)
	{
		A[x][y] = number;
		if(y == SIZE_A-1)
		{
			y = 0;
			x += 1;
		}
		else
		{
			y += 1;
		}
	}
	fclose(fp);                    		//close first file
								   
	number = 0;
	x = 0;
	y = 0;
						//open second file and fill array A
	if((fp = fopen(fileB,"r")) == NULL)
	{
		printf("error in open file\n");
		exit(1);
	}
	while (fscanf(fp, "%d", &number) != EOF)
	{
		B[x][y] = number;
		if(y == SIZE_B-1)
		{
			y = 0;
			x += 1;
		}
		else
		{
			y += 1;
		}
	}
	fclose(fp);                    		//close second file
}
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	

int main()
{								   
	pthread_t threads[NUM_OF_THREADS];
	int i;
	int x,y;
	FILE *outfile;
	struct timeval tv1,tv2;   
	double t;
	
	create_Arrays();			//call function to create arrays A and B
	
	gettimeofday(&tv1,NULL);

	for(i=0; i<NUM_OF_THREADS; i++)		//create threads
		pthread_create(&threads[i], NULL, thrfunc, NULL);
	
	for(i=0; i<NUM_OF_THREADS; i++)
		pthread_join(threads[i], NULL);
	
	gettimeofday(&tv2,NULL);
						//create and open file to write result of multiplication (array C)
	if((outfile = fopen("Cmat1024.txt","w")) == NULL)
	{
		printf("error in open file\n");
		exit(1);
	}
						//write the result (array C) in file
	for (x = 0; x< SIZE_C; x++)
	{
		for(y = 0; y< SIZE_C; y++)
		{
			fprintf(outfile,"%d ",C[x][y]);
		}
		
	}
	
						//runtime calculation
	t = (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)*1.0E-6;
	printf("time: %lf sec.\n",t);
	
	printf("All threads done. Array C is in file 'Cmat1024.txt'. \n");
	fclose(outfile);
	
	return 0;
}
