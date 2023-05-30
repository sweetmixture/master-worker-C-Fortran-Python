/*
        Author:         Woongkyu Jee / woong.jee.16@ucl.ac.uk
        Affiliation:    University College London
        Date:           2023.05.25 - 

        Description:
*/


#include <stdio.h>
#include <mpi.h>

//#include "subprogram.h"

void unittest_tf_config_workgroup_array( MPI_Comm* comm, const int n_comm, const int comm_tag )
{
        int size,rank;
        MPI_Comm_size(*comm,&size);
        MPI_Comm_rank(*comm,&rank);

        for(int i=0;i<n_comm;i++){
                if( comm_tag == i ){
                        for(int j=0;j<size;j++){
                                if( rank == j ){
                                        printf("[ comm_tag, comm_size, cpu_rank ] : [ %d, %d, %d ]\n", \
                                                comm_tag, size ,rank );
                                }
                        }
                }
        }

        if( comm_tag == n_comm - 1 ){
                if( rank == 0 ){
                        printf(" total  comm count: %d\n",n_comm);
                        printf(" master comm tag  : %d\n",comm_tag);
                }
        }

        return;
}

