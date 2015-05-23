#include "stdafx.h"
#include "PiperClassifier.h"
#include "misc_calibr.h"
#include "misc_templ.h"
#include "Classes5.h"
#include "misc_utils.h"
#include "WoolzObject.h"
#include "StdioFile.h"
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
double invvarsqrtdet;
}

#include "VcvData.h"

typedef struct chromosome Chromosome;
typedef struct classcont Classcont;
typedef struct chromcl Chromcl;

struct ChromclPos : public Chromcl
{
	long pos;
};

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

void *woolz_object_by_vt5_object(IUnknown *punkVT5Obj)
{
	IWoolzObject *pwo = NULL;
	sptrINamedDataObject2 sptrNObj(punkVT5Obj);
	long posObj = 0;
	sptrNObj->GetFirstChildPosition((long*)&posObj);
	while (posObj)
	{
		IUnknown *punkCh = 0;
		sptrNObj->GetNextChild((long*)&posObj, &punkCh);
		if (punkCh)
		{
			IWoolzObject *pwo1 = NULL;
			punkCh->QueryInterface(IID_IWoolzObject, (void **)&pwo1);
			punkCh->Release();
			if (pwo1)
			{
				pwo = pwo1;
				break;
			}
		}
	}
	void *pobj = NULL;
	if (pwo)
	{
		pwo->GetWoolzObject(&pobj);
		pwo->Release();
	}
	return pobj;
}

CPiperClassifier::CPiperClassifier()
{
	pVcvData = new CVcvDataArray;
	pTeachData = NULL;//new VcvTeachData;
}

CPiperClassifier::~CPiperClassifier()
{
	delete (CVcvDataArray *)pVcvData;
	delete pTeachData;
}

HRESULT CPiperClassifier::Load( IStream *pStream, SerializeParams *pparams )
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
		int nValues = getdim(0);
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
					i = TeachData.self[h];
					pStream->Read(&TeachData.classmeans[_class][i], sizeof(TeachData.classmeans[_class][i]), &nRead);
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
	return CObjectBase::Load(pStream,pparams);
}

HRESULT CPiperClassifier::Store( IStream *pStream, SerializeParams *pparams )
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
		int nValues = getdim(0);
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
					i = TeachData.self[h];
					pStream->Write(&TeachData.classmeans[_class][i], sizeof(TeachData.classmeans[_class][i]), &nWritten);
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
	return CObjectBase::Store(pStream,pparams);
}

HRESULT CPiperClassifier::IsBaseObject(BOOL * pbFlag)
{
	*pbFlag = false;
	return S_OK;
}

/* logs of prior probabilities */
double lpriorp[MAXCLASS] = {
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
0.0, 0.0, 0.0, -0.5};

int clwn[MAXCLASS] = {
2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 1};

