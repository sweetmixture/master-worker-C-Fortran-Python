#!/bin/bash -l

# Batch script to run an MPI parallel job under SGE with Intel MPI.

# Request ten minutes of wallclock time (format hours:minutes:seconds).
#$ -l h_rt=00:10:00

# Request 1 gigabyte of RAM per process (must be an integer)
#$ -l mem=2G

# Set the name of the job.
#$ -N PythonMP_test

# Select the MPI parallel environment and 16 processes.
#$ -pe mpi 80

# Set the working directory to somewhere in your scratch space.
# Replace "<your_UCL_id>" with your UCL user ID :
#$ -cwd

#$ -P Test
#$ -A UCL_chemM_Woodley

# Run our MPI job.  GERun is a wrapper that launches MPI jobs on our clusters.
python /home/uccawkj/Software/Python_Mult/multiNode_2/program.py
