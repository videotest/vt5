/*
 * intcount.c		Jim Piper		14/11/84
 *
 * count the number of intervals in a type 1 interval domain
 *
 * Modifications
 *
 *	05 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>


intcount(idom)
struct intervaldomain *idom;
{
	register l,ll,intcount;
	register struct intervalline *intl;

	/* protection introduced here is just against null domain, which would
	return a zero count. An empty domain will be handled and return a zero
	count also. Initialisation of count, and method of value return has
	been rearranged to accomodate the above. bdp 5/5/87  */
	
	intcount = 0;
	if (idom != NULL)
	{
		switch (idom->type) {
		case 1:
			intl = idom->intvlines;
			ll = idom->lastln;
			for (l=idom->line1; l<=ll; l++) {
				intcount += intl->nintvs;
				intl++;
			}
			break;
		default:
		case 2:
			intcount = idom->lastln - idom->line1 + 1;
		}
	}
	return(intcount);
}
