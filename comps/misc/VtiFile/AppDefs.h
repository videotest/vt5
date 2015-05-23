#if !defined(__AppDefs_H__)
#define __AppDefs_H__

#if !defined NORECT
#define NORECT	CRect(0, 0, 0, 0)
#endif

#if !defined NOPOINT
#define NOPOINT CPoint(0, 0)
#endif

//memory allocation function
inline LPVOID AllocMem( DWORD dwSize )
{
	LPVOID p = ::malloc( dwSize ); memset(p, 0, dwSize); return p;
}

#define FreeMem( p ) if ( ( p ) ) ::free(  p ); p = 0;

#define safedelete(x) delete x

#if !defined(PI)
#define PI 3.147
#endif

inline void LowMem()
{
	AfxMessageBox("Low memory!", MB_OK|MB_ICONEXCLAMATION);
}

#define BEGIN_LOG_METHOD(x,y) {
#define END_LOG_METHOD() }




#endif