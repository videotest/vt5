#include "stdafx.h"
#include "actions.h"

#include "nameconsts.h"
#include "misc_utils.h"

#include "constant.h"
#include "OleData.h"
#include "core5.h"
#include "misc_utils.h"
#include "data5.h"
#include "docview5.h"


IUnknown* GetActiveOleObjectFromDoc( IUnknown* punkDoc, _bstr_t* pbstrName )
{
	IDocumentSitePtr ptrDS( punkDoc );
	if( ptrDS == 0 )
		return 0;

	IUnknown* punkV = 0;
	ptrDS->GetActiveView( &punkV );
	if( punkV == 0 )
		return 0;

	IDataContext2Ptr ptrDC( punkV );
	punkV->Release();	punkV = 0;

	if( ptrDC == 0 )
		return 0;


	LONG_PTR lPos = 0;
	ptrDC->GetFirstSelectedPos( _bstr_t( szTypeOleItem ), &lPos );
	if( !lPos )
		return 0;

	IUnknown* punk = 0;
	ptrDC->GetNextSelected( _bstr_t( szTypeOleItem ), &lPos, &punk );

	if( !punk )
		return 0;

	if( !pbstrName )
		return punk;


	INamedObject2Ptr ptrND2( punk );
	if( ptrND2 )
	{
		BSTR bstr = 0;
		ptrND2->GetName( &bstr );
		(*pbstrName) = bstr;
		::SysFreeString( bstr );	bstr = 0;
	}

	return punk;
}


//////////////////////////////////////////////////////////////////////
//
//
//	class CInsertOleObject
//
//
//////////////////////////////////////////////////////////////////////

#define szArgInputFileName	"InputFileName"

