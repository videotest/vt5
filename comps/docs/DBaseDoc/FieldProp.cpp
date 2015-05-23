// FieldProp.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "FieldProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "obj_types.h"

/////////////////////////////////////////////////////////////////////////////
// CFieldProp dialog


CFieldProp::CFieldProp() : CPropertyPage(CFieldProp::IDD)
{
	//{{AFX_DATA_INIT(CFieldProp)
	m_bAutoLabel = FALSE;
	m_strCaption = _T("");
	m_strDefValue = _T("");
	m_bEnableEnumeration = FALSE;
	m_strField = _T("");
	m_strTable = _T("");
	m_bHasDefValue = FALSE;
	m_nLabelPos = -1;
	//}}AFX_DATA_INIT
}


void CFieldProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldProp)
	DDX_Check(pDX, IDC_AUTOLABEL, m_bAutoLabel);
	DDX_Text(pDX, IDC_CAPTION, m_strCaption);
	DDX_Text(pDX, IDC_DEF_VALUE, m_strDefValue);
	DDX_Check(pDX, IDC_ENABLEENUMERATION, m_bEnableEnumeration);
	DDX_Text(pDX, IDC_FIELD_NAME, m_strField);
	DDX_Text(pDX, IDC_TABLE_NAME, m_strTable);
	DDX_Check(pDX, IDC_HAS_DEF_VALUE, m_bHasDefValue);
	DDX_Radio(pDX, IDC_RADIO_LEFT, m_nLabelPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFieldProp, CPropertyPage)
	//{{AFX_MSG_MAP(CFieldProp)
	ON_BN_CLICKED(IDC_HAS_DEF_VALUE, OnHasDefValue)
	ON_BN_CLICKED(IDC_AUTOLABEL, OnAutolabel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldProp message handlers

BOOL CFieldProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	OnHasDefValue();
	OnAutolabel();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CFieldProp::OnHasDefValue() 
{
	UpdateData( true );

	CWnd* pWnd = GetDlgItem( IDC_DEF_VALUE );
	if( !pWnd )	
		return;

	if( m_bHasDefValue )
		pWnd->EnableWindow( true );
	else
		pWnd->EnableWindow( false );	

	AnalizeVTObject();
}

/////////////////////////////////////////////////////////////////////////////
void CFieldProp::OnAutolabel() 
{	
	UpdateData( true );

	if( GetDlgItem( IDC_CAPTION ) )
		GetDlgItem( IDC_CAPTION )->EnableWindow( m_bAutoLabel );

	if( GetDlgItem( IDC_RADIO2 ) )
		GetDlgItem( IDC_RADIO2 )->EnableWindow( m_bAutoLabel );

	if( GetDlgItem( IDC_RADIO_LEFT ) )
		GetDlgItem( IDC_RADIO_LEFT )->EnableWindow( m_bAutoLabel );

	AnalizeVTObject();
}

/////////////////////////////////////////////////////////////////////////////
void CFieldProp::AnalizeVTObject()
{
	if( m_ptrDoc == 0 )
		return;

	FieldType ft = ::GetFieldType( m_ptrDoc, m_strTable, m_strField );
	if( ft == ftVTObject )
	{
		if( GetDlgItem(IDC_ENABLEENUMERATION) )
			GetDlgItem(IDC_ENABLEENUMERATION)->EnableWindow( false );

		if( GetDlgItem(IDC_HAS_DEF_VALUE) )
			GetDlgItem(IDC_HAS_DEF_VALUE)->EnableWindow( false );

		if( GetDlgItem(IDC_DEF_VALUE) )
			GetDlgItem(IDC_DEF_VALUE)->EnableWindow( false );
	}
}
/////////////////////////////////////////////////////////////////////////////
// CObjFieldPropPage property page

IMPLEMENT_DYNCREATE(CObjFieldPropPage, CPropertyPage)

CObjFieldPropPage::CObjFieldPropPage() : CPropertyPage(CObjFieldPropPage::IDD)
{
	//{{AFX_DATA_INIT(CObjFieldPropPage)
	m_strViewType = _T("");
	m_bAutoAsign = FALSE;
	//}}AFX_DATA_INIT
}

CObjFieldPropPage::~CObjFieldPropPage()
{
}

void CObjFieldPropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjFieldPropPage)
	DDX_CBString(pDX, IDC_VIEW_TYPE, m_strViewType);
	DDX_Check(pDX, IDC_AUTO_ASIGN, m_bAutoAsign);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjFieldPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CObjFieldPropPage)
	ON_BN_CLICKED(IDC_AUTO_ASIGN, OnAutoAsign)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjFieldPropPage message handlers

