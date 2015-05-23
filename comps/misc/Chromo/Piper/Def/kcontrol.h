/*
 * kcontrol.h
 *
 * Structures for control of karyotyping-by-multiple processes
 * and in multiple processors on the Mizar.
 *
 *******************************************************************
 *		OVERALL CONCEPTS OF CONTROL OF SLAVES
 * There is a command word and a status word for each process.
 * The master process has a separate status.  Because it is possible
 * to have more than one slave process in each processor, the
 * processor number and OS9 process of each slave is stored explicitly.
 * It is not necessary for each slave process to be executing the same
 * function.
 *
 * Initialisation routines start the required slaves in the
 * desired processors.  Then each slave waits for a command in its
 * own command word, executes it (possibly taking account of what
 * the master and other slaves are doing simultaneously), and
 * signals its status in its own status word.  Meanwhile, the
 * master ensures synchronisation by periodically inspecting the
 * slave status words.
 *******************************************************************
 *		GENERAL PURPOSE OBJECT STRUCTURE LISTS
 * The general idea is that the master will set up a function code in
 * the "command" location, with a set of Woolz objects in one of the
 * object lists.  One of the "activity" lists indicates which of these
 * objects is to be processed, and subsequently records the start
 * and finish of processing of each object, and which processor
 * carried it out.  One of the other object lists can be used to
 * accumulate Woolz objects which are the result of processing.
 * The "status" word provides overall information on the total
 * execution of the function.  The slave process which carried out
 * the operation is stored in the activity list; it is then 
 * possible to arrange that a subsequent command will operate only
 * on objects set up prevously by this slave process, leading to
 * processing and space overhead savings..
 *******************************************************************
 * 		OBJECT STRUCTURE LISTS FOR KARYOTYPING
 * With the exception of "rol", all data structures referred to by
 * this list reside in and refer to the master processor's data space
 * and address map.  Feature-measured/rotated objects ("rol") will generally
 * be constructed in slave processes running both in master and
 * slave processors.  For the purposes of classification and karyogram
 * display, master processor copies will need to be made ("mol").
 *
 * This structure's pointer values refer to the master processor.
 * See relevant documentation for more details.
 *******************************************************************
 *
 * Modifications
 *
 *	10 Oct 1989		dcb			Reduce slave priority to 110
 *  14 Mar 1989		SEAJ		Added pointer to new structure enhancement.
 *   1 Mar 1989		SEAJ		Added new activity bit PROCESSED.
 *	21 Sep 1987		CAS			Added explode field
 *	 3 Sep 1987		CAS			Added kdispscale field
 *	19 Aug 1987		BDP			Added DDGS channel pointer stuff
 *	19 Aug 1987		jimp@IRS	Integrated MRC and IRS versions,
 *								took out MASTDERIVED bit again.
 *	05 Aug 1987		jimp		New defines for swapping identification sets
 *	31 Mar 1987		jimp		More defines for karyotype services
 *	30 Mar 1987		jimp		More defines for karyotype services
 *	17 Mar 1987		jimp		New defines for karyotype services
 *	12 feb 1987		CAS			Added MASTDERIVED bit to ACL
 *	 9 feb 1987		CAS			Added kol 'keeplist pointer'
 * JP	12-11-86	Renamed some unused slots for use by boundaries
 *					and convex hulls.  Took the opportunity to
 *					rename the classifier slot.
 * JP	12-12-86	Added INTERACTING bit to object status word
 *	10 Oct 1986		CAS			Added classifier pointer
 *	 6 Oct 1986		CAS			Added current_obj to help sort out
 *								screen tracking of objects with mice
 */

#define NSLAVES	8				/* the number of slave processes */
#include <ddgs.h>

