#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#include <sys/time.h>
#include <time.h>

double get_time() {

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

void getCurrentDateTime(char *dateTimeStr) {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	time_t seconds = ts.tv_sec;
	long int nanoseconds = ts.tv_nsec;
	int year, month, day, hour, minute, second, microsecond;
	struct tm* tm = localtime(&seconds);
	year = tm->tm_year + 1900;
	month = tm->tm_mon + 1;
	day = tm->tm_mday;
	hour = tm->tm_hour;
	minute = tm->tm_min;
	second = tm->tm_sec;
	microsecond = nanoseconds / 1000;
	sprintf(dateTimeStr, "%04d-%02d-%02d %02d:%02d:%02d.%06d", year, month, day, hour, minute, second, microsecond);
}

int main(int argc, char *argv[]) {

	int rank, size, i, N, N_local, N_inside = 0, N_total_inside;
	double x, y, pi_local, pi_total, t_start, t_end;
	char start_timestamp[20], end_timestamp[20];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	const int iter_max = 16;
	int iter;
	double* data = (double*)malloc(iter_max*sizeof(double));

	N = 100000000;  // Number of points to generate
	//N = 999999999;  // Number of points to generate
	N_local = N / size;  // Number of points to generate per process
	//Seed the random number generator based on the rank
	srand(rank + 1 + atoi(argv[1]));
	
	t_start = get_time();
	getCurrentDateTime(start_timestamp);

	if( rank == 0 ){
		printf("pid: %s ntasks: %d rank: %d start: %s \n",argv[1], size,rank,start_timestamp);
	}

	for(iter = 0; iter < iter_max; iter++ ){

		// Generate N_local random points and count how many fall inside the circle
		for (i = 0; i < N_local; i++) {

			x = (double)rand() / RAND_MAX;
			y = (double)rand() / RAND_MAX;

			if (sqrt(x*x + y*y) <= 1.0) {
				N_inside++;
			}
		}

		// Sum up the number of points inside the circle across all processes
		MPI_Reduce(&N_inside, &N_total_inside, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

		// Calculate the local estimate of pi and sum it up across all processes
		pi_local = 4.0 * (double)N_inside / (double)N_local;
		MPI_Reduce(&pi_local, &pi_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

		data[iter] = pi_total;
		N_inside = 0;
	}
	
	double res = 0.;
	for(iter = 0; iter < iter_max ; iter++){
		res += data[iter];
	}
	pi_total = res / (double)iter_max;

	t_end = get_time();
	getCurrentDateTime(end_timestamp);
	if( rank == 0)
		printf("pid: %s ntasks: %d rank: %d start/end: %s/%s elapsedd_t: %.3f \n",argv[1],size,rank,start_timestamp,end_timestamp,t_end - t_start);

	// Output the final estimate of pi from the root process
	if (rank == 0) {
		pi_total /= size;
		printf("Approximate value of pi: %.10f / ", pi_total);
		printf("Elapsed time: %.3f seconds\n", t_end - t_start);
	}

	free(data);

	MPI_Finalize();
	return 0;
}
