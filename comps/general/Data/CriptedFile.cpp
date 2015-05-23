#include "stdafx.h"
#include "criptedfile.h"
#include "alloc.h"

CCriptedFile::CCriptedFile(void)
{
}

CCriptedFile::~CCriptedFile(void)
{
}

UINT CCriptedFile::Read( void* lpBuf, UINT nCount )
{ // Недошифрование - xor с 0x55
	UINT cnt = __super::Read( lpBuf, nCount );
	BYTE* p = (BYTE*)lpBuf;
	for(UINT i=0; i<cnt; i++)
		p[i]^=0x55;
	return cnt;
}

void CCriptedFile::Write(const void* lpBuf, UINT nCount)
{ // Недошифрование - xor с 0x55
	const int maxlen = 1024;
	BYTE buf[maxlen];
	BYTE* p = (BYTE*)lpBuf;

	while(nCount>0)
	{
		int cnt = min(nCount, maxlen);
		for(UINT i=0; i<cnt; i++)
			buf[i] = p[i]^0x55;
		p += cnt;
		nCount -= cnt;
	}
}
