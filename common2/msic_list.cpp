#include "stdafx.h"
#include "misc_list.h"


//delete operators
void FreeEmpty( void *ptr )
{}

void FreeReleaseUnknown( void *ptr )
{	IUnknown	*punk = (IUnknown	*)ptr;	if( punk )punk->Release();}

