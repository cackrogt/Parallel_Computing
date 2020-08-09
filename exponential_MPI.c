#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>




float custom_term(int exp,float x){\

	float temp_prod = 1;
	for(int i =1; i<=exp; i++){
		temp_prod *= (float)x;
		temp_prod /= (float)i;
	}

	return temp_prod;
}

void Get_Input(int* MAX_VAL,float *x, int id){
	//we will look for the errors later
	if(id == 0){
		printf("enter x (float ) and number of terms(int)(number of terms should be greated than number of processes)\n");
		scanf("%f %d",x,MAX_VAL);
		//scanf();
	}
	//we need to brodcast to everyone this
	MPI_Bcast(x, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(MAX_VAL, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

float Actual(int id, int interval, int size,float x, int MAX_VAL){
	
	float sum = 0;

	if(id < size-1){
		float init;

		int exp_start = id*interval;//finding power and factorial
		//initial sum value for this interval
		//printf("%d proc id %d exp_start \n", id, exp_start);

		sum+= custom_term(exp_start, x);
		//printf("%d proc id %f sum \n", id, sum);
		init = sum;// a temporary variable
		for(int i =(id*interval) + 1;i<(id*interval + interval);i++){
			init *= x;
			init /= i;
			sum+=init;
		}
		//printf("%d proc id %f sum ", id, sum);
	}

	else if(id == size-1){
		float init;

		int exp_start = id*interval;//finding power and factorial
		//initial sum value for this interval
		//printf("%d proc id %d exp_start \n", id, exp_start);

		sum+= custom_term(exp_start,x);
		//printf("%d proc id %f sum \n", id, sum);
		init = sum;// a temporary variable
		for(int i =(id*interval) + 1;i<MAX_VAL;i++){
			init *= x;
			init /= i;
			sum+=init;
		}
	}
	//printf("%d proc id %f sum\n", id, sum);

	return sum;
}

int main(int argc, char* argv[]){
	float x;
	int size;
	int proc_id;
	int interval;
	float local_calc;
	float final_value;
	int MAX_VAL;
	

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    	MPI_Comm_size(MPI_COMM_WORLD, &size);

    	Get_Input(&MAX_VAL,&x, proc_id);
    	interval = MAX_VAL/size;
	//printf("%d proc id %d intrval\n", proc_id, interval);
    
    	MPI_Barrier(MPI_COMM_WORLD);

    	local_calc = Actual(proc_id,interval,size,x,MAX_VAL);
	//printf("%d proc id %f localcalc\n", proc_id, local_calc);
    	MPI_Reduce(&local_calc, &final_value, 1, MPI_FLOAT, MPI_SUM, 0,MPI_COMM_WORLD);
    	if(proc_id == 0){
    		printf("the value is %f\n", final_value);
    	}
    	MPI_Finalize();
	return 0; 



}



