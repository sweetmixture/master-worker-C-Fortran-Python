/*
	Author:		Woongkyu Jee / woong.jee.16@ucl.ac.uk
	Affiliation:	University College London
	Date:		2023.05.25 - 

	Description:
*/

#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

#include "taskfarm_def.h"
#include "subprogram.h"

#include "unit_test.h"

// ----------------------------------------------------- master -> workgroup
typedef void (*taskfunction_ptr)( MPI_Comm* , int );

typedef struct function_task_{
	taskfunction_ptr fp;
	int task_status;	// done ? 0 false - 1 true
}function_task;
// -----------------------------------------------------


// master <-> workgroup
typedef struct result_package_{

	char start_t[20];
	char end_t[20];
	double elapsed_t;
	double value;

}result_package;

void master( MPI_Comm* BaseComm, MPI_Comm* WorkgroupComm, const WorkgroupConfig* workgroup_config, const int n_workgroup )
{
	int brank,bsize;
	int wrank,wsize;

	MPI_Comm_size(*BaseComm,&bsize);
	MPI_Comm_rank(*BaseComm,&brank);
	MPI_Comm_size(*WorkgroupComm,&wsize);
	MPI_Comm_rank(*WorkgroupComm,&wrank);

	// Setting tasks ----------------------------
	int task_count = 10;
	int finished_task_count = 0;

	function_task* work_array = malloc(task_count*sizeof(function_task));

	for(int i=0;i<task_count;i++){
		work_array[i].fp = subprogram_pi;	// function pointer setting or task setting
		work_array[i].task_status = 123;
	}
	//work_array[0].fp(WorkgroupComm,123); 	// execution ...

	// send work to 0 workgroup - using baserank of the head of workgroup 0 -> workgroup_config[0].base_rank
	MPI_Send(&work_array[0],sizeof(function_task),MPI_CHAR,workgroup_config[0].base_rank,TF_WORKTAG,*BaseComm);
	//printf("send message to: %d tag: %d\n",workgroup_config[0].base_rank,TF_WORKTAG);
	printf("MASTER> SEND brank: %d to workgroup %d - brank %d \n",workgroup_config[n_workgroup-1].base_rank, 0, workgroup_config[0].base_rank);
	printf("MASTER> functionptr %x\n",work_array[0].fp);

	MPI_Send(&work_array[1],sizeof(function_task),MPI_CHAR,workgroup_config[1].base_rank,TF_WORKTAG,*BaseComm);
	printf("MASTER> SEND brank: %d to workgroup %d - brank %d \n",workgroup_config[n_workgroup-1].base_rank, 0, workgroup_config[1].base_rank);
	printf("MASTER> functionptr %x\n",work_array[1].fp);

	printf("wgroup 3 config - tag size rank %d %d %d\n",workgroup_config[3].workgroup_tag,workgroup_config[3].workgroup_size,workgroup_config[3].base_rank);

	MPI_Send(&work_array[2],sizeof(function_task),MPI_CHAR,workgroup_config[3].base_rank,TF_WORKTAG,*BaseComm);
	printf("MASTER> SEND brank: %d to workgroup %d - brank %d \n",workgroup_config[n_workgroup-1].base_rank, 0, workgroup_config[3].base_rank);
	printf("MASTER> functionptr %x\n",work_array[2].fp);

	free(work_array);
	return;
}

