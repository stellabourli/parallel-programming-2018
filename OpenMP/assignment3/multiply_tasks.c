//Mpourlh Stulianh
//AM:  2774

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <omp.h>

#include <string.h>

#define SIZE_A 1024                		//size of A array
#define SIZE_B SIZE_A	           		//size of B array
#define SIZE_C SIZE_A              		//size of C array, result of multiplication
#define N SIZE_C                   		//N is the size of array C


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void create_Arrays();					//prototypes
void taskexecute(int wid, int M, int S); 

int A[SIZE_A][SIZE_A] = {0};       		//definition and initialization of arrays
int B[SIZE_B][SIZE_B] = {0};
int C[SIZE_C][SIZE_C] = {0};

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
                   
void taskexecute(int wid, int M, int S)          		//function-task, multiply blocks
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

int main(int argc, char* argv[])
{								   
	int x, y, taskid=0;
	FILE *outfile;

	int S = atoi(argv[1]);				//S is the size of the block
	int M = N/S;	               		//blocks 
	int NTASK = M*M;		           	//NTASK is the number of blocks 
	
	create_Arrays();					//call function to create arrays A and B
	
	double start = omp_get_wtime();
	omp_set_dynamic(0);
	#pragma omp parallel num_threads(4) shared(M,S,NTASK)
	{
		#pragma omp single nowait
		{
			while(taskid < NTASK)
			{
				#pragma omp task firstprivate(taskid)
				{
					taskexecute(taskid, M, S);  
				}
				taskid++;
			}
		}
	}
	double end = omp_get_wtime();
	
	printf("Parallel multiplication with tasks: %lf sec.\n",end-start);
	
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
	
	printf("Array C is in file 'Cmat1024.txt'. \n");
	fclose(outfile);
	
	return 0;
}
