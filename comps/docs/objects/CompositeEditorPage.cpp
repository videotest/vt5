#include "stdafx.h"
#include "objects.h"
#include "CompositeEditorPage.h"



IMPLEMENT_DYNCREATE(CCompositeEditorPage, CDialog)
IMPLEMENT_DYNCREATE(CPhaseEditorPage, CDialog)

// {046CA70C-D79E-4280-BDE7-6FF530EB2599}
GUARD_IMPLEMENT_OLECREATE(CCompositeEditorPage, "Objects.CompositeEditorPage", 
0x46ca70c, 0xd79e, 0x4280, 0xbd, 0xe7, 0x6f, 0xf5, 0x30, 0xeb, 0x25, 0x99);

// {2250767F-98C3-4f62-9544-D06AD4E54FD7}
GUARD_IMPLEMENT_OLECREATE(CPhaseEditorPage, "Objects.PhaseEditorPage", 
0x2250767f, 0x98c3, 0x4f62, 0x95, 0x44, 0xd0, 0x6a, 0xd4, 0xe5, 0x4f, 0xd7);



BEGIN_MESSAGE_MAP(CCompositeEditorPage,CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_CLASS, OnSelChanged)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

CCompositeEditorPage::CCompositeEditorPage():
CPropertyPageBase(IDD_COMPOSITE_EDITOR_PP)
{
	_OleLockApp( this );
	m_sName = c_szCompositeEditorPropPage;
	m_sUserName.LoadString( IDS_COMPOSITE_EDITOR_TITLE );
	strData =0;
	classCount =0;
}

BEGIN_MESSAGE_MAP(CPhaseEditorPage,CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PHASE, OnSelChanged)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()




CCompositeEditorPage::~CCompositeEditorPage()
{
	_OleUnlockApp( this );
	if(strData)
	{
		for(int i=0; i<classCount;i++)
			delete[] strData[i].name;
		delete[] strData;
	}
}


BOOL CCompositeEditorPage::OnInitDialog() 
{

	BOOL ret = CPropertyPageBase::OnInitDialog();
	CComboBox* cmb = (CComboBox*)GetDlgItem(IDC_COMBO_CLASS);

	read_classfile();
	for(int i=0;i<classCount;i++)
	{
		TCHAR buff[3];
		buff[2]=0;
		_itot(i, buff, 10);
		int index = cmb->AddString(buff);	
		cmb->SetItemData(index,i);
	}
	int level = ::GetValueInt( GetAppUnknown(), "Composite", "EditorClassNum", 0 );
	DWORD data;
	for(i=0;i<classCount;i++)
	{
		data = cmb->GetItemData(i);
		if(data==level) 
		{
			cmb->SetCurSel(i);
			break;
		}
	}
	cmb->UpdateWindow();


	ShowWindow(SW_SHOW);
	return ret;
}
void CCompositeEditorPage::read_classfile()
{
	CString classfile = GetValueString(GetAppUnknown(), _T("Classes"),_T("CompositeObjectsClassFile"),0);
	if(classfile=="") return;
	int r,g,b;
	TCHAR buffer[128];
	TCHAR sz[5]; 
	::GetPrivateProfileString( "Classes", "ClassCount", "0", buffer, sizeof( TCHAR )*128, classfile );
	classCount = _ttoi(buffer);
	if(!classCount) return;
	strData = new CLASSINFO[classCount];
	for(int i=0;i<classCount;i++)
	{
	_itot(i,sz,10);
	::GetPrivateProfileString( "Classes", sz, "", buffer, sizeof( TCHAR )*128, classfile );
	sscanf( buffer, "(%d,%d,%d),%[^;]s", &r, &g, &b, buffer );
	strData[i].name = new TCHAR[ _tcslen(buffer)+1];
    _tcscpy(strData[i].name,buffer);
	strData[i].color = RGB(r,g,b);
	}
}

