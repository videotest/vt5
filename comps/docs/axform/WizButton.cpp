// WizButton.cpp : implementation file
//

#include "stdafx.h"
#include "axform.h"
#include "WizButton.h"
#include "FormManager.h"

#include "formpage.h"
#include "button_messages.h"
/////////////////////////////////////////////////////////////////////////////
// CWizButton

CWizButton::CWizButton(CWizard* pOwner, HWND hwndParent, UINT nID, CString strCaption)
{
	Create(strCaption, WS_CHILD|WS_VISIBLE|WS_TABSTOP/*|BS_PUSHBUTTON*/, NORECT, CWnd::FromHandle(hwndParent), nID);
	m_pWizard = pOwner;
	m_pFormManager = m_pWizard->GetFormManager();
	SetFont(CWnd::FromHandle(hwndParent)->GetFont());
//
	m_hwnd_notify = hwndParent;
	m_id = nID;

	while( HWND hwnd = ::GetParent( m_hwnd_notify ) )
		m_hwnd_notify = hwnd;
	
	::SendMessage( m_hwnd_notify, WM_SETSHEETBUTTON, m_id, SHEET_BUTTON_CREATED );
}

CWizButton::CWizButton(CFormManager* pOwner, HWND hwndParent, UINT nID, CString strCaption)
{
	Create(strCaption, WS_CHILD|WS_VISIBLE|WS_TABSTOP/*|BS_PUSHBUTTON*/, NORECT, CWnd::FromHandle(hwndParent), nID);
	m_pFormManager = pOwner;
	m_pWizard = 0;
	SetFont(CWnd::FromHandle(hwndParent)->GetFont());
//
	m_hwnd_notify = hwndParent;
	m_id = nID;

	while( HWND hwnd = ::GetParent( m_hwnd_notify ) )
		m_hwnd_notify = hwnd;
	
	::SendMessage( m_hwnd_notify, WM_SETSHEETBUTTON, m_id, SHEET_BUTTON_CREATED );
}

CWizButton::~CWizButton()
{
	m_pWizard = 0;
	m_pFormManager = 0;
}


BEGIN_MESSAGE_MAP(CWizButton, CButton)
	//{{AFX_MSG_MAP(CWizButton)
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_COMMAND(777, OnBtnClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizButton message handlers


void CWizButton::OnBtnClick()
{
	switch( m_id )
	{
	case WIZB_NEXT:
		m_pWizard->Next();
		break;
	case WIZB_BACK:
		m_pWizard->Prev();
		break;
	case WIZB_FINISH:
		m_pWizard->Cancel();
		break;
	case IDOK:
		m_pFormManager->OnPageOk();
		break;
	case IDCANCEL:
		m_pFormManager->OnPageCancel();
		break;
	case IDAPPLY:
		m_pFormManager->OnPageApply();
		break;
	case IDHELP:
		if( m_pWizard )
		{
			if( m_pWizard->GetActivePage() )
				m_pWizard->GetActivePage()->OnHelp();
		}
		else
			m_pFormManager->OnPageHelp();
		break;
	}
}


void CWizButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	
	::SendMessage( m_hwnd_notify, WM_SETSHEETBUTTON, m_id, bEnable?SHEET_BUTTON_ENABLED:SHEET_BUTTON_DISABLED );
}
