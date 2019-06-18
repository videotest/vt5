#include "stdafx.h"
#include "shell.h"
#include "docs.h"

#include "ShellFrame.h"
#include "shellDoc.h"
#include "shellView.h"
#include "MainFrm.h"


#include "CommandManager.h"
#include "misc.h"
#include "scriptdocint.h"

#include "misc_str.h"
#include "utils.h"


IMPLEMENT_DYNAMIC(CShellDocManager, CDocManager);

BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
BOOL AFXAPI AfxResolveShortcut(CWnd* pWnd, LPCTSTR pszShortcutFile,
	LPTSTR pszPath, int cchPath);





/////////////////////////////////////////////////////////////////////////////
// CShellDocManager message handlers

/*BOOL IsGoodExt( CString &strExt )
{

	if( strExt.IsEmpty() )
		return true;


	BOOL bUseFilterTemplate = ::GetValueInt( GetAppUnknown(), "\\FileOpenDialog", "UseTemplate", 0 );
	
	if( bUseFilterTemplate )
	{
		CString strFilterTemplate = ::GetValueString( GetAppUnknown(), "\\FileOpenDialog", "FilterTemplate", "" );

		if( strFilterTemplate.IsEmpty() )
			return true;

		if( strExt.Find( "\n", 0 ) == -1 )
		{
			if( strFilterTemplate.Find( strExt.Right( strExt.GetLength() - 1 ), 0 ) == - 1 )
				return false;
		}
		else
		{
			CString ResultStr = "";
			int nSz = strExt.GetLength();
			int nBrID = -1;
			for(int i = 0; i < nSz;i++ )
			{
				if( strExt[i] == '\n' )
				{
					CString Res;
					
					if( nBrID >= 0 )
					{
						Res = strExt.Right( nBrID );
						Res = Res.Left( i - nBrID );
					}
					else
						Res = strExt.Left( i );
					
					nBrID = i;

					if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
					{
						if( !ResultStr.IsEmpty() )
							ResultStr += "\n" + Res;
						else
							ResultStr = Res;
					}
				}
			}

			{
				CString Res;
				
				if( nBrID >= 0 )
					Res = strExt.Right( strExt.GetLength() - nBrID - 1 );
				else
					Res = strExt.Left( i );
				
				nBrID = i;

				if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
				{
					if( !ResultStr.IsEmpty() )
						ResultStr += "\n" + Res;
					else
						ResultStr = Res;
				}
			}


			if( ResultStr.IsEmpty() )
				return false;

			strExt = ResultStr;

		}
	}

	return true;

}*/

CShellDocManager::CShellDocManager()
{
	m_bInitialized = false;
	m_pDispImpl = 0;
}


CShellDocManager::~CShellDocManager()
{
	ASSERT( m_bInitialized == false );
}

void CShellDocManager::Init()
{
	if( m_bInitialized )
		return;

	m_man.AttachComponentGroup( szPluginDocs );
	m_man.SetRootUnknown( theApp.GetControllingUnknown() );

	CCompRegistrator compReg;
	compReg.SetSectionName(szPluginDocs);

	for( int i = 0; i < compReg.GetRegistredComponentCount(); i++ )
	{
		CString strProgID;
		HRESULT hr;
		strProgID = compReg.GetComponentName(i);
		if (strProgID.IsEmpty())
			continue;

		CShellDocTemplate	*ptempl = new CShellDocTemplate(NULL);

		BSTR bstr;

		CLSID	clsid;
		bstr = strProgID.AllocSysString();

		hr = ::CLSIDFromProgID( bstr, &clsid );
		if (hr != S_OK)
		{
			delete ptempl;
			continue;
		}

		::SysFreeString( bstr );

		hr = CoCreateInstance(clsid,
        ptempl->GetControllingUnknown(), CLSCTX_INPROC_SERVER,
        IID_IUnknown, (LPVOID*)&ptempl->m_punkTemplate);
		if (hr != S_OK)
		{
			delete ptempl;
			continue;
		}
		//ptempl->m_spTemplate = ptempl->m_punkTemplate;

		if( CheckInterface( ptempl->m_punkTemplate, IID_IRootedObject ) )
		{
			IRootedObjectPtr	sptrR( ptempl->m_punkTemplate );
			sptrR->AttachParent( ::GetAppUnknown() );
		}
		

		m_man.AddComponent(ptempl->m_punkTemplate);

	
		ptempl->LoadTemplate();

		AddDocTemplate( ptempl );

		//punk->Release();
	}

	//m_man.Init();
	
	/*for( int i = 0; i < m_man.GetComponentCount(); i++ )
	{
		IUnknown *punk = m_man.GetComponent( i );
		CShellDocTemplate	*ptempl = new CShellDocTemplate( punk );
		
		ptempl->LoadTemplate();

		AddDocTemplate( ptempl );

		punk->Release();
	}*/

	m_bInitialized = true;

}
void CShellDocManager::DeInit()
{
	CloseAllDocuments( true );

	m_man.FreeComponents();
	m_man.DeInit();

	m_bInitialized = false;
}

//operation with dispatch implementation
CShellDocManagerDisp	*CShellDocManager::GetDispatchImpl() const
{
	return m_pDispImpl;
}

void CShellDocManager::SetDispatchImpl( CShellDocManagerDisp	* pimpl )
{
	m_pDispImpl = pimpl;
}

//size of DocumentTemplate list
long CShellDocManager::GetDocTemplateCount()
{
	return m_templateList.GetCount();
}

//return Document template by position
CShellDocTemplate *CShellDocManager::GetDocTemplateByPos( long nPos )
{
	POSITION	pos = GetFirstDocTemplatePosition();

	while( pos )
	{
		CDocTemplate	*pdocTemplate = GetNextDocTemplate( pos );

		if( !nPos-- )
			return dynamic_cast<CShellDocTemplate*>(pdocTemplate);
	}
	return 0;
}

void CShellDocManager::ReloadFiles()
{
//reload last document (or create empty)
	POSITION	pos = GetFirstDocTemplatePosition();

	while( pos )
	{
		CDocTemplate	*pdocTemplate = GetNextDocTemplate( pos );
		ASSERT_KINDOF(CShellDocTemplate, pdocTemplate );

		CShellDocTemplate	*p = (CShellDocTemplate	*)pdocTemplate;

		if( p->IsReloadAtStartup() )
		{
			CString	strFileName = p->GetLastDocumentFileName();

			if( strFileName.IsEmpty() )
			{
				CString	strParam = CString( "\"" )+p->GetTemplateName()+"\"";
				::ExecuteAction( "FileNew", strParam, aefNoShowInterfaceAnyway );
			}
			else
			{
				CString	strParam = CString( "\"" )+strFileName+"\", \""+p->GetTemplateName()+"\"";
				::ExecuteAction( "FileOpen", strParam, aefNoShowInterfaceAnyway );
			}
		}
	}
}

