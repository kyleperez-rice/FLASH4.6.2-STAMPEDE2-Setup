export LMOD_EXPERT=1
module purge
module load intel/17.0.4
module load mvapich2
module use /opt/apps/intel17/impi17_0/modulefiles
module load petsc/3.7
module use /opt/apps/intel17/impi17_0/modulefiles
module load phdf5
module load python
export TACC_VEC_FLAGS="-xCORE-AVX2 -axCOMMON-AVX512,MIC-AVX512"
module list
