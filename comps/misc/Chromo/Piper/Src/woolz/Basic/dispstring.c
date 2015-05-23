/*
 * dispstring.c	Jim Piper	10 August 1983
 *
 * Modifications
 *
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

dispstring(s,x,y)
char *s;
{
	moveto(x,y);
	text(s);
}
