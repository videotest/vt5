#include "stdafx.h"
#include "editor.h"

#include "selectbinary.h"
#include "PropPage.h"


IMPLEMENT_DYNCREATE(CSelectBinaryPP, CDialog)

// {71E6B10A-571D-4c56-819B-312BAE6E51DD}
GUARD_IMPLEMENT_OLECREATE(CSelectBinaryPP, szSelectBinaryPropPageProgID, 
						  0x71e6b10a, 0x571d, 0x4c56, 0x81, 0x9b, 0x31, 0x2b, 0xae, 0x6e, 0x51, 0xdd);




CSelectBinaryPP::CSelectBinaryPP(CWnd* pParent):
CPropertyPageBase(CSelectBinaryPP::IDD)
{
	m_sName = c_szBinEditorPropPage;
	m_sUserName.LoadString( IDS_PROPPAGE_SELECT );
}
CSelectBinaryPP::~CSelectBinaryPP()
{

}

BEGIN_MESSAGE_MAP(CSelectBinaryPP, CDialog)
		ON_CBN_SELCHANGE(IDC_COMBO_SELECT, OnSelChanged)
END_MESSAGE_MAP()

int CSelectBinaryPP::find_pos(GuidKey key)
{
	CComboBox* cb = (CComboBox*)GetDlgItem( IDC_COMBO_SELECT);
	int n = cb->GetCount();
	for(int i=0;i<n;i++)
	{
		int k = cb->GetItemData(i);
		GuidKey gk = m_keys[k];
		if( compare(key,gk)<0)  
			return i;
	}
	return -1;
}

int CSelectBinaryPP::compare(GuidKey k1,  GuidKey k2)
{

	bstr_t bstrbi(szTypeBinaryImage);
	long pos;

	IUnknown* bin = FindObjectByKey(k1);
	if(bin==0) return 0;
	INumeredObjectPtr no = bin;
	long n1,n2;
	no->GetCreateNum(&n1);
	bin->Release();
///
	bin = FindObjectByKey(k2);
	if(bin==0) return 0;
	no = bin;
	no->GetCreateNum(&n2);
	bin->Release();
	return n1-n2;
}

BOOL CSelectBinaryPP::OnInitDialog()
{
	__super::OnInitDialog();

	DoLoadSettings();

	ShowWindow(SW_SHOW);
	return 1;
}

IDataContext2* CSelectBinaryPP::GetDataContext()
{
	IApplicationPtr app = GetAppUnknown(false);
	IUnknown *pDoc;
	app->GetActiveDocument( &pDoc );

	IDocumentSitePtr ds(pDoc);
	if(pDoc) pDoc->Release();
	if(ds==0) return 0;

	IUnknown *punkView;
	ds->GetActiveView( &punkView );
	IDataContext2Ptr ptrDC = punkView;
	if(punkView) punkView->Release();
	return ptrDC.Detach();
}
void CSelectBinaryPP::DoLoadSettings()
{
	
	if(!IsWindow(GetWindow()->m_hWnd)) 
		return;
	m_keys.RemoveAll();
	IDataContext2Ptr ptrDC(GetDataContext(),false);
	

	bstr_t bstrbi(szTypeBinaryImage);

	CComboBox* cb = (CComboBox*)GetDlgItem(IDC_COMBO_SELECT);
	cb->ResetContent( );
	IUnknown* punkActive;
	ptrDC->GetActiveObject(bstrbi, &punkActive);
	CString strActive;
	if(punkActive)
	{
		strActive = GetObjectName(punkActive);
		punkActive->Release();
	}

	long pos =0;
	ptrDC->GetFirstObjectPos(bstrbi, &pos);
	IUnknown *punkObject=0;
	CString strName;
	while(pos)
	{
		ptrDC->GetNextObject(bstrbi, &pos, &punkObject);
		if(punkObject)
		{
			INT_PTR i = m_keys.Add( GetObjectKey(punkObject));
			strName = GetObjectName(punkObject);

			int n = find_pos(m_keys[i]);
			int index = cb->InsertString(n, strName.GetBuffer());
		
			cb->SetItemData(index, i);
			if(strName==strActive)
				cb->SetCurSel(index);
			punkObject->Release();
		}
	}
	int i = cb->FindStringExact(0,strActive);
	cb->SetCurSel(i);
}

void CSelectBinaryPP::OnSelChanged()
{
	CComboBox* cb = (CComboBox*)GetDlgItem(IDC_COMBO_SELECT);
	int i = cb->GetCurSel();
	DWORD index = cb->GetItemData(i);
	GuidKey key = m_keys.GetAt(index);
	IUnknown* bin = FindObjectByKey(key);
	IDataContext2Ptr ptrDC(GetDataContext(),false);
	if(bin)
	{
		DWORD dwFlags = aofSkipIfAlreadyActive;
		bstr_t bstr(szTypeBinaryImage);
		ptrDC->SetActiveObject(bstr, bin, dwFlags);
		bin->Release();
	}
	
}	
