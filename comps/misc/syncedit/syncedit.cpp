// syncedit.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "syncedit.h"
#include "editorint.h"
#include "undoint.h"
#include "binary.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSynceditApp

BEGIN_MESSAGE_MAP(CSynceditApp, CWinApp)
	//{{AFX_MSG_MAP(CSynceditApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSynceditApp construction

CSynceditApp::CSynceditApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSynceditApp object

CSynceditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSynceditApp initialization

BOOL CSynceditApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	CCompRegistrator	rm( szPluginApplication );
	rm.RegisterComponent( "SyncEditManager.SyncEdit" );
	CActionRegistrator	ra;
	ra.RegisterActions();

	return S_OK;
}


// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll( false );

	CCompRegistrator	rm( szPluginApplication );
	rm.UnRegisterComponent( "SyncEditManager.SyncEdit" );
	CActionRegistrator	ra;
	ra.RegisterActions( false );

	return S_OK;
}

class CSyncEditManager : public CCmdTargetEx, 
					public CEventListenerImpl
{
	DECLARE_DYNCREATE(CSyncEditManager);
	GUARD_DECLARE_OLECREATE(CSyncEditManager);

	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();
public:
	CSyncEditManager();
	~CSyncEditManager();

	BEGIN_INTERFACE_PART(Sync, ISyncEditManager)
		com_call GetEnabled( DWORD *pdwEnable );
		com_call SetEnabled( DWORD dwEnable );
	END_INTERFACE_PART(Sync)

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
protected:
	bool	m_bEnabled;

};

IMPLEMENT_DYNCREATE(CSyncEditManager, CCmdTargetEx);
// {4853B27B-8E24-4df2-89F6-7AE4AC80F592}
GUARD_IMPLEMENT_OLECREATE(CSyncEditManager, "SyncEditManager.SyncEdit", 
0x4853b27b, 0x8e24, 0x4df2, 0x89, 0xf6, 0x7a, 0xe4, 0xac, 0x80, 0xf5, 0x92);

CSyncEditManager::CSyncEditManager()
{
	_OleLockApp( this );
	Register( GetAppUnknown() );
	m_bEnabled = false;
}

CSyncEditManager::~CSyncEditManager()
{
	UnRegister( GetAppUnknown() );
	_OleUnlockApp( this );
}

BEGIN_INTERFACE_MAP(CSyncEditManager, CCmdTargetEx)
	INTERFACE_PART(CSyncEditManager, IID_IEventListener, EvList)
	INTERFACE_PART(CSyncEditManager, IID_ISyncEditManager, Sync)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CSyncEditManager, Sync)

HRESULT CSyncEditManager::XSync::GetEnabled( DWORD *pdwEnable )
{
	METHOD_PROLOGUE_EX(CSyncEditManager, Sync)
	*pdwEnable = pThis->m_bEnabled;
	return S_OK;
}
HRESULT CSyncEditManager::XSync::SetEnabled( DWORD dwEnable )
{
	METHOD_PROLOGUE_EX(CSyncEditManager, Sync)
	pThis->m_bEnabled = dwEnable != 0;
	return S_OK;
}

