#if !defined(__Vcv_H__)
#define __Vcv_H__

#include "stdlib.h"
#include "stdio.h"

#define CLASSIFYINFO      "ClassifyInfo"
#define CLASSIFYINFO_INI  "Classes\\ClassifyInfo.ini"


#define MAXDIM		100
#define MAXCLASS	100
#define UNDEFCLASS	MAXCLASS-1  /* is variable in struct screenform, should be removed */
#define MAXCHILD	30
#define MAXOBJS		250
#define LRATIO		-2.0	/* ln(e**(-2)) */
#define MAXCHAIN	3
#define CUTOFF		4


typedef struct _VcvChromoData
{
	int nClass;
	double *pdValues;
//	_bstr_t sSource;
//	int nSourceNum;
} VcvChromoData;

typedef struct _VcvParams
{
	// Feature selection data
	int nFeatureMethod;
	int nFeatures;
	double dMaxcor; // used if nFeatureMethod == 1 
	int *pnExplicitFeatArray; // used if nFeatureMethod == 3 
	// Statistic mode
	int nStat;
	char szRptPath[_MAX_PATH];
	int nClasses;
	int nDim;
	char *pszClassFile;
}VcvParams;

enum TeachFlags
{
	TeachInteger = 1,
	PiperParams = 2,
};

typedef struct _VcvTeachData
{
	int flags;
	int nclass,ndim,nself;
	int self[MAXDIM];
	double classmeans[MAXCLASS][MAXDIM];
	float sqrtdet,clsqrtdet[MAXCLASS];
	float vcv[MAXDIM*(MAXDIM+1)/2];
	float clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2];
	double invvar[MAXDIM];
	double invvarsqrtdet;
	double classinvvars[MAXCLASS][MAXDIM];
	double clinvvarsqrtdet[MAXCLASS];
	int iclassmeans[MAXCLASS][MAXDIM];
	int iclassinvvars[MAXCLASS][MAXDIM];
	double conversionf[MAXCLASS];
	long keys[MAXDIM];
}VcvTeachData;

struct cllik
{
	int _class;
	double lval;
};

struct chromcl
{
	int rawclass;
	int trueclass;
	int newclass;
	int bestclass;
	double moveval;
	struct chchain *movechain;
	struct cllik cllika[CUTOFF];
};

struct chchain
{
	struct chromcl *chrcl;
	double accum_cost;
	int nchildren;
	int _class;
	int bestchild;
	struct chchain *child[MAXCHILD];
};

struct classcont
{
	unsigned char lock;
	int n;
	struct chromcl *chcl[MAXCHILD];
};

struct resultdata
{
	double AvrV; // average V value
};

struct CSelFeats
{
	int *m_pnFeats;
	int m_nAlloc;
	int m_nSelFeats;
	CSelFeats()
	{
		m_pnFeats = NULL;
		m_nAlloc = m_nSelFeats = 0;
	}
	~CSelFeats()
	{
		delete m_pnFeats;
	}
	void Alloc(int nSize)
	{
		if (nSize <= m_nAlloc) return;
		delete m_pnFeats;
		m_pnFeats = new int[nSize];
		m_nAlloc = nSize;
	}
};

struct CSelFeatsFull
{
	CSelFeats m_Sel,m_SelDv[7];
	void Alloc(int nSize, int nSelf = 0)
	{
		m_Sel.Alloc(nSize);
		for (int i = 0; i < 7; i++)
			m_SelDv[i].Alloc(nSize);
		if (nSelf > 0 )
		{
			m_Sel.m_nSelFeats = nSelf;
			for (int i = 0; i < 7; i++)
				m_SelDv[i].m_nSelFeats = nSelf;
		}
	};
};

int TeachClassifier(VcvParams *pParams, int nChromos, VcvChromoData *pChromos, long *keys, resultdata *presult, FILE *rpt);
void makevcv(int nself,int *self,int nclass,int nsamp,double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2],
	double clcp[MAXCLASS][MAXDIM][MAXDIM],int *clnsamp,double cp[MAXDIM][MAXDIM],double clm[MAXCLASS][MAXDIM],
	double m[MAXDIM],double vcv[MAXDIM*(MAXDIM+1)/2]);
//void trainclass(int nself,int *self,int nclass,int ndim,double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2],
//	double clm[MAXCLASS][MAXDIM],double clsd[MAXCLASS][MAXDIM],double m[MAXDIM],double vcv[MAXDIM*(MAXDIM+1)/2],
//	double clsqrtdet[MAXDIM],double psd[MAXDIM],long *keys,FILE *rpt);
void trainclass(CSelFeatsFull &Self,int nclass,int ndim, double clm[MAXCLASS][MAXDIM],
	double clsd[MAXCLASS][MAXDIM],long *keys,const char *pszClassFile,FILE *rpt);
void classmeansd(int STATS,int i,int nclass,int nsamp,int *clnsamp,double clm[MAXCLASS][MAXDIM],
	double clsd[MAXCLASS][MAXDIM],double m[MAXDIM],double s[MAXDIM],double cls[MAXCLASS][MAXDIM],
	double clss[MAXCLASS][MAXDIM],FILE *rpt);
void classfandv(int STATS,int i,int nclass,int *cpos,char *split,int *clnsamp,double clm[MAXCLASS][MAXDIM],
	double clsd[MAXCLASS][MAXDIM],double V[MAXDIM][8],FILE *rpt);
void correlations(int STATS,int ndim,double cp[MAXDIM][MAXDIM],int nsamp,double m[MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt);
void Vtable(int ndim,double V[MAXDIM][8],FILE *rpt);
void withinclasscorrel(int STATS,int ndim,int nclass,int nsamp,double clcp[MAXCLASS][MAXDIM][MAXDIM],
	int *clnsamp,double clm[MAXCLASS][MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt);
void perclasscorrel(int STATS,int ndim,int nclass,int nsamp,int c,double clcp[MAXCLASS][MAXDIM][MAXDIM],
	int *clnsamp,double clm[MAXCLASS][MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt);
void betweenclasscorrel(int STATS,int ndim,int nclass,int nsamp,int *clnsamp,double clm[MAXCLASS][MAXDIM],
	double m[MAXDIM],double correl[MAXDIM][MAXDIM],FILE *rpt);
void totalsd(int STATS,int ndim,int nsamp,double m[MAXDIM],double cp[MAXDIM][MAXDIM],FILE *eigenfile);
void poolsd(int STATS,int ndim,int nsamp,double m[MAXDIM],double cp[MAXDIM][MAXDIM],double psd[MAXDIM],FILE *rpt);
void sepcor(int *self,int *nself,double V[MAXDIM][8],double correl[MAXDIM][MAXDIM],int ndim,
	double maxcor,FILE *rpt);
void modsepcor(CSelFeatsFull &Self,double V[MAXDIM][8],double correl[MAXDIM][MAXDIM],int ndim,FILE *rpt);
void selectedfeatures(int nparam, long *keys, int *sfv,int *nsfv);
void sinv(double *a,int n,double eps,int *ier,double *sqrtdet);


typedef int (__cdecl *COMPARE)(const void *elem1, const void *elem2 );

// Classification procedure
void DoClassify(IUnknown *punkObjectsList, IUnknown *punkDoc, _ptr_t2<int> &arrClasses, bool bRearrange);
void SetClasses(IUnknown *punkObjectsList, _ptr_t2<int> &arrClasses);


#endif