#!/bin/bash
#
#SBATCH --cpus-per-task=8
#SBATCH --time=02:00
#SBATCH --mem=1G
#SBATCH --partition=slow
#SBATCH --output=test_thread.txt

srun ./mst_thread inputGraphs/50k-60k.csv 4 true
