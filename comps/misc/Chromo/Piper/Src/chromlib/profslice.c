/*
 * profslice.c		Jim Piper		20/1/84
 *
 * Extract a slice from an object which is perpendicular to a
 * midpoints polygon, for profile construction.
 *
 * The parameters are
 * (1) the object address,
 * (2) a halfwidth to extend the slice to either side of the specified point,
 * (3) a GREY array to place the interpolated pixels which comprise the slice.
 * (4) a slope for the slice
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		remove Mizar defs -> osk
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>


profslice(obj,x2,y2,sin2,cos2,halfwidth,g)
struct object *obj;
int x2,y2,sin2,cos2;
register GREY *g;
{
	register int i;
	int xxx,yyy,xinc,yinc,xbar,ybar,wd1,wd2;
	int ix3,iy3,ixfrac,iyfrac;
	register int ixx,iyy,jxx,jyy;
	GREY g4[4];
	
	ix3= x2 >> 3;
	iy3= y2 >> 3;
	/*
	 * fractional remainder, out of 256
	 */
	ixfrac = (x2<<5) - (ix3<<8);
	iyfrac = (y2<<5) - (iy3<<8);
	/*
	 * everything will now be done centered round ix3,iy3.
	 */
	for (i= -halfwidth; i<=halfwidth; i++) {
		/*
		 * coordinates, minus ix3, iy3 respectively, *256
		 */
		xxx=ixfrac+i*cos2;
		yyy=iyfrac+i*sin2;
		ixx=xxx>>8;
		iyy=yyy>>8;
		xinc=xxx-(ixx<<8);
		if (xinc < 0) {
			xinc += 256;
			ixx--;
		}
		yinc=yyy-(iyy<<8);
		if (yinc < 0) {
			yinc += 256;
			iyy--;
		}
		/*
		 * xinc, xbar, yinc, ybar should now all be non-negative
		 */
		jxx=ix3+ixx;
		jyy=iy3+iyy;
		/*
		 * interpolated grey value
		 */
		grey4val(obj,jyy,jxx,g4);
#ifdef OSK
		*g++ = interp(g4,yinc,xinc);
#else OSK
		xbar=256-xinc;
		ybar=256-yinc;
		*g++ = (g4[0] * ybar*xbar + g4[1] * ybar*xinc
		     + g4[2] * yinc*xbar + g4[3] * yinc*xinc
		     + 32676) / 65536;
#endif OSK
	}
}
