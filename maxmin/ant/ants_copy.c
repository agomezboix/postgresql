/*------------------------------------------------------------------------
 *
 * geqo_copy.c
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/backend/optimizer/geqo/geqo_copy.c
 *
 *-------------------------------------------------------------------------
 */

/* contributed by:
   =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
   *  Martin Utesch				 * Institute of Automatic Control	   *
   =							 = University of Mining and Technology =
   *  utesch@aut.tu-freiberg.de  * Freiberg, Germany				   *
   =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
 */

/* this is adopted from D. Whitley's Genitor algorithm */

/*************************************************************/
/*															 */
/*	Copyright (c) 1990										 */
/*	Darrell L. Whitley										 */
/*	Computer Science Department								 */
/*	Colorado State University								 */
/*															 */
/*	Permission is hereby granted to copy all or any part of  */
/*	this program for free distribution.   The author's name  */
/*	and this copyright notice must be included in any copy.  */
/*															 */
/*************************************************************/

#include "postgres.h"
#include "ants_copy.h"

/* geqo_copy
 *
 *	 copies one gene to another
 *
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


void updatePheromone_Global(Ants ant, int sizeProblem, double constEvaporation)
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
    double *m;
    matrixPheromone=malloc(sizeof(double)*sizeProblem);
    for (i = 0; i < sizeProblem; i++)
    {
    	matrixPheromone[i]=malloc(sizeof(double)*sizeProblem);
    }

    inicializeConstPheromone(constEvaporation,sizeProblem);
    for (i = 0; i < sizeProblem; i++)
    {	matrixPheromone[i][i]=0.0;
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
    double *m;
    matrixWeight=malloc(sizeof(double)*sizeProblem);
    for (i = 0; i < sizeProblem; i++)
        {
        	matrixWeight[i]=malloc(sizeof(double)*sizeProblem);
        }
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

int ruleta(PlannerInfo * root,double *array, int length)
{
	double minimo = 0;
	    double maximo = 0;
	    int numAux = 0;

	    //double q = geqo_rand();
	    //srand((unsigned)time(NULL));
	    double q=ants_rand(root);// da un numero entre 0 y 1***************************************************************************************************************************
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


Ants searchBestSolution(PlannerInfo *root,Ants * arrayAnts,int cantAnts,int sizeProblem)
{
    int i;
    int bestIndex=0;
    //Ants bestAnts=arrayAnts[0];
    double temp;
    //double bestCost=calculateCoste(arrayAnts[0].tourMemory,sizeProblem);
    double bestCost=ant_eval(root,arrayAnts[0].tourMemory,sizeProblem);
	arrayAnts[bestIndex].cost=bestCost;
    for(i=1;i < cantAnts;i++)
    {
        //temp=calculateCoste(arrayAnts[i].tourMemory,sizeProblem);
    	temp=ant_eval(root,arrayAnts[i].tourMemory,sizeProblem);
    	arrayAnts[i].cost=temp;
        if(bestCost>temp)
        {
            bestCost=temp;
            //bestAnts=arrayAnts[i];
            bestIndex=i;
//           arrayAnts[bestIndex].cost=bestCost;
        }
    }
    //return arrayAnts[bestIndex].tourMemory;
    return arrayAnts[bestIndex];
}
