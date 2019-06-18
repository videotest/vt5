#include "stdafx.h"
#include "resource.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "misc_utils.h"
#include "misc_templ.h"
#include "core5.h"
#include "Classes5.h"
#include "docview5.h"
#include "stdio.h"
#include "class_utils.h"
#include "stdio.h"
#include "vcv.h"
#include "StatFile.h"
#include "MessageException.h"
#include "ChObjUtils.h"
#include "Ver30.h"
#include "mahalanobis.h"

void DoReClassifyDenver(IUnknown *punkObjectsList, IUnknown *punkDoc, bool bRearrange, int nDenverClass);

void _getself(ICalcObject *ptrObj, int nsfv, long *sfv, double *svec)
{
	for (int i = 0; i < nsfv; i++)
	{
		HRESULT hr = ptrObj->GetValue(sfv[i], &svec[i]);
		if (FAILED(hr))
		{
			BOOL bMessages = ::GetValueInt(::GetAppUnknown(), "Classification", "Messages", 0);
			if (bMessages)
				throw new CMessageException(IDS_NO_FEATURES);
			else 
				svec[i] = 0.;
		}
	}
}

int liksort(cllik *a, cllik *b)
{
	if (a->lval > b->lval)
		return(-1);
	else if (a->lval < b->lval)
		return(1);
	else
	    return(0);
}

// Strings type 9,6,3,4,...
void _parse_long_list(const char *pstrParse, _ptr_t2<long> &FeatBuff, int &nFeatures)
{
	char *buff = const_cast<char*>(pstrParse);
	// 1. Calc Features
	nFeatures = 0;
	while (1)
	{
		const char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures++;
		buff = const_cast<char*>(p+1);
	}
	if (nFeatures)
		nFeatures++;
	else
	{ // if no commas in string , string may be empty or contain one integer
		if (buff[0] && (isdigit_ru(buff[0]) || buff[0]=='-' && isdigit_ru(buff[1])))
			nFeatures++;
	}
	// 2. Read features from string
	FeatBuff.alloc(nFeatures);
	buff = const_cast<char*>(pstrParse);
	int nFeatures1 = 0;
	while (nFeatures1 < nFeatures)
	{
		long nKey = atol(buff);
		FeatBuff[nFeatures1] = nKey;
		char *p = strchr(buff, ',');
		if (!p)
			break;
		nFeatures1++;
		buff = p+1;
	}
}

// String type 1.9, (2.1,9.1), ...
void _parse_class_string(char *pszParse, int nPairs, double &d, _ptr_t2<double> &ad1, _ptr_t2<double> &ad2)
{
	ad1.alloc(nPairs);
	ad2.alloc(nPairs);
	// parse
	char *p1,*p2;
	d = strtod(pszParse, &p2);
	p1 = pszParse;
	for (int i = 0; i < nPairs; i++)
	{
		const char *pb = strchr(p1,'(');
		if (!pb)
			break;
		const char *pe = strchr(pb+1, ')');
		int n = pe-pb;
		_ptr_t2<char> buf(n+1);
		strncpy(buf, pb+1, n);
		pb = strchr(buf, ',');
		ad1[i] = strtod(buf, &p2);
		if (pb)
			ad2[i] = strtod(pb+1, &p2);
		else
			ad2[i] = 0.;
		p1 = (char *)pe+1;
	}
	for (; i < nPairs; i++)
	{
		ad1[i] = 0.;
		ad2[i] = 0.;
	}
}

int CDenverClasses::m_anDenverClasses[] =
{0,0,0,1,1,2,2,2,2,2,2,2,3,3,3,4,4,4,5,5,6,6,2,6};

