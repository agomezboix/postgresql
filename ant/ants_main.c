/*------------------------------------------------------------------------
 *
c
 *	  solution to the query optimization problem
 *	  by means of a Genetic Algorithm (GA)
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/backend/optimiz
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

#include <math.h>



#include "optimizer/ants_pool.h"
#include "optimizer/ants_random.h"
#include "optimizer/ant.h"
#include <time.h>
#include <sys/timeb.h>



RelOptInfo *
ants(PlannerInfo *root, int number_of_rels, List *initial_rels)
//(double **matrix,int sizeProblem,double constEvaporation,int cantAnts,int cantCicle, int alpha, int beta)
{
	PrivateData private;
    int nodeInitial;
    struct timeb init,end;
    RelOptInfo *best_rel;
    Ants arrayAnts[CANTANTS];
    int i,j,w,x,state,iteration,cant,dif;
    Ants bestLocalSolution;
    Ants globalSolution;
    Ants *newAnts,*tempA;
    double **matrix,*m;
    double costo,evaporation;
    int * tmp;FILE *f,*f1;
    matrix=malloc(sizeof(double)*number_of_rels*number_of_rels);

    evaporation=0.6;
	root->join_search_private = (void *) &private;
	private.initial_rels = initial_rels;
	f= fopen("/home/alejandro/file","w");
	tmp=palloc(2*sizeof(int));
	cant=CANTANTS;
	bestLocalSolution=init_Ants(0);
	ftime(&init);
	for(i=0;i<number_of_rels;i++){
		matrix[i]=malloc(sizeof(double)*number_of_rels);
	}
	for(i=0;i<number_of_rels;i++){
		for(j=0;j<number_of_rels;j++){
			tmp[0]=i+1;
			tmp[1]=j+1;
			if(i==j){
				matrix[i][j]=0;
			}
			else{
				costo=ant_eval(root,tmp,2);
				matrix[i][j]=costo;
			}
		}
	}





    //double matrixPheromone[sizeProblem][sizeProblem];
    inicializeMatrixPheromone(number_of_rels,evaporation);
    inicializeMatrixWeight(matrix,number_of_rels);



    for (i = 0; i < CANTCICLES; i++)
    {
           //Programaci�n_de_actividades
          //Generaci�n_de_Hormigas_y_actividad()
        int j;
        for(j=0;j<cant;j++)
        {
            //Nueva_Hormiga(k)
            nodeInitial=ants_randint(root,number_of_rels,1);//randint es de postgre

            arrayAnts[j] = init_Ants(nodeInitial);
        }
        state=1;
        while(state != number_of_rels)
        {
            int k;
            for(k=0;k<cant;k++)
            {	tempA=&arrayAnts[k];
                setNextStep(root,tempA,number_of_rels,ALPHA,BETA);
            }
            state++;
        }

        // find a local solution
        bestLocalSolution=searchBestSolution(root,arrayAnts,CANTANTS,number_of_rels);





        if(globalSolution.cost==0 || bestLocalSolution.cost<globalSolution.cost)
        {
            globalSolution=bestLocalSolution;
            iteration=i;
        }
        fprintf(f,"%d  \n",i);
        for(w=0;w<cant;w++){

        	for(x=0;x<number_of_rels;x++){
        		fprintf(f,"%d ",arrayAnts[w].tourMemory[x]);
        	}
        	fprintf(f,"--->%If\n",arrayAnts[w].cost);
        }
        updatePheromone_Global(bestLocalSolution,number_of_rels,EVAP);
        calculate_PheromoneMaxMin(bestLocalSolution.cost,number_of_rels,EVAP);
        regulate_PheromoneMaxMin(number_of_rels);

    }
    fprintf(f,"\nGlobal solution\n");
    for(x=0;x<number_of_rels;x++){
    	fprintf(f,"%d ",globalSolution.tourMemory[x]);
    }
    fprintf(f,"--->%If\n",globalSolution.cost);


	best_rel = gimme_tree(root, globalSolution.tourMemory, number_of_rels);
ftime(&end);
	dif=(int)((1000.0*end.time+end.millitm)-(1000.0*init.time+init.millitm));
	 fprintf(f,"\nel tiempo es %d",dif);


	 fclose(f);
    free(matrixWeight);
    free(matrixPheromone);
    free(newAnts);

    return best_rel;
}
