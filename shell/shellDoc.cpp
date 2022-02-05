// shellDoc.cpp : implementation of the CShellDoc class
//

#include "stdafx.h"
#include "shell.h"

#include "shellDoc.h"
#include "docs.h"
#include "targetman.h"
#include "ShellView.h"
#include "ShellFrame.h"
#include "MainFrm.h"
#include "misc.h"
#include "undoint.h"
#include "wnd_misc.h"
#include "mapi.h"


BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

/////////////////////////////////////////////////////////////////////////////
// CShellDoc

IMPLEMENT_DYNCREATE(CShellDoc, CDocument)

BEGIN_MESSAGE_MAP(CShellDoc, CDocument)
	//{{AFX_MSG_MAP(CShellDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CShellDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CShellDoc)
	DISP_FUNCTION(CShellDoc, "GetViewsCount", GetViewsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "GetView", GetView, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CShellDoc, "GetFramesCount", GetFramesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "GetFrame", GetFrame, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CShellDoc, "GetData", GetData, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "GetActiveDataContext", GetActiveDataContext, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "GetDocPathName", GetDocPathName, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "GetActiveDocumentWindow", GetActiveDocumentWindow, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "GetActiveView", _GetActiveView, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "GetModifiedFlag", dispGetModifiedFlag, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "SetModifiedFlag", dispSetModifiedFlag, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CShellDoc, "GetDocType", GetDocType, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CShellDoc, "Reload", Reload, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IShell to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C48CE9B7-F947-11D2-A596-0000B493FF1B}
static const IID IID_IShell =
{ 0xc48ce9b7, 0xf947, 0x11d2, { 0xa5, 0x96, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CShellDoc, CDocument)
	INTERFACE_PART(CShellDoc, IID_IShell, Dispatch)
	INTERFACE_PART(CShellDoc, IID_ICompManager, CompMan)
	INTERFACE_PART(CShellDoc, IID_ICompRegistrator, CompReg)
	INTERFACE_PART(CShellDoc, IID_IDocumentSite, Site)
	INTERFACE_PART(CShellDoc, IID_IDocumentSite2, Site)
	INTERFACE_PART(CShellDoc, IID_INumeredObject, Num)		
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[0])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[1])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[2])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[3])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[4])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[5])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[6])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[7])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[8])
	INTERFACE_AGGREGATE(CShellDoc, m_aggrs.m_pinterfaces[9])
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CShellDoc, Site);

