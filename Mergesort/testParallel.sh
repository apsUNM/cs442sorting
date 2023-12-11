#!/bin/bash
#SBATCH --job-name test_PMerge
#SBATCH --mail-user aps@unm.edu
#SBATCH --output results.out
#SBATCH --error errors.err
#SBATCH --time 00:10:00
#SBATCH --ntasks 8
#SBATCH --ntasks-per-node 8
#SBATCH --partition debug

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NODE(S) / TASK(S) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo Node list: $SLURM_JOB_NODELIST
echo Number of tasks: $SLURM_NTASKS
echo
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% testParallel %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

module load gcc/10.2.0-3kjq     
module load slurm/20.02.05-hfsl
module load cuda/11.4.2-rgi3    
module load openmpi/4.1.4-7gqe
module load cmake/3.26.3-vyco

mpicxx -o testParallel.exe testMerge.cpp
#mpirun -np 2 ./testParallel.exe -n 10 -s 20 -t "parallel"
mpiexec -np 8 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 50 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"