#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <time.h>
#include "Library.h"



#include "fmgr.h"


PG_MODULE_MAGIC;

extern void _PG_init(void);
extern void _PG_fini(void);
void geqo_set_seed(PlannerInfo *root, double seed);
Ants localSerch2_opt(PlannerInfo *root, Ants solution,int sizeProblem);
Ants hillClimbing(PlannerInfo *root, Ants solution,int sizeProblem);
join_search_hook_type join_search_hook = NULL;

FILE*f,*f1;

RelOptInfo * executeACS2Pasos(PlannerInfo *root, int number_of_rels, List *initial_rels)
{
	int countState, k, nodeInitial, ini, end;
	int iteration;GeqoPrivateData private;
	Ants bestLocalSolution, newAnts,local;
	Ants globalSolution, partInitial;    struct timeb init,ends;
	Ants *arrayAnts;
	Ants *setAnts;    RelOptInfo *best_rel;
	int i,j,valueRandom,sizeSet,cantCicle,cantAnts;
	int countSetAnts=0;



	int * tmp,w,x;
	double alpha,beta,q0,pheromoneInicial,constEvaporation,**matrix;
	double r=0.3;
	alpha=5;
	f=fopen("/home/boix/registro","a");f1=fopen("/home/boix/ants","w");
	pheromoneInicial=0.1;
	beta=1;
	q0=0.25;
	constEvaporation=0.1;
	cantAnts=10;
	cantCicle=30;
	ftime(&init);
	//Values for Step One
	int cantCicleStepOne=cantCicle*r;
	int cantAntsStepOne=cantAnts*r+1;
	int sizeProblemStepOne=number_of_rels*r;

	//Values for Step Two
	int cantCicleStepTwo=cantCicle-cantCicleStepOne;
	int cantAntsStepTwo=cantAnts-cantAntsStepOne;
	int sizeProblemStepTwo=number_of_rels-sizeProblemStepOne;

	//double result;

	globalSolution.cost=99999999999;
	arrayAnts = (Ants *)malloc(sizeof(Ants)*cantAntsStepTwo);
	sizeSet=(cantCicleStepOne*cantAntsStepOne*0.99)+1;//50% de las soluciones de la primera etapa
	setAnts = (Ants *) malloc(sizeof(Ants) * sizeSet);
	iteration = 20;


	root->join_search_private = (void *) &private;
	private.initial_rels = initial_rels;



	tmp=palloc(2*sizeof(int));
	matrix = malloc(sizeof(double*) * number_of_rels);
	for (i = 0; i < number_of_rels; i++) {
		matrix[i] = malloc(sizeof(double) * number_of_rels);
	}

	for(i=0;i<number_of_rels;i++){
			for(j=0;j<number_of_rels;j++){
				tmp[0]=i+1;
				tmp[1]=j+1;
				if(i==j){
					matrix[i][j]=0;
				}
				else{
					matrix[i][j]=geqo_eval(root,tmp,2);
				}
			}
		}



	inicializeMatrixWeight(matrix,number_of_rels);
	inicializeMatrixPheromone(number_of_rels,pheromoneInicial);





	root->join_search_private = (void *) &private;
	private.initial_rels = initial_rels;
	geqo_set_seed(root, Geqo_seed);

	for (i = 1; i <= cantCicleStepOne; i++)
	{
		int j;
		for(j=0;j<cantAntsStepOne;j++)
		{
			//con geqo_randint los caminos de las hormigas tendran valores entre 1 y sizeProblem incluyendo ambos
			nodeInitial=geqo_randint(root,number_of_rels,1);

			newAnts=init_Ants(nodeInitial);
			arrayAnts[j] = newAnts;
		}
		countState=1;
		while(countState < sizeProblemStepOne)
		{
			for(k=0;k < cantAntsStepOne;k++)
			{
				setNextStep(&arrayAnts[k],number_of_rels,alpha,beta,q0);
			}
			countState++;
			for(k=0;k < cantAntsStepOne;k++)
			{
				ini=arrayAnts[k].tourMemory[countState-2]-1;
				end=arrayAnts[k].tourMemory[countState-1]-1;
				updatePheromone_Step_by_Step(ini, end, constEvaporation, pheromoneInicial);
			}
		}

		for(k=0;k < cantAntsStepOne;k++)
		{
			countSetAnts=insertInOrden(root,setAnts,sizeSet,arrayAnts[k],countSetAnts);
		}


		bestLocalSolution=searchBestSolution(root,arrayAnts,cantAntsStepOne,sizeProblemStepOne,1);
		updatePheromone_Global(bestLocalSolution,number_of_rels,constEvaporation,sizeProblemStepOne);
	}//fin de la primera etapa



	//segunda etapa
	for(j=0;j<cantAntsStepTwo;j++)
	{
		//con geqo_randint los caminos de las hormigas tendran valores entre 1 y sizeProblem incluyendo ambos
		valueRandom=geqo_randint(root,sizeSet-1,0);
		partInitial=setAnts[valueRandom];
		arrayAnts[j] = partInitial;
	}
    for (i = 1; i <= cantCicleStepTwo; i++)
    {
        /***************************************************/
        countState=0;
        while(countState < sizeProblemStepTwo)
        {
            for(k=0;k < cantAntsStepTwo;k++)
            {
            	bestLocalSolution=arrayAnts[k];//para debuguear
                setNextStep(&arrayAnts[k],number_of_rels,alpha,beta,q0);
            }
            countState++;
			for(k=0;k < cantAntsStepTwo;k++)
			{
				ini=arrayAnts[k].tourMemory[(sizeProblemStepOne+countState)-2]-1;
				end=arrayAnts[k].tourMemory[(sizeProblemStepOne+countState)-1]-1;
				updatePheromone_Step_by_Step(ini, end, constEvaporation, pheromoneInicial);
			}
        } 

        bestLocalSolution=searchBestSolution(root,arrayAnts,cantAntsStepTwo,number_of_rels,0);

		fprintf(f1, "%d  \n", i);

		for (w = 0; w < cantAntsStepTwo; w++) {

			for (x = 0; x < number_of_rels; x++) {
				fprintf(f1, "%d ", arrayAnts[w].tourMemory[x]);
			}
			fprintf(f1, "--->%If\n", arrayAnts[w].cost);
		}


		for (k = 0; k < cantAntsStepTwo; k++) {
			ini=arrayAnts[k].tourMemory[number_of_rels-1]-1;
			end=arrayAnts[k].tourMemory[0]-1;
			updatePheromone_Step_by_Step(ini, end, constEvaporation, pheromoneInicial);
			
			//bestAnts=arrayAnts[j];
			//inicializar las hormigas de nuevo para comenzar el proximo ciclo
			valueRandom=geqo_randint(root,sizeSet-1,0);
			partInitial=setAnts[valueRandom];
			arrayAnts[k] = partInitial;
			/*if(bestAnts.cost < globalAnts.cost)
			{
				globalAnts=bestAnts;
				iteration=i;
			}	*/
		}

		/*bestLocalSolution.tourMemory=localSerch2_opt(root,bestLocalSolution.tourMemory,number_of_rels);
		 bestLocalSolution.cost=geqo_eval(root,bestLocalSolution.tourMemory,number_of_rels);
		 bestLocalSolution.beginState=bestLocalSolution.tourMemory[0];*/

		if (globalSolution.cost > bestLocalSolution.cost)
			globalSolution = bestLocalSolution;


		updatePheromone_Global(globalSolution, number_of_rels, constEvaporation,
				sizeProblemStepTwo);
	}

	local = localSerch2_opt(root, globalSolution, number_of_rels);

	if (local.cost < globalSolution.cost) {
		fprintf(f1, "mejoria de %If a %If", globalSolution.cost, local.cost);
		globalSolution = local;
	}

	best_rel = gimme_tree(root, globalSolution.tourMemory, number_of_rels);
	ftime(&ends);

	fprintf(f, "ants2S  %d, %If, t: %If,c: %d, a:%d", number_of_rels,
			globalSolution.cost,
			((1000.0 * ends.time + ends.millitm)
					- (1000.0 * init.time + init.millitm)), cantCicle,
			cantAnts);

	fprintf(f1, "\nGlobal solution\n");
	for (x = 0; x < number_of_rels; x++) {
		fprintf(f1, "%d ", globalSolution.tourMemory[x]);
	}
	fprintf(f1, "--->%If\n", globalSolution.cost);
	fclose(f1);
	fclose(f);
	free(matrixWeight);
	free(matrixPheromone);
	//free(newAnts);free(bestAnts);free(globalAnts);free(bestLocalSolution);free(globalSolution);
	return best_rel;
}