HRESULT CPiperClassifier::Classify(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc)
{
	// Data block for Piper's code
	struct chromosome *obj;
	struct chromosome *objlist[MAXOBJS]; // All woolz objects
	struct chromosome *otype1_3_list[MAXOBJS]; // Objects with type == 1 and plist->otype <= 3
	struct chromosome *chrlist[MAXOBJS]; // Chromosomes (objects with type == 1 and plist->otype == 1)
	int numobj,ntype1_3,numchr,numdel; // Counters of this arrays
	int chrindex[MAXOBJS]; // Index of elements of chrlist in objlist
	long positions[MAXOBJS]; // Positions of found woolz objects (objlist) in VT5 object list
	struct classcont clcontents[MAXCLASS];  // Structures for classification with temporary class assignment
	struct classcont *classcontp; // pointer to current structure
	struct chromcl *chp,chrcl[MAXOBJS]; // Another structures - holds one variant of class assignment
	int fvec[MAXDIM],svec[MAXDIM]; // Unnormalized and normalized parameters.
	struct normaliser vn[MAXDIM]; // Normalizer
	int ndim,sfv[MAXDIM],nsfv; // Selected features array and its size
	int nclass; // Number of classes during cllassifier training.
	// Check that classifiers was already teached	
	VcvTeachData *pTeachData = (VcvTeachData *)this->pTeachData;
	if (!pTeachData) return S_FALSE;
	// Make array of classes. It will be used during classifier.
	sptrINamedDataObject2 sptrNCl(punkClassesList);
	long lClasses; // Number of classes in array now
	sptrNCl->GetChildsCount(&lClasses);
	if (lClasses < pTeachData->nclass)
		lClasses = pTeachData->nclass;
	_ptr_t<GuidKey> arrClasses(lClasses);
	long posCl = 0;
	int nClass = 0;
	sptrNCl->GetFirstChildPosition((long*)&posCl);
	while (posCl)
	{
		IUnknown *punkCl = 0;
		sptrNCl->GetNextChild((long*)&posCl, &punkCl);
		if (punkCl)
		{
			INumeredObjectPtr sptrCNO(punkCl);
			GuidKey lClassKey;
			sptrCNO->GetKey(&lClassKey);
			punkCl->Release();
			((GuidKey *)arrClasses)[nClass] = lClassKey;
		}
		nClass++;
	}
	// Find all objects in woolz form and save it in array.
	sptrINamedDataObject2 sptrN(punkObjectsList);
	long lObjects;
	sptrN->GetChildsCount(&lObjects);
	long pos = 0;
	numobj = ntype1_3 = numchr = numdel = 0;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknown	*punk = 0;
		long posSaved = pos;
		sptrN->GetNextChild( (long*)&pos, &punk );
		if (punk)
		{
			ICalcObjectPtr sptrObj(punk);
			obj = (Chromosome *)woolz_object_by_vt5_object(punk);
			if (obj)
			{
				objlist[numobj] = obj;
				positions[numobj] = posSaved;
				if (obj->type == 1 && obj->plist->otype <= 3)
				{
					otype1_3_list[ntype1_3++] = obj;
					if (obj->plist->otype <= 1)
					{
						chrlist[numchr] = obj;
						chrindex[numchr] = numobj;
						numchr++;
					}
				}
				numobj++;
			}
			punk->Release();
		}
	}
	// Prepare for statistic output
	_StdioFile Stat;
	int nStat = ::GetValueInt(GetAppUnknown(), "Woolz", "Statistics", 1);
	if (nStat)
	{
		char szRptPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
		GetModuleFileName(NULL,szRptPath,_MAX_PATH);
		_splitpath(szRptPath,szDrive,szDir,NULL,NULL);
		_makepath(szRptPath,szDrive,szDir,"classify.txt", NULL);
		try
		{
			Stat.Open(szRptPath, "wt");
		}
		catch(_StdioFileException *e)
		{
			e->ReportError();
			e->Delete();
		}
	}
	// Classify objects in array.
	read_class_params(pTeachData,&nclass,&ndim,sfv,&nsfv,Stat.f);
	for (int i=0; i<pTeachData->nclass; i++)
		clcontents[i].n = 0;
	normvec(otype1_3_list,ntype1_3,vn); // Normalized - calculate using all objects with otype <= 3, not only chromosomes.
	chp = chrcl;
	if (Stat.f) fprintf(Stat.f, "Primary classification\n");
	for (int j=0; j<numchr; j++)
	{
		obj = chrlist[j];
		getvec(obj->plist,fvec); /* feature vector */
		normivec(fvec,svec,sfv,vn,ndim,nsfv); /* normalised feature vector */
		if (Stat.f)
		{
			fprintf(Stat.f, "Features for object %d:", j);
			for(int i = 0; i < nsfv; i++)
				fprintf(Stat.f, " %d", svec[i]);
			fprintf(Stat.f, "\n");
		}
		/* likelihood classifier */
		int _class = iclassdist(svec,lpriorp,&(chp->cllika[0]),pTeachData->nclass,nsfv) - 1;
		chp->trueclass = obj->plist->Cpgroup - 1;
		chp->rawclass = _class;
		chp->newclass = _class;
		classcontp = &clcontents[_class];
		classcontp->chcl[classcontp->n] = chp;
		classcontp->n++;
		if (Stat.f) fprintf(Stat.f, "Object %d classified as %d\n", j, _class+1);
#ifndef MIZAR
		/* sort the class likelihods of this chromosome into order */
		qsort(&(chp->cllika[0]),nclass,sizeof(struct cllik),liksort);
#endif MIZAR
		chp++;
	}
	// re-arrange so as to move objects from over-subscribed
	// classes to under-subscribed as far as plausibility allows
	if (Stat.f) fprintf(Stat.f, "Rearrangement\n");
	rearrange(chrcl,clcontents,numchr,clwn,pTeachData->nclass);
	for (i=0; i<numchr; i++)
	{
		chrlist[i]->plist->pgroup = chrcl[i].newclass+1;
		if (Stat.f) fprintf(Stat.f, "%d rearranged as %d\n", i, chrcl[i].newclass+1);
		j = chrindex[i];
		pos = positions[j];
		IUnknown	*punk = 0;
		sptrN->GetNextChild(&pos, &punk);
		if (punk)
		{
			ICalcObjectPtr sptrObj(punk);
			//GuidKey lClassKey = ((GuidKey *)arrClasses)[chrcl[i].newclass];
			//???[AY]
			set_object_class( sptrObj, chrcl[i].newclass  );
			punk->Release();
		}
	}

	return S_OK;
}

HRESULT CPiperClassifier::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = 0;
	return S_OK;
}