void mahalanobis::read_class_params(FILE *rpt, int nDenverClass)
{
	char	sz_ini[MAX_PATH];
	local_filename(sz_ini, CLASSIFYINFO_INI, sizeof(sz_ini));
	strcpy( sz_ini, (const char*)::GetValueString(GetAppUnknown(), "\\Classes", CLASSIFYINFO, sz_ini ));
	if (nDenverClass == -1)
	{
		nclass = ::GetPrivateProfileInt(CLASSIFYINFO, "ClassCount", 0, sz_ini);
		_ptr_t2<char> big_buf(64000);
		GetPrivateProfileString(CLASSIFYINFO, "Keys", "", big_buf, 64000, sz_ini);
		_parse_long_list(big_buf, sfv, nsfv);
		if (nclass == 0 || nsfv == 0)
			throw new CMessageException(IDS_CLASSIFIER_NOT_TEACHED);
		if (!IsVersion3x() && nsfv != 3)
			throw new CMessageException(IDS_NOT_30_DATABASE);
		for (int i = 0; i < nclass; i++)
		{
			char sz_cl[10];
			_itoa(i+1, sz_cl, 10);
			GetPrivateProfileString(CLASSIFYINFO, sz_cl, "", big_buf, 64000, sz_ini);
			_parse_class_string(big_buf, nsfv, clinvvarsqrtdet[i], classmeans[i], classinvvars[i]);
		}
	}
	else
	{
		int nclass1 = ::GetPrivateProfileInt(CLASSIFYINFO, "ClassCount", 0, sz_ini);
		char szSec[256],szBuff1[3];
		strcpy(szSec,CLASSIFYINFO);
		szBuff1[0] = '_';
		szBuff1[1] = 'A'+nDenverClass;
		szBuff1[2] = 0;
		strcat(szSec,szBuff1);
		_ptr_t2<char> big_buf(64000);
		GetPrivateProfileString(szSec, "Keys", "", big_buf, 64000, sz_ini);
		_parse_long_list(big_buf, sfv, nsfv);
		if (nclass1 == 0 || nsfv == 0)
			throw new CMessageException(IDS_CLASSIFIER_NOT_TEACHED);
		if (!IsVersion3x() && nsfv != 3)
			throw new CMessageException(IDS_NOT_30_DATABASE);
		CDenverClasses Denv;
		nclass = 0;
		for (int i = 0; i < nclass1; i++)
		{
			if (Denv.DenverClass(i) != nDenverClass)
				continue;
			char sz_cl[10];
			_itoa(i+1, sz_cl, 10);
			GetPrivateProfileString(szSec, sz_cl, "", big_buf, 64000, sz_ini);
			_parse_class_string(big_buf, nsfv, clinvvarsqrtdet[nclass], classmeans[nclass], classinvvars[nclass]);
			nclass++;
		}
	}
	cutoff = min(nclass,CUTOFF);
}

int mahalanobis::classdist (double *fvec, double *lpriorp, cllik *liklv)
{
	int i,maxc;
	double d, lik, maxl;
	struct cllik liks[MAXCLASS];
	struct cllik *liklvp;
	liklvp = liks;
	for (i=0; i<nclass; i++)
	{
		d = distvar(fvec,classmeans[i],classinvvars[i],nsfv);
		lik = -d/2.0;
		lik += clinvvarsqrtdet[i];
		lik += lpriorp[i];	/* log of prior probability */
		liklvp->lval = lik;
		liklvp->_class = i;
		liklvp++;
		if (i == 0 || lik > maxl)
		{
			maxl = lik;
			maxc = i;
		}
	}
	/*
	 * sort likelihoods into order and copy first CUTOFF
	 * values into class likelihood structure
	 */
	qsort(liks,nclass,sizeof(cllik),(COMPARE)liksort);
	liklvp = liks;
	for (i=0; i<cutoff; i++)
	{
		liklv->lval = liklvp->lval;
		liklv->_class = liklvp->_class;
		liklv++;
		liklvp++;
	}
	return (maxc+1);
}

