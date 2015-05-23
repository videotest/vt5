#if !defined(__Comm_H__)
#define __Comm_H__

#include "MessageException.h"
#include "InfoFieldInt.h"

class CComm
{
	HANDLE m_hFile;
	bool DoRead(InfoFieldData &Data);
public:
	CComm();
	~CComm();
	void Open();
	void Close();
	bool IsOpened() {return m_hFile != INVALID_HANDLE_VALUE;}
	bool ReadInfoField(InfoFieldData &Data);
};


extern CComm g_Comm;



#endif