void CCompositeEditorPage::OnSelChanged()
{
	CComboBox* cmb = (CComboBox*)GetDlgItem(IDC_COMBO_CLASS);
	long n = cmb->GetCurSel();

	SetValue( GetAppUnknown(), "Composite", "EditorClassNum", (long)cmb->GetItemData(n) );
	UpdateAction();
}


void CCompositeEditorPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int n = nIDCtl;
	if(classCount==0) return;
	if(ODT_COMBOBOX == lpDrawItemStruct->CtlType)
	{
		if(nIDCtl == IDC_COMBO_CLASS )
		{	
			CComboBox* cmb = (CComboBox*)GetDlgItem(IDC_COMBO_CLASS);
			HDC dc = lpDrawItemStruct->hDC;
			
			int oldDC = SaveDC(dc);
			COLORREF bkColor = RGB(255,255,255);;
			if(lpDrawItemStruct->itemState & ODS_SELECTED)
			{
				bkColor= GetSysColor(COLOR_HIGHLIGHT);
				FillRect(dc, &lpDrawItemStruct->rcItem, CBrush(bkColor));
				SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
			else FillRect(dc, &lpDrawItemStruct->rcItem, CBrush(bkColor));
 
			SetBkMode(dc,TRANSPARENT);
			RECT rect;	
			/*rect.left = lpDrawItemStruct->rcItem.right-70;
			rect.top = lpDrawItemStruct->rcItem.top;
			rect.bottom = lpDrawItemStruct->rcItem.bottom;
			rect.right = lpDrawItemStruct->rcItem.right-20;
			FillRect(dc, &rect, CBrush(strData[cmb->GetItemData(lpDrawItemStruct->itemID)].color));*/
			rect = lpDrawItemStruct->rcItem;
			OffsetRect(&rect, 5,0);
			DrawText(dc, strData[cmb->GetItemData(lpDrawItemStruct->itemID)].name,-1, &rect, DT_SINGLELINE | DT_VCENTER  );
			RestoreDC(dc,oldDC);
		}
	}
	
}


CPhaseEditorPage::CPhaseEditorPage():
CPropertyPageBase(IDD_PHASE_EDITOR_PP)
{
	_OleLockApp( this );
	m_sName = c_szPhaseEditorPropPage;
	m_sUserName.LoadString( IDS_PHASE_EDITOR_TITLE );
	strData.reserve(20);
	classCount =0;
}

BOOL CPhaseEditorPage::OnInitDialog()
{
	BOOL ret = CPropertyPageBase::OnInitDialog();
	CComboBox* cmb = (CComboBox*)GetDlgItem(IDC_COMBO_PHASE);

	read_classfile();

	CString str =  GetValueString(GetAppUnknown(), "Phases", "ActivePhases", "");
	int nClass =  GetValueInt(GetAppUnknown(), "ObjectEditor", "EditorClassNum", -2 );
	int nTemp =0;
	CString strClass;
	int n =0;
	while(1)
	{
		strClass = str.Tokenize(",",n);
		{
			if(strClass.IsEmpty() ) break;
			int k =_ttoi(strClass);

			int index = cmb->AddString(strData[k].name);	
			cmb->SetItemData(index,k);
			if(nClass == k)
			{
				nTemp = index;
			}
		}
	}
	cmb->SetCurSel(nTemp);
	n =cmb->GetItemData(nTemp);
	::SetValue( GetAppUnknown(), "ObjectEditor", "EditorClassNum", (long)n );
	::SetValue( GetAppUnknown(), "ObjectEditor", "EditorColor",(long)strData[n].color );
	UpdateAction();
	cmb->UpdateWindow();

	ShowWindow(SW_SHOW);	
	return ret; 
}

void CPhaseEditorPage::OnSelChanged()
{
	CComboBox* cmb = (CComboBox*)GetDlgItem(IDC_COMBO_PHASE);
	long n = cmb->GetCurSel();

	SetValue( GetAppUnknown(), "ObjectEditor", "EditorClassNum", (long)cmb->GetItemData(n) );
	SetValue( GetAppUnknown(), "ObjectEditor", "EditorColor",(long)strData[cmb->GetItemData(n)].color );
	UpdateAction();
}

