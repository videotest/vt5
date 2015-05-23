#include "stdafx.h"
#include "LikClassifier.h"
#include "misc_calibr.h"
#include "misc_templ.h"
#include "Classes5.h"
#include "misc_utils.h"
#include "WoolzObject.h"
#include "VcvData.h"
#include "StdioFile.h"
#include "ChildArray.h"
#include "class_utils.h"

#define COORD ICOORD
#define __NOTEXT
extern "C"
{
int getdim(int);
void getvec(register struct chromplist *plist, register int *v);
void normvec(struct chromosome **objs,register int number,struct normaliser *vn);
void normivec(register int *fvec,register int *ivec,int *sfv,register struct normaliser *vn,register int ndim,register int nsfv);
int iclassdist(int *fvec,double *priorp,struct cllik *liklv,int nclass,int ndim);
int _classdist(double *fvec,double *lpriorp,register struct cllik *liklv,int nclass,int ndim);
int rearrange(struct chromcl *chrcl,struct classcont *clcontents,int numobj,int *clwn,int nclass);
int _initmahalanobis(int dim);
int liksort(const void *, const void *);

#include "wstruct.h"
#define class _class
#include "chromanal.h"
#undef class


extern double clinvvarsqrtdet[MAXCLASS];

extern int iclassmeans[MAXCLASS][MAXDIM];
extern int iclassinvvars[MAXCLASS][MAXDIM];
extern double conversionf[MAXCLASS];
extern double classmeans[MAXCLASS][MAXDIM];
extern double classinvvars[MAXCLASS][MAXDIM];
extern double invvar[MAXDIM];
extern float sqrtdet,clsqrtdet[MAXCLASS];
extern float vcv[MAXDIM*(MAXDIM+1)/2];
extern float clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2];
extern int **tabmahal;
extern double invvarsqrtdet;
}

#include "Vcv.h"

typedef struct chromosome Chromosome;
typedef struct classcont Classcont;
typedef struct chromcl Chromcl;

static void read_class_params(VcvTeachData *teachdata, int *nclass, int *ndim, int *sf, int *nsf, FILE *rpt)
{
	register int i,j;
/* dimensions and selected feature serial numbers */
	*nclass = teachdata->nclass;
	*ndim = teachdata->ndim;
	*nsf = teachdata->nself;
	if (rpt) fprintf(rpt, "nclass = %d, ndim = %d, nsf = %d\nSelected features :\n", *nclass, *ndim, *nsf);
	for (i=0; i<*nsf; i++)
	{
		sf[i] = teachdata->self[i];
		if (rpt) fprintf(rpt, "%d ", sf[i]);
	}
	if (rpt) fprintf(rpt, "\n");

	if (!(teachdata->flags&TeachInteger))
	{
		// class means
		for (i=0; i<*nclass; i++)
			for (j=0; j<*nsf; j++)
				classmeans[i][j] = teachdata->classmeans[i][j];
		// pooled inverted variance-covariance matrix
		sqrtdet = teachdata->sqrtdet;
		for (i=0; i<*nsf*(*nsf+1)/2; i++)
			vcv[i] = teachdata->vcv[i];
		// per-class inverted variance-covariance matrices
		for (j=0; j<*nclass; j++)
		{
			clsqrtdet[j] = teachdata->clsqrtdet[j];
			for (i=0; i<*nsf*(*nsf+1)/2; i++)
				clvcv[j][i] = teachdata->clvcv[j][i];
		}
		// pooled inverted variances
		for (i=0; i<*nsf; i++)
			invvar[i] = teachdata->invvar[i];
		invvarsqrtdet = teachdata->invvarsqrtdet;
	}

	// per class inverted variances
	if (rpt) fprintf(rpt, "Per class invariances:\nclass invariance\n");
	for (i=0; i<*nclass; i++)
	{
		if (!(teachdata->flags&TeachInteger))
		{
			for (j=0; j<*nsf; j++)
				classinvvars[i][j] = teachdata->classinvvars[i][j];
		}
		clinvvarsqrtdet[i] = teachdata->clinvvarsqrtdet[i];
		if (rpt) fprintf(rpt, "%d %g\n", i, clinvvarsqrtdet[i]);
	}
	if (teachdata->flags&TeachInteger)
	{
		// class integer means
		if (rpt) fprintf(rpt, "Class means:\nclass mean\n");
		for (i=0; i<*nclass; i++)
		{
			if (rpt) fprintf(rpt, "%6d", i);
			for (j=0; j<*nsf; j++)
			{
				iclassmeans[i][j] = teachdata->iclassmeans[i][j];
				if (rpt) fprintf(rpt, " %6d", iclassmeans[i][j]);
			}
			if (rpt) fprintf(rpt, "\n");
		}
		//* per class integer inverted variances
		if (rpt) fprintf(rpt, "Per class integer inverted variances:\nclass conversionf[i] iclassinvvars[i][j]\n");
		for (i=0; i<*nclass; i++)
		{
			conversionf[i] = teachdata->conversionf[i];
			if (rpt) fprintf(rpt, "%6d     %g    ", i, conversionf[i]);
			for (j=0; j<*nsf; j++)
			{
				iclassinvvars[i][j] = teachdata->iclassinvvars[i][j];
				if (rpt) fprintf(rpt, "%6d", iclassinvvars[i][j]);
			}
			if (rpt) fprintf(rpt, "\n");
		}
	}
	else
		_initmahalanobis(*nsf);
}

