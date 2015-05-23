#define register
/*
 * featvect.c	
 *
 * Get feature vector from object.
 * Also get classifier dimension,
 * and normaliser vectors.
 *
 * Modifications
 *   9 Apr 1991		ih/cas		Add checks for zero divide in norm_a_d_l
 *	 7 Feb 1991		CAS			remove Mizar defs -> osk
 *	02 Dec 1988		SCG/dcb		avoid dividing fss as this can sometimes be zero
 *	16 Sep 1988		SCG			Modified norm_a_d_l and normhp so that derived
 *								objects are included in normalisation (history[0] > 0)
 *	13 Nov 1986		jimp		And yet one more additional small number fix,
 *								so that non-zero vn->A values are returned if
 *								zero or one objects input.
 *	15 Oct 1986		CAS/SCG		Further attempts to deal with silly numbers
 *								of chroms (0,1 etc) in norm_a_d_l, standardnorm
 *	19 Sep 1986		SCG/CAS		Fix standardnorm so it doesn't crash with
 *								only one object
 *	26 Nov 1985		Jimp		modified for scaled integer arithmetic
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>
#include <math.h>

#define SCALEFACTOR 256

/*
 * get feature vector
 */
getvec(plist,v)
register struct chromplist *plist;
register int *v;
{
	register int i;

	/* area */
	v[0] = plist->area;

	/* length */
	v[1] = plist->length;

	/* relative density */
	v[2] = plist->mass/plist->area;
	
/* centromeric index by area, ratio of mass c.i. to area c.i. */
	v[3] = plist->cindexa;
	v[4] = plist->cindexm;
	v[26] = plist->cindexl;
	v[27] = plist->hullperim;
	/* Modify c.i. appropriately if object upside-down */
	if (v[3] > 50) {
		v[3] = 100 - v[3];
		v[4] = 100 - v[4];
		v[26] = 100 - v[26];
	}
	/* Convert v[4] to ratio - watch for division by zero */
	if (v[3] < 1) {
		v[3] = 1;
		v[4] = 1;
	}
	v[4] = 100 * v[4] / v[3];

	/* cvdd, nsdd, ddm1, mdra */
	v[5] = plist->cvdd;
	v[6] = plist->nssd;
	v[7] = plist->mdra;

	/* wdd of density profile */
	/* wdd of 2nd moment of density profile */
	/* wdd of gradient of density profile */
	for (i=0; i<6; i++) {
		v[8+i] = plist->wdd[i];
		v[14+i] = plist->mwdd[i];
		v[20+i] = plist->gwdd[i];
	}

	return(28);
}

getdim(print)
{
#ifndef OSK
	if (print) {
		printf("28 features (note numbering from zero) :\n\n");
		printf("0: area normalised by median object area\n");
		printf("1: length normalised by median object length\n");
		printf("2: normalised density (cell mean = 100)\n");
		printf("3: area centromeric index\n");
		printf("4: ratio of mass c.i. to area c.i.\n");
		printf("5: normalised cvdd\n");
		printf("6: normalised nssd\n");
		printf("7: normalised ratio mass to area c.i.\n");
		printf("8-13: normalised wdds\n");
		printf("14-19: normalised mwdds\n");
		printf("20-25: normalised gwdds\n");
		printf("26: length centromeric index\n");
		printf("27: hull perimeter\n");
	}
#endif OSK
	return(28);
}

#ifndef OSK
getclass(print)
{
	if (print)
		printf("\n24 classes (22 autosomes, X = 23, Y = 24)\n");
	return(24);
}
#endif OSK




/*
 * Compute a normalisation vector for classifier - compute pairs
 * of constants A and B such that X = (Ax + B) / SCALEFACTOR.
 *
 * THE VECTOR OF NORMALISERS SHOULD CORRESPOND TO THE VECTOR OF FEATURES
 * IN getvec()
 */


