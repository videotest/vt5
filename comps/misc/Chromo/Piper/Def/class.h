/*
 * 	Copyright (c) and intellectual property right Image Recognition Systems (1988)
 *
 *	File:	class.h
 *
 *		Header file containing classifier structures and defines
 *
 *	Written by: S C Georgiou
 *				720 Birchwood Boulevard
 *				Birchwood Science Park
 *				Warrington
 *				Cheshire
 *
 *	Date: 11th August 1988
 *
 *
 *	Modifications:
 *
 *	20 Mar 1990		dcb		Add RBAND definition
 *	18 May 1989		dcb/SEAJ Protect with CLASS_H
 *	10 May 1989		dcb/SEAJ move defines of BOX, LIN, VCV here
 *	 3 Feb 1989		SCG		Include tis, stn, drange in the head structs
 */
#ifndef CLASS_H
#define CLASS_H

#define BOX 1
#define LIN 2
#define VCV 3
#define VCVTEST 4
#define FAIL 0
#define SUCCESS 1

/*
 * STRUCTURES FOR LINEAR, BOX AND VCV CLASSIFIERS
 *
 */
	struct linfdisc {
		int feat1;
		int feat2;
		int feat3;
		int aflag;			/* describes how feat1, feat2, feat3 are mixed
							   to produce the ultimate feature */
		double const;
	};
	typedef struct linhead {
		int tis,			/* tissue discriptor */
			stn,			/* stains discriptor */
			drange,			/* density range value */
			nfeats;
		struct linfdisc **lfd;
	} LINHEAD;
	
	struct boxfdisc {
		int feat;
		int lower;
		int upper;
	};
	typedef struct boxhead {
		int tis,			/* tissue discriptor */
			stn,			/* stains discriptor */
			drange,			/* density range value */
			nfeats;
		struct boxfdisc **bfd;
	} BOXHEAD;
	
	struct vcvfdisc {
		int feat;
		int *featmean;
		int *featsd;
	};
	
	typedef struct vcvtype {
		int	nfeats,
			nclasses;
		double *logsqrtdet;
		double *scalef;
		struct vcvfdisc **vfd;
	} VCVTYPE;
	
/* 
 * Different sorts of chromosome classifiers 
 */
#define CELLTYPES 3			/* maximum number of different chromosome types */
#define SHORT 0
#define AVERAGE 1
#define LONG 2
#define RBAND 3

	typedef struct vcvhead {
		int tis,			/* tissue discriptor */
			stn,			/* stains discriptor */
			celltype;
		struct vcvtype *vtp[CELLTYPES];
	} VCVHEAD;
/*
 * STRUCTURE HOLDING ALL CLASSIFIER INFO; ONE EACH PER TISSUE & STAIN
 */
	struct rankcf {
		LINHEAD *lhd;
		BOXHEAD *bhd;
		VCVHEAD *vhd;
	};
/* 
 * Default no of features for chrom. classifier; also maximum number of features and classes
 */
#define DEFVCVFEATS 10			
#define MAXVCVFEATS 28
/*
 * number of basic features extracted from each metaphase
 */
#define RAWFEATS 9
#define DEFBOXFEATS RAWFEATS
#define MAXBOXFEATS RAWFEATS
									
#define XMIN 8							/* Min xwidth (sets minimum ywidth */
										/* as  xwidth / 0.65 in the miproc) */
#define XMAX 72							/* Max xwidth (sets maximum ywidth */
										/* as  xwidth / 0.65 in the miproc) */

#define YMIN 12							/* Min xwidth (sets minimum ywidth */
										/* as  xwidth / 0.65 in the miproc) */
#define YMAX 111						/* Max xwidth (sets maximum ywidth */
										/* as  xwidth / 0.65 in the miproc) */

#define COUNTMIN 25						/* Min no of objects */
#define COUNTMAX 50						/* Max no of objects */

#define SFMIN 100						/* Min shape factor, not */
										/* actually sent to the miproc */
#define SFMAX 250						/* Max shape factor */

#define CAREAMIN 0						/* Min cluster area */
#define CAREAMAX 1000					/* Max cluster area */

#define OCTAREAMIN 0					/* Min octagon area */
#define OCTAREAMAX 4000					/* Max octagon area */

#define SKODMIN 0						/* Min skod value, not */
										/* actually sent to the miproc */
#define SKODMAX 63						/* Max skod value */

#define DVMIN 0							/* Min density variation, not */
										/* actually sent to the miproc */
#define DVMAX 32						/* Max density variation */

#define CODMIN 5						/* Min cluster od */
#define CODMAX 250						/* Max cluster od */

/*
 * clustering parameters; these cannot be trained
 */
#define AREA 75							/* Limb area */
#define IOD 100							/* Integrated optical density */
#define CLUS1 12						/* Limb clustering distance */
#define CLUS2 90						/* IOD limit value (2 * upper threshold */

/*
 * Linear classifier defaults
 */
#define MAXLINFEATS 5
#define DEFLINFEATS 4					/* Default linear classifier params */
#define COUNTPR 80
#define SFPR -10						/* shape factor constant */
#define DVPR 10							/* cluster od constant */
#define SKODPR -1000					/* skirt/od constant */

#define MINFEATVAL 0.001				/* not to allow division by zero */

/*
 * predeclarations of associated functions
 */
LINHEAD *alloclin(), *freelin(), *clone_lin();
BOXHEAD *allocbox(), *freebox(), *clone_box();
VCVHEAD *allocvcv(), *freevcv(), *clone_vcv(), *settrain();
VCVTYPE *allocvcvtype(), *freevcvtype();

#endif

