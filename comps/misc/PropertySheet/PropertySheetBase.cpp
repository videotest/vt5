#include "stdafx.h"
#include "resource.h"
#include "ExtPage.h"
#include "PropertySheetBase.h"
#include "utils.h"

CPropertySheetBase::CPropertySheetBase( const char *szSection ) 
	: CPropertySheet( IDS_PROPERTIES )
{
	EnableAutomation();

	m_nPages = 0;
	
	m_sName = "OptionsSheet";
	m_sUserName.LoadString( IDS_PROPERTIES );

	

	AttachComponentGroup(szPluginPropertyPage);
	CCompManager::Init();
	m_nPages = GetComponentCount();


	bool lShowInfo = ( ::GetValueInt( ::GetAppUnknown(), "ProperySheet", "ShowReport", 0L ) == 1L );

	

	CExtPage *pPage = 0;
	bool	bFirst = true;

	if( lShowInfo )
		GetLogFile()<<"-Info about Property pages:-\n";

	CString strReport;
	for( int i = 0; i < m_nPages; i++ )
	{				
		if( lShowInfo )
		{
			CSize size( 0, 0);
			IUnknown* punkPage = GetComponent( i, false);
			IOptionsPagePtr ptrPage( punkPage );
			CString strName = GetObjectName( punkPage );
			if( ptrPage )
			{
				ptrPage->GetSize( &size );
			}
			else
			{
				ASSERT( false );
			}

			CString strProgID;
			IPersistPtr ptrPers( ptrPage );
			if( ptrPers )
			{
				CLSID clsid;
				::ZeroMemory( &clsid, sizeof(CLSID) );
				ptrPers->GetClassID( &clsid );

				BSTR bstr = 0;
				::ProgIDFromCLSID( clsid, &bstr );				
				strProgID = bstr;
				
				::CoTaskMemFree( bstr );	bstr = 0;
			}

			CString _str;
			_str.Format( "Page name:%s, ProgID:%s, cx=%d, cy=%d\n", strName, strProgID, size.cx, size.cy );

			GetLogFile()<<_str;			


		}
		

		IUnknown	*punkP = GetComponent( i, false);
		  
		/*
		#ifdef _DEBUG
		{
			INamedObject2Ptr ptrNO2( punkP );
			if( ptrNO2 == 0 )
				ASSERT( false );
		}
		#endif
		*/

		ASSERT( punkP );

		pPage = new CExtPage();
		if( !pPage->Init(  punkP ) )
		{
			delete pPage;
			continue;
		}



		if( bFirst )
			AddPage( pPage );
		m_PageList.Add( pPage );
		pPage = 0;

		bFirst = false;
	}

	if( lShowInfo )
		GetLogFile()<<"End of -Info about Property pages-\n";

}

CPropertySheetBase::~CPropertySheetBase()
{
	int nCount = m_PageList.GetSize();

	for(int i = 0; i < nCount; i++)
		delete m_PageList.GetAt(i);

	m_PageList.RemoveAll();
}

BEGIN_DISPATCH_MAP(CPropertySheetBase, CPropertySheet)
	//{{AFX_DISPATCH_MAP(CPropertySheetBase)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

