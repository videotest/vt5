/*
 * isqrt.c
 *
 * return sqrt(x) as an integer
 * 2 Dec 1988 dcb Fix /0 possibility
 */
isqrt(x)
{
	register int r0,r1,d;
	register int count=0;
	if (x<=1) return(x);
	r0 = x>>1;
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
