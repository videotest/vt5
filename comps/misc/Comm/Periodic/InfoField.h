#if !defined(__InfoField_H__)
#define __InfoField_H__

class CComThread : public CWinThread
{
public:
	virtual BOOL InitInstance();
};

struct CommData
{
	WORD x,y;
	WORD flag;
};


void AddObject(IUnknown *punkDoc, LPCTSTR lpszString);
CString FormatCoordinates();
extern LONG s_bRun;
extern CommData g_CommData;
extern LONG s_bCapture;
extern CRITICAL_SECTION g_cs;
extern CComThread *g_pThread;


class CActionReadCoords : public CActionBase
{
	DECLARE_DYNCREATE(CActionReadCoords)
	GUARD_DECLARE_OLECREATE(CActionReadCoords)
public:
	CActionReadCoords();
	~CActionReadCoords();
public:
	virtual bool Invoke();
};


#endif
