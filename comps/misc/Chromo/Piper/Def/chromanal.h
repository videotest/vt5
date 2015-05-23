/*
 * chromanal.h		Jim Piper		February 1984
 *
 * header file for chromosome analysis programs
 *
 *	 7 Jan 1991		CAS		Added lock byte to classcont struct
 *  17 Feb 1989		SEAJ	Define constants for status return into chromlyse.
 *	09 Feb 1987		jimp	Define bits with plist->dispmark for permanent
 *							and temporary highlighting.
 *	05 Feb 1987		jimp	NSTACKPOS to 80, moved here from karysubs[12].c
 *							new comment for plist->dispopts
 *	 5 Jan 1987		CAS/jimp	Maxchild to 30 (was 10) 
 *								classcont->chcl[] size = maxchild (was 20)
 *	11 Nov 1986		jimp	Symbolic defines for object types
 *	28 Oct 1986		CAS		Increment MAXOBJS to 250
 *	17 Oct 1986		CAS		Increment MAXOBJS to 120
 */

#ifndef CHROMANAL_H
#define CHROMANAL_H

/*
 * Object segmentation types
 */
#define UNKNOWN		0
#define CHROMOSOME	1
#define COMPOSITE	2
#define OVERLAP		3
#define PIECE		4
#define NUCLEUS		5
#define NOISE		6
#define BLOB		7

/*
 * maxima for classification programs
 */
#define MAXDIM		30
#define MAXCLASS	25
#define UNDEFCLASS	MAXCLASS-1  /* is variable in struct screenform, should be removed */
#define MAXCHILD	30
#define MAXOBJS		250
#define LRATIO		-2.0	/* ln(e**(-2)) */
#define MAXCHAIN	3
#define CUTOFF		4
/*
 * parameters for profile extraction and straightening
 */
#define MAXHALFWIDTH	8
#define HALF_SLOPE_NHD	5
/*
 * parameters for karyogram
 */
#define NSTACKPOS		80
/*
 * parameter for chromosome axis construction from skeleton
 */
#define EXTENDTIP	7
/*
 * parameters for status return to chromlyse.c
 */
#define ERROR 0
#define SEGMENT 1
#define KARYOTYPE 2
#define MANKARY 3
#define COUNT 4
#define EXIT 5
#define QUIT 6
#define FLEXIBLE_SCREEN 7

struct chromosome {
	SMALL type;
	struct intervaldomain *idom;	/* interval domain */
	struct valuetable *vdom;	/* value domain */
	struct chromplist *plist;	/* property domain */
	struct object *assoc;		/* associated object */
};

/*
 * chromosome property list.  This includes three types of data :
 *	(1) feature measurements
 *	(2) processing history
 *	(3) correct (training) feature values
 */
struct chromplist {
	SMALL size;		/* of this structure - ESSENTIAL for I/O */
	SMALL number;		/* serial number in cell */
	/*
	 * undisputed scalar properties
	 */
	int area;
	int mass;
	int hullarea;		/* convex hull parameters */
	int hullperim;		/* c.h. perimeter */
	int rectarea;		/* minimum width rectangle parameters */
	short rectwidth;
	short rangle;		/* m.w. rectangle orientation */
	/*
	 * algorithm dependent measurements
	 */
	short length;
	short width;
	short angle;		/* orientation of axis */
	short nsigchords;	/* significant chords of convex hull */
	short cx;		/* centromere parameters */
	short cy;
	short cangle;
	short clconf;		/* centromere line confidence */
	short cindexa;		/* centromeric index (%) - area */
	short cindexm;		/*      "        "    "  - mass */
	short cindexl;		/*      "        "    "  - length */
	short dispor;		/* 1 if to be drawn right way up,
				  -1 if to be inverted, 0 if unset */
	short disppos;		/* display position in class in karyotype,
				   0 if not yet set */
	short dispmark;		/* See defines below for bit interpretation */
	short dispopts;		/* now used to indicate vertical offset */
	short EXPAND0[9];	/* EXPANSION SLOT */
	SMALL otype;		/* object type :
					1	chromosome
					2	composite
					3	overlap
					4	piece
					5	nucleus
					6	spot noise (small unknown)
					7	blob noise (large unknown) */
	SMALL otconf;		/* object type confidence */
	SMALL pnumber;		/* parent number if derived from composite,
				   of one parent if joined from pieces */
	int history[8];		/* processing history :
					0	how derived from parent
						1 - manual split of composite
						2 - manual extract from overlap
						4 - manually joined
					1	how disposed of (e.g. split)
						1 - manual split of composite
						2 - manual extract from overlap
						4 - manually joined
					2	chromosome count of non-chromosome object
					3   second parent if a joined object
					remainder - to be determined */
	short dgroup;		/* denver group */
	short dgconf;
	short pgroup;		/* paris group/class */
	short pgconf;
	short cvdd;		/* coefficient of variation of density profile */
	short nssd;		/* Granum's NSSD of density profile */
	short mdra;		/* ratio of mass c.i. to area c.i. */
	short ddm2;		/* radius of gyration / length  of dd */
	short wdd[6];		/* weighted density distribution parameters */
	short mwdd[6];		/* wdd of 2nd moment profile parameters */
	short gwdd[6];		/* wdd of profile of absolute gradient of dd */
	short EXPAND1[2];	/* EXPANSION SLOT */
	/*
	 * correct values inserted by cytogeneticist
	 */
	int Cangle;		/* correct orientation */
	int Ccx;		/* correct centromere location */
	int Ccy;
	int Ccangle;
	SMALL Cotype;		/* correct object type */
	SMALL Cdgroup;		/* correct group/class */
	SMALL Cpgroup;
};

