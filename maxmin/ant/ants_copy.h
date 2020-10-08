/*-------------------------------------------------------------------------
 *
 * geqo_copy.h
 *	  prototypes for copy functions in optimizer/geqo
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/geqo_copy.h
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


#include "ant.h"
#include "ants_random.h"



Ants searchBestSolution(PlannerInfo *root,Ants *,int,int);
int ruleta(PlannerInfo *,double *, int);
void inicializeConstPheromone(double, int);
void inicializeMatrixWeight(double **,int);
void inicializeMatrixPheromone(int, double);
void updatePheromone_Global(Ants ant, int sizeProblem, double constEvaporation);
void regulate_PheromoneMaxMin(int);
void calculate_PheromoneMaxMin(double, int,double);


