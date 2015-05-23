#include "stdafx.h"
#include "common.h"
#include "DDXVEx.h"
#include "PropBag.h"

CString CValueLimits::FormatTipString()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString s;
	s.Format(IDS_MIN_MAX_TOOLTIP, m_nMin, m_nMax);
	return s;
}


static bool __DDX_TextWithFormat(CDataExchange* pDX, int nIDC,
	LPCTSTR lpszFormat, bool *pbValid, ...)
	// only supports windows output formats - no floating point
{
	va_list pData;
	va_start(pData, pbValid);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
   ASSERT( hWndCtrl != NULL );

   bool bValid = true;
	TCHAR szT[64];
	if (pDX->m_bSaveAndValidate)
	{
		void* pResult;

		pResult = va_arg( pData, void* );
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, 64);
		int nMaxLen = szT[0]=='-'?10:9;
		if (strlen(szT) > nMaxLen)
			bValid = false;
		else
			bValid = _stscanf(szT, lpszFormat, pResult) == 1;
		if (pbValid) *pbValid = bValid;
	}
	else
	{
		_vstprintf(szT, lpszFormat, pData);
			// does not support floating point numbers - see dlgfloat.cpp
		::SetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
	return bValid;
}

/////////////////////////////////////////////////////////////////////////////
// Simple formatting to text item

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, BYTE& value, bool *pbValid)
{
	int n = (int)value;
	if (pDX->m_bSaveAndValidate)
	{
		bool bValid = __DDX_TextWithFormat(pDX, nIDC, _T("%u"), pbValid, &n);
		if (n > 255 && pbValid)
			*pbValid = false;
		if (bValid)
			value = (BYTE)n;
	}
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%u"), pbValid, n);
}

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, short& value, bool *pbValid)
{
	if (pDX->m_bSaveAndValidate)
		__DDX_TextWithFormat(pDX, nIDC, _T("%hd"), pbValid, &value);
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%hd"), pbValid, value);
}

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, int& value, bool *pbValid)
{
	if (pDX->m_bSaveAndValidate)
		__DDX_TextWithFormat(pDX, nIDC, _T("%d"), pbValid, &value);
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%d"), pbValid, value);
}

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, UINT& value, bool *pbValid)
{
	if (pDX->m_bSaveAndValidate)
		__DDX_TextWithFormat(pDX, nIDC, _T("%u"), pbValid, &value);
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%u"), pbValid, value);
}

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, long& value, bool *pbValid)
{
	if (pDX->m_bSaveAndValidate)
		__DDX_TextWithFormat(pDX, nIDC, _T("%ld"), pbValid, &value);
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%ld"), pbValid, value);
}

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, DWORD& value, bool *pbValid)
{
	if (pDX->m_bSaveAndValidate)
		__DDX_TextWithFormat(pDX, nIDC, _T("%lu"), pbValid, &value);
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%lu"), pbValid, value);
}

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, LONGLONG& value, bool *pbValid)
{
	if (pDX->m_bSaveAndValidate)
		__DDX_TextWithFormat(pDX, nIDC, _T("%I64d"), pbValid, &value);
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%I64d"), pbValid, value);
}

void DDX_Text_Ex(CDataExchange* pDX, int nIDC, ULONGLONG& value, bool *pbValid)
{
	if (pDX->m_bSaveAndValidate)
		__DDX_TextWithFormat(pDX, nIDC, _T("%I64u"), pbValid, &value);
	else
		__DDX_TextWithFormat(pDX, nIDC, _T("%I64u"), pbValid, value);
}

void SetToolTip(CToolTipCtrl &ToolTipCtrl, CWnd *pWnd, bool bNeed, LPCTSTR lpcsText)
{
	if (ToolTipCtrl.m_hWnd == NULL)
		return;
	CToolInfo ToolInfo;
	BOOL bExist = ToolTipCtrl.GetToolInfo(ToolInfo, pWnd);
	if (bExist)
	{
		if (!bNeed)
			ToolTipCtrl.DelTool(pWnd);
	}
	else
	{
		if (bNeed)
		{
			ToolTipCtrl.AddTool(pWnd, lpcsText);
			ToolTipCtrl.Activate(TRUE);
			//BT5275
			/*CRect rc;
			pWnd->GetWindowRect(rc);
			::SetCursorPos(rc.left+rc.Width()/2, rc.top+rc.Height()/2);*/
		}
	}
}
