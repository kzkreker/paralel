#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

int main(char** argv, int args) {
 int    i;
 double a,b;
 gsl_rng * rgen = gsl_rng_alloc(gsl_rng_taus);
 
 a=5E9;
 b=1.0/a;
 printf("a=%e, b=%e, mean=%f, std dev=%f\n",a,b,a*b,sqrt(a)*b);
 for(i=1;i<10;i++){
   printf("%f\n",gsl_ran_gamma(rgen,a,b));
 }
 return 1;
}