void CSyncEditManager::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !m_bEnabled )
		return;
	if( !strcmp( pszEvent, szEventAfterActionExecute ) )
	{
		static bool bInside = false;
		//if( bInside )return;
		bInside = true;
		if( CheckInterface( punkHit, IID_IEditorAction ) )
		{
			CPtrList	list;

			IEditorActionPtr	ptrEA( punkHit );
			IActionPtr		ptrA(  punkHit );
			IUndoneableActionPtr	ptrUA(  punkHit );

			//get target image and store it to processed list
			IUnknown	*punkImage = 0;
			ptrEA->GetImage( &punkImage );
			if( !punkImage )return;


			CString strType = ::GetObjectKind(punkImage);

			CRect	rect = NORECT;

			bool bValidPunk = false;
			if(strType == CString(szTypeImage))
			{
				CImageWrp	image( punkImage );
				rect = image.GetRect();
				bValidPunk = true;
			}
			else if(strType == CString(szTypeBinaryImage))
			{
				CBinImageWrp	binary( punkImage );
				CPoint ptTopLeftGetOffset(0,0);
				binary->GetOffset(&ptTopLeftGetOffset);
				rect = CRect(ptTopLeftGetOffset, CSize(binary.GetWidth(), binary.GetHeight()));
				bValidPunk = true;
			}

			punkImage->Release();
			

			if(!bValidPunk)
				return;
			

			list.AddTail( punkImage );

			//get differences
			int		nDiffCount = 0, nStart = 0;
			void	**ppDiffs = 0;
			DWORD	dwDiffFlags = 0;
			ptrEA->GetDifferences( &ppDiffs, &nStart, &nDiffCount, &dwDiffFlags );

			//get the view
			IUnknown	*punkAI = 0;
			ptrA->GetActionInfo( &punkAI );
			ASSERT( punkAI );
			IActionInfoPtr	ptrAI( punkAI );
			punkAI->Release();


			GuidKey	key;
			ptrUA->GetTargetKey( &key );

			
			IApplicationPtr	ptrApp( GetAppUnknown() );
			IUnknown		*punkTargetMan = 0;
			ptrApp->GetTargetManager( &punkTargetMan );
			IActionTargetManagerPtr	ptrTargetMan = punkTargetMan;
			punkTargetMan->Release();

			IUnknown	*punkTarget = 0;
			ptrTargetMan->GetTargetByKey( key, &punkTarget );
			IDocumentSitePtr	ptrDoc( punkTarget );
			punkTarget->Release();

			IUndoListPtr	ptrUndoList( ptrDoc );

			TPOS	lpos = 0;
			ptrDoc->GetFirstViewPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkView = 0;
				ptrDoc->GetNextView( &punkView, &lpos );
				if( !punkView )continue;

				IImageViewPtr	ptrIV( punkView );
				punkView->Release();

				if( ptrIV == 0 ) continue;

				IUnknown	*punkImage = GetActiveObjectFromContext( ptrIV, strType );
				if( punkImage )
				{
					punkImage->Release();

					if( list.Find( punkImage ) )
					{
/*						IWindowPtr sptrWnd(ptrIV);
						HANDLE hWnd = NULL;
						if (sptrWnd != 0)
						{
							sptrWnd->GetHandle(&hWnd);
							::InvalidateRect((HWND)hWnd, NULL, TRUE);
						}*/
						continue;
					}

					list.AddTail( punkImage );
				}

/////////
				IViewPtr sptrView = punkView;
				IUnknown *punk = 0;
				sptrView->GetDocument( &punk );

				IDocumentSitePtr sptrDoc = punk;
				if( punk )
					punk->Release();
				punk = 0;

				sptrDoc->GetActiveView( &punk );
				IViewPtr sptrActiveView = punk;

				if( punk )
					punk->Release();
				punk = 0;
/////////
				sptrDoc->SetActiveView( punkView );

				//construct new action object

				IUnknown	*punkAction= 0;
				ptrAI->ConstructAction( &punkAction );
				IActionPtr			ptrNewA( punkAction ); 
				IEditorActionPtr	ptrNewEA( punkAction );


				ptrNewA->AttachTarget( ptrIV );
				ptrNewEA->SetDifferences( ppDiffs, rect, dwDiffFlags );
				ptrNewA->Invoke();

				if( ptrUndoList != 0 )
					ptrUndoList->AddAction( ptrNewA );

				punkAction->Release();

				if( dwDiffFlags & dfBinary )
				{
					DWORD dwFlags = 0;
					ptrIV->GetImageShowFlags( &dwFlags );

					if( !( dwFlags & isfBinaryFore ) )
						SetAppropriateView( punkView, avtBinary );
				}
				sptrDoc->SetActiveView( sptrActiveView );
				IWindowPtr sptrWnd(sptrView);
				HANDLE hwnd;
				sptrWnd->GetHandle(&hwnd);
				InvalidateRect((HWND)hwnd,NULL,TRUE);

			}
		}
		else if( CheckInterface( punkHit, IID_IObjectAction ) )
		{
			CPtrList	list;

			IObjectActionPtr	ptrOA( punkHit );
			IActionPtr		ptrA(  punkHit );
			IUndoneableActionPtr	ptrUA(  punkHit );

			//get target image and store it to processed list
			CPtrList	ptrList;

			LONG_PTR lpos = 0;
			ptrOA->GetFirstObjectPosition( &lpos );
			if( !lpos )return;

			IUnknown	*punkObject = 0;
			ptrOA->GetNextObject( &punkObject, &lpos );
			if( !punkObject )return;
			INamedDataObject2Ptr	ptrOldNamedObject( punkObject );
			punkObject->Release();

			IUnknown	*punkParent = 0;
			ptrOldNamedObject->GetParent( &punkParent );
			if( !punkParent )return;
			punkParent->Release();

			list.AddTail( punkParent );

			//get the view
			IUnknown	*punkAI = 0;
			ptrA->GetActionInfo( &punkAI );
			ASSERT( punkAI );
			IActionInfoPtr	ptrAI( punkAI );
			punkAI->Release();


			GuidKey	key;
			ptrUA->GetTargetKey( &key );

			
			IApplicationPtr	ptrApp( GetAppUnknown() );
			IUnknown		*punkTargetMan = 0;
			ptrApp->GetTargetManager( &punkTargetMan );
			IActionTargetManagerPtr	ptrTargetMan = punkTargetMan;
			punkTargetMan->Release();

			IUnknown	*punkTarget = 0;
			ptrTargetMan->GetTargetByKey( key, &punkTarget );
			IDocumentSitePtr	ptrDoc( punkTarget );
			punkTarget->Release();

			IUndoListPtr	ptrUndoList( ptrDoc );

			TPOS pos = 0;
			ptrDoc->GetFirstViewPosition( &pos );

			while( lpos )
			{
				IUnknown	*punkView = 0;
				ptrDoc->GetNextView( &punkView, &pos );
				if( !punkView )continue;

				IImageViewPtr	ptrIV( punkView );
				punkView->Release();

				if( ptrIV == 0 ) continue;

				IUnknown	*punkOL= GetActiveObjectFromContext( ptrIV, szTypeObjectList );
				IUnknown	*punkDL= GetActiveObjectFromContext( ptrIV, szDrawingObjectList );

				if( punkOL )punkOL->Release();
				if( punkDL )punkDL->Release();

				if( list.Find( punkDL )||list.Find( punkOL ) )
					continue;

				if( punkDL )list.AddTail( punkDL );
				if( punkOL )list.AddTail( punkOL );
				
				//construct new action object
				IUnknown	*punkAction= 0;
				ptrAI->ConstructAction( &punkAction );
				IActionPtr			ptrNewA( punkAction ); 
				IObjectActionPtr	ptrNewOA( punkAction );

				ptrNewA->AttachTarget( ptrIV );


				LONG_PTR	lposO = 0;
				ptrOA->GetFirstObjectPosition( &lposO );

				while( lposO )
				{
					IUnknown	*punkObject = 0;
					ptrOA->GetNextObject( &punkObject, &lposO );
					ASSERT( punkObject );
					IUnknown	*punkNewObject = ::CloneObject( punkObject );
					punkObject->Release();
					INamedDataObject2Ptr	ptrNamedObject( punkNewObject );
					punkNewObject->Release();
					ptrNamedObject->SetParent( 0, 0 );
					ptrNewOA->AddObject( ptrNamedObject );
				}

				ptrNewA->Invoke();

				if( ptrUndoList != 0 )
					ptrUndoList->AddAction( ptrNewA );

				punkAction->Release();

				CWnd	*pwnd = ::GetWindowFromUnknown( ptrIV );
				pwnd->Invalidate();
			}
		}
		bInside = false;
	}
}									
//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSyncEdit, CCmdTargetEx);

