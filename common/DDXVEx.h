#pragma once

#include "defs.h"
#include "image5.h"


// Support for smart dialog data exchange and validation
class std_dll CValueLimits
{
	int m_nMin,m_nMax;
public:
	CValueLimits(int nMin, int nMax)
	{
		m_nMin = nMin;
		m_nMax = nMax;
	}
	int Validate(int n)
	{
		return n<m_nMin?m_nMin:n>m_nMax?m_nMax:n;
	}
	bool Check(int n)
	{
		return n>=m_nMin&&n<=m_nMax;
	}
	int Default()
	{
		return m_nMin;
	}
	CString FormatTipString();
};

std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, BYTE& value, bool *pbValid = NULL);
std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, short& value, bool *pbValid = NULL);
std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, int& value, bool *pbValid = NULL);
std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, UINT& value, bool *pbValid = NULL);
std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, long& value, bool *pbValid = NULL);
std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, DWORD& value, bool *pbValid = NULL);
std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, LONGLONG& value, bool *pbValid = NULL);
std_dll void DDX_Text_Ex(CDataExchange* pDX, int nIDC, ULONGLONG& value, bool *pbValid = NULL);

#define COLOR_INVALID_VALUE RGB(255,60,60)

// Only one tool per window supported!
std_dll void SetToolTip(CToolTipCtrl &ToolTipCtrl, CWnd *pWnd, bool bNeed, LPCTSTR lpctstrText);

