// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "axform.h"
#include "OptionsDlg.h"
#include "GfxOutBarCtrl.h"
#include "Page.h"
#include "FormManager.h"
#include "afxpriv.h"


#define IntervalSize 5
/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

template <class Type>void CAccocTable<Type>::SetAt( CString Key, Type Item )
{
	int nSz = m_Keys.size();

	bool bOK = false;
	for( int i = 0; i < nSz; i++ )
	{
		if( m_Keys[i] == Key )
		{
			bOK = true;
			m_Items[i] = Item;
			break;
		}
	}
	
	if( !bOK )
	{
		m_Keys.push_back( Key );
		m_Items.push_back( Item );
	}
}
template <class Type>void CAccocTable<Type>::RemoveAll( bool bDelete )
{
	if( bDelete )
	{
		int nSz = m_Keys.size();

		for( int i = 0; i < nSz; i++ )
		{
			if( m_Items[i] )
				delete m_Items[i];
		}
	}

	m_Keys.clear();
	m_Items.clear();
}

template <class Type>void CAccocTable<Type>::Erase( CString Key )
{
	int nSz = m_Keys.size();

	for( int i = 0; i < nSz; i++ )
	{
		if( m_Keys[i] == Key )
		{
			if( m_Items[i] )
				delete m_Items[i];

			m_Keys.erase( m_Keys.begin() + i );
			m_Items.erase( m_Items.begin() + i );
			return;
		}
	}
}

template <class Type> void CAccocTable<Type>::Erase( Type Item )
{
	int nSz = m_Keys.size();

	for( int i = 0; i < nSz; i++ )
	{
		if( m_Items[i] == Item )
		{
			if( m_Items[i] )
				delete m_Items[i];

			m_Keys.erase( m_Keys.begin() + i );
			m_Items.erase( m_Items.begin() + i );
			return;
		}
	}
}

template <class Type>bool CAccocTable<Type>::Lookup( CString Key, Type &Item )
{
	int nSz = m_Keys.size();

	for( int i = 0; i < nSz; i++ )
		if( m_Keys[i] == Key)
		{
			Item = m_Items[i];
			return true;
		}

	Item = 0;
	return false;
}

COptionsDlg::COptionsDlg()
	: CDialog(COptionsDlg::IDD)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	//}}AFX_DATA_INIT
	_Initial();	
}

void COptionsDlg::SetParent( HWND hwnd )
{
	m_pParentWnd = CWnd::FromHandlePermanent( hwnd );
}


COptionsDlg::~COptionsDlg()
{
	if(m_pOutBarCtrl)
		delete m_pOutBarCtrl;

	m_listLarge.DeleteImageList();	

	for( int i = 0; i < m_pSheets.GetSize(); i++ )
	{
		m_pSheet = m_pSheets.GetItem( i );
		if(m_pSheet)
			_RemoveAllFromSheet();
	}
	m_pSheets.RemoveAll();
	m_pActivates.RemoveAll();
	m_mapPages.RemoveAll( false );
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_STATIC_TAB, m_stTab);
	DDX_Control(pDX, IDC_FAKE_STATIC, m_FakeStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutbarNotify)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_MESSAGE(WM_HELP, OnHelpHitTest)
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

void COptionsDlg::OnApply() 
{
	// TODO: Add your control notification handler code here
	_OnOK();	
}

void COptionsDlg::OnOK() 
{
	// TODO: Add extra validation here
	_OnOK();	

	for( int i = 0; i < m_pSheets.GetSize(); i++ )
	{
		m_pSheet = m_pSheets.GetItem( i );
		_RemoveAllFromSheet();
	}

	CDialog::OnOK();
}

