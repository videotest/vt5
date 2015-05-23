#pragma once
#include "classify_int.h"
#include "classstorage.h"
#include <vector>

extern const TCHAR* szBayesianClassifier;
extern const TCHAR* szKeysForKnb; 
struct ADDPARAM
{
	double high;
	double low;
	long key;
};
struct STAGE
{
	STAGE(){classesIn.reserve(10); params.reserve(10);};
	vector<int> classesIn;
	vector<ADDPARAM> params;

	int classOut;
	void AddParam( ADDPARAM ap){params.push_back(ap);};
	void AddClass(int cls){classesIn.push_back(cls);};
};
//using namespace std;
class PostProcessingData:
	public 	vector<STAGE*>
{
	int m_nStagesCount;
public:
	~PostProcessingData();
	void AttachClassFile(CString strClassFile);
};

class CBayesianClassifier:
	public CCmdTargetEx,
	public IClassifyParams, 
	public IClassifyProxy3, 
	public IClassifyInfoProxy
{

	CClassStorage m_cs;
	CPtrArray m_objects;
public:
	CBayesianClassifier(void);
	virtual ~CBayesianClassifier(void);
	virtual IUnknown* GetInterfaceHook(const void* p);

	//interface IClassifyParams-->

	com_call SetName( /*[in] */BSTR bstrName ) ;
	com_call GetName(/*[out] */BSTR *pbstrName ) ;

	com_call SetClasses( /*[in] */long *plClasses , /*[in]*/ long lSz ) ;
	com_call GetClasses( /*[out] */long **pplClasses , /*[out]*/ long *plSz ) ;
	com_call RemoveClasses() ;

	com_call SetClassParams( /*[in]*/ long lClass, /*[in]*/ CLASSINFO pfParams ) ;
	com_call GetClassParams( /*[in]*/ long lClass, /*[out]*/CLASSINFO *ppParams ) ;

	com_call SetKeys( /*[in] */long *plKeys , /*[in]*/ long lSz ) ;
	com_call GetKeys( /*[out] */long **pplKeys , /*[out]*/ long *plSz ) ;
	com_call RemoveKeys() ;

	com_call GetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[out]*/ double **ppfParams, /*[out]*/ long *plSz ) ;
	com_call SetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[in]*/ double *pfParams, /*[in]*/ long plSz ) ;
	//<--

//interface IClassifyProxy3-->

	com_call Load( /*[in]*/ BSTR bstrFile ) ;
	com_call Store( /*[in]*/ BSTR bstrFile ) ;
						 
	com_call Start() ;
	com_call Process( /*[in]*/ IUnknown *punkObject ) ;
	com_call Finish() ;
	com_call GetObjectClass( /*[in]*/ IUnknown *punkObject, /*out*/ long *plClass ) ;

	com_call GetFlags( /*[out]*/ DWORD *pdwFlag ) ;
	com_call SetFlags( /*[in]*/ DWORD dwFlag ) ;

	com_call Teach() ;
	com_call Teach(IUnknown* punkOL) ;
//<--

//IClassifyInfoProxy-->

	com_call PrepareObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass ) ;
	com_call GetObjectDescrByClass( /*[out]*/ BSTR *pbstrDescription ) ;
//<--
	DECLARE_DYNCREATE(CBayesianClassifier);
	GUARD_DECLARE_OLECREATE(CBayesianClassifier);

	HRESULT _stdcall QueryInterface(REFIID iid, void** ppvObj);
	ULONG _stdcall AddRef();
	ULONG _stdcall Release();

private:
	CString m_strClassFile;
	bool m_bAddon;
	PostProcessingData m_ppd;
	void PostProcess(long* plClass,ICalcObject2* co);
};
