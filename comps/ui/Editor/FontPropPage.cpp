// CommentFontPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "FontPropPage.h"

#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//lfHeight = -MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);

/////////////////////////////////////////////////////////////////////////////
// CCommentFontPropPage dialog
IMPLEMENT_DYNCREATE(CCommentFontPropPage, CDialog)

// {61CD6272-A3E3-11d3-A53C-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CCommentFontPropPage, szEditorFontPropPageProgID, 
0x61cd6272, 0xa3e3, 0x11d3, 0xa5, 0x3c, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);


BEGIN_INTERFACE_MAP(CCommentFontPropPage, CPropertyPageBase)
	INTERFACE_PART(CCommentFontPropPage, IID_IFontPropPage, FontPage)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////////////////
//IFontPropPage interface implementation
IMPLEMENT_UNKNOWN(CCommentFontPropPage, FontPage);
HRESULT CCommentFontPropPage::XFontPage::SetFont(LOGFONT*	pLogFont, int nAlign)
{
	_try_nested(CCommentFontPropPage, FontPage, SetFont)
	{
		pThis->_SetFont(&pLogFont, nAlign);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentFontPropPage::XFontPage::SetColorsAndStuff(OLE_COLOR colorText, OLE_COLOR colorBack, BOOL bBack, BOOL bArrow)
{
	_try_nested(CCommentFontPropPage, FontPage, SetColorsAndStuff)
	{
		pThis->m_clrBack.SetColour((COLORREF)colorBack);
		pThis->m_strFontViewer.SetBackColor((COLORREF)colorBack);
		pThis->m_clrText.SetColour((COLORREF)colorText);
		pThis->m_strFontViewer.SetTextColor((COLORREF)colorText);

		pThis->m_bBack = bBack;
		pThis->m_bArrow = bArrow;
		pThis->m_strFontViewer.SetTransparent(!pThis->m_bBack);
		pThis->m_checkArrow.SetCheck(pThis->m_bArrow?1:0);
		pThis->m_checkBack.SetCheck(pThis->m_bBack?1:0);		

		pThis->m_backColor = (COLORREF)colorBack;
		pThis->m_color = (COLORREF)colorText;

		::SetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", colorText);	
		::SetValueColor(GetAppUnknown(), "Editor", "CommentText_Back", colorBack);    
		::SetValue(GetAppUnknown(), "Editor", "TransparentCommentText", _variant_t((long)(bBack?0:1)));
		::SetValue(GetAppUnknown(), "Editor", "CommentText_DrawArrow", _variant_t((long)(bArrow?1:0)));

		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentFontPropPage::XFontPage::GetColorsAndStuff(OLE_COLOR* pcolorText, OLE_COLOR* pcolorBack, BOOL* pbBack, BOOL* pbArrow)
{
	_try_nested(CCommentFontPropPage, FontPage, GetColorsAndStuff)
	{
		(OLE_COLOR)pThis->m_color	  = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", (OLE_COLOR)pThis->m_color);	
		(OLE_COLOR)pThis->m_backColor = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Back", (OLE_COLOR)pThis->m_backColor);    
		pThis->m_bBack				  = ::GetValueInt(GetAppUnknown(), "Editor", "TransparentCommentText", _variant_t((long)(pThis->m_bBack?0:1))) == 0;
		pThis->m_bArrow				  = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_DrawArrow", _variant_t((long)(pThis->m_bArrow?1:0))) == 1;

		if(pcolorBack)
			*pcolorBack = (OLE_COLOR)pThis->m_backColor;
		if(pcolorText)
			*pcolorText  = (OLE_COLOR)pThis->m_color;
		if(pbBack)
			*pbBack = pThis->m_bBack;
		if(pbArrow)
			*pbArrow = pThis->m_bArrow;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommentFontPropPage::XFontPage::GetFont(LOGFONT*	pLogFont, int* pnAlign)
{
	_try_nested(CCommentFontPropPage, FontPage, GetFont)
	{
		pThis->_GetFont(&pLogFont, &pnAlign);
		return S_OK;
	}
	_catch_nested;
}



CCommentFontPropPage::CCommentFontPropPage(CWnd* pParent /*=NULL*/)
	: CPropertyPageBase(CCommentFontPropPage::IDD)
{
	//{{AFX_DATA_INIT(CCommentFontPropPage)
	m_bArrow = FALSE;
	m_bBack = FALSE;
	m_nFontSize = -1;
	//}}AFX_DATA_INIT
	m_sName = c_szCCommentFontPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_FONT );
	//m_strCurFontFamily = "";
	m_nCurAlign = 0;

	m_bStrikeOut = false;

	//_InitParams();
}

void CCommentFontPropPage::_SetFont(LOGFONT** pplogFont, int nAlign)
{
	CString strName = (*pplogFont)->lfFaceName;
	m_strCurFontFamily = strName;
	CString str1;
	COMBOBOXEXITEM cbItem1;
	int nIdx = 0;
	int nNumItems = m_cbFont.GetCount();
	for(int j = 0; j < nNumItems; j++)
	{
		cbItem1.mask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
		cbItem1.iItem = j;
		cbItem1.pszText = str1.GetBuffer(255);
		cbItem1.cchTextMax = 255;
		m_cbFont.GetItem(&cbItem1);
		if(strName == str1)
			nIdx = j;
		str1.ReleaseBuffer();
	}
	m_cbFont.SetCurSel(nIdx);
	if((*pplogFont)->lfWeight == FW_BOLD)
	{
		m_pbBold.SetPressedState();
		m_bBold = TRUE;
	}
	else
	{
		m_pbBold.ResetPressedState();
		m_bBold = FALSE;
	}
	if((*pplogFont)->lfItalic)
	{
		m_pbItalic.SetPressedState();
		m_bItalic = TRUE;
	}
	else
	{
		m_pbItalic.ResetPressedState();
		m_bItalic = FALSE;
	}
	if((*pplogFont)->lfUnderline)
	{
		m_pbUnder.SetPressedState();
		m_bUnder = TRUE;
	}
	else
	{
		m_pbUnder.ResetPressedState();
		m_bUnder = FALSE;
	}


	CClientDC dc(0);
	int  nSize = MulDiv((*pplogFont)->lfHeight, 72, GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY));
	str1.Format("%d", nSize);

	if(m_cbFontSize.SelectString(-1, str1) == CB_ERR )
		m_cbFontSize.SelectString(-1, "12");

	m_nCurHeight = (*pplogFont)->lfHeight;

	m_nCurAlign = nAlign;
	m_pbAlignLeft.ResetPressedState();
	m_pbAlignCenter.ResetPressedState();
	m_pbAlignRight.ResetPressedState();

	::SetValue(GetAppUnknown(), "Editor", "CommentText_Name", _variant_t(strName));
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Height", _variant_t((long)m_nCurHeight));	
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Bold", _variant_t((long)(m_bBold==TRUE?1:0)));
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Italic", _variant_t((long)(m_bItalic==TRUE?1:0)));
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Underline", _variant_t((long)(m_bUnder==TRUE?1:0)));
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Align", _variant_t((long)m_nCurAlign));
	::SetValue(GetAppUnknown(), "Editor", "CommentText_StrikeOut", _variant_t((long)m_bStrikeOut));			

	_ManageButtonState(m_nCurAlign);
	_SetFontPreview();

}

void CCommentFontPropPage::_GetFont(LOGFONT** ppLogFont, int** ppnAlign)
{
	/*CString strName;1
	m_cbFont.GetLBText(m_cbFont.GetComboBoxCtrl( )->GetCurSel(), strName);
	CString strSize;
	m_cbFontSize.GetLBText(m_cbFontSize.GetCurSel(), strSize);
	long nSize;
	VERIFY(sscanf(strSize, "%d", &nSize)==1);
	*/

	CString strName = m_strCurFontFamily;

	strName		 = ::GetValueString(GetAppUnknown(), "Editor", "CommentText_Name", strName);
	m_nCurHeight = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Height", _variant_t((long)m_nCurHeight));	
	m_bBold		 = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Bold", _variant_t((long)(m_bBold==TRUE?1:0))) == 1;
	m_bItalic    = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Italic", _variant_t((long)(m_bItalic==TRUE?1:0))) == 1;
	m_bUnder     = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Underline", _variant_t((long)(m_bUnder==TRUE?1:0))) == 1;
	m_nCurAlign  = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Align", _variant_t((long)m_nCurAlign));
	m_bStrikeOut = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_StrikeOut", _variant_t((long)m_bStrikeOut));

	
	LOGFONT	logFont;
	logFont.lfHeight = m_nCurHeight;
	logFont.lfWidth = 0;
	//memcpy((char*)logFont.lfFaceName, strName.GetBuffer(strName.GetLength()), strName.GetLength());
	//strName.ReleaseBuffer();
	strcpy((char*)logFont.lfFaceName, strName);
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = (m_bBold==TRUE)?FW_BOLD:FW_NORMAL;
	logFont.lfItalic = m_bItalic;
	logFont.lfUnderline = m_bUnder;
	logFont.lfStrikeOut = m_bStrikeOut;
	logFont.lfCharSet = DEFAULT_CHARSET;
	byte family = FF_DONTCARE;
	family = family << 4;
	family |= DEFAULT_PITCH;
	logFont.lfPitchAndFamily = family;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;

	if(ppLogFont)
		memcpy(*ppLogFont, &logFont, sizeof(LOGFONT));
	if(ppnAlign)
		**ppnAlign = m_nCurAlign;
}

void CCommentFontPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommentFontPropPage)
	DDX_Control(pDX, IDC_ST_FONT, m_strFontViewer);
	DDX_Control(pDX, IDC_CHECK_BACK, m_checkBack);
	DDX_Control(pDX, IDC_CHECK_ARROW, m_checkArrow);
	DDX_Control(pDX, IDC_COMBO2, m_cbFontSize);
	DDX_Control(pDX, IDC_CB_FONT, m_cbFont);
	DDX_Control(pDX, IDC_COLORPICKER_BACK, m_clrBack);
	DDX_Control(pDX, IDC_COLORPICKER_TEXT, m_clrText);
	DDX_Check(pDX, IDC_CHECK_ARROW, m_bArrow);
	DDX_Check(pDX, IDC_CHECK_BACK, m_bBack);
	DDX_CBIndex(pDX, IDC_COMBO2, m_nFontSize);
	DDX_Control(pDX, IDC_PB_BOLD, m_pbBold);
	DDX_Control(pDX, IDC_PB_ITALIC, m_pbItalic);
	DDX_Control(pDX, IDC_PB_UNDER, m_pbUnder);
	DDX_Control(pDX, IDC_PB_ALIGNLEFT, m_pbAlignLeft);
	DDX_Control(pDX, IDC_PB_ALIGNCENTER, m_pbAlignCenter);
	DDX_Control(pDX, IDC_PB_ALIGNRIGHT, m_pbAlignRight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommentFontPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CCommentFontPropPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_ARROW, OnCheckArrow)
	ON_BN_CLICKED(IDC_CHECK_BACK, OnCheckBack)
	ON_CBN_SELCHANGE(IDC_CB_FONT, OnSelchangeCbFont)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
	ON_MESSAGE(CPN_SELENDOK,    OnSelEndOK)
END_MESSAGE_MAP()


int CALLBACK EnumFontsProc(const LOGFONT *pLogFont, const TEXTMETRIC *pTextMetric, DWORD FontType, LPARAM lParam)
{
	CCommentFontPropPage* pThis = (CCommentFontPropPage*)lParam;
	/*if(pThis->m_strCurFontFamily == (char*)pLogFont->lfFaceName)
	{
		CString str;
		str.Format("%d %d", pTextMetric->tmHeight, pTextMetric->tmInternalLeading);
		pThis->m_cbFontSize.AddString(str);
	}
	if(!pThis->m_strCurFontFamily.IsEmpty())
		return TRUE;
	*/
	//COMBOBOXEXITEM	cbItem;
	//cbItem.mask = CBEIF_DI_SETITEM|CBEIF_TEXT|(FontType==TRUETYPE_FONTTYPE ? CBEIF_IMAGE|CBEIF_SELECTEDIMAGE  : 0);
	//cbItem.iItem = pThis->m_cbFont.GetCount();
	//cbItem.pszText = (char*)pLogFont->lfFaceName;
	//cbItem.iImage = 0;
	//cbItem.iSelectedImage = 0;
	//pThis->m_cbFont.InsertItem(&cbItem);
	CString str = pLogFont->lfFaceName;
	str.Insert(0, FontType==TRUETYPE_FONTTYPE ? "1" : "0");
	pThis->m_strArray.Add(str);
	return TRUE; 
}
 

/////////////////////////////////////////////////////////////////////////////
// CCommentFontPropPage message handlers

void CCommentFontPropPage::_InitParams()
{
	m_color = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", RGB(0,255,0));
	m_backColor = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Back", RGB(127,255,127));
	m_bBack = ::GetValueInt(GetAppUnknown(), "Editor", "TransparentCommentText", 0) == 0;
	m_bArrow = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_DrawArrow", 1) == 1;
	m_strCurFontFamily = ::GetValueString(GetAppUnknown(), "Editor", "CommentText_Name", "Arial");
	m_bBold = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Bold", 0) == 1;
	m_bItalic = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Italic", 0) == 1;
	m_bUnder = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Underline", 0) == 1;
	m_nCurHeight = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Height", 12);
	m_nCurAlign = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Align", 0); //0-left; 1-center; 2-right
	m_bStrikeOut = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_StrikeOut", 0);
}


BOOL CCommentFontPropPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	//BYTE R, G, B;

	//R = (byte)::GetValueInt(GetAppUnknown(), "Editor", "CommentText_R", 0);
	//G = (byte)::GetValueInt(GetAppUnknown(), "Editor", "CommentText_G", 0);
	//B = (byte)::GetValueInt(GetAppUnknown(), "Editor", "CommentText_B", 255);

	_InitParams();

	m_clrText.SetColour(m_color);
	m_strFontViewer.SetTextColor(m_color);
	
	
	m_clrBack.SetColour(m_backColor);
	m_strFontViewer.SetBackColor(m_backColor);
	
	
	m_strFontViewer.SetTransparent(!m_bBack);

	
	m_checkArrow.SetCheck(m_bArrow?1:0);
	m_checkBack.SetCheck(m_bBack?1:0);


	//m_strCurFontFamily = "";

	m_imgFont.Create(13, 13, ILC_COLOR, 0, 1);
	m_imgFont.SetBkColor(::GetSysColor(COLOR_WINDOW));
	//m_imgFont.Create(IDB_BITMAP_TRUETYPE, 15, ILC_COLOR, 1, RGB(255,255,255));
	m_imgFont.Add(AfxGetApp()->LoadIcon(IDI_ICON_TRUETYPE));
	m_cbFont.SetImageList(&m_imgFont);
	//DWORD style = m_cbFont.GetExtendedStyle();
	//style |= CBES_EX_NOEDITIMAGEINDENT;
	//style |= CBES_EX_NOEDITIMAGE;
	//m_cbFont.SetExtendedStyle(0, style);

	CClientDC dc(0);
	LOGFONT logfont;
	//logfont.lfFaceName = '\0';
	logfont.lfPitchAndFamily = 0;
	logfont.lfCharSet = DEFAULT_CHARSET;
	//::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontEnumProc, (LPARAM)this);
	::EnumFonts(dc.GetSafeHdc(), NULL, EnumFontsProc, (LPARAM)this);

	/*int nNumItems = m_cbFont.GetCount();
	COMBOBOXEXITEM cbItem1, cbItem2;
	//bubble gum
	UINT setmask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT|CBEIF_DI_SETITEM;
	UINT getmask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
	CString str1, str2;
	for(int j = 0; j < nNumItems-1; j++)
	for(int i = 0; i < nNumItems-1; i++)
	{
		cbItem2.mask = cbItem1.mask = getmask;
		cbItem1.iItem = i;
		cbItem2.iItem = i+1;
		cbItem1.pszText = str1.GetBuffer(255);
		cbItem2.pszText = str2.GetBuffer(255);
		cbItem1.cchTextMax = 255;
		cbItem2.cchTextMax = 255;
		m_cbFont.GetItem(&cbItem1);
		m_cbFont.GetItem(&cbItem2);
		if(strcmp(cbItem1.pszText, cbItem2.pszText) > 0)
		{
			cbItem2.mask = cbItem1.mask = setmask;
			cbItem1.iItem = i+1;
			cbItem2.iItem = i;
			m_cbFont.SetItem(&cbItem1);
			m_cbFont.SetItem(&cbItem2);
		}
		str1.ReleaseBuffer();
		str2.ReleaseBuffer();
	}*/

	CString str1, str2;
	int nNumItems = m_strArray.GetSize();
	COMBOBOXEXITEM cbItem;
	//bubble gum
	UINT setmask = CBEIF_TEXT|CBEIF_DI_SETITEM;
	for(int j = 0; j < nNumItems-1; j++)
	for(int i = 0; i < nNumItems-1; i++)
	{
		str1 = m_strArray[i];
		str1.Delete(0);
		str2 = m_strArray[i+1];
		str2.Delete(0);
		if(str1 > str2)
		{
			str1 = m_strArray[i+1];
			m_strArray[i+1] = m_strArray[i];
			m_strArray[i] = str1;
		}
	}

	//m_cbFont.SelectString(-1, strName);
	int nIdx = 0;
	for(j = 0; j < nNumItems; j++)
	{
		str1 = m_strArray[j];
		cbItem.mask = setmask|((str1[0] == '1') ? CBEIF_IMAGE|CBEIF_SELECTEDIMAGE : 0);
		str1.Delete(0);
		cbItem.pszText = (LPTSTR)(LPCTSTR)str1;
		cbItem.iImage = 0;
		cbItem.iSelectedImage = 0;
		cbItem.iItem = j;
		m_cbFont.InsertItem(&cbItem);
		if(m_strCurFontFamily == str1)
			nIdx = j;
	}

	m_cbFont.SetCurSel(nIdx);
	

	/*m_nHeight = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Height", 12);
	
	m_bUnderline = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Underline", 0) == 1;
	m_bItalic = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Italic", 0) == 1;
	
	m_nFamily = FF_DONTCARE;
	m_nFamily = m_nFamily << 4;
	m_nFamily |= DEFAULT_PITCH;
	*/


	if(m_bBold)
		m_pbBold.SetPressedState();
	else
		m_pbBold.ResetPressedState();
	
	if(m_bItalic)
		m_pbItalic.SetPressedState();
	else
		m_pbItalic.ResetPressedState();
	
	if(m_bUnder)
		m_pbUnder.SetPressedState();
	else
		m_pbUnder.ResetPressedState();


	CString str;
	/*int nHeight = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Size", 12);
	if(nHeight < 6)
		nHeight = 6;
	if(nHeight > 72)
		nHeight = 72;
	str.Format("%d", nHeight);
	if(m_cbFontSize.SelectString(-1, str) == CB_ERR )
		m_cbFontSize.SelectString(-1, "12");
	nHeight = MulDiv(nHeight, GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY), 72);
	m_nCurHeight = nHeight;
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Height", _variant_t((long)nHeight));	
	*/
	
	int nSize = MulDiv(m_nCurHeight, 72, GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY));
	if(nSize < 6)
		nSize = 6;
	if(nSize > 72)
		nSize = 72;
	str.Format("%d", nSize);
	if(m_cbFontSize.SelectString(-1, str) == CB_ERR )
		m_cbFontSize.SelectString(-1, "12");
	
	m_pbAlignLeft.ResetPressedState();
	m_pbAlignCenter.ResetPressedState();
	m_pbAlignRight.ResetPressedState();
	_ManageButtonState(m_nCurAlign);

	_SetFontPreview();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCommentFontPropPage::OnDestroy() 
{
	m_imgFont.DeleteImageList();	
	m_strArray.RemoveAll();

	CPropertyPageBase::OnDestroy();
	// TODO: Add your message handler code here
}

void CCommentFontPropPage::OnCheckArrow() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	::SetValue(GetAppUnknown(), "Editor", "CommentText_DrawArrow", _variant_t((long)(m_bArrow?1:0)));
	UpdateAction();
}

void CCommentFontPropPage::OnCheckBack() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	::SetValue(GetAppUnknown(), "Editor", "TransparentCommentText", _variant_t((long)(m_bBack?0:1)));
	m_strFontViewer.SetTransparent(!m_bBack);
	UpdateAction();
}


