#include "stdafx.h"
#undef sqrt
#include <stdio.h>
#include <math.h>

double dbg_sqrt(double x)
{
	if(x<0)
	{
		_bstr_t s("");
		char s1[200];
		sprintf(s1,"sqrt(%f)", x);
		s=s+s1;
		if(0) MessageBox(0, s, "Sqrt error: ", MB_OK);
		return(0);
	}
	return(sqrt(x));
}
