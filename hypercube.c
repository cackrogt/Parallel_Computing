#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

//only decreases when passes through node 0
char* binaryof(int n, char* res){
	int binary[32];

	int i = 0; 
	while(n>0){
		binary[i] = n%2;
		i++;
		n/=2;
	}
	res = (char*)malloc(sizeof(char)*i);
	for(int j = i-1; j>=0; j--){
		res[j] = binary[j] + '0';
	}
	return res;
}
int graycode(int n){

	return (n ^ (n >> 1));
}

void Get_Input(int my_id, int* arr,int num_nodes){
	if(my_id == 0){
		printf("enter elements of the array,<num elements> should be equal to <num processes>\n");
		printf("each element will be transported to process number corresponding to gray code value of its index in array\n");
		for(int i =0; i<num_nodes; i++ ){
			scanf("%d",&arr[i]);//only changes for 0th rank
		}
	}
	if(my_id == 0){
		for(int i =1; i<num_nodes; i++){
			int temp[2];
			temp[0] = arr[i];
			temp[1] = i;
			int send = graycode(i);
			MPI_Send(temp, 2, MPI_INT, send,0,MPI_COMM_WORLD);
		}
	}

	//MPI_Bcast(num_revolutions, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(){
	//int num_nodes;
	int my_id;
	int size;

	

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int num_nodes = size;
	int rcv[2];
	int* arr = (int*)malloc(sizeof(int)*num_nodes);
	Get_Input(my_id,arr,num_nodes);
	//we transport the elements to their 
	int m = size-1;
	int sizeofbin = 0;
	while(m>0){
			//binary[i] = m%2;
			sizeofbin++;
			m/=2;
		}

	if(my_id == 0){
		char bleah[sizeofbin+1];
		for(int i =0; i<sizeofbin; i++){
			bleah[i] = '0';
		}
		bleah[sizeofbin] = '\0';
		printf("we already have 0th term with process 0, gray code of process is %s, value is: %d\n", bleah, arr[0]);
	}
	
	
	if(my_id!=0){
		MPI_Recv(rcv, 2,MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int binary[32];
		int n = my_id;
		int i = 0; 
		while(n>0){
			binary[i] = n%2;
			i++;
			n/=2;
		}
		char res[sizeofbin+1];
		for(int x = 0; x<sizeofbin; x++){
			res[x] = '0';
		}
		for(int j = i; j>0; j--){
				res[sizeofbin-j] = binary[j-1] + '0';
				//i--;
		}
		res[sizeofbin] = '\0';
		printf("recieved from 0,value:%d index in original array:%d embedded in process %d, process num in binary %s\n",rcv[0],rcv[1],my_id,res);
	}
	
	

	MPI_Finalize();
	return 0;
}