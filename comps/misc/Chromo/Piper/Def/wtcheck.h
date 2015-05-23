/*
 *      W T C H E C K . H  --  woolz checker error constants
 *
 *
 *  Written: Bruce D Pinnington
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    5th November 1987
 *
 *	The following constants are those used by the woolz object check routine
 *	to identify a detected error, the routine prerror defines a series of
 *	corresponding error strings used for user error reporting. This approach
 *	keeps all these routines independent of the error number
 *
 * Modifications:
 *
 *	 1 Dec 1988		CAS		WOOLZSIG moved back to application
 *  21 Nov 1988		dcb		WOOLZ_ERROR_MIN, & WOOLZ_ERROR_MAX, for min &max nos
 *	18 Nov 1988		SCG		define BDLNULL
 * 	19 Nov 1987		BDP		New types in for extra memory pointer checks
 *	17 Nov 1987		BDP		Added new chord length check error and Alloc_size errors
 *
 */

char *woolz_err_msg();
/*
 * Error Number range
 */
#define WOOLZ_ERROR_MIN -4
#define WOOLZ_ERROR_MAX 140	
 
/*					Alloc_size errors									*/
/*					-----------------									*/
#define ALCNULL		-1		/* NULL block pointer supplied	*/
#define ALCEMAGICNO	-2		/* Corrupt magic number found	*/
#define ALCEBLKFREE -3		/* Block has already been freed */
#define ALCECKSUM	-4		/* Check sum error in header 	*/

/*					Object level errors									*/
/*					-------------------									*/
#define CKNULL		1		/* NULL object */
#define CKNULLDMN	2		/* NULL domain */
#define CKINVTYPE	3		/* Invalid object type */
#define CKEMPTYDMN	4		/* Empty domain(warning) */

/*					Interval Domain errors 								*/
/*					----------------------								*/
#define IDMNULL		10		/* Interval domain NULL */
#define IDMETYPE	11		/* Idom type should be 1 or 2 */
#define IDMESIZE	12		/* Total size of all parts should equal stored allocated size */
#define IDMEILPTR	13		/* Interval line ptr should point to next location */
#define IDMEIVSIZE	14		/* More intervals than space allocated */
#define IDMEIVPTR	15		/* Interval pointers should leave no gaps */
#define IDMEIVCOORD	16		/* Interval coordinates not sensible */
#define IDMELC		17		/* Negative or high link count found */
#define IDMECOORD	18		/* Coordinates of domain out of sensible range */
#define IDMEFSIZE	19		/* Interval pointer outside reserved space */
#define IDMEFPTR	20		/* Interval line ontside reserved range */


/*					Value Domain Errors									*/
/*					-------------------									*/
#define VDMETYPE	30		/* Type should be 1,11,21 */
#define VDMELC		31		/* Negative or high link count found */
#define VDMEAOBJ	32		/* Unrecognised associated object */
#define VDMNULL		33		/* NULL domain */

#define VDMSFPTR	40		/* First grey table pointer <> freeptr */
#define VDMSVSIZE	41		/* Value line pointers overlap valueline array */
#define VDMSVPTR	42		/* Grey pointer outside reserved range */
#define VDMSCOORD	43		/* Coordinates not sensible */
#define VDMSIVCOORD	44		/* Interval coordinates not sensible */
#define VDMSSIZE	45		/* Value line array exceeds allocated memory */

#define VDMRFPTR	50		/* Grey table pointer <> freeptr */
#define VDMRCOORD	51		/* Coordinate block values not sensible */
#define VDMRCIDM	52		/* Domain does not fully cover idomain */
#define VDMRFSIZE	53		/* Grey pointer outside reserved space */

#define VDMISIZE	60		/* Allocated size calculated bigger than recorded */
#define VDMICIDM	61		/* Vdom does not fully cover the idom */
#define VDMIILPTR	62		/* Interval line ptr should be next location */
#define VDMIIVPTR	63		/* Interval pointer should be to calculated addresss */
#define VDMIFPTR	64		/* Free pointer <> first grey pointer */
#define VDMICOORD	65		/* Coordinates not sensible */
#define VDMIGSIZE	66		/* Grey pointer out of reserved space range */
#define VDMILC		67		/* Linkcount not sensible */
#define VDMIIVCOORD 68		/* Coordinates of grey intervals not sensible */


/*					Polygon Domain errors 								*/
/*					---------------------								*/
#define PDMNULL		70		/* NULL polygon domain found */
#define PDMETYPE	71		/* Type should be 1 or 2 */
#define PDMECNT		72		/* vertex count > vertex limit */
#define PDMEVCNT	73		/* vertex count negative or too high */
#define PDMECOORD	74		/* Vertex coordinate not sensible */
#define PDMESIZE	75		/* Vertices outside reserved space */


/*					Boundary List errors								*/
/*					--------------------								*/
#define BDLEVPTR	80		/* Up boundlist pointer for top element not NULL */
#define BDLECHN		81		/* Link error found in bound list chain */
#define BDLEPOLY	82		/* Error found in boundlist polygon */
#define BDLETYPE	83		/* Type should be 0 or 1, piece or hole */
#define BDLEDPTR	84		/* Other domain pointers in obj should be NULL */
#define BDLNULL		85		/* NULL top boundary list */


/*					Convex Hull Domain errors							*/
/*					-------------------------							*/
#define CVDECOORD	90		/* Coordinates not sensible */
#define CVDECNT		91		/* SIG chord count > no of chords */
#define CVDECCNT	92		/* Chord count not sensible */
#define CVDECCOORD	93		/* Chord coordinate not sensible */
#define CVDEBAREA	94		/* Bulge area > bulge max * chord length */
#define CVDNULL		95		/* NULL domain encountered */
#define CVDECLEN	96		/* Chord length not sensible */
#define CVDESIZE	97		/* Chord outside reserved space */


/*					Histogram Domain errors								*/
/*					-----------------------								*/
#define HDMETYPE	100		/* Type should be 1 or 2 */
#define HDMER		101		/* R values should be 1 or 2 */
#define HDMECOORD	102		/* Origin coords not sensible */
#define HDMECNT		103		/* Number of points not sensible */
#define HDMNULL		104		/* Null histogram domain encountered */
#define HDMEDNULL	105		/* Plist and vdom fields should be NULL */
#define HDMESIZE	106		/* Allocated size has been exceeded */


/*					Rectangle Domain errors								*/
/*					-----------------------								*/
#define RDMETYPE	110		/* Type should be 1 or 2 */
#define RDMECOORD	111		/* One of more corners coordinates not sensible */
#define RDMNULL		112		/* NULL rect domain encountered */


/*					Text Domain errors									*/
/*					------------------									*/
#define TDMEASC		120		/* Character(s) out of ascii range found */
#define TDMECOORD	121		/* Text origin coordinates not sensible */
#define TDMECNT		122		/* Character count <0 or >1024	*/
#define TDMNULL		123		/* NULL text domain encountered */
#define TDMECNULL	124		/* NULL character pointer for non zero count found */


/*					Picture Frame errors								*/
/*					--------------------								*/
#define PRFESCL		130		/* Scale not in range 0 - 4096 */
#define PRFEDCOORD	131		/* DDGS coordinate range error */
#define PRFEOCOORD	132		/* Object origin coordinate range error */


/*					Header Record errors								*/
/*					--------------------								*/
#define HDREDPTR	140		/* One or more domain pointers found to be not NULL */
