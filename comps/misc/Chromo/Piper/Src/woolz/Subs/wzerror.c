/*
 *
 *	P R E R R O R . C --
 *
 *  Written: B. Pennington.
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    	   November 1987.
 *
 *	Purpose:	   Error reporting function for prcheck program.
 *
 *	Description:	Contains a table of error constants and text strings  
 *					describing a wtcheck generated error. no string		  
 *					parameters are supported							  
 *																		  
 *
 *	Modifications:														  
 *																		  
 * 21 Nov 1988		dcb		Changed prerror() to woolz_err_msg(), and made the
 *							function return the error string. Also added errors
 *							VDMNULL.
 * 21 Nov 1988		dcb		Moved woolz_fatal() to this module
 * 19 Nov 1987		BDP		Added new error messages for space checks	  
 * 16 Nov 1987		BDP		Added chord length and Alloc_size errors	  
 *																		  
 */

#include <stdio.h>
#include <wstruct.h>
#include <wtcheck.h>

struct sterr {
	int	no;
	char *str;
	};
	
static struct sterr err[] = {
ALCNULL,		"NULL memory pointer detected",
ALCEMAGICNO,	"Bad memory block magic number",
ALCEBLKFREE,	"Memory block already freed",
ALCECKSUM,		"Memory block header check sum",
CKNULL,			"NULL object detected",
CKNULLDMN,		"NULL domain detected",
CKINVTYPE,		"Invalid object type",
CKEMPTYDMN,		"Empty domain detected",
IDMNULL,		"Interval domain NULL",
IDMETYPE,		"Interval domain type",
IDMESIZE,		"Interval domain allocated size exceeded",
IDMEILPTR,		"Interval line pointer incorrect",
IDMEIVSIZE,		"More interval than space allocated",
IDMEIVPTR,		"Gap found in intervals",
IDMEIVCOORD,	"Interval coordinates out of range",
IDMELC,			"Interval domain linkcount out of range",
IDMECOORD,		"Interval domain coordinates out of range",
IDMEFPTR,		"Interval lines outside idom reserved space",
IDMEFSIZE,		"Interval pointer outsize reserved range",
VDMNULL,		"Value domain NULL",
VDMETYPE,		"Value domain type",
VDMELC,			"Value domain linkcount out of range",
VDMEAOBJ,		"Unrecognised associated object",
VDMSFPTR,		"Std. Value domain freeptr does not equal first grey pointer",
VDMSVPTR,		"Std. Value domain grey pointer outside reserved space",
VDMSCOORD,		"Std. Value domain coordinates out of range",
VDMSIVCOORD,	"Std. Value domain interval coordinates out of range",
VDMSSIZE,		"Std. Value domain space exceeded",
VDMRFPTR,		"Rect. Value domain freeptr does not equal grey pointer",
VDMRCOORD,		"Rect. Value domain coordinates out of range",
VDMRCIDM,		"Rect. Value domain does not cover interval domain",
VDMRFSIZE,		"Rect. Value domain grey space exceeded",
VDMISIZE,		"Int. Value domain allocated size exceeded",
VDMICIDM,		"Int. Value domain does not cover interval domain",
VDMIILPTR,		"Int. Value domain interval line pointer incorrect",
VDMIIVPTR,		"Int. Value domain interval pointer incorrect",
VDMIFPTR,		"Int. Value domain freeptr does not equal first grey pointer",
VDMICOORD,		"Int. Value domain coordinates out of range",
VDMIGSIZE,		"Int. Value domain grey pointer outside reserved space",
VDMILC,			"Int. Value domain linkcount out of range",
VDMIIVCOORD,	"Int. Value domain interval coordinates out of range",
PDMNULL,		"Polygon domain NULL",
PDMETYPE,		"Polygon domain type",
PDMECNT,		"Polygon domain count",
PDMEVCNT,		"Polygon vertex count out of range",
PDMECOORD,		"Polygon coordinates out of range",
PDMESIZE,		"Polygon vertices outsize reserved space",
BDLEVPTR,		"Boundlist uppermost up pointer not NULL",
BDLECHN,		"Boundlist chain",
BDLEPOLY,		"Boundlist polygon",
BDLETYPE,		"Boundlist type",
BDLEDPTR,		"Boundlist object contains extraneous pointers",
CVDECOORD,		"Convex hull coordinates out of range",
CVDECNT	,		"Convex hull count",
CVDECCNT,		"Convex hull chord count out of range",
CVDECCOORD,		"Convex hull chord coordinate out of range",
CVDEBAREA,		"Convex hull bulge area out of range",
CVDECLEN,		"Convex hull chord length out of range",
CVDNULL,		"Convex hull domain NULL",
CVDESIZE,		"Convex hull chord outside reserved space",
HDMETYPE,		"Histogram type",			
HDMER,			"Histogram illegal r value",
HDMECOORD,		"Histogram origin coordinates out of range",
HDMECNT,		"Histogram number of points out of range",
HDMNULL,		"Histogram domain NULL",
HDMEDNULL,		"Histogram object contains extraneous pointers",
HDMESIZE,		"Histogram allocated size exceeded",
RDMETYPE,		"Rectangle type",		
RDMECOORD,		"Rectangle domain corners out of range",
RDMNULL,		"Rectangle domain NULL",
TDMEASC,		"Text object contains non-ascii characters",	
TDMECOORD,		"Text domain origin coordinates out of range",
TDMECNT,		"Text domain character count out of range",
TDMNULL,		"Text domain NULL",
TDMECNULL,		"Text domain contains NULL string with non-zero count",
PRFESCL,		"Picture frame scale out of range",			
PRFEDCOORD,		"Picture frame DDGS coordinates out of range",
PRFEOCOORD,		"Picture frame object origin coordinates out of range",
HDREDPTR,		"Header object contains extraneous pointers"
	};
		
/*
 *	W O O L Z _ E R R _ M S G
 *	Function:	woolz_err_msg()
 *	Purpose:	Report to stderr on error found in current object	  
 *	Description: Searches the error table for the supplied error number, and
 *				when found, will return the associated string. As the data
 *				structure is independent of the numeric value of its constant
 *				it is not valid to assume that the table is in numeric order,
 *				although this is the case at its inception, and hence the 
 *				numeric value cannot be searched for with a binary chop method.
 *				Thus the table is just scanned top to bottom. Having the most
 *				common types at the top	of the table will speed the search up.
 *				This fn. modified to return error string (dcb)
 *																		  
 *	Entry:		Error constant										  
 *																		  
 *	Exit:		Returns error string corresponding to error number, or
 *				"Check: OK", when called with zero error number, or
 *				"Check: Error undefined", when called with an undefined value
 *																		  
 */
char *
woolz_err_msg(errno)
int errno;
{
	register int i,count;

	if ( errno == 0) {
		return("Check: OK");
	}
	
	if ( (errno < WOOLZ_ERROR_MIN) || (errno > WOOLZ_ERROR_MAX) ) {
		return("Check: Error undefined");
	} else {
		count = sizeof(err)/sizeof(struct sterr);
		for( i = 0; i < count; i++) {
			if ( err[i].no == errno ) return(err[i].str);
		}
	}
} /* end woolz_err_msg() */

/*
 *	W O O L Z _ F A T A L
 *
 *	Purpose: Returns 0 if error number is not fatal
 *			 Returns 1 if error no. is fatal 
 */
woolz_fatal(errno)
int errno;
{
	return(0); /* All errors non-fatal for the moment */
} /* end woolz_fatal() */


