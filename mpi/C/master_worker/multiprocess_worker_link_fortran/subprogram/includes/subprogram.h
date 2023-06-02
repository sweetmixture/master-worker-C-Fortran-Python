#ifndef _SUBPROGRAM
#define _SUBPROGRAM

#include <mpi.h>

void subprogram_pi( const MPI_Comm* comm, const int pid );


/* FORTRAN */

extern void fortran_subprogram_pi( const MPI_Comm* , int* );
/* subroutine fortran_subprogram_pi( comm, task_id ) bind(C,name="fortran_subprogram_pi")
 * integer(c_int), intent(inout) :: comm, task_id
 */



#endif