void COptionsDlg::OnCancel() 
{
	for( int i = 0; i < m_pSheets.GetSize(); i++ )
	{
		m_pSheet = m_pSheets.GetItem( i );

		int nCount = m_pSheet->GetPageCount();
		for(int i = nCount-1; i >= 0; i--) 
		{

			CPropertyPage *pPage = m_pSheet->GetPage(i);
			if( pPage && pPage->m_hWnd )
				((CPage*)pPage)->m_site.FireOnCancel();
		}
		_RemoveAllFromSheet();
	}
	CDialog::OnCancel();
}

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

//	SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, GetExStyle()|WS_EX_CONTEXTHELP );
	
	
	// TODO: Add extra initialization here

	CRect rcDialog = NORECT;
	GetWindowRect(&rcDialog);

	//paul 	26.04.2001 -> WS_CLIPCHILDREN|WS_CLIPSIBLINGS
	int nSheetWidth, nSheetHeigth, nOutBarWidth;
	for( int i = 0; i < m_pSheets.GetSize(); i++ )
	{
		m_pSheet = m_pSheets.GetItem( i );
		
		m_pSheet->Create(this,  WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS/*|DS_CONTEXTHELP, WS_EX_CONTEXTHELP*/);


		/*SetWindowLong( m_pSheet->GetSafeHwnd(), GWL_EXSTYLE, m_pSheet->GetExStyle()|WS_EX_CONTEXTHELP );
		SetWindowLong( m_pSheet->GetTabControl()->GetSafeHwnd(), GWL_EXSTYLE, m_pSheet->GetTabControl()->GetExStyle()|WS_EX_CONTEXTHELP );*/
		
		CRect rcSheet = NORECT;

		//if(m_pSheet->m_hWnd != 0)
		//	m_pSheet->GetWindowRect(&rcSheet);
		rcSheet = _CalcSheetRect();
		if(rcSheet == NORECT)
		{
			m_FakeStatic.GetWindowRect(&rcSheet);
		}

		CSize size(5,5);

		if(m_pSheet->m_hWnd)
		{
			//size = m_pSheet->GetTabControl()->SetItemSize(CSize(0,0));
			//m_pSheet->GetTabControl()->SetItemSize(size);
			//rcSheet.InflateRect(0, 0, 5, size.cx);
			CRect rcItem = NORECT;
			m_pSheet->GetTabControl()->GetItemRect(0, &rcItem);
			rcSheet.InflateRect(0, 0, 5, rcItem.Height()+5);
		}

		nSheetWidth = rcSheet.Width();
		nSheetHeigth = rcSheet.Height();
		
		//change height of outlook control
		CRect rc = NORECT;
		m_FakeStatic.GetWindowRect(&rc);
		nOutBarWidth = rc.Width();
		//ScreenToClient(&rc);
		rc.left = IntervalSize;
		rc.top = IntervalSize;
		rc.right = rc.left + nOutBarWidth;
		rc.bottom = rc.top + nSheetHeigth;
		m_FakeStatic.MoveWindow(&rc);
		rc.DeflateRect(1,1,1,1);
		if(m_pOutBarCtrl && m_pOutBarCtrl->m_hWnd)
		{
			m_pOutBarCtrl->MoveWindow(&rc);
			m_pOutBarCtrl->SetActiveItem(m_nActiveItem);
		}
		rc.InflateRect(1,1,1,1);

		//move sheet
		rcSheet.left = nOutBarWidth + 2*IntervalSize;
		rcSheet.top = IntervalSize;
		rcSheet.right = rcSheet.left + nSheetWidth;
		rcSheet.bottom = rcSheet.top + nSheetHeigth;
		if(m_pSheet->m_hWnd != 0)
		{
			m_pSheet->MoveWindow(&rcSheet);
			ClientToScreen(&rcSheet);
			m_pSheet->ScreenToClient(&rcSheet);
			m_pSheet->GetTabControl()->MoveWindow(&rcSheet);
		}
	}
	
	//resize dialog
	CRect rcBtn = NORECT;
	GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
	int nBtnHeight = rcBtn.Height();
	rcDialog.bottom = rcDialog.top + nSheetHeigth + 3*IntervalSize + nBtnHeight + 2*GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYCAPTION);
	rcDialog.right = rcDialog.left + nOutBarWidth + 3*IntervalSize + nSheetWidth + 2*GetSystemMetrics(SM_CXDLGFRAME);
	SetWindowPos(0, 0, 0, rcDialog.Width(), rcDialog.Height(), SWP_NOZORDER|SWP_NOMOVE);

	GetClientRect(&rcDialog);

	//move buttons
	int	nCurrent = 1;
	int nBtnWidth = rcBtn.Width();
	CRect rcBtnMove = NORECT;
	rcBtnMove = CRect(rcDialog.Width() - nCurrent*IntervalSize - nCurrent*nBtnWidth, nSheetHeigth + 2*IntervalSize, 0, 0);
	rcBtnMove.right = rcBtnMove.left + nBtnWidth;
	rcBtnMove.bottom = rcBtnMove.top + nBtnHeight;
	GetDlgItem(IDHELP)->MoveWindow(&rcBtnMove);
	nCurrent++;

	if( GetValueInt( GetAppUnknown(), "\\Interface", "ShowApplyOnOptions", 1 ) )
	{
		rcBtnMove = CRect(rcDialog.Width() - nCurrent*IntervalSize - nCurrent*nBtnWidth, nSheetHeigth + 2*IntervalSize, 0, 0);
		rcBtnMove.right = rcBtnMove.left + nBtnWidth;
		rcBtnMove.bottom = rcBtnMove.top + nBtnHeight;
		GetDlgItem(ID_APPLY)->MoveWindow(&rcBtnMove);
		nCurrent++;
	}
	else
	{
		GetDlgItem(ID_APPLY)->ShowWindow( SW_HIDE );
	}
	rcBtnMove = CRect(rcDialog.Width() - nCurrent*IntervalSize - nCurrent*nBtnWidth, nSheetHeigth + 2*IntervalSize, 0, 0);
	rcBtnMove.right = rcBtnMove.left + nBtnWidth;
	rcBtnMove.bottom = rcBtnMove.top + nBtnHeight;
	GetDlgItem(IDCANCEL)->MoveWindow(&rcBtnMove);
	nCurrent++;
	
	rcBtnMove = CRect(rcDialog.Width() - nCurrent*IntervalSize - nCurrent*nBtnWidth, nSheetHeigth + 2*IntervalSize, 0, 0);
	rcBtnMove.right = rcBtnMove.left + nBtnWidth;
	rcBtnMove.bottom = rcBtnMove.top + nBtnHeight;
	GetDlgItem(IDOK)->MoveWindow(&rcBtnMove);
	nCurrent++;

	if(m_strActiveCategory == "")
	{
		if(m_pOutBarCtrl->GetItemCount() > 0)
		{
			SetActiveCategory(m_pOutBarCtrl->GetItemText(0));
		}
	}
	_SetSheetByCategory(m_strActiveCategory);

	GetDlgItem( IDC_STATIC_TAB )->ShowWindow( SW_HIDE );
	GetDlgItem( IDC_FAKE_STATIC )->ShowWindow( SW_HIDE );
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int COptionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pOutBarCtrl->UseAdvMode(false);
	if(m_listLarge.m_hImageList == 0)
	{
		//AfxMessageBox("Image list is empty!");
		LoadImageList("");
		ASSERT(m_listLarge.m_hImageList > 0);
	}
	//else
	{
		m_pOutBarCtrl->SetImageList( &m_listLarge, CGfxOutBarCtrl::fLargeIcon );
		m_pOutBarCtrl->Create(WS_VISIBLE, NORECT, this, 7489);
	}
	return 0;
}

