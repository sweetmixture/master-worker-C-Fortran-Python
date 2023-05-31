#!/bin/bash -l

# Batch script to run an MPI parallel job under SGE with Intel MPI.

# Request ten minutes of wallclock time (format hours:minutes:seconds).
#$ -l h_rt=00:20:00

# Request 1 gigabyte of RAM per process (must be an integer)
#$ -l mem=1G

# Set the name of the job.
#$ -N taskfarm_1

# Select the MPI parallel environment and 16 processes.
#$ -pe mpi 40

# Set the working directory to somewhere in your scratch space.
# Replace "<your_UCL_id>" with your UCL user ID :
#$ -cwd

#$ -P Gold
#$ -A UCL_chemM_Woodley

export OMP_NUM_THREADS=1
# Run our MPI job.  GERun is a wrapper that launches MPI jobs on our clusters.
gerun /home/uccawkj/Software/master-worker-C-Fortran-Python/mpi/C/master_worker/multiprocess_worker/build/tf.x > log.txt
