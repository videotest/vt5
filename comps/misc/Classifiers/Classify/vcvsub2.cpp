#include "StdAfx.h"
#include <stdio.h>
//#include <wstruct.h>
//#include <chromanal.h>
#include <math.h>
#include <float.h>
#include "Vcv.h"

/* print per class means, standard deviations */
void classmeansd(int STATS,int i,int nclass,int nsamp,int *clnsamp,double clm[MAXCLASS][MAXDIM],
	double clsd[MAXCLASS][MAXDIM],double m[MAXDIM],double s[MAXDIM],double cls[MAXCLASS][MAXDIM],
	double clss[MAXCLASS][MAXDIM],FILE *rpt)
{
	int _class;
	if (STATS > 1 && rpt)
		fprintf(rpt,"\f\nclass   n      mean     standard deviation       for feature %d\n",i);
	m[i] = s[i]/nsamp;
	for (_class=0; _class<nclass; _class++) {
		if (clnsamp[_class] > 0)
			clm[_class][i] = cls[_class][i]/clnsamp[_class];
		else
			clm[_class][i] = 0.0;
		if (clnsamp[_class] > 1)
			clsd[_class][i] = sqrt((clss[_class][i] - clnsamp[_class]*clm[_class][i]*clm[_class][i])/(clnsamp[_class]-1));
		else
			clsd[_class][i] = 0.0;
		if (STATS > 1 && rpt) {
			fprintf(rpt,"%2d   %4d  ",_class+1,clnsamp[_class]);
			fprintf(rpt," %8.2f",clm[_class][i]);
			fprintf(rpt," %8.2f\n",clsd[_class][i]);
		}
	}
}


void classfandv(int STATS,int i,int nclass,int *cpos,char *split,int *clnsamp,double clm[MAXCLASS][MAXDIM],
	double clsd[MAXCLASS][MAXDIM],double V[MAXDIM][8],FILE *rpt)
{
	int j,k,clasi,clasj,_class;
	double ss,t,msd;
	if (STATS > 1 && rpt) {
		fprintf(rpt,"\nF values for feature %d\nclass",i);
		for (_class=1; _class < nclass; _class++)
			fprintf(rpt,"%5d",cpos[_class]+1);
		fprintf(rpt,"\n");
		for (clasi=0; clasi<nclass-1; clasi++) {
			_class = cpos[clasi];
			if (split[clasi] != ' ') {
				for (j=0; j<nclass*5; j++)
					fputc('-',rpt);
				fprintf(rpt,"\n");
			}
			fprintf(rpt,"%3d  ",_class+1);
			for (j=1; j<=clasi; j++)
				fprintf(rpt,"%c    ",split[j]);
			for (clasj=clasi+1; clasj<nclass; clasj++) {
				j = cpos[clasj];
				ss = (clnsamp[_class]-1)*clsd[_class][i]*clsd[_class][i]
					+ (clnsamp[j]-1)*clsd[j][i]*clsd[j][i];
				ss /= (clnsamp[_class] + clnsamp[j] - 2);
				if (ss == 0.0)
					fprintf(rpt,"%c****",split[clasj]);
				else {
					t = clm[_class][i] - clm[j][i];
					t /= sqrt(ss);
					if (t<0)
						t = -t;
					fprintf(rpt,"%c%4.1f",split[clasj],t);
				}
			}
			fprintf(rpt,"\n");
		}
	}

	/* now for V */
	t = 0;
	ss = 0;
	msd = 0;
	for (_class=0; _class<nclass; _class++) {
		t += clm[_class][i];
		ss += clm[_class][i]*clm[_class][i];
		msd += clsd[_class][i];
	}
	t /= nclass;
	ss = sqrt((ss - nclass*t*t)/(nclass-1));
	msd /= nclass;
	ss = msd==0.?DBL_MAX:ss/msd;
	V[i][0] = ss;
	if (STATS > 1 && rpt) {
		fprintf(rpt,"Overall V : %6.2f\n",ss);
		/* per-denver _class V */
		fprintf(rpt,"Super group:         A      B      C      D      E      F      G\nwithin-group V: ");
	}
	t = 0;
	ss = 0;
	msd = 0;
	j = 0;
	k = 0;
	for (_class=0; _class<nclass; _class++) {
		j++;
		clasi = cpos[_class];
		t += clm[clasi][i];
		ss += clm[clasi][i]*clm[clasi][i];
		msd += clsd[clasi][i];
		if (split[_class+1] != ' ') {
			k++;
			t /= j;
			ss = sqrt((ss - j*t*t)/(j-1));
			msd /= j;
			ss = msd==0.?DBL_MAX:ss/msd;
			if (STATS > 1 && rpt)
				fprintf(rpt," %6.2f",ss);
			V[i][k] = ss;
			t = 0;
			ss = 0;
			msd = 0;
			j = 0;
		}
	}
	if (STATS > 1 && rpt)
		fprintf(rpt,"\n");
}