void COptionsDlg::ResetControls()
{
	if(m_pOutBarCtrl)
	{
		delete m_pOutBarCtrl;
		m_pOutBarCtrl = new CGfxOutBarCtrl();
		m_pOutBarCtrl->crBackGroundColor = ::GetValueColor(GetAppUnknown(), "\\Interface", "OptionsBarBackground", ::GetSysColor(COLOR_BACKGROUND));
	}
	
	m_listLarge.DeleteImageList();

	//_KillPages();
	for( int i = 0; i < m_pSheets.GetSize(); i++ )
	{
		m_pSheet = m_pSheets.GetItem( i );
		if(m_pSheet)
			_RemoveAllFromSheet();
	}
	m_pSheets.RemoveAll();
	m_pActivates.RemoveAll();
	m_mapPages.RemoveAll( false );
}

void COptionsDlg::LoadImageList(CString strFileName)
{
	HBITMAP hBmp = 0;
	if(!strFileName.IsEmpty())
		hBmp = (HBITMAP)::LoadImage(0, strFileName, IMAGE_BITMAP, 0, 32, LR_LOADFROMFILE);
	else
	{
		hBmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_IMAGELIST));
	}

	if(hBmp == NULL)return;

	m_listLarge.DeleteImageList();
	m_listLarge.Create(32, 32, ILC_COLOR|ILC_MASK, 0, 1);
	if(m_pOutBarCtrl)
		m_listLarge.SetBkColor(m_pOutBarCtrl->crBackGroundColor);
	m_listLarge.Add(CBitmap::FromHandle(hBmp), RGB(255,0,255));
}

