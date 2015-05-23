// ViewAXPpg.cpp : Implementation of the CViewAXPropPage property page class.

#include "stdafx.h"
#include "ViewAX.h"
#include "ViewAXPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
IMPLEMENT_DYNCREATE(CViewAXPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CViewAXPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CViewAXPropPage)
	ON_CBN_CLOSEUP(IDC_COMBO_VIEW, OnCloseupComboView)
	ON_EN_SETFOCUS(IDC_EDIT_OBJECT, OnSetfocusEditObject)
	ON_EN_KILLFOCUS(IDC_EDIT_OBJECT, OnKillfocusEditObject)
	ON_CBN_CLOSEUP(IDC_COMBO_DEFINITION, OnCloseupComboDefinition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
#ifdef _DEBUG
GUARD_IMPLEMENT_OLECREATE_CTRL(CViewAXPropPage, "VIEWAX.ViewAXPropPageD.1",
	0x3596f84d, 0x3c93, 0x4d56, 0xbf, 0xc8, 0xa2, 0xaf, 0x3e, 0x7d, 0xce, 0x3f)
#else
// {DBF67F88-356D-409f-9927-F76A73838477}
GUARD_IMPLEMENT_OLECREATE_CTRL(CViewAXPropPage, "VIEWAX.ViewAXPropPage.1",
0xdbf67f88, 0x356d, 0x409f, 0x99, 0x27, 0xf7, 0x6a, 0x73, 0x83, 0x84, 0x77);
#endif


/////////////////////////////////////////////////////////////////////////////
// CViewAXPropPage::CViewAXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CViewAXPropPage

BOOL CViewAXPropPage::CViewAXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	return UpdateRegistryPage(bRegister, AfxGetInstanceHandle(), IDS_VIEWAX_PPG);
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXPropPage::CViewAXPropPage - Constructor

CViewAXPropPage::CViewAXPropPage() :
	COlePropertyPage(IDD, IDS_VIEWAX_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CViewAXPropPage)
	m_strProgID = _T("");
	m_nDPI = 0;
	m_fZoom = 0.0;
	m_nAlign = -1;
	m_strObject = _T("");
	m_bUseActiveView = FALSE;
	m_nObjectDefinition = -1;
	//}}AFX_DATA_INIT

	m_sptrApp = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXPropPage::DoDataExchange - Moves data between page and properties

void CViewAXPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CViewAXPropPage)
	DDX_Control(pDX, IDC_COMBO_VIEW, m_cbView);
	DDP_Text(pDX, IDC_EDIT_PROGID, m_strProgID, _T("ViewProgID") );
	DDX_Text(pDX, IDC_EDIT_PROGID, m_strProgID);
	DDP_Text(pDX, IDC_EDIT_DPI, m_nDPI, _T("DPI") );
	DDX_Text(pDX, IDC_EDIT_DPI, m_nDPI);
	DDP_Text(pDX, IDC_EDIT_ZOOM, m_fZoom, _T("Zoom") );
	DDX_Text(pDX, IDC_EDIT_ZOOM, m_fZoom);
	DDP_CBIndex(pDX, IDC_COMBO_ALIGN, m_nAlign, _T("Mode") );
	DDX_CBIndex(pDX, IDC_COMBO_ALIGN, m_nAlign);
	DDP_Text(pDX, IDC_EDIT_OBJECT, m_strObject, _T("ViewObject") );
	DDX_Text(pDX, IDC_EDIT_OBJECT, m_strObject);
	DDP_CBIndex(pDX, IDC_COMBO_DEFINITION, m_nObjectDefinition, _T("ObjectDefinition") );
	DDX_CBIndex(pDX, IDC_COMBO_DEFINITION, m_nObjectDefinition);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CViewAXPropPage message handlers

void CViewAXPropPage::OnCloseupComboView() 
{
	// TODO: Add your control notification handler code here
	_UpdateProgID();
}


IUnknown* CViewAXPropPage::_GetDataByObjectName(long nTemplPos, const char *szName)
{
	IUnknown* punkData = 0;
	IUnknown* punkObj = 0;

	long nDocPos = 0;
	m_sptrApp->GetFirstDocPosition(nTemplPos, &nDocPos);
	while(nDocPos)
	{
		m_sptrApp->GetNextDoc(nTemplPos, &nDocPos, &punkData);
		punkObj = FindObjectByName(punkData, szName);
		if(punkObj)
		{
			//punkObj->Release();
			punkData->Release();
			return punkObj;
		}
		if(punkData)
			punkData->Release();
	}
	return 0;
}


