#ifndef MASTER_WORKER_TASK
#define MASTER_WORKER_TASK

#include <mpi.h>

#define TASK_INIT     55
#define TASK_EXECUTED 56
#define TASK_FINISHED 57

#define TASK_DIETAG  666

typedef void (*taskfunction_ptr)( const MPI_Comm* , int );

typedef struct function_task_{

	taskfunction_ptr fp;
	int task_id;
	int task_status;

}function_task;

typedef struct result_package_{

	int task_status;
	int task_id;

	char start_t[40];
	char end_t[40];

	double elapsed_t;
	double value;

}result_package;


void master_worker_task_call_master( const MPI_Comm* base_comm, const WorkgroupConfig* wc, const int n_workgroup, const int task_count );

void master_worker_task_call_workgroup( const MPI_Comm* base_comm, const MPI_Comm* workgroup_comm, const int n_workgroup, const int workgroup_tag );


#endif