/*
 * plist->dispmark bits
 */
#define PERM_MARK		01
#define	TEMP_MARK_REQ	02
#define TEMP_MARK_DONE	04



/*
 * linear normaliser for feature normalisation
 * uses scaled-integer arithmetic (jimp 27/11/85)
 */
struct normaliser {
	int A;	/* multiplier */
	int B;	/* additive constant */
};


/*
 * structures for classification: first per chromosome, then per class
 */
struct cllik {
	int class;
	double lval;
};

struct chromcl {
	int rawclass;
	int trueclass;
	int newclass;
	int bestclass;
	double moveval;
	struct chchain *movechain;
	struct cllik cllika[CUTOFF];
};

struct chchain {
	struct chromcl *chrcl;
	double accum_cost;
	int nchildren;
	int class;
	int bestchild;
	struct chchain *child[MAXCHILD];
};

struct classcont {
	unsigned char lock;
	int n;
	struct chromcl *chcl[MAXCHILD];
};

struct cellid {
	SMALL type;			/* 110 cell identifier */
	struct domain *idom;		/* = NULL */
	struct value *vdom;		/* = NULL */
	struct cellplist *plist;	/* contains slide ID and descriptions*/
	struct object *assoc;		/* =NULL */
};

struct cellplist {
	SMALL size;			/* size of list */
	char slideid[20]; 		/* slide identifier */
	char doc[10];			/* date of culture */
	char sex[2];			/* sex of specimen */
	char stain[10];			/*  type of stain used */
	char operatorid[20];		/* m/c operator */
	char karyotype[40];		/* symbolic karyotype */
};



/*
 * object identification for karyogram rearrangement
 */
struct kident {
	int no;		/* chromosome number in object table */
	int kclass;	/* measured from zero to MAXCLASS-1 */
	int pos;	/* first, second, third, ... of this class */
	int stackpos;	/* position in general object stack when required */
};


/*
 * class contents for karyogram rearrangement
 */
struct kclasscont {
	int n;		/* how many in this class */
	int maxn;	/* high-water mark of table for this class */
	int kno[MAXCHILD]; /* table of objects in class, -1 if unused entry */
	int sp[MAXCHILD]; /* table of bottom line stack occupancy */
};

/*
 *-------------------------------------------------------------------------
 * Classifier structure.  In static data space so that multi-processing
 * and re-classification calls have some chance of working correctly !
 *-------------------------------------------------------------------------
 * BEWARE - this structure contains some arrays and some pointers.
 * When converting addesses for multi-processing, the compiler will
 * access the arrays correctly by offsetting from the structure address,
 * which itself nust of course be converted in a slave processor.
 * But the POINTER VALUES in the structure must be explicitly converted.
 *-------------------------------------------------------------------------
 */
struct classifier {
	int phenosex;	/* not currently used */
	int nclass;		/* number of classes */
	int ndim;		/* total number of features */
	int nsfv;		/* number of selected feature vectors */
	double clinvvarsqrtdet[MAXCLASS];
	int iclassmeans[MAXCLASS][MAXDIM];
	int iclassinvvars[MAXCLASS][MAXDIM];
	double conversionf[MAXCLASS];
	int numchr;		/* number of chromosomes */
	int ntype1_3;	/* number of chroms, comps, ovlps, for normaliser */
	int sfv[MAXDIM];/* selected feature vectors */
	struct chromosome *chrlist[MAXOBJS];
	struct chromosome *otype1_3_list[MAXOBJS];
	struct classcont clcontents[MAXCLASS];
	struct chromcl chrocl[MAXOBJS];
	struct normaliser vn[MAXDIM];
	double *lpriorp;	/* prior probabilities table */
	int *clwn;		/* expected class size table */
};

#endif CHROMANAL_H