void CViewAXPropPage::_RebuildViewsDesc()
{
	ULONG nControls = 0;
	LPDISPATCH FAR* ppDisp = GetObjectArray(&nControls);
	ASSERT(nControls > 0);
	IViewCtrlPtr sptrVC(ppDisp[0]);
	if(sptrVC == 0) return;
	IUnknown* punkApp = 0;
	sptrVC->GetApp(&punkApp);
	m_sptrApp = punkApp;
	if(punkApp)
		punkApp->Release();
	if(m_sptrApp == 0)
		return;

	m_cbView.ResetContent();
	m_arrProgID.RemoveAll();

	UpdateData();
	CString strObjName(m_strObject);
	
	long pos = 0;
	m_sptrApp->GetFirstDocTemplPosition(&pos);
	while(pos)
	{
		IUnknown* punkObj = _GetDataByObjectName(pos, strObjName);

		BSTR bstrName = 0;
		m_sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
		CString strTempl(bstrName);
		::SysFreeString(bstrName);

		bool bHaveObject = punkObj != 0;
		//if(punkObj)
		{
			m_sptrApp->GetFirstDocTemplPosition(&pos);

			while(pos)
			{

				m_sptrApp->GetNextDocTempl(&pos, &bstrName, 0);
				strTempl = bstrName;
				::SysFreeString(bstrName);


				_bstr_t bstrType(::GetObjectKind(punkObj));
				
			
				CCompRegistrator	rv( strTempl+"\\"+szPluginView );
				long nComps = rv.GetRegistredComponentCount();
				for(long i = 0; i < nComps; i++)
				{
					CString strProgID = rv.GetComponentName(i);
					
					BSTR bstrProgID = strProgID.AllocSysString();
					IViewPtr sptrV(bstrProgID);
					::SysFreeString(bstrProgID);

					DWORD dwMatch = mvNone;
					if(bHaveObject)
						sptrV->GetMatchType( bstrType, &dwMatch );
					if((dwMatch > mvNone) || (!bHaveObject && m_nObjectDefinition != odFixedObject))
					{
						BSTR bstrName = 0;
						INamedObject2Ptr	sptrNO(sptrV);
						sptrNO->GetName(&bstrName);
						CString strVName(bstrName);
						::SysFreeString(bstrName);
						
						if(m_cbView.FindString(-1, strVName) == CB_ERR)
						{
							m_cbView.AddString(strVName);

							m_arrProgID.Add(strProgID);
						}
					}
				}
			}
			if(punkObj)
				punkObj->Release();

			break;
		}
	}

	bool bSetUp = false;
	for(int i = 0; i < m_arrProgID.GetSize(); i++)
	{
		if(m_strProgID == m_arrProgID[i])
		{
			//m_cbView.SetCurSel(i);
			_UpdateProgID(i);
			bSetUp = true;
			break;
		}
	}

	if(!bSetUp)
		_UpdateProgID(0);
}

void CViewAXPropPage::_UpdateProgID(int nSel)
{
	if(nSel == -1)
	{
		nSel = m_cbView.GetCurSel();

		if(nSel == CB_ERR)
			return;
	}
	else
		m_cbView.SetCurSel(nSel);

	m_strProgID = "";
	if(nSel >= 0 && nSel < m_arrProgID.GetSize())
	{
		m_strProgID = m_arrProgID[nSel];
	}

	GetDlgItem(IDC_EDIT_PROGID)->SetWindowText(m_strProgID);

	UpdateData();
}

void CViewAXPropPage::OnSetfocusEditObject() 
{
	// TODO: Add your control notification handler code here
	//m_cbView.EnableWindow(FALSE);
}

void CViewAXPropPage::OnKillfocusEditObject() 
{
	// TODO: Add your control notification handler code here
	_RebuildViewsDesc();

	//m_cbView.EnableWindow(TRUE);
}

BOOL CViewAXPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_nObjectDefinition == odActiveView)
	{
		m_cbView.ResetContent();
		m_arrProgID.RemoveAll();
		GetDlgItem(IDC_EDIT_PROGID)->SetWindowText("");
		GetDlgItem(IDC_EDIT_OBJECT)->SetWindowText("");
		GetDlgItem(IDC_EDIT_OBJECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_VIEW)->EnableWindow(FALSE);
		return TRUE;
	}
	else if(m_nObjectDefinition == odActiveObject)
	{
		GetDlgItem(IDC_EDIT_OBJECT)->SetWindowText("");
		GetDlgItem(IDC_EDIT_OBJECT)->EnableWindow(FALSE);
	}
	
	_RebuildViewsDesc();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewAXPropPage::OnCloseupComboDefinition() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	GetDlgItem(IDC_EDIT_OBJECT)->EnableWindow();
	GetDlgItem(IDC_COMBO_VIEW)->EnableWindow();
	bool bRebuildViewsDesc = true;
	switch(m_nObjectDefinition)
	{
	case odActiveView:
		m_cbView.ResetContent();
		m_arrProgID.RemoveAll();
		bRebuildViewsDesc = false;
		GetDlgItem(IDC_EDIT_PROGID)->SetWindowText("");
		GetDlgItem(IDC_EDIT_OBJECT)->SetWindowText("");
		GetDlgItem(IDC_EDIT_OBJECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_VIEW)->EnableWindow(FALSE);
		break;
	case odActiveObject:
		GetDlgItem(IDC_EDIT_OBJECT)->SetWindowText("");
		GetDlgItem(IDC_EDIT_OBJECT)->EnableWindow(FALSE);
		break;
	default: //odFixedObject
		break;
	}
	if(bRebuildViewsDesc)
		_RebuildViewsDesc();
}
*/