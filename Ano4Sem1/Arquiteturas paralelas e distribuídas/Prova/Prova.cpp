#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// mpirun -np 5 ./Ola

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

void Algoritmo(char);
char *VectorList(int *);

void Lamport(char*);
void LamportVetor(char *);
void Centralizado();
void Anel();
void Bully();


MPI_Status stat;
int rank,Mpisize,repet = 2;

typedef struct _Ptc
{
	char msg[30];
	int rank;
	int timestamp;
	int timestamplist[10];
} PTC;
MPI_Datatype mpi_ptc_type;

int main (int argc, char *argv[])
{
	// primeiro parametro eh a escolha do exercicio
	// o segundo parametro eh a msg que sera transmitida

	int escolha = atoi(argv[1]);

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&Mpisize);

	const int nitems=4;
    int blocklengths[nitems] = {10,1,1,10};
    MPI_Datatype types[nitems] = {MPI_CHARACTER,MPI_INT,MPI_INT,MPI_INT};
    MPI_Aint     offsets[nitems];
    offsets[0] = offsetof(PTC, msg);
    offsets[1] = offsetof(PTC, rank);
    offsets[2] = offsetof(PTC, timestamp);
    offsets[3] = offsetof(PTC, timestamplist);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_ptc_type);
    MPI_Type_commit(&mpi_ptc_type);

	switch(escolha){
		case 1:
			// Exercicio 1
			Lamport(argv[2]);
			break;
		case 2:
			LamportVetor(argv[2]);
			break;
		case 3:
			Centralizado();
			break;
		case 4:
			Algoritmo('c');
			break;
		case 5:
			Anel();
			break;
		case 6:
			Bully();
			break;
		case 7:
			Algoritmo('c');
			break;
		case 8:
			Algoritmo('c');
			break;
		case 9:
			return 0;
	}
	
	MPI_Finalize();
}

void Algoritmo(char escolha){
	int msg,source,dest,tag;

	int i;

	msg=666;dest=rank+1;tag=0;
	
	if(rank<Mpisize-1){
		MPI_Send(&msg,1,MPI_INT,dest,tag,MPI_COMM_WORLD);
		printf("%c - Processo %d enviou %d para o processo %d, Total:%d\n",escolha, rank,msg,dest,Mpisize);
		
	}

	source=rank-1;tag=0;
	if(rank>0){
		MPI_Recv(&msg,1,MPI_INT,source,tag,MPI_COMM_WORLD,&stat);
		printf("%c - Processo %d recebeu %d do processo %d Total:%d \n",escolha,rank,msg,source,Mpisize);
	}
}

int RandI(int Seed){
	srand(time(NULL));
	int r = rand() % Mpisize;

	while(Seed == r)
		r = rand() % Mpisize;
	return r;
}

void Send(PTC *Prot,int Dest){
	MPI_Send(Prot,1,mpi_ptc_type,Dest,0,MPI_COMM_WORLD);
}

PTC Recv(){
	PTC recvptc;
	MPI_Recv(&recvptc,1,mpi_ptc_type,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&stat);
	recvptc.rank = stat.MPI_SOURCE;

	return recvptc;
}

PTC RecvTimeOut(int Time){
	MPI_Request request;
	PTC recvptc;
	recvptc.rank = -1;

	MPI_Irecv(&recvptc,1,mpi_ptc_type,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&request);
    time_t now = time(NULL);
    while(time(NULL) < now + Time){
        int flag = 0;
        MPI_Test(&request, &flag, &stat);
        if(flag){
            recvptc.rank = stat.MPI_SOURCE;
            break;
        }
    }

	return recvptc;
}

void Lamport(char *tmsg){
	PTC ptc,recvptc;

	strcpy(ptc.msg, tmsg);

	ptc.timestamp = 0;
	int dest = rank+1;;
	char *msg;
	
	if(rank == 0){
		// so o primeiro faz o primeiro envio
		Send(&ptc,dest);
		printf("Time:%3d Msg: %s Fr:%d To:%d\n",ptc.timestamp,ptc.msg,rank,dest);
	}

	int i;
	for(i = 0;i<repet;i++){
		// aguarda receber de algum outro processo
		recvptc = Recv();
		ptc.timestamp = max(ptc.timestamp,recvptc.timestamp) +1;
		dest=rank + 1;
		//printf("Time:%3d Msg: %s Rank:%d\n",recvptc.timestamp,recvptc.msg,rank);

		// se estiver no ultimo envia para o primeiro
		if(dest	== Mpisize)
			dest = 0;

		// envia para o proximo processo
		Send(&ptc,dest);
		printf("Time:%3d Msg: %s Fr:%d To:%d\n",ptc.timestamp,ptc.msg,rank,dest);
	}
}

