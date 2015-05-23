// AXInsertPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "axeditor.h"
#include "AXInsertPropPage.h"
#include "other\insctldg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define ToolBarID				WM_USER+381
#define ToolBarButtonsIDBase	/*WM_USER+*/382
#define MaxButtonsCount			50
#define IconSide				15

#include "PropPage.h"

#define SZ_AXSECTION		"\\AXEditor"
#define SZ_PRINTSECTION		"\\ReportEditor"
#define SZ_DATABASE			"\\BlankEditor"


IMPLEMENT_DYNCREATE(CAXInsertPropPage, CDialog)

// {BBC7A191-D941-11d3-A096-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CAXInsertPropPage, szAXInsertPropPageProgID, 
0xbbc7a191, 0xd941, 0x11d3, 0xa0, 0x96, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);


HBITMAP CAXInsertPropPage::_GetBitmapFromProgID(CString strProgID)
{
	//return  0;
	_try(CAXInsertPropPage, Invoke)
	{
		HBITMAP hBitmap = 0;
		CLSID clsid;
		::CLSIDFromProgID(_bstr_t(strProgID), &clsid);
	
		BSTR	bstr = 0;
		CString strCLSID;
		if(!::StringFromCLSID(clsid, &bstr))
		{
			strCLSID = bstr;
			CoTaskMemFree(bstr);
		
	
			LONG lSize ;
			LONG	 regResult = ERROR_SUCCESS;
			HKEY hKey;
			char szBuffer[MAX_PATH*2] ;
		
	
			regResult = ::RegOpenKey(HKEY_CLASSES_ROOT, CString("CLSID\\")+strCLSID, &hKey);
			if (regResult != ERROR_SUCCESS) return 0;
				lSize = sizeof(szBuffer) ; 
			regResult = ::RegQueryValue(hKey, "ToolboxBitmap32", szBuffer, &lSize) ;
			if (regResult == ERROR_SUCCESS)
			{
				CString strPathAndResNum(szBuffer, (int)lSize) ;
				// Let's pretend we're using Basic.
				int indexComma = strPathAndResNum.Find(',') ; 
				CString strPath, strResNum;
				if( indexComma != -1 )
				{
					strPath = strPathAndResNum.Left(indexComma) ;
					strResNum = strPathAndResNum.Mid(indexComma+1) ;	
				}
				WORD wResNum = atoi(strResNum) ;

				HINSTANCE hInstCtl  = ::LoadLibrary(strPath) ;
				if (hInstCtl != NULL)
				{
					// The .OCX file might not actually exist.
					hBitmap = ::LoadBitmap(hInstCtl, MAKEINTRESOURCE(wResNum)) ;
					::FreeLibrary(hInstCtl) ;
				}
			}
			::RegCloseKey(hKey); 
		}
		return hBitmap;
	}
	_catch;
	return  0;
}

/////////////////////////////////////////////////////////////////////////////
// CAXInsertPropPage dialog


CAXInsertPropPage::CAXInsertPropPage(CWnd* pParent /*=NULL*/)
	: CPropertyPageBase(CAXInsertPropPage::IDD)
{

	//{{AFX_DATA_INIT(CAXInsertPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCAXInsertPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_INSERT );
	m_pToolBarCtrl = 0;
	m_nCurButton = 0;
	m_nAddedButtonsCount = 0;

	m_bButtonPressed = false;

	m_bitmaps.SetSize( 0, 100 );	
	
}


CAXInsertPropPage::~CAXInsertPropPage()
{
	if(m_pToolBarCtrl)
	{
		delete m_pToolBarCtrl;
		m_pToolBarCtrl = 0;
	}

	m_arrProgIDs.RemoveAll();

	//m_imgList.DeleteImageList();
}

void CAXInsertPropPage::DoLoadSettings()
{
	//paul
	//_LoadToolBar();
	//TRACE("Loading settings ....\n");
	//eofCode paul
}

void CAXInsertPropPage::DoStoreSettings()
{
	//paul
	//_SaveToolBar( );
	//TRACE("Saving settings ....\n");
	//eofCode paul
}



void CAXInsertPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAXInsertPropPage)
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAXInsertPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CAXInsertPropPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ToolBarButtonsIDBase, (ToolBarButtonsIDBase + MaxButtonsCount), OnCommandToolBar)
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CAXInsertPropPage, CPropertyPageBase)
	INTERFACE_PART(CAXInsertPropPage, IID_IAXInsertPropPage, Insert)	
	INTERFACE_PART(CAXInsertPropPage, IID_IEventListener, EvList)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////////////////
//ITextPage interface implementation
IMPLEMENT_UNKNOWN(CAXInsertPropPage, Insert);
HRESULT CAXInsertPropPage::XInsert::GetCurProgID( BSTR *pbstrProgID )
{
	_try_nested(CAXInsertPropPage, Insert, GetCurProgID)
	{
		if(pbstrProgID && pThis->m_nCurButton >= 0 && pThis->m_arrProgIDs.GetSize() > 0)
			*pbstrProgID = pThis->m_arrProgIDs[pThis->m_nCurButton].AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXInsertPropPage::XInsert::UnCheckCurrentButton()
{
	_try_nested(CAXInsertPropPage, Insert, UnCheckCurrentButton)
	{		
		if( pThis->m_pToolBarCtrl && pThis->m_nCurButton >= 0 )
		{			
			pThis->m_pToolBarCtrl->CheckButton( ToolBarButtonsIDBase+pThis->m_nCurButton, FALSE);
			pThis->m_bButtonPressed = false;
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXInsertPropPage::XInsert::CheckCurrentButton()
{
	_try_nested(CAXInsertPropPage, Insert, CheckCurrentButton)
	{		
		if( pThis->m_pToolBarCtrl && pThis->m_nCurButton >= 0 )
		{			
			//pThis->m_pToolBarCtrl->CheckButton( ToolBarButtonsIDBase+pThis->m_nCurButton, TRUE);
			pThis->_CheckButton( pThis->m_nCurButton );
			pThis->m_bButtonPressed = true;
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CAXInsertPropPage::XInsert::OnActivateView()
{
	_try_nested(CAXInsertPropPage, Insert, OnActivateView)
	{		
		pThis->OnActivateView();

		return S_OK;
	}
	_catch_nested;
}



void CAXInsertPropPage::RunActionAXInsert( )
{
	bool bButtonPressed = m_bButtonPressed;
	if( m_nCurButton >= 0 )
	{					
		ExecuteAction( GetActionNameByResID( IDS_ACTION_INSERT_CONTROL ));		
		if( bButtonPressed )
			ExecuteAction( GetActionNameByResID( IDS_ACTION_INSERT_CONTROL ));		
	}
}

void CAXInsertPropPage::OnActivateView()
{
	if( m_pToolBarCtrl == NULL )
		return;

	if( !::IsWindow( m_pToolBarCtrl->GetSafeHwnd()) )
		return;

	//Change view type
	CString strPrevSection = GetPrevSection();
	if( strPrevSection != GetCurrentSection() )
	{

		CreateToolbar();
		_LoadToolBar();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAXInsertPropPage message handlers

/////////////////////////////////////////////////////////////////////////////
bool CAXInsertPropPage::CreateToolbar()
{
	m_arrProgIDs.RemoveAll();	
	m_nCurButton = -1;
	m_nAddedButtonsCount = 0;


	if( GetDlgItem(IDC_ST_NAME) )
		GetDlgItem(IDC_ST_NAME)->SetWindowText( "" );

	for( int i = 0; i < m_bitmaps.GetSize(); i++ )
	{
		HBITMAP	h = (HBITMAP)m_bitmaps[i];
		::DeleteObject( h );
	}

	m_bitmaps.RemoveAll();


	if( m_pToolBarCtrl  )
	{
		if( m_pToolBarCtrl->GetSafeHwnd() )
		{
			while( m_pToolBarCtrl->GetButtonCount() )
				m_pToolBarCtrl->DeleteButton( 0 );		
			
			VERIFY( m_pToolBarCtrl->DestroyWindow() );
		}

		delete m_pToolBarCtrl;
		m_pToolBarCtrl = 0;
	}

	CRect rcParent = NORECT;
	GetClientRect(rcParent);
	CRect rcBtn = NORECT;
	m_btnAdd.GetWindowRect(rcBtn);
	ScreenToClient(&rcBtn);
	CRect rcTool = rcParent;

	rcTool.DeflateRect(0, 0, 0, rcParent.bottom-rcBtn.top+5);
	m_pToolBarCtrl = new CToolBarCtrl();
	ASSERT(m_pToolBarCtrl != 0);
	return m_pToolBarCtrl->Create(WS_CHILD|WS_VISIBLE|TBSTYLE_WRAPABLE, rcTool, this, ToolBarID) == TRUE;
	//m_pToolBarCtrl->SetRows(2, TRUE, &rcTool);

	//m_imgList.Create(IconSide, IconSide, ILC_COLOR, 0, 1);
	//m_pToolBarCtrl->SetImageList(&m_imgList);	

}

BOOL CAXInsertPropPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	// TODO: Add extra initialization here

	INotifyControllerPtr sptrNC( GetControllingUnknown() );
	if( sptrNC )
		sptrNC->RegisterEventListener( NULL, GetControllingUnknown() );


	CreateToolbar();
		
	_LoadToolBar();


	if( GetValueInt( GetAppUnknown(), SZ_AXSECTION, "ShowAddDeleteButtons", 1) == 0 )
	{
		GetDlgItem( IDC_BUTTON_ADD )->ShowWindow( SW_HIDE );
		GetDlgItem( IDC_BUTTON_REMOVE )->ShowWindow( SW_HIDE );
	}


	return TRUE;

}

void CAXInsertPropPage::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	CInsertControlDlg	dlg( this);

	if( dlg.DoModal() != IDOK )
			return;
	/*BSTR	bstr = 0;
	if( ProgIDFromCLSID( dlg.m_clsid, &bstr ) )
		return;
	CString strCurProgID = bstr;
	CoTaskMemFree( bstr );*/

	//long nEntriesCount = 0;
	sptrINamedData spShellData( GetAppUnknown() );
	if( spShellData )
	{		
		spShellData->SetupSection( _bstr_t(GetCurrentSection()) );
		//spShellData->GetEntriesCount( &nEntriesCount );				
	}

	int nSize = (int)dlg.m_arrCLSID.GetSize();
	for(int i = 0; i < nSize; i++)
	{
		BSTR	bstr = 0;
		if( ProgIDFromCLSID( dlg.m_arrCLSID[i], &bstr ) )
			break;
		CString strCurProgID(bstr);
		CoTaskMemFree( bstr );
		

		if( spShellData )
		{								  			
			CString strEntry;
			strEntry.Format("Control_%s", strCurProgID );
			::SetValue(GetAppUnknown(), GetCurrentSection(), strEntry, _variant_t(strCurProgID));
		}

		_AddCtrlButton(strCurProgID);
	}

	::SetValue(GetAppUnknown(), GetCurrentSection(), "ActiveButton", _variant_t((long)m_nCurButton));
	
}

void CAXInsertPropPage::OnCommandToolBar(UINT nID)
{	

	m_nCurButton = nID - ToolBarButtonsIDBase;
	ASSERT(m_nCurButton >= 0);

	_CheckButton();
	//::SetValue(GetAppUnknown(), SZ_AXSECTION, "ActiveButton", _variant_t((long)m_pToolBarCtrl->CommandToIndex(nID)));	
	//::ExecuteAction( GetActionNameByResID( IDS_ACTION_INSERT_CONTROL ) );

	RunActionAXInsert( );	
}

void CAXInsertPropPage::OnButtonRemove() 
{
	if(m_nCurButton < 0)return;
	m_pToolBarCtrl->CheckButton(ToolBarButtonsIDBase+m_nCurButton, FALSE);
	int idx = m_pToolBarCtrl->CommandToIndex(ToolBarButtonsIDBase+m_nCurButton);
	


	////////////////////////
	//remove from shell data
	int nBtnCount = m_pToolBarCtrl->GetButtonCount();
	if( m_nCurButton >= 0 && m_nCurButton < nBtnCount )
	{		
		
		TBBUTTON tbb;
		m_pToolBarCtrl->GetButton(m_nCurButton, &tbb);
		int index = tbb.idCommand - ToolBarButtonsIDBase;
		if( index >= 0 && index < m_arrProgIDs.GetSize() )
		{
			//long nEntriesCount = 0;
			sptrINamedData spShellData( GetAppUnknown() );
			if( spShellData )
			{		
				//spShellData->SetupSection( _bstr_t() );
				//spShellData->GetEntriesCount( &nEntriesCount );				
				CString strEntry;
				strEntry.Format("%s\\Control_%s", GetCurrentSection(), m_arrProgIDs[index] );
				spShellData->DeleteEntry( _bstr_t(strEntry) );
			}
		}
	}
	
	m_pToolBarCtrl->DeleteButton(idx);
	////////////////////////

	nBtnCount = m_pToolBarCtrl->GetButtonCount();
	m_nCurButton = -1;
	if(idx >= nBtnCount)
		idx--;

	m_nCurButton = idx ;
	_CheckButton( idx );
	::SetValue(GetAppUnknown(), GetCurrentSection(), "ActiveButton", _variant_t((long)m_nCurButton));

}

void CAXInsertPropPage::_AddCtrlButton(CString strProgID)
{
	
	int nBtnCount = m_pToolBarCtrl->GetButtonCount();
	bool bAlreadyInArray = false;

	
	if(m_nAddedButtonsCount > MaxButtonsCount)
	{
		ASSERT(FALSE);
		return;
	}

	//check whether this progid button already in toolbar 
	for(int i = 0; i < nBtnCount; i++)
	{
		int idx = 0;
		TBBUTTON tbb;
		m_pToolBarCtrl->GetButton(i, &tbb);
		idx = tbb.idCommand - ToolBarButtonsIDBase;
		if(m_arrProgIDs[idx] == strProgID)
		{
			bAlreadyInArray = true;
			break;
		}
	}

	
	if(!bAlreadyInArray)
	{
		if(m_nCurButton >= 0)
		{
			//uncheck prev button
			m_pToolBarCtrl->CheckButton(ToolBarButtonsIDBase+m_nCurButton, FALSE);
		}
		
		//HICON	hIcon = 0;
		//hIcon = COleClientItem::GetIconFromRegistry(clsid);
		HBITMAP hBitmap = _GetBitmapFromProgID(strProgID);
		if(hBitmap == 0)
			hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_NOTSPECIFIED));
		if( hBitmap )
		{
			m_pToolBarCtrl->AddBitmap(1, CBitmap::FromHandle(hBitmap));
			m_bitmaps.Add( (void*)hBitmap );
			//::DeleteObject( hBitmap );
		}
		m_arrProgIDs.Add(strProgID);
		//add button to toolbar & check it
		CRect rcLastBtn = NORECT;
		m_pToolBarCtrl->GetItemRect(nBtnCount-1, &rcLastBtn);
		CRect rcClient = NORECT;
		GetClientRect(rcClient);
		TBBUTTON tbb;
		tbb.iBitmap = m_nAddedButtonsCount;
		tbb.idCommand = ToolBarButtonsIDBase+m_nAddedButtonsCount;
		tbb.fsState = TBSTATE_ENABLED;
		//paul
		//|TBSTATE_CHECKED;
		//eofCode paul
		tbb.fsStyle = TBSTYLE_CHECKGROUP;
		if(rcLastBtn.right + 2*(LOWORD(m_pToolBarCtrl->GetButtonSize())) > rcClient.right)
		{
			tbb.fsState |= TBSTATE_WRAP;
		}
		tbb.iString = 0;
		tbb.dwData = 0;
		m_pToolBarCtrl->AddButtons(1, &tbb);
		m_nCurButton = m_nAddedButtonsCount;
		//::SetValue(GetAppUnknown(), SZ_AXSECTION, "ActiveButton", _variant_t((long)m_pToolBarCtrl->CommandToIndex(m_nCurButton+ToolBarButtonsIDBase)));

		//paul
		//_CheckButton(-1);
		//eofCode paul
		
		m_nAddedButtonsCount++;				
	}
}


void CAXInsertPropPage::_LoadToolBar()
{
	CWaitCursor wait;

	CString strSection = GetCurrentSection();

	//Paul New code.
	long nEntriesCount = 0;
	sptrINamedData spShellData( GetAppUnknown() );
	if( spShellData )
	{		
		spShellData->SetupSection( _bstr_t(strSection) );
		spShellData->GetEntriesCount( &nEntriesCount );				
	}

	if( spShellData )
	{								  				
		for(int i = 0; i < nEntriesCount; i++)
		{
			BSTR bstrEntryName;
			spShellData->GetEntryName( i, &bstrEntryName );
			CString strEntry = bstrEntryName;
			::SysFreeString( bstrEntryName );
			if( strEntry.Find( "Control_" ) != -1 )			
			{
				CString srtProgID = ::GetValueString(GetAppUnknown(), strSection, strEntry, "" );
				if( !srtProgID.IsEmpty() )
					_AddCtrlButton( srtProgID );
			}
		}
		m_nCurButton = ::GetValueInt(GetAppUnknown(), strSection, "ActiveButton", 0);
		m_nCurButton = min(m_nCurButton, m_nAddedButtonsCount);
		/*
		if(m_nCurButton >= 0)
			_CheckButton(m_nCurButton);
			*/

	}

	//paul
	/*
	CString strVal, strEntry;
	int idx = 0;
	while( true )				
	{
		strEntry.Format("Control_%d", idx);
		strVal	= ::GetValueString(GetAppUnknown(), SZ_AXSECTION, strEntry, "");
		::DeleteEntry(GetAppUnknown(), strEntry);
		if(!strVal.IsEmpty())
			_AddCtrlButton(strVal);
		else
			break;
		idx++;
	}
	m_nCurButton = ::GetValueInt(GetAppUnknown(), SZ_AXSECTION, "ActiveButton", 0);
	m_nCurButton = min(m_nCurButton, m_nAddedButtonsCount);	;	
	
	if(m_nCurButton >= 0)
		_CheckButton(m_nCurButton);
	//eofCode paul
	*/

}

void CAXInsertPropPage::_SaveToolBar()
{
	//Do not call' cos each ProgID is written to shell data after Add and remove after delete at once
	ASSERT( FALSE );

	CString strEntry;
	int nBtnCount = m_pToolBarCtrl->GetButtonCount();
	long nActive = -1;
	for(int i = 0; i < nBtnCount; i++)
	{
		int idx = 0;
		TBBUTTON tbb;
		m_pToolBarCtrl->GetButton(i, &tbb);
		idx = tbb.idCommand - ToolBarButtonsIDBase;
		if(idx == m_nCurButton)
			nActive = i;
		strEntry.Format("Control_%d", i);
		::SetValue(GetAppUnknown(), GetCurrentSection(), strEntry, _variant_t(m_arrProgIDs[idx]));
	}
	::SetValue(GetAppUnknown(), GetCurrentSection(), "ActiveButton", _variant_t(nActive));	
	
}

void CAXInsertPropPage::OnDestroy() 
{

	INotifyControllerPtr sptrNC( GetControllingUnknown() );
	if( sptrNC )
		sptrNC->UnRegisterEventListener( NULL, GetControllingUnknown() );
	
	//_SaveToolBar();	
	for( int i = 0; i < m_bitmaps.GetSize(); i++ )
	{
		HBITMAP	h = (HBITMAP)m_bitmaps[i];
		::DeleteObject( h );
	}

	m_bitmaps.RemoveAll();

	CPropertyPageBase::OnDestroy();
	// TODO: Add your message handler code here
}


void CAXInsertPropPage::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	CPropertyPageBase::OnFinalRelease();
}

void CAXInsertPropPage::_CheckButton(int nBtn)
{
	if(m_arrProgIDs.GetSize() <= 0) return;	

	GetDlgItem(IDC_ST_NAME)->SetWindowText( "" );

	if(nBtn >= 0) 
		m_pToolBarCtrl->CheckButton(ToolBarButtonsIDBase+nBtn, TRUE);
	else
		nBtn = m_nCurButton;

	//paul
	if( !( nBtn >=0 && nBtn < m_arrProgIDs.GetSize() ) )
		return;
	//eofCode paul
	

	//Get Name from ProgId
	CString strProgID = m_arrProgIDs[nBtn];
	CString strName;
	CLSID clsid;
	::CLSIDFromProgID(_bstr_t(strProgID), &clsid);
	BSTR	bstr = 0;
	CString strCLSID;
	if(!::StringFromCLSID(clsid, &bstr))
	{
		strCLSID = bstr;
		CoTaskMemFree(bstr);
	
		LONG lSize ;
		LONG	 regResult = ERROR_SUCCESS;
		HKEY hKey;
		char szBuffer[MAX_PATH*2] ;
		
	
		regResult = ::RegOpenKey(HKEY_CLASSES_ROOT, CString("CLSID\\")+strCLSID, &hKey);
		if (regResult == ERROR_SUCCESS)
		{
			lSize = sizeof(szBuffer) ; 
			regResult = ::RegQueryValue(hKey, NULL, szBuffer, &lSize) ;
			if (regResult == ERROR_SUCCESS)
			{
				strName = CString(szBuffer, (int)lSize) ;
			}
		}
	}

	GetDlgItem(IDC_ST_NAME)->SetWindowText(strName);


	::SetValue(GetAppUnknown(), GetCurrentSection(), "ActiveButton", _variant_t((long)m_nCurButton));
	
}

//return section in shell data depend of curent view
CString CAXInsertPropPage::GetCurrentSection()
{
	sptrIApplication spApp( GetAppUnknown() );
	{
		if( spApp )
		{
			IUnknown* pUnk = NULL;
			spApp->GetActiveDocument( &pUnk );
			if( pUnk )
			{
				sptrIDocumentSite spDocSite( pUnk );
				pUnk->Release();
				pUnk = NULL;

				if( spDocSite )
				{
					spDocSite->GetActiveView( &pUnk );
					if( pUnk )
					{
						CString strSection;
						if( CheckInterface( pUnk, IID_IReportView ) )
						{
							strSection = SZ_PRINTSECTION;
						}
						else if( CheckInterface( pUnk, IID_IBlankView ) )
						{
							strSection = SZ_DATABASE;
						}
						else
						{
							if( CheckInterface( pUnk, IID_ILayoutView ) )
							{
								strSection = SZ_AXSECTION;
							}
						}

						pUnk->Release();
						pUnk = NULL;
						
						m_strPrevSection = strSection;
						VERIFY( !strSection.IsEmpty() );
						return strSection;
					}
				}
			}
		}
	}

	m_strPrevSection = SZ_AXSECTION;
	return SZ_AXSECTION;
}

CString CAXInsertPropPage::GetPrevSection()
{
	return m_strPrevSection;
}



void CAXInsertPropPage::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp(pszEvent, szEventAXInsertControlDone) )
	{
		sptrIAXInsertPropPage sptrAXPP( GetControllingUnknown() );
		if( sptrAXPP )
			sptrAXPP->UnCheckCurrentButton();		
	}
	else
	if( !strcmp(pszEvent, "AXInsertControlInit") )
	{
		sptrIAXInsertPropPage sptrAXPP( GetControllingUnknown() );
		if( sptrAXPP )
			sptrAXPP->CheckCurrentButton();		
	}

}
