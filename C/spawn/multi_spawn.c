#include <stdio.h>
#include <mpi.h>

/*
MPI_Comm_spawn_multi(
	int	count,				
	char*	array_of_commands[],
	char**	array_of_command_argv[],
	int	array_of_maxprocs[],
	MPI_Info array_of_info[],
	int	root,
	MPI_Comm comm,
	MPI_Comm *intercomm,
	int	array_of_errcodes[]
	)

count			: Number of commands
array_of_commands	: Program
array_of_argv		: Program args
array_of_info		: How and where to create processes
root			: Root rank, creating processes
comm			: Communicator holding processes
intercomm		: Intercommunicator
array_of_errcodes	: Error codes for each processes
*/


