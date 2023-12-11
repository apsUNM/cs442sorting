#!/bin/bash
#SBATCH --job-name Serial_Quick
#SBATCH --mail-user aps@unm.edu
#SBATCH --output Serialresults.out
#SBATCH --error Serialerrors.err
#SBATCH --time 01:00:00
#SBATCH --ntasks 1
#SBATCH --ntasks-per-node 1
#SBATCH --partition singleGPU

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% testSerial %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

module load gcc/10.2.0-3kjq     
module load slurm/20.02.05-hfsl
module load cuda/11.4.2-rgi3    
module load openmpi/4.1.4-7gqe
module load cmake/3.26.3-vyco

mpicxx -o testSerial.exe testSort.cpp
mpirun -np 1 ./testSerial.exe -n 10 -s 20 -t "serial"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"