void CObjFieldPropPage::SetBaseType( CString strBaseType )
{
	m_strBaseType = strBaseType;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObjFieldPropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	

	if( m_strBaseType.IsEmpty() )
	{
		if( GetDlgItem(IDC_VIEW_TYPE) )
			GetDlgItem(IDC_VIEW_TYPE)->EnableWindow( FALSE );

		if( GetDlgItem(IDC_AUTO_ASIGN) )
			GetDlgItem(IDC_AUTO_ASIGN)->EnableWindow( FALSE );

		return false;
	}		

	_bstr_t bstrType = m_strBaseType;

	//int resnumcur;
	CComboBox* pComo = (CComboBox*)GetDlgItem( IDC_VIEW_TYPE );
	if( pComo )
	{
		//int numcur=-1;
		//CString swintext;
		//pComo->GetWindowText(swintext);

		pComo->ResetContent();

		int count=0;
		int res=-1;
		BOOL mFind=false;
		CString s;

		CStringArray arViewProgID;
		GetAvailableViewProgID( arViewProgID );
		for( int i=0;i<arViewProgID.GetSize();i++ )
		{
			try
			{
				BSTR bstr = arViewProgID[i].AllocSysString();
				BSTR bstr_old=bstr;
				IViewPtr ptrV( bstr );

				if( bstr )
					::SysFreeString( bstr );	bstr = 0;

				DWORD dwMatch = mvNone; 
				ptrV->GetMatchType( bstrType, &dwMatch );
				if( !( dwMatch & mvFull ) && !( dwMatch & mvPartial ) )
					continue;

				//numcur++;
				INamedObject2Ptr	ptrNO( ptrV );
				BSTR bstrName = 0;
				ptrNO->GetUserName( &bstrName );
				
				CString SOne,STwo,SThree;
				SOne.Format("DBViewFilter\\%s",m_strBaseType);
				STwo=(CString)(bstr_old);
				mFind=false;
				count=0;
				res=-1;
				while(!mFind)
				{
					count=STwo.Find('.',res+1);
					if(count>=0)
						res=count;
					else
						mFind=true;
				}
				if(mFind)
				{
					STwo.Delete(0,res+1);
					SThree.Format("Name%s",STwo);
				}
				int OnOffView = ::GetValueInt(::GetAppUnknown(), SOne, STwo, 2);
				if( OnOffView < 2 )
				{
					if( OnOffView ) 
					{
						s = ::GetValueString(::GetAppUnknown(), SOne, SThree, "");
						if( !s.IsEmpty() )
						{
							pComo->AddString( s );
							BSTR bbb=s.AllocSysString();
							ptrNO->SetUserName(bbb);
							if(bbb)
								::SysFreeString(bbb);
							bbb=0;
						}
						else
				pComo->AddString( CString( bstrName ) );
					}
					//else
					//	numcur--;
				}
				else
					pComo->AddString( CString( bstrName ) );

				//if(swintext==(CString)(bstrName))
				//	resnumcur=numcur;

				if( bstrName )
					::SysFreeString( bstrName );	
				bstrName = 0;
			}
			catch(...)
			{

			}
		}
	}	

	AnalizeAppearance();

	UpdateData( FALSE );
	
	/*CString sc;
	pComo->GetLBText(resnumcur,sc);
	pComo->GetWindowText(sc);*/

	return TRUE;
}

void CObjFieldPropPage::OnAutoAsign() 
{
	UpdateData( TRUE );
	AnalizeAppearance();	
}

void CObjFieldPropPage::AnalizeAppearance()
{
	CWnd* pwnd = GetDlgItem(IDC_VIEW_TYPE);
	if( !pwnd )
		return;			

	pwnd->EnableWindow( !m_bAutoAsign );
}