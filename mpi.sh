#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --partition=slow
#SBATCH --mem=1G
#SBATCH --output=test_mpi.txt

srun ./mst_mpi inputGraphs/50k-60k.csv true