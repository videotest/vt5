// VTEditor.cpp : Implementation of the CVTEditor ActiveX Control class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTEditorl.h"
#include "VTEditorPpg.h"
#include "FramesPropPage.h"

#include <msstkppg.h>
#include "utils.h"
#include "BaseColumn.h"

HWND APIENTRY CreateToolTip(HWND hWndParent); 
void APIENTRY FillInToolInfo(TOOLINFO* ti, HWND hWnd, UINT nIDTool = 0); 
BOOL APIENTRY AddTool(HWND hTip, HWND hWnd, RECT* pr = NULL, UINT nIDTool = 0, LPCTSTR szText = NULL); 
void APIENTRY UpdateTipText(HWND hTip, HWND hWnd, UINT nIDTool = 0, LPCTSTR lpszText = NULL);
void APIENTRY GetTipText(HWND hTip, HWND hWnd, UINT nIDTool, LPSTR szText);
void APIENTRY EnableToolTip(HWND hTip, BOOL activate);
BOOL APIENTRY DestroyToolTip(HWND hWnd);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CVTEditor, COleControl)


#ifndef FOR_HOME_WORK
BEGIN_INTERFACE_MAP(CVTEditor, COleControl)
	INTERFACE_PART(CVTEditor, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CVTEditor, IID_IVtActiveXCtrl2, ActiveXCtrl)
	INTERFACE_PART(CVTEditor, IID_IVtActiveXCtrl3, ActiveXCtrl)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTEditor, ActiveXCtrl)
#endif

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTEditor, COleControl)
	//{{AFX_MSG_MAP(CVTEditor)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()	
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_DRAWITEM()

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTEditor, COleControl)
	//{{AFX_DISPATCH_MAP(CVTEditor)
	DISP_PROPERTY_EX(CVTEditor, "Name", GetName, SetName, VT_BSTR)
	DISP_PROPERTY_EX(CVTEditor, "CaptionPosition", GetCaptionPosition, SetCaptionPosition, VT_I2)
	DISP_PROPERTY_EX(CVTEditor, "DataType", GetDataType, SetDataType, VT_I2)
	DISP_PROPERTY_EX(CVTEditor, "CaptionText", GetCaptionText, SetCaptionText, VT_BSTR)
	DISP_PROPERTY_EX(CVTEditor, "CaptionFieldWidth", GetEditFieldWidth, SetEditFieldWidth, VT_I2)
	DISP_PROPERTY_EX(CVTEditor, "SpinEnable", GetSpinEnable, SetSpinEnable, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "MinValue", GetMinValue, SetMinValue, VT_R4)
	DISP_PROPERTY_EX(CVTEditor, "MaxValue", GetMaxValue, SetMaxValue, VT_R4)
	DISP_PROPERTY_EX(CVTEditor, "ValidationEnable", GetValidationEnable, SetValidationEnable, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "CaptionEnable", GetCaptionEnable, SetCaptionEnable, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "BackgroundColor", GetBackgroundColor, SetBackgroundColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEditor, "CaptionBackColor", GetCaptionBackColor, SetCaptionBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEditor, "CaptionTextColor", GetCaptionTextColor, SetCaptionTextColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEditor, "EditBackColor", GetEditBackColor, SetEditBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEditor, "EditTextColor", GetEditTextColor, SetEditTextColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTEditor, "EditFont", GetEditFont, SetEditFont, VT_FONT)
	DISP_PROPERTY_EX(CVTEditor, "CaptionFont", GetCaptionFont, SetCaptionFont, VT_FONT)
	DISP_PROPERTY_EX(CVTEditor, "CaptionAutoWidth", GetCaptionAutoWidth, SetCaptionAutoWidth, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "Key", GetKey, SetKey, VT_BSTR)
	DISP_PROPERTY_EX(CVTEditor, "AutoUpdate", GetAutoUpdate, SetAutoUpdate, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "AlignClient", GetAlignClient, SetAlignClient, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "MultiLine", GetMultiLine, SetMultiLine, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "InnerSunken", GetInnerSunken, SetInnerSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "OuterRaised", GetOuterRaised, SetOuterRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "OuterSunken", GetOuterSunken, SetOuterSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "InnerRaised", GetInnerRaised, SetInnerRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "Text", GetText, SetText, VT_BSTR)
	DISP_PROPERTY_EX(CVTEditor, "ValueInt", GetValueInt, SetValueInt, VT_I4)
	DISP_PROPERTY_EX(CVTEditor, "ValueDouble", GetValueDouble, SetValueDouble, VT_R8)
	DISP_FUNCTION(CVTEditor, "StoreToData", StoreToData, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CVTEditor, "SetAutoSize", SetAutoSize, VT_BOOL, VTS_NONE)
	DISP_PROPERTY_EX(CVTEditor, "Disabled", GetDisabled, SetDisabled, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_FUNCTION(CVTEditor, "SetFocus", SetFocus, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CVTEditor, "UseSystemFont", GetUseSystemFont, SetUseSystemFont, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "VT5StaticStyle", GetVT5StaticStyle, SetVT5StaticStyle, VT_BOOL)
	DISP_PROPERTY_EX(CVTEditor, "NumberCharAFP", GetNumberCharAFP, SetNumberCharAFP, VT_I4)
	//}}AFX_DISPATCH_MAP
	//DISP_PROPERTY_EX_ID(CVTEditor, "VT5StaticStyle", 37, GetVT5StaticStyle, SetVT5StaticStyle, VT_BOOL)
	DISP_FUNCTION_ID(CVTEditor, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTEditor, COleControl)
	//{{AFX_EVENT_MAP(CVTEditor)
	EVENT_CUSTOM("DataChange", FireDataChange, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages
/*
DEFINE_GUID(CLSID_CColorPropPage, 
	0x0be35201,0x8f91,0x11ce,0x9d,0xe3,0x00,0xaa,0x00,0x4b,0xb8,0x51);
{0BE35201-8F91-11CE-9DE3-00AA004BB851}
*/
DEFINE_GUID(CLSID_CColorPropPageMy, 
	0x7629CFA4,0x3FE5,0x101B,0xA3,0xC9,0x08,0x00,0x2B,0x2F,0x49,0xFB);


// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTEditor, 5)
	PROPPAGEID(CVTEditorPropPage::guid)
	PROPPAGEID(CVTEditorPropPageExt::guid)
	PROPPAGEID(CFramesPropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTEditor)



/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTEditor, "VTCONTROLS.VTEditor.1",
	0x6bdf8b84, 0xc1b9, 0x448e, 0x89, 0x65, 0xe, 0x45, 0x57, 0x46, 0x7f, 0xda)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTEditor, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTEditor =
		{ 0x19b862bf, 0x87e2, 0x42c7, { 0x98, 0x31, 0x57, 0xb6, 0xa6, 0x5c, 0x36, 0x9e } };
const IID BASED_CODE IID_DVTEditorEvents =
		{ 0xe96348b8, 0xfa6a, 0x4f9d, { 0x86, 0xb8, 0x25, 0x79, 0x2c, 0xdf, 0x46, 0x81 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTEditorOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTEditor, IDS_VTEDITOR, _dwVTEditorOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTEditor::CVTEditorFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTEditor

BOOL CVTEditor::CVTEditorFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VTEDITOR,
			IDB_VTEDITOR,
			afxRegApartmentThreading,
			_dwVTEditorOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTEditor::CVTEditor - Constructor

CVTEditor::CVTEditor() : m_fontHolderEdit( &m_xFontNotification ),
						 m_fontHolderCaption( &m_xFontNotification  )
{
	InitializeIIDs(&IID_DVTEditor, &IID_DVTEditorEvents);

#ifndef FOR_HOME_WORK
	m_ptrSite = m_ptrApp = NULL;
#endif
	
	InitDefaults( );

	hAccel = NULL;

	m_bCanEnterValidation = TRUE;
	m_bVTStaticStyle = TRUE;

}					 

/////////////////////////////////////////////////////////////////////////////////////
//
//
//		InitDefaults
//		For: Constructor
//
////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::InitDefaults()
{
	m_bWasCreated		= FALSE;

	m_strName			= _T("VTEditor");

	m_CaptionAlignEnum	= caAlignUp;
	m_DataTypeEnum		= dtTypeString;

	m_bCaptionEnable	= TRUE;
	m_bSpinEnable		= FALSE;
	m_bValidationEnable	= TRUE;
	
	m_strCaptionText	= _T("Caption");
	m_strEditText		= _T("Edit");
	
	m_nCaptionFieldWidth	= 100;
	m_fMaxValue			= 100.0;
	m_fMinValue			= 0.0;

	m_clBackgroundColor			= 0x80000000 + COLOR_BTNFACE;
	m_clCaptionBackgroundColor	= 0x80000000 + COLOR_BTNFACE;
	m_clCaptionTextColor		= 0x80000000 + COLOR_WINDOWTEXT;

	m_clEditBackgroundColor		= 0x80000000 + COLOR_WINDOW;
	m_clEditTextColor			= 0x80000000 + COLOR_WINDOWTEXT;

	m_bCaptionAutoSize			= TRUE;


	m_strKeyValue				= "";
	m_bAutoUpdate				= FALSE;

	m_bMultiLine				= FALSE;
	m_bAlignClient				= FALSE;


	m_bInnerRaised = m_bInnerSunken = 
		m_bOuterRaised = m_bOuterSunken = FALSE;


	//m_fontCaption.DeleteObject( );
	//m_fontEdit.DeleteObject( );

	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );
	
	pFont->GetLogFont( &m_logfontEdit );

	memcpy( &m_logfontCaption, &m_logfontEdit, sizeof(LOGFONT) );


	

	//m_fontCaption.Attach( (HGDIOBJ)::GetStockObject(ANSI_VAR_FONT) );
	//m_fontEdit.Attach( (HGDIOBJ)::GetStockObject(ANSI_VAR_FONT) );


	m_bInternalTextUpdate = false;


	m_bUseSystemFont = TRUE;
	m_bVTStaticStyle = FALSE;

	m_bMinValue=false;
	m_bMinMaxVerify=false;
	m_bKillFocus=false;
	m_nCharAFP=-1;
}

/////////////////////////////////////////////////////////////////////////////
// CVTEditor::~CVTEditor - Destructor

CVTEditor::~CVTEditor()
{
	if(hTip)
		DestroyToolTip(hTip);
	if( hAccel ) DestroyAcceleratorTable( hAccel );
}


int CVTEditor::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_captionCtrl.Create("",
						m_bVTStaticStyle?WS_CHILD|WS_VISIBLE|SS_OWNERDRAW:WS_CHILD|WS_VISIBLE,
						CRect( 0, 0, 0, 0 ),
						this,
						ID_STATIC_CTRL 
						);
	
	m_editCtrl.Create(	
						WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
						CRect( 0, 0, 0, 0 ),
						this,
						ID_EDIT_CTRL 
						);

	m_spinCtrl.Create(	
						WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS   ,
						CRect( 0, 0, 0, 0 ),
						this,
						ID_SPIN_CTRL 
						);


	m_editCtrl.SetParentCtrl( this );

	//OnActivateInPlace (TRUE, NULL); 

	m_bWasCreated = TRUE;

	BuildAppearence( );	

	m_editCtrl.SetFocus( );

	hTip = NULL;
	static char* buf[]={"Edit Box"};
	HWND hDlg=(m_editCtrl.GetParent())->m_hWnd;
	hTip = CreateToolTip(hDlg);
	BOOL res = AddTool(hTip,m_editCtrl.m_hWnd,NULL,0,buf[0]);
    EnableToolTip(hTip,FALSE);
  
	return 0;
}

BOOL CVTEditor::PreCreateWindow(CREATESTRUCT& cs) 
{
	//cs.dwExStyle |= WS_EX_CONTROLPARENT;
	cs.style		|= WS_TABSTOP;	
	cs.style		|= WS_CLIPCHILDREN;
	cs.style		|= WS_CLIPSIBLINGS;

	return COleControl::PreCreateWindow(cs);
}

BOOL CVTEditor::OnEraseBkgnd(CDC* pDC) 
{	
	
	if( GetSafeHwnd() )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillRect( &rc, &(CBrush( TranslateColor( m_clBackgroundColor ) ) ) );
		return TRUE;		
	}	
	
	
	
	return COleControl::OnEraseBkgnd(pDC);
}


