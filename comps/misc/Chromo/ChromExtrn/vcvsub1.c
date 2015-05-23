/*
 * vcvsub1.c	Jim Piper	17/9/84
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>
#include <math.h>
#include "vcv.h"

/*
 * compute variance and covariances  and invert
 * (upper triangular, stored columnwise).
 * "self" MUST BE IN ASCENDING ORDER.
 */

makevcv(nself,self,nclass,nsamp,clvcv,clcp,clnsamp,cp,clm,m,vcv)
int *self, *clnsamp;
double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2];
double clcp[MAXCLASS][MAXDIM][MAXDIM], vcv[MAXDIM*(MAXDIM+1)/2];
double clm[MAXCLASS][MAXDIM], m[MAXDIM];
double cp[MAXDIM][MAXDIM];
{
	
	register int i,j,k,class;
	int g,h;
	k = 0;
	for (h=0; h<nself; h++) {
		i = self[h];
		for (g=0; g<=h; g++) {
			j = self[g];
			for (class=0; class<nclass; class++)
				clvcv[class][k] = (clcp[class][j][i] - clnsamp[class]*clm[class][i]*clm[class][j])/(clnsamp[class]-1);	
			vcv[k++] = (cp[j][i] - nsamp*m[i]*m[j])/(nsamp-1);	
		}
	}
}


/*
 * output classifier data to file
 */
trainclass_i(nself,self,nclass,ndim,clm,clsd,m,psd,teachdata,rpt)
int *self;
double clm[MAXCLASS][MAXDIM], m[MAXDIM], psd[MAXDIM];
double clsd[MAXCLASS][MAXDIM];
VcvTeachData *teachdata;
FILE *rpt;
{
	int im,intvar,g,h,i,j,k,class,err,sinverr;
	double conversionf[MAXCLASS];
	double logsqrtdet, mm, ivar, sqrtdet;

#ifndef MIZAR
	sinverr = 0;
#ifdef PRINTVCV
	fprintf(fStdOut,"\npooled variance - covariance matrix\n");
	printtriang(fStdOut,vcv,nself);
#endif PRINTVCV
	sinv(vcv,nself,0.0000000001,&err,&sqrtdet);
	if (err != 0) {
		fprintf(fStdOut,"error %d inverting pooled variance-covariance matrix\n",err);
#ifdef PRINTVCV
	} else
		printtriang(fStdOut,vcv,nself);
#else PRINTVCV
	}
#endif PRINTVCV
	for (class=0; class<nclass; class++) {
#ifdef PRINTVCV
		fprintf(fStdOut,"\nclass %d variance - covariance matrix\n",class+1);
		printtriang(fStdOut,&clvcv[class][0],nself);
#endif PRINTVCV
		sinv(&clvcv[class][0],nself,0.0000000001,&err,&clsqrtdet[class]);
		if (err != 0) {
			fprintf(fStdOut,"error %d inverting vcv matrix for class %d\n",err,class+1);
			sinverr = 1;
#ifdef PRINTVCV
		} else
			printtriang(fStdOut,&clvcv[class][0],nself);
#else PRINTVCV
		}
#endif PRINTVCV
	}
#endif MIZAR

	/*
	 * store class mean vectors and variance-covariance matrices.
	 * First, selected feature serial numbers.
	 */
//	fprintf(classfile,"%d %d %d\n",nclass,ndim,nself);
	teachdata->nclass = nclass;
	teachdata->ndim = ndim;
	teachdata->nself = nself;

	for (i=0; i<nself; i++)
//		fprintf(classfile,"%3d ",self[i]);
		teachdata->self[i] = self[i];
//	fprintf(classfile,"\n");

#ifndef MIZAR
	/* class means */
	for (class=0; class<nclass; class++) {
		for (h=0; h<nself; h++) {
			i = self[h];
//			if (h%6 == 0)
//				fprintf(classfile,"\n");
//			fprintf(classfile,"%12.5e ",clm[class][i]);
			teachdata->classmeans[class][i] = clm[class][i];
		}
	}

	/* pooled inverted variance-covariance matrix */
//	fprintf(classfile,"\n");
//	fprintf(classfile,"%12.5e",sqrtdet);
	teachdata->sqrtdet = sqrtdet;
	for (i=0; i<nself*(nself+1)/2; i++) {
//		if (i%6 == 0)
//			fprintf(classfile,"\n");
//		fprintf(classfile,"%12.5e ",vcv[i]);
		teachdata->vcv[i] = vcv[i];
	}
