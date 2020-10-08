#include <stdio.h>
#include <stdlib.h>

#include "Library.h"
extern double **matrixWeight;
extern double **matrixPheromone;

Ants * init_Ants(int);
int visited(Ants,int);
double exponential(double, int);
void setNextStep(Ants, int, int, int);
void setState(Ants,int);
double calculateCoste(int *,int);

Ants * init_Ants(int nodeInitial)
{
    Ants *newAnts;
    int *tourMem;

    newAnts=(Ants *)malloc(sizeof(Ants));
    newAnts->beginState=(int)nodeInitial;
    newAnts->tourMemory=(int *)malloc(1*sizeof(int));

    newAnts->tourMemory[0]=nodeInitial;
    newAnts->cantState=1;
    return newAnts;
}

/*Calcula el costo de ir del estado en el que estoy a todos los que faltan por recorrer
luego divide cada costo individual  entre la suma de todos y elije uno con la ruleta
*/
//void setNextStep(Ants ants, double **matrixWeight,double **matrixPheromone, int sizeProblem, int alpha, int beta)
void setNextStep(Ants ants, int sizeProblem, int alpha, int beta)
{
    double sumAj=0;
    double arrayTemp[sizeProblem];
    int currentState=ants.tourMemory[ants.cantState-1];
    int i;
    double temp=0;
    double aux=0;
    double pheromone;
    int resultState;
    for(i=0; i<sizeProblem; i++)
    {
        if(!visited(ants,i))
        {
            pheromone=matrixPheromone[currentState][i];
            aux=1/matrixWeight[currentState][i];
            temp=exponential(pheromone, alpha) * exponential(aux, beta);
            sumAj+=temp;
        }
        arrayTemp[i]=temp;
    }
    for (i = 0; i < sizeProblem; i++)
    {
        arrayTemp[i] /= sumAj;
    }
    resultState = ruleta(arrayTemp,sizeProblem);
    setState(ants,resultState);
}

int visited(Ants ants,int state)
{
    int i=0;
    while(i<ants.cantState)
    {
        if(state==ants.tourMemory[i])
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
void setState(Ants ants,int state)
{
    int *tour=ants.tourMemory;
    int length=ants.cantState;
    int *temp=malloc(sizeof(int)*(length+1));
    int i;
    for(i=0; i<length; i++)
    {
        temp[i]=tour[i];
    }
    temp[i]=state;
    ants.cantState=length+1;
    ants.tourMemory=temp;
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
//calcula el costo de un viaje circular
double calculateCoste_Circle(int *tour,int lengthTour)
{
    //int lengthTour=ants.cantState;
    //int *tour=ants.tourMemory;
    int i;
    double cost=0;
    for(i=0;i<lengthTour-1;i++)
    {
        cost+=matrixWeight[tour[i]][tour[i+1]];
    }
    cost+=matrixWeight[0][lengthTour-1];
    return cost;
}