CPhaseEditorPage::~CPhaseEditorPage()
{
clear_strData();
_OleUnlockApp( this );
}

void CPhaseEditorPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int n = nIDCtl;
	if(classCount==0) return;
	if(ODT_COMBOBOX == lpDrawItemStruct->CtlType)
	{
		if(nIDCtl == IDC_COMBO_PHASE )
		{	
			CComboBox* cmb = (CComboBox*)GetDlgItem(IDC_COMBO_PHASE);
			if(cmb->GetCount()==0) return;
			HDC dc = lpDrawItemStruct->hDC;
			
			int oldDC = SaveDC(dc);
			COLORREF bkColor = RGB(255,255,255);;
			if(lpDrawItemStruct->itemState & ODS_SELECTED)
			{
				bkColor= GetSysColor(COLOR_HIGHLIGHT);
				FillRect(dc, &lpDrawItemStruct->rcItem, CBrush(bkColor));
				SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
			else FillRect(dc, &lpDrawItemStruct->rcItem, CBrush(bkColor));
 
			SetBkMode(dc,TRANSPARENT);
			RECT rect;	
			rect.left = lpDrawItemStruct->rcItem.right-70;
			rect.top = lpDrawItemStruct->rcItem.top;
			rect.bottom = lpDrawItemStruct->rcItem.bottom;
			rect.right = lpDrawItemStruct->rcItem.right-20;
			int l = cmb->GetItemData(lpDrawItemStruct->itemID);
			if(l>=0)
			{
				COLORREF clr = strData[l].color;
				FillRect(dc, &lpDrawItemStruct->rcItem, CBrush(strData[l].color));
				rect = lpDrawItemStruct->rcItem;
				OffsetRect(&rect, 5,0);
				clr = strData[l].color;
				BYTE r = GetBValue(clr), g = GetGValue(clr), b = GetRValue(clr); 
				if(r<=128) r=255;
				else r=0;
				if(g<=128) g=255;
				else g=0;
				if(b<=128) b=255;
				else b=0;
				clr =RGB(r,g,b);

				SetTextColor(dc, clr);
				DrawText(dc, strData[l].name,-1, &rect, DT_SINGLELINE | DT_VCENTER  );
			}
			RestoreDC(dc,oldDC);
		}
	}
}

void CPhaseEditorPage::clear_strData()
{
	int n = strData.size();
	for(int i=0;i<n;i++)
	{
		delete[] strData[i].name;
	}
	strData.clear();
}

void CPhaseEditorPage::read_classfile()
{
	CString classfile = GetValueString(GetAppUnknown(), _T("Classes"),_T("ClassFile"),"");
	if(classfile.IsEmpty()) return;

	int r,g,b;
	TCHAR buffer[128];
	TCHAR sz[5]; 
	::GetPrivateProfileString( "Classes", "ClassCount", "0", buffer, sizeof( TCHAR )*128, classfile );
	classCount = _ttoi(buffer);
	if(!classCount) return;
	clear_strData();
	int i=0;
	_itot(i,sz,10);
	::GetPrivateProfileString( "Classes", sz, "", buffer, sizeof( TCHAR )*128, classfile );
	while(*buffer)
	{
		CLASSINFO clsInfo;
		ZeroMemory(&clsInfo, sizeof(clsInfo));
		sscanf( buffer, "(%d,%d,%d),%[^;]s", &r, &g, &b, buffer );
		clsInfo.name = new TCHAR[ _tcslen(buffer)+1];
		_tcscpy(clsInfo.name,buffer);
		clsInfo.color = RGB(r,g,b);
		strData.push_back(clsInfo);

		i++;
		_itot(i,sz,10);
		::GetPrivateProfileString( "Classes", sz, "", buffer, sizeof( TCHAR )*128, classfile );
	}
}

