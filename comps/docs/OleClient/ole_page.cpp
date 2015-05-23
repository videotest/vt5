#include "stdafx.h"
#include "ole_page.h"
#include "constant.h"
/*
#include "OleData.h"
#include "Core5.h"
#include "DocView5.h"
*/

#include "action5.h"

COlePropertyPage::COlePropertyPage()
{
	m_bstrName = "Ole";
	m_bstrUserName = "OLE";
	m_bstrCaption = "Ole";
}

COlePropertyPage::~COlePropertyPage()
{
	
}


IUnknown *COlePropertyPage::DoGetInterface( const IID &iid )
{
	IUnknown	*punk = DoGetDlgInterface( iid );
	if( punk )return punk;
	else if( iid == IID_IPersist )return (IPersist*)this;
	else if( iid == IID_IHelpInfo )return (IHelpInfo*)this;
	else if( iid == IID_INamedObject2 )return (INamedObject2*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

HRESULT COlePropertyPage::GetClassID( CLSID *pclsid )
{
	*pclsid = clsidOlePropPage;
	return S_OK;
}




bool COlePropertyPage::DoInitDialog()
{
	// Нужна кнопка со стилем BS_NOTIFY, но в ресурсах нельзя его поставить т.к.
	// DIALOGEX не может быть использован для property page. Установить стиль вызовом
	// SetWindowLong(...,GWL_STYLE,...) тоже нельзя.
	HWND hwndButton = ::GetDlgItem(hwnd(), IDC_DEACTIVATE);
	if (hwndButton)
	{
		char szBuff[256];
		GetWindowText(hwndButton, szBuff, 256);
		LONG lStyles = GetWindowLong(hwndButton, GWL_STYLE);
		lStyles |= BS_NOTIFY;
		_rect rc;
		GetWindowRect(hwndButton, &rc);
		_point pt = rc.top_left();
		_size  sz = rc.size();
		ScreenToClient(hwnd(), &pt);
		rc = _rect(pt, sz);
		HFONT hFont = (HFONT)SendMessage(hwndButton, WM_GETFONT, 0, 0);
		int nId = GetWindowLong(hwndButton, GWL_ID);
		::DestroyWindow(hwndButton);
		hwndButton = ::CreateWindow("Button",szBuff,lStyles,pt.x, pt.y, sz.cx, sz.cy,
			hwnd(), NULL, NULL, NULL);
		SendMessage(hwndButton, WM_SETFONT, (WPARAM)hFont, TRUE);
		SetWindowLong(hwndButton, GWL_ID, nId);
	}
	return true;
}

void COlePropertyPage::DoCommand( UINT wParam )
{
	IUnknown* punk = 0;
	punk = ::_GetOtherComponent( ::GetAppUnknown(), IID_IActionManager );

	if( punk )
	{
		IActionManagerPtr ptrAM( punk );
		punk->Release();	punk = 0;

		if( ptrAM )
		{
			// A.M. fix, BT 3796
			UINT nNotify = HIWORD(wParam);
			UINT nCmd = LOWORD(wParam);
			if( nCmd == IDC_DEACTIVATE )
			{
				if( nNotify == BN_SETFOCUS )
					ptrAM->ExecuteAction( _bstr_t("DeactivateOleObject"), 0, 0 );			
			}
			else
			if( nCmd == IDC_UPDATE )						
				ptrAM->ExecuteAction( _bstr_t("UpdateOleObject"), 0, 0 );			
		}
	}

	/*
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	IUnknown* punkDoc =  0;

	ptrApp->GetActiveDocument( &punkDoc );
	if( !punkDoc )
		return;

	IDocumentSitePtr ptrDS( punkDoc );
	punkDoc->Release();	punkDoc = 0;

	IUnknown* punkView = 0;
	ptrDS->GetActiveView( &punkView );
	if( !punkView )
		return;

	IDataContext3Ptr ptrC( punkView );
	punkView->Release();	punkView = 0;

	if( ptrC == 0 )
		return;


	IUnknown* punkObj = 0;
	ptrC->GetActiveObject( _bstr_t(szTypeOleItem), &punkObj );	
	if( !punkObj )
		punkObj->Release();

	IOleObjectUIPtr ptrOle = punkObj;
	punkObj->Release();	punkObj = 0;
	if( ptrOle == 0 )
		return;



	if( nCmd == IDC_DEACTIVATE )
	{
		HWND hWnd = 0;
		ptrOle->GetInPlaceEditHwnd( &hWnd );

		if( hWnd )
			ptrOle->Deactivate( hWnd );
	}
	else
	if( nCmd == IDC_UPDATE )
	{
		ptrOle->ForceUpdate();
	}
	*/

}


