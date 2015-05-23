// VTWidthPpg.cpp : Implementation of the CVTWidthPropPage property page class.

#include "stdafx.h"
#include "vtcontrols2.h"
#include "VTWidthPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTWidthPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTWidthPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTWidthPropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTWidthPropPage, "VTCONTROLS2.VTWidthPropPage.1",
	0xdea234fc, 0xdeb0, 0x438d, 0x8c, 0xca, 0xd, 0x40, 0x4e, 0x5a, 0x36, 0xe7)


/////////////////////////////////////////////////////////////////////////////
// CVTWidthPropPage::CVTWidthPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTWidthPropPage

BOOL CVTWidthPropPage::CVTWidthPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTWIDTH_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthPropPage::CVTWidthPropPage - Constructor

CVTWidthPropPage::CVTWidthPropPage() :
	COlePropertyPage(IDD, IDS_VTWIDTH_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTWidthPropPage)
	m_Max = 0;
	m_Min = 0;
	m_Width = 0;
	m_orientation = FALSE;
	m_sizeWidth=100;
	m_sizeHeight=50;
	m_IsCreated=false;
	m_checked = FALSE;
	m_sizeHeight = 0;
	m_sizeWidth = 0;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CVTWidthPropPage::DoDataExchange - Moves data between page and properties

void CVTWidthPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTWidthPropPage)
	DDP_Text(pDX, IDC_EDITMAX, m_Max, _T("Max") );
	DDX_Text(pDX, IDC_EDITMAX, m_Max);
	DDP_Text(pDX, IDC_EDITMIN, m_Min, _T("Min") );
	DDX_Text(pDX, IDC_EDITMIN, m_Min);
	DDP_Text(pDX, IDC_EDITWIDTH, m_Width, _T("Width") );
	DDX_Text(pDX, IDC_EDITWIDTH, m_Width);
	DDP_Check(pDX, IDC_CHECKORIENTATION, m_orientation, _T("Orientation") );
	DDX_Check(pDX, IDC_CHECKORIENTATION, m_orientation);
	DDP_Check(pDX, IDC_CHECKCHECKED, m_checked, _T("Checked") );
	DDX_Check(pDX, IDC_CHECKCHECKED, m_checked);
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
// CVTWidthPropPage message handlers

BOOL CVTWidthPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
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

/*void CVTWidthPropPage::OnButtoncolorbtn() 
{
	CColorDialog dlg;
	if(dlg.DoModal()!=IDOK) return;

	COLORREF color=dlg.GetColor();

	ULONG nObjects;
	IDispatch** lpDisp=GetObjectArray(&nObjects);
	DISPID dispid;
	OLECHAR* name=L"ColorText";

	HRESULT hr=lpDisp[0]->GetIDsOfNames(IID_NULL,&name,1,GetUserDefaultLCID(),&dispid);
	ASSERT(SUCCEEDED(hr));

	VARIANTARG varg;
	VARIANTARG result;
	::VariantInit(&varg);
	
	varg.vt=VT_I4;
	varg.lVal=color;

	DISPPARAMS param;
	param.cArgs=1;
	param.rgvarg=&varg;

	param.cNamedArgs=0;
	param.rgdispidNamedArgs=NULL;

    UINT arg=0;
	
	hr=lpDisp[0]->Invoke(dispid,
		                 IID_NULL,
						 GetUserDefaultLCID(),
						 DISPATCH_PROPERTYPUT,
						 &param,
						 &result,
						 NULL,
						 &arg);

    //ASSERT(hr!=DISP_E_BADVARTYPE);
	
}

void CVTWidthPropPage::OnButtonColorHighlight() 
{
	CColorDialog dlg;
	if(dlg.DoModal()!=IDOK) return;

	COLORREF color=dlg.GetColor();

	ULONG nObjects;
	IDispatch** lpDisp=GetObjectArray(&nObjects);
	DISPID dispid;
	OLECHAR* name=L"ColorTextHighlight";

	HRESULT hr=lpDisp[0]->GetIDsOfNames(IID_NULL,&name,1,GetUserDefaultLCID(),&dispid);
	ASSERT(SUCCEEDED(hr));

	VARIANTARG varg;
	VARIANTARG result;
	::VariantInit(&varg);
	
	varg.vt=VT_I4;
	varg.lVal=color;

	DISPPARAMS param;
	param.cArgs=1;
	param.rgvarg=&varg;

	param.cNamedArgs=0;
	param.rgdispidNamedArgs=NULL;

    UINT arg=0;
	
	hr=lpDisp[0]->Invoke(dispid,
		                 IID_NULL,
						 GetUserDefaultLCID(),
						 DISPATCH_PROPERTYPUT,
						 &param,
						 &result,
						 NULL,
						 &arg);

}*/
