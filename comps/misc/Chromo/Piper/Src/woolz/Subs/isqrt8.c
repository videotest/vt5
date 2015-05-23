/*
 * isqrt8.c
 *
 * return 8*sqrt(x) as an integer
 *
 *	Modifications
 *
 *	 4 Nov 1987		CAS		Trap very large (go -ve) and 0 requests
 */
isqrt8(x)
{
	register int r0,r1,d;
	register int count=0;
	r0 = x;
	x <<= 6;	/* 64*x, to get 8* factor on output */
	if (x <= 0) return(0);
	do {
		r1 = (x/r0 + r0) >> 1;
		d = r1-r0;
		if (d<0)
			d = -d;
		r0 = r1;
		if (d <= 1)
			count++;
	} while (d > 1 || count == 0);
	return(r1);
}
