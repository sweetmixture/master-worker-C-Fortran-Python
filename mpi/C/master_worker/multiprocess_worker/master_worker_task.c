#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "taskfarm_def.h"	// WorkgroupConfig (struct)
#include "master_worker_task.h"	// function_task (struct) result_pacakge (struct)

#include "subprogram.h"		// subprogram_pi( MPI_Comm* workgroup_comm, int rand_seed )

// set_task()
// get_next_task()
// get_task_result()
// logger

function_task* get_next_task( function_task* task_array, const int task_count, int* sent_task_count )
{
	//printf("task_count / sent_task_count : %d / %d\n",task_count,*sent_task_count);
	if( *sent_task_count >= task_count ) return NULL;

	function_task* task = &task_array[*sent_task_count];
	(*sent_task_count)++;

	return task;
}


void master_worker_task_call_master( const MPI_Comm* base_comm, const WorkgroupConfig* wc, const int n_workgroup, const int task_count )
{
	const int master_tag = n_workgroup - 1;
 	
	/* access to a head (base_rank) of each workgroup: wc[i].base_rank / i: 0 - master_tag  */

	// initilise tasks
	int sent_task_count = 0;
	function_task* task_array = malloc(task_count*sizeof(function_task));
	for(int i=0;i<task_count;i++){
		task_array[i].fp = subprogram_pi;
		task_array[i].task_id = i;
		task_array[i].task_status = TASK_INIT;
	}

	// messaging tasks
	MPI_Status status;
	MPI_Request request;
	function_task* task;
	result_package res;
	
	// Initial task messaging 
	for(int n=0;n<master_tag;n++){
		task = get_next_task(task_array,task_count,&sent_task_count);

		if( task == NULL ){ break; }
		//printf("send_count / task fp / id / status : %d %x %d %d\n",sent_task_count,task->fp,task->task_id,task->task_status);
		MPI_Isend(task,sizeof(function_task),MPI_CHAR,wc[n].base_rank,TF_WORKTAG,*base_comm,&request);
		MPI_Wait(&request,&status);

		printf("MASTER> MPI_Isend complete: master -> %d (base-rank) with [ tag, size ] = [ %d, %d ]\n",wc[n].base_rank,wc[n].workgroup_tag,wc[n].workgroup_size);
	}

	task = get_next_task(task_array,task_count,&sent_task_count);

	while( task != NULL ){

		MPI_Recv(&res,sizeof(result_package),MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,*base_comm,&status);
		printf("MASTER> MPI_Recv complete: from %d\n",status.MPI_SOURCE);


		// logging 'res'

		MPI_Send(task,sizeof(function_task),MPI_CHAR,status.MPI_SOURCE,TF_WORKTAG,*base_comm);	// using ... MPI handle ... MPI_Status stauts -> MPI_SOURCE
		printf("MASTER> MPI_Send complete: master -> %d (base-rank)\n",status.MPI_SOURCE);
		task = get_next_task(task_array,task_count,&sent_task_count);
	}

	// Termination message
	for(int n=0;n<master_tag;n++){
			MPI_Send(0,0,MPI_CHAR,wc[n].base_rank,TF_DIETAG,*base_comm);
			printf("MASTER - DIETAG > MPI_Send complete: master -> %d (base-rank)\n",wc[n].base_rank);
	}

	free(task_array);

	return;
}

// ----------------------------------------------------------------------------------------------------------------------

void master_worker_task_call_workgroup( const MPI_Comm* base_comm, const MPI_Comm* workgroup_comm, const int n_workgroup, const int workgroup_tag )
{
	int brank,bsize;
	int workgroup_size,worker_rank;

	MPI_Comm_size(*base_comm,&bsize);
	MPI_Comm_rank(*base_comm,&brank);
	
	MPI_Comm_size(*workgroup_comm,&workgroup_size);
	MPI_Comm_rank(*workgroup_comm,&worker_rank);

	const int workgroup_count = n_workgroup - 1;
	const int master_base_rank = bsize - 1;

	MPI_Status status;
	MPI_Request request;

	// recv buffer
	function_task task;
	// result
	result_package res;

	for(;;){

		for(int n=0;n<workgroup_count;n++){

			// task recv - head process of each workgroup
			if( n == workgroup_tag && worker_rank == 0 ){
				MPI_Irecv(&task,sizeof(function_task),MPI_CHAR,master_base_rank,TF_WORKTAG,*base_comm,&request);
				MPI_Wait(&request,&status);
				//printf("WORKGROUP [%d] > MPI_Irecv complete : task %x %d %d\n",workgroup_tag,task.fp,task.task_id,task.task_status);
			}
			
			// workgroup interanl bcast - task
			if( n == workgroup_tag ){
			
				MPI_Bcast(&task,sizeof(function_task),MPI_CHAR,0,*workgroup_comm);
				//printf("WORKGROUP [%d] > MPI_Bcast complete : worker_rank [%d] task %x %d %d\n",workgroup_tag,worker_rank,task.fp,task.task_id,task.task_status);
		
				if( status.MPI_TAG == TF_DIETAG ){ return; }
		
				//execution
				task.task_status = TASK_EXECUTED;
				task.fp(workgroup_comm,task.task_id);
				task.task_status = TASK_FINISHED;

				// setup result;
				res.task_status = TASK_FINISHED;

				if( worker_rank == 0 ){
					MPI_Send(&res,sizeof(result_package),MPI_CHAR,master_base_rank,res.task_status,*base_comm);
				}
			}
		}
	}

	return;
}
