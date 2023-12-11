#!/bin/bash
#SBATCH --job-name Parallel_Insertionsort
#SBATCH --mail-user aps@unm.edu
#SBATCH --output PInsertionsort.out
#SBATCH --error PInsertionsort.err
#SBATCH --time 00:01:00
#SBATCH --ntasks 1
#SBATCH --ntasks-per-node 8
#SBATCH --partition debug

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% PInsertion Sort %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

module load gcc
module load openmpi

mpicc -o PInsertionsort.exe PInsertionsort.c
mpirun -np 16 ./PInsertionsort.exe 10 25

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"