CShellDocTemplate *CShellDocManager::GetDocTemplate( const char *szTemplateName )
{
	POSITION	pos = GetFirstDocTemplatePosition();

	while( pos )
	{
		CDocTemplate	*pdocTemplate = GetNextDocTemplate( pos );
		ASSERT_KINDOF(CShellDocTemplate, pdocTemplate );

		CShellDocTemplate	*p = (CShellDocTemplate	*)pdocTemplate;

		if( p->GetTemplateName() == szTemplateName )
			return p;
	}

	return 0;
}

CShellDocTemplate *CShellDocManager::GetMatchDocTemplate( const char *lpszFileName )
{
	// find the highest confidence
	POSITION pos = m_templateList.GetHeadPosition();
	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocTemplate* pBestTemplate = NULL;
	CDocument* pOpenDocument = NULL;

	TCHAR szPath[_MAX_PATH];
	
	//ASSERT(lstrlen(lpszFileName) < _countof(szPath));


	TCHAR szTemp[_MAX_PATH];
	if (lpszFileName[0] == '\"')
		++lpszFileName;
	lstrcpyn(szTemp, lpszFileName, _MAX_PATH);
	LPTSTR lpszLast = _tcsrchr(szTemp, '\"');
	if (lpszLast != NULL)
		*lpszLast = 0;
	AfxFullPath(szPath, szTemp);
	TCHAR szLinkName[_MAX_PATH];
	if (AfxResolveShortcut(AfxGetMainWnd(), szPath, szLinkName, _MAX_PATH))
		lstrcpy(szPath, szLinkName);

	while (pos != NULL)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CDocTemplate::Confidence match;
		ASSERT(pOpenDocument == NULL);
		match = pTemplate->MatchDocType(szPath, pOpenDocument);
		if (match > bestMatch)
		{
			bestMatch = match;
			pBestTemplate = pTemplate;
		}
		if (match == CDocTemplate::yesAlreadyOpen)
			break;      // stop here
	}

	if( bestMatch == CDocTemplate::noAttempt )
		return 0;

	return (CShellDocTemplate*)pBestTemplate;
}

//!!!todo add support dockable frames
CDocument *CShellDocManager::GetActiveDocument()
{
	/*CWnd	*pwndFocus = CWnd::GetFocus();

	if( !pwndFocus )return 0;*/


	//************************************************
	// Old code here
	/*
	CWnd	*pwnd = AfxGetMainWnd();

	ASSERT_KINDOF(CMDIFrameWnd, pwnd);

	CMDIFrameWnd	*pMDIFrm = (CMDIFrameWnd*)pwnd;

	CFrameWnd	*pfrm = pMDIFrm->MDIGetActive();

	if( !pfrm )return 0;

	CDocument	*pdoc = pfrm->GetActiveDocument();

	return pdoc;
	*/
	// End old code
	//************************************************

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
	CWnd	*pwnd = AfxGetMainWnd( );

	ASSERT_KINDOF(CMainFrame, pwnd);

	CMainFrame	*pMDIFrm = (CMainFrame*)pwnd;

	CFrameWnd	*pfrm = pMDIFrm->_GetActiveFrame( );

	if( !pfrm )return 0;

	CDocument	*pdoc = pfrm->GetActiveDocument( );

	return pdoc;	
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

}


CDocument *CShellDocManager::GetDocumentByTitle( const char *szDocumentTitle )
{
	POSITION posTempl = GetFirstDocTemplatePosition();

	while( posTempl )
	{
		CDocTemplate *pdocTempl = GetNextDocTemplate( posTempl );

		POSITION	posDoc = pdocTempl->GetFirstDocPosition();

		while( posDoc )
		{
			CDocument *pdoc = pdocTempl->GetNextDoc( posDoc );

			ASSERT( pdoc );

			if( pdoc->GetTitle() == szDocumentTitle )
				return pdoc;
		}
	}
	return 0;
}

static UINT FindFilterIndex(LPCTSTR lpstrFilters, LPCTSTR lpstrExt)
{
	UINT n = 0;
	while (1)
	{
		LPCTSTR lpFltName = lpstrFilters;
		LPCTSTR lpFltExt = lpFltName+_tcslen(lpFltName)+1;
		if (*lpFltExt == 0) break;
		if (_tcscmp(lpFltExt+1,lpstrExt) == 0)
			return n;
		n++;
		lpstrFilters = lpFltExt+_tcslen(lpFltExt)+1;
		if (*lpstrFilters == 0) break;
	}
	return 0;
}