void COptionsDlg::AddItemToBar(int nImageIdx, CString strTitle)
{
	if( !m_pSheets.Lookup( strTitle, m_pSheet ) )
	{
		m_pSheet = new CSheet();
		m_pSheets.SetAt( strTitle, m_pSheet );
	}


	if(m_pOutBarCtrl->GetFolderCount() == 0)
	{
		m_pOutBarCtrl->AddFolder( "", 0 );
	}
	int nNum = m_pOutBarCtrl->GetItemCount();
	m_pOutBarCtrl->InsertItem( 0, nNum, strTitle, nImageIdx, /*dwExData*/0 );
}

void COptionsDlg::SetActiveCategory(CString strName)
{
	m_nActivePage = 0;

	int nNum = m_pOutBarCtrl->GetItemCount();
	for(int i = 0; i < nNum; i++)
	{
		if(m_pOutBarCtrl->GetItemText(i) == strName)
		{
			m_nActiveItem = i;
			m_strActiveCategory = strName;
		}
	}
}

void COptionsDlg::GetActiveCategory(CString& strName)
{
	strName = m_strActiveCategory;
}

void COptionsDlg::AddPageToCategory(CString strCategory, CString strForm, const char* psz_form_title)
{
	IUnknown	*punkForm = CFormManager::_GetFormByName( strForm );
	if(punkForm == 0) return;


	IActiveXFormPtr sptrForm(punkForm);

	if( !m_pSheets.Lookup( strCategory, m_pSheet ) )
	{
		m_pSheet = new CSheet();
		m_pSheets.SetAt( strCategory, m_pSheet );
	}


	CString	strTitle;
	BSTR	bstr;
	sptrForm->GetTitle( &bstr );
	strTitle = bstr;
	::SysFreeString( bstr );

	if( psz_form_title )
		strTitle = psz_form_title;


	//init m_template
	CDialogTemplate	dt;
	dt.Load( MAKEINTRESOURCE(IDD_DEFAULT_PAGE));
	HGLOBAL	hTempl = dt.Detach();
	SIZE size;	   
	sptrForm->GetSize( &size );
	((DLGTEMPLATE*)hTempl)->cx = (short)size.cx;
	((DLGTEMPLATE*)hTempl)->cy = (short)size.cy;

	CPage* pPage = new CPage();
	pPage->SetForm( punkForm );
	punkForm->Release();
	pPage->SetDlgTitle( strTitle );

	pPage->m_psp.dwFlags |= PSP_USETITLE|PSP_DLGINDIRECT|PSP_HASHELP;
	pPage->m_psp.pResource = (DLGTEMPLATE*)hTempl;


	bool bPresent = false;
	int nNum = m_pOutBarCtrl->GetItemCount();
	for(int i = 0; i < nNum; i++)
	{
		if(m_pOutBarCtrl->GetItemText(i) == strCategory)
		{
			bPresent = true;
		}
	}

	if(bPresent) //category is valid
	{
		CString strEntry = strCategory + "\n";
		strEntry += ::GetObjectName(pPage->GetControllingUnknown());
		m_pSheet->AddPage((CPropertyPage*)pPage);
		//m_pSheet->SetActivePage((CPropertyPage*)pPage);

		m_mapPages.SetAt(strEntry, pPage);
	}
	else
		delete pPage;
}

