#if !defined(__MV500VT5Grab_H__)
#define __MV500VT5Grab_H__

#include "MV500Grab.h"
#include "Input.h"


class CMV500VT5Grab : public CMV500Grab
{
	bool m_bAviPreview;
public:
	CMV500VT5Grab()
	{
		m_bAviPreview = false;
	}
	void CopyGrabbedImage(int nDev, IUnknown *pUnkImg);
	void CopyGrabbedImage(IUnknown *pUnkImg, LPBYTE lpData, DWORD dwSize);
	void CopyGrabbedImage(IDIBProvider *pDIBPrv, int nDevice);
	void Grab(int nDev, IUnknown *punkImg, bool bWait);
	void OnSetValueString(LPCTSTR lpSec, LPCTSTR lpName, LPCTSTR lpValue, CArray<IInputPreviewSite *,IInputPreviewSite *&> &arrSites);
	void OnSetValueInt(LPCTSTR lpSec, LPCTSTR lpName, int nValue, CArray<IInputPreviewSite *,IInputPreviewSite *&> &arrSites);
};











#endif