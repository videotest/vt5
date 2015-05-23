#if !defined(__MultiCam_H__)
#define __MultiCam_H__

class CMultyCamThread : public CWinThread
{
public:
	static long s_bRun;
//	HANDLE m_hevStop;
	CMultyCamThread();
	~CMultyCamThread();
	virtual BOOL InitInstance();
};


#endif
