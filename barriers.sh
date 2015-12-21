#!/bin/bash

#SBATCH --time=00:30:00   # walltime
#SBATCH --ntasks=16 	# number of processor cores
#SBATCH --nodes=1   # number of nodes
#SBATCH --mem-per-cpu=1024M   # memory per CPU core
#SBATCH --qos=test

# Compatibility variables for PBS. Delete if not needed.
export PBS_NODEFILE=`/fslapps/fslutils/generate_pbs_nodefile`
export PBS_JOBID=$SLURM_JOB_ID
export PBS_O_WORKDIR="$SLURM_SUBMIT_DIR"
export PBS_QUEUE=batch

# LOAD MODULES, INSERT CODE, AND RUN YOUR PROGRAMS HERE
make clean
make all

echo "SEQ"
./bin/seqMain.o

for i in 1 2 4 8 16; do
        echo "-----------------------"
        echo $i threads
        echo "-----------------------"
        echo "PTHREAD"
        ./bin/pthreadMain.o $i
        echo "LINEAR"
        ./bin/linearMain.o $i
        echo "LOG"
        ./bin/logMain.o $i
        echo "REBUILD"
        ./bin/rebuildMain.o $i
        echo "\n\n"
done