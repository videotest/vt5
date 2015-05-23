#if !defined(__TwainWork_H__)
#define __TwainWork_H__

#include "Twcore.h"

class CTwainWork
{
	CTwainDataSource *m_pDataSource;
	BOOL m_bUIControllable;

	bool InitDS(int nXferMech, BOOL fShowUI);
	void DeinitDS();
	int WaitForAquisition();
	BOOL DoEndXfer();
	void DoAbortXfer();
	HGLOBAL DoNativeXfer(BOOL fShowUI);
//	void DoMemoryXfer();
public:
	CTwainWork();
	~CTwainWork();
	
	HGLOBAL ScanNative(CTwainDataSource *pDataSource, BOOL fShowUI);
};



#endif