void CCommentFontPropPage::OnSelchangeCbFont() 
{
	CString strName;
	m_cbFont.GetLBText(m_cbFont.GetComboBoxCtrl( )->GetCurSel(), strName);
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Name", _variant_t(strName));
	m_strCurFontFamily = strName;
	_SetFontPreview();
	UpdateAction();
}

LRESULT CCommentFontPropPage::OnSelChange(WPARAM /*lParam*/, LPARAM /*wParam*/)
{
	COLORREF Color;
	
	Color = m_clrText.GetColour();
	m_strFontViewer.SetTextColor(Color);
	m_color = Color;

	Color = m_clrBack.GetColour();
	m_strFontViewer.SetBackColor(Color);
	m_backColor = Color;
	return TRUE;
}

LPARAM CCommentFontPropPage::OnSelEndOK(WPARAM lParam, LPARAM wParam)
{
	COLORREF Color;
	_variant_t var;

	Color = m_clrText.GetColour();
	::SetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", Color);	
	m_color = Color;

	Color = m_clrBack.GetColour();
	::SetValueColor(GetAppUnknown(), "Editor", "CommentText_Back", Color);    
	m_backColor = Color;

	UpdateAction();
    return TRUE;
}

void CCommentFontPropPage::OnSelchangeCombo2() 
{
	// TODO: Add your control notification handler code here
	CString strSize;
	m_cbFontSize.GetLBText(m_cbFontSize.GetCurSel(), strSize);
	long nSize;
	VERIFY(sscanf(strSize, "%d", &nSize)==1);
	CClientDC	dc(0);
	//::SetValue(GetAppUnknown(), "Editor", "CommentText_Size", _variant_t(nSize));	
	long nHeight = MulDiv(nSize, GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY), 72);
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Height", _variant_t(nHeight));	
	m_nCurHeight = (int)nHeight;
	_SetFontPreview();
	UpdateAction();
}

