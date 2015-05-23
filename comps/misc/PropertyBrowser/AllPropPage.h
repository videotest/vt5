// AllPropPage.h : Declaration of the CAllPropPage

#ifndef __ALLPROPPAGE_H_
#define __ALLPROPPAGE_H_

#include "resource.h"       // main symbols
#include "PropertyBrowserPage.h"

EXTERN_C const CLSID CLSID_AllPropPage;

/////////////////////////////////////////////////////////////////////////////
// CAllPropPage
class ATL_NO_VTABLE CAllPropPage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAllPropPage, &CLSID_AllPropPage>,
	public CPropertyBrowserPage<CAllPropPage>
{
public:
	CAllPropPage() 
	{
		m_dwTitleID = IDS_TITLEAllPropPage;
		m_dwHelpFileID = IDS_HELPFILEAllPropPage;
		m_dwDocStringID = IDS_DOCSTRINGAllPropPage;
	}

	enum {IDD = IDD_ALLPROPPAGE};

DECLARE_REGISTRY_RESOURCEID(IDR_ALLPROPPAGE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAllPropPage) 
	COM_INTERFACE_ENTRY(IPropertyPage)
END_COM_MAP()

BEGIN_MSG_MAP(CAllPropPage)
	CHAIN_MSG_MAP(CPropertyBrowserPage<CAllPropPage>)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	/*STDMETHOD(Apply)(void)
	{
		ATLTRACE(_T("CAllPropPage::Apply\n"));
		for (UINT i = 0; i < m_nObjects; i++)
		{
			// Do something interesting here
			// ICircCtl* pCirc;
			// m_ppUnk[i]->QueryInterface(IID_ICircCtl, (void**)&pCirc);
			// pCirc->put_Caption(CComBSTR("something special"));
			// pCirc->Release();
		}
		m_bDirty = FALSE;
		return S_OK;
	}*/
};

#endif //__ALLPROPPAGE_H_
