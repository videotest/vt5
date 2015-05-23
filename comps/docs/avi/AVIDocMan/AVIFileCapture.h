#if !defined(__AVIFileCapture_H__)
#define __AVIFileCapture_H__

#include "VTAVIInfo.h"

class CAVIFileCapture
{
	bool m_bInitialized;
	CString m_sFileName;
	PAVIFILE   m_pfile;
	PAVISTREAM m_pstream;
	PGETFRAME  m_pgetframe;
	LPBITMAPINFOHEADER m_pFormatInfo;
public:
	CAVIFileCapture();
	~CAVIFileCapture();
	bool Open(LPCTSTR lpFileName, VTVideoInfo *pinfo);
	void Close();
	bool Capture(int nFrame, IUnknown *punkImg);
	bool IsAviFile(); // Initialized and contain .avi file
};












#endif