double mahalanobis::distvar(double *fv, double *mv,double *cliv, int dim)
{
	double d;
	double dv;
	int i;

	d = 0.0;
	for (i=0; i<dim; i++)
	{
		/* difference vector between sample and class mean */
		dv = *fv++ - *mv++;
		/* generalised Euclidean distance */
		d += (dv * dv * (*cliv++));
	}
	return(d);
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

void _DoClassify(IUnknown *punkObjectsList, IUnknown *punkDoc, _ptr_t2<int> &arrClasses,
	bool bRearrange)
{
	sptrINamedDataObject2 sptrN(punkObjectsList); // Object list
	long lObjects; // Number of objects
	sptrN->GetChildsCount(&lObjects);
	// Prepare classes for undo
	arrClasses.alloc(lObjects);
	// Data block for Piper's code
	_ptr_t2<classcont> clcontents(class_count());  // Information about class i
	_ptr_t2<chromcl> chrcl(lObjects+1); // Information about object i
	_ptr_t2<long> positions(lObjects+1);
	// Prepare for statistic output
	static int nCallCount = 0;
	_StatFile Stat("classify.txt", -1, nCallCount++==0?"wt":"at");
	IDocumentSitePtr sptrDS(punkDoc);
	BSTR bstr;
	sptrDS->GetPathName(&bstr);
	_bstr_t bstr1(bstr);
	::SysFreeString(bstr);
	if (Stat.f) fprintf(Stat.f, "\n%s\n", (const char *)bstr1);
	// Initialize classifier data.
	mahalanobis mh;
	mh.read_class_params(Stat.f);
	if (mh.nclass != class_count())
	{
		VTMessageBox(IDS_WRONG_NUMBER_OF_CLASSES, app::instance()->handle(), NULL, MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	// Classify objects in list.
	for (int i=0; i<mh.nclass; i++)
		clcontents[i].n = 0;
	chromcl *chp;// = chrcl;
	_ptr_t2<double> svec(mh.nsfv); // Values of the features of the object will be saved here
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
			_getself(sptrObj, mh.nsfv, mh.sfv, svec);
			if (Stat.f)
			{
				fprintf(Stat.f, "Features for object %d:", iObj+1);
				for(i = 0; i < mh.nsfv; i++)
					fprintf(Stat.f, " %f", svec[i]);
				fprintf(Stat.f, "\n");
			}
			chp = &chrcl[iObjOk];
			int _class = mh.classdist(svec,lpriorp,&(chp->cllika[0])) - 1;
			chp->trueclass = -1;
			positions[iObjOk] = pos1;
			chp->rawclass = _class;
			chp->newclass = _class;
			if (clcontents[_class].n < MAXCHILD)
			{
				clcontents[_class].chcl[clcontents[_class].n] = chp;
				clcontents[_class].n++;
			}
			chp->bestclass = -1;
			if (Stat.f) fprintf(Stat.f, "Object %d classified as %d\n", iObj+1, _class+1);
			// sort the class likelihods of this chromosome into order
			qsort(&(chp->cllika[0]),mh.cutoff,sizeof(struct cllik),(COMPARE)liksort);
			iObjOk++;
		}
		else
			if (Stat.f) fprintf(Stat.f, "Object %d is strange\n", iObj);
		iObj++;
	}
	// re-arrange so as to move objects from over-subscribed
	// classes to under-subscribed as far as plausibility allows
	if (bRearrange)
	{
		if (Stat.f) fprintf(Stat.f, "Rearrangement\n");
		_ptr_t2<int> clwn1(mh.nclass);
		clwn1.zero();
		int n = min(mh.nclass,sizeof(clwn)/sizeof(clwn[0]));
		memcpy((int *)clwn1, clwn, n*sizeof(int));
		for (i = 0; i < mh.nclass; i++)
		{
			char szn[25];
			_itoa(i+1, szn, 10);
			clwn1[i] = ::GetValueInt(::GetAppUnknown(), "Classification\\ObjectsInShot", szn, clwn1[i]);
		}
		rearrange(chrcl,clcontents,lObjects,clwn1,mh.nclass,mh.cutoff);
	}
	for (i = 0; i < iObjOk; i++)
	{
		if (Stat.f) fprintf(Stat.f, "%d rearranged as %d\n", i+1, chrcl[i].newclass+1);
		pos = positions[i];
		ICalcObjectPtr sptrObj;
		IUnknownPtr sptr;
		sptrN->GetNextChild(&pos, &sptr);
		sptrObj = sptr;
		arrClasses[i] = get_object_class(sptrObj);
		bool bOk = sptrObj;
		if (bOk && chrcl[i].newclass >= 0)
		{
			int nManualClass;
			get_class(sptrObj, &nManualClass);
			if (nManualClass == 0)
				set_object_class( sptrObj, chrcl[i].newclass );
		}
	}
}

void DoReClassifyDenver(IUnknown *punkObjectsList, IUnknown *punkDoc, bool bRearrange, int nDenverClass)
{
	CDenverClasses Denv;
	sptrINamedDataObject2 sptrN(punkObjectsList); // Object list
	long lObjects; // Number of objects
	sptrN->GetChildsCount(&lObjects);
	// Data block for Piper's code
	_ptr_t2<classcont> clcontents(class_count());  // Information about class i
	_ptr_t2<chromcl> chrcl(lObjects+1); // Information about object i
	_ptr_t2<long> positions(lObjects+1);
	// Prepare for statistic output
	_StatFile Stat("classify.txt", -1, "at");
	// Initialize classifier data.
	mahalanobis mh;
	mh.read_class_params(Stat.f, nDenverClass);
	// Classify objects in list.
	for (int i=0; i<mh.nclass; i++)
		clcontents[i].n = 0;
	chromcl *chp;// = chrcl;
	_ptr_t2<double> svec(mh.nsfv); // Values of the features of the object will be saved here
	if (Stat.f) fprintf(Stat.f, "Reclassification for Denver class %c\n", nDenverClass+'A');
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
		if (bOk && Denv.DenverClass(get_object_class(sptrObj)) == nDenverClass)
		{
			_getself(sptrObj, mh.nsfv, mh.sfv, svec);
			if (Stat.f)
			{
				fprintf(Stat.f, "Features for object %d:", iObj+1);
				for(i = 0; i < mh.nsfv; i++)
					fprintf(Stat.f, " %f", svec[i]);
				fprintf(Stat.f, "\n");
			}
			chp = &chrcl[iObjOk];
			int _intclass = mh.classdist(svec,lpriorp,&(chp->cllika[0])) - 1;
			chp->trueclass = -1;
			positions[iObjOk] = pos1;
			if (clcontents[_intclass].n < MAXCHILD)
			{
				chp->rawclass = _intclass;
				chp->newclass = _intclass;
				clcontents[_intclass].chcl[clcontents[_intclass].n] = chp;
				clcontents[_intclass].n++;
			}
			if (Stat.f) fprintf(Stat.f, "Object %d reclassified as %d\n", iObj+1, _intclass+1);
			// sort the class likelihods of this chromosome into order
			qsort(&(chp->cllika[0]),mh.cutoff,sizeof(struct cllik),(COMPARE)liksort);
			iObjOk++;
		}
		iObj++;
	}
	// re-arrange so as to move objects from over-subscribed
	// classes to under-subscribed as far as plausibility allows
	if (bRearrange)
	{
		if (Stat.f) fprintf(Stat.f, "Rearrangement for Denver class %c\n", nDenverClass+'A');
		_ptr_t2<int> clwn1(mh.nclass);
		clwn1.zero();
		for (i = 0; i < mh.nclass; i++)
		{
			int nClass = Denv.ClassByIntclass(nDenverClass,i);
			char szn[25];
			_itoa(nClass+1, szn, 10);
			clwn1[i] = ::GetValueInt(::GetAppUnknown(), "Classification\\ObjectsInShot", szn, clwn[nClass]);
		}
		rearrange(chrcl,clcontents,lObjects,clwn1,mh.nclass,mh.cutoff);
	}
	for (i = 0; i < iObjOk; i++)
	{
		pos = positions[i];
		ICalcObjectPtr sptrObj;
		IUnknownPtr sptr;
		sptrN->GetNextChild(&pos, &sptr);
		sptrObj = sptr;
		bool bOk = sptrObj;
		int nPrevClass = -1;
		if (bOk && chrcl[i].newclass >= 0)
		{
			int nManualClass;
			nPrevClass = get_object_class(sptrObj);
			get_class(sptrObj, &nManualClass);
			if (nManualClass == 0)
				set_object_class(sptrObj, Denv.ClassByIntclass(nDenverClass, chrcl[i].newclass) );
		}
		if (Stat.f) fprintf(Stat.f, "%d from Denver class %c rearranged as %d (was %d)\n", i+1, nDenverClass+'A', chrcl[i].newclass+1, nPrevClass+1);
	}
}

