#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "postgres.h"
#include "optimizer/geqo.h"
#include "optimizer/geqo_random.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"

double **matrixWeight;
double **matrixPheromone;
double pheromoneMaximum;
double pheromoneMinimum;
extern unsigned short *random_state;
extern double		Geqo_seed;//en Postgre es cero por defecto

typedef struct Ants
{
	int	   *tourMemory;
	int    beginState;
	int    cantState;
	double cost;
} Ants;

RelOptInfo * executeACS2Pasos(PlannerInfo *root, int number_of_rels, List *initial_rels);



//en Ants.c
Ants init_Ants(int);
int visited(Ants,int);
double exponential(double, int);
void setNextStep(PlannerInfo * root,Ants *ants, int sizeProblem, int alpha, int beta);
void setState(Ants *,int);



//en resource.c
void inicializeConstPheromone(double constEvaporation, int sizeProblem);
Ants searchBestSolution(PlannerInfo * root,Ants *,int,int,int);
int ruleta(PlannerInfo*,double *, int);
void inicializeMatrixWeight(double **,int);
void inicializeMatrixPheromone(int, double);
void regulate_PheromoneMaxMin(int sizeProblem);
void calculate_PheromoneMaxMin(double cost, int sizeProblem,double constEvaporation);
void updatePheromone_Global(Ants ant, int sizeProblem, double constEvaporation, int sizeProblemSteps);
//en eran.c

int insertInOrden(PlannerInfo *root, Ants *setAnts,int sizeSet,Ants ants, int countSetAnts);
//esto permite generar numeros entre cero y 1
#define frand() ((double) rand() / (RAND_MAX+1.0))



