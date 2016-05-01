#!/bin/bash
#SBATCH -n 25
#SBATCH --ntasks-per-node=1
#SBATCH --time=02:00:00

srun bash run.sh
