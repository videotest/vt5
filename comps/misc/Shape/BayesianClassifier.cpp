#include "StdAfx.h"
#include "bayesianclassifier.h"
#include "cccontour.h"
#include "classifyshape.h"

const TCHAR* szBayesianClassifier ="Shape.Bayesian";
const TCHAR* szKeysForKnb ="KeysForKnb";

inline CString _get_value_string( CString strFileName, CString strSection, CString strKey )
{
	TCHAR szT[4096];
	DWORD dw = ::GetPrivateProfileString( strSection, strKey,
		"", szT, sizeof( szT ) / sizeof( szT[0] ), strFileName );
	if( dw > 0 )
		return szT;
	return "";
}

IMPLEMENT_DYNCREATE(CBayesianClassifier, CCmdTargetEx);

// {C963F026-0B84-4b08-B1C3-992F9F7263B5}
GUARD_IMPLEMENT_OLECREATE(CBayesianClassifier, szBayesianClassifier ,
0xc963f026, 0xb84, 0x4b08, 0xb1, 0xc3, 0x99, 0x2f, 0x9f, 0x72, 0x63, 0xb5);

CBayesianClassifier::CBayesianClassifier(void)
{
	
}

CBayesianClassifier::~CBayesianClassifier(void)
{

	int n = m_objects.GetCount();
	for(int i=0;i<n;i++)
	{
		((IUnknown*)m_objects[i])->Release();
	}
}

IUnknown* CBayesianClassifier::GetInterfaceHook(const void* p)
{
	if (*(IID*)p == IID_IClassifyParams)
		return (IClassifyParams*)this;
	else if(*(IID*)p == IID_IClassifyProxy2 || *(IID*)p == IID_IClassifyProxy )
		return (IClassifyProxy2*)this;
	else if(*(IID*)p == IID_IClassifyInfoProxy)
		return (IClassifyInfoProxy*)this;
	else if(*(IID*)p == IID_IClassifyProxy3)
		return (IClassifyProxy3*)this;
	else if(*(IID*)p == IID_IUnknown)
		return (IClassifyInfoProxy*)this;
	else return __super::GetInterfaceHook(p);
}

ULONG CBayesianClassifier::AddRef()
{
    return ExternalAddRef();
}
 
ULONG CBayesianClassifier::Release()
{

    return ExternalRelease();
}

HRESULT CBayesianClassifier::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{

    return ExternalQueryInterface(&iid, ppvObj);
}



//interface IClassifyParams-->

HRESULT CBayesianClassifier::SetName( /*[in] */BSTR bstrName )
{
	return S_OK;
}
HRESULT CBayesianClassifier::GetName(/*[out] */BSTR *pbstrName )
{
	return S_OK;
}

HRESULT CBayesianClassifier::SetClasses( /*[in] */long *plClasses , /*[in]*/ long lSz )
{
	return S_OK;
}
HRESULT CBayesianClassifier::GetClasses( /*[out] */long **pplClasses , /*[out]*/ long *plSz )
{
	return S_OK;
}
HRESULT CBayesianClassifier::RemoveClasses()
{
	return S_OK;
}

HRESULT CBayesianClassifier::SetClassParams( /*[in]*/ long lClass, /*[in]*/ CLASSINFO pfParams )
{
	return S_OK;
}
HRESULT CBayesianClassifier::GetClassParams( /*[in]*/ long lClass, /*[out]*/CLASSINFO *ppParams )
{
	return S_OK;
}

HRESULT CBayesianClassifier::SetKeys( /*[in] */long *plKeys , /*[in]*/ long lSz )
{
	return S_OK;
}
HRESULT CBayesianClassifier::GetKeys( /*[out] */long **pplKeys , /*[out]*/ long *plSz )
{
	return S_OK;
}
HRESULT CBayesianClassifier::RemoveKeys()
{
	return S_OK;
}

HRESULT CBayesianClassifier::GetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[out]*/ double **ppfParams, /*[out]*/ long *plSz )
{
	return S_OK;
}
HRESULT CBayesianClassifier::SetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[in]*/ double *pfParams, /*[in]*/ long plSz )
{
	return S_OK;
}
//<--

//interface IClassifyProxy2-->

