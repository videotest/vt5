#if !defined(__ExposureMask_H__)
#define __ExposureMask_H__

#include "CamValues.h"

extern CCamBoolValue g_OUExpEnabled;

class CExposureMask
{
	bool m_bInit;
//	bool m_bEnable;
	int  m_nUE8;
	int  m_nOE8;
	RGBQUAD m_rgbUE,m_rgbOE;
public:
	CExposureMask();
	void Init();
	void Reinit();
	bool IsEnabled();
	void SetMask(LPBITMAPINFOHEADER lpbi, LPVOID lpData, bool bValidData);
	void SetMask(IUnknown *punkImage); // punkImage MUST be true color image
};


#endif
