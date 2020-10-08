#include "Library.h"

double geqo_rand()
{
    short unsigned int v=10;

 	return pg_erand48(&v);
}

int geqo_randint(int upper, int lower)
{

    // srand(time(NULL));
	return ( (int) ( geqo_rand()*(((upper)-(lower))+0.999999) ) + (lower));
//	return lower+(int)(((upper-lower+1.0)*rand())/(RAND_MAX+1.0));
}


/*void geqo_set_seed( double seed)
{
     random_state= malloc(sizeof(unsigned short)*3);
	/*
	 * XXX. This seeding algorithm could certainly be improved - but it is not
	 * critical to do so.
	 *//*
	memset(random_state, 0, sizeof(random_state));
	memcpy(random_state, &seed, Min(sizeof(random_state), sizeof(seed)));
}*/
