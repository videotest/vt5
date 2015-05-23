#include "stdafx.h"
#include "cryptedfile.h"
#include "alloc.h"

CCryptedFile::CCryptedFile(void) : CFile()
{
	m_bCrypted = false;
}

CCryptedFile::CCryptedFile(HANDLE hFile) : CFile(hFile)
{
	m_bCrypted = false;
}

CCryptedFile::CCryptedFile(LPCTSTR lpszFileName, UINT nOpenFlags) : CFile(lpszFileName, nOpenFlags)
{
	m_bCrypted = false;
}

CCryptedFile::~CCryptedFile(void)
{
}

UINT CCryptedFile::Read( void* lpBuf, UINT nCount )
{ // Недошифрование - xor с 0x55
	UINT cnt = __super::Read( lpBuf, nCount );
	if(!m_bCrypted) return cnt;

	BYTE* p = (BYTE*)lpBuf;
	for(UINT i=0; i<cnt; i++)
		p[i]^=0x55;
	return cnt;
}

void CCryptedFile::Write(const void* lpBuf, UINT nCount)
{ // Недошифрование - xor с 0x55
	if(!m_bCrypted) return __super::Write(lpBuf, nCount);

	const UINT maxlen = 1024;
	BYTE buf[maxlen];
	BYTE* p = (BYTE*)lpBuf;

	while(nCount>0)
	{
		UINT cnt = min(nCount, maxlen);
		for(UINT i=0; i<cnt; i++)
			buf[i] = p[i]^0x55;
		__super::Write(buf, cnt);
		p += cnt;
		nCount -= cnt;
	}
}