static void _getself(ICalcObjectPtr sptrObj, int nsfv, int *sfv, double *svec)
{
	for (int i = 0; i < nsfv; i++)
	{
		HRESULT hr = sptrObj->GetValue(sfv[i], &svec[i]);
		if (FAILED(hr))
			svec[i] = 0.;
	}
}

CVcvDataArray::CVcvDataArray()
{
	pArray = NULL;
	nSize = 0;
	nFeatures = 0;
	plFeatKeys = NULL;
}

CVcvDataArray::~CVcvDataArray()
{
	for (int i = 0; i < nSize; i++)
	{
		if (pArray[i].pdValues) free(pArray[i].pdValues);
	}
	free(pArray);
	if (plFeatKeys) free(plFeatKeys);
}

void CVcvDataArray::Add(int nSize)
{
	if (pArray)
	{
		VcvChromoData *p = (VcvChromoData *)calloc(this->nSize+nSize,sizeof(VcvChromoData));
		memcpy(p, pArray, this->nSize*sizeof(VcvChromoData));
		free(pArray);
		pArray = p;
	}
	else
		pArray = (VcvChromoData *)calloc(nSize,sizeof(VcvChromoData));
	this->nSize += nSize;
}

bool CVcvDataArray::InitFeatures(ICalcObjectPtr sptrObj, IUnknown *punkObjectsList)
{
	if (plFeatKeys) return true;
	ICalcObjectContainerPtr sptrCOCObjList(punkObjectsList);
	// Calculate total number of features.
	long nFeatAll = 0;
	sptrCOCObjList->GetParameterCount(&nFeatAll);
	_ptr_t<long> arrAllFeats(nFeatAll);
	_ptr_t<_bstr_t> arrAllFeatNames(nFeatAll);
	int nFeatEn = 0; // we will count number of features enabled for classification
	long pos;
	long key;
	double value;
	struct ParameterContainer *pParamDef;
	sptrObj->GetFirstValuePos(&pos);
	while(pos)
	{
		sptrObj->GetNextValue(&pos, &key, &value);
		sptrCOCObjList->ParamDefByKey(key, &pParamDef);
		if (!pParamDef || !pParamDef->pDescr)
			continue;
		_bstr_t bstrParamName = pParamDef->pDescr->bstrName;
		int nEnabled = ::GetValueInt(::GetAppUnknown(), "Classification\\Parameters", bstrParamName, 0);
		if (nEnabled)
			arrAllFeats[nFeatEn++] = key;
	}
	if (nFeatEn == 0)
		return false;
	plFeatKeys = (long *)malloc(nFeatEn*sizeof(int));
	for (int i = 0; i < nFeatEn; i++)
		plFeatKeys[i] = arrAllFeats[i];
	nFeatures = nFeatEn;
	return true;
}

