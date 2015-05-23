/*
 *      D I S P F O R M . H  --  Display format/layout structure defs
 *
 *
 *  Written: Clive A Stubbings
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems (1988)
 *
 *  Date:    13th May 1988
 *
 *  Modifications
 *
 *  20 Jan 1992		Joanne	Added undefclass to screenform, is now variable
 *  16 Jul 1991		ih		Added structures to screenform for lines on karyogram
 *  28 Apr 1989		SEAJ	Added kar scale into hcopyform structure.
 *  25 Apr 1989		SEAJ	Added extra constants.
 *  13 Apr 1989		SEAJ	Added all hardcopy format structures.
 *  22 Mar 1989		SEAJ	Added height into kdisppos and viewed_stack into screenform.
 *	 2 Dec 1988		CAS		Define VIEWED_STACK here for ktyping
 *	23 Aug 1988		CAS		More flex display
 *	19 Aug 1988		CAS		Include display position stuff
 */

#define VERSION 3
#define DISPTEXT	40
#define VIEWED_STACK 8

#define CLASS 0
#define STK 1
#define METAPHASE 2
#define TXT 3
#define CHCLASS 4
#define VIEWED 5
#define KARSCALE 6
#define GROUP 6

#define CASE 0
#define SLIDE 1
#define CELL 2
#define OPERATOR 3
#define DATE 4
#define SUPERVISOR 5
#define SUPERDATE 6
#define PATNAME 7
#define COMMENT1 8
#define COMMENT2 9
#define KARYOTYP 10
#define LABNAME 11
#define BANNER 12
#define MAXTEXT 13		/* maximum number of different text blocks */

/* 
 * bits defined to decide how to display chromosomes
 */
#define TOP  00001
#define ACRO 00010


struct txt {
		int		x, y, size;
		char	text[DISPTEXT];
	};

struct pos {
		int	x, y;
	};

struct idtext {
		int		display;
		int		len;		/* Max length of sys string to display */
		int		x,y,size;		
		char	text[DISPTEXT];
	};

/*
 * structure which describes placement of chromosomes on screen
 * one entry per class.
 */
struct kdisppos {
	int col;		/* DIGS column number of center of area */
	int lin;		/* DIGS line number of base of area */
	int defaultpos;	/* default number of positions for class members */
	int npos;		/* current number of positions for class members */
	int space;		/* DIGS width available */
	int height;		/* DIGS height */
	char tag[6];	/* character string which names this class */
};
struct databox {
		struct pos		position;		/* Position of box bottom left corner */
		int				width,			/* Box width */
						height;			/* Box height */
		struct txt		banner;			/* Box banner */
		struct idtext	case,			/* Case id */
						slide,			/* Slide id */
						cell,			/* Cell number */
						operator,		/* Operator id */
						date,			/* Date */
						supervisor,		/* Supervisor acceptance */
						superdate,		/* Supervisor date */
						patname,		/* Patient name */
						comment1,		/* 1st line of comments */
						comment2,		/* 2nd line of comments */
						karyotype,		/* Karyotype */
						labname;		/* Laboratory name */
	};
struct lineint {
	int x1;
	int y1;
	int x2;
	int y2;
	struct lineint *prev;
	struct lineint *next;
};
struct linebuf {
	int count;
	struct lineint *lineint;
};
/*
 *	Karyotype layout for screen display
 */
struct screenform {
		short	size;				/* Structure size */
		short	version;
		struct	databox	idbox;
		struct	kdisppos disppos[MAXCLASS];
		int	stacklin;
		int 	viewed_stack;
		int     undefclass;		/* was defined in chromanal.h */
		int	stackcol[NSTACKPOS+2];
		int 	mode;			/* 0=normal, 
						 * 1=hang from top, 
						 * 2=acrocentric 
						 */
		struct	pos	chromclass;
		struct linebuf linebuf;
	};
/*
 * hardcopy structures for LASER3
 */
struct box {
		struct pos posn;
		int width;
		int height;
		int col;			/* colour of box */
	};
struct hcobject  {
		int type;					/* Type of hcopy object */
		struct box b;				/* Box */
		int size;					/* size of object */
		char txt[DISPTEXT];			/* text for text object */
		int asstxt;					/* assosiated text from kmr */
		int class;					/* class number for enlargements/ideograms etc */
		int res;					/* resolution for ideograms */
		struct hcobject *next;		/* pointer to next object */
		struct hcobject *ass;		/* pointer to associated objects */
	};
struct hcopyform {
		short	size;				/* Structure size */
		short 	version;			/* Version number */
		int 	chromclasses;		/* Number of classes displayed */
		int 	viewed_stack;		/* Number of stack places viewed */
		int 	karscale;			/* Scale for karyotype */
		struct 	hcobject *hco;		/* Head for linked list of objects */
	};