void COptionsDlg::_KillPages()
{
/*	POSITION pos = m_mapPages.GetStartPosition();
	while(pos)
	{
		CObject* pPage;
		CString strEntry;
		m_mapPages.GetNextAssoc( pos, strEntry, pPage );
		if(pPage)
		{
			delete pPage;
			pPage = 0;
		}
	}
*/	m_mapPages.RemoveAll();
}

void COptionsDlg::_SetSheetByCategory(CString strCategory)
{
	//if( m_pSheet && m_pSheet->GetSafeHwnd() )
	//	m_pSheet->ShowWindow( SW_HIDE );

	bool *bOK = 0;
	if( !m_pActivates.Lookup( strCategory, bOK ) )
	{
		bOK = new bool( false );
		m_pActivates.SetAt( strCategory, bOK );
	}
	else
		*bOK = true;


	for( int i = 0; i < m_pSheets.GetSize(); i++ )
	{
		m_pSheet = m_pSheets.GetItem( i );
		if( m_pSheet )
			m_pSheet->ShowWindow( SW_HIDE );
	}

	if( *bOK )
	{
		if( m_pSheets.Lookup( strCategory, m_pSheet ) )
			m_pSheet->ShowWindow( SW_SHOW );
		else
			*bOK = false;
	}


	if( !*bOK )
	{
		if( m_pSheets.Lookup( strCategory, m_pSheet ) )
		{
			_RemoveAllFromSheet();

			int nSz = m_mapPages.GetSize();
			for( int i = 0; i < nSz; i++ )
			{
				CString strEntry = m_mapPages.GetKey( i );
				CObject* pPage   = m_mapPages.GetItem( i );

				CString strCat = strEntry.Left(strEntry.Find("\n", 0));
				CString strForm =  strEntry.Right(strEntry.GetLength()-strEntry.Find("\n", 0)-1);
				if(strCat == strCategory)
				{
					AddPageToCategory(strCategory, strForm);
					//m_pSheet->AddPage((CPropertyPage*)pPage);
				}
			}
			m_pSheet->ShowWindow( SW_SHOW );
		}
	}


	m_pSheet->SetActivePage(m_nActivePage);

	{
		CPropertyPage *pPage = m_pSheet->GetPage(m_nActivePage);
		if( pPage )
			((CPage *)pPage)->m_site.FireOnShow();
	}

	if(m_pSheet->m_hWnd)
		m_pSheet->Invalidate();

	//if( m_pSheet && m_pSheet->GetSafeHwnd() )
		//m_pSheet->ShowWindow( SW_SHOWNA );

}