//	fprintf(classfile,"\n");

	/* per class inverted variance-covariance matrices */
	for (class=0; class<nclass; class++) {
		if (sinverr == 0 )
//			fprintf(classfile,"%12.5e",clsqrtdet[class]);
			teachdata->clsqrtdet[class] = clsqrtdet[class];
		else
//			fprintf(classfile,"%12.5e",sqrtdet);
			teachdata->clsqrtdet[class] = sqrtdet;
		for (i=0; i<nself*(nself+1)/2; i++) {
//			if (i%6 == 0)
//				fprintf(classfile,"\n");
			if (sinverr == 0 )
//				fprintf(classfile,"%12.5e ",clvcv[class][i]);
				teachdata->clvcv[class][i] = clvcv[class][i];
			else
//				fprintf(classfile,"%12.5e ",vcv[i]);
				teachdata->clvcv[class][i] = vcv[i];
		}
//		fprintf(classfile,"\n");
	}
	/*
	 * reciprocal of variances: first overall, then per class.
	 * Also, the log-square-root
	 * of the determinant of the variance diagonal matrix.
	 */
	logsqrtdet = 0.0;
	for (h=0; h<nself; h++) {
		i = self[h];
		ivar = psd[i];
		/* prevent overflow */
		if (ivar == 0.0)
			ivar = 1.0;
		logsqrtdet -= log(ivar);
		ivar *= ivar;
		ivar = 1.0/ivar;
//		if (h%6 == 0)
//			fprintf(classfile,"\n");
//		fprintf(classfile,"%12.5e ",ivar);
		teachdata->invvar[h] = ivar;
	}
//	fprintf(classfile,"\n%12.5e",logsqrtdet);
	teachdata->invvarsqrtdet = logsqrtdet;
#endif MIZAR
	/*
	 * inverted variances and log of square root of
	 * determinant of variance diagonal matrix
	 */
	for (class=0; class < nclass; class++) {
		logsqrtdet = 0.0;
		conversionf[class] = 0.0;
		for (h=0; h<nself; h++) {
			i = self[h];
			ivar = clsd[class][i];
			/* prevent overflow */
			if (ivar == 0.0)
				ivar = 1.0;
			logsqrtdet -= log(ivar);
			ivar *= ivar;
			ivar = 1.0/ivar;
			if (ivar > conversionf[class])
				conversionf[class] = ivar;
#ifndef MIZAR
//			if (h%6 == 0)
//				fprintf(classfile,"\n");
//			fprintf(classfile,"%12.5e ",ivar);
			teachdata->classinvvars[class][h] = ivar;
#endif MIZAR
		}
//		fprintf(classfile,"\n%12.5e",logsqrtdet);
		teachdata->clinvvarsqrtdet[class] = logsqrtdet;
	}

	/* integer means */
	for (class=0; class<nclass; class++) {
		for (h=0; h<nself; h++) {
			i = self[h];
			mm = clm[class][i];
			im = (mm >= 0? mm+0.49: mm-0.49);
//			if (h%16 == 0)
//				fprintf(classfile,"\n");
//			fprintf(classfile,"%6d ",im);
			teachdata->iclassmeans[class][h] = im;
		}
	}

	/* integer inverted variances and scaling factor */
	for (class=0; class<nclass; class++) {
		conversionf[class] = 1000.0/conversionf[class];
//		fprintf(classfile,"\n%12.5e",conversionf[class]);
		teachdata->conversionf[class] = conversionf[class];
		for (h=0; h<nself; h++) {
			i = self[h];
			ivar = clsd[class][i];
			/* prevent overflow */
			if (ivar == 0.0)
				ivar = 1.0;
			ivar *= ivar;
			intvar = conversionf[class]/ivar + 0.49;
//			if (h%16 == 0)
//				fprintf(classfile,"\n");
//			fprintf(classfile,"%6d ",intvar);
			teachdata->iclassinvvars[class][h] = intvar;
		}
	}
//	fprintf(classfile,"\n");
}