void CVTEditor::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);	


	CString	str;
	str.Format( "\nEditor: %d %d", cx, cy );
	OutputDebugString( str );
	RepaintControl( );
}

BOOL CVTEditor::IsSubclassedControl()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CVTEditor::OnDraw - Drawing function
void CVTEditor::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{	
	//if (m_hWnd == NULL)
	//	CreateWindowForSubclassedControl();

	CRect rc;

	_GetClientRect( &rc );

	pdc->FillRect( rc, &(CBrush( TranslateColor( m_clBackgroundColor ) ) ) );

	UINT	edgeType = 0;
	if(m_bInnerRaised)
		edgeType |= BDR_RAISEDINNER;
	if(m_bInnerSunken)
		edgeType |= BDR_SUNKENINNER;
	if(m_bOuterRaised)
		edgeType |= BDR_RAISEDOUTER;
	if(m_bOuterSunken)
		edgeType |= BDR_SUNKENOUTER;
	if(edgeType != 0)
	{
		pdc->DrawEdge(CRect(0,0,rc.Width(),rc.Height()), edgeType, BF_RECT);

		rc.left		+= VT_BORDER_WIDTH;
		rc.top		+= VT_BORDER_WIDTH;
		rc.right	-= VT_BORDER_WIDTH;
		rc.bottom	-= VT_BORDER_WIDTH;
	}	



	
	
				   
	if( m_captionCtrl.GetSafeHwnd() ) 
	{
		m_captionCtrl.Invalidate();
		m_captionCtrl.UpdateWindow();
	}

	if( m_editCtrl.GetSafeHwnd() ) 
	{
		m_editCtrl.RedrawWindow( NULL, NULL, 
			RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );	
	}

	if( m_spinCtrl.GetSafeHwnd() ) 
	{
		m_spinCtrl.RedrawWindow( NULL, NULL, 
			RDW_FRAME|RDW_INVALIDATE|RDW_INTERNALPAINT );	
	}
	
	

}


/////////////////////////////////////////////////////////////////////////////
// CVTEditor::DoPropExchange - Persistence support

void CVTEditor::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

}


/////////////////////////////////////////////////////////////////////////////
// CVTEditor::OnResetState - Reset control to default state

void CVTEditor::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	//ResetStockProps();


	// TODO: Reset any other control state here.
}

/////////////////////////////////////////////////////////////////////////////
// CVTEditor::Serialize - Serialization
void CVTEditor::Serialize(CArchive& ar)
{

	
	try
	{
		

		long	nVersion = 9;
		COleControl::Serialize( ar );

		if( ar.IsStoring( ) )
		{				
			ar<<nVersion;

			ar.Write( &m_logfontEdit, sizeof(LOGFONT) );		

			ar.Write( &m_logfontCaption, sizeof(LOGFONT) );

			ar << m_strName;
			ar << (SHORT)m_CaptionAlignEnum;
			ar << m_strCaptionText;
			ar << m_bCaptionEnable;
			ar << (SHORT)m_DataTypeEnum;				
			ar << m_strEditText;
			ar << m_nCaptionFieldWidth;			
			ar << m_bSpinEnable;
			ar << m_bValidationEnable;
			ar << m_fMaxValue;
			ar << m_fMinValue;
			ar << m_clEditTextColor;
			ar << m_clEditBackgroundColor;
			ar << m_clCaptionTextColor;
			ar << m_clCaptionBackgroundColor;
			ar << m_clBackgroundColor;
			ar << m_bCaptionAutoSize;
			ar << m_strKeyValue;
			ar << m_bAutoUpdate;
			ar << m_bMultiLine;
			ar << m_bAlignClient;

			ar << m_bInnerRaised;
			ar << m_bInnerSunken;
			ar << m_bOuterRaised;
			ar << m_bOuterSunken;

			SerializeStockProps(ar);

			ar << m_bUseSystemFont;
			//SerializeExtent(ar);

			
		}else	
		{
			ar>>nVersion;
						
			if( nVersion >= 1 )
			{
				ar.Read( &m_logfontEdit, sizeof(LOGFONT) );
				
				ar.Read( &m_logfontCaption, sizeof(LOGFONT) );						

				SHORT nReadType;
				ar >> m_strName;
				ar >> (SHORT)nReadType;
				m_CaptionAlignEnum = (CaptionAlignEnum)nReadType;

				ar >> m_strCaptionText;
				ar >> m_bCaptionEnable;
				ar >> (SHORT)nReadType;
				m_DataTypeEnum = (DataTypeEnum)nReadType;

				ar >> m_strEditText;
				ar >> m_nCaptionFieldWidth;			
				ar >> m_bSpinEnable;
				ar >> m_bValidationEnable;
				ar >> m_fMaxValue;
				ar >> m_fMinValue;
				ar >> m_clEditTextColor;
				ar >> m_clEditBackgroundColor;
				ar >> m_clCaptionTextColor;
				ar >> m_clCaptionBackgroundColor;
				ar >> m_clBackgroundColor;

			}



			if( nVersion >= 2 )
			{
				ar >> m_bCaptionAutoSize;
			}

			if( nVersion >= 3 )
			{
				ar >> m_strKeyValue;
				ar >> m_bAutoUpdate;

			}

			if( nVersion >= 4 )
			{
				ar >> m_bMultiLine;
			}				

			if( nVersion >= 5 )
			{
				ar >> m_bAlignClient;
			}				


			if( nVersion >= 6 )
			{
				ar >> m_bInnerRaised;
				ar >> m_bInnerSunken;
				ar >> m_bOuterRaised;
				ar >> m_bOuterSunken;
			}				

			if( nVersion >= 7 )
				SerializeStockProps(ar);

			if( nVersion >= 9 )
				ar >> m_bUseSystemFont;

			m_logfontEdit.lfCharSet		= DEFAULT_CHARSET;
			m_logfontCaption.lfCharSet	= DEFAULT_CHARSET;			

			BuildAppearence( );
		}		


	}	
	catch(CException *pe)
	{
		pe->ReportError();
		pe->Delete();
		InitDefaults();
		BuildAppearence( );

	}
	  
}


/////////////////////////////////////////////////////////////////////////////
// CVTEditor::AboutBox - Display an "About" box to the user

void CVTEditor::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTEDITOR);
	dlgAbout.DoModal();
}



