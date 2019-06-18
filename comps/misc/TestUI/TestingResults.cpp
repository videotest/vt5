// TestingResults.cpp : implementation file
//

#include "stdafx.h"
#include "TestUI.h"
#include "TestingResults.h"
#include "TestUIUtils.h"
#include "CompareImagesDlg.h"
#include "CompareValuesDlg.h"


// CTestingResults dialog

IMPLEMENT_DYNAMIC(CTestingResults, CDialog)
CTestingResults::CTestingResults(CWnd* pParent /*=NULL*/)
	: CDialog(CTestingResults::IDD, pParent)
{
	m_nSelectedItem = -1;
}

CTestingResults::~CTestingResults()
{
}

void CTestingResults::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TESTS_RESULTS, m_Results);
	DDX_Control(pDX, IDC_COMBO_FILTRATION, m_ComboFilter);
}

void CTestingResults::InitializeList()
{
	// Initialize columns
	CString sName,sState,sDev;
	sName.LoadString(IDS_TEST_NAME);
	m_Results.InsertColumn(0, sName, LVCFMT_LEFT, 200, 1);
	sState.LoadString(IDS_TESTING_STATE);
	m_Results.InsertColumn(1, sState, LVCFMT_LEFT, 200, 2);
	sState.LoadString(IDS_TESTING_DEVIATION);
	m_Results.InsertColumn(2, sState, LVCFMT_LEFT, 200, 2);
	// Initialize rows
	InitRows(0);
}

void CTestingResults::InitRows(int nFilter)
{
	IUnknownPtr punkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
	ITestManErrorDescrPtr sptrTMED(punkTM);
	if (sptrTMED)
	{
		long lPos;
		int iItem = 0;
		sptrTMED->GetFirstErrorPos(&lPos);
		while (lPos)
		{
			TEST_ERR_DESCR *pDescr;
			sptrTMED->GetNextError(&lPos, &pDescr);
			if (nFilter == 1)
			{
				if (!pDescr->bTestError)
					continue;
			}
			else if (nFilter == 2 || nFilter == 3)
			{
				if (pDescr->bTestError)
					continue;
				if (nFilter == 2)
				{
					if (pDescr->dwDiffTime > 0)
						continue;
				}
				else
				{
					if (pDescr->dwDiffTime < 0)
						continue;
				}
			}
			CString sFullName(_TestFullNameFromPath(CString(pDescr->bstrPath),pDescr->nTryIndex));
			m_Results.InsertItem(iItem, sFullName);
			CTestInfoRec rec(sFullName, 0, pDescr->nTryIndex, true, !pDescr->bTestError,
				pDescr->dwDiffTime);
			rec.m_pErrDescr = pDescr;
			m_arrTests.Add(rec);
			iItem++;
		}
	}
}

BEGIN_MESSAGE_MAP(CTestingResults, CDialog)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_COMBO_FILTRATION, OnCbnSelchangeComboFiltration)
//	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickListTestsResults)
ON_NOTIFY(NM_RCLICK, IDC_LIST_TESTS_RESULTS, OnNMRclickListTestsResults)
ON_COMMAND(ID_EXPORT_TEST, OnExportTest)
ON_COMMAND(ID_COMPARE_VALUES, OnCompareValues)
ON_COMMAND(ID_COMPARE_WITH_SAMPLE, OnCompareWithSample)
//ON_UPDATE_COMMAND_UI(ID_COMPARE_VALUES, OnUpdateCompareValues)
//ON_UPDATE_COMMAND_UI(ID_COMPARE_WITH_SAMPLE, OnUpdateCompareWithSample)
END_MESSAGE_MAP()


// CTestingResults message handlers