void CVcvDataArray::InitFeatures(int nFeats)
{
	nFeatures = nFeats;
	if (nFeatures)
		plFeatKeys = (long *)malloc(nFeats*sizeof(int));
}


struct VcvParamsHld : public VcvParams
{
	VcvParamsHld()
	{
		nFeatureMethod = ::GetValueInt(GetAppUnknown(), "Woolz", "FeatureSelectionMethod", 3);
		switch (nFeatureMethod)
		{
		case 1:
			nFeatures = ::GetValueInt(GetAppUnknown(), "Woolz", "Features", 1);
			dMaxcor = ::GetValueDouble(GetAppUnknown(), "Woolz", "Maxcor", 0.);
			break;
		case 2:
			nFeatures = ::GetValueInt(GetAppUnknown(), "Woolz", "Features", 1);
			break;
		case 3:
			nFeatures = ::GetValueInt(GetAppUnknown(), "Woolz", "Features", 1);
			pnExplicitFeatArray = new int[nFeatures];
			for (int i = 0; i < nFeatures; i++)
			{
				char szBuff[50];
				sprintf(szBuff, "Param%d", i+1);
				pnExplicitFeatArray[i] = ::GetValueInt(GetAppUnknown(), "Woolz", szBuff, i);
			}
			break;
		}
		nStat = ::GetValueInt(GetAppUnknown(), "Woolz", "Statistics", 1);
		if (nStat)
		{
			char szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
			GetModuleFileName(NULL,szRptPath,_MAX_PATH);
			_splitpath(szRptPath,szDrive,szDir,NULL,NULL);
			_makepath(szRptPath,szDrive,szDir,"train.txt", NULL);
		}
	}
	~VcvParamsHld()
	{
		if (nFeatureMethod == 3)
			delete pnExplicitFeatArray;
	}
};

CLikClassifier::CLikClassifier()
{
	pVcvData = new CVcvDataArray;
	pTeachData = NULL;//new VcvTeachData;
}

CLikClassifier::~CLikClassifier()
{
	delete (CVcvDataArray *)pVcvData;
	delete pTeachData;
}

