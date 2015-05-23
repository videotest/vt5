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
#include "StatClassifier.h"
#include "LearningInfo.h"
#include "Timetest.h"

void CClassParamsEx::read_class_params(const char *lpszIniFile, FILE *rpt, int nDenverClass)
{
	if (nDenverClass == -1)
	{
		nclass = ::GetPrivateProfileInt(CLASSIFYINFO, "ClassCount", 0, lpszIniFile);
		_ptr_t2<char> big_buf(64000);
		GetPrivateProfileString(CLASSIFYINFO, "Keys", "", big_buf, 64000, lpszIniFile);
		_parse_long_list(big_buf, sfv, nsfv);
		if (nclass == 0 || nsfv == 0)
			throw new CMessageException(IDS_CLASSIFIER_NOT_TEACHED);
		if (!IsVersion3x() && nsfv != 3)
			throw new CMessageException(IDS_NOT_30_DATABASE);
		for (int i = 0; i < nclass; i++)
		{
			char sz_cl[10];
			_itoa(i+1, sz_cl, 10);
			GetPrivateProfileString(CLASSIFYINFO, sz_cl, "", big_buf, 64000, lpszIniFile);
			_parse_class_string(big_buf, nsfv, clinvvarsqrtdet[i], classmeans[i], classinvvars[i]);
		}
	}
	else
	{
		int nclass1 = ::GetPrivateProfileInt(CLASSIFYINFO, "ClassCount", 0, lpszIniFile);
		char szSec[256],szBuff1[3];
		strcpy(szSec,CLASSIFYINFO);
		szBuff1[0] = '_';
		szBuff1[1] = 'A'+nDenverClass;
		szBuff1[2] = 0;
		strcat(szSec,szBuff1);
		_ptr_t2<char> big_buf(64000);
		GetPrivateProfileString(szSec, "Keys", "", big_buf, 64000, lpszIniFile);
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
			GetPrivateProfileString(szSec, sz_cl, "", big_buf, 64000, lpszIniFile);
			_parse_class_string(big_buf, nsfv, clinvvarsqrtdet[nclass], classmeans[nclass], classinvvars[nclass]);
			nclass++;
		}
	}
	cutoff = min(nclass,CUTOFF);
}

void _DoClassify(LPCSTR lpszIniFile, CSimpleListIterator<CObjectRecord> &iter, long lObjects,
	bool bRearrange)
{
	// Prepare for statistic output
	static int nCallCount = 0;
	int nStat = _GetClassifierInt("Woolz", "Statistics", 0, lpszIniFile);
	_StatFile Stat("classify.txt", nStat, nCallCount++==0?"wt":"at");
	// Initialize classifier data.
	CClassParamsEx mh;
	mh.read_class_params(lpszIniFile, Stat.f);
	// Data block for Piper's code
	_ptr_t2<classcont> clcontents(mh.nclass);  // Information about class i
	_ptr_t2<chromcl> chrcl(lObjects+1); // Information about object i
	_ptr_t2<CObjectRecord*> positions(lObjects+1);
	// Classify objects in list.
	for (int i=0; i<mh.nclass; i++)
		clcontents[i].n = 0;
	chromcl *chp;// = chrcl;
	_ptr_t2<double> svec(mh.nsfv); // Values of the features of the object will be saved here
	if (Stat.f) fprintf(Stat.f, "Primary classification\n");
	int iObj = 0, iObjOk = 0;
	for (CObjectRecord *prec = iter.FirstPtr(); prec != NULL; prec = iter.NextPtr())
	{
		ICalcObjectPtr sptrObj(prec->punk);
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
			positions[iObjOk] = prec;
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
			clwn1[i] = ::GetPrivateProfileInt("Classification\\ObjectsInShot", szn, clwn1[i], lpszIniFile);
		}
		rearrange(chrcl,clcontents,lObjects,clwn1,mh.nclass,mh.cutoff);
	}
	for (i = 0; i < iObjOk; i++)
	{
		if (Stat.f) fprintf(Stat.f, "%d rearranged as %d\n", i+1, chrcl[i].newclass+1);
		CObjectRecord *prec = positions[i];
		if (chrcl[i].newclass >= 0)
			prec->lClass = chrcl[i].newclass;
	}
}

CStatClassifier::CStatClassifier(void) : m_ObjectsIterator(&m_ObjectsList)
{
}

