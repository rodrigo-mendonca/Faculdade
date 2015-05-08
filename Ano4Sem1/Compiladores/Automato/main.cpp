#include <stdio.h>
#include <stdlib.h>

using namespace std;

void Automato1();
bool A2S0(char *entrada);

void Automato2();
bool A1S0(char *entrada);


bool AceitaL(char *);

char Ling[] = {'0','1'};

int main()
{
    int escolha;

    while(true){
        printf("Escolha um Automato!\n");
        printf("1 - L={0,1} {w | w contem no minimo dois 0s e no maximo um 1}\n");
        printf("2 - L={0,1} {w | w contem um numero par de 0s ou contem exatamente dois 1s}\n");
        printf("3 - Sair\n");
        printf("->");
        scanf("%i",&escolha);

        // verifica se foi digitado um escolha valida
        while(!(escolha> 0 && escolha< 4))
        {
            printf("->");
            scanf("%i",&escolha);
        }

        if(escolha == 1)
            Automato1();
        if(escolha == 2)
            Automato2();

        // sair
        if(escolha == 3)
            return 0;
    }
    return 0;
}

bool AceitaL(char *entrada)
{
    bool aceita = false;
    int tam = 100;

    while(!aceita)
    {
        printf("Entrada -> ");
        scanf("%s",entrada);
        aceita =true;

        for(int i=0;i<tam;i++)
        {
            if(entrada[i] == NULL)
                break;

            if(entrada[i] != '0' && entrada[i]!='1')
            {
                aceita = false;
                printf("Entrada nao esta na linguagem!\n");
                break;
            }
        }
    }
    return aceita;
}

void Automato1()
{
    char *entrada = (char*) malloc(sizeof(char)*100);
    printf("Automato 1\n");
    printf("AFD -> {w | w contem no minimo dois 0s e no maximo um 1}\n");

    if(AceitaL(entrada))
        printf("Linguagem aceita!\n");

    int i = 0;
    bool aceita = false;
    int etapa = 1;

    while(entrada[i]!=NULL)
    {
        switch(etapa)
        {
            case 1:
                if(entrada[i] == '0')
                    etapa = 5;
                if(entrada[i] == '1')
                    etapa = 2;
                break;
            case 2:
                if(entrada[i] == '0')
                    etapa = 3;
                if(entrada[i] == '1')
                    etapa = 8;
                break;
            case 3:
                if(entrada[i] == '0')
                    etapa = 4;
                if(entrada[i] == '1')
                    etapa = 8;
                break;
            case 4:
                if(entrada[i] == '0')
                    etapa = 4;
                if(entrada[i] == '1')
                    etapa = 8;
                break;
            case 5:
                if(entrada[i] == '0')
                    etapa = 7;
                if(entrada[i] == '1')
                    etapa = 6;
                break;
            case 6:
                if(entrada[i] == '0')
                    etapa = 4;
                if(entrada[i] == '1')
                    etapa = 8;
                break;
            case 7:
                if(entrada[i] == '0')
                    etapa = 7;
                if(entrada[i] == '1')
                    etapa = 4;
                break;
            case 8:
                etapa = 8;
                break;

        }
        i++;
    }
    aceita = etapa == 4 || etapa == 7;

    if(aceita)
        printf("Entrada aceita pela automato!\n\n\n");
    else
        printf("Entrada nao foi aceita pela automato!\n\n\n");
}

void Automato2()
{
    char *entrada = (char*) malloc(sizeof(char)*100);
    printf("Automato 2\n");
    printf("AFN -> {w | w contem um numero par de 0s ou contem exatamente dois 1s}\n");

    if(AceitaL(entrada))
        printf("Linguagem aceita!\n");

    int i = 0;
    bool aceita = false;
    int etapa = 1;

    while(entrada[i]!=NULL)
    {
        switch(etapa)
        {
            case 1:
                if(entrada[i] == '0')
                    etapa = 2;
                if(entrada[i] == '1')
                    etapa = 4;
                break;
            case 2:
                if(entrada[i] == '0')
                    etapa = 3;
                if(entrada[i] == '1')
                    etapa = 6;
                break;
            case 3:
                if(entrada[i] == '0')
                    etapa = 2;
                if(entrada[i] == '1')
                    etapa = 7;
                break;
            case 4:
                if(entrada[i] == '0')
                    etapa = 6;
                if(entrada[i] == '1')
                    etapa = 0;
                break;
            case 5:
                if(entrada[i] == '0')
                    etapa = 5;
                if(entrada[i] == '1')
                    etapa = 5;
                break;
            case 6:
                if(entrada[i] == '0')
                    etapa = 3;
                if(entrada[i] == '1')
                    etapa = 5;
                break;
            case 7:
                if(entrada[i] == '0')
                    etapa = 6;
                if(entrada[i] == '1')
                    etapa = 5;
                break;
        }
        i++;
    }
    aceita = etapa == 3 || etapa == 5;

    if(aceita)
        printf("Entrada aceita pela automato!\n\n\n");
    else
        printf("Entrada nao foi aceita pela automato!\n\n\n");
}