/*
 * compute overall correlations between features
 */
void correlations(int STATS,int ndim,double cp[MAXDIM][MAXDIM],int nsamp,double m[MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt)
{
	int i,j;
	double cc,cd;
	if (STATS) {
		fprintf(rpt,"\n\f\nOverall correlation coefficients between features\n\nfeature");
		for (i=0; i<ndim-1; i++) {
			fprintf(rpt,"  %2d  ",i+1);
			if (i%20 == 0 && i != 0 && i!=ndim-1)
				fprintf(rpt,"\n       ");
		}
		fprintf(rpt,"\n\n");
	}
	for (i=0; i<ndim; i++)
		correl[i][i] = 1.0;
	for (i=0; i<ndim-1; i++) {
		if (STATS) {
			fprintf(rpt," %2d: ",i);
			for (j=0; j<i; j++) {
				fprintf(rpt," .  . ");
				if (j%20 == 0 && j != 0 && j!=ndim-1)
					fprintf(rpt,"\n     ");
			}
		}
		for (j=i+1; j<ndim; j++) {
			cc = (cp[i][j] - nsamp*m[i]*m[j]);
			cd = sqrt((cp[i][i] - nsamp*m[i]*m[i])*(cp[j][j] - nsamp*m[j]*m[j]));
			cc /= cd;
			correl[i][j] = correl[j][i] = cc;
			if (STATS) {
				fprintf(rpt," %5.2f",cc);
				if ((j-1)%20 == 0 && j != 1 && j!=ndim-1)
					fprintf(rpt,"\n     ");
			}
		}
		if (STATS)
			fprintf(rpt,"\n");
	}
}


/* Overall and per-Super-group V */
void Vtable(int ndim,double V[MAXDIM][8],FILE *rpt)
{
	int i,j;
	fprintf(rpt,"\f\nV-values    Overall             Within Super groups\nFeature               A      B      C      D      E      F      G\n");
	for (i=0; i<ndim; i++) {
		fprintf(rpt,"\n%2d:        ",i);
		for (j=0; j<8; j++)
			fprintf(rpt," %6.2f",V[i][j]);
		fprintf(rpt,"\n");
	}
}


//#ifndef MIZAR
/*
 * compute mean within-class correlations between features
 */
void withinclasscorrel(int STATS,int ndim,int nclass,int nsamp,double clcp[MAXCLASS][MAXDIM][MAXDIM],
	int *clnsamp,double clm[MAXCLASS][MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt)
{
	int i,j,c;
	double wccc,cc,cd;
	if (STATS) {
		fprintf(rpt,"\n\f\nMean within-class correlation coefficients between features\n\nfeature");
		for (i=0; i<ndim-1; i++) {
			fprintf(rpt,"  %2d  ",i+1);
			if (i%20 == 0 && i != 0 && i!=ndim-1)
				fprintf(rpt,"\n       ");
		}
		fprintf(rpt,"\n\n");
	}
	for (i=0; i<ndim; i++)
		correl[i][i] = 1.0;
	for (i=0; i<ndim-1; i++) {
		if (STATS) {
			fprintf(rpt," %2d: ",i);
			for (j=0; j<i; j++) {
				fprintf(rpt," .  . ");
				if (j%20 == 0 && j != 0 && j!=ndim-1)
					fprintf(rpt,"\n     ");
			}
		}
		for (j=i+1; j<ndim; j++) {
			wccc = 0.0;
			for (c = 0; c < nclass; c++) {
				cc = (clcp[c][i][j] - clnsamp[c]*clm[c][i]*clm[c][j]);
				cd = sqrt((clcp[c][i][i] - clnsamp[c]*clm[c][i]*clm[c][i])*(clcp[c][j][j] - clnsamp[c]*clm[c][j]*clm[c][j]));
				cc /= cd;
				wccc += clnsamp[c]*cc;
			}
			correl[i][j] = correl[j][i] = wccc/nsamp;
			if (STATS) {
				fprintf(rpt," %5.2f",correl[i][j]);
				if ((j-1)%20 == 0 && j != 1 && j!=ndim-1)
					fprintf(rpt,"\n     ");
			}
		}
		if (STATS)
			fprintf(rpt,"\n");
	}
}


/*
 * compute per-class correlations between features
 */
void perclasscorrel(int STATS,int ndim,int nclass,int nsamp,int c,double clcp[MAXCLASS][MAXDIM][MAXDIM],
	int *clnsamp,double clm[MAXCLASS][MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt)
{
	int i,j;
	double cc,cd;
	if (STATS > 1) {
		fprintf(rpt,"\n\f\nCorrelation coefficients between features for class %d\n\nfeature",c);
		for (i=0; i<ndim-1; i++) {
			fprintf(rpt,"  %2d  ",i+1);
			if (i%20 == 0 && i != 0 && i!=ndim-1)
				fprintf(rpt,"\n       ");
		}
		fprintf(rpt,"\n\n");
	}
	for (i=0; i<ndim; i++)
		correl[i][i] = 1.0;
	for (i=0; i<ndim-1; i++) {
		if (STATS > 1) {
			fprintf(rpt," %2d: ",i);
			for (j=0; j<i; j++) {
				fprintf(rpt," .  . ");
				if (j%20 == 0 && j != 0 && j!=ndim-1)
					fprintf(rpt,"\n     ");
			}
		}
		for (j=i+1; j<ndim; j++) {
			cc = (clcp[c][i][j] - clnsamp[c]*clm[c][i]*clm[c][j]);
			cd = sqrt((clcp[c][i][i] - clnsamp[c]*clm[c][i]*clm[c][i])*(clcp[c][j][j] - clnsamp[c]*clm[c][j]*clm[c][j]));
			correl[i][j] = correl[j][i] = cc/cd;
			if (STATS > 1) {
				fprintf(rpt," %5.2f",correl[i][j]);
				if ((j-1)%20 == 0 && j != 1 && j!=ndim-1)
					fprintf(rpt,"\n     ");
			}
		}
		if (STATS > 1)
			fprintf(rpt,"\n");
	}
}
//#endif MIZAR


/*
 * compute correlations between class feature means
 */
void betweenclasscorrel(int STATS,int ndim,int nclass,int nsamp,int *clnsamp,double clm[MAXCLASS][MAXDIM],
	double m[MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt)
{
	double sd[MAXDIM];
	int i,j,c;
	double wccc,cd;

	for (i=0; i<ndim; i++) {
		cd = 0.0;
		for (c=0; c<nclass; c++) {
			cd += clm[c][i] * clm[c][i] * clnsamp[c];
		}
		sd[i] = sqrt((cd - nsamp * m[i] * m[i])/(nsamp-1));
	}
	if (STATS && rpt) {
		fprintf(rpt,"\n\f\nWeighted between-class-feature-means correlation coefficients \n\nfeature");
		for (i=0; i<ndim-1; i++) {
			fprintf(rpt,"  %2d  ",i+1);
//			if (i%20 == 0 && i != 0 && i!=ndim-1)
//				fprintf(rpt,"\n       ");
		}
		fprintf(rpt,"\n\n");
	}
	for (i=0; i<ndim; i++)
		correl[i][i] = 1.0;
	for (i=0; i<ndim-1; i++) {
		if (STATS) {
			fprintf(rpt," %2d: ",i);
			for (j=0; j<i; j++) {
				fprintf(rpt," .  . ");
//				if (j%20 == 0 && j != 0 && j!=ndim-1)
//					fprintf(rpt,"\n     ");
			}
		}
		for (j=i+1; j<ndim; j++) {
			wccc = 0.0;
			for (c = 0; c < nclass; c++)
				wccc += clnsamp[c] * (clm[c][i] - m[i]) * (clm[c][j] - m[j]);
			correl[i][j] = correl[j][i] = wccc/(nsamp*sd[i]*sd[j]);
			if (STATS) {
				fprintf(rpt," %5.2f",correl[i][j]);
//				if ((j-1)%20 == 0 && j != 1 && j!=ndim-1)
//					fprintf(rpt,"\n     ");
			}
		}
		if (STATS)
			fprintf(rpt,"\n");
	}
}

//#ifndef MIZAR
/*
 * compute pooled standard deviation and write to file
 */
void totalsd(int STATS,int ndim,int nsamp,double m[MAXDIM],double cp[MAXDIM][MAXDIM],FILE *eigenfile)
{
	double tsd[MAXDIM];
	int i;
	poolsd(STATS,ndim,nsamp,m,cp,tsd,NULL);
	fprintf(eigenfile,"%3d\n",ndim);
	for (i=0; i<ndim; i++) {
		fprintf(eigenfile,"%12e %12e\n",m[i],tsd[i]);
	}
}
//#endif MIZAR


/*
 * compute pooled standard deviations
 */
void poolsd(int STATS,int ndim,int nsamp,double m[MAXDIM],double cp[MAXDIM][MAXDIM],double psd[MAXDIM],FILE *rpt)
{
	int i;
	if (STATS && rpt) {
		fprintf(rpt, "\f\n\nPOOLED MEANS AND STANDARD DEVIATIONS\n");
		fprintf(rpt,      "------------------------------------\n\n");
		fprintf(rpt,"feature      mean        s.d.\n\n");
	}
	for (i=0; i<ndim; i++) {
		psd[i] = sqrt((cp[i][i] - nsamp*m[i]*m[i])/(nsamp-1));
		if (STATS && rpt)
			fprintf(rpt, "%2d    %12.4f %12.4f\n",i,m[i],psd[i]);
	}
}
