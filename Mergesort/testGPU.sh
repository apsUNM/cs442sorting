#!/bin/bash
#SBATCH --job-name GPU_MatrixMerge
#SBATCH --mail-user aps@unm.edu
#SBATCH --output GPUresults.out
#SBATCH --error GPUerrors.err
#SBATCH --time 00:10:00
#SBATCH --ntasks 1
#SBATCH --ntasks-per-node 1
#SBATCH --partition bigmem-1TB

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% testGPU %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

module load gcc/10.2.0-3kjq     
module load slurm/20.02.05-hfsl
module load cuda/11.4.2-rgi3    
module load openmpi/4.1.4-7gqe
module load cmake/3.26.3-vyco

nvcc -arch=sm_35 -o testGPU.exe testMerge.cpp
#mpicxx -o testGPU.exe testMerge.cpp
mpiexec -np 1 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testGPU.exe -n 20 -s 20 -t "matrixSerial"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"