/*
 *      F I L E H E A D E R S . H  --  Structure definitions for file headers
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
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    9th December 1986
 *
 *  Modifications
 *
 *	9 Feb  1991		ih			Added type 2 headers
 *	17 Dec 1990		jimp@MRC		Added #define FLEXFILE 6 and
 *							struct flex_header
 *	27 Oct 1989		dcb			redefine HEADER_VERSION to be 3
 *	16 Aug 1989		CAS			Extra digitisation type stuff for fluor
 *	25 Jul 1989		dcb			Add SCOREFILE type
 *	17 May 1989		dcb			TISSUELENGTH/STAINLENGTH to 25
 *	14 Jul 1987		CAS			Added patnam to cyto_file for 4-slide
 *	10 Jul 1987		SCG			Mod for four slide stage.  New headers
 *								and updated version.
 *	 2 Jun 1987		CAS			Added cyto_file struct
 *	29 Jan 1987		CAS			Cockup in DIG ratios...
 *	 8 Jan 1987		CAS			Added tissue + stain lengths
 *	17 Dec 1986		CAS			Added field size defines
 *
 */

#ifndef FILEHEADERS_H
#include <machdef.h>

#define HEADER_VERSION 3

/*
 *	File types
 */
#define CELLFILE 1
#define KARYFILE 2
#define METFILE 3
#define GDATFILE 4
#define SCOREFILE 5
#define FLEXFILE 6

/*
 * File access modes
 */
#define READ 1
#define WRITE 2

/*
 *	Digitisation types
 */
#define DIGFIP 		1	/* Fip array digitised (pixels are 3 x 4 approx) */
#define DIGTV		2	/* TV camera digitised (pixels are square) */
#define DIGFIPEXP	3
#define	DIGMASK		7	/* Mask to get just source type */
#define	DIGFLUOR	8	/* Fluorescent image - invert the world (bit field) */

/*
 *	Field sizes
 */
#define CASELENGTH	16
#define PATLENGTH	40
#define SLIDELENGTH	 10
#define SLIDELENGTH_V1	 4
#define SLIDELENGTH_V2	 4
#define DATELENGTH	20
#define LABLENGTH	40
#define OPERLENGTH	40
#define SCORERLENGTH 40
#define TEXTLENGTH	40
#define TISSUELENGTH	25
#define STAINLENGTH	25

/*
 *	Structure used by various routines to manipulate a file name
 */
struct cyto_file {
		int		type;
		char	case[PATLENGTH+1];
		char	patnam[PATLENGTH+1];
		char	slide[SLIDELENGTH+1];
		int		cellno;
	};


/*
 *	C E L L _ H E A D E R _ 1 --  Header record for digitised cells and
 *								  karyotypes - version 0.4.3 and previous
 *
 */
struct cell_header_1 {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[CASELENGTH];		/* Patient/case */
			char	slideid[SLIDELENGTH_V1];	/* slide */
			int		cellid;					/* cell number in metlist */
			int		dig_type;				/* Type of digitisation */
/*
 *	Metaphase dependant information
 */
			int		x_coord;				/* Co-ord and size of metaphase */
			int		y_coord;				/* on origonal slide */
			int		z_coord;
			int		x_size;
			int		y_size;
/*
 *	Karyotype specific information
 */
			char	comments[2][TEXTLENGTH];
			char	krtype[TEXTLENGTH];
			char	supid[OPERLENGTH];		/* Supervisor id + date for when */
			char	supdate[DATELENGTH];	/* ktype is OK'd */
	};
	
/*
 *	C E L L _ H E A D E R      --  Header record for digitised cells and
 *								   karyotypes - after version 0.4.3
 *
 */
struct cell_header {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[CASELENGTH];		/* Patient/case */
			char	patnam[PATLENGTH];		/* Patient name */
			char	slideid[SLIDELENGTH];	/* slide */
			int		cellid;					/* cell number in metlist */
			int		dig_type;				/* Type of digitisation */
/*
 *	Metaphase dependant information
 */
			int		x_coord;				/* Co-ord and size of metaphase */
			int		y_coord;				/* on origonal slide */
			int		z_coord;
			int		x_size;
			int		y_size;
/*
 *	Karyotype specific information
 */
			char	comments[2][TEXTLENGTH];
			char	krtype[TEXTLENGTH];
			char	supid[OPERLENGTH];		/* Supervisor id + date for when */
			char	supdate[DATELENGTH];	/* ktype is OK'd */
	};

