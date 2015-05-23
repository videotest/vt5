/*
 * dispnum.c	Jim Piper	10 August 1983
 *
 * Modifications
 *
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

dispnum(num,x,y)
{
	char s[10];
#ifdef C68000
	int l;
	l = decode(s,10,"%d",num);
	s[l] = '\0';
#else C68000
	sprintf(s,"%d",num);
#endif C68000
	dispstring(s,x,y);
}
