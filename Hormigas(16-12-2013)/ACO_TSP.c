#include <stdio.h>
#include <stdlib.h>

//#include <fcntl.h>
//#include <unistd.h>

#include "Library.h"

//double **matrixWeight;
unsigned short *random_state;
double		Geqo_seed=0;//en Postgre es cero por defecto
int executeACO_As_TSP(FILE*,double **, int, double, int, int,int, int);
int * localSerch2_opt(int *,int);
void writeSolution(FILE*,int *,int, int);

int executeACO_As_TSP(FILE* fileOut,double **matrix,int sizeProblem,double constEvaporation,int cantAnts,int cantCicle, int alpha, int beta)
{
    int nodeInitial;
    int iteration;
    Ants arrayAnts[cantAnts];
    int i;
    int * bestLocalSolution;
    int * globalSolution=NULL;
    Ants *newAnts;
    //double matrixPheromone[sizeProblem][sizeProblem];
    //arrayAnts=malloc(sizeof(Ants)*cantAnts);
    inicializeMatrixPheromone(sizeProblem,constEvaporation);
    inicializeMatrixWeight(matrix,sizeProblem);
    //set_Seed();
    for (i = 1; i <= cantCicle; i++)
    {
        //int currentState;
          //Programación_de_actividades
          //Generación_de_Hormigas_y_actividad()
        int j;
        for(j=0;j<cantAnts;j++)
        {
            //Nueva_Hormiga(k)
            //nodeInitial=geqo_randint(0,sizeProblem - 1);//randint es de postgre
            //******************************************************************************lo que cambie*******************************************************************
            //nodeInitial=newRandint(sizeProblem,0);//este randint no incluye al upper

        	//con geqo_randint los caminos de las hormigas tendran valores entre 1 y sizeProblem incluyendo ambos
        	nodeInitial=geqo_randint(sizeProblem,1);

            newAnts=init_Ants(nodeInitial);
            arrayAnts[j] = *newAnts;
        }
        int state=1;
        while(state != sizeProblem)
        {
            int k;
            for(k=0;k<cantAnts;k++)
            {
                setNextStep(&arrayAnts[k],sizeProblem,alpha,beta);
            }
            state++;
        }
        // find a local solution
        bestLocalSolution=(int *)searchBestSolution(arrayAnts,cantAnts,sizeProblem);
        bestLocalSolution=localSerch2_opt(bestLocalSolution,sizeProblem);

        if(globalSolution==NULL)
        {
            globalSolution=bestLocalSolution;
            iteration=i;
        }
        else
        	//if(calculateCoste(bestLocalSolution,sizeProblem)<calculateCoste(globalSolution,sizeProblem))
        	if(calculateCoste_Circle(bestLocalSolution,sizeProblem)<calculateCoste_Circle(globalSolution,sizeProblem))
        	{
        		globalSolution=bestLocalSolution;
        		iteration=i;
        	}
        updatePheromone_Global(bestLocalSolution,sizeProblem,constEvaporation);
        calculate_PheromoneMaxMin(calculateCoste_Circle(bestLocalSolution,sizeProblem),sizeProblem,constEvaporation);
        regulate_PheromoneMaxMin(sizeProblem);
        /*Evaporacion_de_Feromona()
        Acciones_del_demonio() {opcional}
        fin Programación_de_actividades */
    }
    double c=calculateCoste_Circle(globalSolution,sizeProblem);
    printf("El costo de la mejor solucion es de:%f\n",c);
    writeSolution(fileOut,globalSolution,sizeProblem,iteration);
    free(matrixWeight);
    free(matrixPheromone);
    free(newAnts);
    free(bestLocalSolution);
    free(globalSolution);
    return iteration;
}

int * localSerch2_opt(int * solution,int sizeProblem)
{
    int flag = 1;
    int i;
    int j;
    int h;
    int temp;

    int valor;

    int * aux = malloc(sizeof(int)*sizeProblem);
    int * aux1 = malloc(sizeof(int)*sizeProblem);
    for(i=0;i<sizeProblem;i++)
    {
    	valor=solution[i];
    	aux[i]=solution[i];
    }
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

                if (calculateCoste_Circle(aux,sizeProblem) > calculateCoste_Circle(aux1,sizeProblem))
                {
                    aux = aux1;
                    flag = 1;
                }
                j = (j + 1) % sizeProblem;
            }
        }
    }
  return aux;
}

//escribe los resultados en el fichero de salida
void writeSolution(FILE* fileOut,int *globalSolution,int sizeProblem, int iteration)
{
    int i;
    fprintf(fileOut,"\n");
    fprintf(fileOut,"con costo %f\nnumero de iteraciones: %d\n",calculateCoste_Circle(globalSolution,sizeProblem),iteration);
    fprintf(fileOut,"El mejor camino es: ");
    for (i=0; i<sizeProblem; i++)
    {
        fprintf(fileOut,"%d, ",globalSolution[i]);
    }

}

/* Procedimiento Nueva_Hormiga(id_Hormiga)
2  inicializa_hormiga(id_Hormiga)
3  L = actualiza_memoria_hormiga()
4  mientras (estado_actual  š  estado_objetivo)
5    P = calcular_probabilidades_de_transición(A,L,W)
6    siguiente_estado = aplicar_política_decisión(P,W)
7    mover_al_siguiente_estado(siguiente_estado)
    si (actualizacion_feromona_en_linea_paso_a_paso)
8      depositar_feromona_en_el_arco_vistado()
     fin si
9    L = actualizar_estado_interno()
10  fin mientras
   si (actualizacion_feromona_en_linea_a_posteriori)
11    para cada arco visitado
12      depositar_feromona_en_el_arco_visitado()
13    fin para
   fin si
14  liberar_recursos_hormiga(id_Hormiga)
15 fin Procedimiento
*/
