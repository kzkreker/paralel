void *doWork( void *arg){

	int N;
	int i,j,k,r;
        N = (int *) arg;
 	printf("|%i|", N);
	double vector = 5;
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
	for(r = 0; r<50; r++){
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
				M3[i][j] =0;
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