HRESULT CLikClassifier::Load( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;
	ULONG nRead = 0;
	int	nVersion;
	pStream->Read(&nVersion, sizeof(nVersion), &nRead);
	// Read array of measured objects.
	BOOL bObjArray;
	pStream->Read(&bObjArray, sizeof(bObjArray), &nRead);
	if (bObjArray)
	{
		if (pVcvData) delete pVcvData;
		pVcvData = new CVcvDataArray;
		CVcvDataArray &VcvData = (*(CVcvDataArray *)pVcvData);
		int nSize;
		pStream->Read(&nSize, sizeof(nSize), &nRead);
		VcvData.Add(nSize);
		int nValues;
		pStream->Read(&nValues, sizeof(int), &nRead);
		VcvData.InitFeatures(nValues);
		pStream->Read(VcvData.plFeatKeys, nValues*sizeof(long), &nRead);
		for (int i = 0; i < nSize; i++)
		{
			int nClass;
			pStream->Read(&nClass , sizeof(nClass ), &nRead);
			VcvData[i].nClass = nClass;
			double *pdValues = (double *)malloc(nValues*sizeof(double));;
			for (int j = 0; j < nValues; j++)
				pStream->Read(&pdValues[j], sizeof(pdValues[j]), &nRead);
			VcvData[i].pdValues = pdValues;
		}
	}
	// Read vcv data.
	BOOL bVcvData;
	pStream->Read(&bVcvData, sizeof(bVcvData), &nRead);
	if (bVcvData)
	{
		if (pTeachData) delete pTeachData;
		pTeachData = new VcvTeachData;
		VcvTeachData &TeachData = *(VcvTeachData *)pTeachData;
		int _class,i;
		pStream->Read(&TeachData.flags, sizeof(TeachData.flags), &nRead);
		pStream->Read(&TeachData.nclass, sizeof(TeachData.nclass), &nRead);
		pStream->Read(&TeachData.ndim, sizeof(TeachData.ndim), &nRead);
		pStream->Read(&TeachData.nself, sizeof(TeachData.nself), &nRead);
		for (i = 0; i < TeachData.nself; i++)
			pStream->Read(&TeachData.self[i], sizeof(TeachData.self[i]), &nRead);
		BOOL bInt = TeachData.flags&TeachInteger;
		if (!bInt)
		{
			for (_class=0; _class < TeachData.nclass; _class++)
			{
				for (int h=0; h < TeachData.nself; h++)
				{
//					i = TeachData.self[h];
					pStream->Read(&TeachData.classmeans[_class][h], sizeof(TeachData.classmeans[_class][h]), &nRead);
				}
			}
			pStream->Read(&TeachData.sqrtdet, sizeof(TeachData.sqrtdet), &nRead);
			for (i=0; i < TeachData.nself*(TeachData.nself+1)/2; i++)
				pStream->Read(&TeachData.vcv[i], sizeof(TeachData.vcv[i]), &nRead);
			for (_class=0; _class < TeachData.nclass; _class++)
			{
				pStream->Read(&TeachData.clsqrtdet[_class], sizeof(TeachData.clsqrtdet[_class]), &nRead);
				for (i=0; i < TeachData.nself*(TeachData.nself+1)/2; i++)
					pStream->Read(&TeachData.clvcv[_class][i], sizeof(TeachData.clvcv[_class][i]), &nRead);
			}
			for (i = 0; i < TeachData.nself; i++)
			{
				pStream->Read(&TeachData.invvar[i], sizeof(TeachData.invvar[i]), &nRead);
			}
			pStream->Read(&TeachData.invvarsqrtdet, sizeof(TeachData.invvarsqrtdet), &nRead);
		}
		for (_class=0; _class < TeachData.nclass; _class++)
		{
			if (!bInt)
			{
				for (i=0; i < TeachData.nself; i++)
					pStream->Read(&TeachData.classinvvars[_class][i], sizeof(TeachData.classinvvars[_class][i]), &nRead);
			}
			pStream->Read(&TeachData.clinvvarsqrtdet[_class], sizeof(TeachData.clinvvarsqrtdet[_class]), &nRead);
		}
		for (_class=0; _class < TeachData.nclass; _class++)
		{
			for (i=0; i < TeachData.nself; i++)
				pStream->Read(&TeachData.iclassmeans[_class][i], sizeof(TeachData.iclassmeans[_class][i]), &nRead);
		}
		for (_class=0; _class < TeachData.nclass; _class++)
		{
			pStream->Read(&TeachData.conversionf[_class], sizeof(TeachData.conversionf[_class]), &nRead);
			for (i=0; i < TeachData.nself; i++)
				pStream->Read(&TeachData.iclassinvvars[_class][i], sizeof(TeachData.iclassinvvars[_class][i]), &nRead);
		}
	}
	return CObjectBase::Load(pStream, pparams);
}

