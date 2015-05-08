#include <mpi.h>
#include <stdio.h>

// mpirun -np 5 ./Ola

int main (int argc, char *argv[])
{
	int rank,size;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	printf("Olá do processo %d de %d\n", rank,size);

	MPI_Finalize();
}