// olecreate 

// {F25D3C28-87E7-4e49-9793-5F84B100DA14}
GUARD_IMPLEMENT_OLECREATE(CActionSyncEdit, "SyncEdit.SyncEdit",
0xf25d3c28, 0x87e7, 0x4e49, 0x97, 0x93, 0x5f, 0x84, 0xb1, 0x0, 0xda, 0x14);
// guidinfo 

// {92E4E558-3A35-4a67-9526-1B2735E05B5E}
static const GUID guidSyncEdit =
{ 0x92e4e558, 0x3a35, 0x4a67, { 0x95, 0x26, 0x1b, 0x27, 0x35, 0xe0, 0x5b, 0x5e } };


//[ag]6. action info

ACTION_INFO_FULL(CActionSyncEdit, IDS_ACTION_SYNEDIT, -1, -1, guidSyncEdit);
//[ag]7. targets

ACTION_TARGET(CActionSyncEdit, "anydoc");
//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionSyncEdit implementation
CActionSyncEdit::CActionSyncEdit()
{
}

CActionSyncEdit::~CActionSyncEdit()
{
}

bool CActionSyncEdit::Invoke()
{
	IUnknown	*punkSyncMan = _GetOtherComponent( GetAppUnknown(), IID_ISyncEditManager );
	if( !punkSyncMan )return false;
	ISyncEditManagerPtr	ptrSyncMan( punkSyncMan );

	DWORD dwEnabled = 0;
	ptrSyncMan->GetEnabled( &dwEnabled );
	dwEnabled ^= 1;
	ptrSyncMan->SetEnabled( dwEnabled );

	punkSyncMan->Release();
	return true;
}

bool CActionSyncEdit::IsAvaible()
{
	IApplicationPtr sptrApp(GetAppUnknown());
	IUnknownPtr punkActiveDoc;
	sptrApp->GetActiveDocument(&punkActiveDoc);
	IDocumentSitePtr sptrActiveDoc = punkActiveDoc;
	if (sptrActiveDoc == 0)
		return false;
	TPOS lPosView;
	int nViews = 0;
	sptrActiveDoc->GetFirstViewPosition(&lPosView);
	while (lPosView)
	{
		IUnknownPtr punkView;
		sptrActiveDoc->GetNextView(&punkView, &lPosView);
		if (++nViews >= 2)
			break;
	}
	return nViews >= 2;
}

bool CActionSyncEdit::IsChecked()
{
	IUnknown	*punkSyncMan = _GetOtherComponent( GetAppUnknown(), IID_ISyncEditManager );
	if( !punkSyncMan )return false;
	ISyncEditManagerPtr	ptrSyncMan( punkSyncMan );
	DWORD dwEnabled = 0;
	ptrSyncMan->GetEnabled( &dwEnabled );
	punkSyncMan->Release();
	return dwEnabled != 0;
}

DWORD CActionSyncEdit::GetActionState()
{	
	DWORD	dw = __super::GetActionState();
	if ((dw&afEnabled)==0 && (dw&afChecked) != 0)
		dw &= ~afChecked;
	return dw;
}
