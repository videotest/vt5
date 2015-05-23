#pragma once

class CCriptedFile :
	public CFile
{
public:
	CCriptedFile(void);
	~CCriptedFile(void);
	virtual UINT Read( void* lpBuf, UINT nCount );
	virtual void Write(const void* lpBuf, UINT nCount);
};
