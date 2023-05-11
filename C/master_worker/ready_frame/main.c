#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define WORKTAG 1
#define DIETAG 2

void master();
void worker();

int main(int argc, char *argv[])
{
	int myrank;
	MPI_Init(&argc, &argv); /* initialize MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank); /* process rank, 0 to N-1 */

	if (myrank == 0)	master();
	else 			worker();

	MPI_Finalize(); /* cleanup MPI */
	return 0;
}

///

/* __JOB__ : keeping dummy data */
typedef struct work_s
{
	int pos;	// job_id
	double x[2];
	double y;
} work_t;

/* __GLOBAL__ : managing jobs */
work_t *workarray;	// job array
int work_id;		// number of passed (to worker) jobs
int nworks;		// total number of initial jobs


void init_work(int n)	// called within 'master()', 'n' -> number of jobs
{
	int i;

	// three variables below are __GLOBAL__: 'work_array' keeping a certain number of jobs in 'canonical form'
	workarray = malloc(n*sizeof(work_t));	// for 'n' number of jobs, wrap them up as 'work_t'
	work_id = 0;	// number of jobs done
	nworks = n;	// 'n' number of jobs

	// put dummy data to each job
	for (i = 0; i < n; i++)
	{
		workarray[i].pos = i;		// set 'i' -> job_tag

		// setting dummy data
		workarray[i].x[0] = drand48();
		workarray[i].x[1] = drand48();
	}
}

///
work_t *get_next_work_request()
{
	if (work_id >= nworks) return NULL;	// work_id >= nworks : all jobs are done !

	work_t *w = &workarray[work_id];	// 
	work_id++;				// increase value for the number of finished or passed, by message jobs to workers
	return w;				// return job in package: 'work_t'
}

void print_work()
{
	int i;
	int n = nworks;
	for (i = 0; i < n; i++)
	{
		printf("work[%d]: %f %f -> %f\n", i, workarray[i].x[0],
		workarray[i].x[1], workarray[i].y);
	}
}

///
/* __JOB_RESULT__ */
typedef struct result_s
{	int pos;	// 'pos' -> job_id taken from the 'job' (work_t)
	double y;	// 'y'   -> information tkaem from the 'job' (work_t)
} result_t;		// return pacakge, 'result_t' - finished job

void worker()
{
	int rank;
	result_t result;
	work_t work;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	// 'rank' of this worker

	// keep receiving mesages until all jobs finished
	for (;;)
	{	
		/*
			1. Recv a job - job message sent from 'master()'
			
			2. 
		*/
		MPI_Recv(&work, sizeof(work_t), MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);	// Blocking -> reason explicit !

		/* Check the tag of the received message */
		if (status.MPI_TAG == DIETAG)
		{	return;
		}

		result.pos = work.pos;
		result.y = work.x[0] + work.x[1];
		sleep(1);
		printf("result of work %d on process [ %d ] : %f\n", result.pos, rank, result.y);

		MPI_Send(&result, sizeof(result_t), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
}

///
void master()
{
	int ntasks, rank;
	double result;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks); /* #processes in application */

	init_work(64);	// number of works
	/*
	 *  * Seed the workers.
	 *   */
	work_t *work;

	for (rank = 1; rank < ntasks; ++rank)
	{
		work = get_next_work_request();
		printf("sending work %d\n", work->pos);

		MPI_Send(work, /* message buffer */
		sizeof(work_t), /* one data item */
		MPI_CHAR, /* data item is a struct */
		rank, /* destination process rank */
		WORKTAG, /* user chosen message tag */
		MPI_COMM_WORLD); /* always use this */
	}

	/*
	 *  * Receive a result from any worker and dispatch a new work
	 *   * until work requests have been exhausted.
	 *    */
	result_t res;
	work = get_next_work_request();

	while (work != NULL)
	{
		MPI_Recv(&res, /* message buffer */
		sizeof(result_t), /* one data item .. */
		MPI_CHAR, /* of a struct */
		MPI_ANY_SOURCE, /* receive from any sender */
		MPI_ANY_TAG, /* any type of message */
		MPI_COMM_WORLD, /* always use this */
		&status); /* received message info */

		workarray[res.pos].y = res.y;
		printf("sending work %d\n", work->pos);
		MPI_Send(work, sizeof(work_t), MPI_CHAR, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);
		work = get_next_work_request();
	}

	/*
	 *  * Receive results for pending work requests.
	 *   */
	for (rank = 1; rank < ntasks; ++rank)
	{
		MPI_Recv(&res,sizeof(result_t),MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		workarray[res.pos].y = res.y;
	}
	/*
	 *  * Tell all the workers to exit.
	 *   */
	for (rank = 1; rank < ntasks; ++rank)
	{
		MPI_Send(0, 0, MPI_CHAR, rank, DIETAG, MPI_COMM_WORLD);
	}

	/* Print the results */
	print_work();
}
