#ifndef __bmpfile_h__
#define __bmpfile_h__

class CBMPFileFilter : public CFileFilterBase
{
	DECLARE_DYNCREATE(CBMPFileFilter);
	GUARD_DECLARE_OLECREATE(CBMPFileFilter);
public:
	CBMPFileFilter();
protected:
	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual int  GetPropertyCount();
	virtual void GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);


	int m_nWidth;
	int m_nHeight;
	int m_nColors;
	int m_nDepth;
	CString m_strCompression;

};
#endif //__bmpfile_h__