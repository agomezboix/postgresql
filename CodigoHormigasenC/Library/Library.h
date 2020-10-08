#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double **matrixWeight;
double **matrixPheromone;
double pheromoneMaximum;
double pheromoneMinimum;
//extern unsigned short *random_state=2;

typedef struct Ants
{
	int	   *tourMemory;
	int    beginState;
	int    cantState;
} Ants;

int executeACO_As_TSP(FILE*,double **,int, double, int, int, int, int);

double geqo_rand();

int geqo_randint(int, int);

void geqo_set_seed(double);

static void _dorand48(unsigned short *);

double pg_erand48(unsigned short *);

long pg_lrand48(void);

void pg_srand48(long);

Ants * init_Ants(int);

//bool visited(Ants *,int);

#define Min(x, y)		((x) < (y) ? (x) : (y))
