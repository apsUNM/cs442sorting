#!/bin/bash
#SBATCH --job-name CudaCannon
#SBATCH --mail-user aps@unm.edu
#SBATCH --output cannon.out
#SBATCH --error cannon.err
#SBATCH --time 00:10:00
#SBATCH --ntasks 1
#SBATCH --ntasks-per-node 8
#SBATCH --gpus-per-node=1
#SBATCH --partition debug

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% timing_cannon %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"



module load gcc/10.2.0-3kjq     
module load slurm/20.02.05-hfsl
module load cuda/11.4.2-rgi3    
module load openmpi/4.1.4-7gqe
module load cmake/3.26.3-vyco

nvcc -arch=sm_35 -o timing_cannon.exe timing_cannon.cu
mpirun -np 4 --oversubscribe ./timing_cannon.exe


echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
