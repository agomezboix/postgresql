/*-------------------------------------------------------------------------
 *
 * geqo.h
 *	  prototypes for various files in optimizer/geqo
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/geqo.h
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

#ifndef GEQO_H
#define GEQO_H

#include "nodes/relation.h"


#include <math.h>

/* GEQO debug flag */
/*
 #define GEQO_DEBUG
 */

/* recombination mechanism */
/*
 #define ERX
 #define PMX
 #define CX
 #define PX
 #define OX1
 #define OX2
 */
#define ERX


/*
 * Configuration options
 *
 * If you change these, update backend/utils/misc/postgresql.conf.sample
 */
		/* 1 .. 10, knob for adjustment of defaults */


#define ALPHA 5
#define BETA 1
#define CANTCICLES 25
#define CANTANTS 10
#define EVAP 0.6




double **matrixWeight;
double **matrixPheromone;
double pheromoneMaximum;
double pheromoneMinimum;
extern unsigned short *random_state;


typedef struct Ants
{
	int	   *tourMemory;
	int    beginState;
	int    cantState;
	double cost;
} Ants;




/*
 * Private state for a GEQO run --- accessible via root->join_search_private
 */
typedef struct
{
	List	   *initial_rels;	/* the base relations we are joining */
	unsigned short random_state[3];		/* state for pg_erand48() */
} PrivateData;


/* routines in geqo_main.c */
extern RelOptInfo *ant(PlannerInfo *root,
	 int number_of_rels, List *initial_rels);

/* routines in geqo_eval.c */
extern Cost ant_eval(PlannerInfo *root, int *tour, int num_gene);
extern RelOptInfo *gimme_tree(PlannerInfo *root, int *tour, int num_gene);


extern RelOptInfo *executeACO_As_TSP(PlannerInfo *root,
	 int number_of_rels, List *initial_rels);
//int * localSerch2_opt(PlannerInfo *root,int *,int);

//en Ants.c
Ants  init_Ants(int);
int visited(Ants,int);
double exponential(double, int);
void setNextStep(PlannerInfo * root,Ants *ants, int sizeProblem, int alpha, int beta);
void setState(Ants*,int);
double calculateCoste(int *,int);


//en resource.c
Ants searchBestSolution(PlannerInfo *root,Ants *,int,int);
int ruleta(PlannerInfo*,double *, int);
void inicializeConstPheromone(double, int);
void inicializeMatrixWeight(double **,int);
void inicializeMatrixPheromone(int, double);
void updatePheromone_Global(Ants ant, int sizeProblem, double constEvaporation);
void regulate_PheromoneMaxMin(int);
void calculate_PheromoneMaxMin(double, int,double);

RelOptInfo * ants(PlannerInfo *root, int number_of_rels, List *initial_rels);

Cost ant_eval(PlannerInfo *root, int *tour, int num_gene);
RelOptInfo * gimme_tree(PlannerInfo *root, int *tour, int num_gene);


#define Min(x, y)		((x) < (y) ? (x) : (y))


#endif   /* GEQO_H */