void COptionsDlg::_RemoveAllFromSheet()
{
	int nCount = m_pSheet->GetPageCount();
	for(int i = nCount-1; i >= 0; i--) 
	{
/*		CObject* pPage = 0;
		CString strEntry;
		POSITION pos = m_mapPages.GetStartPosition();
		while(pos)
		{
			m_mapPages.GetNextAssoc( pos, strEntry, pPage );
			if(m_pSheet->GetPage(i) == pPage)
				break;
		}
*/
		CPropertyPage *pPage = m_pSheet->GetPage(i);
		m_pSheet->RemovePage(i);
//		m_mapPages.Erase( pPage );

		if(pPage)
		{
			m_mapPages.SetAt( m_mapPages.GetKey( i ), 0);
			delete pPage;
			pPage = 0;
		}

/*		if(pPage)
		{
			m_mapPages.SetAt(strEntry, 0);
			delete pPage;
			pPage = 0;
		}
*/	}
}

LRESULT COptionsDlg::OnOutbarNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case NM_OB_ITEMCLICK:
		// cast the lParam to an integer to get the clicked item
		{
			if(lParam != m_nActiveItem)
			{
				m_nActiveItem = lParam;
				m_strActiveCategory = m_pOutBarCtrl->GetItemText(m_nActiveItem);
				_SetSheetByCategory(m_strActiveCategory);
			}
		}
		return 0;
	}
	return 0;
}

CRect COptionsDlg::_CalcSheetRect()
{
	CRect rcPage = NORECT;
	CRect rcSheet = NORECT;

/*	POSITION pos = m_mapPages.GetStartPosition();
	while(pos)
	{
		CObject* pPage;
		CString strEntry;
		m_mapPages.GetNextAssoc( pos, strEntry, pPage );
		if(pPage)
		{
			rcPage = CRect(0, 0, ((CPropertyPage*)pPage)->m_psp.pResource->cx, ((CPropertyPage*)pPage)->m_psp.pResource->cy);
			rcSheet.UnionRect(&rcSheet, &rcPage);
			rcSheet.NormalizeRect();		
		}
	}
*/
	int nSz = m_mapPages.GetSize();
	for( int i = 0; i < nSz; i++ )
	{
		CObject* pPage   = m_mapPages.GetItem( i );
		if(pPage)
		{
			rcPage = CRect(0, 0, ((CPropertyPage*)pPage)->m_psp.pResource->cx, ((CPropertyPage*)pPage)->m_psp.pResource->cy);
			rcSheet.UnionRect(&rcSheet, &rcPage);
			rcSheet.NormalizeRect();		
		}
	}

	return rcSheet;
}

bool COptionsDlg::SetActivePage(CString strForm)
{
	CString strEntry = m_strActiveCategory + "\n";
	strEntry += strForm;
	CPropertyPage* pPage;
	if(m_mapPages.Lookup(strEntry, pPage))
	{
		m_nActivePage = m_pSheet->GetPageIndex(pPage);
		return true;
	}
	return false;
}

void COptionsDlg::GetActivePage(CString& strForm)
{
	strForm = "";
	if(m_pSheet->GetPageCount() == 0) return;
	CPropertyPage* pPage = m_pSheet->GetPage(m_nActivePage);
	if(pPage)
		strForm = pPage->m_psp.pszTitle;
}

void COptionsDlg::_OnOK()
{
	for( int i = 0; i < m_pSheets.GetSize(); i++ )
	{
		m_pSheet = m_pSheets.GetItem( i );

		int nCount = m_pSheet->GetPageCount();
		for(int i = nCount-1; i >= 0; i--) 
		{

			CPropertyPage *pPage = m_pSheet->GetPage(i);
			if( pPage && pPage->m_hWnd )
				((CPage*)pPage)->m_site.FireOnOK();
		}
	}
}

void COptionsDlg::_Initial()
{
	m_pOutBarCtrl = 0;
	m_pOutBarCtrl = new CGfxOutBarCtrl();
	m_pOutBarCtrl->crBackGroundColor = ::GetValueColor(GetAppUnknown(), "\\Interface", "OptionsBarBackground", ::GetSysColor(COLOR_BACKGROUND));
	m_nActiveItem = 0;
	m_strActiveCategory = "";
//	m_pSheet = new CSheet();
	m_pSheet = 0;
	m_nActivePage = 0;

}

