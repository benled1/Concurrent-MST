#!/bin/bash
#
#SBATCH --cpus-per-task=8
#SBATCH --time=02:00
#SBATCH --mem=1G
#SBATCH --partition=slow
#SBATCH --output=test_serial.txt

srun ./mst_serial inputGraphs/50k-60k.csv true