CStatClassifier::~CStatClassifier(void)
{
}

IUnknown *CStatClassifier::DoGetInterface( const IID &iid )
{
	if (iid == IID_IClassifyProxy || iid == IID_IClassifyProxy2)
		return (IClassifyProxy *)this;
	else
		return __super::DoGetInterface(iid);
}

HRESULT CStatClassifier::Load(/*[in]*/ BSTR bstrFile)
{
	m_bstrIniFileName = bstrFile;
	return S_OK;
}

HRESULT CStatClassifier::Store( /*[in]*/ BSTR bstrFile )
{
	return S_OK;
}

HRESULT CStatClassifier::Start()
{
	return S_OK;
}

HRESULT CStatClassifier::Process( /*[in]*/ IUnknown *punkObject )
{
	CObjectRecord Rec;
	Rec.punk = punkObject;
	Rec.lClass = -1;
	Rec.key = ::GetKey(punkObject);
	m_ObjectsList.AddTail(Rec);
	return S_OK;
}

HRESULT CStatClassifier::Finish()
{
	try
	{
		bool bRearrange = ::GetPrivateProfileInt("Classification", "Rearrange", 0,
			(LPCSTR)m_bstrIniFileName)>0;
		_DoClassify((LPCSTR)m_bstrIniFileName, m_ObjectsIterator, m_ObjectsList.GetItemsCount(),
			bRearrange);
	}
	catch(CMessageException *e)
	{
		return HandleExc(e, E_UNEXPECTED);
	}
	return S_OK;
}

HRESULT CStatClassifier::GetObjectClass( /*[in]*/ IUnknown *punkObject, /*out*/ long *plClass )
{
	GuidKey key = ::GetKey(punkObject);
	for (CObjectRecord *prec = m_ObjectsIterator.FirstPtr(); prec != NULL;
		prec = m_ObjectsIterator.NextPtr())
	{
		if (key == prec->key)
		{
			*plClass = prec->lClass;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

HRESULT CStatClassifier::GetFlags( /*[out]*/ DWORD *pdwFlag )
{
	return E_NOTIMPL;
}

HRESULT CStatClassifier::SetFlags( /*[in]*/ DWORD dwFlag )
{
	return E_NOTIMPL;
}

HRESULT CStatClassifier::Teach()
{
	BOOL bMessages = _GetClassifierInt("Classification", "Messages", 0, m_bstrIniFileName);
	try
	{
		CLearningInfo LearningInfo(m_bstrIniFileName,0,0);
		LearningInfo.ReadData();
		// If learning database is empty, teaching will be invalid.
		if (LearningInfo.m_nObjectsCount == 0 || LearningInfo.m_FeaturesArray.GetSize() == 0 ||
			LearningInfo.m_FeaturesArray.nFeatures == 0)
			throw new CMessageException(IDS_LEARNDB_EMPTY);
		// If all objects not classified or same class, teaching will be invalid.
		int nClass = LearningInfo.m_FeaturesArray[0].nClass;
		bool bDiffClasses = false;
		for (int i = 1; i < LearningInfo.m_FeaturesArray.GetSize(); i++)
			if (LearningInfo.m_FeaturesArray[i].nClass != nClass)
			{
				bDiffClasses = true;
				break;
			}
		if (!bDiffClasses && bMessages)
			throw new CMessageException(IDS_LEARNDB_SAME_CLASS);
		VcvParamsHld Params(m_bstrIniFileName);
		Params.nClasses = LearningInfo.m_nClassesCount;
		Params.nDim = LearningInfo.m_FeaturesArray.nFeatures;
		if (Params.nFeatures > LearningInfo.m_FeaturesArray.nFeatures)
			Params.nFeatures = LearningInfo.m_FeaturesArray.nFeatures;
		_StatFile Stat("train.txt", Params.nStat);
		resultdata r;
		TeachClassifier((VcvParams*)&Params, LearningInfo.m_FeaturesArray.GetSize(), LearningInfo.m_FeaturesArray.GetData(),
			LearningInfo.m_FeaturesArray.plFeatKeys, &r, Stat.f);
		SetValue(GetAppUnknown(), "Classification", "LastResult", r.AvrV);
	}
	catch(CMessageException *e)
	{
		return HandleExc(e, E_UNEXPECTED, m_bstrIniFileName);
	}
	return S_OK;
}
