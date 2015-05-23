#if !defined(__Comm_H__)
#define __Comm_H__

#include <comdef.h>

struct CCoordData
{
	unsigned x,y,o,z;
};

class CComm
{
	HANDLE m_hFile;
	_bstr_t m_bstrCom;
public:
	CComm();
	~CComm();
	void Open();
	void Close();
	bool IsOpened() {return m_hFile != INVALID_HANDLE_VALUE;}
	bool Read(unsigned nBytes, LPBYTE lpByte);
	bool Write(unsigned nBytes, LPBYTE lpByte);
	void Purge();
};


extern CComm g_Comm;

#endif