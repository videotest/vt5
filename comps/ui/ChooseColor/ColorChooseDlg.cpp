// ColorChooseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ChooseColor.h"
#include "ColorChooseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CColorChooseDlg, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CColorChooseDlg dialog


CColorChooseDlg::CColorChooseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorChooseDlg::IDD, pParent)
{
	EnableAutomation();

	m_sName = "ChooseColor";
	m_sUserName.LoadString( IDS_WINDOWCAPTION );
	//m_pwndConvert = 0;

	//{{AFX_DATA_INIT(CColorChooseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	_OleLockApp( this );
}

CColorChooseDlg::~CColorChooseDlg()
{
	TRACE("CColorChooseDlg released!\n");
	_OleUnlockApp( this );
}

void CColorChooseDlg::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	//if(m_pwndConvert)
	//	delete m_pwndConvert;
	CDialog::OnFinalRelease();
	delete this;
}

void CColorChooseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorChooseDlg)
	DDX_Control(pDX, IDC_PALETTECTRL1, m_ctrlPalette);
	DDX_Control(pDX, IDC_ACIVECOLORSVIEWCTRL1, m_ctrlColorsView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorChooseDlg, CDialog)
	//{{AFX_MSG_MAP(CColorChooseDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CColorChooseDlg, CDialog)
	//{{AFX_DISPATCH_MAP(CColorChooseDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IColorChooseDlg to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {09A5EBF3-A0E0-11d3-A538-0000B493A187}
static const IID IID_IColorChooseDlg = 
{ 0x9a5ebf3, 0xa0e0, 0x11d3, { 0xa5, 0x38, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };


BEGIN_INTERFACE_MAP(CColorChooseDlg, CDialog)
	INTERFACE_PART(CColorChooseDlg, IID_IColorChooseDlg, Dispatch)
	INTERFACE_PART(CColorChooseDlg, IID_IWindow, Wnd)
	INTERFACE_PART(CColorChooseDlg, IID_IDockableWindow, Dock)
	INTERFACE_PART(CColorChooseDlg, IID_IRootedObject, Rooted)
	INTERFACE_PART(CColorChooseDlg, IID_INamedObject2, Name)
	INTERFACE_PART(CColorChooseDlg, IID_IChooseColor, Choose)
	INTERFACE_PART(CColorChooseDlg, IID_IHelpInfo, Help)
END_INTERFACE_MAP()



// {09A5EBF2-A0E0-11d3-A538-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CColorChooseDlg, "ChooseColor.ChooseColorDlg", 0x9a5ebf2, 0xa0e0, 0x11d3, 0xa5, 0x38, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);



/////////////////////////////////////////////////////////////////////////////
// IChooseColor implement
IMPLEMENT_UNKNOWN(CColorChooseDlg, Choose);


HRESULT CColorChooseDlg::XChoose::SetLButtonColor(OLE_COLOR color)
{
	_try_nested(CColorChooseDlg, Choose, SetLButtonColor)
	{
		if(pThis->m_ctrlColorsView.GetSafeHwnd())
			pThis->m_ctrlColorsView.SetLeftBtnColor(color);
		return S_OK;
	}
	_catch_nested;
}


HRESULT CColorChooseDlg::XChoose::SetRButtonColor(OLE_COLOR color)
{
	_try_nested(CColorChooseDlg, Choose, SetRButtonColor)
	{
		if(pThis->m_ctrlColorsView.GetSafeHwnd())
			pThis->m_ctrlColorsView.SetRightBtnColor(color);
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
// CColorChooseDlg message handlers

CWnd* CColorChooseDlg::GetWindow()
{
	return this;
}

bool CColorChooseDlg::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	if (!Create(IDD_DIALOG1, pparent))
		return false;
	CString strCaption;
	strCaption.LoadString(IDS_WINDOWCAPTION);
	SetWindowText(strCaption);
	GetWindowRect(&rc);
	m_size = CSize(rc.Width(), rc.Height());
	m_bFixed = true;
	//m_pwndConvert = new CConvertorChooser();
	CRect	rcSt;


	return true;
}




BEGIN_EVENTSINK_MAP(CColorChooseDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CColorChooseDlg)
	ON_EVENT(CColorChooseDlg, IDC_PALETTECTRL1, 1 /* LeftClick */, OnLeftClickPalettectrl, VTS_COLOR)
	ON_EVENT(CColorChooseDlg, IDC_PALETTECTRL1, 2 /* RightClick */, OnRightClickPalettectrl, VTS_COLOR)
	ON_EVENT(CColorChooseDlg, IDC_PUSHBUTTONCTRL1, -600 /* Click */, OnClickPushbuttonctrl1, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CColorChooseDlg::OnLeftClickPalettectrl(OLE_COLOR Color) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_ctrlColorsView.SetLeftBtnColor(Color);
	::SetValueColor(GetAppUnknown(), "Editor", "Fore", Color);	
	UpdateSheet();
}

void CColorChooseDlg::OnRightClickPalettectrl(OLE_COLOR Color) 
{
	// TODO: Add your control notification handler code here
	m_ctrlColorsView.SetRightBtnColor(Color);
	::SetValueColor(GetAppUnknown(), "Editor", "Back", Color);	
	UpdateSheet();
}

BOOL CColorChooseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	COLORREF color;
	color = ::GetValueColor(GetAppUnknown(), "Editor", "Fore", RGB(0,255,0));	
	m_ctrlColorsView.SetLeftBtnColor((OLE_COLOR)color);
	color = ::GetValueColor(GetAppUnknown(), "Editor", "Back", RGB(255,255,255));	
	m_ctrlColorsView.SetRightBtnColor((OLE_COLOR)color);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorChooseDlg::OnClickPushbuttonctrl1() 
{
	// TODO: Add your control notification handler code here
	m_ctrlPalette.SetDefaultColors();
}


void CColorChooseDlg::UpdateSheet()
{
	IUnknown	*punkPropertySheet = ::GetPropertySheet();

	if( punkPropertySheet )
	{
		IPropertySheetPtr	sptrO( punkPropertySheet );
		punkPropertySheet->Release();
		int	nPagesCount = 0;
		sptrO->GetPagesCount(&nPagesCount);
		for( long n = nPagesCount-1; n >-1; n-- )
		{
			IUnknown *punkPage = 0;
			sptrO->GetPage(n, &punkPage);
			if( !punkPage )
				continue;
			IWindowPtr sptrW(punkPage);
			punkPage->Release();
			if(sptrW != 0)
			{
				HWND hwnd = 0;
				sptrW->GetHandle((void**)&hwnd);
				if(hwnd && ::IsWindow(hwnd))
				{
					IOptionsPagePtr	page(punkPage);
					if(page != 0)
						page->LoadSettings();
				}
			}
		}
	}
}