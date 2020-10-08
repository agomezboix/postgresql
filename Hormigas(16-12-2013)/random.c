#include "Library.h"

//devuelve un número aleatorio
/*double get_Random()
{
	return rand();
}*/

void set_Seed()
{
	srand((unsigned)time(NULL));
}
/*double geqo_rand()
{
	return pg_erand48(random_state);
}

void geqo_set_seed( double seed)
{
     random_state= malloc(sizeof(unsigned short)*3);

	 // XXX. This seeding algorithm could certainly be improved - but it is not
	 // critical to do so.

	memset(random_state, 0, sizeof(random_state));
	memcpy(random_state, &seed, Min(sizeof(random_state), sizeof(seed)));
}*/