normvec(objs,number,vn)
struct chromosome **objs;
struct normaliser *vn;
{
	int i, j;
	struct chromplist plist;
	norm_a_d_l(objs,number,&vn[0]);		/* area, density/mass, length */
	norm_hp(objs,number,&vn[27]);		/* hull perimeter */
#ifdef OSK
	/* FOLLOWING CONVERSION EQUIVALENT TO "fstand1.c" ON VAX */
	vn[3].A = 10 * SCALEFACTOR;
	vn[3].B = -250 * SCALEFACTOR;
	vn[4].A = SCALEFACTOR;
	vn[4].B = 0;
#else OSK
	for (i=3; i<=4; i++) {
		vn[i].A = SCALEFACTOR; vn[i].B = 0;	/* area, mass c.i. */
	}
#endif OSK
	vn[26].A = SCALEFACTOR; vn[26].B = 0;		/* length c.i. */
	/* cvdd, nssd, ddm1, mdra */
	standardnorm(objs,number,((char *)&plist.cvdd - (char *)&plist.size),&vn[5]);
	standardnorm(objs,number,((char *)&plist.nssd - (char *)&plist.size),&vn[6]);
	standardnorm(objs,number,((char *)&plist.mdra - (char *)&plist.size),&vn[7]);
	/* wdd distributions normalised to standard mean 0, s.d. 100 */
	for (i=0; i<6; i++) {
		standardnorm(objs,number,((char *)&plist.wdd[i] - (char *)&plist.size),&vn[8+i]);
		standardnorm(objs,number,((char *)&plist.mwdd[i] - (char *)&plist.size),&vn[14+i]);
		standardnorm(objs,number,((char *)&plist.gwdd[i] - (char *)&plist.size),&vn[20+i]);
	}
}

/*
 * normalise area so that object median area = 500
 * normalise cell mass so that mean relative density is 100
 * normalise length so that object median length = 50
 */
norm_a_d_l(objs,number,vn)
register struct chromosome **objs;
register struct normaliser *vn;
{
	register struct chromplist *p;
	register int i,nchr,tarea,tmass;
	int areas[MAXOBJS],nchr2,compare();
	int lengths[MAXOBJS];
	double d;

	tmass = 0;
	tarea = 0;
	nchr = 0;
	for (i=0; i<number; i++) {
		/*
		 * check object history - we accept underived objects only,
		 * giving worst-case least-assumption normalisation of size
		 */
		p = objs[i]->plist;
/*		if (p->otype <= 3 && p->history[0] == 0) { */
		if(p->otype <= 1) {
			tarea += p->area;
			tmass += p->mass;
			areas[nchr] = p->area;
			lengths[nchr++] = p->length;
		}
	}
	if (nchr) {
		int tmp;
		qsort(areas,nchr,sizeof(int),compare);
		qsort(lengths,nchr,sizeof(int),compare);
		/* take mean of central 2 (if even number) */
		nchr2 = nchr>>1;
		if ((nchr&1) == 0) {
			if (tmp=(areas[nchr2-1]+areas[nchr2]))
				vn[0].A = 1000 * SCALEFACTOR/tmp;
			else
				vn[0].A=1;
			if (tmp=(lengths[nchr2-1]+lengths[nchr2]))
				vn[1].A = 100 * SCALEFACTOR/tmp;
			else
				vn[1].A=1;
		} else {
			if (tmp=areas[nchr2])
				vn[0].A = 500 * SCALEFACTOR/tmp;
			else
				vn[0].A=1;
			if (tmp=lengths[nchr2])
				vn[1].A = 50 * SCALEFACTOR/tmp;
			else
				vn[1].A=1;
		}
		vn[0].B = 0;
		vn[1].B = 0;
		/*
		 * the aim of the following is to normalise the mean density
		 * to 100.  The following assumes that the area has been
		 * extracted RAW by getvec() for this feature !!!!!
		 */
		d = 100.0 * (double) tarea;
		if (tmass) d /= (double)tmass;
		vn[2].A = d * SCALEFACTOR;
		vn[2].B = 0;
	} else {
		vn[0].A = 1;
		vn[1].A = 1;
		vn[2].A = 1;
		vn[0].B = 0;
		vn[1].B = 0;
		vn[2].B = 0;
	}
}


compare(i,j)
register int *i,*j;
{
	return(*j - *i);
}


/*
 * standardise short integer member of property list to mean zero, s.d. 100.
 * BUT IGNORE NON-CHROMOSOMES WHEN COMPUTING THE NORMALISERS.
 * "off" is (byte) offset in propertylist to value in question.
 */