void DoClassify(IUnknown *punkObjectsList, IUnknown *punkDoc, _ptr_t2<int> &arrClasses,
	bool bRearrange)
{
	_DoClassify(punkObjectsList, punkDoc, arrClasses, bRearrange);
	if (GetValueInt(GetAppUnknown(), "Classification", "UseDenverClasses", 0))
	{
		DoReClassifyDenver(punkObjectsList, punkDoc, bRearrange, 0);
		DoReClassifyDenver(punkObjectsList, punkDoc, bRearrange, 1);
		DoReClassifyDenver(punkObjectsList, punkDoc, bRearrange, 2);
		DoReClassifyDenver(punkObjectsList, punkDoc, bRearrange, 3);
		DoReClassifyDenver(punkObjectsList, punkDoc, bRearrange, 4);
		DoReClassifyDenver(punkObjectsList, punkDoc, bRearrange, 5);
		DoReClassifyDenver(punkObjectsList, punkDoc, bRearrange, 6);
	}
}

void SetClasses(IUnknown *punkObjectsList, _ptr_t2<int> &arrClasses)
{
	// Exchange classes
	sptrINamedDataObject2 sptrN(punkObjectsList); // Object list
	int i = 0;
	long pos = 0;
	sptrN->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr sptr;
		ICalcObjectPtr sptrObj;
		sptrN->GetNextChild((long*)&pos, &sptr);
		sptrObj = sptr;
		bool bOk = sptrObj;
		if (bOk)
		{
			int nNewClass = arrClasses[i];
			arrClasses[i] = get_object_class(sptrObj);
			set_object_class(sptrObj, nNewClass);
			i++;
		}
	}
	// Now all OK. Redraw document.
	INamedDataObject2Ptr NDO(punkObjectsList);
	IUnknown *punk = NULL;
	HRESULT hr = NDO->GetData(&punk);
	if (SUCCEEDED(hr) && punk)
	{
		long l = cncReset;
		INotifyControllerPtr sptr(punk);
		sptr->FireEvent(_bstr_t(szEventChangeObjectList), NULL, NDO, &l, 0);
		punk->Release();
	}
}





