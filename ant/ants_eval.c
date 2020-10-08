/*------------------------------------------------------------------------
 *
 *luate query trees
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/backend/optimizer
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

#include "postgres.h"

#include <float.h>
#include <limits.h>
#include <math.h>

#include "optimizer/ant.h"
#include "optimizer/geqo.h"
#include "optimizer/joininfo.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"
#include "utils/memutils.h"


/* A "clump" of already-joined relations within gimme_tree */
typedef struct
{
	RelOptInfo *joinrel;		/* joinrel for the set of relations */
	int			size;			/* number of input relations in clump */
} Clump;

static List *merge_clump(PlannerInfo *root, List *clumps, Clump *new_clump,
			bool force);
static bool desirable_join(PlannerInfo *root,
			   RelOptInfo *outer_rel, RelOptInfo *inner_rel);


/*
 * a
 *
 * Returns cost of a query tree as an individual of the population.
 */
Cost
ant_eval(PlannerInfo *root, int *tour, int num_gene)
{
	MemoryContext mycontext;
	MemoryContext oldcxt;
	RelOptInfo *joinrel;
	Cost		fitness;
	int			savelength;
	struct HTAB *savehash;

	/*
	 * Create a private memory context that will hold all temp storage
	 * allocated inside gimme_tree().
	 *
	lled many times, we can't afford to let all
	 * that memory go unreclaimed until end of statement.  Note we make the
	 * temp context a child of the planner's normal context, so that it will
	 * be freed even if we abort via ereport(ERROR).
	 */
	mycontext = AllocSetContextCreate(CurrentMemoryContext,
									  "GEQO",
									  ALLOCSET_DEFAULT_MINSIZE,
									  ALLOCSET_DEFAULT_INITSIZE,
									  ALLOCSET_DEFAULT_MAXSIZE);
	oldcxt = MemoryContextSwitchTo(mycontext);

	/*
	 * gimme_tree will add entries to root->join_rel_list, which may or may
	 * not already contain some entries.  The newly added entries will be
	 * recycled by the MemoryContextDelete below, so we must ensure that the
	 * list is restored to its former state before exiting.  We can do this by
	 * truncating the list to its original length.	NOTE this assumes that any
	 * added entries are appended at the end!
	 *
	 * We also must take care not to mess up the outer join_rel_hash, if there
	 * is one.	We can do this by just temporarily setting the link to NULL.
	 * (If we are dealing with enough join rels, which we very likely are, a
	 * new hash table will get built and used locally.)
	 *
	 * join_rel_level[] shouldn't be in use, so just Assert it isn't.
	 */
	savelength = list_length(root->join_rel_list);
	savehash = root->join_rel_hash;
	Assert(root->join_rel_level == NULL);

	root->join_rel_hash = NULL;

	/* construct the best path for the given combination of relations */
	joinrel = gimme_tree(root, tour, num_gene);

	/*
	 * compute fitness
	 *
	y support optimization for partial result
	 * retrieval --- how to fix?
	 */
	fitness = joinrel->cheapest_total_path->total_cost;

	/*
	 * Restore join_rel_list to its former state, and put back original
	 * hashtable if any.
	 */
	root->join_rel_list = list_truncate(root->join_rel_list,
										savelength);
	root->join_rel_hash = savehash;

	/* release all the memory acquired within gimme_tree */
	MemoryContextSwitchTo(oldcxt);
	MemoryContextDelete(mycontext);

	return fitness;
}

/*
 * gimme_tree
 *	  Form planner estimates for a join tree constructed in the specified
 *	  order.
 *
 *	 'tour' is the proposed join order, of length 'num_gene'
 *
 * Returns a new join relation whose cheapest path is the best plan for
 * this join order.
 *
 * The original implementation of this routine always joined in the specified
 * order, and so could only build left-sided plans (and right-sided and
 * mixtures, as a byproduct of the fact that make_join_rel() is symmetric).
 * It could never produce a "bushy" plan.  This had a couple of big problems,
 * of which the worst was that there are situations involving join order
 * restrictions where the only valid plans are bushy.
 *
 * The present implementation takes the given tour as a guideline, but
 * postpones joins that are illegal or seem unsuitable according to some
 * heuristic rules.  This allows correct bushy plans to be generated at need,
 * and as a nice side-effect it seems to materially improve the quality of the
 * generated plans.
 */


/*
 * Merge a "clump" into the list of existing clumps for gimme_tree.
 *
 * We try to merge the clump into some existing clump, and repeat if
 * successful.	When no more merging is possible, insert the clump
 * into the list, preserving the list ordering rule (namely, that
 * clumps of larger size appear earlier).
 *
 * If force is true, merge anywhere a join is legal, even if it causes
 * a cartesian join to be performed.  When force is false, do only
 * "desirable" joins.
 */
static List *
merge_clump(PlannerInfo *root, List *clumps, Clump *new_clump, bool force)
{
	ListCell   *prev;
	ListCell   *lc;

	/* Look for a clump that new_clump can join to */
	prev = NULL;
	foreach(lc, clumps)
	{
		Clump	   *old_clump = (Clump *) lfirst(lc);

		if (force ||
			desirable_join(root, old_clump->joinrel, new_clump->joinrel))
		{
			RelOptInfo *joinrel;

			/*
			 * Construct a RelOptInfo representing the join of these two input
			 * relations.  Note that we expect the joinrel not to exist in
			 * root->join_rel_list yet, and so the paths constructed for it
			 * will only include the ones we want.
			 */
			joinrel = make_join_rel(root,
									old_clump->joinrel,
									new_clump->joinrel);

			/* Keep searching if join order is not valid */
			if (joinrel)
			{
				/* Find and save the cheapest paths for this joinrel */
				set_cheapest(joinrel);

				/* Absorb new clump into old */
				old_clump->joinrel = joinrel;
				old_clump->size += new_clump->size;
				pfree(new_clump);

				/* Remove old_clump from list */
				clumps = list_delete_cell(clumps, lc, prev);

				/*
				 * Recursively try to merge the enlarged old_clump with
				 * others.	When no further merge is possible, we'll reinsert
				 * it into the list.
				 */
				return merge_clump(root, clumps, old_clump, force);
			}
		}
		prev = lc;
	}

	/*
	 * No merging is possible, so add new_clump as an independent clump, in
	 * proper order according to size.	We can be fast for the common case
	 * where it has size 1 --- it should always go at the end.
	 */
	if (clumps == NIL || new_clump->size == 1)
		return lappend(clumps, new_clump);

	/* Check if it belongs at the front */
	lc = list_head(clumps);
	if (new_clump->size > ((Clump *) lfirst(lc))->size)
		return lcons(new_clump, clumps);

	/* Else search for the place to insert it */
	for (;;)
	{
		ListCell   *nxt = lnext(lc);

		if (nxt == NULL || new_clump->size > ((Clump *) lfirst(nxt))->size)
			break;				/* it belongs after 'lc', before 'nxt' */
		lc = nxt;
	}
	lappend_cell(clumps, lc, new_clump);

	return clumps;
}

/*
 * Heuristics for gimme_tree: do we want to join these two relations?
 */
static bool
desirable_join(PlannerInfo *root,
			   RelOptInfo *outer_rel, RelOptInfo *inner_rel)
{
	/*
	 * Join if there is an applicable join clause, or if there is a join order
	 * restriction forcing these rels to be joined.
	 */
	if (have_relevant_joinclause(root, outer_rel, inner_rel) ||
		have_join_order_restriction(root, outer_rel, inner_rel))
		return true;

	/* Otherwise postpone the join till later. */
	return false;
}