HRESULT CLikClassifier::Store( IStream *pStream, SerializeParams *pparams )
{
	if(!pStream)
		return E_INVALIDARG;
	int	nVersion = 0;
	ULONG nWritten = 0;
	pStream->Write(&nVersion, sizeof(nVersion), &nWritten);
	// Write array of measured objects.
	BOOL bObjArray = pVcvData!=NULL;
	pStream->Write(&bObjArray, sizeof(bObjArray), &nWritten);
	if (bObjArray)
	{
		CVcvDataArray &VcvData = (*(CVcvDataArray *)pVcvData);
		int nSize = VcvData.GetSize();
		pStream->Write(&nSize, sizeof(nSize), &nWritten);
		int nValues = VcvData.nFeatures;
		pStream->Write(&VcvData.nFeatures, sizeof(int), &nWritten);
		pStream->Write(VcvData.plFeatKeys, nValues*sizeof(long), &nWritten);
		for (int i = 0; i < nSize; i++)
		{
			int nClass = VcvData[i].nClass;
			pStream->Write(&nClass , sizeof(nClass ), &nWritten);
			double *pdValues = VcvData[i].pdValues;
			for (int j = 0; j < nValues; j++)
				pStream->Write(&pdValues[j], sizeof(pdValues[j]), &nWritten);
		}
	}
	// Write vcv data.
	BOOL bVcvData = pTeachData!=NULL;
	pStream->Write(&bVcvData, sizeof(bVcvData), &nWritten);
	if (bVcvData)
	{
		VcvTeachData &TeachData = *(VcvTeachData *)pTeachData;
		int _class,i;
		pStream->Write(&TeachData.flags, sizeof(TeachData.flags), &nWritten);
		pStream->Write(&TeachData.nclass, sizeof(TeachData.nclass), &nWritten);
		pStream->Write(&TeachData.ndim, sizeof(TeachData.ndim), &nWritten);
		pStream->Write(&TeachData.nself, sizeof(TeachData.nself), &nWritten);
		for (i = 0; i < TeachData.nself; i++)
			pStream->Write(&TeachData.self[i], sizeof(TeachData.self[i]), &nWritten);
		if (!(TeachData.flags&TeachInteger))
		{
			for (_class=0; _class < TeachData.nclass; _class++)
			{
				for (int h=0; h < TeachData.nself; h++)
				{
//					i = TeachData.self[h];
					pStream->Write(&TeachData.classmeans[_class][h], sizeof(TeachData.classmeans[_class][h]), &nWritten);
				}
			}
			pStream->Write(&TeachData.sqrtdet, sizeof(TeachData.sqrtdet), &nWritten);
			for (i=0; i < TeachData.nself*(TeachData.nself+1)/2; i++)
				pStream->Write(&TeachData.vcv[i], sizeof(TeachData.vcv[i]), &nWritten);
			for (_class=0; _class < TeachData.nclass; _class++)
			{
				pStream->Write(&TeachData.clsqrtdet[_class], sizeof(TeachData.clsqrtdet[_class]), &nWritten);
				for (i=0; i < TeachData.nself*(TeachData.nself+1)/2; i++)
					pStream->Write(&TeachData.clvcv[_class][i], sizeof(TeachData.clvcv[_class][i]), &nWritten);
			}
			for (i = 0; i < TeachData.nself; i++)
			{
				pStream->Write(&TeachData.invvar[i], sizeof(TeachData.invvar[i]), &nWritten);
			}
			pStream->Write(&TeachData.invvarsqrtdet, sizeof(TeachData.invvarsqrtdet), &nWritten);
		}
		for (_class=0; _class < TeachData.nclass; _class++)
		{
			if (!(TeachData.flags&TeachInteger))
			{
				for (i=0; i < TeachData.nself; i++)
					pStream->Write(&TeachData.classinvvars[_class][i], sizeof(TeachData.classinvvars[_class][i]), &nWritten);
			}
			pStream->Write(&TeachData.clinvvarsqrtdet[_class], sizeof(TeachData.clinvvarsqrtdet[_class]), &nWritten);
		}
		for (_class=0; _class < TeachData.nclass; _class++)
		{
			for (i=0; i < TeachData.nself; i++)
				pStream->Write(&TeachData.iclassmeans[_class][i], sizeof(TeachData.iclassmeans[_class][i]), &nWritten);
		}
		for (_class=0; _class < TeachData.nclass; _class++)
		{
			pStream->Write(&TeachData.conversionf[_class], sizeof(TeachData.conversionf[_class]), &nWritten);
			for (i=0; i < TeachData.nself; i++)
				pStream->Write(&TeachData.iclassinvvars[_class][i], sizeof(TeachData.iclassinvvars[_class][i]), &nWritten);
		}
	}
	return CObjectBase::Store(pStream, pparams);
}

HRESULT CLikClassifier::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = false;
	return S_OK;
}

/* logs of prior probabilities */
static double lpriorp[MAXCLASS] = {
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
0.0, 0.0, 0.0, -0.5};

static int clwn[MAXCLASS] = {
2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 1};

