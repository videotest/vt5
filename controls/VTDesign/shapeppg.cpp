// shapeppg.cpp : implementation file
//

#include "stdafx.h"
#include "VTDesign.h"
#include "shapeppg.h"
#include "ax_ctrl_misc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVTShapePropPage dialog

IMPLEMENT_DYNCREATE(CVTShapePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTShapePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTShapePropPage)
	ON_MESSAGE(CPN_SELENDOK, OnColorChange)
	ON_MESSAGE(LPN_SELENDOK, OnLineChange)	
	ON_MESSAGE(APN_SELENDOK, OnArrowChange)		
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

// {7C781C1E-35AB-454F-8733-7584F3296774}
IMPLEMENT_OLECREATE_EX(CVTShapePropPage, "VTDESIGN.CVTShapePropPage.1",
	0x7c781c1e, 0x35ab, 0x454f, 0x87, 0x33, 0x75, 0x84, 0xf3, 0x29, 0x67, 0x74)


/////////////////////////////////////////////////////////////////////////////
// CVTShapePropPage::CVTShapePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTShapePropPage
						
BOOL CVTShapePropPage::CVTShapePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTSHAPE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTShapePropPage::CVTShapePropPage - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CVTShapePropPage::CVTShapePropPage() :
	COlePropertyPage(IDD, IDS_VTSHAPE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTShapePropPage)
	m_bTransparentBk = FALSE;
	m_bTransparentFill = FALSE;
	m_bRoundedLine = FALSE;
	m_fLineWidth = 1.0;
	//}}AFX_DATA_INIT
	m_lineColor = m_fillColor = m_backColor = RGB(0,0,0);
	m_lineStyle = (DashStyle)-1;
	m_arrowStart = none;
	m_arrowEnd = none;
}


/////////////////////////////////////////////////////////////////////////////
// CVTShapePropPage::DoDataExchange - Moves data between page and properties

void CVTShapePropPage::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(CVTShapePropPage)
	DDX_Control(pDX, IDC_START_ARROW, m_arrowStartPicker);
	DDX_Control(pDX, IDC_END_ARROW, m_arrowEndPicker);
	DDX_Control(pDX, IDC_STYLE_LINE, m_lineStylePicker);
	DDX_Control(pDX, IDC_COLOR_LINE, m_lineColorPicker);
	DDX_Control(pDX, IDC_COLOR_FILL, m_fillColorPicker);
	DDX_Control(pDX, IDC_COLOR_BK, m_backColorPicker);
	DDP_Check(pDX, IDC_ROUNDED_LINE, m_bRoundedLine, _T("RoundedLine") );
	DDX_Check(pDX, IDC_ROUNDED_LINE, m_bRoundedLine);
	DDP_Text(pDX, IDC_LINE_WIDTH, m_fLineWidth, _T("LineWidth") );
	DDX_Text(pDX, IDC_LINE_WIDTH, m_fLineWidth);
	//}}AFX_DATA_MAP


	DDP_Text(pDX, IDC_COLOR_FILL, m_fillColor, _T("FillColor") );
	DDX_ColourPicker(pDX, IDC_COLOR_FILL, m_fillColor);
	
	DDP_Text(pDX, IDC_COLOR_LINE, m_lineColor, _T("LineColor") );
	DDX_ColourPicker(pDX, IDC_COLOR_LINE, m_lineColor);
	

	DDP_Text(pDX, IDC_COLOR_BK, m_backColor, _T("BackColor") );
	DDX_ColourPicker(pDX, IDC_COLOR_BK, m_backColor);


	DDP_Text(pDX, IDC_COLOR_FILL, m_bTransparentFill, _T("TransparentFill") );
	DDX_ColourPicker(pDX, IDC_COLOR_FILL, m_bTransparentFill);

	DDP_Text(pDX, IDC_COLOR_BK, m_bTransparentBk, _T("TransparentBack") );
	DDX_ColourPicker(pDX, IDC_COLOR_BK, m_bTransparentBk);


	BYTE red, gereen, blue;
	red		= GetRValue(m_fillColor);
	gereen	= GetRValue(m_fillColor);
	blue	= GetRValue(m_fillColor);
	
	COLORREF test = RGB(255,255,255);
	
	if( pDX->m_bSaveAndValidate )
	{
		DDX_LinePicker(pDX, IDC_STYLE_LINE, m_lineStyle);
		nLineStyle = (short)m_lineStyle;
		DDP_Text(pDX, IDC_STYLE_LINE, nLineStyle, _T("LineStyle") );
		
		DDX_ArrowPicker(pDX, IDC_START_ARROW, m_arrowStart);
		nArrowStart = (short)m_arrowStart;
		DDP_Text(pDX, IDC_START_ARROW, nArrowStart, _T("StartArrow") );
		
		DDX_ArrowPicker(pDX, IDC_END_ARROW, m_arrowEnd);
		nEndStart = (short)m_arrowEnd;
		DDP_Text(pDX, IDC_END_ARROW, nEndStart, _T("EndArrow") );
		

	}
	else
	{
		
		DDP_Text(pDX, IDC_STYLE_LINE, nLineStyle, _T("LineStyle") );		
		m_lineStyle = (DashStyle)nLineStyle;
		DDX_LinePicker(pDX, IDC_STYLE_LINE, m_lineStyle);
		
		DDP_Text(pDX, IDC_START_ARROW, nArrowStart, _T("StartArrow") );
		m_arrowStart = (ArrowType)nArrowStart;
		DDX_ArrowPicker(pDX, IDC_START_ARROW, m_arrowStart);		
		
		DDP_Text(pDX, IDC_END_ARROW, nEndStart, _T("EndArrow") );
		m_arrowEnd = (ArrowType)nEndStart;
		DDX_ArrowPicker(pDX, IDC_END_ARROW, m_arrowEnd);
		
	}		

	DDP_PostProcessing(pDX);

	if( pDX->m_bSaveAndValidate )
		FirePropExchange( this );
}


