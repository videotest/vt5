//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************
//
// This code is based on Ivan Zhakov (vanya@mail.axon.ru)'s
// MDI Windows Manager dialog 
// http://codeguru.developer.com/doc_view/WindowsManager.shtml
//

// BCGWindowsManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "BCGMDIFrameWnd.h"
#include "BCGWindowsManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGWindowsManagerDlg dialog


CBCGWindowsManagerDlg::CBCGWindowsManagerDlg(CBCGMDIFrameWnd* pMDIFrame)
	: CDialog(CBCGWindowsManagerDlg::IDD, pMDIFrame),
	m_pMDIFrame (pMDIFrame)
{
	//{{AFX_DATA_INIT(CBCGWindowsManagerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT_VALID (m_pMDIFrame);
}


void CBCGWindowsManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCGWindowsManagerDlg)
	DDX_Control(pDX, IDC_BCGBARRES_LIST, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCGWindowsManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CBCGWindowsManagerDlg)
	ON_BN_CLICKED(IDC_BCGBARRES_ACTIVATE, OnActivate)
	ON_BN_CLICKED(IDC_BCGBARRES_SAVE, OnSave)
	ON_BN_CLICKED(IDC_BCGBARRES_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_BCGBARRES_CASCADE, OnCascade)
	ON_BN_CLICKED(IDC_BCGBARRES_TILEHORZ, OnTilehorz)
	ON_BN_CLICKED(IDC_BCGBARRES_TILEVERT, OnTilevert)
	ON_BN_CLICKED(IDC_BCGBARRES_MINIMIZE, OnMinimize)
	ON_LBN_SELCHANGE(IDC_BCGBARRES_LIST, OnSelchangeBcgbarresList)
	ON_WM_DRAWITEM()
	ON_LBN_DBLCLK(IDC_BCGBARRES_LIST, OnActivate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGWindowsManagerDlg message handlers

void CBCGWindowsManagerDlg::MDIMessage (UINT uMsg, WPARAM flag)
{
	CWaitCursor wait;

	int nItems = m_wndList.GetCount();

	if(nItems!=LB_ERR && nItems>0)
	{
		HWND	hMDIClient=m_pMDIFrame->m_hWndMDIClient;
		::LockWindowUpdate(hMDIClient);
		for(int i=nItems-1;i>=0;i--)
		{
			HWND hWnd=(HWND) m_wndList.GetItemData(i);
			if(m_wndList.GetSel(i)>0)	::ShowWindow(hWnd,SW_RESTORE);
			else						::ShowWindow(hWnd,SW_MINIMIZE);
		}
		::SendMessage(hMDIClient,uMsg, flag,0);	
		::LockWindowUpdate(NULL);
	}
}

void CBCGWindowsManagerDlg::OnActivate() 
{
	if(m_wndList.GetSelCount()==1)
	{
		int		index;	
		if(m_wndList.GetSelItems(1,&index)==1)
		{						 
			DWORD_PTR dw=m_wndList.GetItemData(index);
			if(dw!=LB_ERR)
			{
				WINDOWPLACEMENT	wndpl;
				wndpl.length = sizeof(WINDOWPLACEMENT);
				::GetWindowPlacement((HWND) dw,&wndpl);

				if(wndpl.showCmd == SW_SHOWMINIMIZED) ::ShowWindow((HWND) dw,SW_RESTORE);
				::SendMessage(m_pMDIFrame->m_hWndMDIClient,WM_MDIACTIVATE,(WPARAM) dw,0);
				EndDialog(IDOK);
			}
		}
	}
}

void CBCGWindowsManagerDlg::OnSave() 
{
	int		nItems=m_wndList.GetCount();
	if(nItems!=LB_ERR && nItems>0)
	{
		for(int i=0;i<nItems;i++)
		{
			if(m_wndList.GetSel(i)>0)
			{
				HWND		hWnd=(HWND) m_wndList.GetItemData(i);
				CWnd		*pWnd=CWnd::FromHandle(hWnd);
				CFrameWnd	*pFrame=DYNAMIC_DOWNCAST(CFrameWnd,pWnd);
				if(pFrame != NULL)
				{
					CDocument *pDoc=pFrame->GetActiveDocument();
					if(pDoc != NULL)	pDoc->SaveModified();
				}
			}
		}
	}

	FillWindowList();
	SelActive();
	UpdateButtons();
}

void CBCGWindowsManagerDlg::OnClose() 
{
	int		nItems=m_wndList.GetCount();
	if(nItems!=LB_ERR && nItems>0)
	{
		HWND	hMDIClient=m_pMDIFrame->m_hWndMDIClient;
		
		m_wndList.SetRedraw(FALSE);

		for(int i=nItems-1;i>=0;i--)
		{
			if(m_wndList.GetSel(i)>0)
			{					   
				HWND hWnd=(HWND) m_wndList.GetItemData(i);
				::SendMessage(hWnd,WM_CLOSE,(WPARAM) 0, (LPARAM) 0);
				if(::GetParent(hWnd)==hMDIClient) break;
			}				  
		}
		m_wndList.SetRedraw(TRUE);
	}

	FillWindowList();
	SelActive();
	UpdateButtons();
}

void CBCGWindowsManagerDlg::OnCascade() 
{
	MDIMessage(WM_MDICASCADE,0);
}

void CBCGWindowsManagerDlg::OnTilehorz() 
{
	MDIMessage(WM_MDITILE,MDITILE_HORIZONTAL);	
}

void CBCGWindowsManagerDlg::OnTilevert() 
{
	MDIMessage(WM_MDITILE,MDITILE_VERTICAL);	
}

void CBCGWindowsManagerDlg::OnMinimize() 
{
	int		nItems=m_wndList.GetCount();
	if(nItems!=LB_ERR && nItems>0)
	{
		m_wndList.SetRedraw(FALSE);

		for(int i=nItems-1;i>=0;i--)
		{
			if(m_wndList.GetSel(i)>0)
			{
				HWND hWnd=(HWND) m_wndList.GetItemData(i);
				::ShowWindow(hWnd,SW_MINIMIZE);
			}
		}
		m_wndList.SetRedraw(TRUE);
	}

	FillWindowList();
	SelActive();
	UpdateButtons();
}

BOOL CBCGWindowsManagerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillWindowList(); 	
	SelActive();
	UpdateButtons();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBCGWindowsManagerDlg::OnSelchangeBcgbarresList() 
{
	UpdateButtons();
}

// Enables/Disables states of buttons
void CBCGWindowsManagerDlg::UpdateButtons()
{						   
	int	nSel=m_wndList.GetSelCount();

	GetDlgItem(IDC_BCGBARRES_CLOSE)->EnableWindow(nSel>0);	
	GetDlgItem(IDC_BCGBARRES_SAVE)->EnableWindow(nSel>0);
	GetDlgItem(IDC_BCGBARRES_TILEHORZ)->EnableWindow(nSel>=2);
	GetDlgItem(IDC_BCGBARRES_TILEVERT)->EnableWindow(nSel>=2);
	GetDlgItem(IDC_BCGBARRES_CASCADE)->EnableWindow(nSel>=2);
	GetDlgItem(IDC_BCGBARRES_MINIMIZE)->EnableWindow(nSel>0);

	GetDlgItem(IDC_BCGBARRES_ACTIVATE)->EnableWindow(nSel==1);
}

// Selects currently active window in listbox
void CBCGWindowsManagerDlg::SelActive()
{
	int		nItems=m_wndList.GetCount();
	if(nItems != LB_ERR && nItems>0)
	{
	
		m_wndList.SetRedraw(FALSE);
		m_wndList.SelItemRange(FALSE,0,nItems-1);
		
		HWND	hwndActive=(HWND) ::SendMessage(m_pMDIFrame->m_hWndMDIClient,WM_MDIGETACTIVE,0,0);
	
		for(int i=0;i<nItems;i++) {
			if((HWND) m_wndList.GetItemData(i)==hwndActive)  {
				m_wndList.SetSel(i);
				break;
			}
		}
		m_wndList.SetRedraw(TRUE);
	}
}

// Refresh windows list
void CBCGWindowsManagerDlg::FillWindowList(void)
{
	m_wndList.SetRedraw(FALSE);
	m_wndList.ResetContent();
	HWND hwndT;
	hwndT=::GetWindow(m_pMDIFrame->m_hWndMDIClient, GW_CHILD);
	while (hwndT != NULL)
	{
		TCHAR	szWndTitle[256];
		::GetWindowText(hwndT,szWndTitle,sizeof(szWndTitle)/sizeof(szWndTitle[0]));

		int index=m_wndList.AddString(szWndTitle);
		m_wndList.SetItemData(index,(DWORD) hwndT);
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
	}
	m_wndList.SetRedraw(TRUE);
}


void CBCGWindowsManagerDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS) 
{
	if(nIDCtl == IDC_BCGBARRES_LIST)
	{
		if (lpDIS->itemID == LB_ERR) return; 
  
		HBRUSH		brBackground;
		RECT		rcTemp = lpDIS->rcItem; 
		HDC			hDC=lpDIS->hDC;

		COLORREF	clText; 

		if (lpDIS->itemState & ODS_SELECTED)  { 
			brBackground = GetSysColorBrush (COLOR_HIGHLIGHT); 
			clText = GetSysColor (COLOR_HIGHLIGHTTEXT); 
		} else { 
			brBackground = GetSysColorBrush (COLOR_WINDOW); 
			clText = GetSysColor (COLOR_WINDOWTEXT); 
		} 

		if (lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) 	FillRect(hDC,&rcTemp, brBackground); 
	
		int			OldBkMode=::SetBkMode(hDC,TRANSPARENT); 
		COLORREF	clOldText=::SetTextColor(hDC,clText); 

		TCHAR		szBuf[1024];
		::SendMessage(lpDIS->hwndItem,LB_GETTEXT,(WPARAM) lpDIS->itemID,(LPARAM) szBuf);		
		
		int h=rcTemp.bottom-rcTemp.top;
		rcTemp.left+=h+4;
		DrawText(hDC,szBuf,-1,&rcTemp,DT_LEFT|DT_VCENTER|	
			DT_NOPREFIX| DT_SINGLELINE);

		HICON	hIcon=(HICON) ::GetClassLongPtr((HWND) lpDIS->itemData,GCLP_HICONSM);/*AfxGetApp()->LoadStandardIcon(IDI_HAND);*///(HICON) ::SendMessage((HWND) lpDIS->itemData,WM_GETICON,(WPARAM)ICON_BIG,(LPARAM) 0);
		rcTemp.left=lpDIS->rcItem.left;
		::DrawIconEx(hDC,rcTemp.left+2,rcTemp.top,			
			hIcon,h,h,0,0,DI_NORMAL);		
		
		::SetTextColor(hDC,clOldText);
		::SetBkMode(hDC,OldBkMode);
  
		if(lpDIS->itemAction & ODA_FOCUS)   DrawFocusRect(hDC,&lpDIS->rcItem); 
		return;		
	}
	CDialog::OnDrawItem(nIDCtl, lpDIS);
}
