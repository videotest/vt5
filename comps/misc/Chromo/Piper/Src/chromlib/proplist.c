/*
 * proplist.c
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>


struct propertylist * makechromplist()
{
	struct propertylist *p;
	p = (struct propertylist *) Calloc(sizeof(struct chromplist),1);
	p->size = sizeof(struct chromplist);
	return(p);
}
