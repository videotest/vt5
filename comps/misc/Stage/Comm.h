#if !defined(__Comm_H__)
#define __Comm_H__

#include <comdef.h>
#include "LStep4X.h"

class CComm
{
public:
	HANDLE m_hFile;
	_bstr_t m_bstrCom;
	CComm();
	~CComm();
	void Open();
	void Close();
	bool IsOpened() {return m_hFile != INVALID_HANDLE_VALUE;}
	bool Read(unsigned nBytes, char *lpByte);
	bool Write(unsigned nBytes, char const *lpByte);
};


extern CComm g_Comm;

#endif