HRESULT CLikClassifier::Classify(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc)
{
	// Check that classifiers was already teached	
	if (!pTeachData) return S_FALSE;
	VcvTeachData *pTeachData = (VcvTeachData *)this->pTeachData; // Teach data
	CChildrenArray arrClasses(punkClassesList,true); // Class list in array form
	sptrINamedDataObject2 sptrN(punkObjectsList); // Object list
	long lObjects; // Number of objects
	sptrN->GetChildsCount(&lObjects);
	// Data block for Piper's code
	_ptr_t<Classcont> clcontents(arrClasses.ChildrenCount());  // Information about class i
	_ptr_t<Chromcl> chrcl(lObjects+1); // Information about object i
	_ptr_t<long> positions(lObjects+1);
//	struct chromcl *chp;
//	struct classcont *classcontp; // pointer to current structure
	// Prepare for statistic output
	_StatFile Stat("classify.txt");
	// Initialize classifier data.
	int nclass,ndim,sfv[MAXDIM],nsfv;
	read_class_params(pTeachData,&nclass,&ndim,sfv,&nsfv,Stat.f);
	// Classify objects in list.
	for (int i=0; i<pTeachData->nclass; i++)
		clcontents[i].n = 0;
	Chromcl *chp;// = chrcl;
	CVcvDataArray &VcvData = (*(CVcvDataArray *)pVcvData); 
	_ptr_t<int> keys(pTeachData->nself);
	_ptr_t<double> svec(pTeachData->nself);
	if (Stat.f) fprintf(Stat.f, "Primary classification\n");
	int iObj = 0, iObjOk = 0;
	long pos = 0;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		ICalcObjectPtr sptrObj;
		long pos1 = pos;
		sptrN->GetNextChild((long*)&pos, &sptr);
		sptrObj = sptr;
		bool bOk = sptrObj;
		if (bOk)
		{
			for (i = 0; i < pTeachData->nself; i++)
				keys[i] = VcvData.plFeatKeys[pTeachData->self[i]];
			_getself(sptrObj, pTeachData->nself, (int *)keys, (double *)svec);
			if (Stat.f)
			{
				fprintf(Stat.f, "Features for object %d:", iObj);
				for(i = 0; i < pTeachData->nself; i++)
					fprintf(Stat.f, " %f", svec[i]);
				fprintf(Stat.f, "\n");
			}
			chp = &chrcl[iObjOk];
			int _class = _classdist((double *)svec,lpriorp,&(chp->cllika[0]),pTeachData->nclass,
				pTeachData->nself) - 1;
			chp->trueclass = -1;
			positions[iObjOk] = pos1;
			if (clcontents[_class].n < MAXCHILD)
			{
				chp->rawclass = _class;
				chp->newclass = _class;
				clcontents[_class].chcl[clcontents[_class].n] = chp;
				clcontents[_class].n++;
			}
			if (Stat.f) fprintf(Stat.f, "Object %d classified as %d\n", iObj, _class+1);
			// sort the class likelihods of this chromosome into order
			if (!(pTeachData->flags&TeachInteger))
				qsort(&(chp->cllika[0]),CUTOFF,sizeof(struct cllik),liksort);
			iObjOk++;
		}
		else
			if (Stat.f) fprintf(Stat.f, "Object %d is strange\n", iObj);
		iObj++;
	}
	// re-arrange so as to move objects from over-subscribed
	// classes to under-subscribed as far as plausibility allows
//#if 0
	if (Stat.f) fprintf(Stat.f, "Rearrangement\n");
	rearrange((Chromcl*)chrcl,(Classcont*)clcontents,lObjects,clwn,pTeachData->nclass);
	for (i = 0; i < iObjOk; i++)
	{
		if (Stat.f) fprintf(Stat.f, "%d rearranged as %d\n", i, chrcl[i].newclass+1);
		pos = positions[i];
		ICalcObjectPtr sptrObj;
		IUnknownPtr sptr;
		sptrN->GetNextChild(&pos, &sptr);
		sptrObj = sptr;
		bool bOk = sptrObj;
		if (bOk && chrcl[i].newclass >= 0)
		{
			IUnknownPtr sptr1 = arrClasses.GetAt(chrcl[i].newclass);
			/*INumeredObjectPtr sptrCNO(sptr1);
			GuidKey lClassKey;
			sptrCNO->GetKey(&lClassKey);*/
			set_object_class( sptrObj, chrcl[i].newclass );
		}
	}