/////////////////////////////////////////////////////////////////////////////
// CVTShapePropPage message handlers
extern const IID BASED_CODE IID_DVTDLine;
extern const IID BASED_CODE IID_DVTDRectangle;
extern const IID BASED_CODE IID_DVTDEllipse;

BOOL CVTShapePropPage::OnInitDialog() 
{	
	BOOL bBackGroup = FALSE;
	BOOL bFillGroup = FALSE;
	BOOL bLineGroup = FALSE;
	BOOL bArrowGroup = FALSE;


	COlePropertyPage::OnInitDialog();

	ULONG Ulong;
	LPDISPATCH *lpDispatch = NULL;

	lpDispatch = GetObjectArray(&Ulong);	
	
	if( !lpDispatch[0] )
	{
		AfxMessageBox( "Can't find Object dispatch interface" , MB_OK|MB_ICONASTERISK );
		return FALSE;	
	}

	//Determine control type
	IUnknown* punk = NULL;
	
	lpDispatch[0]->QueryInterface( IID_DVTDLine, (void**)&punk );
	if( punk != NULL )
	{
		punk->Release();
		punk = NULL;
		bLineGroup = bArrowGroup = bBackGroup = TRUE;

	}

	lpDispatch[0]->QueryInterface( IID_DVTDRectangle, (void**)&punk );
	if( punk != NULL )
	{
		punk->Release();
		punk = NULL;
		bLineGroup = bFillGroup = TRUE;
	}

	lpDispatch[0]->QueryInterface( IID_DVTDEllipse, (void**)&punk );
	if( punk != NULL )
	{
		punk->Release();
		punk = NULL;
		bLineGroup = bFillGroup = bBackGroup = TRUE;
	}

	//Now set windows disable
	if( !bBackGroup )
		GetDlgItem(IDC_COLOR_BK)->EnableWindow( FALSE );		

	
	if( !bFillGroup )	
		GetDlgItem(IDC_COLOR_FILL)->EnableWindow( FALSE );		


	if( !bLineGroup )
	{
		GetDlgItem(IDC_COLOR_LINE)->EnableWindow( FALSE );
		GetDlgItem(IDC_STYLE_LINE)->EnableWindow( FALSE );
		GetDlgItem(IDC_LINE_WIDTH)->EnableWindow( FALSE );
		GetDlgItem(IDC_SPIN_WIDTH)->EnableWindow( FALSE );
		GetDlgItem(IDC_ROUNDED_LINE)->EnableWindow( FALSE );		
	}


	if( !bArrowGroup )
	{
		GetDlgItem(IDC_START_ARROW)->EnableWindow( FALSE );
		GetDlgItem(IDC_END_ARROW)->EnableWindow( FALSE );
	}	


	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WIDTH);
	pSpin->SetRange( 0, UD_MAXVAL );
	

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CVTShapePropPage::OnColorChange(WPARAM lParam, LPARAM wParam)
{
	SetModifiedFlag( TRUE );		
	return 1;
}

LRESULT CVTShapePropPage::OnLineChange(WPARAM lParam, LPARAM wParam)
{
	SetModifiedFlag( TRUE );		
	return 1;
}

LRESULT CVTShapePropPage::OnArrowChange(WPARAM lParam, LPARAM wParam)
{
	SetModifiedFlag( TRUE );		
	return 1;
}
