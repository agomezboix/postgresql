#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
} Ants;

int executeACO_As_TSP(FILE*,double **,int, double, int, int, int, int);
int * localSerch2_opt(int *,int);
void writeSolution(FILE*,int *,int, int);

//random.c
int newRandint(int, int);
double geqo_rand();
void geqo_set_seed( double);

//en Ants.c
Ants * init_Ants(int);
int visited(Ants,int);
double exponential(double, int);
void setNextStep(Ants *, int, int, int);
void setState(Ants *,int);
double calculateCoste(int *,int);
double calculateCoste_Circle(int *,int);

//en resource.c
int * searchBestSolution(Ants *,int,int);
int ruleta(double *, int);
void inicializeConstPheromone(double, int);
void inicializeMatrixWeight(double **,int);
void inicializeMatrixPheromone(int, double);
void updatePheromone_Global(int *, int, double);
void regulate_PheromoneMaxMin(int);
void calculate_PheromoneMaxMin(double, int,double);

//en eran.c
double pg_erand48(unsigned short [3]);

//esto permite generar numeros entre cero y 1
#define frand() ((double) rand() / (RAND_MAX+1.0))

/* geqo_randint returns integer value between lower and upper inclusive */
#define geqo_randint( upper, lower) \
	 ( (int) ( frand()*(((upper)-(lower))+0.999999) ) + (lower));

//bool visited(Ants *,int);

#define Min(x, y)		((x) < (y) ? (x) : (y))
