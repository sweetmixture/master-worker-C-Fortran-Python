#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define __TEST
#define SUBCOMM_SIZE 4

int main()
{
	int basecomm_rank, basecomm_size;
	MPI_Comm BaseComm = MPI_COMM_WORLD;
	MPI_Comm SubComm;

	int Nsubcomm;
	int subcomm_color;	// by 'color' it tells the subcomm, i.e., comms in same color, in the same group

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(BaseComm,&basecomm_rank);
	MPI_Comm_size(BaseComm,&basecomm_size);

	/* SubComm with same 'subcomm_color' will be allocated to the same 'SubComm'

		e.g., with 8 processes
		rank |	color
		0    |   0
		1    |   0
		2    |   0
		3    |   0
		4    |   1
		5    |   1
		6    |   1
		7    |   1

		rank 0-3 -> subcomm color: 0
		rank 4-7 -> subcomm color: 1

		2 subcomms created
	*/

	// Get number of subcomms (or work groups)
	Nsubcomm = basecomm_size / SUBCOMM_SIZE;
	if ( basecomm_size % SUBCOMM_SIZE != 0 ){ Nsubcomm++; }

	if( basecomm_rank == 0 )
	{	printf("number of groups generated : %d \n", Nsubcomm);
	}
	// Get subcomm color
	subcomm_color = basecomm_rank / SUBCOMM_SIZE;

	// Split the BaseCommunicator - using 'basecomm_rank' the original order of processes (in 'BaseComm') goes to subcomm
	// this does not mean the 'BaseComm' is dead
	MPI_Comm_split(BaseComm, subcomm_color, basecomm_rank, &SubComm);

	int subcomm_rank, subcomm_size;
	MPI_Comm_rank(SubComm,&subcomm_rank);
	MPI_Comm_size(SubComm,&subcomm_size);

	//printf("BaseCommRank/BaseCommSize: %d/%d \t SubCommRank/SubCommSize: %d/%d\n", basecomm_rank+1,basecomm_size, subcomm_rank+1,subcomm_size);

	printf("subcomm group / rank / size : %d / %d / %d\n",subcomm_color,subcomm_rank,subcomm_size);

	#ifdef __TEST

	//for(int i=0;i<basecomm_size;i++){ printf("basecomm rank/size : %d/%d \n",basecomm_rank+1,basecomm_size); }
	//printf("basecomm rank/size : %d/%d \n",basecomm_rank+1,basecomm_size); 
	//sleep(1); printf("basecomm rank/size : %d/%d\n",basecomm_rank+1,basecomm_size);

	// looping subcomms
	
	sleep(1);

	for(int i=0;i<Nsubcomm;i++)
	{
		if ( subcomm_color == i )
		{	
			if( subcomm_rank == 0 )
			{	printf("# this is group: %d\n",subcomm_color);
			}
	
			MPI_Barrier(SubComm);

			for(int j=0;j<subcomm_size;j++)
			{
				if( subcomm_rank == j )
					printf("Hi, I am the group memeber %d\n",subcomm_rank);
			}
		}
		sleep(1);
	}
	

	// get number of groups



	// Summary

	/*
	 1)	MPI_Comm_rank/size();
		BaseComm: basecomm_rank / bascomm_size

		# define SubComm
		MPI_Comm_split(BaseComm, basecomm_size / nprocs_per_workgroup, basecomm_rank, &SubComm);
		
			each proc will see their 'color' (basecomm_size/NPW) and 'itself' (basecomm_rank) -> committed as 'SubComm'

	 2)	MPI_Comm_rank/size();
		SubComm : subcomm_rank  / subcomm_size
	*/


	#endif

	MPI_Comm_free(&SubComm);
	MPI_Finalize();

	return 0;
}
