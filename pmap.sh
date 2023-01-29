#!/bin/bash
# --reservation=COURSE_RESERVATION_jhlsrf022
#SBATCH --job-name="pmap"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=01:00:00
#SBATCH --partition=normal
#SBATCH --output=pmap_%j.out
#SBATCH --error=pmap_%j.err

module purge
module load 2020
module load GCC/9.3.0

echo "OpenMP parallelism"
echo

for ncores in {3}
do
  export OMP_NUM_THREADS=$ncores

  echo "CPUS: " $OMP_NUM_THREADS
  echo "CPUS: " $OMP_NUM_THREADS >&2
  ./pmap
  echo "DONE "
done
