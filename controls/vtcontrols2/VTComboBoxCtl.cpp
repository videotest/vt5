// VTComboBoxCtl.cpp : Implementation of the CVTComboBoxCtrl ActiveX Control class.

#include "stdafx.h"
#include <comdef.h>
#include "vtcontrols2.h"
#include "VTComboBoxCtl.h"
#include "VTComboBoxPpg.h"
#include <msstkppg.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTComboBoxCtrl, COleControl)
IMPLEMENT_DYNCREATE(CVTComboBoxCtrl2, CVTComboBoxCtrl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTComboBoxCtrl, COleControl)
	//{{AFX_MSG_MAP(CVTComboBoxCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTComboBoxCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CVTComboBoxCtrl)
	DISP_PROPERTY_NOTIFY(CVTComboBoxCtrl, "Visible", m_visible, OnVisibleChanged, VT_BOOL)
	DISP_FUNCTION(CVTComboBoxCtrl, "GetText", GetText, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CVTComboBoxCtrl, "GetStringsCount", GetStringsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CVTComboBoxCtrl, "GetString", GetString, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CVTComboBoxCtrl, "DeleteString", DeleteString, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CVTComboBoxCtrl, "ReplaceString", ReplaceString, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CVTComboBoxCtrl, "DeleteContents", DeleteContents, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CVTComboBoxCtrl, "InsertString", InsertString, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CVTComboBoxCtrl, "GetSel", GetSel, VT_I4, VTS_NONE)
	DISP_FUNCTION(CVTComboBoxCtrl, "SetSel", SetSel, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CVTComboBoxCtrl, "SetType", SetType, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CVTComboBoxCtrl, "GetType", GetType, VT_I4, VTS_NONE)
	DISP_FUNCTION(CVTComboBoxCtrl, "SetSortStrings", SetSortStrings, VT_EMPTY, VTS_BOOL)
	DISP_STOCKPROP_ENABLED()
	DISP_STOCKPROP_FONT()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTComboBoxCtrl, COleControl)
	//{{AFX_EVENT_MAP(CVTComboBoxCtrl)
	EVENT_CUSTOM("OnChange", FireOnChange, VTS_NONE)
	EVENT_CUSTOM("OnSelectChange", FireOnSelectChange, VTS_NONE)
	EVENT_CUSTOM_ID("Click", DISPID_CLICK, FireClick, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTComboBoxCtrl, 2)
	PROPPAGEID(CVTComboBoxPropPage::guid)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTComboBoxCtrl)

BEGIN_PROPPAGEIDS(CVTComboBoxCtrl2, 2)
	PROPPAGEID(CVTComboBoxPropPage::guid)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTComboBoxCtrl2)

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTComboBoxCtrl, "VTCONTROLS2.VTComboBoxCtrl.1",
	0xb4b4c08f, 0x8d9a, 0x4655, 0xa0, 0xf1, 0xd5, 0xb0, 0x41, 0x9d, 0x1e, 0xd7)


IMPLEMENT_OLECREATE_EX(CVTComboBoxCtrl2, "VTCONTROLS2.VTComboBoxCtrl2.1",
	0x824c79e5, 0x6ddf, 0x427a, 0x90, 0xa9, 0x5f, 0xc4, 0xc7, 0x85, 0x9c, 0x91)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTComboBoxCtrl, _tlid, _wVerMajor, _wVerMinor)
