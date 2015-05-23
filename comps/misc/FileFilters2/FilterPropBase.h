#ifndef __FilterPropBase_h__
#define __FilterPropBase_h__

class CFilterPropBase : public CFileFilterBase
{
	route_unknown();
public:
	virtual int  _GetPropertyCount();
	virtual void _GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);

protected:
	int m_nWidth;
	int m_nHeight;
	int m_nColors;
	int m_nDepth;
	int m_nNumPages;
	char m_strCompression[255];
};

#endif