/*
 *	C E L L _ H E A D E R 2      --  Header record for digitised cells and
 *								   karyotypes - after version 0.4.3
 *
 */
struct cell_header_2 {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[CASELENGTH];		/* Patient/case */
			char	patnam[PATLENGTH];		/* Patient name */
			char	slideid[SLIDELENGTH_V2];	/* slide */
			int		cellid;					/* cell number in metlist */
			int		dig_type;				/* Type of digitisation */
/*
 *	Metaphase dependant information
 */
			int		x_coord;				/* Co-ord and size of metaphase */
			int		y_coord;				/* on origonal slide */
			int		z_coord;
			int		x_size;
			int		y_size;
/*
 *	Karyotype specific information
 */
			char	comments[2][TEXTLENGTH];
			char	krtype[TEXTLENGTH];
			char	supid[OPERLENGTH];		/* Supervisor id + date for when */
			char	supdate[DATELENGTH];	/* ktype is OK'd */
	};


/*
 *	M E T _ H E A D E R_1  --  Header record for metaphase lists
 *							 version 0.4.3 and previous
 *
 */
struct met_header_1 {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[CASELENGTH];		/* Patient/case */
			char	slideid[SLIDELENGTH_V1];	/* slide */
/*
 *	Metaphase list dependant information
 */
 			short	nrecs;				/* Number of records */
 			short	comments;			/* How big are comments (if any) */
 			int		tissue;				/* tissue culture */
 			int		stain;				/* stain type */
	};
	
	
/*
 *	M E T _ H E A D E R _ 2   --  Header record for metaphase lists
 *								 after version 0.4.3
 *
 */
struct met_header_2 {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[CASELENGTH];		/* Patient/case id */
			char	patnam[PATLENGTH];		/* Patient name */
			char	slideid[SLIDELENGTH_V2];	/* slide */
/*
 *	Metaphase list dependant information
 */
 			short	nrecs;				/* Number of records */
 			short	comments;			/* How big are comments (if any) */
 			int		tissue;				/* tissue culture */
 			int		stain;				/* stain type */
 			int		slwidth;			/* slide width; if measured */
/*
 *	Scan dependent information
 */
 			short	btop, bbot,			/* Box size */
 					bleft, bright;
 			char	lastsub[11];		/* Includes/excludes */
 			short	allrecs, listlow,	/* Last current subset */
 					listhigh, minrank,
 					maxrank, sany,
 					ngrps, byg;
 			char	retrain;			/* Retrain/threshold flags */
	};

/*
 *	M E T _ H E A D E R   --  Header record for metaphase lists
 *								 after version 0.4.3
 *
 */
struct met_header {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[CASELENGTH];		/* Patient/case id */
			char	patnam[PATLENGTH];		/* Patient name */
			char	slideid[SLIDELENGTH];	/* slide */
/*
 *	Metaphase list dependant information
 */
 			short	nrecs;				/* Number of records */
 			short	comments;			/* How big are comments (if any) */
 			int		tissue;				/* tissue culture */
 			int		stain;				/* stain type */
 			int		slwidth;			/* slide width; if measured */
/*
 *	Scan dependent information
 */
 			short	btop, bbot,			/* Box size */
 					bleft, bright;
 			char	lastsub[11];		/* Includes/excludes */
 			short	allrecs, listlow,	/* Last current subset */
 					listhigh, minrank,
 					maxrank, sany,
 					ngrps, byg;
 			char	retrain;			/* Retrain/threshold flags */
	};

/*
 *	G D A T _ H E A D E R  -- Header record for general data (D) files
 *
 */
struct gdat_header {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[PATLENGTH];		/* Patient/case */
			char	slideid[SLIDELENGTH];	/* slide */
	};


/*
 *	F L E X _ H E A D E R      --  Header record for flexible screen
 *
 *
 */
struct flex_header {
			SMALL	size;			/* Size of structure (incl size) */
			int		version;		/* Header version number */
			int		file_type;		/* Type of file this header is attached to */
/*
 * General header data, as in cell_header
 */
			char	date[DATELENGTH];		/* Date of last change */
			char	lab_name[LABLENGTH];
			char	opid[OPERLENGTH];		/* last operator to change data */
			char	patid[CASELENGTH];		/* Patient/case */
			char	patnam[PATLENGTH];		/* Patient name */
			char	slideid[SLIDELENGTH];	/* slide */
			int		cellid;					/* cell number in metlist */
			int		dig_type;				/* Type of digitisation */
	};




#define FILEHEADERS_H
#endif FILEHEADERS_H



