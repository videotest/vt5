#if !defined(__TimeTest_H__)
#define __TimeTest_H__


#include "MessageException.h"

class CTimeTest
{
	DWORD m_dwTickCount;
	_bstr_t m_bstrMsg;
public:
	CTimeTest(const char *p = " ")
	{
		m_bstrMsg = p;
		m_dwTickCount = GetTickCount();
	}
	~CTimeTest()
	{
		MsgBox();
	}

	void MsgBox()
	{
		if (m_dwTickCount != 0)
		{
			DWORD dwTicks = GetTickCount();
			char szBuff[50];
			sprintf(szBuff, "%s : %d msecs", (const char *)m_bstrMsg, dwTicks-m_dwTickCount);
			_MessageBoxCaption MsgBoxCaption;
			VTMessageBox(szBuff, MsgBoxCaption, MB_OK|MB_ICONEXCLAMATION);
			m_dwTickCount = 0;
		}
	}
};





















#endif