HRESULT CBayesianClassifier::Load( /*[in]*/ BSTR bstrFile )
{
	m_strClassFile = bstrFile;
	m_ppd.AttachClassFile(m_strClassFile);
	CString strKnBase = _get_value_string(m_strClassFile, ID_SECT_GENERAL, "KnowledgeBase" );
	int n = m_strClassFile.ReverseFind('\\');
	
	strKnBase = m_strClassFile.Left(n+1)+ strKnBase;
	if(strKnBase.GetLength())
	{
		if(!m_cs.Init(strKnBase)) return E_FAIL;
	}
	CString strKeys = _get_value_string(m_strClassFile, ID_SECT_KEY_INFO, szKeysForKnb);
	int pos =0;
	CString token = strKeys.Tokenize(",",pos);
	while(!token.IsEmpty())
	{
		int key = _ttoi(token);
		if(m_cs.IsNew()) m_cs.AddParameter(key);
		else if(!m_cs.CheckParameter(key))
		{
			AfxMessageBox("There is conflict between classifier and knowledge base data.\nResolve collision and try again.");
			return E_FAIL;
		}
		token = strKeys.Tokenize(",",pos);
	}

	return S_OK;
}
HRESULT CBayesianClassifier::Store( /*[in]*/ BSTR bstrFile )
{
	m_strClassFile = bstrFile;
			
	CString strKnBase = _get_value_string(m_strClassFile, ID_SECT_GENERAL, "KnowledgeBase" );
	int n = m_strClassFile.ReverseFind('\\');
	
	strKnBase = m_strClassFile.Left(n+1)+ strKnBase;
	m_cs.Write(strKnBase);
	return S_OK;
}

HRESULT CBayesianClassifier::Start()
{
	m_cs.Recalculate();
	return S_OK;
}
HRESULT CBayesianClassifier::Process( /*[in]*/ IUnknown *punkObject )
{
	//punkObject->AddRef();
	//m_objects.Add(punkObject);
	return S_OK;
}
HRESULT CBayesianClassifier::Finish()
{
	//int n = m_objects.GetCount();
	//for(int i=0;i<n;i++)
	//{
	//	IUnknown* punk =(IUnknown*)m_objects[i];
	//	


	//	ICalcObject2Ptr cobj(punk);			
	//	punk->Release();
	//}
	//m_objects.RemoveAll();
	return S_OK;
}
HRESULT CBayesianClassifier::GetObjectClass( /*[in]*/ IUnknown *punk, /*out*/ long *plClass )
{
		ICalcObjectPtr co =punk;
	
		if(!punk) return E_FAIL;
		//TODO  Release


		CChCodedContour ccc(punk);
		if(!ccc.GetContourSize()) 
		{
			*plClass =-1;
			return S_OK;
		}

		CMorphoGraph mg;
		CArray<double>& arr = ccc.GetArray();
		mg.Init(arr.GetData(), arr.GetSize(), 1000, 200, 10);


		CurveDescr cd;
		GDescr gd;
		GPARAM gp;

		int n = m_cs.GetParamsCount();
		for(int i=0;i<n;i++ )
		{
			gp.key = m_cs.GetParameterAt(i);
			HRESULT hr = co->GetValue(gp.key,&gp.value);
			if(FAILED(hr))
			{
				//AfxMessageBox("The object contains not enough calculated parameters for current classifier!\nPlease, enable all neccesary parameters, recalculate object list and try again.", MB_ICONEXCLAMATION | MB_OK, 0);  
				return hr;
			}
			gd.Add(gp);
		}

		mg.GetDescriptor(&cd);
		cd.SetGlobalParameters(gd);
		*plClass = m_cs.ClarifyClass(cd);
		
		PostProcess(plClass,(ICalcObject2*)co.GetInterfacePtr());

	

	return S_OK;
}



