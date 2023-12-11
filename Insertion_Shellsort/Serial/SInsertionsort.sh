#!/bin/bash
#SBATCH --job-name SInsertionsort
#SBATCH --mail-user aps@unm.edu
#SBATCH --output SInsertionsort.out
#SBATCH --error SInsertionsort.err
#SBATCH --time 00:01:00
#SBATCH --ntasks 1
#SBATCH --ntasks-per-node 8
#SBATCH --partition debug

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% INSERTION SORT %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

module load gcc
module load openmpi

mpicc -o SInsertionsort.exe SInsertionsort.c
mpirun -np 1 ./SInsertionsort.exe 5 5

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo