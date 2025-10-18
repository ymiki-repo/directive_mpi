#!/bin/bash
#PBS -q lecture-g
#PBS -l select=2:mpiprocs=1
#PBS -l walltime=00:10:00
#PBS -W group_list=gt00

module purge
module load nvidia nv-hpcx

cd ${PBS_O_WORKDIR}

export UCX_MEMTYPE_CACHE=n
export UCX_IB_GPU_DIRECT_RDMA=y
mpiexec sh/common/wrapper.sh ./run
