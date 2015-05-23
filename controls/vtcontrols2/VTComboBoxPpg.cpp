// VTComboBoxPpg.cpp : Implementation of the CVTComboBoxPropPage property page class.

#include "stdafx.h"
#include <comdef.h>
#include "vtcontrols2.h"
#include "VTComboBoxPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTComboBoxPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTComboBoxPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTComboBoxPropPage)
	ON_BN_CLICKED(IDC_BUTTONDELETE, OnButtondelete)
	ON_BN_CLICKED(IDC_BUTTONDELETEALL, OnButtondeleteall)
	ON_BN_CLICKED(IDC_BUTTONINSERT, OnButtoninsert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTComboBoxPropPage, "VTCONTROLS2.VTComboBoxPropPage.1",
	0x5ecb6558, 0x2d3f, 0x4c42, 0xb2, 0x4, 0x54, 0x98, 0x55, 0x5f, 0x95, 0x64)


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxPropPage::CVTComboBoxPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTComboBoxPropPage

BOOL CVTComboBoxPropPage::CVTComboBoxPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTCOMBOBOX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxPropPage::CVTComboBoxPropPage - Constructor

CVTComboBoxPropPage::CVTComboBoxPropPage() :
	COlePropertyPage(IDD, IDS_VTCOMBOBOX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTComboBoxPropPage)
	m_string = _T("");
	m_sizeHeight = 0;
	m_sizeWidth = 0;
	//}}AFX_DATA_INIT
	m_IsCreated=false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxPropPage::DoDataExchange - Moves data between page and properties

void CVTComboBoxPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTComboBoxPropPage)
	DDX_Control(pDX, IDC_LISTDROPDOWN, m_dropdown);
	DDX_Text(pDX, IDC_EDITSTRING, m_string);
	DDX_Text(pDX, IDC_EDITSIZEHEIGHT, m_sizeHeight);
	DDV_MinMaxInt(pDX, m_sizeHeight, 1, 1000);
	DDX_Text(pDX, IDC_EDITSIZEWIDTH, m_sizeWidth);
	DDV_MinMaxInt(pDX, m_sizeWidth, 1, 1000);
	//}}AFX_DATA_MAP
	if(m_IsCreated)
	{
		ULONG nObjects;
		CClientDC dc(this);
		IDispatch** lpDisp=GetObjectArray(&nObjects);
		IOleObject* pIOleObj;
		lpDisp[0]->QueryInterface(__uuidof(IOleObject),(void**)&pIOleObj);
		CSize size;
		size.cy=m_sizeHeight;
		size.cx=m_sizeWidth;
		dc.DPtoHIMETRIC(&size);
		pIOleObj->SetExtent(DVASPECT_CONTENT,&size);
		pIOleObj->Release();
	}

	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CVTComboBoxPropPage message handlers

void CVTComboBoxPropPage::OnButtondelete() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	
	ULONG nObjects;
	
	IDispatch** lpDisp=GetObjectArray(&nObjects);
	DISPID dispid;
	OLECHAR* name=L"DeleteString";

	HRESULT hr=lpDisp[0]->GetIDsOfNames(IID_NULL,&name,1,GetUserDefaultLCID(),&dispid);

	ASSERT(SUCCEEDED(hr));

	int index=m_dropdown.GetCurSel();

	VARIANTARG varg;
	::VariantInit(&varg);
	
	varg.vt=VT_I4;
	varg.lVal=index;
	
	DISPPARAMS params;
	params.cArgs=1;
	params.rgvarg=&varg;

	params.cNamedArgs=0;
	params.rgdispidNamedArgs=NULL;

	lpDisp[0]->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_METHOD,&params,NULL,NULL,NULL);
	m_dropdown.DeleteString(index);

	
}

void CVTComboBoxPropPage::OnButtondeleteall() 
{

	ULONG nObjects;
	IDispatch** lpDisp=GetObjectArray(&nObjects);
	DISPID dispid;
	OLECHAR* name=L"DeleteContents";

	lpDisp[0]->GetIDsOfNames(IID_NULL,&name,1,GetUserDefaultLCID(),&dispid);

	DISPPARAMS dispparams =
	{
		NULL,
		NULL,
		0,
		0
	};

	lpDisp[0]->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_METHOD,&dispparams,NULL,NULL,NULL);

	m_dropdown.ResetContent();
	
}

