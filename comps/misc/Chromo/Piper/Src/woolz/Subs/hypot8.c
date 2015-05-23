/*
 * hypot8.c		Jim Piper	4 September 1984
 *
 * Return 8 * sqrt(i*i + j*j) where i, j are integers by using
 * table look up for small values of i,j to avoid sqrt() calls.
 *
 * The coding of this routine could be varied if either i or j
 * is frequently zero, or if i==j frequently.
 */

#define MAXTABLU	16

unsigned char hlutab[(MAXTABLU+1) * (MAXTABLU+2) / 2] = {
  0,
  8, 11,
 16, 18, 23,
 24, 25, 29, 34,
 32, 33, 36, 40, 45,
 40, 41, 43, 47, 51, 57,
 48, 49, 51, 54, 58, 62, 68,
 56, 57, 58, 61, 64, 69, 74, 79,
 64, 64, 66, 68, 72, 75, 80, 85, 90,
 72, 72, 74, 76, 79, 82, 87, 91, 96,102,
 80, 80, 82, 84, 86, 89, 93, 98,102,108,113,
 88, 88, 89, 91, 94, 97,100,104,109,114,119,124,
 96, 96, 97, 99,101,104,107,111,115,120,125,130,136,
104,104,105,107,109,111,115,118,122,126,131,136,142,147,
112,112,113,115,116,119,122,125,129,133,138,142,148,153,158,
120,120,121,122,124,126,129,132,136,140,144,149,154,159,164,170,
128,128,129,130,132,134,137,140,143,147,151,155,160,165,170,175,181,
};

hypot8(i,j)
register i,j;
{
	register t;

	/* make arguments positive */
	if (i < 0)
		i = -i;
	if (j < 0)
		j = -j;
	/* get in right order */
	if (i > j) {
		t = i;
		i = j;
		j = t;
	}
	/* check whether can use table look-up */
	if (j > MAXTABLU)
		return(isqrt8(i*i + j*j));
	else
		return((int)hlutab[((j*(j+1)) >> 1) + i]);
}