HRESULT CShellDoc::XSite::GetDocumentTemplate( IUnknown **punk )
{
	_try_nested(CShellDoc, Site, GetDocumentTemplate)
	{
		*punk = pThis->GetDocTemplate()->m_punkTemplate;
		(*punk)->AddRef();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::GetFirstViewPosition(TPOS *plPos)
{
	_try_nested(CShellDoc, Site, GetDocumentTemplate)
	{
		*plPos = pThis->GetFirstViewPosition();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::GetNextView(IUnknown **ppunkView, TPOS *plPos)
{
	_try_nested(CShellDoc, Site, GetDocumentTemplate)
	{
		POSITION	pos = (POSITION)*plPos;

		CShellView *pview = (CShellView *)pThis->GetNextView( pos );
		*plPos = pos;
		*ppunkView = pview->GetViewUnknown();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::GetActiveView( IUnknown **ppunkView )
{
	_try_nested(CShellDoc, Site, GetActiveView)
	{
		*ppunkView = 0;
		
		if( pThis->m_punkActiveView )
		{
			*ppunkView = pThis->m_punkActiveView;
			(*ppunkView)->AddRef();
		}

		return S_OK;
	}
	_catch_nested;
}


HRESULT CShellDoc::XSite::SetActiveView( IUnknown *punkView )
{
	_try_nested(CShellDoc, Site, SetActiveView)
	{
		if( punkView != NULL )
		{
			pThis->m_punkActiveView = punkView;	
			INamedDataPtr sptrData = this;
			if (sptrData != 0 && CheckInterface(punkView, IID_IDataContext3))
				sptrData->SetActiveContext( punkView );


			/*if( punkView )
			{
				CShellView	*pview = (CShellView*)pThis->GetActiveView();
				IUnknown	*punkV = pview->GetViewUnknown();
				ASSERT( punkV );

				if( punkView != punkV )
				{
					pview->SetFocus();
					CWnd	*pwnd = ::GetWindowFromUnknown( punkView );
					pwnd->SetFocus();
				}

				punkV->Release();
			}*/
		}
		else
		{
			pThis->m_punkActiveView = 0;
			
			INamedDataPtr sptrData = this;
			if (sptrData != 0 )
				sptrData->SetActiveContext( 0 );
				

		}
		return S_OK;
	}
	_catch_nested;
}


HRESULT CShellDoc::XSite::GetPathName( BSTR *pbstrPathName )
{
	_try_nested(CShellDoc, Site, GetPathName)
	{
		CString	str = pThis->GetPathName();

		if( str.IsEmpty() )
			str = pThis->GetTitle();
		*pbstrPathName = str.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::GetDocType( BSTR *pbstrDocType )
{
	_try_nested(CShellDoc, Site, GetDocType)
	{
		CString	str = pThis->GetDocType();

		*pbstrDocType = str.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CShellDoc::XSite::SaveDocument( BSTR bstrFileName )
{
	_try_nested(CShellDoc, Site, SaveDocument)
	{		
		CString	str = bstrFileName;
		return pThis->DoSave( str ) ? S_OK : S_FALSE;		
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::GetPathName2( BSTR *pbstrPathName, BSTR *pbstrTitle )
{
	_try_nested(CShellDoc, Site, GetPathName2)
	{
		CString	strPN = pThis->GetPathName();
		CString strT = pThis->GetTitle();
		if (pbstrPathName)
			*pbstrPathName = strPN.AllocSysString();
		if (pbstrTitle)
			*pbstrTitle = strT.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::GetSavingPathName( BSTR *pbstrPathName )
{
	_try_nested(CShellDoc, Site, GetPathName2)
	{
		if (pbstrPathName)
			*pbstrPathName = pThis->m_strSaving.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::LockDocument( BOOL bLock )
{
	_try_nested(CShellDoc, Site, LockDocument)
	{
		pThis->m_bLockDoc = bLock;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellDoc::XSite::IsDocumentLocked( BOOL *pbLock )
{
	_try_nested(CShellDoc, Site, IsDocumentLocked)
	{
		*pbLock = pThis->m_bLockDoc;
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
// CShellDoc construction/destruction

CShellDoc::CShellDoc()
{
	//m_benable_map	= false;

	EnableAutomation();
	EnableAggregation();

	_OleLockApp( this );

	m_pActiveView = 0;
	m_punkActiveView = 0;

	m_bFirstViewInitialized = false;
	m_bEmptyDoc = true;
	m_guidLoadedObject = INVALID_KEY;
	m_bLockDoc = FALSE;
}

CShellView	*CShellDoc::GetActiveView()
{
	return m_pActiveView;
}

void CShellDoc::SetActiveView( CShellView *pView )
{
	IViewPtr sptrView = pView ? pView->GetControllingUnknown() : 0;
	m_pActiveView = pView;

	if( pView )
		m_punkActiveView = sptrView;
	else
		m_punkActiveView = 0;

	INamedDataPtr sptrData = GetControllingUnknown();
	if (sptrData != 0)// && sptrView != 0)
		sptrData->SetActiveContext(sptrView);


	if( !m_bFirstViewInitialized )
	{
		m_bFirstViewInitialized = true;
		OnFirstViewInitialized();
	}
}

void CShellDoc::OnFirstViewInitialized()
{
	CString	str;
	CShellDocTemplate *ptempl = GetDocTemplate();

	//register target for interactive actions here, because
	//window object alredy initialized
	ptempl->GetDocString( str, CDocTemplate::docName );
	str += "\n";
	str += szPluginDocument;

	sptrIDocument	sptr( GetControllingUnknown() );
	g_TargetManager.RegisterTarget( sptr, str );

	CString str_doc_name = m_strTitle;
	str_doc_name += ":AutoExec";
	::ExecuteObjectScript( GetControllingUnknown(), "AutoExec", str_doc_name );
}

CShellDoc::~CShellDoc()
{
	m_benable_map	= false;

	CShellDocTemplate *pTempl = static_cast<CShellDocTemplate*>( GetDocTemplate() );
	pTempl->SetActiveDocument( 0 );

//detach this document from frame windows
	POSITION	pos = GetFirstFramePosition();

	while( pos )
	{
		CShellFrame *pFrm = GetNextFrame( pos );
		pFrm->SetDocument( 0 );
	}

	_OleUnlockApp( this );
}																		

BOOL CShellDoc::OnNewDocument()
{
	if( IsSDIMode() )
		return FALSE;

	DeInitNotEmpty();
	m_guidLoadedObject = INVALID_KEY;
	m_strFilterFileName.Empty();

	if (!CDocument::OnNewDocument())
		return FALSE;



	IDocumentPtr ptrIDocument( GetDocumentInterface( false ) );
	if( ptrIDocument->CreateNew() != S_OK )return false;

	m_bEmptyDoc = false;

	return TRUE;
}

bool CShellDoc::Init()
{	
	CString	str;
	CShellDocTemplate *ptempl = GetDocTemplate();
//init aggregates
	ptempl->GetDocString( str, CDocTemplate::docName );
	str += "\n";
	str += szPluginDocumentsAggr;

	m_aggrs.AttachComponentGroup( str );
	m_aggrs.SetOuterUnknown( GetControllingUnknown() );
	m_aggrs.Init();
//init components						
 	AttachComponentGroup( szPluginDocument );
	SetRootUnknown( GetControllingUnknown() );

	if( !CCompManager::Init() )
		return false;

	
	/////////////////////////////////
	//Data Base support
	//
	//
	// We must kill NameData agregate cos IDBaseDocument has private INameData 
	//
	sptrIDBaseDocument spDBaseDoc( GetControllingUnknown() );
	if( spDBaseDoc )
	{			  		
		int nRemoveComponent = -1;
		spDBaseDoc->InitNamedData( );		
		//IUnknown* punkPrivateDBNamedData = NULL;
		//spDBaseDoc->GetPrivateNamedData( &punkPrivateDBNamedData );

		//if( punkPrivateDBNamedData )
		{
			//punkPrivateDBNamedData->Release();
			for(int i=0;i<m_aggrs.GetComponentCount();i++)
			{					
				if( m_aggrs.m_pinterfaces[i] != NULL )
				{
					if( CheckInterface( m_aggrs.m_pinterfaces[i], IID_INamedData ) &&
						//cos IDBaseDocument impl. INameData interface
						!CheckInterface( m_aggrs.m_pinterfaces[i], IID_IDocument )
						&& (nRemoveComponent == -1)
						)
					{
						nRemoveComponent = i;						
					}										
				}				
			}
		}			

		if( nRemoveComponent != -1 )					
			m_aggrs.RemoveComponent( nRemoveComponent );

	}						
	/////////////////////////////////
	
	


	ASSERT( CheckInterface( GetControllingUnknown(), IID_IDocument ) );
	ASSERT( CheckInterface( GetControllingUnknown(), IID_INotifyController ) );
	ASSERT( CheckInterface( GetControllingUnknown(), IID_INamedData ) );
	ASSERT( CheckInterface( GetControllingUnknown(), IID_IDataContext2 ) );
//init the document implementation
	sptrIDocument	sptr( GetControllingUnknown() );

	if( sptr == 0 )
		return false;

	sptr->Init( GetControllingUnknown(), ptempl->m_punkTemplate, theApp.GetControllingUnknown() );
	IDataContextPtr sptrC = GetControllingUnknown();
	if (sptrC)
		sptrC->AttachData(GetControllingUnknown());

	return true;
}

IDocument *CShellDoc::GetDocumentInterface( bool bAddRef )
{
	IUnknown *punk = GetControllingUnknown();

	if( !punk )
		return 0;

	IDocument	*pIDocument = 0;

	punk->QueryInterface( IID_IDocument, (void **)&pIDocument );

	if( !bAddRef  )
		pIDocument->Release();

	return pIDocument;
}



/////////////////////////////////////////////////////////////////////////////
// CShellDoc serialization

void CShellDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CShellDoc diagnostics

#ifdef _DEBUG
void CShellDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CShellDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

bool CShellDoc::IsNativeFormat(IUnknownPtr &punkFF)
{
	CString sPathName = GetPathName();
	CShellDocTemplate	*pdocTemplate = GetDocTemplate();
	IFileFilterPtr punk;
	if( pdocTemplate->GetFileFilter( sPathName, true, &punk ) == CDocTemplate::noAttempt ||
		punk == 0 )
		return true;
	punkFF = punk;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CShellDoc commands

BOOL CShellDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_strSaving = lpszPathName;
	CShellView	*pview = GetActiveView();

	if( pview )
	{
		IDataContext3Ptr	ptrSrc( pview->GetControllingUnknown() );
		IDataContext3Ptr	ptrDst( GetControllingUnknown() );

		// -->> paul 27.10.2000
		if( ptrDst != NULL )
			ptrDst->SynchronizeWithContext( ptrSrc );
		// --<< paul 27.10.2000

		//save last active view
		//paul 18.04.2003
		{
			IPersistPtr ptr_persist( pview->GetControllingUnknown() );
			if( ptr_persist )
			{
				CLSID clsid = {0};
				ptr_persist->GetClassID( &clsid );

				BSTR bstr_prog_id = 0;
				::ProgIDFromCLSID( clsid, &bstr_prog_id );
				if( bstr_prog_id )
				{					
					::SetValue( GetControllingUnknown(), LAST_VIEW_SECTION, LAST_VIEW_PROGID, _bstr_t( bstr_prog_id ) );
					::CoTaskMemFree( bstr_prog_id );	bstr_prog_id = 0;
				}
			}
		}		
	}


	CShellDocManager *pman = theApp.GetDocManager();
	pman->GetDispatchImpl()->FireOnSaveDocument( lpszPathName, GetIDispatch( false ) );

	_bstr_t	bstrFileName( lpszPathName );
//try to save native document format
	sptrIDocument sptrDoc( GetDocumentInterface( false ) );

	if( sptrDoc->SaveNativeFormat( bstrFileName ) == S_OK )
		return TRUE;
	
	CShellDocTemplate	*pdocTemplate = GetDocTemplate();
//get file filter by filename
	IFileFilter *punk  = 0;
	if( pdocTemplate->GetFileFilter( lpszPathName, true, &punk ) == CDocTemplate::noAttempt )
	{
		if( punk )punk->Release();
		return 0;
	}

//it is native file format
	if( !punk )	
	{

		sptrINamedData		sptrD( GetControllingUnknown() );
		sptrD->EnableBinaryStore( true );	//it is more beatifull, if it is binary format
		
		sptrIFileDataObject	sptr( GetControllingUnknown() );	//3. Получаем NamedData от документа
		if( sptr->SaveFile( bstrFileName ) != S_OK )
		{
			return false;
		}
		pdocTemplate->SetLastDocumentFileName( lpszPathName );
		SetModifiedFlag( false );
		return true;
	}

	sptrIFileFilter	sptrIFilter( punk );
	sptrIFileDataObject	sptrIFile( punk );
	punk->Release();

//get NamedData and active DataContext
	sptrINamedData			sptrD( GetControllingUnknown() );
	IUnknown	*punkView = pview->GetControllingUnknown();
	

	ASSERT( CheckInterface( punkView, IID_IDataContext ) );
//init FileFilter
	sptrIFilter->AttachNamedData( sptrD, punkView );
	::FireEvent( GetAppUnknown(), szEventBeforeSave, GetControllingUnknown(), 0, (void *)lpszPathName, (long)strlen(lpszPathName)+1 );
//check the base key
	IFileFilter2Ptr	ptrFF2( sptrIFilter );
	if( ptrFF2 != 0 && m_guidLoadedObject != INVALID_KEY && 
		m_strFilterFileName == lpszPathName )
	{
		int	nTypesCount = 0;
		ptrFF2->GetObjectTypesCount( &nTypesCount );
		if( nTypesCount > 0 )
		{
			BSTR	bstrType = 0;
			ptrFF2->GetObjectType( 0, &bstrType );

			IDataContextPtr	ptrC( sptrD );
			IUnknown	*punkObj = 0;
			ptrC->GetActiveObject( bstrType, &punkObj );

			INamedDataObject2Ptr	ptrN( punkObj );
			if( punkObj )punkObj->Release();

			if( ptrN != 0 )
			{
				GUID	guid;
				ptrN->GetBaseKey( &guid );

				if( guid !=	m_guidLoadedObject )
				{
					if( AfxMessageBox( IDS_NOTSOURCEOBJECT, MB_YESNO ) == IDNO )
					{
						sptrIFilter->AttachNamedData( 0, 0);
						::SysFreeString( bstrType );
						return false;
					}
					m_guidLoadedObject = guid;
				}
			}
			::SysFreeString( bstrType );
		}
	}

//write data
	bool bSucceded = sptrIFile->SaveFile( bstrFileName ) == S_OK;
	m_strFilterFileName = lpszPathName;

//attach named data
	sptrIFilter->AttachNamedData( 0, 0);

	if( !bSucceded )
		return false;

	//don't set last file if not native format
	//pdocTemplate->SetLastDocumentFileName( lpszPathName );

	::FireEvent( GetAppUnknown(), szEventAfterSave, GetControllingUnknown(), 0, (void *)lpszPathName, (long)strlen(lpszPathName)+1 );
	SetModifiedFlag( false );

	return true;
}

BOOL CShellDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if( IsSDIMode() )
		return FALSE;

	m_guidLoadedObject = INVALID_KEY;
	m_strFilterFileName.Empty();

	DeInitNotEmpty();
	m_bEmptyDoc = false;

	_bstr_t	bstrFileName( lpszPathName );
//try to save native document format
	sptrIDocument sptrDoc( GetDocumentInterface( false ) );

	::FireEvent( GetAppUnknown(), szEventBeforeOpen, GetControllingUnknown(), 0, (void *)lpszPathName, (long)strlen(lpszPathName)+1 );

	if( sptrDoc->LoadNativeFormat( bstrFileName ) == S_OK )
		return TRUE;
	
	CShellDocTemplate	*pdocTemplate = GetDocTemplate();

//get file filter by filename

	//get file filter by filename
	IFileFilter *punk  = 0;
	CDocTemplate::Confidence conf = pdocTemplate->GetFileFilter( lpszPathName, true, &punk );

	if( conf == CDocTemplate::yesAlreadyOpen ||
		conf == CDocTemplate::noAttempt )
	{
		if( punk )punk->Release();
		return conf == CDocTemplate::yesAlreadyOpen;
	}

	if( !punk )	//It is native document format
	{
		sptrIFileDataObject	sptr( GetControllingUnknown() );

		if( sptr->LoadFile( bstrFileName ) != S_OK )
			return false;

		pdocTemplate->SetLastDocumentFileName( lpszPathName );

		::FireEvent( GetAppUnknown(), szEventAfterOpen, GetControllingUnknown(), 0, (void *)lpszPathName, (long)strlen(lpszPathName)+1 );

		return true;
	}

	sptrIFileFilter	sptrIFilter( punk );
	sptrIFileDataObject	sptrIFile( punk );
	punk->Release();

//remove all entries from NamedData
	sptrINamedData			sptrD( GetControllingUnknown() );
	sptrD->DeleteEntry( 0 );

//initialize FileFilter
	sptrIFilter->AttachNamedData( sptrD, 0 );
//and read data
	bool bSucceded = sptrIFile->LoadFile( bstrFileName ) == S_OK;

	if( !bSucceded )
	{
		sptrIFilter->AttachNamedData( 0, 0 );
		return false;
	}


//get the first object key
	IFileFilter2Ptr	ptrFF2( punk );
	int		nCreatedCount = 0;

	ptrFF2->GetCreatedObjectCount( &nCreatedCount );

	IUnknown	*punkObjToActivate = 0;


	if( nCreatedCount > 0 )
	{
		IUnknown	*punkObject = 0;
		ptrFF2->GetCreatedObject( 0, &punkObject );

		INamedDataObject2Ptr	ptrNamed2( punkObject );
		if( punkObject )punkObject->Release();
		if( ptrNamed2 != 0 )
		{
			ptrNamed2->GetBaseKey( &m_guidLoadedObject );
			m_strFilterFileName = lpszPathName;
		}

		if( !punkObjToActivate )
		{
			BOOL	base;
			ptrNamed2->IsBaseObject( &base );

			if( base )
				punkObjToActivate = ptrNamed2;
		}
	}

	if( punkObjToActivate )
	{
		CShellView	*pview = GetActiveView();
		if( pview )
		{
			IDataContextPtr	ptrC( pview->GetControllingUnknown() );
			if( ptrC != 0 )ptrC->SetActiveObject( 0, punkObjToActivate, aofActivateDepended );

		}
	}

	sptrIFilter->AttachNamedData( 0, 0 );


	//no native format
	//pdocTemplate->SetLastDocumentFileName( lpszPathName );

	::FireEvent( GetAppUnknown(), szEventAfterOpen, GetControllingUnknown(), 0, (void *)lpszPathName, (long)strlen(lpszPathName)+1 );

	return true;
}

void CShellDoc::Reload()
{
	CString sPath = GetPathName();
	OnOpenDocument(sPath);
}


void CShellDoc::FirenNewDocument( const char* psz_file_name )
{
	CShellDocManager *pman = theApp.GetDocManager();

	if( !pman )
		return;

	if( psz_file_name )	
	{
		::FireEvent( GetControllingUnknown(), "OnOpenDocument", GetControllingUnknown(), 0, 0, 0 );
		pman->GetDispatchImpl()->FireOnOpenDocument( psz_file_name, GetIDispatch( false ) );		
		pman->GetDispatchImpl()->FireOnActivateDocument( GetIDispatch( false ) );
	}
	else	
	{
		::FireEvent( GetControllingUnknown(), "OnNewDocument", GetControllingUnknown(), 0, 0, 0 );
		pman->GetDispatchImpl()->FireOnNewDocument( GetIDispatch( false ) );		
		pman->GetDispatchImpl()->FireOnActivateDocument( GetIDispatch( false ) );
	}

	

}

void CShellDoc::OnFinalRelease() 
{
	//CDocument::OnFinalRelease();

	ASSERT( false );
}

void CShellDoc::DeleteContents()
{
	CString str_doc_name = m_strTitle;
	str_doc_name += ":TerminateExec";

	::ExecuteObjectScript( GetControllingUnknown(), "TerminateExec", str_doc_name );

	CShellDocTemplate	*ptempl = GetDocTemplate();
//	if( ptempl->IsAutoSave() )SaveModified();
	CDocument::DeleteContents();
}

void CShellDoc::OnCloseDocument() 
{	
	::FireEvent( GetAppUnknown(), szEventBeforeClose, GetControllingUnknown(), 0, 0 );

	DeInitNotEmpty();

	{
		IDataContextPtr sptrC = GetControllingUnknown();
		if (sptrC)sptrC->AttachData(0);
	}

	if( IsSDIMode() )
		return;

	CShellDocManager *pman = theApp.GetDocManager();

	VARIANT varRes;
	varRes.vt = VT_BOOL;
	varRes.boolVal = true;

	if( !pman )
		return;
	pman->GetDispatchImpl()->FireOnCloseDocument( GetIDispatch( false ), &varRes );

	if( !varRes.boolVal )
		return;
	
	IUndoListPtr	ptrList( GetControllingUnknown() );
	if( ptrList != 0 )ptrList->ClearUndoRedoList();
	ptrList = 0;



	g_TargetManager.UnRegisterTarget( GetControllingUnknown() );

	CCompManager::DeInit();

	GetControllingUnknown()->AddRef();
	m_aggrs.SetOuterUnknown( 0 );


	m_bAutoDelete = false;

	for (POSITION pos = m_viewList.GetHeadPosition(); 0!=pos;)
	{
		CView* pview = (CView*)m_viewList.GetNext(pos);
		ASSERT_VALID(pview);
		CFrameWnd* pFrame = pview->GetParentFrame(); 

		if( !pFrame )
		{
			if( pview->GetSafeHwnd() )
				pview->DestroyWindow();
		}
		else
		{
			ASSERT_VALID(pFrame);

			// and close it
			PreCloseFrame(pFrame);
			if(::IsWindow(pFrame->m_hWnd))
				pFrame->DestroyWindow();
		}
	}
	m_viewList.RemoveAll();

	if( m_viewList.GetHeadPosition() )
	{
		AfxMessageBox( "Internal error: Can't destroy all views" );
	}
	
	::FireEvent( GetAppUnknown(), "AfterClose", GetControllingUnknown(), 0, 0 );

	// clean up contents of document before destroying the document itself
	DeleteContents();

	m_aggrs.DeInit();


	//ASSERT( !m_bAutoDelete );
	//GetControllingUnknown()->Release();
	delete this;
}

CShellDocTemplate *CShellDoc::GetDocTemplate()
{
	return (CShellDocTemplate *)CDocument::GetDocTemplate();
}

long CShellDoc::GetViewsCount() 
{
	POSITION pos = GetFirstViewPosition();
	long nCounter = 0;

	while( pos )
	{
		GetNextView( pos );
		nCounter++;
	}

	return nCounter;
}

LPDISPATCH CShellDoc::GetView(POSITION nPos)
{
	POSITION pos = GetFirstViewPosition();

	while( pos )
	{
		CView *pview = GetNextView( pos );

		if( !nPos-- )
			return pview->GetIDispatch( true );
	}

	return NULL;
}

LPDISPATCH CShellDoc::_GetActiveView() 
{
	if( m_pActiveView )
		return m_pActiveView->GetIDispatch( true );

	return NULL;
}

long CShellDoc::GetFramesCount() 
{
	return (long)m_listFrames.GetCount();
}

LPDISPATCH CShellDoc::GetFrame(POSITION nPos) 
{
	POSITION	pos = GetFirstFramePosition();

	while( pos )
	{
		CShellFrame	*pFrm = GetNextFrame( pos );
		if( !nPos-- )
			return pFrm->GetIDispatch( true );
	}

	return 0;
}

// {ADBA6C03-1D68-11D3-A5D0-0000B493FF1B}
static const IID IID_INamedDataDisp =
{ 0xadba6c03, 0x1d68, 0x11d3, { 0xa5, 0xd0, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };


LPDISPATCH CShellDoc::GetData() 
{
	IDBaseDocumentPtr	ptrDBDoc( GetControllingUnknown() );
	
	IUnknown	*punk = GetControllingUnknown();
	if( ptrDBDoc != 0 )
	{
		ptrDBDoc->GetPrivateNamedData( &punk );
	}
	else
		punk->AddRef();

	IDispatch	*pdisp = 0;
	punk->QueryInterface( IID_INamedDataDisp, (void **)&pdisp );
	punk->Release();

	return pdisp;
}

LPDISPATCH CShellDoc::GetActiveDataContext() 
{
	// TODO: Add your dispatch handler code here

	return NULL;
}

//register frame in document's list
void CShellDoc::AddFrame( CShellFrame *pFrm )
{
	ASSERT( !m_listFrames.Find( pFrm ) );
	
	m_listFrames.AddHead( pFrm );
}

//unregister frame in document's list
void CShellDoc::RemoveFrame( CShellFrame *pFrm )
{
	POSITION	pos = m_listFrames.Find( pFrm );
	ASSERT( pos );

	m_listFrames.RemoveAt( pos );
}

BOOL CShellDoc::IsModified()
{
	CShellDocTemplate	*ptempl = GetDocTemplate();

	if( ::GetValueInt( GetAppUnknown(), "\\General", "PromtCloseModified", 1 ) == 0 )
		return false;

	sptrIFileDataObject	sptrF( GetControllingUnknown() );

	BOOL	bDirty = false;

	sptrF->IsDirty( &bDirty );

	return bDirty;
}


void CShellDoc::SetModifiedFlag(BOOL bModified)
{ 
	IFileDataObject2Ptr	sptrF( GetControllingUnknown() );
	sptrF->SetModifiedFlag( bModified );
}



//returns first position
POSITION CShellDoc::GetFirstFramePosition()
{
	return m_listFrames.GetHeadPosition();
}

//returns frame by position
CShellFrame *CShellDoc::GetNextFrame( POSITION &pos )
{
	return (CShellFrame*)m_listFrames.GetNext( pos );
}

BSTR CShellDoc::GetDocPathName() 
{
	CString strResult = GetPathName();

	return strResult.AllocSysString();
}

BSTR CShellDoc::GetDocType()
{	
	CShellDocTemplate *pTempl = GetDocTemplate();
	return pTempl->GetTemplateName().AllocSysString();
}

void CShellDoc::EnsureViewRemoved( CView *pViewToRemove )
{
	if( GetActiveView() == pViewToRemove )
		SetActiveView( 0 );

/*	POSITION pos = m_viewList.GetHeadPosition();

	while( pos )
	{
		// get frame attached to the view
		CView* pView = (CView*)m_viewList.GetNext( pos );

		if( pViewToRemove == pView )
			m_viewList.RemoveAt( pos );
	}*/
}

BOOL  CShellDoc::IsSDIMode()
{
	CMainFrame* pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CWnd* pActiveWnd = NULL;
	if( pMainFrame )
		pActiveWnd = pMainFrame->_GetActiveFrame();
		
	if( pMainFrame )
	{
		ASSERT_KINDOF( CMainFrame, pMainFrame );

		if( pMainFrame->IsSDIMode() )
			return TRUE;
	}
		

	return FALSE;
}

BOOL CShellDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	if( IsSDIMode() )
		return FALSE;
	if( m_bLockDoc )
		return FALSE;
	
	return CDocument::CanCloseFrame(pFrame);
}

LPDISPATCH CShellDoc::GetActiveDocumentWindow() 
{
	// TODO: Add your dispatch handler code here

	CShellFrame	*pFrm = m_pActiveView->GetFrame();
	if(pFrm)
		return pFrm->GetIDispatch( true );
	else
		return NULL;
}

void CShellDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	// store the path fully qualified
	TCHAR szFullPath[_MAX_PATH];
	AfxFullPath(szFullPath, lpszPathName);
	m_strPathName = szFullPath;
	ASSERT(!m_strPathName.IsEmpty());       // must be set to something
	m_bEmbedded = FALSE;
	ASSERT_VALID(this);

	// set the document title based on path name
	if( strlen( szFullPath ) )
	{
		char	*pszTitle = ::strrchr( szFullPath, '\\' );
		if( !pszTitle )pszTitle = szFullPath;
		else pszTitle++;
		SetTitle( pszTitle );
	}

	// add it to the file MRU list
	if (bAddToMRU)
		AfxGetApp()->AddToRecentFileList(m_strPathName);

	ASSERT_VALID(this);
}

void CShellDoc::DeInitNotEmpty()
{
	if( m_bEmptyDoc )return;

	CString str_doc_name = m_strTitle;
	str_doc_name += ":DocTerminate";

	ExecuteObjectScript( GetControllingUnknown(), szDocTerminate, str_doc_name );
	m_bEmptyDoc = true;
}


BOOL CShellDoc::SaveModified()
{
	if( ::GetValueInt( GetAppUnknown(), "\\General", "DontCloseInScript", 0 ) != 0 )
	{
		bool bScriptRunning = false;
		IUnknown *punkAM = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
		sptrIActionManager	sptrAM(punkAM);
		if(punkAM) punkAM->Release();
		if (sptrAM != 0)
			bScriptRunning = sptrAM->IsScriptRunning() == S_OK;
		if(bScriptRunning) return false;
	}

	if( IsModified() )
	{
		IUnknownPtr punkFF;
		bool bNative = IsNativeFormat(punkFF);
		IFileDataObject4Ptr sptrFDO4(punkFF);
		if (!bNative && sptrFDO4 != 0)
		{
			BOOL bReadOnly = false;
			sptrFDO4->IsFileFormatReadonly(&bReadOnly);
			if (bReadOnly)
			{
				int r = AfxMessageBox(IDS_READONLY_FORMAT, MB_YESNO);
				if (r == IDYES)
				{
					SetModifiedFlag(FALSE);
					return TRUE;
				}
				else
					return FALSE;
			}
		}
	}

	CShellDocTemplate	*ptempl = GetDocTemplate();
	if( !ptempl->IsAutoSave() )
	{
		if( CDocument::SaveModified() )
		{
			SetModifiedFlag( false );
			return true;
		}
		return false;
	}

	return DoSave( m_strPathName );
}

bool IsNeedPrompt(LPCTSTR lpstrFileName, CDocument *pdoc, CString *pstrExt)
{
	sptrIDocument sptrDoc( pdoc->GetControllingUnknown() );
	if( sptrDoc == NULL )
		return false;
	if (pstrExt != NULL)
		pstrExt->Empty();

	CShellDocTemplate* pdocTemplate = (CShellDocTemplate*)pdoc->GetDocTemplate();
//get file filter by filename
	IFileFilter *punk  = 0;
	if( pdocTemplate->GetFileFilter( lpstrFileName, true, &punk ) == CDocTemplate::noAttempt )
	{
		if( punk )
		{
			punk->Release();
			punk = 0;
		}

		return false;
	}

	if( !punk ) //native format
	{
		return false;
	}

	punk->Release();
	punk = 0;


	IDataContext2Ptr	ptrContext( sptrDoc );
	
	long	lObjectsCount = 0;

	long lTypesCount = 0;
	ptrContext->GetObjectTypeCount( &lTypesCount );

	for( LPOS lPos=0; lPos<lTypesCount; lPos++ )
	{
		BSTR bstrType = 0;
		ptrContext->GetObjectTypeName( lPos, &bstrType );
		
		long lCount = 0;
		ptrContext->GetObjectCount( bstrType, &lCount );
		::SysFreeString( bstrType );

		lObjectsCount += lCount;

		if( lObjectsCount > 1 )
		{
			if( AfxMessageBox( IDS_CANT_SAVE_ALL_OBJECTS, MB_YESNO ) == IDYES )
			{

				CString	strFilter;
				pdocTemplate->GetDocString( strFilter, CDocTemplate::filterExt );
				::SetValue( GetAppUnknown(), "\\FileOpen", "LastExt", strFilter );
				if (pstrExt != NULL)
					*pstrExt = strFilter;
				return true;
			}
			else
				return false;
		}
	}

	return false;

}


BOOL CShellDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
	if (lpszPathName == NULL)
		return CDocument::DoSave(lpszPathName, bReplace);
	if (IsNeedPrompt(lpszPathName, this))
	{
		CShellDocManager	*pman = theApp.GetDocManager();
		CString s;
		if( !pman->DoPromptFileName(s, IDS_FILE_SAVE_DLG_TITLE, 0x80000000, FALSE, GetDocTemplate() ) )
			//user cancel
			return FALSE;
		if( s.IsEmpty() )
			//bad filename
			return FALSE;
		return CDocument::DoSave(s, bReplace);
	}
	return CDocument::DoSave(lpszPathName, bReplace);
}


BOOL CShellDoc::dispGetModifiedFlag() 
{
	BOOL bModified = FALSE;
	IFileDataObject2Ptr	sptrF( GetControllingUnknown() );
	if( sptrF )
		sptrF->IsDirty( &bModified );

	return bModified;
}

void CShellDoc::dispSetModifiedFlag(BOOL bModified) 
{
	IFileDataObject2Ptr	sptrF( GetControllingUnknown() );
	if( sptrF )
		sptrF->SetModifiedFlag( bModified );
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CShellDoc::GetInterfaceHook( const void* p )
{	
	return get_fast_interface( (const GUID*)p );
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CShellDoc::raw_get_interface( const GUID* piid )
{	
	IUnknown* punk = GetInterface( piid );
	if( !punk )
	{
		punk = QueryAggregates( piid );
		if( punk )
			punk->Release();
	}		
	return punk;
}

class _CKeepString
{
	CString *m_psKeep,m_sPrev;
public:
	_CKeepString(CString *psKeep)
	{
		m_psKeep = psKeep;
		m_sPrev = *m_psKeep;
	};
	~_CKeepString()
	{
		*m_psKeep = m_sPrev;
	}
};

void CShellDoc::OnFileSendMail()
{
	CString strExt;
	CDocTemplate* pTemplate = GetDocTemplate();
	if (pTemplate != NULL)
		pTemplate->GetDocString(strExt, CDocTemplate::filterExt);
	ASSERT(!strExt.IsEmpty());
	if (!m_strPathName.IsEmpty())
	{
		TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
		_tsplitpath(m_strPathName, szDrive, szDir, szFName, szExt);
		if (_tcscmp(szExt, strExt) != 0)
		{
			_CKeepString KeepString(&m_strPathName);
			_tmakepath(szPath, szDrive, szDir, szFName, strExt);
			m_strPathName = szPath;
			CDocument::OnFileSendMail();
		}
		else
			CDocument::OnFileSendMail();
	}
	else
	{
		CString s = m_strTitle;
		s += strExt;
		_CKeepString KeepString(&m_strTitle);
		m_strTitle = s;
		CDocument::OnFileSendMail();
	}
};

/*
/////////////////////////////////////////////////////////////////////////////
// MAPI implementation helpers and globals

AFX_STATIC_DATA BOOL _afxIsMailAvail = (BOOL)-1;    // start out not determined

/////////////////////////////////////////////////////////////////////////////
// _AFX_MAIL_STATE

class _AFX_MAIL_STATE : public CNoTrackObject
{
public:
	HINSTANCE m_hInstMail;      // handle to MAPI32.DLL
	virtual ~_AFX_MAIL_STATE();
};

_AFX_MAIL_STATE::~_AFX_MAIL_STATE()
{
	if (m_hInstMail != NULL)
		::FreeLibrary(m_hInstMail);
}

EXTERN_PROCESS_LOCAL(_AFX_MAIL_STATE, _afxMailState)

#define _countof(array) (sizeof(array)/sizeof(array[0]))
UINT AFXAPI AfxGetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);

void CShellDoc::OnFileSendMail1()
{
	ASSERT_VALID(this);
	ASSERT(_afxIsMailAvail);   // update handler always gets called first

	CWaitCursor wait;

	_AFX_MAIL_STATE* pMailState = _afxMailState;
	if (pMailState->m_hInstMail == NULL)
		pMailState->m_hInstMail = ::LoadLibraryA("MAPI32.DLL");

	if (pMailState->m_hInstMail == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
		return;
	}
	ASSERT(pMailState->m_hInstMail != NULL);

	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = GetProcAddress(pMailState->m_hInstMail, "MAPISendMail");
	if (lpfnSendMail == NULL)
	{
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
		return;
	}
	ASSERT(lpfnSendMail != NULL);

	TCHAR szTempName[_MAX_PATH];
	TCHAR szPath[_MAX_PATH];
	BOOL bRemoveTemp = FALSE;
	if (m_strPathName.IsEmpty() || IsModified())
	{
		// save to temporary path
		VERIFY(GetTempPath(_countof(szPath), szPath) != 0);
		VERIFY(GetTempFileName(szPath, _T("afx"), 0, szTempName) != 0);

		// save it, but remember original modified flag
		BOOL bModified = IsModified();
		BOOL bResult = DoSave(szTempName, FALSE);
		SetModifiedFlag(bModified);
		if (!bResult)
		{
			TRACE(traceAppMsg, 0, "Warning: file save failed during File.Send Mail.\n");
			return;
		}
		bRemoveTemp = TRUE;
	}
	else
	{
		// use actual file since it isn't modified
		lstrcpyn(szTempName, m_strPathName, _countof(szTempName));
	}
#ifdef _UNICODE
	char szTempNameA[_MAX_PATH];
	_wcstombsz(szTempNameA, szTempName, _countof(szTempNameA));
#endif

	// build an appropriate title for the attachment
	TCHAR szTitle[_MAX_PATH];
	if (!m_strPathName.IsEmpty())
		AfxGetFileName(m_strPathName, szTitle, _countof(szTitle));
	else
	{
		lstrcpyn(szTitle, m_strTitle, _countof(szTitle));
		if (::PathFindExtension(m_strTitle) == NULL) // no extension
		{
			// append the default suffix if there is one
			CString strExt;
			CDocTemplate* pTemplate = GetDocTemplate();
			if (pTemplate != NULL &&
				pTemplate->GetDocString(strExt, CDocTemplate::filterExt))
			{
				lstrcat(szTitle, strExt);
			}
		}
	}

#ifdef _UNICODE
	char szTitleA[_MAX_PATH];
	_wcstombsz(szTitleA, szTitle, _countof(szTitleA));
#endif

	// prepare the file description (for the attachment)
	MapiFileDesc fileDesc;
	memset(&fileDesc, 0, sizeof(fileDesc));
	fileDesc.nPosition = (ULONG)-1;
#ifdef _UNICODE
	fileDesc.lpszPathName = szTempNameA;
	fileDesc.lpszFileName = szTitleA;
#else
	fileDesc.lpszPathName = szTempName;
	fileDesc.lpszFileName = szTitle;
#endif

	// prepare the message (empty with 1 attachment)
	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.nFileCount = 1;
	message.lpFiles = &fileDesc;

	// prepare for modal dialog box
	AfxGetApp()->EnableModeless(FALSE);
	HWND hWndTop;
	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, &hWndTop);

	// some extra precautions are required to use MAPISendMail as it
	// tends to enable the parent window in between dialogs (after
	// the login dialog, but before the send note dialog).
	pParentWnd->SetCapture();
	::SetFocus(NULL);
	pParentWnd->m_nFlags |= WF_STAYDISABLED;

	int nError = lpfnSendMail(0, (ULONG_PTR)pParentWnd->GetSafeHwnd(),
		&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	// after returning from the MAPISendMail call, the window must
	// be re-enabled and focus returned to the frame to undo the workaround
	// done before the MAPI call.
	::ReleaseCapture();
	pParentWnd->m_nFlags &= ~WF_STAYDISABLED;

	pParentWnd->EnableWindow(TRUE);
	::SetActiveWindow(NULL);
	pParentWnd->SetActiveWindow();
	pParentWnd->SetFocus();
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	AfxGetApp()->EnableModeless(TRUE);

	if (nError != SUCCESS_SUCCESS &&
		nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
	}

	// remove temporary file, if temporary file was used
	if (bRemoveTemp)
		CFile::Remove(szTempName);
}

PROCESS_LOCAL(_AFX_MAIL_STATE, _afxMailState)
*/