void CVTComboBoxPropPage::OnButtoninsert() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	ULONG nObjects;
	BSTR bstr;

	bstr=m_string.AllocSysString();

	IDispatch** lpDisp=GetObjectArray(&nObjects);
	DISPID dispid;
	OLECHAR* name=L"InsertString";

	HRESULT hr=lpDisp[0]->GetIDsOfNames(IID_NULL,&name,1,GetUserDefaultLCID(),&dispid);

	ASSERT(SUCCEEDED(hr));

	long index;
	if(m_dropdown.GetCount()==0)
		index=0;
	else
		index=m_dropdown.GetCount();
	m_dropdown.InsertString(index,m_string);

	VARIANTARG varg[2];
	::VariantInit(&varg[0]);
	::VariantInit(&varg[1]);

	varg[1].vt=VT_I4;
	varg[1].lVal=index;

	varg[0].vt=VT_BSTR;
	varg[0].bstrVal=bstr;

//	VariantChangeType(&varg[1],&varg[1],0,VT_BSTR);

	DISPPARAMS params;
	params.cArgs=2;
	params.rgvarg=varg;

	params.cNamedArgs=0;
	params.rgdispidNamedArgs=NULL;

UINT arg;
	
	hr=lpDisp[0]->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_METHOD,&params,NULL,NULL,&arg);

//	ASSERT(hr==DISP_E_PARAMNOTFOUND);



	::SysFreeString(bstr);

}

BOOL CVTComboBoxPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();

	int i;
	for(i=0; i<GetStringsCount(); i++)
	{
		m_dropdown.AddString(_bstr_t(GetString(i)));
	}

		ULONG nObjects;

	CClientDC dc(this);

	IDispatch** lpDisp=GetObjectArray(&nObjects);
	IOleObject* pIOleObj;
	lpDisp[0]->QueryInterface(__uuidof(IOleObject),(void**)&pIOleObj);

	CSize size;
	pIOleObj->GetExtent(DVASPECT_CONTENT,&size);
	dc.HIMETRICtoDP(&size);
	m_sizeHeight=size.cy;
	m_sizeWidth=size.cx;

	pIOleObj->Release();
	m_IsCreated=true;
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

long CVTComboBoxPropPage::GetStringsCount()
{
		ULONG nObjects;
	
	IDispatch** lpDisp=GetObjectArray(&nObjects);
	DISPID dispid;
	OLECHAR* name=L"GetStringsCount";

	HRESULT hr=lpDisp[0]->GetIDsOfNames(IID_NULL,&name,1,GetUserDefaultLCID(),&dispid);

	ASSERT(SUCCEEDED(hr));

	DISPPARAMS params;
	params.cArgs=0;
	params.rgvarg=NULL;

	params.cNamedArgs=0;
	params.rgdispidNamedArgs=NULL;

	VARIANT result={VT_EMPTY};

	lpDisp[0]->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_METHOD,&params,&result,NULL,NULL);

	return result.lVal;


}

BSTR CVTComboBoxPropPage::GetString(long idx)
{
	ULONG nObjects;
	IDispatch** lpDisp=GetObjectArray(&nObjects);
	DISPID dispid;
	OLECHAR* name=L"GetString";

	HRESULT hr=lpDisp[0]->GetIDsOfNames(IID_NULL,&name,1,GetUserDefaultLCID(),&dispid);

	ASSERT(SUCCEEDED(hr));

	VARIANTARG varg;
	::VariantInit(&varg);
	
	varg.vt=VT_I4;
	varg.lVal=idx;
	
	DISPPARAMS params;
	params.cArgs=1;
	params.rgvarg=&varg;

	params.cNamedArgs=0;
	params.rgdispidNamedArgs=NULL;

	VARIANT result={VT_EMPTY};

	lpDisp[0]->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_METHOD,&params,&result,NULL,NULL);

	return result.bstrVal;


}
