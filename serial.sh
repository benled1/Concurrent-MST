#!/bin/bash
#
#SBATCH --cpus-per-task=8
#SBATCH --time=02:00
#SBATCH --mem=1G
#SBATCH --partition=slow

srun ./mst_serial /home/bledingh/CMP431-slurm/final_project/CMPT431-Project/inputGraphs/testData.csv true