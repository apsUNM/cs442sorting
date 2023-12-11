#!/bin/bash
#SBATCH --job-name test_MatrixSMerge
#SBATCH --mail-user aps@unm.edu
#SBATCH --output Matrixresults.out
#SBATCH --error Matrixerrors.err
#SBATCH --time 01:00:00
#SBATCH --ntasks 1
#SBATCH --ntasks-per-node 1
#SBATCH --partition singleGPU

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% testMatrixSerial %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

module load gcc/10.2.0-3kjq     
module load slurm/20.02.05-hfsl
module load cuda/11.4.2-rgi3    
module load openmpi/4.1.4-7gqe
module load cmake/3.26.3-vyco

mpicxx -o testMatrixSerial.exe testMerge.cpp
mpiexec -np 1 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testMatrixSerial.exe -n 1000 -s 20 -t "matrixSerial"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"