bool COptionsDlg::StoreOptionsConfig(CString strFileName)
{
	INamedDataPtr sptrND = 0;
	sptrIFileDataObject	sptrF = 0;
	/*
#ifdef _DEBUG
const char *szNamedDataProgID = "Data.NamedDataD";
#else
const char *szNamedDataProgID = "Data.NamedData";
#endif
	 */
	if(!strFileName.IsEmpty())
	{
		//create
		sptrF.CreateInstance( szNamedDataProgID );
		if( sptrF == 0 )
			return false;
		try
		{
			CFile	file(strFileName, CFile::modeCreate|CFile::modeNoTruncate);
		}
		catch(CFileException* e) 
		{
			e->Delete();
			return false;
		}
		if( FAILED(sptrF->LoadFile( _bstr_t(strFileName) )))
			return false;
		sptrND = sptrF;
	}
	else
		sptrND = GetAppUnknown();

	ASSERT(sptrND != 0);

	int i = 0;
	int nNumCat = m_pOutBarCtrl->GetItemCount();
	CString strCatName;
	CString strFormName;
	CString strEntry;
	long nImgIndex = 0;
	for(i = 0; i < nNumCat; i++)
	{
		strCatName = m_pOutBarCtrl->GetItemText(i);
		nImgIndex = m_pOutBarCtrl->GetItemImage(i);
		strEntry.Format("%s_%d_%s", "Category", i, "Name"); 
		::SetValue(sptrND, "OptionsDlgConfig", strEntry, _variant_t(strCatName));
		strEntry.Format("%s_%d_%s", "Category", i, "ImgIndex"); 
		::SetValue(sptrND, "OptionsDlgConfig", strEntry, _variant_t(nImgIndex));

/*
		POSITION pos = m_mapPages.GetStartPosition();
		int nIdx = 0;
		while(pos)
		{
			CObject* pPage;
			CString strMapEntry;
			m_mapPages.GetNextAssoc( pos, strMapEntry, pPage );
			strMapEntry = strMapEntry.Left(strMapEntry.Find("\n", 0));
			if(strMapEntry == strCatName && pPage)
			{
				strFormName = ::GetObjectName(((CPage*)pPage)->GetControllingUnknown());
				strEntry.Format("%s_%d_%s_%d_%s", "Category", i, "Form", nIdx, "Name"); 
				::SetValue(sptrND, "OptionsDlgConfig", strEntry, _variant_t(strFormName));
				nIdx++;
			}
		}
*/
		int nSz = m_mapPages.GetSize();
		int nIdx = 0;
		for( int i = 0; i < nSz; i++ )
		{
			CString strMapEntry = m_mapPages.GetKey( i );
			CObject* pPage   = m_mapPages.GetItem( i );
			strMapEntry = strMapEntry.Left(strMapEntry.Find("\n", 0));
			if(strMapEntry == strCatName && pPage)
			{
				strFormName = ::GetObjectName(((CPage*)pPage)->GetControllingUnknown());
				strEntry.Format("%s_%d_%s_%d_%s", "Category", i, "Form", nIdx, "Name"); 
				::SetValue(sptrND, "OptionsDlgConfig", strEntry, _variant_t(strFormName));
				nIdx++;
			}
		}

	}
	strEntry = "ActiveCategory";
	::SetValue(sptrND, "OptionsDlgConfig", strEntry, _variant_t(m_strActiveCategory));
	strEntry = "ActivePage"; 
	CString strActivePage;
	GetActivePage(strActivePage);
	::SetValue(sptrND, "OptionsDlgConfig", strEntry, _variant_t(strActivePage));

	if(sptrF != 0)
		if( sptrF->SaveFile( _bstr_t(strFileName) ) != S_OK )
			return false;


	CString strPath;
	if(strFileName.IsEmpty())
	{
		strPath = ::MakeAppPathName("optiondlg.img");
	}
	else
	{
		strPath = strFileName;
		_ChangeExt(strPath, "img");
	}
	CFile	file(strPath, CFile::modeCreate|CFile::modeWrite);
	CArchive ar(&file, CArchive::store);
	m_listLarge.Write(&ar);

	return true;
}