BEGIN_EVENTSINK_MAP(CCommentFontPropPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CCommentFontPropPage)
	ON_EVENT(CCommentFontPropPage, IDC_PB_BOLD, -600 /* Click */, OnClickPbBold, VTS_NONE)
	ON_EVENT(CCommentFontPropPage, IDC_PB_ITALIC, -600 /* Click */, OnClickPbItalic, VTS_NONE)
	ON_EVENT(CCommentFontPropPage, IDC_PB_UNDER, -600 /* Click */, OnClickPbUnder, VTS_NONE)
	ON_EVENT(CCommentFontPropPage, IDC_PB_ALIGNLEFT, -600 /* Click */, OnClickPbAlignleft, VTS_NONE)
	ON_EVENT(CCommentFontPropPage, IDC_PB_ALIGNCENTER, -600 /* Click */, OnClickPbAligncenter, VTS_NONE)
	ON_EVENT(CCommentFontPropPage, IDC_PB_ALIGNRIGHT, -600 /* Click */, OnClickPbAlignright, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CCommentFontPropPage::OnClickPbBold() 
{
	// TODO: Add your control notification handler code here
	m_bBold = m_pbBold.IsPressed();
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Bold", _variant_t((long)(m_bBold==TRUE?1:0)));
	_SetFontPreview();
	UpdateAction();
}	

void CCommentFontPropPage::OnClickPbItalic() 
{
	// TODO: Add your control notification handler code here
	m_bItalic = m_pbItalic.IsPressed();
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Italic", _variant_t((long)(m_bItalic==TRUE?1:0)));
	_SetFontPreview();
	UpdateAction();
}

void CCommentFontPropPage::OnClickPbUnder() 
{
	// TODO: Add your control notification handler code here
	m_bUnder = m_pbUnder.IsPressed();
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Underline", _variant_t((long)(m_bUnder==TRUE?1:0)));
	_SetFontPreview();
	UpdateAction();
}

void CCommentFontPropPage::OnClickPbAlignleft() 
{
	// TODO: Add your control notification handler code here
	_ManageButtonState(0);
}

void CCommentFontPropPage::OnClickPbAligncenter() 
{
	// TODO: Add your control notification handler code here
	_ManageButtonState(1);
}

void CCommentFontPropPage::OnClickPbAlignright() 
{
	// TODO: Add your control notification handler code here
	_ManageButtonState(2);
}

void CCommentFontPropPage::_ManageButtonState(int nButton)
{
	if(nButton != m_nCurAlign) 
	{
	switch(m_nCurAlign)
	{
	case 0:
		m_pbAlignLeft.ResetPressedState();
		break;
	case 1:
		m_pbAlignCenter.ResetPressedState();
		break;
	case 2:
		m_pbAlignRight.ResetPressedState();
		break;
	}
	m_nCurAlign = nButton;
	}

	switch(m_nCurAlign)
	{
	case 0:
		m_pbAlignLeft.SetPressedState();
		break;
	case 1:
		m_pbAlignCenter.SetPressedState();
		break;
	case 2:
		m_pbAlignRight.SetPressedState();
		break;
	}
	::SetValue(GetAppUnknown(), "Editor", "CommentText_Align", _variant_t((long)m_nCurAlign));
	UpdateAction();
}


void CCommentFontPropPage::_SetFontPreview()
{
	LOGFONT* plogFont = new LOGFONT;
	_GetFont(&plogFont);
	m_strFontViewer.SetFont(plogFont);
	delete plogFont;
}

