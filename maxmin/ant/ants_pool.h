/*-------------------------------------------------------------------------
 *
 * geqo_pool.h
 *	  pool representation in optimizer/geqo
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/geqo_pool.h
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

extern Ants init_Ants(int);
extern int visited(Ants,int);
extern double exponential(double, int);
extern void setNextStep(PlannerInfo * root,Ants *ants, int sizeProblem, int alpha, int beta);
extern void setState(Ants *,int);
extern double calculateCoste(int *,int);



