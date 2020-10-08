#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Library.h"

/*extern double **matrixPheromone;
extern double **matrixWeight;
extern double pheromoneMaximum;
extern double pheromoneMinimum;*/

int * searchBestSolution(Ants *,int,int);
int ruleta(double *, int);
void inicializeConstPheromone(double, int);
void inicializeMatrixWeight(double **,int);
void inicializeMatrixPheromone(int, double);
void updatePheromone_Global(int *, int, double);
void regulate_PheromoneMaxMin(int);
void calculate_PheromoneMaxMin(double, int,double);

// este metodo calcula las cuotas superior e inferior de la feromona.
    /**
     * @param cost :el valor de la mejor solucion en la iteracion.
     * @param pBest : probabilidad de eleccion de cada candidato.
     * @param iteration: la iteracion actual.
     */

void calculate_PheromoneMaxMin(double cost, int sizeProblem,double constEvaporation)
{
    pheromoneMaximum = (1 / (1 - constEvaporation)) * (1 / cost);
    pheromoneMinimum = pheromoneMaximum / (2.0 * sizeProblem);
}
// este metodo es el encargado de ajustar los valores de feromana para el Max Min

void regulate_PheromoneMaxMin(int sizeProblem)
{
    int i;
    int j;
    for (i = 0; i < sizeProblem; i++)
    {
        for (j = i + 1; j < sizeProblem; j++) {
            if (matrixPheromone[i][j] > pheromoneMaximum)
            {
               matrixPheromone[i][j] = matrixPheromone[j][i] = pheromoneMaximum;
            }
            else if (matrixPheromone[i][j] < pheromoneMinimum)
            {
                matrixPheromone[i][j] = matrixPheromone[j][i] = pheromoneMinimum;
            }
        }
    }
}

// funciones de update de ferromona para el algoritmo AS

void updatePheromone_Global(int *tour, int sizeProblem, double constEvaporation)
{
    int i;
    int j;
    int k;
    double coste = calculateCoste_Circle(tour,sizeProblem);
    for (i = 0; i < sizeProblem; i++)
    {
        for (j = i + 1; j < sizeProblem; j++)
        {
            matrixPheromone[j][i] = matrixPheromone[i][j] = (1 - constEvaporation) * matrixPheromone[i][j];
        }
    }
    for (i = 1; i < sizeProblem; i++)
    {
        j = tour[i - 1]-1;
        k = tour[i]-1;
        matrixPheromone[k][j] = matrixPheromone[j][k] += constEvaporation * (1 / coste);
    }
    matrixPheromone[tour[sizeProblem - 1]-1][tour[0]-1] += constEvaporation * (1 / coste);

    //el -1 es porque los valores de los estados en tour van desde 1 hasta sizeProblem incluyendo ambos numeros
}

void inicializeMatrixPheromone(int sizeProblem, double constEvaporation)
{
    int i;
    int j;
    matrixPheromone=malloc(sizeof(double*)*sizeProblem);
	for (i=0; i<sizeProblem; i++)
	{
		matrixPheromone[i]=malloc(sizeof(double)*sizeProblem);
	}
    //matrixPheromone=malloc(sizeof(double)*sizeProblem*sizeProblem);
    inicializeConstPheromone(constEvaporation,sizeProblem);
    for (i = 0; i < sizeProblem; i++)
    {
        for (j = i + 1; j < sizeProblem; j++)
        {
            matrixPheromone[j][i] = pheromoneMaximum;
            matrixPheromone[i][j] = pheromoneMaximum;
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

void inicializeConstPheromone(double constEvaporation, int sizeProblem)
{
    pheromoneMaximum=constEvaporation * 1 / 1000 + constEvaporation;
    pheromoneMinimum=pheromoneMaximum / (2 * sizeProblem);
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
int * searchBestSolution(Ants * arrayAnts,int cantAnts,int sizeProblem)
{
    int i;
    int bestIndex=0;
    //Ants bestAnts=arrayAnts[0];
    double temp;
    //double bestCost=calculateCoste(arrayAnts[0].tourMemory,sizeProblem);
    double bestCost=calculateCoste_Circle(arrayAnts[0].tourMemory,sizeProblem);
    for(i=1;i<cantAnts;i++)
    {
        //temp=calculateCoste(arrayAnts[i].tourMemory,sizeProblem);
    	temp=calculateCoste_Circle(arrayAnts[i].tourMemory,sizeProblem);
        if(bestCost>temp)
        {
            bestCost=temp;
            //bestAnts=arrayAnts[i];
            bestIndex=i;
        }
    }
    return arrayAnts[bestIndex].tourMemory;
}