trainclass_d(nself,self,nclass,ndim,clvcv,clm,clsd,m,vcv,clsqrtdet,psd,teachdata,rpt)
double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2];
double vcv[MAXDIM*(MAXDIM+1)/2], clsqrtdet[MAXDIM];
int *self;
double clm[MAXCLASS][MAXDIM], m[MAXDIM], psd[MAXDIM];
double clsd[MAXCLASS][MAXDIM];
VcvTeachData *teachdata;
FILE *rpt;
{
	int im,intvar,g,h,i,j,k,class,err,sinverr;
	double conversionf[MAXCLASS];
	double logsqrtdet, mm, ivar, sqrtdet;

	sinverr = 0;
	sinv(vcv,nself,0.0000000001,&err,&sqrtdet);
	if (err != 0) {
		fprintf(rpt,"error %d inverting pooled variance-covariance matrix\n",err);
	}
	for (class=0; class<nclass; class++) {
		sinv(&clvcv[class][0],nself,0.0000000001,&err,&clsqrtdet[class]);
		if (err != 0) {
			fprintf(rpt,"error %d inverting vcv matrix for class %d\n",err,class+1);
			sinverr = 1;
		}
	}

	/*
	 * store class mean vectors and variance-covariance matrices.
	 * First, selected feature serial numbers.
	 */
	teachdata->nclass = nclass;
	teachdata->ndim = ndim;
	teachdata->nself = nself;

	for (i=0; i<nself; i++)
		teachdata->self[i] = self[i];

	/* class means */
	for (class=0; class<nclass; class++) {
		for (h=0; h<nself; h++) {
			i = self[h];
			// Andy FIX
			//teachdata->classmeans[class][i] = clm[class][i];
			teachdata->classmeans[class][h] = clm[class][i];
		}
	}

	/* pooled inverted variance-covariance matrix */
	teachdata->sqrtdet = sqrtdet;
	for (i=0; i<nself*(nself+1)/2; i++) {
		teachdata->vcv[i] = vcv[i];
	}

	/* per class inverted variance-covariance matrices */
	for (class=0; class<nclass; class++) {
		if (sinverr == 0 )
			teachdata->clsqrtdet[class] = clsqrtdet[class];
		else
			teachdata->clsqrtdet[class] = sqrtdet;
		for (i=0; i<nself*(nself+1)/2; i++) {
			if (sinverr == 0 )
				teachdata->clvcv[class][i] = clvcv[class][i];
			else
				teachdata->clvcv[class][i] = vcv[i];
		}
	}
	/*
	 * reciprocal of variances: first overall, then per class.
	 * Also, the log-square-root
	 * of the determinant of the variance diagonal matrix.
	 */
	logsqrtdet = 0.0;
	for (h=0; h<nself; h++) {
		i = self[h];
		ivar = psd[i];
		/* prevent overflow */
		if (ivar == 0.0)
			ivar = 1.0;
		logsqrtdet -= log(ivar);
		ivar *= ivar;
		ivar = 1.0/ivar;
		teachdata->invvar[h] = ivar;
	}
	teachdata->invvarsqrtdet = logsqrtdet;
	/*
	 * inverted variances and log of square root of
	 * determinant of variance diagonal matrix
	 */
	for (class=0; class < nclass; class++) {
		logsqrtdet = 0.0;
		conversionf[class] = 0.0;
		for (h=0; h<nself; h++) {
			i = self[h];
			ivar = clsd[class][i];
			/* prevent overflow */
			if (ivar == 0.0)
				ivar = 1.0;
			logsqrtdet -= log(ivar);
			ivar *= ivar;
			ivar = 1.0/ivar;
			if (ivar > conversionf[class])
				conversionf[class] = ivar;
			teachdata->classinvvars[class][h] = ivar;
		}
		teachdata->clinvvarsqrtdet[class] = logsqrtdet;
	}

	/* integer means */
	for (class=0; class<nclass; class++) {
		for (h=0; h<nself; h++) {
			i = self[h];
			mm = clm[class][i];
			im = (mm >= 0? mm+0.49: mm-0.49);
			teachdata->iclassmeans[class][h] = im;
		}
	}

	/* integer inverted variances and scaling factor */
	for (class=0; class<nclass; class++) {
		conversionf[class] = 1000.0/conversionf[class];
		teachdata->conversionf[class] = conversionf[class];
		for (h=0; h<nself; h++) {
			i = self[h];
			ivar = clsd[class][i];
			/* prevent overflow */
			if (ivar == 0.0)
				ivar = 1.0;
			ivar *= ivar;
			intvar = conversionf[class]/ivar + 0.49;
			teachdata->iclassinvvars[class][h] = intvar;
		}
	}
}

#ifndef MIZAR
#ifdef PRINTVCV
printtriang (o,a,n)
FILE *o;
double *a;
{
	int i,j,k;
	for (i=1; i<=n; i++) {
		for (j=1; j<i; j++) {
			if ((j-1)%8 == 0)
				fprintf(o,"\n");
			fprintf(o,"              ");
		}
		k = i*(i+1)/2 - 1;
		for (j=i; j<=n; j++) {
			if ((j-1)%8 == 0)
				fprintf(o,"\n");
			fprintf(o,"  %12.5e",a[k]);
			k += j;
		}
		fprintf(o,"\n");
	}
}
#endif PRINTVCV
#endif MIZAR


