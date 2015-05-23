// VTDesign.cpp : Implementation of CVTDesignApp and DLL registration.

#include "stdafx.h"
#include "VTDesign.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CVTDesignApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xbc127b08, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


void InvalidateCtrl( COleControl* pCtrl, LPOLEINPLACESITEWINDOWLESS pInPlaceSiteWndless, bool bEraseParentBk/*=false*/ )
{
	if( !pCtrl )
		return;

	if( !pCtrl->m_hWnd )
		return;


	//At first erase parent bckg
	CWnd* pWndParent = pCtrl->GetParent();
	if( pWndParent && pWndParent->GetSafeHwnd() )
	{
		CDC* pDC = pCtrl->GetDC();
		
		POINT  pt;		
		pt.x       = 0;
		pt.y       = 0;

		CRect rc;
		pCtrl->GetClientRect( &rc );
		

		//pCtrl->MapWindowPoints( pWndParent, &pt, 1 );
		//OffsetWindowOrgEx(pDC->m_hDC, pt.x, pt.y, &pt );

		::InvalidateRect( pWndParent->m_hWnd, &rc, FALSE );

		::SendMessage( pWndParent->m_hWnd, WM_ERASEBKGND,
				  (WPARAM)pDC->m_hDC, 0);	

		//SetWindowOrgEx( pDC->m_hDC, pt.x, pt.y, NULL );

		pCtrl->ReleaseDC( pDC );
		

		/*
		HWND hWndParent = pWndParent->GetSafeHwnd();
		HDC hdc = ::GetDC( pCtrl );
		if( hdc )
		{
			 CRect rc;
			 pCtrl->GetClientRect( &rc );
			 pCtrl->ClientToScreen( &rc );
			 pWndParent->ScreenToClient( &rc );

			 CDC* pDC = CDC::FromHandle( hdc );

			 pDC->FillRect( &rc, &CBrush( RGB(0,0,0)) );

			//::InvalidateRect( hWndParent, &rc, FALSE );
			//::UpdateWindow( hWndParent );
			//::SendMessage( hWndParent, WM_ERASEBKGND, (WPARAM)hdc, (LPARAM)0L );

			::ReleaseDC( m_hWnd, hdc );
		}
		*/
		
	}
	
		

	pCtrl->Refresh();	
	if( !pCtrl || !pCtrl->GetSafeHwnd() )
		return;

	((CWnd*)pCtrl)->Invalidate( );
	((CWnd*)pCtrl)->UpdateWindow( );


	
	//if( pInPlaceSiteWndless) 
		//pInPlaceSiteWndless->InvalidateRect( NULL, TRUE );


	//bEraseParentBk

	//((CWnd*)pWndParent)->Invalidate( );
	//((CWnd*)pWndParent)->UpdateWindow( );

	//::SendMessage( WM_ERASEBKGND
	
	

}

/////////////////////////////////////////////////////////////////////////////////////
// Helper function for visible status

BOOL _IsWindowVisible( CWnd* pWnd )
{
	if( pWnd )
		return FALSE;

	if( !::IsWindow( pWnd->GetSafeHwnd() ) )
		return FALSE;

	return pWnd->IsWindowVisible();
}


////////////////////////////////////////////////////////////////////////////
// CVTDesignApp::InitInstance - DLL initialization

BOOL CVTDesignApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CVTDesignApp::ExitInstance - DLL termination

int CVTDesignApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
