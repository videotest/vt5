#if !defined(AFX_DBASELISTENERIMPL_H__F526CDAA_836E_4A16_997A_04C369B3F3BF__INCLUDED_)
#define AFX_DBASELISTENERIMPL_H__F526CDAA_836E_4A16_997A_04C369B3F3BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBaseListenerImpl.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDBaseEventController

class CDBaseEventController
{
// Construction
public:
	CDBaseEventController();
	~CDBaseEventController();
	void FireEvent(const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );
	bool RegisterListener( IUnknown* punk );
	bool UnRegisterListener( IUnknown* punk );
protected:
	CPtrList m_listeners;
};



/////////////////////////////////////////////////////////////////////////////
// CDBaseListenerImpl window

class CDBaseListenerImpl : public CImplBase
{
// Construction
public:
	CDBaseListenerImpl();
	BEGIN_INTERFACE_PART(DBaseListener, IDBaseListener)
		com_call OnNotify( BSTR bstrEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, tagVARIANT var );
	END_INTERFACE_PART(DBaseListener)
public:
	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var ) = 0;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBASELISTENERIMPL_H__F526CDAA_836E_4A16_997A_04C369B3F3BF__INCLUDED_)
