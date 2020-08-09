
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int N = 1000;
int NUM_THRDS = 4;
int main(){

	double sum[NUM_THRDS]; // to avoid race condition cant store all in one
	double final_sum= 0 ;  // final sum
	int nthreads;          //actual number of threads
	omp_set_num_threads(NUM_THRDS);//setting number of threads;
	double step = 1/(double)N; // setting the size of steps;


#pragma omp parallel
	{                                     //all declerations are for individual threads
		double x;                         //dont want overlap
		int id = omp_get_thread_num();    //thread num
		int nthrds = omp_get_num_threads(); // total number of threads
		int division = N/nthrds;
		if(id ==0) nthreads = nthrds;       // the actual number of threads allocated for us
		int i = id;
		for(i =id*division; i< (id*division)+division; i++){ // here we divide into division # of blocks and id is where we start
			x = (double)i*step + step*0.5;
			sum[id] = sum[id] + 4.0/(1.0 + x*x);
		}
#pragma omp critical
		final_sum += sum[id];              // classic case of critical section, but should be outside the previous loop, otherwise its practically a serial program
	}
	

	double pi = final_sum * step;
	printf("%lf \n",pi);
}

