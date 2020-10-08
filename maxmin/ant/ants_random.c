/*------------------------------------------------------------------------
 *
 * geqo_random.c
 *	   random number generator
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/backend/optimizer/geqo/geqo_random.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "ants_random.h"


void
ants_set_seed(PlannerInfo *root, double seed)
{
	PrivateData *private = (PrivateData *) root->join_search_private;

	/*
	 * XXX. This seeding algorithm could certainly be improved - but it is not
	 * critical to do so.
	 */
	memset(private->random_state, 0, sizeof(private->random_state));
	memcpy(private->random_state,
		   &seed,
		   Min(sizeof(private->random_state), sizeof(seed)));
}

double
ants_rand(PlannerInfo *root)
{
	PrivateData *private = (PrivateData *) root->join_search_private;

	return pg_erand48(private->random_state);
}
