#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Library.h"

double		Geqo_seed;

void getSolution(char * filename, char * filenameOut,double constEvaporation,int cantAnts, int alpha, int beta)
{
    FILE* fe = fopen(filename,"r");
    FILE* fs = fopen(filenameOut,"w");
    //int *solution;

    int sizeProblem, i, j,opt, itermax, iteration;
    double dist;
    double o;
    double **matrix;
    fscanf(fe,"%d %d %d",&sizeProblem, &opt,&itermax);
    //printf("tamano del problema: %d\n",sizeProblem);
    matrix=malloc(sizeof(double*)*sizeProblem);
	for (i=0; i<sizeProblem; i++)
	{
		matrix[i]=malloc(sizeof(double)*sizeProblem);
	}

    for (i=0; i<sizeProblem; i++)
    {
        for (j=0; j<sizeProblem; j++)
        {
            fscanf(fe,"%lf", &dist);
            matrix[i][j]=dist;
            o=matrix[i][j];
        }
        printf("\n");
    }
    fclose(fe);

    //poniendo semillas
    //srand((unsigned)time(NULL));
    set_Seed();

    iteration=executeACO_As_TSP(fs,matrix,sizeProblem,constEvaporation,cantAnts,itermax,alpha,beta);

    printf("cantidad de ciclos = %d\n",iteration);
    fclose(fe);
    fclose(fs);
    free(matrix);
}

int main(int argc, char *argv[])
{
    double constEvaporation=0.6;
    int cantAnts=10;
    //int cantCicle=100;
    int alpha=1;
    int beta=2;
    getSolution("entrada.txt", "my resultdo.txt",constEvaporation,cantAnts,alpha,beta);
    //srand((unsigned)time(NULL));
    /*int i=0;
    double n;
    for(i=0; i<50; i++)
    {
        n=newRandint(50,15);
        printf("el %d es:%f\n",i,n);
    }*/

    //system("PAUSE");
    return 0;
}