int main(int argc, char* argv[])
{
	int brank,bsize;

	MPI_Comm BaseComm;
	BaseComm = MPI_COMM_WORLD;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(BaseComm,&bsize);
	MPI_Comm_rank(BaseComm,&brank);

	// ---------------------------- BaseComm configuration done

	MPI_Comm WorkgroupComm;
	int n_workgroup;		// number of workgroups
	int workgroup_tag;		// workgroup tag
	int workgroup_size;		// workgroup size
	int worker_rank;		// internal worker(cpu) rank within a workgroup
	
	int mastergroup_brank;

	n_workgroup = tf_config_workgroup(&BaseComm, &WorkgroupComm, &workgroup_tag, &workgroup_size, &worker_rank, TF_CPUS_PER_WORKER);
	mastergroup_brank  = bsize - 1;

	//unittest_tf_config_workgroup_array(&WorkgroupComm, n_workgroup, workgroup_tag);	// unittest
	//exit(1);

	// workgroup config saver
	WorkgroupConfig workgroup_config[n_workgroup];
	tf_get_workgroup_config(&BaseComm,&WorkgroupComm,&workgroup_config[0],n_workgroup,workgroup_tag);	// make sure this is done

	// unittest 'tf_get_workgroup_config()'
	for(int i=0;i<n_workgroup;i++)
	{
		if( i == workgroup_tag ){
			if( worker_rank == 0 ){
				//printf("i %d workgroup_tag_1 %d workgroup_tag_2 %d worker_head-base_rank %d\n",i, workgroup_tag,workgroup_config[i].workgroup_tag,workgroup_config[i].base_rank);
				//printf("workgroup_tag %d worker_head-base_rank %d\n",workgroup_config[i].workgroup_tag,workgroup_config[i].base_rank);
			}
		}
	}
	//exit(1);

	// master section
	if ( brank == bsize - 1 ){ master(&BaseComm,&WorkgroupComm,workgroup_config,n_workgroup); }
	//exit(1);


	// worker section .... 
	function_task ft;	// recv buffer
	MPI_Status status;

	if ( workgroup_tag == 0 && worker_rank == 0 ){

		printf("!!! workgroup_tag %d, worker_rank %d, worker_brank %d - tag %d \n", workgroup_tag, worker_rank, workgroup_config[workgroup_tag].base_rank,workgroup_config[workgroup_tag].workgroup_tag);
		MPI_Recv(&ft,sizeof(function_task),MPI_CHAR,bsize-1,TF_WORKTAG,BaseComm,&status);
		printf("from head workgroup[0] - task_status: %d %x\n",ft.task_status,ft.fp);
	}

	//	
	if( workgroup_tag == 0 ){
	
		MPI_Bcast(&ft,sizeof(function_task),MPI_CHAR,0,WorkgroupComm);

		for(int i=0;i<workgroup_size;i++){
			if( worker_rank == i ){
				printf("wg 0 bcast message %d (%x) from internal rank %d / %d\n",ft.task_status,ft.fp,worker_rank,workgroup_size);
			}
		}

		// execution within workgroup - 0
		if( worker_rank == 0 ){	printf("workgroup 0 start! with message %d \n",ft.task_status); }
		ft.fp(&WorkgroupComm,ft.task_status);
	}

	// worker group 1
	if ( workgroup_tag == 1 && worker_rank == 0 ){

		printf("!!! workgroup_tag %d, worker_rank %d, worker_brank %d - tag %d \n", workgroup_tag, worker_rank, workgroup_config[workgroup_tag].base_rank,workgroup_config[workgroup_tag].workgroup_tag);
		MPI_Recv(&ft,sizeof(function_task),MPI_CHAR,bsize-1,TF_WORKTAG,BaseComm,&status);
		ft.task_status += 100;
		printf("from head workgroup[1] - task_status: %d %x\n",ft.task_status,ft.fp);
	}

	if ( workgroup_tag == 1 ){
		
		//MPI_Bcast(&ft,sizeof(function_task),MPI_CHAR,workgroup_config[1].base_rank,WorkgroupComm);
		MPI_Bcast(&ft,sizeof(function_task),MPI_CHAR,0,WorkgroupComm);

		for(int i=0;i<workgroup_size;i++){
			if( worker_rank == i ){
				printf("wg 1 bcast message %d (%x) from internal rank %d / %d\n",ft.task_status,ft.fp,worker_rank,workgroup_size);
			}
		}

		// execution within workgroup - 1
		if ( worker_rank ==  0 ){ printf("workgroup 1 start! with message %d \n",ft.task_status); }
		ft.fp(&WorkgroupComm,ft.task_status);
	}

	// worker group 3
	if ( workgroup_tag == 3 && worker_rank == 0 ){

		printf("!!! workgroup_tag %d, worker_rank %d, worker_brank %d - tag %d \n", workgroup_tag, worker_rank, workgroup_config[workgroup_tag].base_rank,workgroup_config[workgroup_tag].workgroup_tag);
		MPI_Recv(&ft,sizeof(function_task),MPI_CHAR,bsize-1,TF_WORKTAG,BaseComm,&status);
		ft.task_status += 200;
		printf("from head workgroup[1] - task_status: %d %x\n",ft.task_status,ft.fp);
	}

	if ( workgroup_tag == 3 ){
		
		MPI_Bcast(&ft,sizeof(function_task),MPI_CHAR,0,WorkgroupComm);

		for(int i=0;i<workgroup_size;i++){
			if( worker_rank == i ){
				printf("wg 3 bcast message %d (%x) from internal rank %d / %d\n",ft.task_status,ft.fp,worker_rank,workgroup_size);
			}
		}
	
		if( worker_rank == 0 ){ printf("workgropu 3 start! with message %d\n",ft.task_status); }
		ft.fp(&WorkgroupComm,ft.task_status);
	}








	// want to get tag = 0 base rank
/*
	for(int i=0;i<n_workgroup;i++){
		if( workgroup_tag == i && worker_rank == 0 ){
				printf("workgroup %d worker_rank %d -> base_rank %d\n",workgroup_tag,worker_rank,workgroup_config[i].base_rank);
		}
	}
*/

	// launcher test
	// on workgroup_tag == 0
	/*
	if( workgroup_tag == 0 ){
		printf("worker_rank / workgroup_size: %d / %d\n",worker_rank,workgroup_size);
		// lauch?
		subprogram_pi(&WorkgroupComm, worker_rank);
	}
	*/

	MPI_Finalize();
	return 0;
}


