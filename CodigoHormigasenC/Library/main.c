#include <stdio.h>
#include <stdlib.h>
//#include "Library.h"

void getSolution(char * filename, char * filenameOut,double constEvaporation,int cantAnts,int cantCicle, int alpha, int beta)
{
    FILE* fe = fopen(filename,"r");
    FILE* fs = fopen(filenameOut,"w");
    int *solution;
    int number, sizeProblem, i, j, dist,opt, itermax, iteration;
    double **matrix;
    fscanf(fe,"%d %d %d",&sizeProblem, &opt,&itermax);
    matrix=malloc(sizeof(int)*sizeProblem*sizeProblem);
    for (i=0; i<sizeProblem; i++)
    {
        for (j=0; j<sizeProblem; j++)
        {
            fscanf(fe,"%f", &dist);
            matrix[i][j]=dist;
        }
    }
    fclose(fe);
    iteration=executeACO_As_TSP(fs,matrix,sizeProblem,constEvaporation,cantAnts,cantCicle,alpha,beta);

    fclose(fe);
    fclose(fs);
    free(matrix);
}

int main(int argc, char *argv[])
{
    double constEvaporation=0.6;
    int cantAnts=10;
    int cantCicle=100;
    int alpha=1;
    int beta=2;
    getSolution("entrada.txt", "my resultdo.txt",constEvaporation,cantAnts,cantCicle,alpha,beta);
    system("PAUSE");
    return 0;
}


