#if !defined(__wstruct_h__)
#define __wstruct_h__
/*
 * structure definitions for object structures for woolz system
 *
 * Load system dependant definitions
 *
 * Modifications
 *
 *	23 Oct 1990	jimp@MRC	Added compound objects (from MRC Woolz)
 *	 7 Oct 1986		CAS				Moved wmachdef to /dd/defs/machdef.h
 *	 6 Oct 1986		CAS (GJP)		Added a link count to some of the
 *									more 'shared' structures, to enable
 *									us to only free them when everyone
 *									has finished using them. (Done under
 *									protest from GJP). (Sorry Jim)
 */
#include <machdef.h>
/* main object structure.  By use of pointers to sub-structures, these
   do not necessarily need to be of the type specified, but can be re-typed
   to be any pointer-to-structure type. */

struct object {
	SMALL type;	/* object type : in the following ranges :
				1-9 - interval/grey table objects :
					 1 - "conventional" object
				10-19 - polygons, boundaries, etc. :
					10 - polygon
					11 - boundlist - tree of polygon domains
					12 - convex hull
					13 - histogram or profile
				20-29 - rectangles
					20 - rectangle specified by vertices
				30-39 - lines :
					30 - vector (integer vertices)
					31 - vector (float vertices)
				40-49 - points :
					40 - point (integer vertices)
					41 - point (float vertices)
				50-59 - convolution :
					50 - int space-domain convolution
					51 - float space-domain convolution
				60-69 - miscellaneous
					60 - display frame
				70-79 - text and labels
					70 - text
				80-89 - compound objects
					80 - compound array (all elements same type)
					81 - compound array (elements may be different)
					82 - compound linked list (same types)
					83 - compound linked list (different types)
			*/
	struct intervaldomain *idom;	/* interval domain */
	struct valuetable *vdom;	/* value domain */
	struct propertylist *plist;	/* property domain */
	struct object *assoc;		/* associated object */
};

/* standard interval list structure for pixel-objects */

struct intervaldomain {
	SMALL type;			/* type :
					  1 - conventional with interval list
					  2 - rectangular (NULL interval list)
					*/
	int		linkcount;		/* Number of times data is used */
	char *freeptr;			/* If != NULL, can be used to free intervals table */
	COORD line1;			/* first line coordinate */
	COORD lastln;			/* last    "     "	 */
	COORD kol1;			/* first column  "	 */
	COORD lastkl;			/* last    "     "	 */
	struct intervalline *intvlines;	/* array of intervals in a line structures
					   note "intervals in a line" must be contiguous*/
};

/* structure  of intervals in a line */

struct intervalline {
	SMALL nintvs;			/* number of intervals in this line */
	struct interval *intvs;		/* array of intervals (must be contiguous)*/
};

/* interval structure */

struct interval {
	COORD ileft;	/* offset from domain->kol1 of interval left end */
	COORD iright;	/* offset from domain->kol1 of interval right end */
};

/* standard value table for pixel objects */

struct valuetable {
	SMALL type;			/* type :
						1 : GREY pixel values
						2 : float values (not implemented)
					*/
	int		linkcount;		/* Number of times data used */
	char *freeptr;			/* If != NULL, can be used to free grey table */
	COORD line1;			/* first line */
	COORD lastln;			/* last line */
	GREY bckgrnd;			/* background value for points not in object */
	struct valueline **vtblines;	/* array of pointers to structures of grey table lines */
	struct object *original;	/* pointer to originating object */
};

/* structure of grey values in a line */
struct valueline {
	COORD vkol1;			/* left end */
	COORD vlastkl;			/* right end */
	GREY *values;			/* array of values */
};


/* rectangular values table structure */
struct rectvaltb {
	SMALL type;	/* type
				11 : GREY pixel values
				12 : float pixel values (not implemented)
			*/
	int		linkcount;		/* Number of times data used */
	char *freeptr;
	COORD line1;
	COORD lastln;
	COORD kol1;
	COORD width;	/* more use than lastkl */
	GREY bckgrnd;
	GREY *values;	/* contiguous array of values */
	struct object *original;
};

/* interval-structured value table for pixel objects */

struct ivtable {
	SMALL type;			/* type :
						21 : GREY pixel values
						22 : float values (not implemented)
					*/
	int		linkcount;		/* Number of times data used */
	char *freeptr;			/* If != NULL, can be used to free grey table */
	COORD line1;			/* first line */
	COORD lastln;			/* last line */
	GREY bckgrnd;			/* background value for points not in object */
	struct valintline *vil;		/* pointers to structures of grey table lines */
	struct object *original;	/* pointer to originating object */
};