struct enhancement {
	int white;					/* percentage white for htnormalise */
	int black;					/* percentage black for htnormalise */
	struct convolution *conv;
	int matrix[25];
};
struct kcontrol {
	char lock;					/* TAS lock byte for unique access */
	char FIP;					/* FIP or TV digitisation ? */
			/* ------ overall per-process command and status ------ */
	SMALL maxnumber;			/* current maximum number of objects */
	int command[NSLAVES];		/* per-slave command */
	int status[NSLAVES];		/* per-slave processing status */
	int mstatus;				/* master process status */
	int auxstat;				/* shared status, used for some commands */
	unsigned char processor[NSLAVES];	/* per-slave PROCESSOR (<<5) and OS9 PROCESS */
			/* ------ properties of master ------ */
	SMALL seat;					/* which seat in two-seat Cytoscan 110 */
	char *terminal;				/* control VDU port name */
	char *mouse;				/* mouse port name */
			/* ------ general purpose display parameters and frames ------ */
	SMALL colour;
	SMALL intens;
	int		current_obj;		/* used in mouse interaction */
	SMALL identify_set;			/* Which set of objects to be identified */
	struct seg_interact *SI;		/* segmentation control structure */
	struct pframe *f;			/* default display frame pointer */
	struct pframe *ivf;			/* list of INDIVIDUAL frames for objects */
	int		kdispscale;			/* ktype display scale */
	int		explode;			/* Use exploded frame */
/*
 *--------  Classifier bits  --------
 */
	struct classifier *C;		/* Pointer to classifier structure */
	struct classifier *OC;		/* Classifier data structure (obj type) */
/*
 *------ general purpose object and status lists ------
 */
	char *gool1;		/* general purpose pointers */
	char *gool2;
	char *gool3;
	char *gool4;
	char *grol1;		/* general purpose pointers */
	char *grol2;
	char *grol3;
	char *grol4;
	SMALL *gacl1;				/* general purpose activity status lists */
	SMALL *gacl2;
	SMALL *gacl3;
	SMALL *gacl4;
/*
 *------ particular lists for karyotyping ------
 */
	struct chromosome **ool;	/* original objects pointer */
	struct chromosome **rol;	/* returned objects pointer */
	struct chromosome **mol;	/* returned objects, master processor copies */
	struct chromosome **kol;	/* 'keeplist' orig mol pointers */
	struct object **bol;		/* "blob" (marker point) list */
	SMALL *acl;					/* feature measurement activity list */
/*
 *------ pointer to list of associated goodies structures ------
 */
	struct othings *oth;		/* object boundaries, hulls, etc. */
	struct enhancement *enh;		/* structure to contain varible enhancement stuff*/
	DDGS	*ddgschannel;		/* Information about current ddgs device */
};

/*
 * a structure to keep per-object goodies such as boundaries.
 * I am not entirely convinced about all of this but at least
 * planting the pointer above makes the kcontrol structure
 * a little bit future-proof
 */
struct othings {
	struct object *bound;
	struct object *hull;
	char procb;					/* which processor owns boundary */
	char proch;					/* which processor owns hull */
};

/*
 * slave-status-word bits for karyotyping control 
 */
#define SEGBEGIN	0000001
#define SEGEND		0000002
#define MEASBEGIN	0000004
#define MEASEND		0000010
#define NORMBEGIN	0000020
#define NORMEND		0000040
#define CLASSBEGIN	0000100
#define CLASSEND	0000200
#define KARYBEGIN	0000400
#define KARYEND		0001000

#define ABORT		0x80000000


/*
 * general purpose slave-status-word bits 
 */
#define CLEARBEGIN	0010000	/* m-p screen clear commenced */
#define CLEAREND	0020000	/* m-p screen clear completed */
#define MAXNCHANGE	0040000	/* maxnumber likely to be increasing */

/*
 * per-object activity bits 
 */
#define ACTIVE		00001	/* still of interest */
#define MSTART		00002	/* function started */
#define MEND		00004	/* function ended */
#define MASTCOPY	00010	/* master processor copy of returned object made */
#define PROCESSED   00020	/* set if slave hits object */
#define MPROC		00700	/* mask for processor in which object processed */
#define INTERACTING	01000	/* So don't start measurements - Jim P 17-10-86 */
/*
 * general purpose slave-status-word or per-object activity bits 
 */
#define FUNCBEGIN	02000	/* slave function commenced by slaves */
#define FUNCEND		04000	/* slave function finished by slaves */
#define	SLFREE		010000	/* slave has freed its copy of object after
				MASTCOPY set by master - Jim P 17-03-87 */
#define IDENTIFY1	020000	/* match to kcontrol "identify_set" */
#define IDENTIFY2	040000	/* match to kcontrol "identify_set" */


/*
 * auxiliary status word bits 
 */
#define ALL_CLEAR	0177777	/* first 8 bits for clear start, next 8 for end */

/*
 * slave function definitions
 */
#define CHROM_FEATURE_MEASURE	1
#define OBJ_FEATURE_MEASURE		2
#define SCREEN_CLEAR			3
#define COM_DISPLAY_LIST		4
#define INDV_DISPLAY_LIST		5
#define HULL_LIST				6
#define R_RECT_LIST				7
#define BOUND_LIST				8
#define CHROM_LIKL_MEASURE	    9
/*
 * process and display karyotyping service commands 
 */
#define K_STRAIGHTEN			10
#define	K_ENHANCE				11
#define K_NORMALISE				12
#define K_RECOVER				13	/* recover original karyotyped object */
#define K_ROTATE				14
#define K_ENLARGE				15
#define K_STR_POL				16	/* straighten about specified polygon */
/*
 *	Other commands
 */
#define SLAVE_DEBUG				17	/* Turn on/off debugging for slave */

/*
 * processor byte bits and shifts 
 */
#define P_SS	037		/* mask for OS9 process number */
#define P_OR	5		/* shift right by this to get processor */

/*
 * PROCESSING PRIORITIES:
 */
#define MPRIOR 140
#define SPRIOR 110

