// ObjectProps.cpp: implementation of the CDataObjectPropsImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectProps.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CDataObjectPropsImpl::CDataObjectPropsImpl()
{
	m_pSheet = 0;

}

CDataObjectPropsImpl::~CDataObjectPropsImpl()
{
	if (m_pSheet)
		delete m_pSheet, m_pSheet = 0;
}

IMPLEMENT_UNKNOWN_BASE(CDataObjectPropsImpl, Props);

HRESULT CDataObjectPropsImpl::XProps::CreatePropSheet()
{
	_try_nested_base(CDataObjectPropsImpl, Props, CreatePropSheet)
	{
		return (pThis->CreateProperties()) ? S_OK : E_NOTIMPL;
	}
	_catch_nested;
}


void CDataObjectPropsImpl::ConstructPropertySheet(CObjectPropSheet* pSheet)
{
	CObjectPropPageGeneral *pPage = new CObjectPropPageGeneral();
	pSheet->AddPage(pPage);
	pPage->AttachObject(pSheet->GetObject());
}

bool CDataObjectPropsImpl::CreateProperties()
{
	CString strName("Properties for ");
	strName += ::GetObjectName((IUnknown*)this);
	
	m_pSheet = new CObjectPropSheet((IUnknown*)this, strName);

	if (!m_pSheet)
		return false;

	ConstructPropertySheet(m_pSheet);

	m_pSheet->DoModal();

	delete m_pSheet, m_pSheet = 0;

	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageBase property page

IMPLEMENT_DYNCREATE(CObjectPropPageBase, CPropertyPage)

BEGIN_MESSAGE_MAP(CObjectPropPageBase, CPropertyPage)
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageGeneral property page

IMPLEMENT_DYNCREATE(CObjectPropPageGeneral, CObjectPropPageBase)

CObjectPropPageGeneral::CObjectPropPageGeneral()
	:	CObjectPropPageBase(CObjectPropPageGeneral::IDD)
{
	//{{AFX_DATA_INIT(CObjectPropPageGeneral)
	m_strObjectName = _T("");
	m_strObjectType = _T("");
	//}}AFX_DATA_INIT
	
	LoadTemplate();
	m_psp.dwFlags |= PSP_USETITLE;
	m_psp.pszTitle = "General";
}

CObjectPropPageGeneral::~CObjectPropPageGeneral()
{
}

void CObjectPropPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CObjectPropPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectPropPageGeneral)
	DDX_Control(pDX, _IDC_OBJECT_ICON, m_Icon);
	DDX_Text(pDX, _IDC_OBJECT_NAME, m_strObjectName);
	DDX_Text(pDX, _IDC_OBJECT_TYPE, m_strObjectType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectPropPageGeneral, CObjectPropPageBase)
	//{{AFX_MSG_MAP(CObjectPropPageGeneral)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectPropPageGeneral message handlers
BOOL CObjectPropPageGeneral::OnInitDialog() 
{
	CObjectPropPageBase::OnInitDialog();

	if (m_sptr == NULL)
		return false;

	m_strObjectName = ::GetObjectName(m_sptr);
	m_strObjectType = ::GetObjectKind(m_sptr);
	UpdateData(false);

	SetWindowText("General");
	return true;
}


bool CObjectPropPageGeneral::LoadTemplate()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	const DLGTEMPLATE* pTemplate;

	if (m_psp.dwFlags & PSP_DLGINDIRECT)
	{
		pTemplate = m_psp.pResource;
	}
	else
	{
		HRSRC hResource = ::FindResource(AfxGetInstanceHandle(), m_psp.pszTemplate, RT_DIALOG);
		HGLOBAL hTemplate = LoadResource(AfxGetInstanceHandle(), hResource);
		pTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);
	}

	ASSERT(pTemplate != NULL);


	m_psp.pResource = pTemplate;
	m_psp.dwFlags |= PSP_DLGINDIRECT;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectPropSheet

IMPLEMENT_DYNAMIC(CObjectPropSheet, CPropertySheet)

CObjectPropSheet::CObjectPropSheet(IUnknown* punk, LPCTSTR lpszTitle)
:	CPropertySheet((lpszTitle) ? lpszTitle : "", NULL, 0)
{
	AttachObject(punk);
	CString strName = ::GetObjectName(m_sptr);
	SetTitle(strName, PSH_PROPTITLE);

	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CObjectPropSheet::~CObjectPropSheet()
{
}


BEGIN_MESSAGE_MAP(CObjectPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CObjectPropSheet)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectPropSheet message handlers

int CObjectPropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_sptr == NULL)
		return -1;

//	CString strName = ::GetObjectName(m_sptr);
//	SetTitle(strName, PSH_PROPTITLE);

	return 0;
}

void CObjectPropSheet::RemovePages() 
{
	int nCount = GetPageCount();
	for (int i = 0; i < nCount; i++)
	{	
		CObjectPropPageBase* pPage = (CObjectPropPageBase*)GetPage(i);
		RemovePage(i);
		delete pPage;
	}
}

void CObjectPropSheet::OnDestroy() 
{
	RemovePages();
	CPropertySheet::OnDestroy();
}


void CObjectPropSheet::PostNcDestroy() 
{
	CPropertySheet::PostNcDestroy();
}