/////////////////////////////////////////////////////////////////////////////////////
//
//
//		Recalc layout. Move window to new positions
//		For : RepaintControl
//
////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::RecalcLayout( int cx, int cy )
{
	///m_editCtrl.MoveWindow( 0, 0, cx, cy , FALSE );


	BOOL bHaveEdge = (m_bInnerRaised || m_bInnerSunken
					||m_bOuterRaised || m_bOuterSunken);

	//m_nCaptionFieldWidth
	int nEditCtrlHeight		= 0;

	
	int nCaptionCtrlWidth	= 0;
	int nCaptionCtrlHeight	= 0;	

	int nCaptionFieldWidth  = 0;


	{
		CClientDC dc(this);
		TEXTMETRIC tm;

		CFont font;
		::GetFontFromDispatch( this, font, TRUE );
		LOGFONT lfDefault;
		::ZeroMemory( &lfDefault, sizeof(LOGFONT) );
		font.GetLogFont( &lfDefault );

		//Determine  nEditCtrlHeight
		CFont tmpFontEdit, tmpFontCaption;
		tmpFontEdit.CreateFontIndirect( m_bUseSystemFont ? &lfDefault : &m_logfontEdit );
		tmpFontCaption.CreateFontIndirect( m_bUseSystemFont ? &lfDefault : &m_logfontCaption );

		CFont* pOldFont = (CFont*)dc.SelectObject( &tmpFontEdit );
		dc.GetTextMetrics( &tm );					
		nEditCtrlHeight = tm.tmHeight + 2 * ::GetSystemMetrics( SM_CYEDGE ) + 4;

		//Determine  nCaptionCtrlWidth & nCaptionCtrlHeight
		CRect rectCalc;
		dc.SelectObject( &tmpFontCaption );
		dc.DrawText( m_strCaptionText, &rectCalc, DT_CALCRECT );

		nCaptionCtrlWidth	= rectCalc.Width( );
		nCaptionCtrlHeight	= rectCalc.Height( );

		dc.SelectObject( pOldFont );
	}

	if( !m_bCaptionAutoSize ) nCaptionCtrlWidth = m_nCaptionFieldWidth;


	
	//Indentions
	int nCaptionIndentionHorz		= 5;
	int nCaptionIndentionVert		= 5;

	/*
	int nRightIndention				= (bHaveEdge ? VT_BORDER_WIDTH : 0);
	int nLeftIndention				= (bHaveEdge ? VT_BORDER_WIDTH : 0);

	int nBottomIndention			= (bHaveEdge ? VT_BORDER_WIDTH : 0);
	int nTopIndention				= (bHaveEdge ? VT_BORDER_WIDTH : 0);
	*/
	int nRightIndention				= (bHaveEdge ? VT_BORDER_WIDTH : 1);
	int nLeftIndention				= (bHaveEdge ? VT_BORDER_WIDTH : 1);

	int nBottomIndention			= (bHaveEdge ? VT_BORDER_WIDTH : 1);
	int nTopIndention				= (bHaveEdge ? VT_BORDER_WIDTH : 1);
	

	int nEditFieldWidth				= 0;

	
	if( m_bAlignClient )
		nEditCtrlHeight = cy - nBottomIndention - nTopIndention;
	

	nEditFieldWidth = cx - nLeftIndention - nRightIndention;

	if( m_bCaptionEnable )
	{
		if( m_CaptionAlignEnum == caAlignLeft )
		{
			nEditFieldWidth -= (nCaptionCtrlWidth + nCaptionIndentionHorz);
		}
		else
		{
			if( m_bAlignClient )
				nEditCtrlHeight -= (nCaptionCtrlHeight + nCaptionIndentionVert);
		}
	}

	int nEditCtrlLeft	= 
			cx - nEditFieldWidth - nRightIndention;

	int nEditCtrlTop	=
			cy - nEditCtrlHeight - nBottomIndention;

	


	if( m_bCaptionEnable )
	{
		if( m_CaptionAlignEnum == caAlignLeft )
		{
			m_captionCtrl.MoveWindow(
									nLeftIndention,
									nEditCtrlTop + nEditCtrlHeight / 2 
											- nCaptionCtrlHeight / 2,
									nCaptionCtrlWidth,
									nCaptionCtrlHeight
									, FALSE
									);
		}


		if( m_CaptionAlignEnum == caAlignUp )
		{
			m_captionCtrl.MoveWindow(
									nEditCtrlLeft,
									nEditCtrlTop - nCaptionCtrlHeight
											- nCaptionIndentionVert,
									nCaptionCtrlWidth,
									nCaptionCtrlHeight
									, FALSE
									);
		}



	}


	if( !m_bSpinEnable || m_bMultiLine)
	{
		CRect rectSpin;
		m_spinCtrl.GetWindowRect( &rectSpin );
		nEditFieldWidth += rectSpin.Width( );
		nEditFieldWidth -= nLeftIndention;


	}

	m_editCtrl.MoveWindow( 	
							nEditCtrlLeft,
							nEditCtrlTop,
							nEditFieldWidth, 
							nEditCtrlHeight, 
							FALSE
							);


	m_spinCtrl.SetBuddy( (CWnd*)&m_editCtrl );

	if( m_bCaptionEnable )
		m_captionCtrl.ShowWindow( SW_SHOWNA );
	else
		m_captionCtrl.ShowWindow( SW_HIDE );

	if( m_bSpinEnable && !m_bMultiLine )
		m_spinCtrl.ShowWindow( SW_SHOWNA );
	else
		m_spinCtrl.ShowWindow( SW_HIDE );
  
	

}

/////////////////////////////////////////////////////////////////////////////////////
//
//
//		RepaintControl : repaint controls
//		For: WM_SIZE, BuildAppearance, InvalidateAndSetModifiedFlag
//
////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::RepaintControl()
{
	m_captionCtrl.SetWindowText( m_strCaptionText );	

	//FindAccelKey();

	CRect rectClient;
	_GetClientRect( &rectClient );
	RecalcLayout( rectClient.Width(), rectClient.Height()  );
	
	InvalidateControl( );
								
	m_captionCtrl.Invalidate( );
	m_spinCtrl.Invalidate( );	
	
	 
	if( (m_DataTypeEnum == dtTypeInt) || (m_DataTypeEnum == dtTypeFloat) ){
		m_spinCtrl.EnableWindow( TRUE );
		m_spinCtrl.SetRange( (int)m_fMinValue, (int)m_fMaxValue );
		
		m_bInternalTextUpdate = true;
		m_spinCtrl.SetPos( atoi(m_strEditText) );
		m_bInternalTextUpdate = false;
	}
	else
		m_spinCtrl.EnableWindow( FALSE );


	
	DWORD dwStyle = m_editCtrl.GetStyle( );
		/*
	if( m_DataTypeEnum == dtTypeInt )
	{
		dwStyle |= ES_NUMBER;
		SetWindowLong(m_editCtrl.GetSafeHwnd() , GWL_STYLE, dwStyle );
	}
	else
	{
		dwStyle &= ~ES_NUMBER;	
		SetWindowLong(m_editCtrl.GetSafeHwnd() , GWL_STYLE, dwStyle );
	}	
		  */

	
	m_editCtrl.Invalidate( );	
	
	
	
}


/////////////////////////////////////////////////////////////////////////////////////
//
//
//		BuildAppearence : For loading and create. Create temp objects for correct paint
//		For: Serialize, WM_CREATE
//					Serialize
////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::BuildAppearence()
{

	if( !m_bWasCreated ) return;//For serialization which occure befor OnCreate

	
	//m_captionCtrl.SetWindowText( m_strCaptionText );
	//m_strEditText = "";
	
	m_bInternalTextUpdate = true;
	m_editCtrl.SetWindowText( m_strEditText );
	m_bInternalTextUpdate = false;

	CreateBackgroundBrushes( STL_FOR_CAPTION | STL_FOR_EDIT );	
	
	SetFontToControl( STL_FOR_CAPTION | STL_FOR_EDIT );	

	FindAccelKey();

	SwitchMultiSingleLine();

	RepaintControl();
}

/////////////////////////////////////////////////////////////////////////////////////
//
//		CreateBackgroundBrushes: create bkBrushes for controls in
//			addition to have correct bkground.
//		For: OnCtlColor, BuildAppearence
//		 
////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::CreateBackgroundBrushes( DWORD dwCreateStyle )
{
	if( dwCreateStyle & STL_FOR_CAPTION )
	{
		m_brushCaptionBackground.DeleteObject();
		m_brushCaptionBackground.CreateSolidBrush( TranslateColor( m_clCaptionBackgroundColor ) );
	}
	

	if( dwCreateStyle & STL_FOR_EDIT )
	{
		m_brushEditBackground.DeleteObject();
		m_brushEditBackground.CreateSolidBrush( TranslateColor( m_clEditBackgroundColor ) );
	}

}


/////////////////////////////////////////////////////////////////////////////////////
//
//		SetFontToControl: set font to control 
//			
//		For: BuildAppearence, OnChange???_Font
//		 
////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::SetFontToControl( DWORD dwCreateStyle )
{
	CFont font;
	::GetFontFromDispatch( this, font, TRUE );
	LOGFONT lfDefault;
	::ZeroMemory( &lfDefault, sizeof(LOGFONT) );
	font.GetLogFont( &lfDefault );

	if( dwCreateStyle & STL_FOR_CAPTION )
	{	
		m_fontCaption.DeleteObject();		
		m_fontCaption.CreateFontIndirect( m_bUseSystemFont ? &lfDefault : &m_logfontCaption );

		m_captionCtrl.SetFont( &m_fontCaption );				
	}

	if( dwCreateStyle & STL_FOR_EDIT )
	{
		m_fontEdit.DeleteObject();		
		m_fontEdit.CreateFontIndirect( m_bUseSystemFont ? &lfDefault : &m_logfontEdit );
		m_editCtrl.SetFont( &m_fontEdit );		
	}

}