BOOL CTestingResults::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitializeList();
	m_ComboFilter.SetCurSel(0);
	m_bmpExport.LoadBitmap(IDB_EXPORT);
	m_bmpCmpWithSmp.LoadBitmap(IDB_COMPARE_WITH_SAMPLE);
	m_bmpCmpVals.LoadBitmap(IDB_COMPARE_VALUES);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTestingResults::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_LIST_TESTS_RESULTS)
	{
		CDC *pdc = CDC::FromHandle(lpDrawItemStruct->hDC);
		COLORREF crBackground = GetSysColor(lpDrawItemStruct->itemState&ODS_SELECTED?COLOR_HIGHLIGHT:COLOR_WINDOW);
		COLORREF crText = GetSysColor(lpDrawItemStruct->itemState&ODS_SELECTED?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT);
		pdc->SetBkColor(crBackground);
		pdc->SetTextColor(crText);
		pdc->FillSolidRect(&lpDrawItemStruct->rcItem, crBackground);
		int iItem = lpDrawItemStruct->itemID;
		CRect rcItemLabel,rcState,rcDeviation;
		if (m_Results.GetItemRect(iItem, rcItemLabel, LVIR_LABEL))
		{
			CString s = m_Results.GetItemText(iItem, 0);
			pdc->DrawText(s, rcItemLabel, 0);
		}
		CTestInfoRec *p = &m_arrTests.ElementAt(iItem);
		if (m_Results.GetSubItemRect(iItem,1,LVIR_BOUNDS,rcState))
		{
			rcState.InflateRect(-1,-1,-1,-1);
			if (!p->m_bPassed)
			{
				CBrush br(RGB(0,0,0));
				pdc->FrameRect(rcState, &br);
			}
			else if (p->m_bSuccess)
				pdc->FillSolidRect(rcState, p->m_nDeviation==0?RGB(0,255,0):p->m_nDeviation<0?RGB(255,255,0):RGB(0,255,255));
			else
				pdc->FillSolidRect(rcState, RGB(255,0,0));
		}
		if (m_Results.GetSubItemRect(iItem,2,LVIR_LABEL,rcDeviation))
		{
			CString s;
			s.Format(_T("%d ms"), p->m_nDeviation);
			pdc->DrawText(s, rcDeviation, 0);
		}
	}
	else
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CTestingResults::OnCbnSelchangeComboFiltration()
{
	m_Results.DeleteAllItems();
	m_arrTests.RemoveAll();
	InitRows(m_ComboFilter.GetCurSel());
}

void CTestingResults::OnNMRclickListTestsResults(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE *nmia = (NMITEMACTIVATE *)pNMHDR;
	m_nSelectedItem = nmia->iItem;
	CMenu menu;
	menu.LoadMenu(IDR_MENU_TESTING_RESULTS);
	CMenu *pPopup = menu.GetSubMenu(0);
	pPopup->SetMenuItemBitmaps(ID_EXPORT_TEST, MF_BYCOMMAND, &m_bmpExport, &m_bmpExport);
	pPopup->SetMenuItemBitmaps(ID_COMPARE_WITH_SAMPLE, MF_BYCOMMAND, &m_bmpCmpWithSmp, &m_bmpCmpWithSmp);
	pPopup->SetMenuItemBitmaps(ID_COMPARE_VALUES, MF_BYCOMMAND, &m_bmpCmpVals, &m_bmpCmpVals);

	if( m_nSelectedItem == -1 )
	{
		::EnableMenuItem( pPopup->GetSafeHmenu(), ID_EXPORT_TEST, MF_BYCOMMAND | MF_GRAYED );
		::EnableMenuItem( pPopup->GetSafeHmenu(), ID_COMPARE_WITH_SAMPLE, MF_BYCOMMAND |MF_GRAYED );
		::EnableMenuItem( pPopup->GetSafeHmenu(), ID_COMPARE_VALUES, MF_BYCOMMAND | MF_GRAYED );
	}
	else
	{
		CTestInfoRec *p = &m_arrTests.ElementAt( m_nSelectedItem );
		if( p && p->m_pErrDescr && p->m_pErrDescr->lszArrays == 0 )
			::EnableMenuItem( pPopup->GetSafeHmenu(), ID_COMPARE_VALUES, MF_BYCOMMAND | MF_GRAYED );
		if( p && p->m_pErrDescr && p->m_pErrDescr->lszImages == 0 )
			::EnableMenuItem( pPopup->GetSafeHmenu(), ID_COMPARE_WITH_SAMPLE, MF_BYCOMMAND | MF_GRAYED );
	}

	CPoint pt;
	GetCursorPos(&pt);
	pPopup->TrackPopupMenu(0, pt.x, pt.y, this);
	*pResult = 0;
}

