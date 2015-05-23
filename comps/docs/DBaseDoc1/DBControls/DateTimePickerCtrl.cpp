// DateTimePickerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DBControls.h"
#include "DateTimePickerCtrl.h"


// CDateTimePickerCtrl

IMPLEMENT_DYNAMIC(CDateTimePickerCtrl, CDateTimeCtrl)
CDateTimePickerCtrl::CDateTimePickerCtrl()
{
	m_bReadOnly = FALSE;
}

CDateTimePickerCtrl::~CDateTimePickerCtrl()
{
}


BEGIN_MESSAGE_MAP(CDateTimePickerCtrl, CDateTimeCtrl)
	ON_NOTIFY_REFLECT_EX(DTN_DROPDOWN, OnDtnDropdown)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CDateTimePickerCtrl message handlers

BOOL CDateTimePickerCtrl::OnDtnDropdown(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( m_bReadOnly )
	{
		// read-only - берем текущую дату и установи ее в качестве диапазона в календаре
		CTime time;
		GetTime( time );
		CMonthCalCtrl* pMonthCtrl = GetMonthCalCtrl();
		if( pMonthCtrl )
			pMonthCtrl->SetRange( &time, &time );
	}
	else
	{
		// сбрасываем диаппазон в календаре
		CMonthCalCtrl* pMonthCtrl = GetMonthCalCtrl();
		if( pMonthCtrl )
			pMonthCtrl->SetRange( (CTime *)(NULL), (CTime *)(NULL));
	}

	*pResult = 0;
	return FALSE;
}

void CDateTimePickerCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( m_bReadOnly )
	{
		MessageBeep(0);
		return;
	}

	CDateTimeCtrl::OnChar(nChar, nRepCnt, nFlags);
}
