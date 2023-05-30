/*
        Author:         Woongkyu Jee / woong.jee.16@ucl.ac.uk
        Affiliation:    University College London
        Date:           2023.05.25

        Description:
*/

#ifndef __TASKFARM_DEF
#define __TASKFARM_DEF

// Basic defines -------

#define _IN
#define _OUT
#define _IO

// ---------------------

#define TF_WORKTAG 1
#define TF_DIETAG  2

#define TF_CPUS_PER_WORKER 4

typedef struct WorkgroupConfig_{

	int base_size;
	int base_rank;
	int workgroup_tag;
	int workgroup_size;
	int worker_rank;

}WorkgroupConfig;
// 

int tf_config_workgroup( MPI_Comm* base_comm, MPI_Comm* workgroup_comm, int* workgroup_tag, int* workgroup_size, int* worker_rank, const int n_workers_per_workgroup );

void tf_get_workgroup_config( const MPI_Comm* base_comm, const MPI_Comm* workgroup_comm, WorkgroupConfig* workgroup_config, const int n_workgroup, const int workgroup_tag );


// general function pointer get library(?) function



#endif
