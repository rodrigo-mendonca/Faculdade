#include <mpi.h>
#include <stdio.h>

// mpirun -np 5 ./Ola

int rank,size,source,dest,tag;
char *msg;
int main (int argc, char *argv[])
{
	MPI_Status stat;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	if(rank==0){
		msg="Bla bla";dest=1;tag=0;

		MPI_Send(&msg,10,MPI_CHARACTER,dest,tag,MPI_COMM_WORLD);
		printf("Processo %d enviou %d para o processo %d\n", rank,msg,dest);

		MPI_Recv(&msg,10,MPI_CHARACTER,source,tag,MPI_COMM_WORLD,&stat);
		printf("Processo %d recebeu %d do processo %d\n",rank,msg,source);
	}
	if(rank==1){
		source=0;tag=0;
		MPI_Recv(&msg,10,MPI_CHARACTER,source,tag,MPI_COMM_WORLD,&stat);
		printf("Processo %d recebeu %d do processo %d\n",rank,msg,source);

		msg="Bla bla";
		MPI_Send(&msg,10,MPI_CHARACTER,dest,tag,MPI_COMM_WORLD);
		printf("Processo %d enviou %d para o processo %d\n", rank,msg,dest);
	}

	MPI_Finalize();
}
