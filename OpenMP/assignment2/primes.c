//Mpourlh Stulianh
//AM:  2774

#include <stdio.h>
#include <omp.h>

#define UPTO 10000000

long int count,      /* number of primes */
         lastprime;  /* the last prime found */
         
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void serial_primes(long int n) {
	long int i, num, divisor, quotient, remainder;

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	for (i = 0; i < (n-1)/2; ++i) {    /* For every odd number */
		num = 2*i + 3;

		divisor = 1;
		do 
		{
			divisor += 2;                  /* Divide by the next odd */
			quotient  = num / divisor;  
			remainder = num % divisor;  
		} while (remainder && divisor <= quotient);  /* Don't go past sqrt */

		if (remainder || divisor == num) /* num is prime */
		{
			count++;
			lastprime = num;
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void openmp_primes(long int n) {
	long int i, num, divisor, quotient, remainder;

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	omp_set_dynamic(0);
	#pragma omp parallel private(i,num,divisor,quotient,remainder) shared(count,lastprime) firstprivate(n) num_threads(4) 
	{
		#pragma omp for schedule(guided,1000000) nowait
			for (i = 0; i < (n-1)/2; ++i) {    /* For every odd number */
				num = 2*i + 3;
		
				divisor = 1;
				do 
				{
					divisor += 2;                  /* Divide by the next odd */
					quotient  = num / divisor;  
					remainder = num % divisor;  
				} while (remainder && divisor <= quotient);  /* Don't go past sqrt */
		
				if (remainder || divisor == num) /* num is prime */
				{
					#pragma omp critical
						count++;
						lastprime = num;
				}
			}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
	printf("Serial and parallel prime number calculations:\n\n");
	
	double start_s = omp_get_wtime();
	serial_primes(UPTO);     
	double end_s = omp_get_wtime();	  
	printf("[serial] count = %ld, last = %ld (time = %lf sec.)\n", count, lastprime,end_s-start_s);
	
	double start_p = omp_get_wtime();
	openmp_primes(UPTO);  
	double end_p = omp_get_wtime();	  
	printf("[openmp] count = %ld, last = %ld (time = %lf sec.)\n", count, lastprime,end_p-start_p);
	
	return 0;
}