//#endif

	return S_OK;
}


HRESULT CLikClassifier::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = 0;
	return S_OK;
}

HRESULT CLikClassifier::Teach(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc)
{
	if (!punkObjectsList && !punkClassesList && !punkDoc)
	{
		// Empty classifier
		delete pVcvData;
		pVcvData = new CVcvDataArray;
		delete pTeachData;
		pTeachData = NULL;
		return S_OK;
	}
	if (!pTeachData) // Check teach data for compatibility or init it
	{
		pTeachData = new VcvTeachData;
		memset(pTeachData, 0 , sizeof(VcvTeachData));
	}
	else if (((VcvTeachData *)pTeachData)->flags&TeachInteger)
	{
		MessageBox(NULL, "Classifier was teached in integer mode", "ChromExtrn", MB_OK|MB_ICONEXCLAMATION);
		return S_FALSE;
	}
	CVcvDataArray &VcvData = (*(CVcvDataArray *)pVcvData); 
	CChildrenArray arrClasses(punkClassesList,true); // Class list in array form
	sptrINamedDataObject2 sptrN(punkObjectsList); // Objects list
	long lObjects;
	sptrN->GetChildsCount(&lObjects);
	int nObj1 = VcvData.GetSize(); // Add needed records to array
	VcvData.Add(lObjects);
	// Iterate all objects and save feature values
	long pos = 0;
	int nMaxClass = ((VcvTeachData *)pTeachData)->nclass-1;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		sptrN->GetNextChild((long*)&pos, &sptr);
		ICalcObjectPtr sptrObj = sptr;
		bool bOk = sptrObj;
		if (bOk)
		{
			int nClass = get_object_class( sptrObj );
			VcvData[nObj1].nClass = nClass; // save class number
			nMaxClass = max(nMaxClass,nClass);
			if (VcvData.nFeatures == 0) // If this is first object in classifier, initialize objects list
				if (!VcvData.InitFeatures(sptrObj,punkObjectsList))
				{
					MessageBox(NULL, "No parameters, enabled for classification!", "ChromExtrn", MB_OK|MB_ICONEXCLAMATION);
					return S_FALSE;
				}
			VcvData[nObj1].pdValues = (double *)malloc(VcvData.nFeatures*sizeof(double));
			_getself(sptrObj, VcvData.nFeatures, (int *)VcvData.plFeatKeys, VcvData[nObj1].pdValues);
			nObj1++;
		}
	}
	// Initialize number of params and classes
	((VcvTeachData *)pTeachData)->ndim = VcvData.nFeatures;
	((VcvTeachData *)pTeachData)->nclass = nMaxClass+1;
	// Calculate classification parameters
	VcvParamsHld Params;
	_StatFile Stat("train.txt");
	if (Stat.f)
	{
		fprintf(Stat.f, "Features:\n");
		ICalcObjectContainerPtr sptrCOCObjList(punkObjectsList);
		struct ParameterContainer *pParamDef;
		for (int i = 0; i < VcvData.nFeatures; i++)
		{
			int key = VcvData.plFeatKeys[i];
			sptrCOCObjList->ParamDefByKey(key, &pParamDef);
			if (pParamDef && !pParamDef->pDescr)
			{
				_bstr_t bstrParamName = pParamDef->pDescr->bstrName;
				fprintf(Stat.f, "%3d %5d %s\n", i, key, (const char *)bstrParamName);
			}
			else
				fprintf(Stat.f, "%3d %5d\n", i, key);
		}
	}
	Vcv((VcvParams*)&Params, VcvData.GetSize(), VcvData.GetData(), (VcvTeachData *)pTeachData, 1, Stat.f);
	// In pTeachData->self only numbers not keys. Convert it.
	return S_OK;
}



