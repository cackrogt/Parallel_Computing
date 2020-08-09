#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

int size;
int largest_arr;

//for qsort
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void Print_answer(int* arr, int n){
   FILE *fp;

   fp = fopen("answeris.txt", "w+");
   
   

   int i =0;
   for(i =0; i<n; i++){
      fprintf(fp,"%d ", arr[i]);
   }
   printf("\n");
   fclose(fp);
}
void print_time(double time, int n_threads){
	FILE *fp;

   fp = fopen("timeis.txt", "w+");
   fprintf(fp,"for # %d threads, time is %lf", n_threads,time);
   fclose(fp);
}
void Print_answer_1(int* arr, int n,int my_id){
   int i =0;
   printf("%d in the oe fun \n",my_id);
   for(i =0; i<n; i++){
      printf("%d ", arr[i]);
   }
   printf("\n");
}

void get_input(int my_id, int *global_n, int *global_arr){
   
	FILE* fp;
	fp = fopen("input.txt", "r");
   if(my_id == 0){
      printf("num of values(greater than num processes, lim is 50000), then values is in the input file\n");

      
      
      fscanf(fp,"%d", global_n);
      //global_arr = (int *)malloc((int)sizeof(int)*(*global_n));   
   }
   MPI_Bcast(global_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
   if(my_id == 0){
      for(int i= 0; i<*global_n; i++){
            fscanf(fp,"%d",&global_arr[i]);
         }
      //printf("%d is recieving \n", my_id);
      //Print_answer(global_arr,*global_n);
         
   }
   fclose(fp);
   
   MPI_Bcast(global_arr, *global_n, MPI_INT, 0, MPI_COMM_WORLD);

   
}

void oddeven_merge_etc(int* local_arr, int local_n, int my_id, int partner, int phase, int interval){
   MPI_Status status;
   //now in my case the last interval can be large, so one can be 

   
   int temp_B_size = 0;

   // temp_B_size should be the size of the partner array
   if(partner == size-1){
      temp_B_size = largest_arr;
   }
   else{
      temp_B_size = interval;
   }
   //printf("my id and partners id %d %d\n",my_id, partner );
   int* temp_B = (int*)malloc(sizeof(int)*temp_B_size);
   //printf("%d %d is in oddevenmerge \n", my_id,temp_B_size);

   //printf("the id %d is reaching the oddeven part i = %d\n",my_id, phase );

   MPI_Barrier(MPI_COMM_WORLD);
   if(partner >= 0 && partner < size){
   
      MPI_Sendrecv(local_arr, local_n, MPI_INT, partner, 0,
                  temp_B, temp_B_size, MPI_INT, partner, 0, MPI_COMM_WORLD,
                  &status);
   }

   MPI_Barrier(MPI_COMM_WORLD);

   if(partner >= 0 && partner < size){
      int* temp_C = (int*)malloc(sizeof(int)*(temp_B_size+local_n));
      for(int i =0; i<local_n; i++){
         temp_C[i] = local_arr[i];
      }
      for(int i = local_n; i<(local_n + temp_B_size); i++){
         temp_C[i] = temp_B[i-local_n];
      }
      
      //qsort(temp_C, (local_n + temp_B_size),sizeof(int), cmpfunc);
      //printf("got temp_c in odd even %d\n",my_id );
      //Print_answer_1(temp_C, temp_B_size+local_n,my_id);

      // which part to take

      //merge logic
      int i =0, j=0;

      while((i+j)< (local_n+temp_B_size)){
      	if(i<local_n && j < temp_B_size){
      		if(local_arr[i] < temp_B[j]){
      			temp_C[i+j] = local_arr[i];
      			i++; 
      		}
      		else{
      			temp_C[i+j] = temp_B[j];
      			j++;
      		}
      	}
      	else if(i == local_n && j<temp_B_size){
      		temp_C[i+j] = temp_B[j];
      		j++;
      	}
      	else{
      		temp_C[i+j] = local_arr[i];
      		i++;
      	}
      }

      if(my_id < partner){
         for(int i =0; i< local_n; i++){
            local_arr[i] = temp_C[i];
         }
      }
      else{
         for(int i =temp_B_size; i< temp_B_size+local_n; i++){
            local_arr[i - temp_B_size] = temp_C[i];
         }
      }

   }
   free(temp_B);
   //free(temp_C);

}







void sort(int* local_arr, int* global_arr, int interval, int my_id, int n_threads, int global_n,int* local_n,int* global_arr_2,int* count,int* displs){
   
   if(my_id < n_threads-1){
      //local_arr = (int*) malloc(sizeof(int)*interval);
      *local_n = interval;
      //printf("interval in sort %d\n",local_n );
      for(int i =0; i<interval; i++){
         local_arr[i] = global_arr[(my_id*interval) + i];
      }
   }
   else{
      //local_arr = (int*) malloc(sizeof(int)*(global_n - ((n_threads-1)*interval)));
      *local_n = global_n - ((n_threads-1)*interval);
      //printf("interval in sort for last %d\n",local_n );
      for(int i =0; i<*local_n; i++){
         local_arr[i] = global_arr[(my_id*interval) + i];
      }
   }

   //qsort(local_arr, *local_n, sizeof(int), cmpfunc);
   //printf("local array after sort %d %d    %d \n", local_arr[0],local_arr[1], local_arr[local_n-1]);

   int i, j;  
    for (i = 0; i < *local_n-1; i++)   {   
      
    // Last i elements are already in place  
    for (j = 0; j < *local_n-i-1; j++)  {
        if (local_arr[j] > local_arr[j+1]){
            int temp = local_arr[j+1];
            local_arr[j+1] = local_arr[j];
            local_arr[j] = temp;

        }
    }

    }
   int phase =0;
   int partner =0;
   for(phase =0; phase<n_threads; phase++){
      if(phase%2 ==0){
         if(my_id%2 == 0){
            partner = my_id +1;
         }
         else{
            partner = my_id -1;
         }
      }
      else{
         if(my_id%2 == 0){
            partner = my_id -1;
         }
         else{
            partner = my_id+1; 
         }
      }
      

      oddeven_merge_etc(local_arr, *local_n, my_id, partner, phase, interval);
   }

   //for(int i =0; i<local_n; i++){
     //    global_arr[(my_id*interval) + i]= local_arr[i];
     // }
   //Print_answer_1(local_arr, *local_n,my_id);

   MPI_Gather( local_n, 1, MPI_INT, count, 1, MPI_INT, 0, MPI_COMM_WORLD);
   //printf("this is the count for rank %d :- %d \n", my_id, count[my_id]);
   MPI_Barrier(MPI_COMM_WORLD);
   if(my_id == 0){
      displs[0] = 0; 
       for (int i=1; i<size; ++i) { 
           displs[i] = displs[i-1]+count[i-1]; 
       }
       
   }

   //MPI_Barrier(MPI_COMM_WORLD);
   MPI_Gatherv(local_arr, *local_n, MPI_INT, global_arr_2, count,displs, MPI_INT, 0, MPI_COMM_WORLD);
   //MPI_Barrier(MPI_COMM_WORLD);

}

int main(int argc, char* argv[]){
   int local_n;
   int global_n;
   // some issue with synchronization of dynamic allocation of these arrays
   int* local_arr = (int*)malloc(sizeof(int)*500001);
   int* global_arr = (int*)malloc(sizeof(int)*500001);
   int* global_arr_2 = (int*)malloc(sizeof(int)*500001);
   int interval;
   int n_threads;
   int my_id;
   double start, finish, loc_elapsed;


   MPI_Init(NULL,NULL);
   MPI_Comm_size(MPI_COMM_WORLD, &n_threads);
   size = n_threads;


   MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

   get_input(my_id, &global_n, global_arr);
   //Print_answer(global_arr,global_n);
   int* count = (int*) malloc(sizeof(int)*n_threads);


   int* displs = (int*) malloc(sizeof(int)*n_threads);

   interval = global_n/n_threads;
   largest_arr = global_n - ((n_threads-1)*interval);
   count[size-1] = largest_arr;
   for(int i =0; i<size-1;i++){
      count[i] = interval;
   }
   //printf("just before barrier\n");
   MPI_Barrier(MPI_COMM_WORLD);
   //printf("just entered barrier\n");
   start = MPI_Wtime();
   sort(local_arr,global_arr,interval,my_id, n_threads,global_n, &local_n,global_arr_2,count,displs);
   finish = MPI_Wtime();
   loc_elapsed = finish-start;

   if(my_id == 0){
      printf("final answer in answeris.txt \n");
      Print_answer(global_arr_2, global_n);
      print_time(loc_elapsed,n_threads);
   }
   free(global_arr);
   free(global_arr_2);
   free(local_arr);
   free(count);
   free(displs);
   MPI_Finalize();
   
}
