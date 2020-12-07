# FLASH makefile definitions for Intel mvapich2 compiler on Stampede2 TACC

#----------------------------------------------------------------------------
# Compiler and linker commands
#
#  We use the f90 compiler as the linker, so some C libraries may explicitly
#  need to be added into the link line.
#----------------------------------------------------------------------------

FCOMP   = ${MPICH_HOME}/bin/mpif90
CCOMP   = ${MPICH_HOME}/bin/mpicc
CPPCOMP = ${MPICH_HOME}/bin/mpic++
LINK    = ${MPICH_HOME}/bin/mpif90

# pre-processor flag

PP     = -D

#-----------------------------------------------------------------------------
# Compilation flags
#
#  Three sets of compilation/linking flags are defined: one for optimized code
#  code ("-opt"), one for debugging ("-debug"), and one for testing ("-test").
#  Passing these flags to the setup script will cause the value associated with
#  the corresponding keys (i.e. those ending in "_OPT", "_DEBUG", or "_TEST") to
#  be incorporated into the final Makefile. For example, passing "-opt" to the
#  setup script will cause the flags following "FFLAGS_OPT" to be assigned to
#  "FFLAGS" in the final Makefile. If none of these flags are passed, the default
#  behavior will match that of the "-opt" flag.
#  In general, "-opt" is meant to optimize compilation and linking. "-debug"
#  should enable runtime bounds checking, debugger symbols, and other compiler-
#  specific debugging options. "-test" is useful for testing different
#  combinations of compiler flags particular to your individual system.
#----------------------------------------------------------------------------

OPENMP = -qopenmp

#Suppress the following compiler warning message:
#icc: command line warning #10120: overriding '-O2' with '-O3'

FFLAGS_OPT   = -c ${TACC_VEC_FLAGS} -r8 -i4 -O3 -real-size 64 -diag-disable 10120
FFLAGS_DEBUG = -c -g ${TACC_VEC_FLAGS} -r8 -i4 -O0 -check bounds -check format \
-check output_conversion -warn all -warn error -real-size 64 -check uninit \
-traceback -fp-stack-check -diag-disable 10120 -fpe0 -check pointers
FFLAGS_TEST  = ${FFLAGS_OPT} -fp-model precise
FFLAGS_HYPRE = -I${TACC_PETSC_DIR}/${PETSC_ARCH}/include
CFLAGS_HYPRE = -I${TACC_PETSC_DIR}/${PETSC_ARCH}/include

F90FLAGS = -I${TACC_HDF5_LIB} -DH5_USE_16_API

CFLAGS_OPT   = -c -O3 ${$TACC_VEC_FLAGS} -D_LARGEFILE64_SOURCE -D_FORTIFY_SOURCE=2 \
-diag-disable 10120
CFLAGS_DEBUG = -c -O0 -g ${$TACC_VEC_FLAGS} -traceback -debug all -debug extended \
-D_LARGEFILE64_SOURCE -diag-disable 10120 -ftrapuv -fp-stack-check
CFLAGS_TEST  = ${CFLAGS_OPT} -fp-model precise

CFLAGS_HDF5 = -I${TACC_HDF5_INC} -DH5_USE_16_API
CFLAGS_NCMPI = -I$(NCMPICH_HOME)/include
CFLAGS_MPI   = -I${MPICH_HOME}/include

#----------------------------------------------------------------------------
# Linker flags
#
#  There is a seperate version of the linker flags for each of the _OPT,
#  _DEBUG, and _TEST cases.
#----------------------------------------------------------------------------

LFLAGS_OPT   = ${$TACC_VEC_FLAGS} -diag-disable 10120 -O3 -o
LFLAGS_DEBUG = -diag-disable 10120 -o
LFLAGS_TEST  = -diag-disable 10120 -O2 -o

#----------------------------------------------------------------------------
# Library specific linking
#
#  If a FLASH module has a 'LIBRARY xxx' line in its Config file, we need to
#  create a macro in this Makefile.h for LIB_xxx, which will be added to the
#  link line when FLASH is built.  This allows us to switch between different
#  (incompatible) libraries.  We also create a _OPT, _DEBUG, and _TEST
#  library macro to add any performance-minded libraries (like fast math),
#  depending on how FLASH was setup.
#----------------------------------------------------------------------------

LIB_HDF4 =
LIB_HDF5 = -L${TACC_HDF5_LIB} -lhdf5 -lhdf5_fortran -lz
LIB_PNG  = -lpng

LIB_MPI   =
LIB_NCMPI = -L$(NCMPICH_HOME)/lib -lpnetcdf
LIB_MPE   = -L${MPICH_HOME}/lib -lmpe
LIB_HYPRE = -L${TACC_PETSC_LIB} -lHYPRE -mkl
LIB_LIBNBC = -L/opt/libNBC/1.1.1/lib -lnbc

# Uncomment the following line to use electic fence memory debugger.
# Need the following environmental variable (see env.sh):
# export EF_ALLOW_MALLOC_0=1
#CONFIG_LIB = -L/usr/lib64 -lefence


LIB_FISHPAK =

#----------------------------------------------------------------------------
# Additional machine-dependent object files
#
#  Add any machine specific files here -- they will be compiled and linked
#  when FLASH is built.
#----------------------------------------------------------------------------

MACHOBJ =

#----------------------------------------------------------------------------
# Additional commands
#----------------------------------------------------------------------------

MV = mv -f
AR = ar -r
RM = rm -f
CD = cd
RL = ranlib
ECHO = echo


FFLAGS_DEBUG_LESS = -c -g -r8 -i4 -O0 -check bounds -check format \
-check output_conversion -real-size 64 -check uninit \
-traceback -fp-stack-check -diag-disable 10120 -fpe0 -check pointers

ifeq ($(FLASHBINARY),true)
FFLAGS_WO_WARNALL = $(patsubst -warn all,,$(FFLAGS))
#Turn off compiler error messages for paramesh files that use wrapper
#functions such as MPI_int_SSEND.
amr_migrate_tree_data.o : %.o : %.F90
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(F90FLAGS) $(FDEFINES)	$<
mpi_amr_test_neigh_values.o : %.o : %.F90
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(F90FLAGS) $(FDEFINES)	$<
mpi_amr_checkpoint_default.o : %.o : %.F90
	$(FCOMP) $(FFLAGS_DEBUG_LESS) $(F90FLAGS) $(FDEFINES)	$<
mpi_amr_morton.o : %.o : %.F90
	$(FCOMP) $(FFLAGS_DEBUG_LESS) $(F90FLAGS) $(FDEFINES)	$<

#Fortran 77 source lines exceed 72 characters
umap.o : %.o : %.F
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(FDEFINES)	$<
fftsg.o : %.o : %.f
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(FDEFINES)	$<
fftsg3d.o : %.o : %.f
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(FDEFINES)	$<

#Files mix and match assumed shape arrays, assumed size arrays
#and scalars in function calls.  This is fine but it is viewed as
#a problem when using strict type checking compiler options.
fftpack.o : %.o : %.f90
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(FDEFINES)	$<
gr_pfftDcftForward.o : %.o : %.F90
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(FDEFINES)	$<
gr_pfftDcftInverse.o : %.o : %.F90
	$(FCOMP) $(FFLAGS_WO_WARNALL) $(FDEFINES)	$<

endif