HRESULT CPiperClassifier::Teach(IUnknown *punkObjectsList, IUnknown *punkClassesList, IUnknown *punkDoc)
{
	int ivec[MAXDIM];
	int fvec[MAXDIM];
	int sfv[MAXDIM];
	int ndim = getdim(0);
	struct normaliser vn[MAXDIM];
	int iObj = 0;
	for (int i=0; i<MAXDIM; i++)
		sfv[i] = i;
	// Allocate sufficient array of data
	sptrINamedDataObject2 sptrN(punkObjectsList);
	long lObjects;
	sptrN->GetChildsCount(&lObjects);
	_ptr_t<Chromosome*> arrObjects(lObjects);
	_ptr_t<bool> arrObjDelete(lObjects);
	// Iterate all objects and find Woolz object data
	long pos = 0;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknown	*punk = 0;
		sptrN->GetNextChild( (long*)&pos, &punk );
		if (punk)
		{
			ICalcObjectPtr sptrObj(punk);
			// If there is woolz object in VT5 object, find and use it.
			void *pobj = woolz_object_by_vt5_object(punk);
			if (pobj)
			{
				((Chromosome **)arrObjects)[iObj] = (Chromosome *)pobj;
				iObj++;
			}
			punk->Release();
		}
	}
	_bstr_t bstrDocName = ::GetName(punkDoc); // Will be used for debug purposes
	// Now normalize object data and save it
	CVcvDataArray &VcvData = (*(CVcvDataArray *)pVcvData);
	normvec((Chromosome **)arrObjects,iObj,vn);
	int nObj1 = VcvData.GetSize();
	VcvData.Add(iObj);
	for (i = 0; i < iObj; i++)
	{
		Chromosome *obj = ((Chromosome **)arrObjects)[i];
		if (obj->plist->otype <= 1 /*&&
			(derived != 0 || obj->plist->history[0] == 0)*/ )
		{
			/* write correct class */
			int nClass = obj->plist->Cpgroup;
			if (nClass == 0)
				nClass = obj->plist->pgroup;
			if (obj->plist->history[0] != 0)
				nClass += 100;
			VcvData[nObj1].nClass = nClass-1;
			/* feature vector */
			getvec(obj->plist,ivec);
			/* compute and write normalised feature vector */
			normivec(ivec,fvec,sfv,vn,ndim,ndim);
			VcvData[nObj1].pdValues = (double *)malloc(ndim*sizeof(double));
			for (int j=0; j<ndim; j++)
			{
				VcvData[nObj1].pdValues[j] = fvec[j];
			}
//			VcvData[nObj1].sSource = bstrDocName;
//			VcvData[nObj1].nSourceNum = i;
			nObj1++;
		}
	}
	// Calculate classification parameters
	VcvParams Params;
	Params.nFeatureMethod = ::GetValueInt(GetAppUnknown(), "Woolz", "FeatureSelectionMethod", 3);
	switch (Params.nFeatureMethod)
	{
	case 1:
		Params.nFeatures = ::GetValueInt(GetAppUnknown(), "Woolz", "Features", 1);
		Params.dMaxcor = ::GetValueDouble(GetAppUnknown(), "Woolz", "Maxcor", 0.);
		break;
	case 2:
		Params.nFeatures = ::GetValueInt(GetAppUnknown(), "Woolz", "Features", 1);
		break;
	case 3:
		Params.nFeatures = ::GetValueInt(GetAppUnknown(), "Woolz", "Features", 1);
		Params.pnExplicitFeatArray = new int[Params.nFeatures];
		for (i = 0; i < Params.nFeatures; i++)
		{
			char szBuff[50];
			sprintf(szBuff, "Param%d", i+1);
			Params.pnExplicitFeatArray[i] = ::GetValueInt(GetAppUnknown(), "Woolz", szBuff, i);
		}
		break;
	}
	Params.nStat = ::GetValueInt(GetAppUnknown(), "Woolz", "Statistics", 1);
	if (Params.nStat)
	{
		char szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
		GetModuleFileName(NULL,Params.szRptPath,_MAX_PATH);
		_splitpath(Params.szRptPath,szDrive,szDir,NULL,NULL);
		_makepath(Params.szRptPath,szDrive,szDir,"train.txt", NULL);
	}
	if (!pTeachData)
	{
		pTeachData = new VcvTeachData;
		memset(pTeachData, 0 , sizeof(VcvTeachData));
		((VcvTeachData*)pTeachData)->flags = TeachInteger;
	}
	_StatFile Stat("train.txt");
	Vcv(&Params, VcvData.GetSize(), VcvData.GetData(), (VcvTeachData *)pTeachData, 0, Stat.f);
	if (Params.nFeatureMethod == 3)
		delete Params.pnExplicitFeatArray;
	return S_OK;
}