bool COptionsDlg::LoadOptionsConfig(CString strFileName)
{
	_try(COptionsDlg, LoadOptionsConfig)
	{
		ResetControls();
		INamedDataPtr sptrND = 0;
		sptrIFileDataObject	sptrF = 0;
		/*
#ifdef _DEBUG
const char *szNamedDataProgID = "Data.NamedDataD";
#else
const char *szNamedDataProgID = "Data.NamedData";
#endif
*/
		if(!strFileName.IsEmpty())
		{
			//create
			sptrF.CreateInstance( szNamedDataProgID );
			if( sptrF == 0 )
				return false;
			if( sptrF->LoadFile( _bstr_t(strFileName) ) != S_OK )
				return false;
			sptrND = sptrF;
		}
		else
			sptrND = GetAppUnknown();

		ASSERT(sptrND != 0);

		CString strPath;
		if(strFileName.IsEmpty())
		{
			strPath = ::MakeAppPathName("optiondlg.img");
		}
		else
		{
			strPath = strFileName;
			_ChangeExt(strPath, "img");
		}

		CFile	file(strPath, CFile::modeRead);
		CArchive ar(&file, CArchive::load);
		m_listLarge.Read(&ar);
		//m_listLarge.Create(32, 32, ILC_COLOR|ILC_MASK, 0, 1);
		int nImages = m_listLarge.GetImageCount();

		CString strEntry;
		CString strCatName;
		int nIdxCat = 0;
		while(true)
		{
			strEntry.Format("%s_%d_%s", "Category", nIdxCat, "Name"); 
			strCatName = ::GetValueString(sptrND, "OptionsDlgConfig", strEntry, "");
			if(strCatName.IsEmpty())
				break;
			strEntry.Format("%s_%d_%s", "Category", nIdxCat, "ImgIndex"); 
			int nImageIdx = ::GetValueInt(sptrND, "OptionsDlgConfig", strEntry, 0);
			AddItemToBar(nImageIdx, strCatName);

			//get pages to category
			int nIdxPage = 0;
			CString strFormName;
			while(true)
			{
				strEntry.Format("%s_%d_%s_%d_%s", "Category", nIdxCat, "Form", nIdxPage, "Name"); 
				strFormName = ::GetValueString(sptrND, "OptionsDlgConfig", strEntry, "");
				if(strFormName.IsEmpty())
					break;
				AddPageToCategory(strCatName, strFormName);
				nIdxPage++;
			}
			nIdxCat++;
		}
		strEntry = "ActiveCategory";
		SetActiveCategory(::GetValueString(sptrND, "OptionsDlgConfig", strEntry, ""));
		strEntry = "ActivePage"; 
		SetActivePage(::GetValueString(sptrND, "OptionsDlgConfig", strEntry, ""));
		return true;
	}
	_catch;
	
	return false;
}

void COptionsDlg::_ChangeExt(CString&  strPath, CString strExt)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(strPath, drive, dir, fname, ext);
	_makepath(strPath.GetBuffer(_MAX_PATH), drive, dir, fname, strExt);
	strPath.ReleaseBuffer();
}

BOOL COptionsDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return OnHelpHitTest( 0, 0 ) != -2;
}

LRESULT COptionsDlg::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	CPage* ppage = (CPage*)m_pSheet->GetActivePage();
	if( ppage )return ppage->m_site.SendMessage( WM_HELPHITTEST );
	return 0;
}

void COptionsDlg::OnHelp() 
{
	CPage* ppage = (CPage*)m_pSheet->GetActivePage();
	if( ppage )ppage->m_site.SendMessage( WM_HELPHITTEST );
}
