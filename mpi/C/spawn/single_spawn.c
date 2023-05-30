#include <stdio.h>
#include <mpi.h>

/*
MPI_Comm_spawn_multi(
	char*	 command,
	char*	 command_argv[],
	int	 maxprocs,
	MPI_Info info,
	int	 root,
	MPI_Comm comm,
	MPI_Comm *intercomm,
	int	 array_of_errcodes[]
	)

commands		: Program
argv			: Program args
info			: How and where to create processes
root			: Root rank, creating processes
comm			: Communicator holding processes
intercomm		: Intercommunicator
array_of_errcodes	: Error codes for each processes


e.g.,

char* command;
char** argv;

command = "./toy_model.x";
argv = (char**)malloc(3*sizeof(char*));
argv[3] = {"arg1", "arg2", "arg3"};

MPI_Comm_spawn(command, argv, MAX_PROC, ,...);

*/


