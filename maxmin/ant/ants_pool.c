/*------------------------------------------------------------------------
 *
 * geqo_pool.c
 *	  Genetic Algorithm (GA) pool stuff
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/backend/optimizer/geqo/geqo_pool.c
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

/* -- parts of this are adapted from D. Whitley's Genitor algorithm -- */

#include "postgres.h"

#include <float.h>
#include <limits.h>
#include <math.h>

#include "ants_copy.h"
#include "ants_pool.h"




/*
 * alloc_pool
 *		allocates memory for GA pool
 */


Ants init_Ants(int nodeInitial)
{
    Ants newAnts;
    newAnts.beginState=(int)nodeInitial;
    newAnts.tourMemory=(int *)malloc(1*sizeof(int));

    newAnts.tourMemory[0]=nodeInitial;
    newAnts.cantState=1;
    newAnts.cost=0;
    return newAnts;
}

/*Calcula el costo de ir del estado en el que estoy a todos los que faltan por recorrer
luego divide cada costo individual  entre la suma de todos y elije uno con la ruleta
*/
//void setNextStep(Ants ants, double **matrixWeight,double **matrixPheromone, int sizeProblem, int alpha, int beta)

void setNextStep(PlannerInfo * root,Ants *ants, int sizeProblem, int alpha, int beta)
{
    double sumAj=0;
    double arrayTemp[sizeProblem];
    int currentState=ants->tourMemory[ants->cantState-1];
    int i;
    double temp=0;
    double aux=0;
    double pheromone;
    int resultState;
    for(i=0; i<sizeProblem; i++)
    {
        if(!visited(*ants,i))
        {
        	//se le resta 1 a currentState porque los valores de los estados son numeros desde 1 a sizeProblem incluyendo ambos n�meros
            pheromone=matrixPheromone[currentState-1][i];
            aux=1/matrixWeight[currentState-1][i];
            temp=exponential(pheromone, alpha) * exponential(aux, beta);
            sumAj+=temp;
        }
        arrayTemp[i]=temp;
        temp=0;//esto los puse para que los estados que ya han sido visitados tengan valor 0
    }
   for (i = 0; i < sizeProblem; i++)
    {
        arrayTemp[i] /= sumAj;
    }
    resultState = ruleta(root,arrayTemp,sizeProblem);
    //se le suma 1 a resultState porque los valores de los estados son numeros desde 1 a sizeProblem incluyendo ambos n�meros
    setState(ants,resultState+1);
}

int visited(Ants ants,int state)
{
    int i=0;
    while(i<ants.cantState)
    {
        if(state==(ants.tourMemory[i]-1))
            return 1;
        i++;
    }
    return 0;
}
double exponential(double base, int exponente)
{
    double result = 1;
    int i;
    for (i = 0; i < exponente; i++)
    {
        result *= base;
    }
    return result;
}
void setState(Ants *ants,int state)
{
    int *tour=ants->tourMemory;
    int length=ants->cantState;
    int *temp=malloc(sizeof(int)*(length+1));
    int i;
    for(i=0; i<length; i++)
    {
        temp[i]=tour[i];
    }
    temp[i]=state;
    ants->cantState=length+1;
    ants->tourMemory=temp;
}
//double calculateCoste(Ants ants,double **matrixWeight)
double calculateCoste(int *tour,int lengthTour)
{
    //int *tour=ants.tourMemory;
    int i;
    double cost=0;
    for(i=0;i<lengthTour-1;i++)
    {
        cost+=matrixWeight[tour[i]][tour[i+1]];
    }
    return cost;
}