void _PG_init(void) {
	join_search_hook = executeACS2Pasos;
}
void _PG_fini(void) {
	join_search_hook = NULL;
}

void
geqo_set_seed(PlannerInfo *root, double seed)
{
	GeqoPrivateData *private = (GeqoPrivateData *) root->join_search_private;

	/*
	 * XXX. This seeding algorithm could certainly be improved - but it is not
	 * critical to do so.
	 */
	memset(private->random_state, 0, sizeof(private->random_state));
	memcpy(private->random_state,
		   &seed,
		   Min(sizeof(private->random_state), sizeof(seed)));
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

Ants hillClimbing(PlannerInfo *root, Ants solution,int sizeProblem)
{

      int i,j,tmp;
      int * bestChild;
      double bestCost,cost;
      int levelTree=10;
      int * children;
      int * dad;
      Ants r=solution;

      bestCost=999999999999;
      children=malloc(sizeof(int)*(sizeProblem));
      bestChild=malloc(sizeof(int)*(sizeProblem));
      dad = malloc(sizeof(int)*sizeProblem);
      memcpy(dad,solution.tourMemory,sizeof(int)*sizeProblem);
      while (levelTree > 0)
      {
            for(i=0; i < sizeProblem-1;i++)
            {
            	memcpy(children,dad,sizeof(int)*sizeProblem);
            	tmp=children[i];
            	children[i]=children[i+1];
            	children[i+1]=tmp;
            	cost=geqo_eval(root,children,sizeProblem);
            	if(cost<bestCost){
            		memcpy(bestChild,children,sizeof(int)*sizeProblem);
            		bestCost=cost;
            	}
            }
            memcpy(dad,bestChild,sizeof(int)*sizeProblem);
            levelTree--;
      }
      if(bestCost<solution.cost) {
    	  r.tourMemory=bestChild;
    	  r.cost=bestCost;
      }
      return r;
}


