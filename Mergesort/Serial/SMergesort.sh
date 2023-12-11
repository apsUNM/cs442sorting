#!/bin/bash
#SBATCH --job-name P_Mergesort
#SBATCH --mail-user aps@unm.edu
#SBATCH --output SMergesort.out
#SBATCH --error SMergesort.err
#SBATCH --time 00:01:00
#SBATCH --ntasks 1
#SBATCH --ntasks-per-node 8
#SBATCH --partition debug

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% SMergesort %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

module load gcc
module load openmpi

mpicc -o SMergesort.exe SMergesort.c
mpirun -np 16 ./SMergesort.exe 5 5

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"