/*
 * seqpar.c	Jim Piper	23/12/82
 * Sequential or local transformation of an object,
 * derived from Fortran version by Marshall Presser
 *
 * There are some simplifications : notably, it is assumed that the
 * C versions of nxxint, nxxiv, etc, work according to a raster
 * direction given to the initialising routine.
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	27 Mar 1987		jimp	Registers
 *	11 May 1987		BDP		protection against null or empty objs
 *	30 Sep 1986		CAS		Change calloc to Calloc
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>
#include <seqpar.h>

struct sppar sppar;		/* define "extern" elsewhere */

struct object *
seqpar(iobj,notovr,seqent,raster,brdrsz,backgr,sparam,extsub)

struct object *iobj;
int (*extsub)();
int notovr,seqent,raster,brdrsz,backgr,sparam;
{

/*
subroutine to perform a sequential or parallel local transform.
Returns pointer to transformed object (which may have overwritten original
or not, depending on notovr, see below).
		
parameters:
	
iobj:on input is adrs of object to be transformed
on output is adrs of transformed object-see notovr values
		
notovr:
if zero, object overwritten in core
if non-zero, new object created in core.
		
seqent:
if true, transform is seqential and transformed values
will be used in the calculation of its neighbors values
if false, the transform always works on the original grey values
a distance transform is an example of a sequential transform while
the calculation of a 7-by-7 laplacian would be a parallel transform.
	
raster (as in greyscan):
(0) increasing columns within increasing lines
(1) decreasing columns within increasing lines
(2) increasing columns within decreasing lines
(3) decreasing columns within decreasing lines
		
brdrsz:
border size, must be either 0,1,2, or 3
the usual 8 immediate neighbors having brdrsz=1
	
backgr:
the grey value assumed for neighbors not in the picture
	
extsub:
the name of an external int function that calculates
the value of the transform(s).
extsub is passed sparam (below) as parameter.
		
sparam:
parameter to extsub (to permit multiple transformations
in one procedure)
		
if the point to be transformed is at line l and column k, there is
an array of pointers sppar.adrptr[-7:7], whose i'th entry gives the address
of the point (l+i*sppar.ldelta, k), but which is only meaningful
for lines within brdrsz of the point
e.g. sppar.adrptr[-3] and sppar.adrptr[3] are undefined if brdrsz<3.

Protection against null or empty objects added. For null object supplied then
null is returned, but for an empty objects, either a pointer to the supplied 
object is returned, or a copy of the original empty object is returned. This
is dependent upon the notovr parameter.
*/


	struct iwspace piou0,piou1;
	struct gwspace gva0,gva1;
	struct intervaldomain *jdp;
	struct propertylist *jpp;
	struct object *jobj,*isetmn();
	struct object *newgrey();
	register GREY *pbuff;
	register int i,itop,kol;
	int curlin,howmany,option;
	int brdrp1,numbufs,kol1,linesize,kofset,lofset;
	COORD *firstc,*lastco,*lastli,*itemp;
	int nextli,inline,needed,curline,jtemp,minline;
	/*
	curbuf is the current line buffer.
	llabuf is the array of line buffers needed to obtain neighbouring points.
	*/
	GREY *curbuf,*llabuf[15];
/*
 *	make an array sppar.adrptr of pointers with range [-7,7]
 */
	GREY *adrbase[15];
/*
 *	Perform the object checking
 */
	if (woolz_check_obj(iobj, "seqpar") != 0 )
		return(NULL);
	if (wzemptyidom(iobj->idom) > 0) {
		if (notovr)
			return(makemain(1, newidomain(iobj->idom) , NULL, NULL, NULL));
		else
			return(iobj);
	}

	sppar.adrptr = adrbase+7;
	sppar.brdrsz = brdrsz;

	jdp = iobj->idom;
	/*
	jdp is adrs of domain list-should be of type 1
	*/
	brdrp1= -brdrsz-1;
	/*
	the buffer must contain both the current line and brdrsz lines each side
	*/
	numbufs=brdrsz*2+1;
	/*
	linesize is the maximally sized line, including borders
	*/
	kol1 = kol = jdp->kol1;
	linesize = jdp->lastkl-kol+numbufs;
	/*
	some pointers to first/last line and column positions
	to accomodate different rasters
	*/
	firstc = &piou1.lftpos;
	lastco = &piou1.rgtpos;
	lastli = &jdp->lastln;
	/*
	sppar.kdelta is increment to adrs when processing points
	if positive, procede left to right in lines
	if negative procede right to left
	sppar.ldelta is increment to line number
	if positive, the raster is proceeding in increasing line numbers
	if negative the raster is procceding with decreasing line numbers
	*/
	sppar.kdelta=1;
	sppar.ldelta=1;
	/*
	alter parameters for right to left rasters
	*/
	if (!seqent)
		raster = 0;
	if(raster == 1 || raster == 3) {
		itemp=firstc;
		firstc=lastco;
		lastco=itemp;
		sppar.kdelta= -1;
	}
	/*
	alter parameters for bottom to top rasters
	*/
	if(raster > 1) {
		sppar.ldelta= -1;
		lastli = &jdp->line1;
	}
	/*
	kofset is the ofset of the column being transformed
	from the adrs given in the sppar.adrptr array.
	Apparently no longer required.
	kofset=brdrsz*sppar.kdelta;
	*/
	/*
	lofset is difference in line numbers between that being transformed
	and that needed in the input buffer for use in the transform
	*/
	lofset=brdrsz*sppar.ldelta;
	/*
	allocate space to buffers
	*/
	curbuf = (GREY *) Calloc(linesize*(numbufs+1),sizeof(int));
	for (i=0; i<numbufs; i++)
		llabuf[i] = curbuf + (i+1) * linesize;
	/*
	create a new grey table object if required
	*/
	jobj=iobj;
	if (notovr != 0) {
		int marg1,marg2,packin,opt1,opt2,noplan;
		/*
		create new object if required
		*/
		jpp=iobj->plist;
		/*
		create new object with 
			SAME interval list and property list
			DIFFERENT but IDENTICAL grey table
				(correct subset if necessary)
		*/
		jobj = newgrey(iobj);
	}

	/*
	use nxxiv in transplant mode, placing and taking grey values
	from the line buffers
	piou0 is the workspace for entering values into the line buffers
	piou1 is the workspace for processing the intervals
	and replacing the transformed values into the grey table
	*/
	gva0.gvio = 1;		/* output (since we are specifying tranpl) */
	gva1.gvio = 0;		/* input */
	initgreyrasterscan(iobj,&piou0,&gva0,raster,1);
	initgreyrasterscan(jobj,&piou1,&gva1,raster,1);
	/*
	curline is the line currently being processed
	inline is the last line entered into the line buffers
	initialize it to an imaginary line just before the first border line
	*/
	nextinterval(&piou0);
	nextli=piou0.linpos;
	inline=piou0.linpos-sppar.ldelta*(brdrsz+1);

	/*
	process the next interval
	*/
	while (nextgreyinterval(&piou1) == 0){
		/*
		when processing curline, the line buffer must contain
		lines upto needed=curline+border width
		*/
		curlin=piou1.linpos;
		needed=curlin+lofset;
		/*
		next action depends which lines are aleady in the line buffers
		*/
		howmany=(needed-inline)*sppar.ldelta;
		if(howmany<0) {
			/*
			error if howmany<0:impossible to have input a line before needed
			*/
			fprintf(stderr,"seqpar error:inline%d :curline%d\n",inline,curline);
			return(NULL);
		}
		else if (howmany > 0) {
			/*
			if howmany > 0 we need to input some lines to input buffer
			routine to fill the input buffers used to determine the value of the transform
			needed=maximal(minimal)line needed to be filled into the buffer
			if backwards raster, needed=curline-brdrsz
			if forwards raster, needed=curline+brdrsz
			nextline=line number of the next interval to be filed by nxxiv
			*/
			for (;;) {
				if ((needed-nextli)*sppar.ldelta < 0 ) {
					/*
					next line needed less far away than next line available from nxxint
					fill required buffer lines with backgr and return
					minline=pmod(inline,numbufs), i.e. the remainder of inline when divided
					by numbufs. it thus can be used as the offset into the array of
					logical line addresses to determine which line in the
					circular buffer inline is being stored
					*/
					while (inline != needed) {
						inline+=sppar.ldelta;
						minline=pmod(inline,numbufs);
						bkgset(llabuf[minline],linesize,backgr);
					}
					break;
				}
				/*
				line(s) needed include some real lines, i.e. obtained from nxxiv
				first fill buffer with backgr
				*/
				while ( inline != nextli ) {
					inline+=sppar.ldelta;
					minline=pmod(inline,numbufs);
					bkgset(llabuf[minline],linesize,backgr);
				}
				/*
				nxxint interval to be put into buffer calculate adrs in buffer by adding
				leftcol to logical adrs stored in llabuf[minline]. then insert with nxxiv
				*/
				do {
					gva0.grintptr=llabuf[minline]+piou0.lftpos+brdrsz-kol1;
					greyinterval(&piou0);
					/*
					call nxxint to get next interval if in same line store in buffer
					*/
					
					if (nextinterval(&piou0) != 0) {
						/*
						when nxxint has finished all interval set a high value into
						nextline, so all future lines input will be dummy lines
						*/
						nextli=(*lastli)-brdrp1*sppar.ldelta;
						goto filled;
					}
				} while (piou0.nwlpos == 0);
				/*
				if next interval in new line,store new line number and
				jump to start of routine to see what sort of line buffer
				processing is required next
				*/
				nextli=piou0.linpos;
			}
		}
		/*
		line buffers filled for processing of this line either from above call
		or buffer filled for previous intervals processed on this line
		kol is the first point in the interval to be processed
		itop is the last point to be processed
		*/
	filled:
		kol= *firstc;
		itop= *lastco;
		/*
		first load sppar.adrptr with addresses of required neighbors
		*/
		i=brdrp1;
		while ( i < brdrsz ) {
			i++;
			jtemp=i*sppar.ldelta;
			sppar.adrptr[i]= llabuf[pmod((curlin+jtemp),numbufs)] + kol + brdrsz - kol1;
		}
		/*
		then set adrs where transformed value to be placed
		if parallel mode, adrs is in current line buffer
		if sequential mode, adrs is in input buffer
		gva1.grintptr is the location in the nxxiv workspace giving the location from
		which nxxiv will transplant values into the grey table
		*/
		if(seqent)
			pbuff=sppar.adrptr[0];
		else
			pbuff= &curbuf[brdrsz];
		gva1.grintptr=piou1.lftpos+pbuff-kol;
		/*
		procede thru interval,calling the transforming function
		at each point and storing the result where required
		*/
		kol -= sppar.kdelta;
		while ( kol != itop ) {
			kol += sppar.kdelta;
			*pbuff= (*extsub) (sparam);
			/*
			after each point, update the neighbor adrs and the output adrs
			*/
			i=brdrp1;
			while ( i < brdrsz ) {
				i++;
				sppar.adrptr[i] += sppar.kdelta;
			}
			pbuff += sppar.kdelta;
		}
		/*
		having finished this interval proceed to the next
		*/
	}
	Free(curbuf);
	return(jobj);
}






static
bkgset(a,l,backgr)
register int l,backgr;
register GREY *a;
{
	int i;
	for (i=0; i<l; i++)
		*a++ = backgr;
}

static
pmod(i,j)
register int i,j;
{
	i = i%j;
	if (i < 0)
		i += j;
	return(i);
}
