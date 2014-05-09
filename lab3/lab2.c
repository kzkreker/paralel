#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <sys/time.h>


#define C_PARAM = 5


void insertionsort(double a[], int n, int stride) {
    int j;
    for (j=stride; j<n; j+=stride) {
        double key = a[j];
        int i = j - stride;
        while (i >= 0 && a[i] > key) {
            a[i+stride] = a[i];
            i-=stride;
        }
        a[i+stride] = key;
    }
}

void shellsort(double a[], int n)
{
    int i, m;

    for(m = n/2; m > 0; m /= 2)
    {
            #pragma omp parallel for shared(a,m,n) private (i) default(none)
            for(i = 0; i < m; i++)
                insertionsort(&(a[i]), n-i, m);
    }
}



void printmatrix (double a[],int n)
{
	int i;

	for(i = 0; i < n; i++){
		printf("|%0.1f|", a[i]);

	}
}

int main(int argc, char* argv[])
{
        int i,j,k, N,r;
        N = atoi(argv[1]);

	struct timeval T1, T2;
	long delta_ms;
	double vector = 5;


	gettimeofday(&T1, NULL);
        printf("Started\n");


 #pragma omp parallel shared(r)
 {
	#pragma omp for schedule(guided,1) private (i,j) nowait
	for (r=0; r<100; r++)
  	{
		double random_number;
        double M1[N][N];
        double M2[N][N];
        double M3[N][N];
        double V[N];
        double sred;
        double num = 0;
        double Nt=N;

        double a,b;

	gsl_rng * rgen = gsl_rng_alloc(gsl_rng_taus);
        a=31,15;
        b=-1,444445;

		//printf("\n\nMatrix1\n");
		for (i = 0; i < N; i++ ){
		      for (j = 0; j < N; j++ ){
			  random_number= gsl_ran_gamma(rgen,a,b);
			  M1[i][j] = random_number;
			  //printf("|%f|", M1[i][j]);
		      }
		//printf("\n");
		}

		//printf("Matrix2\n");

		for (i = 0; i < N; i++ ){
                      for (j = 0; j < N; j++ ){
                          M2[i][j] = M1[i][j]*vector;
                          //printf("|%f|", M2[i][j]);
                      }
                      //printf("\n");
                }

		//printf("Matrix3 resoult\n");

		for (i = 0; i < N; i++ ){
		      for (j = 0; j < N; j++ ){
				M3[i][j] = 0;
				for (k = 0; k < N; k++ ){
					M3[i][j] = M3[i][j] + M1[i][k]*M2[k][j];
				}
				//printf("|%f|", M3[i][j]);

		      }
		      //printf("\n");
		}

		for (i = 0; i < N; i++ ){
		      V[i] =0;
		      sred=0;

		      for (j = 0; j < N; j++ ){
		      		sred += M3[i][j];
		      }
		      sred  = sred/Nt;
		      for (j = 0; j < N; j++ ){
				num = M3[i][j];
				num=num-sred;
		      		V[i] = num*num;
		      }
		      V[i]= V[i]/Nt;
		}

		//printf("\nBefore sort:\n");
		//printmatrix(V,N);
		//printf("\nShell sort:\n");

		shellsort(V,N);

		//printmatrix(V,N);
		//printf("\n\n");

	}
}
        printf("\nStopt\n");
        gettimeofday(&T2, NULL);
    	delta_ms =  1000*(T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec)/1000;
    	printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms);

	return 0;

}


