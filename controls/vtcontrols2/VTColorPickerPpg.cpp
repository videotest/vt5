// VTColorPickerPpg.cpp : Implementation of the CVTColorPickerPropPage property page class.

#include "stdafx.h"
#include "vtcontrols2.h"
#include "VTColorPickerPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTColorPickerPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTColorPickerPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CVTColorPickerPropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTColorPickerPropPage, "VTCONTROLS2.VTColorPickerPropPage.1",
	0x35a1c482, 0x33eb, 0x4247, 0xad, 0xdc, 0xeb, 0xb0, 0x37, 0x6e, 0x53, 0x51)


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerPropPage::CVTColorPickerPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTColorPickerPropPage

BOOL CVTColorPickerPropPage::CVTColorPickerPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_VTCOLORPICKER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerPropPage::CVTColorPickerPropPage - Constructor

CVTColorPickerPropPage::CVTColorPickerPropPage() :
	COlePropertyPage(IDD, IDS_VTCOLORPICKER_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CVTColorPickerPropPage)
	m_likeCombo = FALSE;
	m_sizeHeight = 0;
	m_sizeWidth = 0;
	m_BAW = FALSE;
	m_binary = FALSE;
	m_bEnable = FALSE;
	//}}AFX_DATA_INIT
	m_IsCreated=false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTColorPickerPropPage::DoDataExchange - Moves data between page and properties

void CVTColorPickerPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVTColorPickerPropPage)
	DDP_Check(pDX, IDC_CHECK1, m_likeCombo, _T("LikeCombo") );
	DDX_Check(pDX, IDC_CHECK1, m_likeCombo);
	DDX_Text(pDX, IDC_EDITSIZEHEIGHT, m_sizeHeight);
	DDV_MinMaxInt(pDX, m_sizeHeight, 1, 1000);
	DDX_Text(pDX, IDC_EDITSIZEWIDTH, m_sizeWidth);
	DDV_MinMaxInt(pDX, m_sizeWidth, 1, 1000);
	DDP_Check(pDX, IDC_CHECKBAW, m_BAW, _T("BlackAndWhite") );
	DDX_Check(pDX, IDC_CHECKBAW, m_BAW);
	DDP_Check(pDX, IDC_CHECK2, m_binary, _T("Binary") );
	DDX_Check(pDX, IDC_CHECK2, m_binary);
	DDP_Check(pDX, IDC_ENABLE, m_bEnable, _T("Enabled") );
	DDX_Check(pDX, IDC_ENABLE, m_bEnable);
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
// CVTColorPickerPropPage message handlers

BOOL CVTColorPickerPropPage::OnInitDialog() 
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