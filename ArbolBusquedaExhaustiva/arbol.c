/*
 ============================================================================
 Name        : tree.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "arbol.h"
#include <math.h>

int* newVal(int*, int, int);
int res(int*, int);
int main();

int* bestPath;
int best;

Node* createTree(int num_sons, int* values) {

	Node* tree = malloc(sizeof(Node));
	tree->num_sons = num_sons;
	tree->value = 0;
	tree->sons = NULL;
	insert(tree, values, num_sons);
	return tree;
}

void insert(Node* dad, int* values, int num_sons) {
	int i, val;
	Node* sons, *tmp;
	int* new_sons = malloc(sizeof(int) * (num_sons - 1));
	tmp = malloc(sizeof(Node) * (num_sons - 1));

	if (num_sons != 1) {
		sons = malloc(sizeof(Node) * num_sons);

		for (i = 0; i < num_sons; ++i) {
			sons[i].num_sons = num_sons - 1;
			sons[i].value = values[i];

			val = sons[i].value;

			sons[i].sons = NULL;
			new_sons = newVal(values, values[i], num_sons);
			insert(&sons[i], new_sons, num_sons - 1);
		}

		dad->sons = sons;

	} else if (num_sons == 1) {
		sons = malloc(sizeof(Node));
		sons->num_sons = 0;
		sons->value = values[0];
		val = sons->value;
		sons->sons = NULL;
		dad->sons = sons;
	}
	free(new_sons);
	free(tmp);
}

int* newVal(int* values, int val, int lenght) {
	int i, j;
	int* new = malloc(sizeof(int) * (lenght - 1));
	for (i = 0, j = 0; i < lenght; ++i) {
		if (values[i] != val) {
			new[j] = values[i];
			j++;
		}
	}
	return new;
}

int res(int* values, int lenght) {
	int i;
	int s = values[0];
	for (i = 1; i < lenght; ++i) {
		s -= values[i];
	}
	return s;
}

void search(Node* root, int lenght) {
	int level, i;
	Node* node, *father;

	FILE *f;
	Node** path = malloc(sizeof(Node*) * lenght);
	int* int_path = malloc(sizeof(int) * lenght);
	int* num_brothers = malloc(sizeof(int) * lenght);
	int* index = malloc(sizeof(int) * lenght);
	int nroNode, nroTotal;

	//sons of root, unique case

	path[0] = root->sons;
	num_brothers[0] = root->num_sons;
	index[0] = 0;
	f = fopen("file", "w");
	//next step
	node = root->sons;
	father = root;
	level=-1;//para que al salir del while de llegar a la hoja no se quede level con un valor de mas

	nroNode=index[0];
	nroTotal=num_brothers[0];
	int ok=0;
	int zz=0;
	while(index[0]<num_brothers[0]){
			while (node != NULL) {
				zz++;//para saber si es la primera vez que se entra a este while en cada iteracion del while de mas afuera
				level++;
				path[level] = node;
				if(ok==0)//solo hace falta la primera iteracion
					num_brothers[level] = father->num_sons;
				if(zz==1)
					index[level] = nroNode;
				else
					index[level] = 0;
				node = path[level]->sons;
				father = path[level];
			}
			//generar camino
			for (i = 0; i < lenght; ++i) {
				int_path[i] = path[i]->value;
				printf("%d ", int_path[i]);
			}
			printf("\n");
			//compara con best
			if (res(int_path, lenght) < best) {
				best = res(int_path, lenght);
				for (i = 0; i < lenght; ++i) {
					bestPath[i] = int_path[i];
				}
			}
			index[level]++;
			while(index[level] == num_brothers[level] && level > 0)
			{
				level--;
				index[level]++;
			}
			if(level==0)
				node=&root->sons[index[level]];
			else
				node=&path[level-1]->sons[index[level]];
			nroNode=index[level];
			zz=0;

			level--;

		ok++;
	}
	fclose(f);
}



int main(void) {
	int i;
	int a[4] = { 1, 2, 3, 4 };
	best = -99999999999;
	bestPath = malloc(sizeof(int) * 3);
	Node * tree=malloc(sizeof(Node*));
	tree = createTree(4, a);
	search(tree, 4);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
