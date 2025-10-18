program main
  use mpi
  implicit none
  integer :: ierr, nprocs, rank
  integer, parameter :: nx = 4096
  integer, parameter :: ny = 4096
  integer, parameter :: w = 10
  real, allocatable :: a(:,:), b(:,:)
  integer :: i, j
  double precision :: st, et
  integer :: dst_rank, tag
  integer, allocatable :: istat(:)
  double precision :: sum
  character :: hostname*128
  integer :: len_name

  call MPI_Init(ierr)

  nprocs = 1
  rank = 0

  call MPI_Comm_size(MPI_COMM_WORLD, nprocs, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

  allocate(istat(MPI_STATUS_SIZE))

  if (nprocs .ne. 2) then
     call MPI_Finalize(ierr)
     stop
  end if

  call MPI_get_processor_name(hostname, len_name, ierr)
  write(*,'(A5,I2,A14,A12)') "Rank ", rank, ": hostname = ", hostname

  allocate(a(nx,ny))
  allocate(b(nx,ny))

  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  st = MPI_WTIME()
  !**** Begin ****

  !$omp target enter data map(alloc: a, b)

  !$omp target teams loop collapse(2) map(from: a, b)
  do j = 1, ny
     do i = 1, nx
        a(i,j) = 3.0 * rank * ny
        b(i,j) = 0.0
     end do
  end do

  dst_rank = mod((rank + 1),nprocs)
  tag      = 0

  if (rank == 0) then
     !$omp target data use_device_ptr(b)
     call MPI_Recv(b, w * nx, MPI_FLOAT, dst_rank, tag, MPI_COMM_WORLD, istat, ierr)
     !$omp end target data
  else
     !$omp target data use_device_ptr(a)
     call MPI_Send(a, w * nx, MPI_FLOAT, dst_rank, tag, MPI_COMM_WORLD, ierr)
     !$omp end target data
  end if

  sum = 0.0
  !$omp target teams loop collapse(2) reduction(+: sum) map(to: b)
  do j = 1, ny
     do i = 1, nx
        sum = sum + b(i,j)
     end do
  end do

  !$omp target exit data map(delete: a, b)

  !**** End ****

  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  et = MPI_WTIME()

  if (rank == 0) then
     write(*,'(A6,F10.6)'), "mean =", sum / (nx*ny)
     write(*,'(A6,F10.6,A6)'), "Time =", et-st, " [sec]"
  end if

  deallocate(a,b)

  call MPI_Finalize(ierr)

end program main
