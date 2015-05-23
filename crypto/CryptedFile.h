#pragma once
#include <windows.h>

class CSimpleCrypter
{ // последовательное шифрование блоков данных
public:
	CSimpleCrypter() : m_pos(0), cXor(0), szKey(";xDkGlsDFdjz^Gg67H8&*()9Wds0g") {};
	virtual ~CSimpleCrypter() {};
	void EncryptBuf(char* buf, size_t nSize)
	{
		for(size_t i=0; i<nSize; i++)
		{
			_next();
            buf[i] ^= cXor;
		}
	};
	void DecryptBuf(char* buf, size_t nSize)
	{
		for(size_t i=0; i<nSize; i++)
		{
			_next();
            buf[i] ^= cXor;
		}
	};
private:
	const char* szKey;
	size_t m_pos; // позиция для шифрования
	char cXor;
	void _next()
	{
		cXor += szKey[m_pos];
		m_pos++;
		if(!szKey[m_pos]) m_pos = 0;
	}
};
