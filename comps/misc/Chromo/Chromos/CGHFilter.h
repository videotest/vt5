#if !defined(__CGHFilter_H__)
#define __CGHFilter_H__


class CCGHFileFilter : public CFileFilterBase  
{
	DECLARE_DYNCREATE(CCGHFileFilter);
	GUARD_DECLARE_OLECREATE(CCGHFileFilter);

	CStringArray m_aPanesNames;
	bool InitPanesArray(LPCTSTR pszFileName, CStringArray &sa, LPCTSTR lpszCC);
	void InitPanesNames(CImageWrp &pImgWrp, CStringArray &sa);
	void DoAttachPaneData(IImage *pimg, int nPane, LPBITMAPINFOHEADER pbi, LPBYTE lpData);
	bool ReadBmp(const char *pszFileName, IUnknown *punkData, int nPane, LPCTSTR pszCC);
	void DoGetPaneData(IImage *pimg, int nPane, LPBITMAPINFOHEADER pbi, LPBYTE lpData);
	bool WriteBmp(const char *pszFileName, IUnknown *punkData, int nPane);
public:
	CCGHFileFilter();
	virtual ~CCGHFileFilter() {};

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual void OnSetHandlers(){};

	virtual int  GetPropertyCount();
	virtual void GetPropertyByName(BSTR bstrName, VARIANT *pvarVal) {};
	virtual void GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);
	
	int m_nWidth;
	int m_nHeight;
	int m_nColors;
	int m_nDepth;
	int m_nNumPages;
	CString m_strCompression;
};








#endif