void LamportVetor(char *tmsg){
	int i,j;
	PTC ptc,recvptc;

	strcpy(ptc.msg, tmsg);

	for(i=0;i<Mpisize;i++)
		ptc.timestamplist[i] = 0;

	int dest = rank+1;;
	char *msg;
	
	if(rank == 0){
		// so o primeiro faz o primeiro envio
		Send(&ptc,dest);
		//printf("Time:%3d Msg: %s Fr:%d To:%d\n",ptc.timestamp,ptc.msg,rank,dest);
	}

	for(i = 0;i<repet;i++){
		// aguarda receber de algum outro processo
		recvptc = Recv();
		for(j=0;j<Mpisize;j++){
			if(j!=rank)
				ptc.timestamplist[j] = max(ptc.timestamplist[j],recvptc.timestamplist[j]) ;
		}
		ptc.timestamplist[rank]++;

		dest=rank + 1;
		printf("Time:%s Msg: %s Rank:%d\n",VectorList(ptc.timestamplist),recvptc.msg,rank);

		// se estiver no ultimo envia para o primeiro
		if(dest	== Mpisize)
			dest = 0;

		// envia para o proximo processo
		Send(&ptc,dest);
		//printf("Time:%s Msg: %s Fr:%d To:%d\n",VectorList(ptc.timestamplist),ptc.msg,rank,dest);
	}
}

char *VectorList(int *List){
	int tam = Mpisize;
	char *retorno = (char*)malloc((tam*2) + 1);

	strcpy(retorno, "(");
	char buffer [20];
	int i;
	for(i=0;i<tam;i++){
		if(i > 0)
			strcat(retorno,",");

		sprintf(buffer,"%d",List[i]);
		strcat(retorno,buffer);
	}
	strcat(retorno, ")");
	return retorno;
}

void Centralizado(){

	int area = 0,i = 0;
	int parar = 0;
	PTC ptc,recvptc;

	while(!parar){
		if(rank == 0){
			printf("%d - %s\n",rank,"Esperando Pedido");
			recvptc = Recv();
			//printf("%s%s\n","Recebeu - ",recvptc.msg);

			if(strcmp(recvptc.msg, "Acesso") == 0){
				//printf("%s - %d\n","Pedido",recvptc.rank);
				if(area	== 0){
					strcpy(ptc.msg, "Pode");

					printf("%d -> %s -> %d\n",rank,"Pode",recvptc.rank);
					Send(&ptc,recvptc.rank);
					area = recvptc.rank;
					i++;
				}
				else{
					strcpy(ptc.msg, "Nao");
					printf("%d -> %s -> %d\n",rank,"Nao",recvptc.rank);
					Send(&ptc,recvptc.rank);
				}
			}

			if(strcmp(recvptc.msg, "Liberar") == 0){
				area = 0;
				printf("%d -> %s -> %d\n",recvptc.rank,"Liberar",rank);
			}

			if(i == Mpisize-1)
				parar = 1;
		}

		if(rank > 0){
			strcpy(ptc.msg, "Acesso");
			printf("%d - %s\n",rank,"Pedido");
			Send(&ptc,0);

			recvptc = Recv();

			if(strcmp(recvptc.msg, "Pode") == 0)
				area = 1;


			if(area == 1){
				printf("%d - %s\n",rank,"Area Critica");
				strcpy(ptc.msg, "Liberar");
				
				Send(&ptc,0);
				parar = 1;
			}
		}
	}
}

void Anel(){
	int token = 0,i = 0,dest;
	int parar = 0;
	PTC ptc,recvptc;

	if(rank == 0)
		token = 1;

	for (i = 0; i < repet; ++i)
	{
		while(!parar){
			if(token){
				printf("%d - Posso Acessar\n",rank);

				strcpy(ptc.msg, "Passar");
				dest = rank + 1;

				if(dest == Mpisize)
					dest = 0;

				printf("%d -> %d\n",rank,dest);
				Send(&ptc,dest);
				token = 0;

				parar = 1;
			}

			if(!parar){
				recvptc = Recv();
				token = 1;
			}
		}
		parar = 0;
	}
}

void Bully(){

	int lider = 0;
	int area = 0,i = 0;
	int parar = 0;
	PTC ptc,recvptc;

	while(!parar){
		if(rank == lider){
			printf("%d - %s\n",rank,"Esperando Pedido");
			recvptc = Recv();
			//printf("%s%s\n","Recebeu - ",recvptc.msg);

			if(strcmp(recvptc.msg, "Acesso") == 0){
				//printf("%s - %d\n","Pedido",recvptc.rank);
				if(area	== 0){
					strcpy(ptc.msg, "Pode");

					printf("%d -> %s -> %d\n",rank,"Pode",recvptc.rank);
					Send(&ptc,recvptc.rank);
					area = recvptc.rank;
					i++;
				}
				else{
					strcpy(ptc.msg, "Nao");
					printf("%d -> %s -> %d\n",rank,"Nao",recvptc.rank);
					Send(&ptc,recvptc.rank);
				}
			}

			if(strcmp(recvptc.msg, "Liberar") == 0){
				area = 0;
				printf("%d -> %s -> %d\n",recvptc.rank,"Liberar",rank);
			}

			//if(i == Mpisize-1)
				parar = 1;
		}

		if(rank != lider){
			strcpy(ptc.msg, "Acesso");
			printf("%d - %s\n",rank,"Pedido");
			Send(&ptc,lider);

			recvptc = RecvTimeOut(1);

			if(recvptc.rank == -1)
				printf("%s\n","Time Out!");

			if(strcmp(recvptc.msg, "Pode") == 0)
				area = 1;


			if(area == 1){
				printf("%d - %s\n",rank,"Area Critica");
				strcpy(ptc.msg, "Liberar");
				
				Send(&ptc,lider);
				parar = 1;
			}
		}
	}
}