#if !defined(__InputUtils_H__)
#define __InputUtils_H__

//#include "Common.h"
#include "Input.h"
#include "Defs.h"
#include "action5.h"

IUnknown *FindDocTemplate(LPCTSTR pTemplName);
void DoExecute(sptrIActionManager &pActionMan, LPCTSTR pActionName, LPCTSTR pParams);
void __EnlistAllDocs(CArray<GUID, GUID&>&uiaFoundDocs, LPCTSTR lpTemplateName);
IUnknown *__FindNewDocs(CArray<GUID, GUID&>&uiaFoundDocs, GuidKey &lDocKey, LPCTSTR lpTemplateName);
IUnknown *__FindDocByKey(GuidKey lDocKey, LPCTSTR lpTemplateName);
IUnknown *__FindDocByName(LPCTSTR pDocName, LPCTSTR lpTemplateName);
void _SectionAndEntry(LPCTSTR lpName, CString &sSec, CString &sEntry);

class _CUseState
{
	_bstr_t m_bstrPrevState;
	IDriver4Ptr m_sptrDrv;
public:
	_CUseState(IUnknown *punkDrv, LPCTSTR lpszState)
	{
		CString s(lpszState);
		BSTR bstr = CString(lpszState).AllocSysString();
		Init(punkDrv, bstr);
		::SysFreeString(bstr);
	}
	_CUseState(IUnknown *punkDrv, BSTR bstrState)
	{
		Init(punkDrv, bstrState);
	}
	void Init(IUnknown *punkDrv, BSTR bstrState)
	{
		if (bstrState && bstrState[0])
		{
			m_sptrDrv = punkDrv;
			if (m_sptrDrv != 0)
			{
				BSTR bstr = NULL;
				m_sptrDrv->GetConfiguration(&bstr);
				m_bstrPrevState = bstr;
				::SysFreeString(bstr);
				if (m_bstrPrevState == _bstr_t(bstrState))
					m_sptrDrv.Release();
				else
					m_sptrDrv->SetConfiguration(bstrState);
			}
		}
	}
	~_CUseState()
	{
		// Do not restore state: it can be delete inside driver dialog
		//if (m_sptrDrv != 0)
		//	m_sptrDrv->SetConfiguration(m_bstrPrevState);
	}
};

inline CSize _MaxClientSize(CWnd *pWnd)
{
	CRect rcClient(0,0,800,600);
	CRect rcWnd(rcClient);
	AdjustWindowRect(rcWnd,GetWindowLong(pWnd->m_hWnd,GWL_STYLE),FALSE);
	CSize sz(GetSystemMetrics(SM_CXFULLSCREEN)-(rcWnd.Width()-rcClient.Width()),
		GetSystemMetrics(SM_CYFULLSCREEN)-(rcWnd.Height()-rcClient.Height()));
	return sz;
}

#endif