/////////////////////////////////////////////////////////////////////////////////////
//
//	InvalidateAndSetModifiedFlag. Set modified flag and call repaint func.
//		For: property change
//			
//
////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::InvalidateAndSetModifiedFlag( /*DWORD dwFlags*/ )
{
	SetModifiedFlag();
	RepaintControl();
}

/////////////////////////////////////////////////////////////////////////////////////
//
//	OnSetCursor. Set correct cursor for m_editCtrl
//		For: Call by Frame to set cursor
//			
//
////////////////////////////////////////////////////////////////////////////////////
BOOL CVTEditor::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{	
	/*
	CWnd* pWndEdit = NULL;
	pWndEdit = GetDlgItem( ID_EDIT_CTRL );
	if( pWndEdit == pWnd ) 
	{
		HCURSOR hCursor = ::LoadCursor(NULL,IDC_IBEAM);
		::SetCursor( hCursor );
		::DestroyCursor( hCursor );
		return TRUE;
	}
	*/

	return COleControl::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//	OnCtlColor. Set background color for m_captionCtrl & m_editCtrl
//		For: Call by Frame to set background color
//			
//
////////////////////////////////////////////////////////////////////////////////////
HBRUSH CVTEditor::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = COleControl::OnCtlColor(pDC, pWnd, nCtlColor);
	
	
	CWnd* pEditWnd = NULL;
	pEditWnd = GetDlgItem( ID_EDIT_CTRL );

	CWnd* pCaptionWnd = NULL;
	pCaptionWnd  = GetDlgItem( ID_STATIC_CTRL );

	
	if( pWnd == pEditWnd )
	{
		pDC->SetTextColor( TranslateColor( m_clEditTextColor ) );
		pDC->SetBkColor( TranslateColor( m_clEditBackgroundColor ) );	
		return m_brushEditBackground;
	}
	
	if( pWnd == pCaptionWnd )
	{

		pDC->SetTextColor( TranslateColor( m_clCaptionTextColor ) );
		pDC->SetBkColor( TranslateColor( m_clCaptionBackgroundColor ) );		
		return m_brushCaptionBackground;		
	}	
	

	return hbr;
}




/////////////////////////////////////////////////////////////////////////////////////
//
//	OnEditCtrlKillFocus( )
//		For: Validation
//			
//
////////////////////////////////////////////////////////////////////////////////////
BOOL CVTEditor::Validation( bool bUpdateWindowText )
{	
	if( !m_bCanEnterValidation )
		return TRUE;

	CString strNewValue;

	TCHAR buffer[255];

	m_editCtrl.GetWindowText( buffer, 255 );				
	CString m_strEditText_new = buffer;

	if( m_strEditText_new == "-" )
	{
		m_strEditText=m_strEditText_new;
		return TRUE;
	}

	if( m_DataTypeEnum == dtTypeString )
	{
		m_strEditText=buffer;
		return TRUE;
	}

	BOOL bResult = TRUE;

	if( m_DataTypeEnum == dtTypeInt )
	{
		int nValue = 0;
		bool flag=false;
		while(!flag)
		{
			int i=m_strEditText_new.Find(" ",0);
			if(i!=-1)
			{
				bResult = FALSE;
				m_strEditText_new.Delete(i,1);
			}
			else
				flag=true;
		}
		try
		{
			_variant_t var( m_strEditText_new );
			var.ChangeType( VT_I4 );
			nValue = (int)var.lVal;
		}
		catch(...)
		{
			bResult = FALSE;
			if(m_strEditText_new=="")
				nValue = (int)m_fMinValue;
			else
			{
				try
				{
					_variant_t var( m_strEditText );
					var.ChangeType( VT_I4 );
					nValue = (int)var.lVal;
				}
				catch(...)
				{
				}
			}
		}
		
		if( m_bValidationEnable )
		{
			/*if( nValue < (int)m_fMinValue )								
			{
				nValue = (int)m_fMinValue;				
				bResult = FALSE;
			}
				
			if( nValue > (int)m_fMaxValue )
			{
				nValue = (int)m_fMaxValue;
				bResult = FALSE;
			}*/
			if( /*nValue < (int)m_fMinValue || */nValue > (int)m_fMaxValue)	
			{
				m_strEditText_new=m_strEditText;
				try
				{
					_variant_t var( m_strEditText_new );
					var.ChangeType( VT_I4 );
					nValue = (int)var.lVal;
				}
				catch(...)
				{
				}
				bResult = FALSE;
				if(hTip)
				{
					EnableToolTip(hTip,TRUE);
				}
				if(m_bMinValue && nValue>(int)m_fMinValue)
				{
					m_bMinValue=false;
					m_clEditBackgroundColor=OLE_COLOR(0x80000000 + COLOR_WINDOW);
					CreateBackgroundBrushes( STL_FOR_EDIT );
					if(hTip)
					{
						EnableToolTip(hTip,FALSE);
					}
				}
				if(hTip)
				{
					CString s;
					s.Format("Min=%d, max=%d", (int)m_fMinValue, (int)m_fMaxValue);
					UpdateTipText(hTip,m_editCtrl.m_hWnd,0,s);
					//BT5275
					/*CRect r;
					m_editCtrl.GetWindowRect(r);
					::SetCursorPos(r.left+9,r.top+r.Height()/2);*/
				}
			}
			else
				if( nValue < (int)m_fMinValue )	
				{
					if(!m_bKillFocus)
					{
						if(hTip)
						{
							EnableToolTip(hTip,TRUE);
						}

						try
						{
							_variant_t var( m_strEditText_new );
							var.ChangeType( VT_I4 );
							nValue = (int)var.lVal;
						}
						catch(...)
						{
						}
						bResult = FALSE;
						if(!m_bMinValue)
						{
							m_bMinValue=true;
							m_bMinMaxVerify=true;
							m_clEditBackgroundColor=(OLE_COLOR)RGB(255,60,60);
							CreateBackgroundBrushes( STL_FOR_EDIT );
							m_bMinMaxVerify=false;
							if(hTip)
							{
								CString s;
								s.Format("Min=%d, max=%d", (int)m_fMinValue, (int)m_fMaxValue);
								UpdateTipText(hTip,m_editCtrl.m_hWnd,0,s);
								//BT5275
								/*CRect r;
								m_editCtrl.GetWindowRect(r);
								::SetCursorPos(r.left+9,r.top+r.Height()/2);*/
							}
						}
					}
					else
					{
						bResult = FALSE;
						nValue=(int)m_fMinValue;
						m_bMinValue=false;
						m_clEditBackgroundColor=OLE_COLOR(0x80000000 + COLOR_WINDOW);
						CreateBackgroundBrushes( STL_FOR_EDIT );
						if(hTip)
						{
							EnableToolTip(hTip,FALSE);
						}
					}
				}
				else
					if(m_bMinValue && !m_bMinMaxVerify)
					{
						bResult = FALSE;
						m_bMinValue=false;
						m_clEditBackgroundColor=OLE_COLOR(0x80000000 + COLOR_WINDOW);
						CreateBackgroundBrushes( STL_FOR_EDIT );
						if(hTip)
						{
							EnableToolTip(hTip,FALSE);
						}
					}
		}		


		_variant_t var( (long)nValue );
		var.ChangeType( VT_BSTR );		
		strNewValue = var.bstrVal;		
	}
	if( m_DataTypeEnum == dtTypeFloat )
	{
		float fValue = 0;
		bool flag=false;
		while(!flag)
		{
			int i=m_strEditText_new.Find(" ",0);
			if(i!=-1)
			{
				bResult = FALSE;
				m_strEditText_new.Delete(i,1);
			}
			else
				flag=true;
		}
		try
		{
			_variant_t var( m_strEditText_new );
			var.ChangeType( VT_R4 );
			fValue = var.fltVal;
		}
		catch(...)
		{
			bResult = FALSE;
			if(m_strEditText_new=="")
				fValue = m_fMinValue;
			else
			{
				try
				{
					_variant_t var( m_strEditText );
					var.ChangeType( VT_R4 );
					fValue = var.fltVal;
				}
				catch(...)
				{
				}
			}
			//bResult = FALSE;
		}
		
		if( m_bValidationEnable )
		{
			/*if( fValue < m_fMinValue )								
			{
				fValue = m_fMinValue;				
				bResult = FALSE;
			}
				
			if( fValue > m_fMaxValue )
			{
				fValue = m_fMaxValue;
				bResult = FALSE;
			}*/
			if( /*fValue < m_fMinValue ||*/ fValue > m_fMaxValue)	
			{
				m_strEditText_new=m_strEditText;
				try
				{
					_variant_t var( m_strEditText_new );
					var.ChangeType( VT_R4 );
					fValue = var.fltVal;
				}
				catch(...)
				{
				}
				bResult = FALSE;
				if(hTip)
				{
					EnableToolTip(hTip,TRUE);
				}
				if(m_bMinValue && fValue > m_fMinValue)
				{
					m_bMinValue=false;
					m_clEditBackgroundColor=OLE_COLOR(0x80000000 + COLOR_WINDOW);
					CreateBackgroundBrushes( STL_FOR_EDIT );
				}
				if(hTip)
				{
					CString s;
					s.Format("Min=%f, max=%f", m_fMinValue, m_fMaxValue);
					UpdateTipText(hTip,m_editCtrl.m_hWnd,0,s);
					//BT5275
					/*CRect r;
					m_editCtrl.GetWindowRect(r);
					::SetCursorPos(r.left+9,r.top+r.Height()/2);*/
				}
			}
			else
				if( fValue < m_fMinValue )	
				{
					if(!m_bKillFocus)
					{
						try
						{
							_variant_t var( m_strEditText_new );
							var.ChangeType( VT_R4 );
							fValue = var.fltVal;
						}
						catch(...)
						{
						}
						bResult = FALSE;
						if(!m_bMinValue)
						{
							m_bMinValue=true;
							m_bMinMaxVerify=true;
							m_clEditBackgroundColor=(OLE_COLOR)RGB(255,60,60);
							CreateBackgroundBrushes( STL_FOR_EDIT );
							m_bMinMaxVerify=false;

							if(hTip)
							{
								CString s;
								s.Format("Min=%f, max=%f", m_fMinValue, m_fMaxValue);
								UpdateTipText(hTip,m_editCtrl.m_hWnd,0,s);
								//BT5275
								/*CRect r;
								m_editCtrl.GetWindowRect(r);
								::SetCursorPos(r.left+9,r.top+r.Height()/2);*/
							}
						}
					}
					else
					{
						bResult = FALSE;
						fValue = m_fMinValue;
						m_bMinValue=false;
						m_clEditBackgroundColor=OLE_COLOR(0x80000000 + COLOR_WINDOW);
						CreateBackgroundBrushes( STL_FOR_EDIT );
					}
				}
				else
					if(m_bMinValue && !m_bMinMaxVerify)
					{
						bResult = FALSE;
						m_bMinValue=false;
						m_clEditBackgroundColor=OLE_COLOR(0x80000000 + COLOR_WINDOW);
						CreateBackgroundBrushes( STL_FOR_EDIT );
					}

		}		
		_variant_t var( fValue );
		var.ChangeType( VT_BSTR );		
		strNewValue = var.bstrVal;
	}

	if( bResult == FALSE ) 
	{
		if (bUpdateWindowText)
		{
			m_bCanEnterValidation = FALSE;
			m_bInternalTextUpdate = true;
			m_editCtrl.SetWindowText( strNewValue );
			m_bInternalTextUpdate = false;
			if(!m_bMinValue && !m_bMinMaxVerify)
				bResult=TRUE;
		}
		m_strEditText_new = strNewValue;

		int iSelEnd=m_strEditText_new.GetLength();
		m_editCtrl.SetSel(iSelEnd,iSelEnd);
	}

	m_bCanEnterValidation = TRUE;
	m_strEditText=m_strEditText_new;
	return bResult;
}