IMPLEMENT_OLETYPELIB(CVTComboBoxCtrl2, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTComboBox =
		{ 0xf0a32695, 0x7edb, 0x45a1, { 0x97, 0xf6, 0x4e, 0xf7, 0xc2, 0xa8, 0xa9, 0x88 } };
const IID BASED_CODE IID_DVTComboBoxEvents =
		{ 0x5ca42129, 0x892d, 0x4041, { 0x8a, 0x82, 0xd, 0xda, 0xa3, 0x14, 0x49, 0xe6 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTComboBoxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTComboBoxCtrl,  IDS_VTCOMBOBOX, _dwVTComboBoxOleMisc)
IMPLEMENT_OLECTLTYPE(CVTComboBoxCtrl2, IDS_VTCOMBOBOX2, _dwVTComboBoxOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::CVTComboBoxCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTComboBoxCtrl

BOOL CVTComboBoxCtrl::CVTComboBoxCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTCOMBOBOX,
			IDB_VTCOMBOBOX,
			afxRegApartmentThreading,
			_dwVTComboBoxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::CVTComboBoxCtrl - Constructor

CVTComboBoxCtrl::CVTComboBoxCtrl()
{
	InitializeIIDs(&IID_DVTComboBox, &IID_DVTComboBoxEvents);
	m_nType = 0;
	m_sort=false;
	m_nSelStr=0;
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::~CVTComboBoxCtrl - Destructor

CVTComboBoxCtrl::~CVTComboBoxCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::OnDraw - Drawing function

void CVTComboBoxCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	DoSuperclassPaint(pdc, rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::DoPropExchange - Persistence support

void CVTComboBoxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::OnResetState - Reset control to default state

void CVTComboBoxCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
	m_sort=FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CVTComboBoxCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("COMBOBOX");
	cs.style =  WS_CHILD|WS_VSCROLL|WS_VISIBLE | ( !m_nType ? CBS_DROPDOWN : CBS_DROPDOWNLIST ) | CBS_NOINTEGRALHEIGHT;
	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::IsSubclassedControl - This is a subclassed control

BOOL CVTComboBoxCtrl::IsSubclassedControl()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::OnOcmCommand - Handle command messages

LRESULT CVTComboBoxCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.
    switch(wNotifyCode)
	{
	    case CBN_SELCHANGE:
			{
			if(!m_IsBlankStringInserted) 
			{
				CWnd *pwndParent = GetParent();
				
				if( pwndParent )
				{
					CRect rc;
					
					GetWindowRect( &rc );
					pwndParent->ScreenToClient( &rc );
					pwndParent->RedrawWindow( &rc );
				}
			}
			CString sTextLB;
			CComboBox *combo = (CComboBox*)this;
			combo->GetLBText(combo->GetCurSel(),sTextLB);
			combo->SetWindowText(sTextLB);
			m_nSelStr=combo->GetCurSel();
			FireOnSelectChange();    // if have smthg to select
			}
			break;
		case CBN_EDITCHANGE: 
			{
				CComboBox *combo = (CComboBox*)this;
				DWORD dwSel;
				if(!combo->GetDroppedState())
				{
					CString sText;
					combo->GetWindowText(sText);
					dwSel=combo->GetEditSel();
					ReplaceString(m_nSelStr,sText);
				}
				Refresh();
				combo->SetEditSel(HIWORD(dwSel),HIWORD(dwSel));
				FireOnChange(); 
			}
			break;
		case CBN_DROPDOWN: 
			{
				CRect	rect, rcBounds;
				GetWindowRect( &rect );
				GetClientRect( &rcBounds );
				

				{
					int dy = 0;

					CComboBox *combo = (CComboBox *)this;

					int nSz = combo->GetCount();
					int nHeight = nSz > 0 ? combo->GetItemHeight(0) : ::GetSystemMetrics( SM_CYMENUSIZE );

					if( nSz > 10 )
						nSz = 10;

					dy = nHeight * nSz;
					dy += 2 * ::GetSystemMetrics( SM_CXEDGE );

					rect = rcBounds;
					rect.bottom = rcBounds.bottom + dy;
				}

				::SetWindowPos( GetSafeHwnd(), 0, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE|SWP_NOZORDER );
			}
			break;
		case CBN_CLOSEUP: 
			Refresh();
			break;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl message handlers

BSTR CVTComboBoxCtrl::GetText() 
{
	if(GetSafeHwnd())
	{
		CString strResult;
		CComboBox *combo = (CComboBox*)this;
		/*int cursel = combo->GetCurSel();
		if(cursel!=-1)
		{
			//combo->GetWindowText(strResult);
			//if(strResult.GetLength()>0 && strResult!="" && strResult!=" ")
				combo->GetLBText(cursel,strResult);
			//else
			//	strResult="";
		}
		else
			combo->GetWindowText(strResult);*/
		combo->GetWindowText(strResult);
		return strResult.AllocSysString();
	}
	return NULL;
}

long CVTComboBoxCtrl::GetStringsCount() 
{
	CComboBox *combo = (CComboBox*)this;
	if(m_IsBlankStringInserted) return 0;           // we have no items except blank
	if(GetSafeHwnd()) return combo->GetCount();
	return m_tempStrings.GetSize();
}

BSTR CVTComboBoxCtrl::GetString(long idx) 
{	
	if( idx < 0 )
	{
		ASSERT( false );
		CString str_result;
		str_result = "";
		return str_result.AllocSysString();
	}
		
	if(GetSafeHwnd())
	{
		CString strResult;
		if(m_IsBlankStringInserted)
			strResult.AllocSysString();    // we have no items except blank
		CComboBox *combo = (CComboBox*)this;
		combo->GetLBText(idx,strResult);
		return strResult.AllocSysString();
	}
	return m_tempStrings[idx].AllocSysString();
}

void CVTComboBoxCtrl::DeleteString(long idx) 
{
	CComboBox *combo = (CComboBox*)this;

	if(GetSafeHwnd())
	{
		if(!m_IsBlankStringInserted)
		{
			combo->DeleteString(idx);       // else we have nothing to delete
			if(m_sort)
				m_NameStrings.RemoveAt(idx);
			if(combo->GetCount()==0)
				InsertBlankString();     // we've just deleted the last string
		}
		combo->SetWindowText("");
	}
	else
		m_tempStrings.RemoveAt(idx);
}

void CVTComboBoxCtrl::DeleteContents() 
{
   CComboBox *combo = (CComboBox*)this;
   if(GetSafeHwnd())
   {
	   combo->ResetContent();
	   if(m_sort)
		   m_NameStrings.RemoveAll();
	   InsertBlankString(); 
   }
   else 
	   m_tempStrings.RemoveAll();
}

void CVTComboBoxCtrl::ReplaceString(long idx, LPCTSTR NewVal)
{
	if(GetSafeHwnd())
	{
		if(!m_IsBlankStringInserted)
		{
			CComboBox *combo = (CComboBox*)this;
			combo->DeleteString(idx);
			if(m_sort)
			{
				m_NameStrings.RemoveAt(idx);
				combo->InsertString(m_NameStrings.GetCount(),NewVal);
			}
			else
				combo->InsertString(idx,NewVal);
			SetSel(idx);
			CString sText=NewVal;
			combo->SetWindowText(sText);
			combo->SetEditSel(-1,-1);
			//int iSelEnd=sText.GetLength();
			//combo->SetEditSel(iSelEnd,iSelEnd);
		}
	}
	else
		m_tempStrings[idx]=NewVal;
}

void CVTComboBoxCtrl::InsertString(long idx, LPCTSTR NewVal) 
{
	CComboBox *combo = (CComboBox*)this;
	if(GetSafeHwnd())
	{
		if(m_IsBlankStringInserted)
		{ 
			RemoveBlankString();
		}
		if(idx<0)
		{
			idx=combo->GetCount();
		}
		combo->InsertString(idx,NewVal);

		if(m_sort)
		{
			if(idx==0)
				m_NameStrings.InsertAt(idx,NewVal);
			else
			{
				CStringArray temp;
				int cur=0;
	
				for(int i=0;i<m_NameStrings.GetCount();i++)
					temp.InsertAt(i,m_NameStrings.GetAt(i));
			
				for(int i=0;i<m_NameStrings.GetCount();i++)
				{
					cur=strcmp(NewVal,m_NameStrings.GetAt(i));
					if(cur<0)
					{
						int j;
						temp.SetAt(i,NewVal);
						for(j=i;j<m_NameStrings.GetCount()-1;j++)
							temp.SetAt(j+1,m_NameStrings.GetAt(j));
						temp.InsertAt(j+1,m_NameStrings.GetAt(j));
						break;
					}
					else
						if(i==m_NameStrings.GetCount()-1)
							temp.InsertAt(i+1,NewVal);
				}
				m_NameStrings.RemoveAll();
				combo->ResetContent();
				for(int i=0;i<temp.GetCount();i++)
				{
					m_NameStrings.InsertAt(i,temp.GetAt(i));
					combo->InsertString(i,temp.GetAt(i));
				}
				/*CString s;
				s.Format("string %d, array %d, cur = %d", idx, temp.GetCount()-1, cur);
				::AfxMessageBox(s);*/
				/*LPCTSTR s="";
				for(int i=0;i<m_NameStrings.GetCount();i++)
					s=s+m_NameStrings.GetAt(i)+"\n";
				::AfxMessageBox(s);*/
			}
		}
		else
			m_NameStrings.InsertAt(idx,NewVal);
	} 
	else
		m_tempStrings.InsertAt(idx,NewVal);
}

void CVTComboBoxCtrl::SetSortStrings(BOOL sStr) 
{
	m_sort=sStr;
	if(GetSafeHwnd() && m_sort)
	{
		CComboBox *combo = (CComboBox*)this;
		if(combo->GetCount()>0)
		{
			int i, sLength;
			CString s;
			CStringArray m_tStrings;
			for(i=0;i<combo->GetCount();i++)
			{
				sLength=combo->GetLBTextLen(i);
				combo->GetLBText(i,s.GetBuffer(sLength));
				s.ReleaseBuffer();
				m_tStrings.InsertAt(i,s.GetBuffer(0));
			}
			combo->ResetContent();
			m_NameStrings.RemoveAll();
			for(i=0;i<m_tStrings.GetCount();i++)
				InsertString(i,m_tStrings.GetAt(i));
		}
	}
	Refresh();
}

long CVTComboBoxCtrl::GetSel() 
{
	CComboBox *combo = (CComboBox*)this;
	if(GetSafeHwnd())
	{
		if(m_IsBlankStringInserted)
			return -1;     // we have no any selection except blank
		return combo->GetCurSel();
	}
	return -1;
}

void CVTComboBoxCtrl::SetSel(long idx) 
{
	CComboBox *combo = (CComboBox*)this;
	if(GetSafeHwnd())
		if(!m_IsBlankStringInserted)
			combo->SetCurSel(idx);   // 
}

long CVTComboBoxCtrl::GetType() 
{
	return m_nType;
}

void CVTComboBoxCtrl::SetType(long idx) 
{
   CComboBox *combo = (CComboBox*)this;
   if(GetSafeHwnd())
   {
	   if( m_nType == idx )
		   return;

		m_nType = idx;
//		int nSel = combo->GetCurSel();
//		RecreateControlWindow();
//		combo->SetCurSel( nSel );
   }
}

void CVTComboBoxCtrl::InsertBlankString()
{
  CComboBox *combo = (CComboBox*)this;
  combo->AddString(_T(""));
  m_IsBlankStringInserted=TRUE;
}

void CVTComboBoxCtrl::RemoveBlankString()
{
	CComboBox *combo = (CComboBox*)this;
    combo->DeleteString(0);
	m_IsBlankStringInserted=FALSE;
}


int CVTComboBoxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(!m_tempStrings.GetSize())
		InsertBlankString();
	else
		for(int i=0; i<m_tempStrings.GetSize(); i++)
			InsertString(i,m_tempStrings[i]);
//	ShowWindow(SW_SHOW);
	return 0;
}

void CVTComboBoxCtrl::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
		try
		{
			 int nVer = 2;
			 int i;
			 long StringsCount=GetStringsCount();   // Storing amount of strings
			 ar << long( int( StringsCount ) + ( nVer << 16 ) );
			 for(i=0; i<StringsCount; i++)      //   Storing strings
			 {
				 _bstr_t bstr(GetString(i),FALSE);
				 ar << strlen(bstr);
				 ar.WriteString(bstr);
			 }
			 if (MAKELONG(_wVerMajor,_wVerMinor) >= MAKELONG(1, 1))
			 {
		//		 ar << m_visible;
			 } 

			 ar << m_nType;

			 ar << m_sort;
		}
		catch(CFileException* pe)
		{
			pe->Delete();
		}
	}
	else
	{
		try
		{
			 int nVer = 0;
			 int i;
			 long StringsCount;
			 ar >> StringsCount;

			 nVer = int( StringsCount >> 16 );
			 StringsCount = (int)( StringsCount - ( nVer << 16 ) );

			 m_tempStrings.RemoveAll();
			 m_tempStrings.FreeExtra();
			 
			 for(i=0; i<StringsCount; i++)      //   Loading strings
			 {
				 char str[50]="";
				 int len;
				 ar >> len;
				 ar.ReadString(str,len);
				 InsertString(i,str);
			 }
			 if (MAKELONG(_wVerMajor,_wVerMinor) >= MAKELONG(1, 1))
			 {
//				 ar >> m_visible;
			 } 

			 if( nVer >= 1 )
			 {
	 			 int type = 0;
	 			 ar >> type;
//				 SetType( type );
			 }

			 if(nVer>=2)
			 {
				 ar >> m_sort;
				 SetSortStrings(m_sort);
			 }
		}
		catch(CFileException* pe)
		{
			pe->Delete();
		}
	}
	COleControl::Serialize(ar);
}

BOOL CVTComboBoxCtrl::DestroyWindow() 
{
	m_tempStrings.RemoveAll();
	int i;
	for(i=0; i<GetStringsCount(); i++)
		m_tempStrings.Add(GetString(i));
	return COleControl::DestroyWindow();
}

void CVTComboBoxCtrl::OnFinalRelease() 
{
	COleControl::OnFinalRelease();
}

void CVTComboBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	FireClick();
	::SendMessage(GetSafeHwnd(),CB_SHOWDROPDOWN,(WPARAM)true,NULL);
	
	//COleControl::OnLButtonDown(nFlags, point);
}

void CVTComboBoxCtrl::OnVisibleChanged() 
{
	if(GetSafeHwnd())
	{
		if(m_visible)
			ShowWindow(SW_SHOW);
		else
			ShowWindow(SW_HIDE);
	}
		
	SetModifiedFlag();
}


void CVTComboBoxCtrl::OnFontChanged() 
{	

	CFontHolder& font = InternalGetFont();

	if (font.m_pFont != NULL)
	{
		HFONT hFont = font.GetFontHandle();
		if( hFont )
		{
			CFont* pFont = CFont::FromHandle( hFont );
			if( pFont )
			{
				LOGFONT lf;
				::ZeroMemory( &lf, sizeof(LOGFONT) );
				pFont->GetLogFont( &lf );
				lf.lfCharSet = DEFAULT_CHARSET;

				m_Font.DeleteObject();
				m_Font.CreateFontIndirect( &lf );
				if( GetSafeHwnd() )
					CWnd::SetFont( &m_Font, TRUE );
			}
		}
	}


	/*
	COleControl::OnFontChanged();
	*/
}


CVTComboBoxCtrl2::CVTComboBoxCtrl2()
{
	m_nType = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxCtrl::~CVTComboBoxCtrl - Destructor

CVTComboBoxCtrl2::~CVTComboBoxCtrl2()
{
	// TODO: Cleanup your control's instance data here.
}

BOOL CVTComboBoxCtrl2::CVTComboBoxCtrl2Factory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTCOMBOBOX2,
			IDB_VTCOMBOBOX2,
			afxRegApartmentThreading,
			_dwVTComboBoxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}
