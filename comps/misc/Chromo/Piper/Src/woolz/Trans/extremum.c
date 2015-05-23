/*
 * extremum.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <seqpar.h>

extern struct sppar sppar;



/*
 * extremum filter.
 * Kramer and Bruckner, Pattern Recognition 7, 1975, p53.
 * Lester, Brenner and Selles, C.G.I.P. 13, 1980, p17.
 */
int extremum (size)
{
	register int minneighbour,maxneighbour;
	register GREY *p;
	int original;
	register int j;
	int i;

	original = minneighbour = maxneighbour = *sppar.adrptr[0];
	switch (size) {
	case 0:
		for (i= -1; i<=1; i+=2) {
			p = sppar.adrptr[i];
			if (*p > maxneighbour)
				maxneighbour = *p;
			else if (*p < minneighbour)
				minneighbour = *p;
		}
		p = sppar.adrptr[0] - size;
		for (j= -size; j<= size; j++) {
			if (*p > maxneighbour)
				maxneighbour = *p;
			else if (*p < minneighbour)
				minneighbour = *p;
			p++;
		}
		break;
	default:
		for (i= -size; i<= size; i++) {
			p = sppar.adrptr[i] - size;
			for (j= -size; j<= size; j++) {
				if (*p > maxneighbour)
					maxneighbour = *p;
				else if (*p < minneighbour)
					minneighbour = *p;
				p++;
			}
		}
		break;
	}
	original = maxneighbour+minneighbour - 2*original;
	if (original >= 0)
		return(minneighbour);
	else
		return(maxneighbour);
}
