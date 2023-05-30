#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

#define SEED 12345678

int main(int argc, char** argv) {
    int rank, size, i;
    long long int total_points = 1000000000; // Total number of points
    long long int points_in_circle = 0; // Points inside the circle

    double x, y, distance, pi_estimate, start_time, end_time;
    struct timeval start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    gettimeofday(&start, NULL);
    start_time = start.tv_sec + start.tv_usec / 1000000.0;

    srand(SEED + rank); // Seed the random number generator

    // Each process performs a subset of the iterations
    for (i = 0; i < total_points / size; i++) {
        // Generate random (x, y) coordinates
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        // Calculate the distance from the origin
        distance = x * x + y * y;

        // Check if the point is inside the circle
        if (distance <= 1) {
            points_in_circle++;
        }
    }

    // Sum up the number of points inside the circle from all processes
    long long int global_points_in_circle;
    MPI_Reduce(&points_in_circle, &global_points_in_circle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Calculate the value of Pi
    if (rank == 0) {
        pi_estimate = 4.0 * global_points_in_circle / total_points;
        printf("Estimated Pi: %f\n", pi_estimate);

        gettimeofday(&end, NULL);
        end_time = end.tv_sec + end.tv_usec / 1000000.0;
        printf("Execution Time: %.6f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}