/* structure for one line's worth of grey-intervals */

struct valintline {
	SMALL nintvs;			/* number of grey-intervals */
	struct valueline *vtbint;	/* pointer to grey intervals */
};


/* polygon domain structure */

struct polygondomain {
	SMALL type;			/*
					1 for int vertices
					2 for float vertices
					*/
	SMALL nvertices;
	SMALL maxvertices;
	struct ivertex *vtx;
};

struct ivertex {
	COORD   vtY;
	COORD   vtX;
};

struct fvertex {
	float vtY;
	float vtX;
};

/* boundary list structure - domain of boundary object type 11 */

struct boundlist {
	SMALL type;			/*	0 - piece
						1 - hole */
	struct boundlist *up;		/* the containing hole or piece, NULL
				   	if the universal hole (very top) */
	struct boundlist *next;		/* next hole or piece at same level and
				   	lying within same piece or hole, NULL
				   	if no more at this level */
	struct boundlist *down;		/* first enclosed structure, NULL if none */
	SMALL wrap;			/* wrap number - number of points of
				   	boundary included both at start and end
				   	of polygon representation */
	struct polygondomain *poly;	/* polygon representation of boundary */
};

/*
 * convex hull parameters structure (the vertices as such are stored
 * in a conventional polygon domain).  This is placed in the "vdom"
 * position in an object type 12.
 */

struct cvhdom {
	SMALL type;
	SMALL nchords;	/* number of chords */
	SMALL nsigchords;	/* number of significant chords */
	COORD mdlin;	/* mid-line of enclosed originating object */
	COORD mdkol;	/* mid-column of enclosed originating object */
	struct chord *ch;
};

struct chord {
	SMALL sig;	/* non-zero if judged to be significant */
	int acon;	/* chord equation is acon*x - bcon*y = ccon */
	int bcon;	/* in object-central coordinates */
	int ccon;
	int cl;		/* chord length, *8 */
	COORD bl;	/* line number of bay bottom or bulge top */
	COORD bk;	/* column number of bay bottom or bulge top */
	int barea;	/* bay or bulge area, *8 */
	int bd;		/* bay maximum depth or bulge max height, *8 */
};


/* profile or histogram.  Here only one dimension is specified; the
   other is assumed to have unit increment.  An origin is included
   mostly for display purposes */

struct histogramdomain {
	SMALL type;	/* 1 for integer values, 2 for floats */
	SMALL r;	/* 0 - display with vertical base,
			   1 - display with horizontal base */
	COORD k;	/* column origin (for display) */
	COORD l;	/* line origin */
	SMALL npoints;	/* length */
	int *hv;	/* pointer to array of values - floats if type 2 */
};


/* rectangle domain structures */

struct irect {
	SMALL type;		/* 1 - integer vertices */
	COORD irk[4];		/* column vertex coordinates */
	COORD irl[4];		/* line vertex coordinates */
				/* side from (l[0],k[0]) to (l[1],k[1])
				   is a long side.  Vertices are cyclic */
	float rangle;		/* angle of long side to vertical (radians) */
};

struct frect {
	SMALL type;		/* 2 - real vertices */
	float frk[4];		/* column vertex coordinates */
	float frl[4];		/* line vertex coordinates */
				/* side from (l[0],k[0]) to (l[1],k[1])
				   is a long side.  Vertices are cyclic */
	float rangle;		/* angle of long side to vertical (radians) */
};



/*
 * vector structure.  Type 30 and 31 objects.
 */

struct ivector {
	SMALL type;		/* 30 */
	SMALL style;
	COORD k1;
	COORD l1;
	COORD k2;
	COORD l2;
};

struct fvector {
	SMALL type;		/* 31 */
	SMALL style;
	float k1;
	float l1;
	float k2;
	float l2;
};


/*
 * points (for cursors, etc).    Objects type 40 and 41.
 */

struct ipoint {
	SMALL type;		/* 40 */
	SMALL style;
	COORD k;
	COORD l;
};

struct fpoint {
	SMALL type;		/* 41 */
	SMALL style;
	float k;
	float l;
};


/*
 * convolution mask structure.  Main structure for a type 50 object.
 */
struct convolution {
	SMALL type;	/* 50 integer valued
			   51 real valued */
	SMALL size;	/* size of (square) mask (must be odd) */
	int *cv;	/* size*size convolution mask elements
			   To reduce computational cost at the expense of data
			   storage, complete convolution must be specified even
			   if highly symmetrical */
};


/* standard workspace structure for interval objects */

