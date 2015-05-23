#if !defined(__Vcv_H__)
#define __Vcv_H__

#include "stdlib.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define MAXDIM1       100
#define MAXCLASS1     100


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
	int self[MAXDIM1];
	double classmeans[MAXCLASS1][MAXDIM1];
	float sqrtdet,clsqrtdet[MAXCLASS1];
	float vcv[MAXDIM1*(MAXDIM1+1)/2];
	float clvcv[MAXCLASS1][MAXDIM1*(MAXDIM1+1)/2];
	double invvar[MAXDIM1];
	double invvarsqrtdet;
	double classinvvars[MAXCLASS1][MAXDIM1];
	double clinvvarsqrtdet[MAXCLASS1];
	int iclassmeans[MAXCLASS1][MAXDIM1];
	int iclassinvvars[MAXCLASS1][MAXDIM1];
	double conversionf[MAXCLASS1];
	long keys[MAXDIM1];
}VcvTeachData;

int Vcv(VcvParams *pParams, int nChromos, VcvChromoData *pChromos, VcvTeachData *pTeachData, int bNativeMode, FILE *rpt);


#if defined(__cplusplus)
};
#endif







#endif