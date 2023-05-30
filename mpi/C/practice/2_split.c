#include <stdio.h>
#include <mpi.h>

#define N_SUB_GROUPS 4


int main(int argc, char* argv[])
{
	int base_size,base_rank;
	int subg_size,subg_rank;

	MPI_Comm BaseComm, SubComm;
	BaseComm = MPI_COMM_WORLD;

	// Init MPI Env
	MPI_Init(&argc,&argv);
	// Get BaseComm info
	MPI_Comm_size(BaseComm,&base_size);
	MPI_Comm_rank(BaseComm,&base_rank);

	// Set SubComm group tags and number of subgroups
	int subg_tag = base_rank / N_SUB_GROUPS;
	int n_subg = base_size / N_SUB_GROUPS;
	if ( base_size / N_SUB_GROUPS != 0 ){ n_subg++; }

	// BaseComm split -> SubComm
	MPI_Comm_split(BaseComm,subg_tag,base_rank,&SubComm);
	// Get SubComm info
	MPI_Comm_size(SubComm,&subg_size);
	MPI_Comm_rank(SubComm,&subg_rank);

	
	for(int i=0;i<n_subg;i++)	// Loop subgroups
	{
		if( i == subg_tag )
		{
			if( subg_rank == 0 )	// subgroup rank 
			{	printf("# this is subgroup: %d\n",subg_tag);
			}

			for(int j=0;j<subg_size;j++)	// Loop processors within a subgroup
			{
				if( subg_rank == j )
				{	printf("	this is rank: %d / %d from subgroup %d\n",subg_rank,subg_size,subg_tag);
				}
			}
		}
	}

	MPI_Comm_free(&SubComm);

	MPI_Finalize();

	return 0;
}
	
