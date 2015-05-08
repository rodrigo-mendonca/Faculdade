#include <stdio.h>
#include <time.h>

int f1(int,int);
int f2(int,int);

int main (int argc, char *argv[])
{
	while(true){
		int x,y;
		int escolha;
		int Result;
		printf("1 - Resursivo!\n");
		printf("2 - Nao Resursivo!\n>");
		scanf("%i",&escolha);

		printf("Digite o X:");
		scanf("%i",&x);
		printf("Digite o Y:");
		scanf("%i",&y);

		clock_t oClock = clock();

		if(escolha == 1)
			Result = f1(x,y);
		if(escolha == 2)
			Result = f2(x,y);

		printf("Resultado: %d\n",Result );

		float nNow = (float)(clock() - oClock) / CLOCKS_PER_SEC;
		printf("\n\n%.4f Segundos\n\n",nNow);
	}
}

int f1(int x,int y)
{
	if(x==0 || y==0)
		return 0;
	else
		return f1(x-1,y)+f1(x,y-1)+x*y;
}

int  f2(int x,int y)
{
	int i,j,t[100][100];

	for (i = 0; i <= x; ++i)
        t[i][0] = 0;
    for (j = 0; j <= y; ++j)
        t[0][j] = 0;

	for (i = 1; i <= x; ++i)
		for (j = 1; j <= y; ++j)
			t[i][j] = t[i-1][j] + t[i][j-1]+i*j;

	return t[x][y];
}
