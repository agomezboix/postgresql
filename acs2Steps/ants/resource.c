#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Library.h"

FILE *f;

void inicializeMatrixPheromone(int sizeProblem, double pheromoneInicial)
{
    int i;
    int j;
    matrixPheromone=malloc(sizeof(double*)*sizeProblem);
	for (i=0; i<sizeProblem; i++)
	{
		matrixPheromone[i]=malloc(sizeof(double)*sizeProblem);
	}
    //matrixPheromone=malloc(sizeof(double)*sizeProblem*sizeProblem);
    for (i = 0; i < sizeProblem; i++)
    {	matrixPheromone[i][i]=0;
        for (j = i + 1; j < sizeProblem; j++)
        {
            matrixPheromone[j][i] = pheromoneInicial;
            matrixPheromone[i][j] = pheromoneInicial;
        }
    }
}

void inicializeMatrixWeight(double **matrix,int sizeProblem)
{
    int i;
    int j;
    matrixWeight=malloc(sizeof(double*)*sizeProblem);
   	for (i=0; i<sizeProblem; i++)
   	{
   		matrixWeight[i]=malloc(sizeof(double)*sizeProblem);
   	}
    //matrixWeight=malloc(sizeof(double)*sizeProblem*sizeProblem);
    for(i=0;i<sizeProblem;i++)
    {
        for(j=0;j<sizeProblem;j++)
        {
            matrixWeight[i][j]=matrix[i][j];
        }
    }
}


int ruleta(double *array, int length)
{
    double minimo = 0;
    double maximo = 0;
    int numAux = 0;

    //double q = geqo_rand();
    //srand((unsigned)time(NULL));
    double q=frand();// da un numero entre 0 y 1***************************************************************************************************************************
    int i;
    for (i = 0; i < length; i++)
    {
        maximo += array[i];
        if ((maximo >= q) && (q > minimo))
        {
            numAux = i;
            break;
        }
        minimo = maximo;
    }
    return numAux;
}

//busca la mejor solucion dentro del arreglo de hormigas
//int * searchBestSolution(Ants * arrayAnts,int cantAnts,int sizeProblem)
Ants searchBestSolution(PlannerInfo * root,Ants * arrayAnts,int cantAnts,int sizeProblem, int stepOne)
{
    int i,j;
    int bestIndex=0;
    //Ants bestAnts=arrayAnts[0];
    double temp;
    double bestCost;
    Gene *tmp;

	tmp=arrayAnts[0].tourMemory;
   bestCost=geqo_eval(root,tmp,sizeProblem);

	arrayAnts[bestIndex].cost=bestCost;

    for(i=1;i < cantAnts;i++)
    {
    	arrayAnts[i].cost=geqo_eval(root,arrayAnts[i].tourMemory,sizeProblem);

        if(bestCost>arrayAnts[i].cost)
        {
            bestCost=arrayAnts[i].cost;
            bestIndex=i;
        }
    }
    //return arrayAnts[bestIndex].tourMemory;
    return arrayAnts[bestIndex];
}

void updatePheromone_Step_by_Step(int ini, int end, double constEvaporation, double pheromoneInicial)
{
	matrixPheromone[ini][end]= (1 - constEvaporation)* matrixPheromone[ini][end]+ constEvaporation* pheromoneInicial;
}

void updatePheromone_Global(Ants ant, int sizeProblem, double constEvaporation, int sizeProblemSteps)
{
    int i;
    int j;
    int k;
    int * tour=ant.tourMemory;
    double coste = ant.cost;
    for (i = 0; i < sizeProblem; i++)
    {
        for (j = i + 1; j < sizeProblem; j++)
        {
            matrixPheromone[j][i] = (1 - constEvaporation) * matrixPheromone[i][j];
        }
    }
    //for (i = 1; i < sizeProblem; i++)
    //printf("cantidad de estados: %d",ant.cantState);
    //for (i = 1; i < ant.cantState; i++)
    for (i = 1; i < sizeProblemSteps; i++)
    {
        j = tour[i - 1]-1;
        k = tour[i]-1;
       // matrixPheromone[k][j] += constEvaporation * (1 / coste);
        matrixPheromone[j][k] += constEvaporation * (1 / coste);
    }

}

int insertInOrden(PlannerInfo *root, Ants *setAnts,int sizeSet,Ants ants, int countSetAnts)
{
	//el problema es aqui, deja a setAnts con una homiga menos de la que lleva
	int i,j;
	Ants tempAnts;
	ants.cost=geqo_eval(root,ants.tourMemory,ants.cantState);
	for(i=0;i<sizeSet;i++)
	{
		if(countSetAnts < i+1)
		{
			setAnts[i]=ants;
			countSetAnts++;
			break;
		}
		else if(ants.cost < setAnts[i].cost)
		{
			tempAnts=setAnts[i];
			setAnts[i]=ants;
			ants=tempAnts;
			/*printf("Este es setAnts[%d] ",i);
			for(j=0;j<setAnts[i].cantState;j++)
				printf("%d ",setAnts[i].tourMemory[j]);
			printf("\n ");
			printf("Este es ants ");
			for(j=0;j<ants.cantState;j++)
				printf("%d ",ants.tourMemory[j]);
			printf("\n ");*/

		}
	}
	return countSetAnts;
}



