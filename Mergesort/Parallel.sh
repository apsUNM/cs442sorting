#!/bin/bash
#SBATCH --job-name test_PMerge
#SBATCH --mail-user aps@unm.edu
#SBATCH --output Parallelresults.out
#SBATCH --error Perallelerrors.err
#SBATCH --time 00:30:00
#SBATCH --ntasks 64
#SBATCH --ntasks-per-node 16
#SBATCH --partition singleGPU

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
mpiexec -np 1 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 1000 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Parallel 2  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

mpiexec -np 2 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 1000 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Parallel 4  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

mpiexec -np 4 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 1000 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Parallel 8  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

mpiexec -np 8 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 1000 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Parallel 16  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

mpiexec -np 16 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 1000 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Parallel 32  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

mpiexec -np 32 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 1000 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Parallel 64  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

mpiexec -np 64 --mca orte_base_help_aggregate 0 --mca btl_openib_allow_ib true ./testParallel.exe -n 1000 -s 20 -t "parallel"

echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DONE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"