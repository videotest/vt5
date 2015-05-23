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
public:
	CComm();
	~CComm();
	// Установки берутся из структуры g_Comm.
	void Open();
	void Close();
	bool IsOpened() {return m_hFile != INVALID_HANDLE_VALUE;}
	// Возвращается true если ReadFile вернул true и прочитано нужное 
	// число байт.
	bool Read(unsigned nBytes, LPBYTE lpByte);
	// Возвращается true если ReadFile вернул true. Число прочитанных 
	// байт записыватся по адресу pnRead.
	bool ReadEx(unsigned nBytes, LPBYTE lpByte, DWORD *pnRead);
	// Возвращается true если WriteFile вернул true и записано нужное 
	// число байт.
	bool Write(unsigned nBytes, LPBYTE lpByte);
	// Очищаются буфера COM-порта.
	void Purge();
};


extern CComm g_Comm;

#endif