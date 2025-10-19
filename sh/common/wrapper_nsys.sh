#!/usr/bin/env bash

mpi_rank=${OMPI_COMM_WORLD_RANK:=${MV2_COMM_WORLD_RANK:=${PMI_RANK:=${PMIX_RANK:=0}}}}

# obtain process rank within a node
if [ -n "$OMPI_COMM_WORLD_LOCAL_RANK" ] || [ -n "$MV2_COMM_WORLD_LOCAL_RANK" ]; then
	local_rank=${OMPI_COMM_WORLD_LOCAL_RANK:=${MV2_COMM_WORLD_LOCAL_RANK}}
else
	cores_per_node=`LANG=C command lscpu | command sed -n 's/^CPU(s): *//p'`
	mpi_size=${OMPI_COMM_WORLD_SIZE:=${MV2_COMM_WORLD_SIZE:=${PMI_SIZE:=${OMPI_UNIVERSE_SIZE:=1}}}}
	procs_per_node=`expr $mpi_size / $cores_per_node`
	if [ $procs_per_node -lt 1 ]; then
		procs_per_node=$mpi_size
	fi

	local_rank=`expr $mpi_rank % $procs_per_node`
fi

GPU_ID=${local_rank}
export CUDA_VISIBLE_DEVICES=${GPU_ID}

nsys profile --stats=true --trace=osrt,cuda,openacc,openmp,nvtx,mpi,oshmem,ucx -f true -o report_${mpi_rank} $*