BOOL CShellDocManager::DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate, CString customFilter)
{
	_try(CShellDocManager, DoPromptFileName);
	{

	
	CWinApp* pApp = ::AfxGetApp();
	// get last path
	CString	strExt = pApp->GetProfileString("\\general", "LastOpenExt");


	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.Flags = lFlags;
	CString strFilter;
	CString strDefault;
	CString strE;
	
	if( customFilter.IsEmpty() )
	{
		if (pTemplate != NULL)
		{
			ASSERT_VALID(pTemplate);
			((CShellDocTemplate*)pTemplate)->AppendFilterSuffix( strFilter, &ofn, &strDefault );

			if( !fileName.IsEmpty() )
			{
				int i = fileName.Find( '.' );
				if( i != -1 )
				{
					strE = fileName.Right( fileName.GetLength() - i );
					::SetValue( ::GetAppUnknown(), "\\FileOpen", "LastExt", strE );
				}
				else
				{
					((CShellDocTemplate*)pTemplate)->GetDocString( strE, pTemplate->filterExt );
					::SetValue( ::GetAppUnknown(), "\\FileOpen", "LastExt", strE );
				}
			}
			else
			{
				((CShellDocTemplate*)pTemplate)->GetDocString( strE, pTemplate->filterExt );
				::SetValue( ::GetAppUnknown(), "\\FileOpen", "LastExt", strE );
			}



			if (strFilter.IsEmpty())
			{
				AfxMessageBox(IDS_DOC_NOT_SAVEABLE, MB_OK|MB_ICONEXCLAMATION);
				return FALSE;
			}
		}
		else
		{
			// do for all doc template
			POSITION pos = GetFirstDocTemplatePosition();


			BOOL bFirst = TRUE;
			while (pos != NULL)
			{
				CShellDocTemplate* pTemplate = (CShellDocTemplate*)GetNextDocTemplate( pos );
				if(bOpenFileDialog==TRUE)
				{
					CString	strExt;
					CString	strName;
					pTemplate->GetDocString( strName, pTemplate->filterName );
					pTemplate->GetDocString( strExt, pTemplate->filterExt );

					if(!strName.IsEmpty() && !strExt.IsEmpty())
					{
						if( IsGoodExt( strExt, bOpenFileDialog ) )
						{
							CString str;
							/*int nPos = strName.Find(" ", 0);
							if(nPos > 0)
							strName = strName.Left(nPos); */
							CString strFilterName = strName + " (";
							CString strFilterExt = '*' + strExt;
							strFilterName += '*' + strExt + ")" + '\0' + strFilterExt + '\0'; 
							str = strFilter;
							strFilter = strFilterName;
							strFilter += str;
						}
					}
				}

				((CShellDocTemplate*)pTemplate)->AppendFilterSuffix( strFilter, &ofn, bFirst ? &strDefault : NULL,  bOpenFileDialog==TRUE);
				bFirst = FALSE;
			}
		}
	}
	else
	{
		/*
		CString *strParts = 0;
		long lSz = 0;

		split( customFilter, "|", &strParts, &lSz );

		if( lSz % 2 )
		{
			for( int i = 0; i < lSz; i += 2 )
			{
			}
		}

		if( strParts )
			delete []strParts;
			*/

		strFilter = customFilter + '\0';
	}

	CString allFilter;
	CString title = ::LanguageLoadCString( nIDSTitle );
	//VERIFY(title.LoadString(nIDSTitle));
	if(bOpenFileDialog == TRUE)
	{
		allFilter = ::LanguageLoadCString(IDS_ALLFILTER);
		//VERIFY(allFilter.LoadString(IDS_ALLFILTER));		
		if( ::GetValueInt( ::GetAppUnknown(), "\\FileOpen", "UseShowAllFilter", 1 ) != 0 )
		{
			strFilter += allFilter + (TCHAR)'\0' + _T("*.*") + (TCHAR)'\0';
			ofn.nMaxCustFilter++;
		}
	}

	ofn.lpstrFilter = strFilter;
	ofn.lpstrTitle = title;
	ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
	ofn.nMaxFile = _MAX_PATH;
	ofn.nFilterIndex = FindFilterIndex(ofn.lpstrFilter, strE);
	
	BOOL res;

	res = ExecuteFileDialog(bOpenFileDialog, ofn, 0 );

	pApp->WriteProfileString("\\general", "LastOpenExt", strExt);

	fileName.ReleaseBuffer();

	return res;
	
	/*
	CString	strPath = ::AfxGetApp()->GetProfileString( "general", "LastOpenPath" );
	if( !strPath.IsEmpty() ) 
		::SetCurrentDirectory( strPath );
	  
	CFileDialog dlgFile(bOpenFileDialog, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );

	CString title;
	VERIFY(title.LoadString(nIDSTitle));::LanguageLoadCString

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;
	if (pTemplate != NULL)
	{
		ASSERT_VALID(pTemplate);
		((CShellDocTemplate*)pTemplate)->AppendFilterSuffix( strFilter, &dlgFile.m_ofn, &strDefault );
	}
	else
	{
		// do for all doc template
		POSITION pos = GetFirstDocTemplatePosition();

		BOOL bFirst = TRUE;
		while (pos != NULL)
		{
			CShellDocTemplate* pTemplate = (CShellDocTemplate*)GetNextDocTemplate( pos );
			((CShellDocTemplate*)pTemplate)->AppendFilterSuffix( strFilter, &dlgFile.m_ofn, bFirst ? &strDefault : NULL );
		}
	}

	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));::LanguageLoadCString
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);


	int nResult = dlgFile.DoModal();

	fileName.ReleaseBuffer();

	char	szPath[_MAX_PATH];
	::GetCurrentDirectory( _MAX_PATH, szPath );

	::AfxGetApp()->WriteProfileString( "general", "LastOpenPath", szPath );

	return nResult == IDOK;*/
	}
	_catch;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DYNAMIC(CShellDocTemplate, CMultiDocTemplate);

CShellDocTemplate::CShellDocTemplate( IUnknown *punk )
	:	CMultiDocTemplate( IDR_SHELLTYPE, RUNTIME_CLASS(CShellDoc), 
			RUNTIME_CLASS(CShellFrame), 
			RUNTIME_CLASS(CShellView) )
{
	//m_spTemplate = NULL;
	m_punkTemplate = NULL;
	m_bLoaded = false;
	m_nIDResource = 0;
	m_dwFlags = 0;

	m_pActiveDocument = 0;
	m_pOnlyDoc = 0;

	EnableAggregation();
}

BEGIN_INTERFACE_MAP(CShellDocTemplate, CMultiDocTemplate)
	INTERFACE_PART(CShellDocTemplate, IID_IShellDocTemplate, ShellTempl)
	INTERFACE_AGGREGATE(CShellDocTemplate, m_punkTemplate)
END_INTERFACE_MAP()

// {C48CE9B5-F947-11D2-A596-0000B493FF1B}
static const CLSID clsid =
{ 0xc48ce9b5, 0xf947, 0x11d2, { 0xa5, 0x96, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

CDocument* CShellDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible )
{
	CDocument* pDocument = NULL;
	CFrameWnd* pFrame = NULL;

	if( IsSingleDocument() )
	{
		BOOL bCreated = FALSE;      // => doc and frame created
		BOOL bWasModified = FALSE;

		if (m_pOnlyDoc != NULL)
		{
			POSITION	pos = m_pOnlyDoc->GetFirstViewPosition();
			CView	*pview =  m_pOnlyDoc->GetNextView( pos );

			pFrame = pview->GetParentFrame();

			// already have a document - reinit it
			pDocument = m_pOnlyDoc;

			if (!pDocument->SaveModified())
				return NULL;        // leave the original one

			// vanek 15.10.2003 - fire event OnCloseDocument 
			{
				CShellDocManager *pman = theApp.GetDocManager();

				VARIANT varRes;
				varRes.vt = VT_BOOL;
				varRes.boolVal = true;

				if( !pman )
					return NULL;
				pman->GetDispatchImpl()->FireOnCloseDocument( pDocument->GetIDispatch( false ), &varRes );

				if( !varRes.boolVal )
					return NULL;
			}  


			pDocument->DeleteContents();
		}
		else
		{
			// create a new document
			pDocument = CreateNewDocument();
			ASSERT(pFrame == NULL);     // will be created below
			bCreated = TRUE;
		}

		if (pDocument == NULL)
		{
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			return NULL;
		}
		ASSERT(pDocument == m_pOnlyDoc);

		/* [vanek] : перенесено на момент, когда документ уже открыт, его named data подгружена -> информация 
		о вьюхе тоже - 23.07.2004

		if (pFrame == NULL)
		{
			ASSERT(bCreated);

			// create frame - set as main document frame
			BOOL bAutoDelete = pDocument->m_bAutoDelete;
			pDocument->m_bAutoDelete = FALSE;

			pFrame = CreateNewFrame(pDocument, NULL);
						// don't destroy if something goes wrong
			pDocument->m_bAutoDelete = bAutoDelete;
			if (pFrame == NULL)
			{
				AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
				delete pDocument;       // explicit delete on error
				return NULL;
			}
		}  */

		if (lpszPathName == NULL)
		{
			// create a new document
			SetDefaultTitle(pDocument);

			// avoid creating temporary compound file when starting up invisible
			if (!bMakeVisible)
				pDocument->m_bEmbedded = TRUE;

			if (!pDocument->OnNewDocument())
			{
				// user has been alerted to what failed in OnNewDocument
				TRACE0("CDocument::OnNewDocument returned FALSE.\n");
				//if (bCreated). paul 8.01.2003: if SDI -> reuse old frame, need kill frame anywhere
				if( pFrame )
					pFrame->DestroyWindow();    // will destroy document
				else
				{
					pDocument->OnCloseDocument();
					pDocument=0;
				}

				return NULL;
			}
		}
		else
		{
			CWaitCursor wait;

			// open an existing document
			bWasModified = pDocument->IsModified();
			pDocument->SetModifiedFlag(FALSE);  // not dirty for open

			if (!pDocument->OnOpenDocument(lpszPathName))
			{
				// user has been alerted to what failed in OnOpenDocument
				TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
				if (bCreated)
				{
					try
					{
						if( pFrame )
							pFrame->DestroyWindow();    // will destroy document
						else
						{
							pDocument->OnCloseDocument();
							pDocument=0;
						}
					}
					catch( ... )
					{
					}
				}
				else if (!pDocument->IsModified())
				{
					// original document is untouched
					pDocument->SetModifiedFlag(bWasModified);
				}
				else
				{
					// we corrupted the original document
					SetDefaultTitle(pDocument);

					if (!pDocument->OnNewDocument())
					{
						TRACE0("Error: OnNewDocument failed after trying to open a document - trying to continue.\n");
						// assume we can continue
					}
				}
				return NULL;        // open failed
			}
			pDocument->SetPathName(lpszPathName);
		}

		// [vanek] - 23.07.2004
		if (pFrame == NULL)
		{
			ASSERT(bCreated);

			// create frame - set as main document frame
			BOOL bAutoDelete = pDocument->m_bAutoDelete;
			pDocument->m_bAutoDelete = FALSE;

			pFrame = CreateNewFrame(pDocument, NULL);
						// don't destroy if something goes wrong
			pDocument->m_bAutoDelete = bAutoDelete;
			if (pFrame == NULL)
			{
				AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
				delete pDocument;       // explicit delete on error
				return NULL;
			}
		}  

		CWinThread* pThread = AfxGetThread();
		if (bCreated && pThread->m_pMainWnd == NULL)
		{
			// set as main frame (InitialUpdateFrame will show the window)
			pThread->m_pMainWnd = pFrame;
		}
		InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
	}
	else
	{
		if( lpszPathName )
		{
			CDocument	*pdoc = 0;
			Confidence	conf = MatchDocType( lpszPathName, pdoc );

			if(  conf == yesAlreadyOpen )
			{
				POSITION	pos = pdoc->GetFirstViewPosition();
				CView	*pV = pdoc->GetNextView( pos );

				CFrameWnd	*pF = pV->GetParentFrame();

				CMDIChildWnd	*pMDI = dynamic_cast<CMDIChildWnd*>( pF );

				if( pMDI )
					pMDI->MDIActivate();

				return pdoc;
			}
		}

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		if (pMainFrame && pMainFrame->IsSDIMode())
			return NULL; //SBT1744, active document will be set to NULL in destructor

		
		pDocument = CreateNewDocument();
		if (pDocument == NULL)
		{
			TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			return NULL;
		}
		ASSERT_VALID(pDocument);

	
		if (lpszPathName == NULL)
		{
			// create a new document - with default document name
			SetDefaultTitle(pDocument);

				// avoid creating temporary compound file when starting up invisible
			if (!bMakeVisible)
				pDocument->m_bEmbedded = TRUE;

			if (!pDocument->OnNewDocument())
			{
				delete pDocument;
				// user has be alerted to what failed in OnNewDocument
				TRACE0("CDocument::OnNewDocument returned FALSE.\n");
				return NULL;
			}

				// it worked, now bump untitled count
			m_nUntitledCount++;
		}
		else
		{
			// open an existing document
			CWaitCursor wait;
			if (!pDocument->OnOpenDocument(lpszPathName))
			{
				// user has be alerted to what failed in OnOpenDocument
				TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
				//AfxMessageBox(AFX_IDS_FAILED_TO_CREATE_DOC);
				delete pDocument;//paul 20.09.2002, if fail, this doc include in DocTemplate!!!
				return NULL;
			}
	#ifdef _MAC
			// if the document is dirty, we must have opened a stationery pad
			//  - don't change the pathname because we want to treat the document
			//  as untitled
			if (!pDocument->IsModified())
	#endif
				pDocument->SetPathName(lpszPathName);
			}

		BOOL bAutoDelete = pDocument->m_bAutoDelete;
		pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
		CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
		ASSERT_VALID(pFrame);
		pDocument->m_bAutoDelete = bAutoDelete;

		if (pFrame == NULL)
		{
			AfxMessageBox(IDS_FAILED_TO_OPEN_DOC);
			delete pDocument;       // explicit delete on error
			return NULL;
		}

		InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
	}

	if( !pDocument )
		return 0;

	return pDocument;
}

void CShellDocTemplate::AddDocument(CDocument* pDoc)
{
	if( IsSingleDocument() )
	{
		ASSERT(m_pOnlyDoc == NULL);     // one at a time please
		ASSERT_VALID(pDoc);

		CDocTemplate::AddDocument(pDoc);
		m_pOnlyDoc = pDoc;
	}
	else
		CMultiDocTemplate::AddDocument( pDoc );
}

void CShellDocTemplate::RemoveDocument(CDocument* pDoc)
{
	if( IsSingleDocument() )
	{
		ASSERT(m_pOnlyDoc == pDoc);     // must be this one
		ASSERT_VALID(pDoc);

		CDocTemplate::RemoveDocument(pDoc);
		m_pOnlyDoc = NULL;
	}
	else
		CMultiDocTemplate::RemoveDocument( pDoc );
}

POSITION CShellDocTemplate::GetFirstDocPosition() const
{
	if( IsSingleDocument() )
	{
		return (m_pOnlyDoc == NULL) ? NULL : BEFORE_START_POSITION;
	}

	return CMultiDocTemplate::GetFirstDocPosition();
}

CDocument* CShellDocTemplate::GetNextDoc(POSITION& rPos) const
{	
	if( IsSingleDocument() )
	{
		CDocument* pDoc = NULL;
		if (rPos == BEFORE_START_POSITION)
		{
			// first time through, return a real document
			ASSERT(m_pOnlyDoc != NULL);
			pDoc = m_pOnlyDoc;
		}
		rPos = NULL;        // no more
		return pDoc;
	}
	else
		return CMultiDocTemplate::GetNextDoc( rPos );
}	

void CShellDocTemplate::SetDefaultTitle(CDocument* pDocument)
{
	CString strDocName;
	if (GetDocString(strDocName, CDocTemplate::fileNewName) &&
		!strDocName.IsEmpty())
	{
		TCHAR szNum[8];
		wsprintf(szNum, _T("%d"), m_nUntitledCount+1);
		strDocName += szNum;
	}
	else
	{
		// use generic 'untitled' - ignore untitled count
		strDocName = ::LanguageLoadCString(AFX_IDS_UNTITLED);
		//VERIFY(strDocName.LoadString(AFX_IDS_UNTITLED));
	}
	pDocument->SetTitle(strDocName);
}


void CShellDocTemplate::LoadTemplate()
{
	if( m_bLoaded )
		return;
	m_bLoaded = true;

	BSTR	bstr = 0;
	sptrIDocTemplate sptrIDocTempl(m_punkTemplate);
	sptrIDocTempl->GetDocTemplString( -1, &bstr );
	
	m_strDocStrings = bstr;

	::SysFreeString( bstr );
//!!todo: should be replaced later by registered menus
//!!todo: accel table
//	m_hMenuShared = g_CmdManager.GetMenu();
//nothing intrested in base class
//	CMultiDocTemplate::LoadTemplate();
	m_hAccelTable = g_CmdManager.GetAcceleratorTable();

	sptrIDocTempl->GetTemplateFlags( &m_dwFlags );

	CString	strName;
	GetDocString( strName, CDocTemplate::docName );

	CString strSection = "\\" + strName;
	m_dwFlags = ::GetValueInt( GetAppUnknown(), strSection, "Flags", m_dwFlags );
	m_strLastDocumentFileName = ::GetValueString(  GetAppUnknown(), strName, "LastFile", m_strLastDocumentFileName );

	m_server.ConnectTemplate( clsid, this, FALSE );
	m_server.UpdateRegistry( OAT_DISPATCH_OBJECT );
//get last document filename
	sptrIDocTempl = 0;

	m_manFilters.AttachComponentGroup( strName+"\\"+szPluginExportImport );
	m_manFilters.SetRootUnknown( m_punkTemplate );
	m_manFilters.Init();
}

void CShellDocTemplate::SaveState()
{
	if( m_punkTemplate )
	{
		CString	strName;
		GetDocString( strName, CDocTemplate::docName );

		if( !m_strLastDocumentFileName.IsEmpty() )
			::SetValue(  GetAppUnknown(), strName, "LastFile", m_strLastDocumentFileName );
	}
}

CShellDocTemplate::~CShellDocTemplate()
{
	POSITION	pos = GetFirstDocPosition();

	while( pos )
	{
		CShellDoc	*pdoc = (CShellDoc*)GetNextDoc( pos );
		pdoc->ResetTemplate();
	}	
	ASSERT( GetFirstDocPosition() == 0 );
	if( m_punkTemplate )
		m_punkTemplate->Release();
}

//document initialization added. DocumentManager calls CShellDoc::Init
CDocument* CShellDocTemplate::CreateNewDocument()
{
	CDocument* pdoc = CMultiDocTemplate::CreateNewDocument();

	ASSERT_KINDOF(CShellDoc, pdoc);
	
	((CShellDoc*)pdoc)->Init();

	return pdoc;
}

//create a new frame. New frame is created without resource string 
//(but base class can't create it)
CFrameWnd* CShellDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
//	return CMultiDocTemplate::CreateNewFrame( pDoc, pOther );

	if (pDoc != NULL)
		ASSERT_VALID(pDoc);
	// create a frame wired to the specified document

	CCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = m_pViewClass;
	context.m_pNewDocTemplate = this;

	if (m_pFrameClass == NULL)
	{
		TRACE0("Error: you must override CDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);
		return NULL;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if (pFrame == NULL)
	{
		TRACE1("Warning: Dynamic create of frame %hs failed.\n",
			m_pFrameClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

	if (context.m_pNewViewClass == NULL)
		TRACE0("Warning: creating frame with no default view.\n");

	// create new from resource
	if (!pFrame->LoadFrame(m_nIDResource,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
			NULL, &context))
	{
		TRACE0("Warning: CDocTemplate couldn't create a frame.\n");
		// frame will be deleted in PostNcDestroy cleanup
		return NULL;
	}

	// it worked !
	return pFrame;
}

void CShellDocTemplate::SetLastDocumentFileName( const char *sz )
{
	CString	strName;
	GetDocString( strName, CDocTemplate::docName );

	if( sz && strlen( sz ) ) 
	{
		m_strLastDocumentFileName = sz;
		::SetValue(  GetAppUnknown(), strName, "LastFile", sz );
	}
}
const char *CShellDocTemplate::GetLastDocumentFileName()
{
	CString	strName;
	GetDocString( strName, CDocTemplate::docName );

	m_strLastDocumentFileName = ::GetValueString(  GetAppUnknown(), strName, "LastFile" );
	return m_strLastDocumentFileName;
}

CString	CShellDocTemplate::GetTemplateName()
{
	CString	strTemplate;
	GetDocString( strTemplate, CDocTemplate::docName );

	return strTemplate;
}

//difference from base class is using filter array for computing confidence
CShellDocTemplate::Confidence 
	CShellDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
{
	Confidence	conf, confTest = noAttempt;
	_bstr_t		bstrFileName = lpszPathName;
	IFileFilter *pIFilter = 0;

	conf = CDocTemplate::MatchDocType( lpszPathName, rpDocMatch );

	if( conf ==  yesAttemptForeign )
		conf =  noAttempt;


	for( int i = 0; i  <m_manFilters.GetComponentCount(); i++ )
	{
		sptrIFileFilter		spFilter( m_manFilters.GetComponent( i, false ) );

		spFilter->MatchFileType( bstrFileName, (DWORD *)&confTest );

		if( confTest > conf )
			conf= max( conf, confTest );
	}
	return conf;
}

//return the FileFilter component pointer, best matches with given file
//if the file is a native format file, return 0
CDocTemplate::Confidence CShellDocTemplate::GetFileFilter( const char *pszFileName, bool bExcludeAlreadyOpen, IFileFilter **ppf )
{
	Confidence	conf = noAttempt, confTest = noAttempt;
	_bstr_t		bstrFileName = pszFileName;
	IFileFilter *pIFilter = 0;

	CDocument *pdocDummy = 0;

	if( bExcludeAlreadyOpen  )
	{
			// see if it matches our default suffix
		CString strFilterExt;
		if (GetDocString(strFilterExt, CDocTemplate::filterExt) &&
		  !strFilterExt.IsEmpty())
		{
			// see if extension matches
			ASSERT(strFilterExt[0] == '.');
			LPCTSTR lpszDot = _tcsrchr(pszFileName, '.');
			if (lpszDot != NULL && lstrcmpi(lpszDot, strFilterExt) == 0)
				conf = yesAttemptNative; // extension matches, looks like ours

		}
	}
	else
		conf = CDocTemplate::MatchDocType( pszFileName, pdocDummy );

	bool	bNativeFormat = true;

	for( int i = 0; i  <m_manFilters.GetComponentCount(); i++ )
	{
		IUnknown *punk = m_manFilters.GetComponent( i );
		sptrIFileFilter		spFilter( punk );

		punk->Release();

		spFilter->MatchFileType( bstrFileName, (DWORD *)&confTest );

		if( confTest > conf )
		{
			bNativeFormat = false;
			if( pIFilter )
				pIFilter->Release();

			pIFilter = spFilter.GetInterfacePtr();
			pIFilter->AddRef();
			conf = confTest;
		}

	}	

	if( ppf )
		*ppf = 0;

	if( bNativeFormat )
		return maybeAttemptNative;

	if( ppf )
		*ppf = pIFilter;
	else
		pIFilter->Release();

	return conf;
}

//append file suffix helper
void _AppendFileSuffix( CString &strFilter, const char *szName, CString strExt )
{
	CString strExts;
	strFilter += szName;
	strFilter += " (";
	int	idx = 0;
	CString	strAll = strExt;
	while( idx != -1 )
	{
		idx = strAll.Find( "\n" );
		CString	strExt;
		if( idx == -1 )	 
			strExt = strAll;
		else
		{
			strExt = strAll.Left( idx );
			strAll = strAll.Right( strAll.GetLength()-1-idx );
		}
		strExts += '*' + strExt + ';';
	}
	strExts.Delete(strExts.GetLength()-1);	
	strFilter += strExts;
	strFilter += ")";
	strFilter += '\0';
	strFilter += strExts;
	strFilter += '\0';

}
//append all avaible extension for given document template, including file filters
void CShellDocTemplate::AppendFilterSuffix( CString &strFilter, OPENFILENAME *pofn, CString *pstrDefault, bool bOpenFile)
{
	CString	strExt;
	CString	strName;
	int i;

	GetDocString( strName, filterName );
	GetDocString( strExt, filterExt );

	//_AppendFileSuffix( strFilter, strName, strExt );

	if(bOpenFile)
		if( !strName.IsEmpty() && !strExt.IsEmpty())
		{
			//make common filter fo template
			CString strFilterName;
			CString strFilterExt;

			int nPos = strName.Find("Document");
			strName.MakeLower();
			if(nPos >= 0)
				strName = strName.Left(nPos-1);

			strFilterName = MakeAllFilesStringForDocument(strName);
//			strFilterName.Format( IDS_ALLFILTER_NAME_FORMAT, (const char *)strName );

			if( IsGoodExt( strExt, bOpenFile ) )
				strFilterExt = '*' + strExt + ';';

			for( i = 0; i  <m_manFilters.GetComponentCount(); i++ )
			{
				IUnknown *punk = m_manFilters.GetComponent( i );
				sptrIFileFilter		spFilter( punk );
		
				punk->Release();
		
				BSTR	bstrFilter, bstrExt;
		
				spFilter->GetFilterParams( &bstrExt, &bstrFilter, 0 );
		
				strExt = bstrExt;
				strName = bstrFilter;

				::SysFreeString( bstrExt );
				::SysFreeString( bstrFilter );

				int	idx = 0;
				CString	strAll = strExt;
				while( idx != -1 )
				{
					idx = strAll.Find( "\n" );
					CString	strExt;
					if( idx == -1 )	 
						strExt = strAll;
					else
					{
						strExt = strAll.Left( idx );
						strAll = strAll.Right( strAll.GetLength()-1-idx );
					}

						if( IsGoodExt( strExt, bOpenFile ) )
							strFilterExt += '*' + strExt + ';';
				}
			}
		
			if( strFilterExt.GetLength() )
				strFilterExt.Delete(strFilterExt.GetLength()-1);	

			if( strFilterExt.Find(";") != -1 )
			{
				strFilterName += strFilterExt + ")" + '\0' + strFilterExt + '\0'; 
				
				//strFilter.Insert(0, strFilterName); don't work correctly 

				if( !strFilterExt.IsEmpty() )
				{
					CString str = strFilter;
					strFilter = strFilterName;
					strFilter += str;

			
					if( pofn )
						pofn->nMaxCustFilter++;
				}
			}
		}

	if( pstrDefault )
		*pstrDefault = strExt;


	if(!bOpenFile)
	{
		GetDocString( strName, filterName );
		GetDocString( strExt, filterExt );
		if( !strName.IsEmpty() && !strExt.IsEmpty() && IsGoodExt(strExt,bOpenFile) )
			_AppendFileSuffix( strFilter, strName, strExt );
	}

	for(i = 0; i  <m_manFilters.GetComponentCount(); i++ )
	{
		IUnknown *punk = m_manFilters.GetComponent( i );
		sptrIFileFilter		spFilter( punk );

		punk->Release();

		BSTR	bstrFilter, bstrExt;

		spFilter->GetFilterParams( &bstrExt, &bstrFilter, 0 );

		strExt = bstrExt;
		strName = bstrFilter;

		::SysFreeString( bstrExt );
		::SysFreeString( bstrFilter );

		if(!bOpenFile)
		{
			int idx = strExt.Find("\n");
			if(idx >= 0)
				strExt = strExt.Left(idx);
			sptrIFileFilter3 spFilter3(spFilter);
			if (spFilter3 != 0)
			{
				COleVariant var;
				HRESULT hr = spFilter3->GetPropertyByName(_bstr_t("OnlyImport"), &var);
				if (SUCCEEDED(hr) && var.vt == VT_BOOL)
				{
					if (var.boolVal)
						continue;
				}
			}
		}

		if( IsGoodExt(strExt,bOpenFile) )
		{
			_AppendFileSuffix( strFilter, strName, strExt );

			if( pofn )
				pofn->nMaxCustFilter++;
		}
	}
}


//return count of documents
long CShellDocTemplate::GetDocumentCount()
{
	return m_docList.GetCount();
}

//return document by no
CDocument	*CShellDocTemplate::GetDocumentByPos( long nPos )
{
	POSITION pos = GetFirstDocPosition();

	while( pos )
	{
		CDocument	*pDoc = GetNextDoc( pos );

		if( !nPos-- )
			return pDoc;
	}

	return 0;
}

//return last active document of this template
CDocument	*CShellDocTemplate::GetActiveDocument()
{
	return m_pActiveDocument;
}

//setup the active document
void CShellDocTemplate::SetActiveDocument( CDocument *pActivateDoc, bool bActivateWindow )
{
	m_pActiveDocument = pActivateDoc;

	if( bActivateWindow && m_pActiveDocument )
	{
		ASSERT_KINDOF(CShellDoc, m_pActiveDocument);

		CShellDoc	*pShellDoc = (CShellDoc	*)m_pActiveDocument;
		POSITION	pos = pShellDoc->GetFirstFramePosition();

		if( !pos )
		{
			pos = m_pActiveDocument->GetFirstViewPosition();

			ASSERT( pos );

			CView	*pView = m_pActiveDocument->GetNextView( pos );

			ASSERT( pView );

			if( pView )
				pView->SetFocus();
		}
		else
		{
			CShellFrame	*pframe = pShellDoc->GetNextFrame( pos );
			if( pframe )
			{
				pframe->MDIActivate();
				pframe->SendMessage( WM_NCACTIVATE, WPARAM( bActivateWindow ) );

			}
		}
	}
}

// check this template contains filefilter with requested name
bool CShellDocTemplate::ContainsFilter(LPCTSTR szFilterName)
{
	CString strFilterName(szFilterName);
	
	if (strFilterName.IsEmpty())
		return false;

	for (int i = 0; i < m_manFilters.GetComponentCount(); i++)
	{
		BSTR		bstrFilter = 0;
		BSTR		bstrExt = 0;
		IUnknown *	punk = m_manFilters.GetComponent(i);

		sptrIFileFilter	spFilter(punk);
		punk->Release();

		spFilter->GetFilterParams(&bstrExt, &bstrFilter, 0);

		CString strExt = bstrExt;
		CString strName = bstrFilter;

		::SysFreeString(bstrExt);
		::SysFreeString(bstrFilter);

		if (strName == strFilterName)
			return true;
	}
	return false;
}

IMPLEMENT_UNKNOWN(CShellDocTemplate, ShellTempl);

HRESULT CShellDocTemplate::XShellTempl::GetFileFilterByFile(BSTR bstr, IUnknown **ppunk)
{
	_try_nested(CShellDocTemplate, ShellTempl, GetFileFilterByFile)
	{
		CString strName(bstr);
		IFileFilter	*pfilter;
		pThis->GetFileFilter(strName, true, &pfilter );
		*ppunk = pfilter;
		return S_OK;
	}
	_catch_nested;
}




BEGIN_MESSAGE_MAP(CShellDocManagerDisp, CCmdTargetEx)
	//{{AFX_MSG_MAP(CShellDocManagerDisp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {A23423C4-69D5-11d3-A64D-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CShellDocManagerDisp, "Shell.ShellDocManagerDisp", 
0xa23423c4, 0x69d5, 0x11d3, 0xa6, 0x4d, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

IMPLEMENT_DYNCREATE(CShellDocManagerDisp, CCmdTargetEx)


BEGIN_DISPATCH_MAP(CShellDocManagerDisp, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CShellDocManagerDisp)
	DISP_PROPERTY_EX(CShellDocManagerDisp, "ActiveDocument", GetActiveDocumentDisp, SetActiveDocumentDisp, VT_DISPATCH)
	DISP_FUNCTION(CShellDocManagerDisp, "GetActiveDocument", GetActiveDocumentByType, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CShellDocManagerDisp, "GetDocTypesCount", GetDocTypesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CShellDocManagerDisp, "GetDocTypeName", GetDocTypeName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CShellDocManagerDisp, "GetDocument", GetDocument, VT_DISPATCH, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CShellDocManagerDisp, "GetDocumentsCount", GetDocumentsCount, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CShellDocManagerDisp, "GetActiveObject", GetActiveObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CShellDocManagerDisp, "GenerateGUID", GenerateGUID, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CShellDocManagerDisp, "IsPointerNull", IsPointerNull, VT_BOOL, VTS_VARIANT)
	DISP_FUNCTION(CShellDocManagerDisp, "IdleUpdateActions", IdleUpdateActions, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CShellDocManagerDisp, "LoadString", LoadString, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CShellDocManagerDisp, "LoadStringEx", LoadStringEx, VT_BSTR, VTS_BSTR VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IShellDocManagerDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {05E1F7E9-38E6-11D3-A602-0090275995FE}
static const IID IID_IShellDocManagerDisp =
{ 0x5e1f7e9, 0x38e6, 0x11d3, { 0xa6, 0x2, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

// {A23423C2-69D5-11d3-A64D-0090275995FE}
static const IID IID_IShellDocManagerEvents = 
{ 0xa23423c2, 0x69d5, 0x11d3, { 0xa6, 0x4d, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };


IMPLEMENT_OLETYPELIB(CShellDocManagerDisp, IID_ITypeLibID, 1, 0)

BEGIN_INTERFACE_MAP(CShellDocManagerDisp, CCmdTargetEx)
	INTERFACE_PART(CShellDocManagerDisp, IID_IShellDocManagerDisp, Dispatch)
	INTERFACE_PART(CShellDocManagerDisp, IID_INamedObject2, Name)
	INTERFACE_PART(CShellDocManagerDisp, IID_IConnectionPointContainer, ConnPtContainer)
	INTERFACE_PART(CShellDocManagerDisp, IID_IProvideClassInfo, ProvideClassInfo)
END_INTERFACE_MAP()

BEGIN_EVENT_MAP(CShellDocManagerDisp, COleEventSourceImpl)
	//{{AFX_EVENT_MAP(CShellDocManagerDisp)
	EVENT_CUSTOM("OnOpenDocument", FireOnOpenDocument, VTS_BSTR  VTS_DISPATCH)
	EVENT_CUSTOM("OnSaveDocument", FireOnSaveDocument, VTS_BSTR  VTS_DISPATCH)
	EVENT_CUSTOM("OnCloseDocument", FireOnCloseDocument, VTS_DISPATCH VTS_PVARIANT )
	EVENT_CUSTOM("OnNewDocument", FireOnNewDocument, VTS_DISPATCH)
	EVENT_CUSTOM("OnActivateDocument", FireOnActivateDocument, VTS_DISPATCH)
	EVENT_CUSTOM("OnActivateView", FireOnActivateView, VTS_DISPATCH VTS_I4 VTS_I4 VTS_BOOL )
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


				
CShellDocManagerDisp::CShellDocManagerDisp()
{
	EnableAutomation();
	EnableConnections();

	m_pDocManager = theApp.GetDocManager();
	m_pDocManager->SetDispatchImpl( this );

	m_sName = _T("Docs");

	m_piidEvents = &IID_IShellDocManagerEvents;
	m_piidPrimary = &IID_IShellDocManagerDisp;

	GetModuleFileName( 0, sz_string_file, sizeof( sz_string_file ) );
	strcpy( strrchr( sz_string_file, '\\' )+1, "resource." );

	CString	strLang = GetValueString( GetAppUnknown(), "\\General", "Language", "en" );
	strcat( sz_string_file, strLang );

}


LPDISPATCH CShellDocManagerDisp::GetActiveDocumentDisp() 
{
	CDocument	*pdoc = m_pDocManager->GetActiveDocument();

	if( pdoc )
	{
		//CString str = "...CShellDocManagerDisp::GetActiveDocumentDisp = ";
		//str += pdoc->GetTitle();
		//str += "\r\n";
		//_trace_file_text("trace_aam", str);
		return pdoc->GetIDispatch( true );
	}
	return NULL;
}

CDocument *CShellDocManagerDisp::GetActiveDocument()
{
	return m_pDocManager->GetActiveDocument();
}

void CShellDocManagerDisp::SetActiveDocumentDisp(LPDISPATCH newValue) 
{
	CDocument	*pdocToActivate = 0;

	GuidKey lKey = ::GetObjectKey( newValue );

	POSITION	posTempl = m_pDocManager->GetFirstDocTemplatePosition();

	while( posTempl )
	{
		CDocTemplate	*pTempl = m_pDocManager->GetNextDocTemplate( posTempl );
		POSITION pos = pTempl->GetFirstDocPosition();

		while( pos )
		{
			CDocument *pdoc = pTempl->GetNextDoc( pos );

			GuidKey lKeyTest = ::GetObjectKey( pdoc->GetControllingUnknown() );

			if( lKey == lKeyTest )
				pdocToActivate = pdoc;
		}
	}

	if( pdocToActivate )
	{
		//CString str = "...CShellDocManagerDisp::SetActiveDocumentDisp ";
		//str += pdocToActivate->GetTitle();
		//str += "\r\n";
		//_trace_file_text("trace_aam", str);

		CShellDocTemplate	*pTempl = static_cast<CShellDocTemplate*>(pdocToActivate->GetDocTemplate());

		if( pTempl )
			pTempl->SetActiveDocument( pdocToActivate, true );
	}
	else
	{
		//_trace_file_text("trace_aam", "...CShellDocManagerDisp::SetActiveDocumentDisp ");
	}
}

LPDISPATCH CShellDocManagerDisp::GetActiveDocumentByType(LPCTSTR szType) 
{
	CShellDocTemplate	*pTempl = m_pDocManager->GetDocTemplate( szType );

	if( !pTempl )
		return 0;

	CDocument *pDoc = pTempl->GetActiveDocument();

	if( pDoc )
	{
		//CString str = "...CShellDocManagerDisp::GetActiveDocumentByType( ";
		//str += szType;
		//str += ") = ";
		//str += pDoc->GetTitle();
		//str += "\r\n";
		//_trace_file_text("trace_aam", str);

		return pDoc->GetIDispatch( true );
	}

	return 0;
}

long CShellDocManagerDisp::GetDocTypesCount() 
{
	return m_pDocManager->GetDocTemplateCount();
}

BSTR CShellDocManagerDisp::GetDocTypeName(long nPos) 
{
	CString strResult;
	
	CShellDocTemplate	*pTempl = m_pDocManager->GetDocTemplateByPos( nPos );

	if( pTempl )
		strResult = pTempl->GetTemplateName();

	return strResult.AllocSysString();
}

LPDISPATCH CShellDocManagerDisp::GetDocument(long nPos, LPCTSTR szType) 
{
	CShellDocTemplate	*pTempl = m_pDocManager->GetDocTemplate( szType );

	if( !pTempl )
		return 0;

	CDocument *pDoc = pTempl->GetDocumentByPos( nPos );

	if( pDoc )
		return pDoc->GetIDispatch( true );

	return 0;
}

long CShellDocManagerDisp::GetDocumentsCount(LPCTSTR szType) 
{
	CShellDocTemplate	*pTempl = m_pDocManager->GetDocTemplate( szType );

	if( !pTempl )
		return 0;

	return pTempl->GetDocumentCount();
}



LPDISPATCH CShellDocManagerDisp::GetActiveObject(LPCTSTR lpszProgID) 
{
	IDispatchPtr	ptrDisp;
	ptrDisp.GetActiveObject( lpszProgID );
	if( ptrDisp == 0 )ptrDisp.CreateInstance( lpszProgID );

	return ptrDisp.Detach();
}

BSTR CShellDocManagerDisp::GenerateGUID() 
{
	GUID	guid;

	CoCreateGuid( &guid );
	BSTR	bstr;
	StringFromCLSID( guid, &bstr );
	CString strResult = bstr;
	::CoTaskMemFree( bstr );

	strResult.MakeUpper();

	return strResult.AllocSysString();
}

BOOL CShellDocManagerDisp::IsPointerNull(const VARIANT& var) 
{
	if( var.vt == VT_DISPATCH )
		return var.pdispVal==0;
	if( var.vt == VT_UNKNOWN )
		return var.punkVal==0;

	if( var.vt == (VT_VARIANT|VT_BYREF) )
	{
		if( var.pvarVal->vt == VT_DISPATCH )
			return var.pvarVal->pdispVal == 0;
		if( var.pvarVal->vt == VT_UNKNOWN )
			return var.pvarVal->punkVal == 0;
	}


	return 1;
}

void CShellDocManagerDisp::IdleUpdateActions() 
{
	theApp.OnIdle( 0 );
}

BSTR CShellDocManagerDisp::LoadString(LPCTSTR sz_id) 
{
	return LoadStringEx( sz_id, "Strings" );
}

BSTR CShellDocManagerDisp::LoadStringEx(LPCTSTR sz_id, LPCTSTR sz_sect) 
{
	CString strResult(_T(""));

	// [vanek] - 26.10.2004
	CString str_id( sz_id );
	str_id.TrimLeft();
	str_id.TrimRight();
	if( !str_id.IsEmpty() )
	{
     
		GetModuleFileName( 0, sz_string_file, sizeof( sz_string_file ) );
		strcpy( strrchr( sz_string_file, '\\' )+1, "resource." );

		CString	strLang = GetValueString( GetAppUnknown(), "\\General", "Language", "en" );
		strcat( sz_string_file, strLang );
		
		char	*pbuffer = strResult.GetBuffer( 4096 );
		char sz_default[] = "";
		::GetPrivateProfileString( sz_sect, sz_id, sz_default, pbuffer, 4096, sz_string_file );
		if(0==*pbuffer)
		{
			strcpy(pbuffer, sz_id);
			::WritePrivateProfileString( sz_sect, sz_id, pbuffer, sz_string_file );
		}

		char	*pout = pbuffer;

		while( *pbuffer )
		{
			if( *pbuffer == '\\' )
			{
				pbuffer++;
				if( *pbuffer == 'n' || *pbuffer == 'N' )
					*pbuffer = '\n';
				if( *pbuffer == 'r' || *pbuffer == 'R' )
					*pbuffer = '\r';
			}
			*pout = *pbuffer;
			pout++;pbuffer++;
		}
		*pout = 0;

		strResult.ReleaseBuffer();
	}
	
	return strResult.AllocSysString();
}

