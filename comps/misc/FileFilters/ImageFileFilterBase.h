// ImageFileFilterBase.h: interface for the CImageFileFilterBase class.
//
//////////////////////////////////////////////////////////////////////
extern "C"
void FillVTImageRow(Image *image, unsigned int _y, unsigned short** pPane, unsigned short numPanes),
	FillMagickImageRow(Image *image, unsigned int _y, unsigned short** pPane, unsigned short numPanes);

class CImageFileFilterBase : public CFileFilterBase  
{
public:
	CImageFileFilterBase();
	virtual ~CImageFileFilterBase();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual void OnSetHandlers(){};

	virtual int  GetPropertyCount();
	virtual void GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);
	
	bool m_bFirstTime;
	bool m_bEnableMonitoring;

	int m_nWidth;
	int m_nHeight;
	int m_nColors;
	int m_nDepth;
	int m_nNumPages;
	CString m_strCompression;

	ErrorHandler m_oldErrorHandle;
	MonitorHandler m_oldMonitorHandle;
	
};


//void ourErrorHandler(const unsigned int error,const char *message, const char *qualifier);
//void ourMonitorHandler(const char *msg,const unsigned int curPos,const unsigned int maxPos);
