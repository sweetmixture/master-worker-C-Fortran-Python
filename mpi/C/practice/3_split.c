#include <stdio.h>
#include <mpi.h>

#define SUB_COMM_SIZE 2



void print_subcomm_config_1( MPI_Comm comm )
{
	int rank,size;
	MPI_Comm_rank(comm,&rank);	// possible to get 'rank'/'size' from 'MPI_Comm'
	MPI_Comm_size(comm,&size);

	for(int i=0;i<size;i++){
		
		if( rank == i )
			printf("rank / size : %d / %d\n",rank+1,size);
	}
	return;
}

void print_subcomm_config_2( MPI_Comm comm, const int rank, const int size )
{
	for(int i=0;i<size;i++){

		if( rank == i )
			printf("rank / size : %d / %d\n",rank+1,size);
	}
	return;
}		

int main(int argc, char* argv[])
{
	int bsize,brank;
	int ssize,srank;

	MPI_Comm BaseComm = MPI_COMM_WORLD;
	MPI_Comm SubComm;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(BaseComm,&brank);
	MPI_Comm_size(BaseComm,&bsize);

	// setting subcomm size
	int n_subcomm = bsize / SUB_COMM_SIZE;
	if( (bsize%SUB_COMM_SIZE) != 0 ){ n_subcomm++; }

	// configure subcomm
	int subcomm_tag = brank / SUB_COMM_SIZE;
	MPI_Comm_split(BaseComm,subcomm_tag,brank,&SubComm);
	MPI_Comm_size(SubComm,&ssize);
	MPI_Comm_rank(SubComm,&srank);

	// Exercise 1. Subcomm Access with tag 2
	for(int i=0;i<n_subcomm;i++){

		if( subcomm_tag == i ){
			
			if( srank == 0 ){
				printf("this is subcomm: %d\n",subcomm_tag);
			}
			// Only Subcomm with subcomm_tag = 'i' 
			//print_subcomm_config_1(SubComm);
			print_subcomm_config_2(SubComm,srank,ssize);
		}
	}

	MPI_Finalize();
	return 0;
}