struct iwspace {
	struct object *objaddr;		/* the current object */
	SMALL dmntype;			/* domain type */
	SMALL lineraster;		/* line scan direction as follows :
					  1 increasing rows
					  -1 decreasing rows */
	SMALL colraster;		/* column scan direction as follows :
					  1 increasing columns
					  -1 decreasing columns */
	struct intervaldomain *intdmn;	/* pointer to interval structure */
	struct intervalline *intvln;	/* pointer to current line of intervals */
	struct interval	*intpos;	/* pointer to current interval -
						in the case of domain type = 2
						(rectangle) this is set up to
						point to the column bounds in
						the intervaldomain structure */
	COORD colpos;			/* column position */
	SMALL colrmn;			/* columns remaining */
	COORD linbot;			/* first line */
	COORD linpos;			/* line position */
	SMALL linrmn;			/* lines rnemaining */
	SMALL intrmn;			/* intervals remaining in line */
	COORD lftpos;			/* left end of interval */
	COORD rgtpos;			/* right end of interval */
	SMALL nwlpos;			/* non-zero if new line, counts line
					   increment since last interval */
	struct gwspace *gryptr;		/* pointer to grey table workspace */
};

/* standard workspace for grey-table manipulations */

struct gwspace {
	SMALL gvio;			/* grey value i/o switch :
					   0 = input to object only
					   1 = output from object only
					   Only relevant if tranpl set, as all
					   grey-tables are unpacked. */
	SMALL tranpl;			/* if non-zero, transplant values to a
					   buffer whose address is grintptr.
					   Direction of transplant in gvio */
	struct valuetable *gtable;	/* grey table */
	struct valueline **gline;	/* pointer to current grey table line pointer */
	struct iwspace *intptr;		/* pointer to interval table workspace */
	GREY *grintptr;			/* pointer to interval grey table.
					   Always points to lowest order column,
					   whatever the value of raster */
};

/*
 * trivial property list
 * propertylist must ALWAYS commence with "size" which MUST be
 * initialised to its size in bytes
 */

struct propertylist {
	SMALL size;
};



/*
 * frame for object display
 */
struct pframe {
	SMALL type;	/* ------ 60 ----- */
	SMALL scale;	/* object coordinates to DIGS coordinates.
			   NOTE - positive only (see ix, iy below) */
	COORD dx;	/* DIGS coordinates x/column origin */
	COORD dy;	/* DIGS coordinates y/line origin */
	COORD ox;	/* object coordinates x origin - column in object
			   space to be located at DIGS column origin dx */
	COORD oy;	/* object coordinates y origin - line in object
			   space to be located at DIGS line origin dy */
	SMALL ix;	/* x increment factor - can be used to set x-scale
			   different to yscale, or to reverse plotting
			   direction, etc..   Usually +1 or -1 but can
			   be varied for special effects. */
	SMALL iy;	/* yscale factor */
	int (*func)();	/* transform function passed to picture routine,
			   where if not NULL then applied to each pixel.
			   This facility only applies to type 1 objects.
			   ----- NOT YET IMPLEMENTED -----
			   Note - no parameters permitted */
};

#if !defined(__NOTEXT)
/*
 * text object
 * holds text associated with the object
 * for screen display but will also be written to file
 * with the object
 */
struct text {
	SMALL type;			/* 70 for text*/
	struct textdomain *tdom;	/* details about the text*/
	char *text;
	char *plist;		/*NULL*/
	char *assoc;		/*NULL*/
};
#endif

struct textdomain {
	SMALL type;
	COORD k;			/* origin w.r.t. object coords */
	COORD l;
	SMALL orientation;
	SMALL spacing;		
	SMALL stringdir;		/*string direction 1-4*/
	SMALL boldness;		
	SMALL font;
	SMALL stringlen;		/* length of the text string */
};

/*
 * compound objects, implemented as either ARRAYS or LINKED LISTS
 * of other objects.  There is a distinction between an compound
 * of the same type (e.g. resulting from a labelling) and a
 * compound of different types (e.g. resulting from a range of
 * image processes from a single original object).
 */
struct compounda {
	SMALL type;		/* 80: components all same type */
				/* 81: different types permitted */
	SMALL linkcount;
	SMALL otype;		/* the permitted type if type==80 */
	int n;			/* maximum number of objects (array length) */
	struct object **o;	/* the list of woolz object pointers */
	struct propertylist *p;	/* a non-specific property list */
	struct object *assoc;
};


struct compoundl{
	SMALL type;		/* 82: components all same type */
				/* 83: different types permitted */
	SMALL linkcount;
	SMALL otype;		/* the permitted type if type==82 */
	/*
	 *******UNDETERMINED LINKED LIST STRUCTURE
	 */
	struct plist *p;	/* a non-specific property list */
	struct object *assoc;
};

#endif