#pragma once

class CCryptedFile :
	public CFile
{
public:
	CCryptedFile(void);
	CCryptedFile(HANDLE hFile);
	CCryptedFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	~CCryptedFile(void);
	virtual UINT Read( void* lpBuf, UINT nCount );
	virtual void Write(const void* lpBuf, UINT nCount);

	bool m_bCrypted;
};