BEGIN_MESSAGE_MAP(CPropertySheetBase, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertySheetBase)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// {FD74A2D9-7FAC-11D3-A50C-0000B493A187}
static const IID IID_IPropertySheetMan =
{ 0xfd74a2d9, 0x7fac, 0x11d3, { 0xa5, 0xc, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };

BEGIN_INTERFACE_MAP(CPropertySheetBase, CCmdTarget)
	INTERFACE_PART(CPropertySheetBase, IID_IPropertySheetMan, Dispatch)
	INTERFACE_PART(CPropertySheetBase, IID_IWindow, Wnd)
	INTERFACE_PART(CPropertySheetBase, IID_IRootedObject, Rooted)
	INTERFACE_PART(CPropertySheetBase, IID_INamedObject2, Name)
	INTERFACE_PART(CPropertySheetBase, IID_IPropertySheet, Sheet)
	INTERFACE_PART(CPropertySheetBase, IID_ICompManager, CompMan)
	INTERFACE_PART(CPropertySheetBase, IID_IHelpInfo, Help)
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////////////
//IPropertySheet
IMPLEMENT_UNKNOWN(CPropertySheetBase, Sheet);

HRESULT CPropertySheetBase::XSheet::IncludePage(int nPage)
{
	METHOD_PROLOGUE_EX(CPropertySheetBase, Sheet)
	{
		pThis->DelayShowPage( nPage, true );
		return S_OK;
	}
}

HRESULT CPropertySheetBase::XSheet::ExcludePage(int nPage)
{
	METHOD_PROLOGUE_EX(CPropertySheetBase, Sheet)
	{
		pThis->DelayShowPage( nPage, false );
		return S_OK;
	}
}

HRESULT CPropertySheetBase::XSheet::ActivatePage(int nPage)
{
	METHOD_PROLOGUE_EX(CPropertySheetBase, Sheet)
	{
		if (nPage < pThis->GetPageCount() && nPage >= 0)
		{
			pThis->SetActivePage(nPage);
			return S_OK;
		}
		else
			return E_INVALIDARG;
	}
}

HRESULT CPropertySheetBase::XSheet::GetPagesCount(int* pNumPages)
{
	METHOD_PROLOGUE_EX(CPropertySheetBase, Sheet)
	{
		if (pNumPages != 0)
		{
			*pNumPages = pThis->m_nPages;
			return S_OK;
		}
		else
			return E_INVALIDARG;
		return S_OK;
	}
}

HRESULT CPropertySheetBase::XSheet::GetActivePage( int *pnPage )
{
	METHOD_PROLOGUE_EX(CPropertySheetBase, Sheet)
	{
		*pnPage = pThis->GetActiveIndex();
		return S_OK;
	}
}

HRESULT CPropertySheetBase::XSheet::GetPage( int nPage, IUnknown **ppunkPage )
{
	METHOD_PROLOGUE_EX(CPropertySheetBase, Sheet)
	{
		CExtPage	*ppage = pThis->m_PageList[nPage];

		*ppunkPage = ppage->GetControllingPage();

		if( *ppunkPage )
			(*ppunkPage)->AddRef();

		return S_OK;
	}
}

HRESULT CPropertySheetBase::XSheet::SetModified( long bSetModified )
{
	METHOD_PROLOGUE_EX(CPropertySheetBase, Sheet)
	{
		return S_OK;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CPropertySheetBase message handlers

CWnd* CPropertySheetBase::GetWindow()
{
	return this;
}

void CPropertySheetBase::DelayShowPage( int nPage, bool bShow )
{
	for( int n = m_PagesToDelayShow.GetSize( ) - 1; n >= 0; n-- )
	{
		if( m_PagesToDelayShow[n] == nPage )
			m_PagesToDelayShow[n] = -1;
	}
	for( n = m_PagesToDelayHide.GetSize( ) - 1; n >= 0; n-- )
	{
		if( m_PagesToDelayHide[n] == nPage )
			m_PagesToDelayHide[n] = -1;
	}



	if( bShow )
	{
		if( !IsPageVisible( nPage ) ) 
			m_PagesToDelayShow.Add( nPage );
	}
	else
	{
		if( IsPageVisible( nPage ) ) 
			m_PagesToDelayHide.Add( nPage );
	}
	
}

void CPropertySheetBase::PreformDelayOperations()
{
	HWND hwnd = ::GetFocus();
 
	for( int n = 0; n < m_PagesToDelayShow.GetSize(); n++ )
	{
		int	nPage = m_PagesToDelayShow[n];
		if( nPage == -1 )
			continue;
		if( IsPageVisible( nPage ) )
			continue;

		bool	bEmpty = GetPageCount() == 0;

		CPropertyPage	*ppage = m_PageList.GetAt( nPage );


		AddPage( ppage );
		SetActivePage( ppage );

		if( bEmpty )
			Invalidate();
	}
	m_PagesToDelayShow.RemoveAll();

	for( n = 0; n < m_PagesToDelayHide.GetSize(); n++ )
	{
		int	nPage = m_PagesToDelayHide[n];
		if( nPage == -1 )
			continue;

		if( !IsPageVisible( nPage ) )
			continue;

		bool	bLastPage = GetPageCount() == 1;
		RemovePage(m_PageList.GetAt( nPage ));

		if( bLastPage )
			Invalidate();
	}
	m_PagesToDelayHide.RemoveAll();

	::SetFocus(hwnd);
}

void CPropertySheetBase::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == 777 )
		PreformDelayOperations();

	CPropertySheet::OnTimer(nIDEvent);
}

int CPropertySheetBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer( 777, 100, 0 );	

	return 0;
}

void CPropertySheetBase::OnDestroy() 
{
	KillTimer( 777 );
	CPropertySheet::OnDestroy();
	
}

static AFX_MODULE_STATE* g_pState = 0;
static AFX_MODULE_THREAD_STATE* g_pState_thread = 0;

bool CPropertySheetBase::IsPageVisible( int nPage )
{	
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	/*
	AFX_MODULE_STATE* pState = AfxGetStaticModuleState();
	if( !g_pState )
		g_pState = pState;
	ASSERT( g_pState == pState );

	AFX_MODULE_THREAD_STATE* pState1 = AfxGetModuleThreadState();
	if( !g_pState_thread )
		g_pState_thread = pState1;

	ASSERT( g_pState_thread == pState1 );
	*/

	CPropertyPage	*ppage = m_PageList[nPage];
	int	idx = GetPageIndex( ppage );
	
	return idx != -1;
}

BOOL CPropertySheetBase::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	
	CTabCtrl	*ptabControl = GetTabControl();
	CRect	rectWindow;
	GetWindowRect( &rectWindow );
	CRect	rectTabControl;
	ptabControl->GetWindowRect( &rectTabControl );

	CSize	size( rectTabControl.left-rectWindow.left-
					rectTabControl.right+rectWindow.right, 
					rectTabControl.top-rectWindow.top-
					rectTabControl.right+rectWindow.right );
					
	rectTabControl-= CPoint( rectTabControl.left-rectWindow.left, rectTabControl.top-rectWindow.top );

	rectWindow = rectTabControl;

	CWnd	*pwndParent = CWnd::GetParent();
	if( pwndParent )pwndParent->ScreenToClient( &rectWindow );

	MoveWindow( rectWindow );

	ScreenToClient( rectTabControl );
	ptabControl->MoveWindow( rectTabControl );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