void CVTEditor::OnEditCtrlKillFocus( )
{
	m_bKillFocus=true;
	BOOL bValid = Validation( true );
	if( bValid && !m_bInternalTextUpdate ) 
	{
		TCHAR buffer[255];
		m_editCtrl.GetWindowText( buffer, 255 );				
		CString m_strEditText_new = buffer;

		if(m_strEditText!=m_strEditText_new)
		FireDataChange();
	}
	m_bKillFocus=false;
}


void CVTEditor::OnFireDataChange()
{
	int iSelEnd, iSelStart, iLength, iNewLength;
	m_editCtrl.GetSel(iSelStart,iSelEnd);
	CString m_strEditText_new;
	m_editCtrl.GetWindowText( m_strEditText_new );				
	iLength=m_strEditText_new.GetLength();
	BOOL bValid = Validation(true );
	m_editCtrl.GetWindowText( m_strEditText_new );			
	iNewLength=m_strEditText_new.GetLength();
	if(iLength>iNewLength)
		m_editCtrl.SetSel(iSelEnd-1,iSelEnd-1);

	//if( m_bAutoUpdate && bValid )
	//	WriteToShellData();

	if( bValid && !m_bInternalTextUpdate ) 
		FireDataChange();
}


DWORD CVTEditor::GetControlFlags()
{
	return COleControl::GetControlFlags();// | pointerInactive );
}

DWORD CVTEditor::GetActivationPolicy()
{
	return COleControl::GetActivationPolicy();//POINTERINACTIVE_ACTIVATEONDRAG;
}

BSTR CVTEditor::GetText() 
{
	CString strResult = m_strEditText;
	return strResult.AllocSysString();
}

void CVTEditor::SetText(LPCTSTR lpszNewValue) 
{
	m_bInternalTextUpdate = true;
	m_strEditText = (LPCTSTR)lpszNewValue;
	if( m_editCtrl.GetSafeHwnd() )
	{
		if(m_DataTypeEnum == dtTypeFloat)
		{
			double fValue=atof(m_strEditText);
			if(m_nCharAFP>=0)
			{
				CString s,s_;
				s_.Format("%d",m_nCharAFP);
				s="%."+s_+"f";
				m_strEditText.Format(s,fValue);
			}
			else
				m_strEditText.Format("%f",fValue);
		}
		else
			if(m_DataTypeEnum == dtTypeInt)
			{
				int nValue=atoi(m_strEditText);
				m_strEditText.Format("%d",nValue);
			}
		m_editCtrl.SetWindowText( m_strEditText );
	}
	//m_editCtrl.Invalidate();
	//m_editCtrl.UpdateWindow();
	//InvalidateControl();
	SetModifiedFlag();
	m_bInternalTextUpdate = false;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//
//	
//		
//			
//		Acceleration key support
//
//		
//		
//			
//
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BOOL CVTEditor::FindAccelKey()
{
	m_cAccelKey = 0;

	int nPos = m_strCaptionText.Find( "&" );
	if( (nPos != -1) && nPos < (m_strCaptionText.GetLength() - 1) )
	{
		m_cAccelKey = (char)m_strCaptionText[nPos+1];
		ControlInfoChanged( );
		return TRUE;
	}
	return FALSE;

}



void CVTEditor::OnGetControlInfo(LPCONTROLINFO pControlInfo)
{
	
	if( hAccel ) DestroyAcceleratorTable( hAccel );

	
	TCHAR ch = (TCHAR)m_cAccelKey;
                    

	ACCEL accKey[1];

	accKey[0].fVirt	= FVIRTKEY | FALT;	
	accKey[0].key	= LOBYTE(VkKeyScan(ch));
	accKey[0].cmd	= 1;

	// Create the accel table.
	hAccel = CreateAcceleratorTable(accKey, 1);	

	if (hAccel != NULL)
	{
		// Fill in the CONTROLINFO structure passed in.
		pControlInfo -> hAccel = hAccel;
		pControlInfo -> cAccel = 1;
		pControlInfo -> dwFlags = 0;
	}
	else
	{
		COleControl::OnGetControlInfo(pControlInfo);
	}

}


void CVTEditor::OnMnemonic(LPMSG pMsg)
{ 
	if ((pMsg->message == WM_SYSKEYDOWN)||
		(pMsg->message == WM_SYSCHAR))
	{						 
		
		CString str1,str2;
		
		str1 = str2 = m_cAccelKey;
		str1.MakeLower(); 
		str2.MakeUpper(); 	

		if( (str1.GetLength() == 1)  && (str2.GetLength() == 1) )
		{
			if( ((char)pMsg->wParam == str1[0] ) || 
				((char)pMsg->wParam == str2[0] ))
			{			
				m_editCtrl.SetFocus( );
				return;
			}
		}
	}
	COleControl::OnMnemonic(pMsg);

} 

BOOL CVTEditor::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_SYSKEYDOWN) {
		CString str1,str2;
		
		str1 = str2 = m_cAccelKey;
		str1.MakeLower(); 
		str2.MakeUpper(); 	

		if( (str1.GetLength() == 1)  && (str2.GetLength() == 1) )
		{
			if( ((char)pMsg->wParam == str1[0] ) || 
				((char)pMsg->wParam == str2[0] ))
			{			
				m_editCtrl.SetFocus( );
				return TRUE;
			}
		}
	}
	return COleControl::PreTranslateMessage(pMsg);	
}




void CVTEditor::SwitchMultiSingleLine()
{

	if( !m_bWasCreated ) return;
		

	DWORD dwStyle = m_editCtrl.GetStyle( );
	DWORD dwNewStyle;
	if( m_bMultiLine )							
	{
		if( dwStyle & ES_MULTILINE)	
			return;		
		dwNewStyle =	WS_CHILD | WS_VISIBLE | 
						WS_TABSTOP | WS_BORDER | 
						ES_AUTOHSCROLL | ES_AUTOVSCROLL |
						ES_MULTILINE | WS_VSCROLL;
	}		
	else
	{
		if( !(dwStyle & ES_MULTILINE))
			return;
		dwNewStyle =	WS_CHILD | WS_VISIBLE | 
						WS_TABSTOP | WS_BORDER | 
						ES_AUTOHSCROLL | ES_AUTOVSCROLL;	

	}

	m_editCtrl.DestroyWindow();

	m_editCtrl.Create(	
						dwNewStyle,
						CRect( 0, 0, 0, 0 ),
						this,
						ID_EDIT_CTRL 
						);


	CreateBackgroundBrushes( STL_FOR_EDIT );	
	
	SetFontToControl( STL_FOR_EDIT );	

}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//
//	
//		
//			
//		Read/Set property
//
//		For: Property support
//		
//			
//
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
BSTR CVTEditor::GetName() 
{
	CString strResult = m_strName;
	return strResult.AllocSysString();
}

