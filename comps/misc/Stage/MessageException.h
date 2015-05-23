#if !defined(__MessageException_H__)
#define __MessageException_H__

#include "misc_utils.h"

class _MessageBoxCaption
{
public:
	char szBuff[500];
	_MessageBoxCaption()
	{
		szBuff[0] = 0;
		::LoadString(NULL, 0xE000, szBuff, 500);
	}
	operator char*() {return szBuff;}
};

//#define MsgBoxCaption ((char*)_MessageBoxCaption)


class CMessageException
{
public:
	_bstr_t m_bstrMessage;
	CMessageException(_bstr_t bstrMessage)
	{
		m_bstrMessage = bstrMessage;
	};
	CMessageException(int nIds)
	{
		char szBuf[256];
		::LoadString(app::handle(), nIds, szBuf, 256);
		m_bstrMessage = szBuf;
	};
	virtual void ReportError()
	{
		char szBuff[500];
		::LoadString(NULL, 0xE000, szBuff, 500);
		VTMessageBox(m_bstrMessage, szBuff, MB_OK|MB_ICONEXCLAMATION);
	};
};

inline int HandleExc(CMessageException *e, int nRetCode)
{
	BOOL bMessages = ::GetValueInt(::GetAppUnknown(), "Classification", "Messages", 0);
	if (bMessages)
		e->ReportError();
	delete e;
	return nRetCode;
}

#endif