_ainfo_base::arg	CInsertOleObjectInfo::s_pargs[] = 
{	
	{szArgInputFileName,	szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CInsertOleObjectToExistInfo::s_pargs[] = 
{	
	{szArgInputFileName,	szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

static int nActionCount = 0;
static int nInfoCount = 0;
//////////////////////////////////////////////////////////////////////
CInsertOleObject::CInsertOleObject()
{
	nActionCount++;
	m_bOverwriteActive = false;
}

//////////////////////////////////////////////////////////////////////
CInsertOleObject::~CInsertOleObject()
{
	nActionCount--;
}

//0x01397A58
CInsertOleObjectInfo::CInsertOleObjectInfo()
{
	nInfoCount++;

}

CInsertOleObjectInfo::~CInsertOleObjectInfo()
{
	nInfoCount--;
}

//////////////////////////////////////////////////////////////////////
bool CInsertOleObject::CreateOleObjectContext( IUnknown* punkOleObject )
{
	IOleObjectDataPtr ptrOleObject = punkOleObject;
	if( ptrOleObject == 0 )
		return false;

	HWND hWnd = NULL;
	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp )
	{		
		ptrApp->GetMainWindowHandle( &hWnd );
	}

	if( !hWnd )
		return false;

	OLEUIINSERTOBJECT io;
	::ZeroMemory( &io, sizeof(io) );

	TCHAR	szFile[OLEUI_CCHPATHMAX];    
	DWORD	dwFlags = IOF_SELECTCREATENEW | IOF_SHOWHELP;

	io.cbStruct = sizeof(OLEUIINSERTOBJECT);
	io.dwFlags = dwFlags;
	io.hWndOwner = hWnd;

	szFile[0]=0;
	io.lpszFile=szFile;
	io.cchFile=OLEUI_CCHPATHMAX;
	io.dwFlags= IOF_SELECTCREATENEW | IOF_DISABLEDISPLAYASICON | IOF_DISABLELINK;
	

	UINT	uTemp;
	uTemp = OleUIInsertObject(&io);
	if( uTemp != OLEUI_OK )
		return false;

	return ( S_OK == ptrOleObject->CreateItem( &io ) );

}

//////////////////////////////////////////////////////////////////////
HRESULT CInsertOleObject::DoInvoke()
{
	IUnknown* punkObj = 0;	
	
	punkObj = ::CreateTypedObject( _bstr_t( szTypeOleItem ) );	

	if( !punkObj )
		return E_FAIL;	

	IOleObjectDataPtr ptrOleNew( punkObj );
	IOleObjectDataPtr ptrOleOld;

	punkObj->Release();

	if( ptrOleNew == NULL )
		return E_FAIL;
	
	if( m_bOverwriteActive )
	{
		_bstr_t bstrName;
		IUnknown* punkActive = 0;
		punkActive = ::GetActiveOleObjectFromDoc( m_ptrTarget, &bstrName );
		if( !punkActive )
			return E_FAIL;		

		ptrOleOld = punkActive;
		punkActive->Release();	punkActive = 0;

		INamedObject2Ptr ptrNO( ptrOleNew );		

		if( ptrNO == 0 )
			return E_FAIL;

		ptrNO->SetName( bstrName );		
		
	}


	if( !CreateOleObjectContext( ptrOleNew ) )
		return E_FAIL;		


	long lActivate = ::GetValueInt( ::GetAppUnknown(), "OleClient", "Auto Activate", 1L );
	
	//Set auto activate
	::SetValue( ::GetAppUnknown(), "OleClient", "Auto Activate", 0L );

	if( ptrOleOld )
		RemoveFromDocData( m_ptrTarget, ptrOleOld );

	SetToDocData( m_ptrTarget, ptrOleNew );


	IUnknown* punk = 0;
	punk = ::_GetOtherComponent( ::GetAppUnknown(), IID_IActionManager );

	if( punk )
	{
		IActionManagerPtr ptrAM( punk );
		punk->Release();	punk = 0;

		if( ptrAM )		
			ptrAM->ExecuteAction( _bstr_t("ShowOleObject"), 0, 0 );		
	}
	

	//restore auto activate
	::SetValue( ::GetAppUnknown(), "OleClient", "Auto Activate", lActivate );

	IDocumentSitePtr ptrDoc( m_ptrTarget );
	if( ptrDoc )
	{
		IUnknown* punk = 0;
		ptrDoc->GetActiveView( &punk );

		if( punk )
		{			
			IDataContext3Ptr ptrDC( punk );
			punk->Release();

			if( ptrDC )
				ptrDC->SetActiveObject( _bstr_t(szTypeOleItem),  ptrOleNew, 0 );			

			IOleViewPtr ptrOleView( ptrDC );
			if( ptrOleView )
			{
				HWND hWnd = 0;
				ptrOleView->GetHwnd( &hWnd );
				if( hWnd )
				{
					IOleObjectUIPtr ptrOleUI( ptrOleNew );
					if( ptrOleUI )
						ptrOleUI->Activate( hWnd );
				}
			}			
		}
	}

	
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//
//
//	class CInsertOleObjectToExist
//
//
//////////////////////////////////////////////////////////////////////
CInsertOleObjectToExist::CInsertOleObjectToExist()
{
	m_bOverwriteActive = true;
}

//////////////////////////////////////////////////////////////////////
CInsertOleObjectToExist::~CInsertOleObjectToExist()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CInsertOleObjectToExist::GetActionState( DWORD *pdwState )
{
	IUnknown* punkObj = ::GetActiveOleObjectFromDoc( m_ptrTarget, 0 );
	if( punkObj )
	{
		*pdwState = 1;
		punkObj->Release();	punkObj = 0;
	}
	else
		*pdwState = 0;

	return S_OK;
}



//////////////////////////////////////////////////////////////////////
//
//
//	class CInsertOleObjectFromClipboard
//
//
//////////////////////////////////////////////////////////////////////
CInsertOleObjectFromClipboard::CInsertOleObjectFromClipboard()
{
	m_bOverwriteActive = false;
}

//////////////////////////////////////////////////////////////////////
CInsertOleObjectFromClipboard::~CInsertOleObjectFromClipboard()
{

}

bool CInsertOleObjectFromClipboard::CreateOleObjectContext( IUnknown* punkOleObject )
{
	IOleObjectDataPtr ptrOleObject = punkOleObject;
	if( ptrOleObject == 0 )
		return false;

	return ( S_OK == ptrOleObject->CreateFromClipboard() );

}
//////////////////////////////////////////////////////////////////////
//
//
//	class CInsertOleObjectFromClipboardToExist
//
//
//////////////////////////////////////////////////////////////////////
CInsertOleObjectFromClipboardToExist::CInsertOleObjectFromClipboardToExist()
{
	m_bOverwriteActive = true;
}

//////////////////////////////////////////////////////////////////////
CInsertOleObjectFromClipboardToExist::~CInsertOleObjectFromClipboardToExist()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CInsertOleObjectFromClipboardToExist::GetActionState( DWORD *pdwState )
{
	IUnknown* punkObj = ::GetActiveOleObjectFromDoc( m_ptrTarget, 0 );
	if( punkObj )
	{
		*pdwState = 1;
		punkObj->Release();	punkObj = 0;
	}
	else
		*pdwState = 0;

	return S_OK;

}




//////////////////////////////////////////////////////////////////////
//
//
//	class CShowOleObject
//
//
//////////////////////////////////////////////////////////////////////
CShowOleObject::CShowOleObject()
{

}

//////////////////////////////////////////////////////////////////////
CShowOleObject::~CShowOleObject()
{

}

HRESULT CShowOleObject::GetActionState( DWORD *pdwState )
{
	*pdwState = 0;
	if( m_ptrTarget == NULL )
		return S_OK;
	sptrIFrameWindow	sptrF( m_ptrTarget );
	if( sptrF == NULL )
		return S_OK;
	IUnknownPtr	punkSplitter;
	sptrF->GetSplitter( &punkSplitter );
	if( punkSplitter == 0 )
		return S_OK;
	sptrISplitterWindow	sptrS( punkSplitter );
	if( sptrS == NULL )
		return S_OK;
	IUnknownPtr punkActiveView;
	sptrS->GetActiveView(&punkActiveView);
	if( punkActiveView == 0 )
		return S_OK;
	IUnknownPtr punkActiveOleItem(GetActiveObjectFromContext(punkActiveView,
		_bstr_t(szTypeOleItem)), false);
	if (punkActiveOleItem == 0)
		return S_OK;
	*pdwState = 1;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CShowOleObject::DoInvoke()
{

	if( m_ptrTarget == NULL )
		return E_FAIL;

	sptrIFrameWindow	sptrF( m_ptrTarget );
	if( sptrF == NULL )
		return E_FAIL;

	IUnknown	*punkSplitter = 0;

	sptrF->GetSplitter( &punkSplitter );
	if( !punkSplitter )
		return E_FAIL;

	sptrISplitterWindow	sptrS( punkSplitter );
	punkSplitter->Release();

	if( sptrS == NULL )
		return E_FAIL;	
		
	int	nRow = 0; 
	int	nCol = 0;

	_bstr_t	bstrViewProgID( szOleClientView );
	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->ChangeViewType( nRow, nCol, bstrViewProgID );

	IUnknown	*punkView = 0;
	sptrS->GetViewRowCol( nCol, nRow, &punkView );

	if( !punkView )return E_FAIL;
	punkView->Release();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//
//
//	class CActivateOleObject
//
//
//////////////////////////////////////////////////////////////////////
CActivateOleObject::CActivateOleObject()
{

}

//////////////////////////////////////////////////////////////////////
CActivateOleObject::~CActivateOleObject()
{

}

//////////////////////////////////////////////////////////////////////
IUnknownPtr CActivateOleObject::GetActiveView()
{
	IDocumentSitePtr ptrDS( m_ptrTarget );
	if( ptrDS == 0 )
		return 0;

	IUnknown* punkV = 0;
	ptrDS->GetActiveView( &punkV );
	if( !punkV )
		return 0;

	IUnknownPtr ptrV( punkV );
	punkV->Release();	punkV = 0;

	return ptrV;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActivateOleObject::GetActionState( DWORD *pdwState )
{
	*pdwState = 0;
	IUnknownPtr punkActiveView(GetActiveView());
	if( punkActiveView == 0 )
		return S_OK;
	IUnknownPtr punkActiveOleItem(GetActiveObjectFromContext(punkActiveView,
		_bstr_t(szTypeOleItem)), false);
	if (punkActiveOleItem == 0)
		return S_OK;
	*pdwState = 1;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CActivateOleObject::DoInvoke()
{
	IOleViewPtr ptrV( GetActiveView() );
	if( ptrV == 0 )
	{

		long lActivate = ::GetValueInt( ::GetAppUnknown(), "OleClient", "Auto Activate", 1L );
	
		//Set auto activate
		::SetValue( ::GetAppUnknown(), "OleClient", "Auto Activate", 0L );

		//try to change view
		IUnknown* punk = 0;
		punk = ::_GetOtherComponent( ::GetAppUnknown(), IID_IActionManager );

		if( punk )
		{
			IActionManagerPtr ptrAM( punk );
			punk->Release();	punk = 0;

			if( ptrAM )		
				ptrAM->ExecuteAction( _bstr_t("ShowOleObject"), 0, 0 );		
		}

		//restore auto activate
		::SetValue( ::GetAppUnknown(), "OleClient", "Auto Activate", lActivate );



	}

	ptrV = GetActiveView();
	if( ptrV == 0 )
		return E_FAIL;

	IDataContext3Ptr ptrC( ptrV );
	if( ptrC == 0 )
		return E_FAIL;

	IUnknown	*punkOleObject = 0;	

	ptrC->GetActiveObject( _bstr_t(szTypeOleItem), &punkOleObject );	
	if( !punkOleObject )		
	{
		LONG_PTR lPos = 0;
		ptrC->GetFirstObjectPos( _bstr_t(szTypeOleItem), &lPos );
		ptrC->GetNextObject( _bstr_t(szTypeOleItem), &lPos, &punkOleObject );
		if( punkOleObject )
		{
			ptrC->SetObjectSelect( punkOleObject, 1 );			
		}
	}

	if( !punkOleObject )		
		return E_FAIL;

	IOleObjectUIPtr ptr = punkOleObject;
	
	if( punkOleObject != 0 )
		punkOleObject->Release();	punkOleObject = 0;

	if( ptr == 0 )
		return E_FAIL;


	HWND hWndView = 0;
	ptrV->GetHwnd( &hWndView );
	
	if( hWndView )
		ptr->Activate( hWndView );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//
//
//	class CActivateOleObject
//
//
//////////////////////////////////////////////////////////////////////
CDeactivateOleObject::CDeactivateOleObject()
{

}

//////////////////////////////////////////////////////////////////////
CDeactivateOleObject::~CDeactivateOleObject()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CDeactivateOleObject::DoInvoke()
{
	IOleViewPtr ptrV( m_ptrTarget );
	if( ptrV == 0 )
		return E_FAIL;

	IDataContext3Ptr ptrC( ptrV );
	if( ptrC == 0 )
		return E_FAIL;

	IUnknown	*punkOleObject = 0;	

	ptrC->GetActiveObject( _bstr_t(szTypeOleItem), &punkOleObject );	
	if( !punkOleObject )
		return E_FAIL;

	IOleObjectUIPtr ptr = punkOleObject;
	
	if( punkOleObject != 0 )
		punkOleObject->Release();	punkOleObject = 0;

	if( ptr == 0 )
		return E_FAIL;

	HWND hWndView = 0;
	ptrV->GetHwnd( &hWndView );
	
	ptr->Deactivate( hWndView );	

	return S_OK;
}



//////////////////////////////////////////////////////////////////////
//
//
//	class CShowOleObjectContent
//
//
//////////////////////////////////////////////////////////////////////
CShowOleObjectContent::CShowOleObjectContent()
{

}

//////////////////////////////////////////////////////////////////////
CShowOleObjectContent::~CShowOleObjectContent()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CShowOleObjectContent::DoInvoke()
{

	IDataContext3Ptr ptrC( m_ptrTarget );
	if( ptrC == 0 )
		return false;


	IUnknown	*punkOleObject = 0;	

	ptrC->GetActiveObject( _bstr_t(szTypeOleItem), &punkOleObject );	
	if( !punkOleObject )
		return E_FAIL;

	IOleObjectDataPtr ptr = punkOleObject;
	
	if( punkOleObject != 0 )
		punkOleObject->Release();	punkOleObject = 0;


	BSTR bstrFileName = 0;
	short nCreationType = 0;
	BSTR bstrAppProgID = 0;

	ptr->GetItemInfo( &bstrFileName, &nCreationType, &bstrAppProgID );
	
	_bstr_t _bstrFileName	= bstrFileName;
	_bstr_t _bstrApp		= bstrAppProgID;
	
	::SysFreeString( bstrFileName );	bstrFileName = 0;
	::SysFreeString( bstrAppProgID );	bstrAppProgID = 0;

	OleCreationType oct		= (OleCreationType)nCreationType;

	_bstr_t _bstrCreationType = "Undefine";

	if( oct == octLinkToFile )
		_bstrCreationType = "Link To File";
	else
	if( oct == octInsertFromFile )
		_bstrCreationType = "Insert From File";
	else
	if( oct == octCreateNewItem )
		_bstrCreationType = "Create New Item";
	else
	if( oct == octCreateFromClipboard )
		_bstrCreationType = "Create From Clipboard";		


	_bstr_t _bstrOut;

	_bstrOut  = "Creation type:\t" + _bstrCreationType;
	_bstrOut += "\n";
	_bstrOut += "File name:\t" + _bstrFileName;	
	_bstrOut += "\n";
	_bstrOut += "Application:\t" + _bstrApp;

	VTMessageBox( _bstrOut, "Ole object info", MB_OK | MB_ICONINFORMATION );

	return S_OK;
	
}



//////////////////////////////////////////////////////////////////////
//
//
//	class CUpdateOleObject
//
//
//////////////////////////////////////////////////////////////////////
CUpdateOleObject::CUpdateOleObject()
{

}

//////////////////////////////////////////////////////////////////////
CUpdateOleObject::~CUpdateOleObject()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CUpdateOleObject::DoInvoke()
{
	IOleViewPtr ptrV( m_ptrTarget );
	if( ptrV == 0 )
		return E_FAIL;

	IDataContext3Ptr ptrC( ptrV );
	if( ptrC == 0 )
		return E_FAIL;

	IUnknown	*punkOleObject = 0;	

	ptrC->GetActiveObject( _bstr_t(szTypeOleItem), &punkOleObject );	
	if( !punkOleObject )
		return E_FAIL;

	IOleObjectUIPtr ptr = punkOleObject;
	
	if( punkOleObject != 0 )
		punkOleObject->Release();	punkOleObject = 0;

	if( ptr == 0 )
		return E_FAIL;

	ptr->ForceUpdate( );	

	return S_OK;
}
