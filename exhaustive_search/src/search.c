/*
 ============================================================================
 Name        : search.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int contains(int *array, int value, int length);
int next(int *array, int level, int value, int length);
int more(int * array, int length);

int main(void) {

	int length, i, tmp;
	int * values, *index, *res, level;
	length = 5;

	values = malloc(sizeof(int) * length);
	index = malloc(sizeof(int) * length);
	res = malloc(sizeof(int) * length);
	level = 0;

	for (i = 0; i < length; ++i) {
		values[i] = i + 1;
		index[i] = 0;
		res[i] = length - i;
	}

	while (1) {

		while (level < length) {
			index[level] = next(index, level, index[level], length);
			res[level]--;
			level++;
		}

		printf("\n");
		for (i = 0; i < length; ++i) {
			printf("%d ", index[i]);
		}
		printf("r");
		for (i = 0; i < length; ++i) {
			printf("%d ", res[i]);
		}
		printf("l%d ", level);
		printf("n%d ", next(index, level, index[level], length));

		while (level == length || res[level] == 0) {
			level--;
		}
		if (level == -1)
			break;

		index[level] = next(index, level + 1, index[level], length);
		res[level]--;
		for (i = level + 1; i < length; ++i) {
			res[i] = length - i;

		}
		level++;

		tmp--;
		if (tmp == 0)
			res[0] = tmp;
	}

	return EXIT_SUCCESS;
}

int next(int *array, int level, int value, int length) {
	int count = 0;
	int i = value;

	while (count < length && contains(array, i, level)) {
		count++;
		i++;
		if (i == length)
			i = 0;
	}
	return i;
}

int contains(int* array, int value, int length) {
	int i = 0;
	while (i < length && value != array[i]) {
		i++;
	}
	if (i < length)
		i = 1;
	else {
		i = 0;
	}
	return i;
}

int more(int * array, int length) {
	int i = 0;
	while (i < length && array[i] == 0) {
		i++;
	}
	if (i == length)
		i = 0;
	else {
		i = 1;
	}
	return i;
}