void CVTEditor::SetName(LPCTSTR lpszNewValue) 
{
	m_strName = lpszNewValue;
	InvalidateAndSetModifiedFlag( );
}

short CVTEditor::GetCaptionPosition() 
{	
	if( m_CaptionAlignEnum == caAlignUp )
		return 0;

	if( m_CaptionAlignEnum == caAlignLeft )
		return 1;

	return -1;
}

void CVTEditor::SetCaptionPosition(short nNewValue) 
{
	if( nNewValue == 0 ) m_CaptionAlignEnum = caAlignUp;		

	if( nNewValue == 1 ) m_CaptionAlignEnum = caAlignLeft;		

	InvalidateAndSetModifiedFlag( );

}

short CVTEditor::GetDataType() 
{
	if( m_DataTypeEnum == dtTypeString )
		return 0;
	
	if( m_DataTypeEnum == dtTypeInt )
		return 1;
	
	if( m_DataTypeEnum == dtTypeFloat )
		return 2;
	
	return -1;
}

void CVTEditor::SetDataType(short nNewValue) 
{
	if( nNewValue == 0 ) m_DataTypeEnum = dtTypeString;
	
	if( nNewValue == 1 ) m_DataTypeEnum = dtTypeInt;
	
	if( nNewValue == 2 ) m_DataTypeEnum = dtTypeFloat;

	InvalidateAndSetModifiedFlag( );
}

BSTR CVTEditor::GetCaptionText() 
{
	CString strResult = m_strCaptionText;	
	return strResult.AllocSysString();
}

void CVTEditor::SetCaptionText(LPCTSTR lpszNewValue) 
{
	m_strCaptionText = lpszNewValue;
	FindAccelKey();
	InvalidateAndSetModifiedFlag( );
}

short CVTEditor::GetEditFieldWidth() 
{		
	return m_nCaptionFieldWidth;
}

