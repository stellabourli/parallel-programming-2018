//Mpourlh Stulianh
//AM:  2774

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1024
int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
	int i, j, k, sum;

	// Read matrices from files: "A_file", "B_file" 
	if (readmat("Amat1024", (int *) A, N) < 0) 
		exit( 1 + printf("file problem\n") );
	if (readmat("Bmat1024", (int *) B, N) < 0) 
		exit( 1 + printf("file problem\n") );
		
	//multiply arrays
	double start = omp_get_wtime();
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			omp_set_dynamic(0);
			//parallelization for third loop
			#pragma omp parallel private(sum,k) shared(i,j,A,B,C) num_threads(4)
			{
				#pragma omp for schedule(runtime)
					for (k = sum = 0; k < N; k++)
						sum += A[i][k]*B[k][j];
					#pragma omp critical
					C[i][j] += sum;
			}
		}
	}
	double end = omp_get_wtime();
	
	printf("Parallel multiplication 3: %lf sec.\n",end-start);

	// Save result in "Cmat1024"
	writemat("Cmat1024", (int *) C, N);

	return (0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Utilities to read & write matrices from/to files

#define _mat(i,j) (mat[(i)*n + (j)])


int readmat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "r")) == NULL)
		return (-1);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (fscanf(fp, "%d", &_mat(i,j)) == EOF)
			{
				fclose(fp);
				return (-1); 
			};
	fclose(fp);
	return (0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int writemat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "w")) == NULL)
		return (-1);
	for (i = 0; i < n; i++, fprintf(fp, "\n"))
		for (j = 0; j < n; j++)
			fprintf(fp, " %d", _mat(i, j));
	fclose(fp);
	return (0);
}