//////////////////////////////////////////// for select folder 
static int CALLBACK	BrowseCallbackProc (HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    TCHAR szPath[_MAX_PATH];
    switch (uMsg) {
    case BFFM_INITIALIZED:
        if (lpData)
            SendMessage(hWnd,BFFM_SETSELECTION,TRUE,lpData);
        break;
    case BFFM_SELCHANGED:
        SHGetPathFromIDList(LPITEMIDLIST(lParam),szPath);
        SendMessage(hWnd, BFFM_SETSTATUSTEXT, NULL, LPARAM(szPath));
        break;
    }
    return 0;
}

static BOOL GetFolder (LPCTSTR szTitle, LPTSTR szPath, LPCTSTR szRoot, HWND hWndOwner)
{
    if (szPath == NULL)
        return false;

    bool result = false;

    LPMALLOC pMalloc;
    if (::SHGetMalloc(&pMalloc) == NOERROR) {
		BROWSEINFO bi = {0};
        
		bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;

        // дескриптор окна-владельца диалога
        bi.hwndOwner = hWndOwner;

        // добавление заголовка к диалогу
        bi.lpszTitle = szTitle;

        // отображение текущего каталога
        bi.lpfn      = BrowseCallbackProc;
        //bi.ulFlags  |= BIF_STATUSTEXT;

        // установка каталога по умолчанию
        bi.lParam    = LPARAM(szPath);

        // установка корневого каталога
        if (szRoot != NULL) {
            IShellFolder *pDF;
            if (SHGetDesktopFolder(&pDF) == NOERROR) {
                LPITEMIDLIST pIdl = NULL;
                ULONG        chEaten;
                ULONG        dwAttributes;

                USES_CONVERSION;
                LPOLESTR oleStr = T2OLE(szRoot);

                pDF->ParseDisplayName(NULL,NULL,oleStr,&chEaten,&pIdl,&dwAttributes);
                pDF->Release();

                bi.pidlRoot = pIdl;
            }
        }	

        LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
        if (pidl != NULL) {
            if (::SHGetPathFromIDList(pidl,szPath))
                result = true;
            pMalloc->Free(pidl);
        }
        if (bi.pidlRoot != NULL)
            pMalloc->Free((void*)bi.pidlRoot);
        pMalloc->Release();
    }
    return result;
}
//////////////////////////////////////////////////////////////////

static inline CString NumberedName(CString strName, int nIndex)
{
	if(nIndex==0) return strName;
	CString s;
	s.Format("%d",nIndex);
	return strName + "." + s;
}

static inline CString IndexedName(CString strName, CString strIndex)
{
	int n = strName.ReverseFind('.');
	if(n<0) n = strName.GetLength();
	CString s = strName;
	s.Insert(n, strIndex);
	return s;
}

static bool CopyNumberedFile(CString strFileName, CString strOldPath, CString strOldIndex, CString strNewPath, CString strNewIndex)
{
	if((!strOldPath.IsEmpty()) && strOldPath.Right(1)!="\\")
		strOldPath += "\\";
	if((!strNewPath.IsEmpty()) && strNewPath.Right(1)!="\\")
		strNewPath += "\\";

	CString strOld = IndexedName(strOldPath+strFileName, strOldIndex);
	CString strNew = IndexedName(strNewPath+strFileName, strNewIndex);
	
	{
		CFileFind ff;
		if( ff.FindFile( strNew ) )
		{
			CString str;
			str.Format( (UINT)IDS_FILE_EXIST, strNew );
			AfxMessageBox( str, MB_ICONERROR );
			return false;
		}
	}
	CopyFile(strOld, strNew, FALSE);
	return true;
}