standardnorm(objs,n,off,vn)
register struct chromosome **objs;
register struct normaliser *vn;
{
	register struct chromplist *plist;
	register int i,v, s,ss;
	int nc;
	double fs,fss;
	short *o;
	s = 0;
	ss = 0;
	nc = 0;
	for (i=0; i<n; i++) {
		plist = objs[i]->plist;
		if (plist->otype <= 1) {
			nc++;
			o = (short*) ((char *)plist + off);
			v = *o;
			s += v;
			ss += v*v;
		}
	}
	if (nc <= 1) {
		vn->A = 1;
		vn->B = 0;
	} else {
		fs = s;
		fss = ss;
		fs /= nc;
		fss = fss - nc*fs*fs;
		fss = fss/(nc-1);
		fss = sqrt(fss);
		/*
		 * In certain rare occassions fss can be zero; avoid division
		 */
		if(fss != 0)
			vn->A = 100 * SCALEFACTOR/fss;
		else
			vn->A = 100;
		vn->B = - (fs * vn->A);
	}
}


#ifndef OSK
/*
 * specify which features are to be used for classification.
 * Note that as usual the features are numbered from zero.
 * Program "vcv" REQUIRES SELECTED FEATURES TO BE IN ASCENDING ORDER !
 */
selectedfeatures(sfv,nsfv)
int *sfv,*nsfv;
{
/*
	*nsfv = 15;
	sfv[0] = 0;
	sfv[1] = 2;
	sfv[2] = 3;
	sfv[3] = 5;
	sfv[4] = 6;
	sfv[5] = 8;
	sfv[6] = 9;
	sfv[7] = 11;
	sfv[8] = 12;
	sfv[9] = 15;
	sfv[10] = 17;
	sfv[11] = 20;
	sfv[12] = 21;
	sfv[13] = 23;
	sfv[14] = 24;
*/
	*nsfv = 3;
	sfv[0] = 0;
	sfv[1] = 9;
	sfv[2] = 15;
}
#endif OSK


/*
 * normalise hull perimeter by median
 */
norm_hp(objs,number,vn)
register struct chromosome **objs;
register struct normaliser *vn;
{
	register struct chromplist *p;
	register int i,medianh,nchr,nchr2;
	int hullp[MAXOBJS],compare();

	nchr = 0;
	for (i=0; i<number; i++) {
		/*
		 * check object history - we accept underived objects only,
		 * giving worst-case least-assumption normalisation of size
		 */
		p = objs[i]->plist;
/*		if (p->otype <= 3 && p->history[0] == 0) */
		if(p->otype <= 1)
			hullp[nchr++] = p->hullperim;
	}
	qsort(hullp,nchr,sizeof(int),compare);
	/* take mean of central 2 (if even number) */
	nchr2 = nchr>>1;
	if ((nchr&1) == 0)
		medianh = hullp[nchr2-1]+hullp[nchr2];
	else 
		medianh = 2*hullp[nchr2];
	if (medianh == 0)
		vn->A = SCALEFACTOR;
	else
		vn->A = 1000 * SCALEFACTOR / medianh;
#ifdef OSK
	/* FOLLOWING CONVERSION EQUIVALENT TO "fstand1.c" ON VAX */
	vn->B = -500 * SCALEFACTOR;
#else OSK
	vn->B = 0;
#endif OSK
}


/*
 * normalise feature vector and convert selected
 * feature set to integer
 */
normivec(fvec,ivec,sfv,vn,ndim,nsfv)
register int *fvec;
int *sfv;
register int *ivec;
register struct normaliser *vn;
register int nsfv;
{
	register int i,j,f;

/* FOLLOWING IS EQUIVALENT TO "fstand1.c" ON VAX */
	fvec[0] = (vn[0].A * fvec[0] + vn[0].B) / SCALEFACTOR;
	fvec[1] = (vn[1].A * fvec[1] + vn[1].B) / SCALEFACTOR;
	/* mean of length and area */
	fvec[1] = 10*fvec[1] + fvec[0];
	fvec[0] -= 500;
	fvec[1] = (fvec[1] - 1000)/3;
/* OTHER "fstand1.c" CONVERSIONS PERFORMED IN "featvect.c"  ON OSK */

	for (i=0; i<nsfv; i++){
		j = *sfv++;
		f = fvec[j];
		/* normalise feature vector if not already done above */
		if (j > 1)
			f = (f * vn[j].A + vn[j].B) / SCALEFACTOR;
#ifdef DEBUG4
fprintf(stderr,"\n%2d %2d %8f",i,*(sfv-1),f);
#endif DEBUG4
		*ivec++ = f;
#ifdef DEBUG4
fprintf(stderr," %8d",*(ivec-1));
#endif DEBUG4
	}
#ifdef DEBUG
fprintf(stderr," integer-conversion");
#endif DEBUG
}
