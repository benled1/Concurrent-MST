#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --partition=slow
#SBATCH --mem=1G

srun ./mst_mpi /home/bledingh/CMP431-slurm/final_project/CMPT431-Project/inputGraphs/connected_graph.csv true