HRESULT CBayesianClassifier::GetFlags( /*[out]*/ DWORD *pdwFlag )
{
	return S_OK;
}
HRESULT CBayesianClassifier::SetFlags( /*[in]*/ DWORD dwFlag )
{
	return S_OK;
}
HRESULT CBayesianClassifier::Teach(IUnknown* punkOL)
{
	INamedDataObject2Ptr ol = punkOL;
	POSITION pos;
	ol->GetFirstChildPosition( &pos);
	IUnknown* punk; 
	int n = m_cs.GetParamsCount();
	while(pos)
	{
		ol->GetNextChild( &pos, &punk );
		ICalcObjectPtr co =punk;
	
		if(!punk) return E_FAIL;
		//TODO  Release

		int classN = get_object_class(co, m_strClassFile);
		if(classN<0) 
		{
			punk->Release();
			continue;
		}

		CChCodedContour ccc(punk);
		if(!ccc.GetContourSize()) 
		{
			punk->Release();
			continue;
		}

		CMorphoGraph mg;
		CArray<double>& arr = ccc.GetArray();
		mg.Init(arr.GetData(), arr.GetSize(), 1000, 200, 10);


		CurveDescr cd;
		GDescr gd;
		GPARAM gp;

		for(int i=0;i<n;i++ )
		{
			gp.key = m_cs.GetParameterAt(i);
			HRESULT hr = co->GetValue(gp.key,&gp.value);
			if(FAILED(hr))
			{
				AfxMessageBox("The object contains not enough calculated parameters for current classifier!\nPlease, enable all neccesary parameters, recalculate object list and try again.", MB_ICONEXCLAMATION | MB_OK, 0);
				punk->Release();
				throw 1;
				return hr;
			}
			gd.Add(gp);
		}

		mg.GetDescriptor(&cd);
		cd.SetGlobalParameters(gd);
		cd.SetClass(classN);

		m_cs.AddDescriptor(cd);	
		punk->Release();		
	}
	return S_OK;
}

HRESULT CBayesianClassifier::Teach()
{
	return S_OK;
}
//<--

//IClassifyInfoProxy-->

HRESULT CBayesianClassifier::PrepareObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass )
{
	return S_OK;
}
HRESULT CBayesianClassifier::GetObjectDescrByClass( /*[out]*/ BSTR *pbstrDescription )
{
	return S_OK;
}
//<--
void CBayesianClassifier::PostProcess(long* plClass,ICalcObject2* co)
{
	
	int n = m_ppd.size();
	for(int i=0;i<n;i++)
	{
		bool bIt=true;
		STAGE* s = m_ppd[i];
		int c = s->classesIn.size();
		for(int j=0;j<c;j++)
		{
			if(s->classesIn[j]==*plClass)
			{
				int m = s->params.size();
				for(int x=0;x<m;x++)
				{
					ADDPARAM ap = s->params[x];
					double d;
					HRESULT hr = co->GetValue(ap.key, &d);
					if(FAILED(hr) || d>ap.high || d<ap.low) 
					{
						bIt =false;
						break;
					}
				}
				if(bIt)	*plClass = s->classOut;	
				break;
			}
		}
		
	}
}

void PostProcessingData::AttachClassFile(CString strFile)
{
  
	m_nStagesCount = GetPrivateProfileInt(szPPro, _T("StageCount"), 0, strFile);
	reserve(m_nStagesCount);
	for(int i=0;i<m_nStagesCount;i++)
	{
		STAGE* pst = new STAGE();
		CString s;
		s.Format("Stage%i",i+1);
		TCHAR cls[256];
		GetPrivateProfileString(s, _T("ClassesIn"), "", cls, 256,  strFile);
		TCHAR* token;
		TCHAR* c = _T(",");
		token = _tcstok(cls,c);
		int n = _ttoi(token);
		pst->AddClass(n);
		while(1)
		{
			token = _tcstok(0,c);
			if(token==0) break;
			n = _ttoi(token);
			pst->AddClass(n);
		}
		int N = GetPrivateProfileInt(s, _T("KeysCount"), 0,  strFile);
		for(int j=0;j<N;j++)
		{
			ADDPARAM ap ={0};
			CString skey;
			skey.Format("Key%i",j+1);
			TCHAR keyData[512];
			GetPrivateProfileString(s, skey, "", keyData, 512,  strFile);
			token = _tcstok(keyData,c);
			ap.key = _ttoi(token);
			token = _tcstok(0,c);
			ap.low = _tstof(token);
			token = _tcstok(0,c);
			ap.high =  _tstof(token);
			pst->AddParam(ap);
		}
		pst->classOut = GetPrivateProfileInt(s, _T("ClassOut"), 0,  strFile);
		push_back(pst);
	}
}

PostProcessingData::~PostProcessingData()
{
	while(!empty())
	{
		delete back();
		pop_back();
	}
}