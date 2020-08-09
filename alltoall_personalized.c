#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

//only decreases when passes through node 0

void Get_Input(int my_id, int* num_revolutions){
	if(my_id == 0){
		printf("enter number of revolutions\n");
		scanf("%d",num_revolutions);//only changes for 0th rank
	}

	//MPI_Bcast(num_revolutions, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(){
	int num_nodes;
	int num_revolutions = 0;
	
	int my_id;
	int size;
	int recieve_from;
	int send_to;
	float start, end, total_time;

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	//Get_Input(my_id,&num_revolutions);
	int msg[size];
	//int rcv[size];
	for(int i =0; i<size; i++){
		msg[i]= my_id*size + i;
		//rcv[i] = 0;
	}
	
	if(my_id == 0){
		printf("printing the initial matrix, though mpi gather is used only in the end, not here\n");
		for(int i =0; i<size*size; i++){
			printf("%d ",i);
			if((i+1)%(size) == 0){
				printf("\n");
			}
		}
	}

	num_revolutions = size-1;
	recieve_from = (my_id - 1 + size)%size; // std modular substraction
	send_to = (my_id + 1)%size;
	MPI_Barrier(MPI_COMM_WORLD);
	int sender[size-1];
	int reciever[size-1];
	for(int i =0; i<size-1; i++){
			sender[i] = msg[(my_id + i + 1)%size];
		}

	start = MPI_Wtime();
	
	
	while(num_revolutions >0){


		
		printf("sending for %d th revolution, from id %d\n", num_revolutions, my_id);
		MPI_Send(sender, num_revolutions, MPI_INT, send_to, 0, MPI_COMM_WORLD);



		MPI_Recv(reciever, num_revolutions, MPI_INT, recieve_from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		msg[(my_id-(size-num_revolutions) + size)%size] = reciever[0];

		num_revolutions--;
		for(int i =0; i<num_revolutions; i++){
			sender[i] = reciever[i+1];
		}

	}

	
	end = MPI_Wtime();
	total_time = end - start;
	float final_time;
	MPI_Reduce(&total_time, &final_time, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
	if(my_id == 0){
		printf("maximum time taken by any process is %f\n", final_time);
	}
	int* matrix;
	if(my_id == 0){
		matrix = (int*)malloc(sizeof(int)*size*size);
	}

	MPI_Gather(msg, size, MPI_INT, matrix, size, MPI_INT, 0,
           MPI_COMM_WORLD);
	if(my_id == 0){
		for(int i =0; i<size*size; i++){
			printf("%d ",matrix[i]);
			if((i+1)%(size) == 0){
				printf("\n");
			}
		}
	}
	MPI_Finalize();
	return 0;
}