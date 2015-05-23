/*
 * interp.c
 *
 * linear interpolation between 4 points
 */
#include <stdio.h>
#include "wstruct.h"


GREY interp(g4,fractline,fractkol)
register GREY *g4;
register fractline,fractkol;
{
	register mfractline, mfractkol;
	GREY g;
	mfractline = 256 - fractline;
	mfractkol = 256 - fractkol;
	g = (g4[0] * mfractline*mfractkol
	   + g4[1] * mfractline*fractkol
	   + g4[2] * fractline*mfractkol
	   + g4[3] * fractline*fractkol
	   + 32767) / 65536;
	return(g);
}
