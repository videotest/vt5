// OutlookBar.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "OutlookBar.h"
#include "ShellToolBar.h"

/////////////////////////////////////////////////////////////////////////////
// COutlookBar
IMPLEMENT_DYNAMIC(COutlookBar, CGfxOutBarCtrl);

COutlookBar::COutlookBar()
{
	crBackGroundColor = GetSysColor(COLOR_3DDKSHADOW);//RGB(0,128,128);//
	crBackGroundColor1 = GetSysColor(COLOR_3DDKSHADOW);
}

COutlookBar::~COutlookBar()
{
	for (int t = 0; t < arFolder.GetSize(); t++)
	{
		CBarFolder	*pfb = (CBarFolder*)arFolder.GetAt( t );
		if( !pfb )continue;
		if( !pfb->pChild )continue;

		pfb->pChild->DestroyWindow();
		delete pfb->pChild;
	}
}


BEGIN_MESSAGE_MAP(COutlookBar, CGfxOutBarCtrl)
	//{{AFX_MSG_MAP(COutlookBar)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COutlookBar message handlers
CBCGToolBar *COutlookBar::AddFolderBar( const char *szName )
{
	CShellOutlookToolBar	*pbar1 = new CShellOutlookToolBar;
	pbar1->Create( this, WS_CHILD|WS_VISIBLE, GetFolderCount()+100 );
	int nPos = CGfxOutBarCtrl::AddFolderBar( szName, pbar1 );
	pbar1->SetWindowText( szName );
	pbar1->SetOwner( AfxGetMainWnd() );

	SetSelFolder( nPos );
	return pbar1;
}


void COutlookBar::DeleteFolderBar( CBCGToolBar *pbar )
{
	for (int t = 0; t < arFolder.GetSize(); t++)
	{
		CBarFolder	*pfb = (CBarFolder*)arFolder.GetAt( t );
		if( !pfb )continue;
		if( pfb->pChild != pbar )continue;

		pfb->pChild->DestroyWindow();
		delete pfb->pChild;
		pfb->pChild = 0;

		RemoveFolder( t );
		return;
	}
}

void COutlookBar::DeleteAllFolderBars()
{
	for (int t = (int)arFolder.GetSize()-1; t >=0 ; t-- )
	{
		CBarFolder	*pfb = (CBarFolder*)arFolder.GetAt( t );
		if( pfb->pChild )
		{
			pfb->pChild->DestroyWindow();
			delete pfb->pChild;
			pfb->pChild = 0;
		}
		RemoveFolder( t );
	}
}

CString	COutlookBar::GetFolderName( int nFolder )
{
	CBarFolder	*pfb = (CBarFolder*)arFolder.GetAt( nFolder );
	return pfb->cName;
}

CWnd	*COutlookBar::GetFolderWindow( int nFolder )
{
	CBarFolder	*pfb = (CBarFolder*)arFolder.GetAt( nFolder );
	return pfb->pChild;
}


int COutlookBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CGfxOutBarCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void COutlookBar::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CGfxOutBarCtrl::OnShowWindow(bShow, nStatus);

	if( bShow )
	{
		CRect	rect;
		GetWindowRect( &rect );
		MoveWindow( rect );
	}
	
	/*if( bShow  &&  iSelFolder != -1 && GetFolderCount() )
	{

		CWnd * pc = GetFolderChild();
		if (pc) pc->ShowWindow(SW_SHOW);	
		SetSelFolder( iSelFolder );
	}*/
}

void COutlookBar::OnSize(UINT nType, int cx, int cy) 
{
	CGfxOutBarCtrl::OnSize(nType, cx, cy);

	if( iSelFolder != -1 && GetFolderCount() )
	{
		CWnd * pc = GetFolderChild();
		if (pc)
		{
			CRect rc;
			GetInsideRect(rc);
			pc->MoveWindow(rc);
			pc->ShowWindow(SW_SHOW);
		}		
	}
}

void COutlookBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CWnd * pc = GetFolderChild();
	if (pc)pc->SendMessage( WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0 );

}