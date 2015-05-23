// TextPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "TextPropPage.h"

#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTextPropPage, CDialog)

// {0861E962-A96B-11d3-A548-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CTextPropPage, szEditorTextPropPageProgID, 
0x861e962, 0xa96b, 0x11d3, 0xa5, 0x48, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);


/////////////////////////////////////////////////////////////////////////////
// CTextPropPage dialog


CTextPropPage::CTextPropPage(CWnd* pParent /*=NULL*/):CPropertyPageBase(CTextPropPage::IDD)
{
	//{{AFX_DATA_INIT(CTextPropPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sName = c_szCTextPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_TEXT );
}


void CTextPropPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextPropPage)
	DDX_Control(pDX, IDC_EDIT_TEXT, m_editText);
	DDX_Control(pDX, IDC_LB_TEXT, m_lbText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextPropPage, CPropertyPageBase)
	//{{AFX_MSG_MAP(CTextPropPage)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LB_TEXT, OnSelchangeLbText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CTextPropPage, CPropertyPageBase)
	INTERFACE_PART(CTextPropPage, IID_ITextPage, Text)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////////////////
//ITextPage interface implementation
IMPLEMENT_UNKNOWN(CTextPropPage, Text);
HRESULT CTextPropPage::XText::SetText(BSTR bstrText)
{
	_try_nested(CTextPropPage, Text, SetText)
	{
		CString strVal(bstrText);
		pThis->m_editText.SetWindowText(strVal);
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
// CTextPropPage message handlers

BOOL CTextPropPage::OnInitDialog() 
{
	CPropertyPageBase::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strVal, strEntry;
	int idx = 0;
	while( true )
	{
		//strEntry = "CommentText_PredefinedStr_";
		strEntry.Format("CommentText_PredefinedStr_%d", idx);
		strVal	= ::GetValueString(GetAppUnknown(), "Editor", strEntry, "");
		if(!strVal.IsEmpty())
			m_lbText.AddString(strVal);
		else
			break;
		idx++;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextPropPage::OnDestroy() 
{
	CString strVal, strEntry;
	int nEntries = m_lbText.GetCount();
	for(int i = 0; i < nEntries; i++)
	{
		//strEntry = "CommentText_PredefinedStr_";
		strEntry.Format("CommentText_PredefinedStr_%d", i);
		m_lbText.GetText(i, strVal);
		::SetValue(GetAppUnknown(), "Editor", strEntry, _variant_t(strVal));
	}
	CPropertyPageBase::OnDestroy();
}

BEGIN_EVENTSINK_MAP(CTextPropPage, CDialog)
    //{{AFX_EVENTSINK_MAP(CTextPropPage)
	ON_EVENT(CTextPropPage, IDC_PB_ADD, -600 /* Click */, OnClickPbAdd, VTS_NONE)
	ON_EVENT(CTextPropPage, IDC_PB_DELETE, -600 /* Click */, OnClickPbDelete, VTS_NONE)
	ON_EVENT(CTextPropPage, IDC_PB_WRITE, -600 /* Click */, OnClickPbWrite, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CTextPropPage::OnClickPbAdd() 
{
	// TODO: Add your control notification handler code here
	CString strVal;
	m_editText.GetWindowText(strVal);
	m_lbText.AddString(strVal);
}

void CTextPropPage::OnClickPbDelete() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_lbText.GetCurSel();
	if(nCurSel != LB_ERR)
	{
		m_lbText.DeleteString(nCurSel);
	}
}

void CTextPropPage::OnClickPbWrite() 
{
	// TODO: Add your control notification handler code here
	CString	strCapt;
	strCapt.LoadString(IDS_MSG_CAPTION);
	CString	strContent;
	CString strVal;
	m_editText.GetWindowText(strVal);
	if(strVal.IsEmpty())
	{
		strContent.LoadString(IDS_MSG_CONTENT2);
		MessageBox(strContent, strCapt, MB_ICONERROR|MB_OK|MB_APPLMODAL|MB_DEFBUTTON1);
		return;
	}
	
	if(::GetValueInt(GetAppUnknown(), "Editor", "CommentText_HaveActiveObj", 0)==0)
	{
		strContent.LoadString(IDS_MSG_CONTENT);
		//if(MessageBox(strContent, strCapt, MB_ICONEXCLAMATION|MB_YESNO|MB_APPLMODAL|MB_DEFBUTTON1) == IDNO)
		//	return;
		MessageBox(strContent, strCapt, MB_ICONERROR|MB_OK|MB_APPLMODAL|MB_DEFBUTTON1);
			return;
	}
	::SetValue(GetAppUnknown(), "Editor", "CommentText_WriteText", _variant_t(strVal));
	UpdateAction();
	::SetValue(GetAppUnknown(), "Editor", "CommentText_WriteText", _variant_t(""));
}

void CTextPropPage::OnSelchangeLbText() 
{
	// TODO: Add your control notification handler code here
	CString strVal;
	int nCurSel = m_lbText.GetCurSel();
	if(nCurSel != LB_ERR)
	{
		m_lbText.GetText(nCurSel, strVal);
		m_editText.SetWindowText(strVal);
	}
}



bool CTextPropPage::OnSetActive( )
{
	CString strVal = "";
	//strVal = ::GetValueString(GetAppUnknown(), "Editor", "CommentText_CurEditText", "");
	//m_editText.SetWindowText(strVal);
	return true;
}

