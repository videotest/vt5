// SimpleFieldPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SimpleFieldPage.h"
#include "dbasedoc.h"
#include "wizards.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleFieldPage property page

IMPLEMENT_DYNCREATE(CSimpleFieldPage, CPropertyPage)

CSimpleFieldPage::CSimpleFieldPage() : CPropertyPage(CSimpleFieldPage::IDD)
{
	m_pcreate = 0;
	m_count = 0;
//	m_nExpandY = 0;
	//{{AFX_DATA_INIT(CSimpleFieldPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSimpleFieldPage::~CSimpleFieldPage()
{
	while( m_pcreate )
	{
		field_create_info	*p = m_pcreate;
		m_pcreate = m_pcreate->next;
		delete p;
	}
}

void CSimpleFieldPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimpleFieldPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimpleFieldPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSimpleFieldPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleFieldPage message handlers
#define IDC_EDITOR_BASE	1001


BOOL CSimpleFieldPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	char	sz_filename[MAX_PATH];
	GetModuleFileName( 0, sz_filename, sizeof( sz_filename ) );

	strcpy( strrchr( sz_filename, '\\' ), "\\createfld.dat" );

	CString	strPath  = ::GetValueString( GetAppUnknown(), "\\Database", "SimplePageTemplate", sz_filename );

	FILE	*pfile = ::fopen( strPath, "rt" );
	if( pfile )
	{
		while( !feof( pfile ) )
		{
			field_create_info	*pfc = new field_create_info;
			char	sz[1000];
			::fgets( sz, sizeof( sz ), pfile );
			char	*p = ::strtok( sz, ",\n" ); 
			if( !p )
			{
				delete pfc;
				continue;
			}
			strcpy( pfc->sz_fldbase, p );
			p = strtok( 0, ",\n" );

			if( !p )
			{
				delete pfc;
				continue;
			}

			if( !stricmp( p, "ftDigit" ) )pfc->type = ftDigit;
			else if( !stricmp( p, "ftString" ) )pfc->type = ftString;
			else if( !stricmp( p, "ftDateTime" ) )pfc->type = ftDateTime;
			else if( !stricmp( p, "ftVTObject" ) )pfc->type = ftVTObject;
			else
			{
				delete pfc;
				continue;
			}

			p = strtok( 0, ",\n" );

			if( !p )
			{
				delete pfc;
				continue;
			}

			strcpy( pfc->sz_caption, p );

			p = strtok( 0, ",\n" );

			if( !p )
			{
				delete pfc;
				continue;
			}

			if( sscanf( p, "%d", &pfc->count ) != 1 )
			{
				delete pfc;
				continue;
			}

			if( pfc->type == ftVTObject )
			{
				p = strtok( 0, ",\n" );

				if( !p )
				{
					delete pfc;
					continue;
				}

				strcpy( pfc->sz_objtype, p );
			}

			pfc->next = m_pcreate;
			m_pcreate = pfc;
			m_count++;
		}
		::fclose( pfile );
	}

	HWND	hwnd=m_hWnd;

	HWND	hwnd_edit = ::GetDlgItem( hwnd, IDC_TEST2 ),
			hwnd_text = ::GetDlgItem( hwnd, IDC_TEST1 ),
			hwnd_static = ::GetDlgItem( hwnd, IDC_AREA );


	RECT	rect_edit, rect_text;
	::GetWindowRect( hwnd_edit, &rect_edit );
	::GetWindowRect( hwnd_text, &rect_text );

	RECT	rect_static;
	::GetWindowRect( hwnd_static, &rect_static );

	int	xofs_text = rect_text.left-rect_static.left;
	int xofs_edit = rect_edit.left-rect_static.left;
	int	yofs_text = rect_text.top-rect_static.top;
	int yofs_edit = rect_edit.top-rect_static.top;
	int	w_text = rect_text.right-rect_text.left;
	int	w_edit = rect_edit.right-rect_edit.left;
	int	h_text = rect_text.bottom-rect_text.top;
	int h_edit = rect_edit.bottom-rect_edit.top;

	::ShowWindow( hwnd_edit, SW_HIDE );
	::ShowWindow( hwnd_text, SW_HIDE );

	//read info here//count

	::ScreenToClient( hwnd, (POINT*)&rect_static );
	::ScreenToClient( hwnd, (POINT*)&rect_static+1 );

	int	x = rect_static.left;
	int	y = rect_static.top;
	
	HFONT	hFont = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );
//	int nTextWidth = ::GetValueInt(GetAppUnknown(), "\\Database\\SimpleFieldPage", "TextWidth", 0);
	// Проверяем, чтобы все надписи поместились. Если что-либо не помещается, расширяем ширину
	// поля текста.
	int nTextWidth = 0;
	{
//		::SendMessage( hwnd_text, WM_SETFONT, (WPARAM)hFont, 0 );
		CClientDC ClientDC(CWnd::FromHandle(hwnd_text));
		::SelectObject(ClientDC.m_hDC, hFont);
		for( field_create_info *pfc = m_pcreate; pfc; pfc = pfc->next)
		{
			CRect rc(0,0,0,0);
			ClientDC.DrawText(pfc->sz_caption, -1, &rc, DT_SINGLELINE|DT_CALCRECT);
			if (rc.Width() > nTextWidth)
				nTextWidth = rc.Width();
		}
	}
	// Проверяем, чтобы поле редактирования справа от поля текста поместилось в рамку.
	// Если не помещается, придется уменьшить ширину текстового поля.
	if (2*(xofs_edit+nTextWidth-w_text+w_edit) > rect_static.right-rect_static.left-xofs_text)
		nTextWidth = (rect_static.right-rect_static.left-xofs_text)/2-(xofs_edit-w_text+w_edit);
	if (nTextWidth > w_text)
	{
		xofs_edit += nTextWidth-w_text;
		w_text = nTextWidth;
	}

	int	idx = 0;
	for( field_create_info *pfc = m_pcreate; pfc; pfc = pfc->next, idx++ )
	{
		//это читаем из файла
		char	sz_text[100] = "text";
		strcpy( sz_text, pfc->sz_caption );
		int	def_counter = pfc->count;
		char	sz_count[20];
		sprintf( sz_count, "%d", def_counter );

		//а это уже навсегда
		if( x+xofs_edit+w_edit > rect_static.right-xofs_text )
		{
			x = rect_static.left;
			y += yofs_edit + h_edit;
		}

		if( y + yofs_edit + h_edit > rect_static.bottom-yofs_edit )
			break;


		rect_text.left = x + xofs_text;
		rect_text.right = rect_text.left + w_text;
		rect_text.top = y + yofs_text;
		rect_text.bottom = rect_text.top + h_text;

		rect_edit.left = x + xofs_edit;
		rect_edit.right = rect_edit.left + w_edit;
		rect_edit.top = y + yofs_edit;
		rect_edit.bottom = rect_edit.top + h_edit;

		hwnd_text = ::CreateWindow( "static", sz_text, WS_CHILD|WS_VISIBLE, 
			rect_text.left, rect_text.top, rect_text.right-rect_text.left, rect_text.bottom - rect_text.top,
			hwnd, (HMENU)-1, AfxGetInstanceHandle(), 0 );
		
		hwnd_edit = ::CreateWindowEx( WS_EX_CLIENTEDGE, "edit", sz_count, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 
			rect_edit.left, rect_edit.top, rect_edit.right-rect_edit.left, rect_edit.bottom - rect_edit.top,
			hwnd, (HMENU)(IDC_EDITOR_BASE+idx*2), AfxGetInstanceHandle(), 0 );

		HWND
		hwnd_spin = ::CreateWindow( UPDOWN_CLASS, 0, WS_CHILD|WS_VISIBLE|UDS_ALIGNRIGHT|UDS_AUTOBUDDY|UDS_NOTHOUSANDS|UDS_ARROWKEYS|UDS_SETBUDDYINT,
			0, 0, 0, 0, hwnd, (HMENU)(IDC_EDITOR_BASE+idx*2+1), AfxGetInstanceHandle(), 0 );

		::SendMessage( hwnd_spin, UDM_SETRANGE, 0, MAKELONG( 100, 0 ) );
		::SendMessage( hwnd_text, WM_SETFONT, (WPARAM)hFont, 0 );
		::SendMessage( hwnd_edit, WM_SETFONT, (WPARAM)hFont, 0 );

		::SetWindowText( hwnd_edit, sz_count );

		x += xofs_edit + w_edit;
	}
/*	if( y + yofs_edit + h_edit > rect_static.bottom-yofs_edit )
		m_nExpandY = y + yofs_edit + h_edit - (rect_static.bottom-yofs_edit);
	else
		m_nExpandY = 0;*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSimpleFieldPage::OnSetActive() 
{
/*	if (m_nExpandY > 0)
	{
		CRect rc;
		CWnd *pParent = GetParent();
		pParent->GetWindowRect(&rc);
		pParent->SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height()+m_nExpandY, SWP_NOMOVE|SWP_NOZORDER);
		GetWindowRect(&rc);
		for (CWnd *pWnd = pParent->GetWindow(GW_CHILD); pWnd; pWnd = pWnd->GetWindow(GW_HWNDNEXT))
		{
			CRect rc1;
			pWnd->GetWindowRect(rc1);
			if (pWnd != this && rc1.top >= rc.bottom)
			{
				pParent->ScreenToClient(rc1);
				pWnd->SetWindowPos(NULL, rc1.left, rc1.top+m_nExpandY, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			}
		}
		SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height()+m_nExpandY, SWP_NOMOVE|SWP_NOZORDER);
		m_nExpandY = 0;
	}*/
	PROCESS_SET_ACTIVE_MSG()
}

LRESULT CSimpleFieldPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	PROCESS_WIZARD_BACK_MSG()
}

LRESULT CSimpleFieldPage::OnWizardNext() 
{
	HWND	hwnd = m_hWnd;

	bool	fNoZero = false;
	int	idx = 0;
	for( field_create_info	*pfc = m_pcreate; pfc; pfc = pfc->next, idx++ )
	{
		int	idc = idx*2+IDC_EDITOR_BASE;

		BOOL	fOK;
		pfc->count = ::GetDlgItemInt( hwnd, idc, &fOK,  false );

		if( pfc->count != 0 )
			fNoZero = true;
		if( !fOK )
		{
			CString	str;
			str.Format( IDS_INVALID_NUMBER, pfc->sz_fldbase );
			AfxMessageBox( str );

			
			return -1;
		}
	}

	if( !fNoZero )
	{
		AfxMessageBox( IDS_NO_FIELD );
		return -1;
	}
	
	PROCESS_WIZARD_NEXT_MSG()
}