void CVTEditor::SetEditFieldWidth(short nNewValue) 
{
	m_nCaptionFieldWidth = nNewValue;	
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetSpinEnable() 
{	
	return m_bSpinEnable;
}

void CVTEditor::SetSpinEnable(BOOL bNewValue) 
{
	m_bSpinEnable = bNewValue;
	InvalidateAndSetModifiedFlag( );
}

float CVTEditor::GetMinValue() 
{		
	return m_fMinValue;
}

void CVTEditor::SetMinValue(float newValue) 
{
	m_fMinValue = newValue;
	InvalidateAndSetModifiedFlag( );
}

float CVTEditor::GetMaxValue() 
{
	return m_fMaxValue;
}

void CVTEditor::SetMaxValue(float newValue) 
{
	m_fMaxValue = newValue;
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetValidationEnable() 
{
	return m_bValidationEnable;
}

void CVTEditor::SetValidationEnable(BOOL bNewValue) 
{
	m_bValidationEnable = bNewValue;
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetCaptionEnable() 
{
	return m_bCaptionEnable;
}

void CVTEditor::SetCaptionEnable(BOOL bNewValue) 
{
	m_bCaptionEnable = bNewValue;
	InvalidateAndSetModifiedFlag( );
}


OLE_COLOR CVTEditor::GetBackgroundColor() 
{	
	return m_clBackgroundColor;
}			   

void CVTEditor::SetBackgroundColor(OLE_COLOR nNewValue) 
{
	m_clBackgroundColor = nNewValue;
	InvalidateAndSetModifiedFlag( );
}

	
OLE_COLOR CVTEditor::GetCaptionBackColor() 
{
	return m_clCaptionBackgroundColor;
}

void CVTEditor::SetCaptionBackColor(OLE_COLOR nNewValue) 
{
	m_clCaptionBackgroundColor = nNewValue;
	CreateBackgroundBrushes( STL_FOR_CAPTION );
	InvalidateAndSetModifiedFlag( );
}

OLE_COLOR CVTEditor::GetCaptionTextColor() 
{
	return m_clCaptionTextColor;
}

void CVTEditor::SetCaptionTextColor(OLE_COLOR nNewValue) 
{
	m_clCaptionTextColor = nNewValue;
	InvalidateAndSetModifiedFlag( );
}

OLE_COLOR CVTEditor::GetEditBackColor() 
{
	return m_clEditBackgroundColor;
}

void CVTEditor::SetEditBackColor(OLE_COLOR nNewValue) 
{
	m_clEditBackgroundColor = nNewValue;
	CreateBackgroundBrushes( STL_FOR_EDIT );
	InvalidateAndSetModifiedFlag( );
}

OLE_COLOR CVTEditor::GetEditTextColor() 
{
	return m_clEditTextColor;
}

void CVTEditor::SetEditTextColor(OLE_COLOR nNewValue) 
{
	m_clEditTextColor = nNewValue;
	InvalidateAndSetModifiedFlag( );
}


LPFONTDISP CVTEditor::GetEditFont() 
{	//COleControl
	//m_fontEdit

	FONTDESC fd;
	LOGFONT lf;

	memcpy( &lf, &m_logfontEdit, sizeof(LOGFONT) );

	ZeroMemory( &fd, sizeof(fd) );	

	CString strFontName = lf.lfFaceName;

	fd.cbSizeofstruct	= sizeof( fd );
	
	fd.lpstrName		= strFontName.AllocSysString( ); 	

	fd.cySize.Lo = (unsigned long)(lf.lfHeight > 0 ? lf.lfHeight : -lf.lfHeight )
						* 10000L * 72L / 96L;
	fd.cySize.Hi = 0;

	fd.sWeight			= (SHORT)lf.lfWeight;
	fd.sCharset			= (SHORT)lf.lfCharSet;
	fd.fItalic			= (BOOL)lf.lfItalic;
	fd.fUnderline		= (BOOL)lf.lfUnderline;
	fd.fStrikethrough	= (BOOL)lf.lfStrikeOut;

	m_fontHolderEdit.ReleaseFont( );
	
	m_fontHolderEdit.InitializeFont( &fd );

	return m_fontHolderEdit.GetFontDispatch( );
}

void CVTEditor::SetEditFont(LPFONTDISP newValue) 
{
	m_fontHolderEdit.ReleaseFont( );
	
	m_fontHolderEdit.InitializeFont( NULL, newValue );

	HFONT hFont = m_fontHolderEdit.GetFontHandle( );
	CFont* pFont = CFont::FromHandle( hFont );

	pFont->GetLogFont( &m_logfontEdit );
	m_logfontEdit.lfCharSet = DEFAULT_CHARSET;

	SetFontToControl( STL_FOR_EDIT );	
	InvalidateAndSetModifiedFlag( );
}


LPFONTDISP CVTEditor::GetCaptionFont() 
{
	
	
	FONTDESC fd;
	LOGFONT lf;

	ZeroMemory( &fd, sizeof(fd) );

	memcpy( &lf, &m_logfontCaption, sizeof(LOGFONT) );

	
	CString strFontName = lf.lfFaceName;

	fd.cbSizeofstruct	= sizeof( fd );
	
	fd.lpstrName		= strFontName.AllocSysString( ); 

	fd.cySize.Lo = (unsigned long)(lf.lfHeight > 0 ? lf.lfHeight : -lf.lfHeight )
						* 10000L * 72L / 96L;
	fd.cySize.Hi = 0;

	fd.sWeight			= (SHORT)lf.lfWeight;
	fd.sCharset			= (SHORT)lf.lfCharSet;
	fd.fItalic			= (BOOL)lf.lfItalic;
	fd.fUnderline		= (BOOL)lf.lfUnderline;
	fd.fStrikethrough	= (BOOL)lf.lfStrikeOut;

	m_fontHolderCaption.ReleaseFont( );
	
	m_fontHolderCaption.InitializeFont( &fd );

	return m_fontHolderCaption.GetFontDispatch( );
}

void CVTEditor::SetCaptionFont(LPFONTDISP newValue) 
{
	m_fontHolderCaption.ReleaseFont( );
	
	m_fontHolderCaption.InitializeFont( NULL, newValue );
	

	HFONT hFont = m_fontHolderCaption.GetFontHandle( );
	CFont* pFont = CFont::FromHandle( hFont );

		
	pFont->GetLogFont( &m_logfontCaption );	
	m_logfontCaption.lfCharSet = DEFAULT_CHARSET;

	SetFontToControl( STL_FOR_CAPTION );	
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetCaptionAutoWidth() 
{
	return m_bCaptionAutoSize;
}

void CVTEditor::SetCaptionAutoWidth(BOOL bNewValue) 
{	
	m_bCaptionAutoSize = bNewValue;
	InvalidateAndSetModifiedFlag( );	
}


BSTR CVTEditor::GetKey() 
{
	CString strResult = m_strKeyValue;
	return strResult.AllocSysString();
}

void CVTEditor::SetKey(LPCTSTR lpszNewValue) 
{
	m_strKeyValue = lpszNewValue;

	SetModifiedFlag();
}

BOOL CVTEditor::GetAutoUpdate() 
{
	return m_bAutoUpdate;
}

void CVTEditor::SetAutoUpdate(BOOL bNewValue) 
{
	m_bAutoUpdate = bNewValue;

	SetModifiedFlag();
}

BOOL CVTEditor::GetAlignClient() 
{
	return m_bAlignClient;
}

void CVTEditor::SetAlignClient(BOOL bNewValue) 
{
	m_bAlignClient = bNewValue;
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetMultiLine() 
{
	return m_bMultiLine;
}

void CVTEditor::SetMultiLine(BOOL bNewValue) 
{
	m_bMultiLine = bNewValue;
	SwitchMultiSingleLine();
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetInnerRaised() 
{
	return m_bInnerRaised;
}

void CVTEditor::SetInnerRaised(BOOL bNewValue) 
{
	m_bInnerRaised = bNewValue;
	InvalidateAndSetModifiedFlag( );
}


BOOL CVTEditor::GetInnerSunken() 
{
	return m_bInnerSunken;
}

void CVTEditor::SetInnerSunken(BOOL bNewValue) 
{
	m_bInnerSunken = bNewValue;
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetOuterRaised() 
{	
	return m_bOuterRaised;
}

void CVTEditor::SetOuterRaised(BOOL bNewValue) 
{
	m_bOuterRaised = bNewValue;
	InvalidateAndSetModifiedFlag( );
}

BOOL CVTEditor::GetOuterSunken() 
{
	return m_bOuterSunken;
}

void CVTEditor::SetOuterSunken(BOOL bNewValue) 
{
	m_bOuterSunken = bNewValue;

	InvalidateAndSetModifiedFlag( );
}

/*
	CString strResult = m_strEditText;
	return strResult.AllocSysString();
}

void CVTEditor::SetText(LPCTSTR lpszNewValue) 
{
	m_strEditText = lpszNewValue;
	m_editCtrl.SetWindowText( m_strEditText );
	SetModifiedFlag();

*/
long CVTEditor::GetValueInt() 
{
	long nValue = 0;
	if( m_DataTypeEnum == dtTypeInt )
	{
		CString strEdit;
		m_editCtrl.GetWindowText( strEdit );
		try
		{
			_variant_t var( strEdit );
			var.ChangeType( VT_I4 );
			nValue = var.lVal;
		}
		catch(...)
		{
			nValue = 0;
		}
	}
	else
		if(m_DataTypeEnum == dtTypeFloat )
		{
			CString strEdit;
			m_editCtrl.GetWindowText( strEdit );
			try
			{
				_variant_t var( strEdit );
				var.ChangeType( VT_R8 );				
				nValue = (int)var.dblVal;				
			}
			catch(...)
			{
				nValue = 0;
			}
		}
		else
			if(m_DataTypeEnum == dtTypeString)
			{
				CString strEdit;
				m_editCtrl.GetWindowText( strEdit );
				nValue=atoi(strEdit);
			}
	
	return nValue;
}

void CVTEditor::SetValueInt(long nNewValue) 
{

	//if( m_DataTypeEnum != dtTypeInt )
	//	return;
	if(m_DataTypeEnum == dtTypeInt)
	{
		try
		{
			_variant_t var( nNewValue );

			var.ChangeType( VT_BSTR );		
			m_strEditText = var.bstrVal;		
		}
		catch(...)
		{
			m_strEditText = "0";
		}
	}
	else
		if(m_DataTypeEnum == dtTypeFloat)
		{
			try
			{
				_variant_t var( (double)nNewValue );
				var.ChangeType( VT_BSTR );		
				m_strEditText = var.bstrVal;		
			}
			catch(...)
			{
				m_strEditText = "0";
			}
		}
		else
			if(m_DataTypeEnum == dtTypeString)
			{
				m_strEditText.Format("%d",nNewValue);
			}
	CString sPrev;
	m_editCtrl.GetWindowText(sPrev);
	if (sPrev == m_strEditText)
		return;
	m_bInternalTextUpdate = true;
	m_editCtrl.SetWindowText( m_strEditText );
	m_bInternalTextUpdate = false;
	//[AY] - disable updatinf control
	//m_editCtrl.Invalidate();
	//m_editCtrl.UpdateWindow();
	InvalidateControl();
	SetModifiedFlag();
//	m_editCtrl.SendMessage(EM_SETSEL,0,-1);
//	m_editCtrl.SendMessage(EM_SETSEL,-1,0);
}

double CVTEditor::GetValueDouble() 
{
	double fValue = 0.0;
	if( m_DataTypeEnum == dtTypeFloat )
	{
//		CString strEdit;
//		m_editCtrl.GetWindowText( strEdit );
		try
		{
			_variant_t var( m_strEditText );
			var.ChangeType( VT_R8 );				
			fValue = var.dblVal;				
		}
		catch(...)
		{
			fValue = 0.0;
		}
	}
	else
		if( m_DataTypeEnum == dtTypeInt )
		{
			try
			{
				_variant_t var( m_strEditText );
				var.ChangeType( VT_I4 );
				fValue = (double)var.lVal;
			}
			catch(...)
			{
				fValue = 0.0;
			}
		}
		else
			if(m_DataTypeEnum == dtTypeString)
			{
				fValue=atof(m_strEditText);
			}

	/*
	char sz_buf[200];
	sprintf( sz_buf, "\nCVTEditor::GetValueDouble -> %f", fValue );
	TRACE( sz_buf );
	*/
	
	
	return fValue;
}

void CVTEditor::SetValueDouble(double newValue) 
{
	//if( m_DataTypeEnum != dtTypeFloat )
	//	return;
	if( m_DataTypeEnum == dtTypeFloat )
	{
		try
		{
			_variant_t var( newValue );
			var.ChangeType( VT_BSTR );		
			m_strEditText = var.bstrVal;		
		}
		catch(...)
		{
			m_strEditText = "0";
		}
	}
	else
		if(m_DataTypeEnum == dtTypeInt)
		{
			try
			{
				_variant_t var( (int)newValue );
	
				var.ChangeType( VT_BSTR );		
				m_strEditText = var.bstrVal;		
			}
			catch(...)
			{
				m_strEditText = "0";
			}
		}
		else
			if(m_DataTypeEnum == dtTypeString)
			{
				m_strEditText.Format("%f",newValue);
			}


	m_bInternalTextUpdate = true;
	m_editCtrl.SetWindowText( m_strEditText );
	m_bInternalTextUpdate = false;
	//[AY] - disable updatinf control
	//m_editCtrl.Invalidate();
	//m_editCtrl.UpdateWindow();
	InvalidateControl();
	SetModifiedFlag();

}



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//
//	
//		
//			
//		Shell support
//
//		For: Shell support
//		
//			
//
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#ifndef FOR_HOME_WORK
HRESULT CVTEditor::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CVTEditor, ActiveXCtrl)

	pThis->m_ptrSite = punkSite;
	pThis->m_ptrApp = punkVtApp;

	pThis->RestoreFromShellData();

	return S_OK;
}
  

HRESULT CVTEditor::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CVTEditor, ActiveXCtrl)	
	*pbstrName = pThis->m_strName.AllocSysString();
	return S_OK;
}

HRESULT CVTEditor::XActiveXCtrl::SetDoc( IUnknown *punkDoc )
{
	METHOD_PROLOGUE_EX(CVTEditor, ActiveXCtrl)	
	return S_OK;
}

HRESULT CVTEditor::XActiveXCtrl::SetApp( IUnknown *punkVtApp )
{
	METHOD_PROLOGUE_EX(CVTEditor, ActiveXCtrl)	
	return S_OK;
}

HRESULT CVTEditor::XActiveXCtrl::OnOK( )
{
	METHOD_PROLOGUE_EX(CVTEditor, ActiveXCtrl)

	if( pThis->m_bAutoUpdate )
		pThis->StoreToData();

	return S_OK;
}

HRESULT CVTEditor::XActiveXCtrl::OnApply( )
{
	METHOD_PROLOGUE_EX(CVTEditor, ActiveXCtrl)

	OnOK( );

	return S_OK;
}

HRESULT CVTEditor::XActiveXCtrl::OnCancel( )
{
	METHOD_PROLOGUE_EX(CVTEditor, ActiveXCtrl)

	return S_OK;
}

#endif

BOOL CVTEditor::WriteToShellData()
{	
#ifndef FOR_HOME_WORK

	if( m_ptrApp == NULL ) return FALSE;
		

	CString strSection;
	CString strEntry;

	if( !::GetSectionEntry( m_strKeyValue, strSection, strEntry ) )
		return FALSE;

	if( m_DataTypeEnum == dtTypeString )
		_SetValue( m_ptrApp, strSection, strEntry, 
								m_strEditText );

	if( m_DataTypeEnum == dtTypeInt )
	{
		long nValue = 0;
		try
		{
			_variant_t var( m_strEditText );
			var.ChangeType( VT_I4 );
			nValue = var.lVal;
		}
		catch(...)
		{

		}
		_SetValueInt( m_ptrApp, strSection, strEntry,  nValue );
	}

	if( m_DataTypeEnum == dtTypeFloat )
	{	
		float fValue = 0.0f;
		try
		{
			_variant_t var( m_strEditText );
			var.ChangeType( VT_R4 );				
			fValue = var.fltVal;				
		}
		catch(...)
		{

		}
		_SetValueDouble( m_ptrApp, strSection, strEntry,  (double)fValue );
	}	

#endif
	return TRUE;
}
BOOL CVTEditor::RestoreFromShellData()
{
#ifndef FOR_HOME_WORK
	if( m_ptrApp == NULL ) return FALSE;


	CString strSection;
	CString strEntry;

	if( !::GetSectionEntry( m_strKeyValue, strSection, strEntry ) )
		return FALSE;

	if( m_DataTypeEnum == dtTypeString )
		m_strEditText = _GetValueString( m_ptrApp, strSection, strEntry);

	if( m_DataTypeEnum == dtTypeInt )
	{
		long nValue = _GetValueInt( m_ptrApp, strSection, strEntry );

		try
		{
			_variant_t var( nValue );
			var.ChangeType( VT_BSTR );		
			m_strEditText = var.bstrVal;		
		}
		catch(...)
		{
			m_strEditText = "0";
		}
		

	}

	if( m_DataTypeEnum == dtTypeFloat )
	{
		double fValue = _GetValueDouble( m_ptrApp, strSection, strEntry );

		try
		{
			_variant_t var( fValue );
			var.ChangeType( VT_BSTR );		
			m_strEditText = var.bstrVal;		
		}
		catch(...)
		{
			m_strEditText = "0";
		}

	}


	m_bInternalTextUpdate = true;
	m_editCtrl.SetWindowText( m_strEditText );
	m_bInternalTextUpdate = false;
	RepaintControl();
	
#endif
	return TRUE;
}


void CVTEditor::StoreToData() 
{		
	if( Validation( false ) )
		WriteToShellData();	
}



BOOL CVTEditor::SetAutoSize() 
{
	int nAddSize = 0;

	UINT	edgeType = 0;
	if(m_bInnerRaised)
		edgeType |= BDR_RAISEDINNER;
	if(m_bInnerSunken)
		edgeType |= BDR_SUNKENINNER;
	if(m_bOuterRaised)
		edgeType |= BDR_RAISEDOUTER;
	if(m_bOuterSunken)
		edgeType |= BDR_SUNKENOUTER;
	if(edgeType != 0)
	{
		nAddSize = 2 * VT_BORDER_WIDTH;
	}
	
	int nControlHeight, nControlWidth;

	
	CClientDC dc(this);
	TEXTMETRIC tm;

	CFont font;
	::GetFontFromDispatch( this, font, TRUE );
	LOGFONT lfDefault;
	::ZeroMemory( &lfDefault, sizeof(LOGFONT) );
	font.GetLogFont( &lfDefault );


	//Determine  nEditCtrlHeight
	CFont tmpFontEdit, tmpFontCaption;
	tmpFontEdit.CreateFontIndirect( m_bUseSystemFont ? &lfDefault : &m_logfontEdit );
	tmpFontCaption.CreateFontIndirect( m_bUseSystemFont ? &lfDefault : &m_logfontCaption );

	CFont* pOldFont = (CFont*)dc.SelectObject( &tmpFontEdit );
	dc.GetTextMetrics( &tm );					
	int nEditCtrlHeight = tm.tmHeight + 2 * ::GetSystemMetrics( SM_CYEDGE ) + 8;

	//Determine  nCaptionCtrlWidth & nCaptionCtrlHeight
	CRect rectCalc;
	dc.SelectObject( &tmpFontCaption );
	dc.DrawText( m_strCaptionText, &rectCalc, DT_CALCRECT );

	int nCaptionCtrlWidth	= rectCalc.Width( );
	int nCaptionCtrlHeight	= rectCalc.Height( );	

	m_nCaptionFieldWidth = nCaptionCtrlWidth;

	dc.SelectObject( pOldFont );


	if( m_CaptionAlignEnum == caAlignLeft )
	{
		nControlHeight = max( nCaptionCtrlHeight, nEditCtrlHeight ) + nAddSize;
		nControlWidth = nCaptionCtrlWidth + 50 + nAddSize;
	}
	else
	{
		nControlHeight = nCaptionCtrlHeight + nEditCtrlHeight + 5 + nAddSize;
		nControlWidth = nCaptionCtrlWidth + nAddSize;
	}	

	SetControlSize( nControlWidth, nControlHeight );
		
	InvalidateAndSetModifiedFlag( );
	//InvalidateAndSetModifiedFlag( );
	
	return TRUE;
}


LRESULT CVTEditor::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if( message == WM_GETTEXT )
	{
		lstrcpyn((LPTSTR)lParam, (LPCTSTR)m_strCaptionText, wParam);
		if ((int)wParam > m_strCaptionText.GetLength())
		wParam = m_strCaptionText.GetLength();
		return TRUE;    
	}

	if( message == WM_SETTEXT )
	{
		if (lParam == NULL)
		{
			// NULL lParam means set caption to nothing
			m_strCaptionText.Empty();
		}
		else
		{
			// non-NULL sets caption to that specified by lParam
			lstrcpy(m_strCaptionText.GetBufferSetLength(lstrlen((LPCTSTR)lParam)),
				(LPCTSTR)lParam);
		}
		
		InvalidateAndSetModifiedFlag( );		
		return TRUE;
	}
	return COleControl::DefWindowProc(message, wParam, lParam);
}


BOOL  CVTEditor::_GetClientRect( CRect* lpRect )
{
	if( !GetSafeHwnd() )
	{
		if( ::IsWindow(GetSafeHwnd()) )
		{
			COleControl::GetClientRect( lpRect );
			return TRUE;
		}
	}

	int nCx, nCy;
	GetControlSize( &nCx, &nCy );
	
	lpRect->top = lpRect->left = 0;
	lpRect->right = nCx;
	lpRect->bottom = nCy;

	return FALSE;
}


void CVTEditor::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);	
	m_editCtrl.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////////////
// function for visible & enable control status & SetFocus
/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTEditor::GetDisabled()
{
	return !::_IsWindowEnable( this );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::SetDisabled(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	EnableWindow( !bNewValue );

	m_editCtrl.EnableWindow( !bNewValue );
	m_captionCtrl.EnableWindow( !bNewValue );
	
	if( m_bSpinEnable )
		m_spinCtrl.EnableWindow( !bNewValue );
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTEditor::GetVisible()
{
	return ::_IsWindowVisible( &m_editCtrl );
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::SetVisible(BOOL bNewValue)
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;
	
	/*
	
	if( !bNewValue )		   
	{
		::SendMessage( m_hWnd, WM_SHOWWINDOW, (WPARAM)FALSE, (LPARAM)0L );
	}
	else
	{
		::SendMessage( m_hWnd, WM_SHOWWINDOW, (WPARAM)TRUE, (LPARAM)0L );
	}
	*/
	
		
		
	
	
	if( ::IsWindow( m_editCtrl.GetSafeHwnd() ) )
	{
		if( bNewValue )
		{
			m_editCtrl.ShowWindow( SW_SHOWNA );			
			ShowWindow( SW_SHOWNA );			
		}
		else
		{	
			m_editCtrl.ShowWindow( SW_HIDE );
			ShowWindow( SW_HIDE );			
		}
	}	
	

}

/////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::SetFocus()
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;
	
	CWnd::SetFocus( );
}


BOOL CVTEditor::OnCommand(WPARAM wParam, LPARAM lParam) 
{	
	/*
	if( (LOWORD(wParam) == ID_EDIT_CTRL) && (HIWORD(wParam) == EN_CHANGE) )
	{
		OnFireDataChange();
	}
	*/
	return COleControl::OnCommand(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTEditor::GetUseSystemFont() 
{
	return m_bUseSystemFont;
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::SetUseSystemFont(BOOL bNewValue)
{
	m_bUseSystemFont = bNewValue;
	SetModifiedFlag();
	
	SetFontToControl( STL_FOR_CAPTION | STL_FOR_EDIT );	
	
	InvalidateControl();
	
}


/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTEditor::GetVT5StaticStyle() 
{
	return m_bVTStaticStyle;
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTEditor::SetVT5StaticStyle(BOOL bNewValue)
{
	if (m_bVTStaticStyle != bNewValue)
	{
		m_bVTStaticStyle = bNewValue;
		CString sText;
		CRect rect;
		m_captionCtrl.GetWindowText(sText);
		m_captionCtrl.GetWindowRect(rect);
		ScreenToClient(rect);
		BOOL bEnable = m_captionCtrl.IsWindowEnabled();
		m_captionCtrl.DestroyWindow();
		DWORD dwStyle = WS_CHILD|WS_VISIBLE;
		if (m_bVTStaticStyle) dwStyle |= SS_OWNERDRAW;
		if (!bEnable) dwStyle |= WS_DISABLED;
		m_captionCtrl.Create(sText, dwStyle, rect, this, ID_STATIC_CTRL);
		BuildAppearence();
	}	
}

void CVTEditor::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlID == ID_STATIC_CTRL)
	{
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		BOOL bEnabled = ::IsWindowEnabled(lpDrawItemStruct->hwndItem);
		if (bEnabled)
			pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));		
		else
			pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		CString strTextOut;
		m_captionCtrl.GetWindowText(strTextOut);
		// vk
		COLORREF crBrushColor = pDC->GetBkColor();
		CBrush newBrush;
		newBrush.CreateSolidBrush( crBrushColor );
		pDC->FillRect( &lpDrawItemStruct->rcItem, &newBrush );
		// end vk
		pDC->DrawText(strTextOut, &lpDrawItemStruct->rcItem, DT_END_ELLIPSIS);
	}
	else
		COleControl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CVTEditor::SetNumberCharAFP(long nCharAFP)
{
	m_nCharAFP=nCharAFP;
}

long CVTEditor::GetNumberCharAFP()
{
	return m_nCharAFP;
}