bool file_exist( const char *psz )
{
	HANDLE	h = ::CreateFile( psz, 0, 0, 0, OPEN_EXISTING, 0, 0 );
	if( h == INVALID_HANDLE_VALUE )
		return false;

	::CloseHandle( h );

	return true;
}
void _copy_files( CString sz_path_from, CString sz_path_to, bool bcopy = 1 )
{
	::CreateDirectory( sz_path_from, 0 );
	::CreateDirectory( sz_path_to,   0 );
//	_trace_file( 0, "[~] Begin copy_file" );

	WIN32_FIND_DATA fdata = {0};

	HANDLE hfind = 0;

	char sz_cur_dir[_MAX_PATH];
	::GetCurrentDirectory( _MAX_PATH, sz_cur_dir );

	if( !SetCurrentDirectory( sz_path_from ) )
		return;

	hfind = FindFirstFile( "*.*", &fdata );

	if( hfind == INVALID_HANDLE_VALUE )
		return;

	do
	{
		if( !( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			if( _tcscmp( fdata.cFileName, ".." ) && _tcscmp( fdata.cFileName, "." ) )
			{
				char sz_from[_MAX_PATH], sz_to[_MAX_PATH];

				_tcsncpy( sz_from, sz_path_from, sizeof( sz_from ) );

				if( *( sz_from + _tcslen( sz_from ) - 1 ) != '\\' )
					_tcsncat( sz_from, "\\", sizeof( sz_from ) );

				_tcsncat( sz_from, fdata.cFileName, sizeof( sz_from ) );

				_tcsncpy( sz_to, sz_path_to, sizeof( sz_to ) );
				
				if( *( sz_to + _tcslen( sz_to ) - 1 ) != '\\' )
					_tcsncat( sz_to, "\\", sizeof( sz_to ) );

				_tcsncat( sz_to, fdata.cFileName, sizeof( sz_to ) );

				if( file_exist( sz_from ) )
				{
					if( file_exist( sz_to ) )
						::DeleteFile( sz_to );

					if( bcopy )
					{
				//		_trace_file( 0, "[~O] Copy file from %s to %s", sz_from, sz_to );
						::CopyFile(	sz_from, sz_to, false );
					}
					else
					{
					//	_trace_file( 0, "[~] Move file from %s to %s", sz_from, sz_to );
						::MoveFile(	sz_from, sz_to );
					}
				}

			}
		}
	} while( FindNextFile( hfind, &fdata ) );

	FindClose( hfind );


	if( !SetCurrentDirectory( sz_cur_dir ) )
		return;
}
void CTestingResults::OnExportTest()
{
	if (m_nSelectedItem >= 0 && m_nSelectedItem < m_arrTests.GetSize())
	{
		CWnd *pmainwnd = 0;
		pmainwnd = this;
		CString strNewPath = ".";
		CString strTitle;
		strTitle.LoadString(IDS_SELECTDIRFOREXPORT);
		if( !GetFolder( strTitle, strNewPath.GetBuffer( MAX_PATH ), ".", pmainwnd ? pmainwnd->m_hWnd : 0 ) )
			strNewPath.Empty( );
		strNewPath.ReleaseBuffer( );

		if(!strNewPath.IsEmpty())
		{
			CTestInfoRec *p = &m_arrTests.ElementAt(m_nSelectedItem);
			CString strOldPath = CString(p->m_pErrDescr->bstrPath);
			int nTryIndex = p->m_pErrDescr->nTryIndex;
			CString strIndex="";
			if( 0 != nTryIndex ) strIndex.Format( ".%d", nTryIndex );
			strIndex = strIndex + "_" + ::GetValueString( GetAppUnknown(), "\\General", "Language", "en" );

			/*
			CopyNumberedFile(CString("SDS")+strIndex+".data", strOldPath, strIndex, strNewPath, "");
			CopyNumberedFile(CString("TDC")+strIndex+".image", strOldPath, strIndex, strNewPath, "");
			CopyNumberedFile(CString("TDS")+strIndex+".image", strOldPath, strIndex, strNewPath, "");
			*/
			_copy_files( strOldPath, strNewPath ); 
		}
	}
	m_nSelectedItem = -1;
}

void CTestingResults::OnCompareValues()
{
	if (m_nSelectedItem >= 0 && m_nSelectedItem < m_arrTests.GetSize())
	{
		CCompareValuesDlg dlg(this);
		CTestInfoRec *p = &m_arrTests.ElementAt( m_nSelectedItem );
		dlg.SetTestParam( p->m_pErrDescr );
		dlg.DoModal();
	}
	m_nSelectedItem = -1;
}

void CTestingResults::OnCompareWithSample()
{
	if (m_nSelectedItem >= 0 && m_nSelectedItem < m_arrTests.GetSize())
	{
		CCompareImagesDlg dlg;

		CTestInfoRec *p = &m_arrTests.ElementAt( m_nSelectedItem );
		CString strOldPath = CString(p->m_pErrDescr->bstrPath);
		int nTryIndex = p->m_pErrDescr->nTryIndex;

		dlg.set_test_param( p->m_pErrDescr );

		dlg.do_modal( m_hWnd );
	}
	m_nSelectedItem = -1;
}

