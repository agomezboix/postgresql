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


#include "optimizer/paths.h"
#include "ants_pool.h"
#include "ants_random.h"
#include "ant.h"
#include <time.h>
#include <sys/timeb.h>
#include "fmgr.h"



PG_MODULE_MAGIC;
join_search_hook_type join_search_hook = NULL;
extern void _PG_init(void);
extern void _PG_fini(void);
Ants localSerch2_opt(PlannerInfo *root, Ants solution,int sizeProblem);
FILE*f,*f1;
RelOptInfo *
ants(PlannerInfo *root, int number_of_rels, List *initial_rels)
//(double **matrix,int sizeProblem,double constEvaporation,int cantAnts,int cantCicle, int alpha, int beta)
{
	PrivateData private;
    int nodeInitial;
    struct timeb init,ends;
    RelOptInfo *best_rel;
    Ants arrayAnts[CANTANTS];
    int i,j,w,x,state,iteration,cant,dif;
    Ants bestLocalSolution;
    Ants globalSolution,local;
    Ants *newAnts,*tempA;
    double **matrix,*m;
    double costo,evaporation;
    int * tmp;
    matrix=malloc(sizeof(double)*number_of_rels*number_of_rels);

    evaporation=0.6;
	root->join_search_private = (void *) &private;
	private.initial_rels = initial_rels;
	f=fopen("/home/boix/registro","a");f1=fopen("/home/boix/ants","w");
	tmp=palloc(2*sizeof(int));
	cant=CANTANTS;
	bestLocalSolution=init_Ants(0);
	globalSolution=bestLocalSolution;
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
        fprintf(f1,"%d  \n",i);
        for(w=0;w<cant;w++){

        	for(x=0;x<number_of_rels;x++){
        		fprintf(f1,"%d ",arrayAnts[w].tourMemory[x]);
        	}
        	fprintf(f1,"--->%If\n",arrayAnts[w].cost);
        }
        updatePheromone_Global(bestLocalSolution,number_of_rels,EVAP);
        calculate_PheromoneMaxMin(bestLocalSolution.cost,number_of_rels,EVAP);
        regulate_PheromoneMaxMin(number_of_rels);

	}
    local=globalSolution;
	for (w = 0; w < 20; w++){
		local = localSerch2_opt(root, local, number_of_rels);
	}
	if (local.cost < globalSolution.cost) {
		fprintf(f1, "mejoria de %If a %If", globalSolution.cost, local.cost);
		globalSolution = local;
	}
	ftime(&ends);
	fprintf(f, "maxMin  %d, %If, t: %If,c: %d, a:%d", number_of_rels,
			globalSolution.cost,
			((1000.0 * ends.time + ends.millitm)
					- (1000.0 * init.time + init.millitm)), CANTCICLES,
			cant);

	fprintf(f1, "\nGlobal solution\n");
	for (x = 0; x < number_of_rels; x++) {
		fprintf(f1, "%d ", globalSolution.tourMemory[x]);
	}
	fprintf(f1, "--->%If\n", globalSolution.cost);

	best_rel = gimme_tree(root, globalSolution.tourMemory, number_of_rels);


	fclose(f);
	fclose(f1);
    free(matrixWeight);
    free(matrixPheromone);
    free(newAnts);

    return best_rel;
}
void
_PG_init(void)
{

	 join_search_hook = ants;
}
void
_PG_fini(void)
{

	 join_search_hook = NULL;
}
Ants localSerch2_opt(PlannerInfo *root, Ants solution,int sizeProblem)
{
    Ants r=solution;
    double costAux,costAux1;
	int flag = 1,i,j,h,temp,valor;

    int * aux = malloc(sizeof(int)*sizeProblem);
    int * aux1 = malloc(sizeof(int)*sizeProblem);

    for(i=0;i<sizeProblem;i++)
    {
    	aux1[i]=solution.tourMemory[i];
    	aux[i]=solution.tourMemory[i];
    }
    costAux=geqo_eval(root,aux,sizeProblem);
    costAux1= geqo_eval(root,aux1,sizeProblem);
    while (flag==1)
    {
        flag = 0;
        for (i = 0; i < sizeProblem; i++)
        {
            j = i + 2;
            while (j != i)
            {
                for (h = 0; h < sizeProblem; h++)
                {
                    aux1[h] = aux[h];
                }
                temp = aux1[(i + 1) % sizeProblem];
                aux1[(i + 1) % sizeProblem] = aux1[j % sizeProblem];
                aux1[j % sizeProblem] = temp;

                costAux1=geqo_eval(root,aux1,sizeProblem);
                if (costAux > costAux1 )
                {
                    aux = aux1;
                    costAux=costAux1;
                    flag = 1;
                }
                j = (j + 1) % sizeProblem;
            }
        }
    }

  r.tourMemory=aux;
  r.cost=costAux;
  return r;
}
