#ifndef __misc_new_h__
#define __misc_new_h__

#include "new"
#include "memory"

#ifdef _DEBUG
#include "crtdbg.h"

#ifndef __AFXWIN_H__

#if _MSC_VER >= 1300
	#include <xdebug>	
/*	inline void* __cdecl operator new( unsigned int size, int type, const char *file, int line, int	dummy )
	{ 
		void *pres = _malloc_dbg(size, type, file, line);

		if( size == 12 )
		{
			char	sz[100];
			sprintf( sz, "Allocated %d bytes [%08x] - %s %d\n", size, pres, file, line );
			OutputDebugString( sz );
		}
		return pres;
	}
*/
/*	inline void *operator new(size_t sz, const std::_DebugHeapTag_t &tag,	char *file, int line, int	dummy ) _THROW1(std::bad_alloc)
	{
		void *pres = _malloc_dbg(sz, tag._Type, file, line);
		if (pres == 0)
			std::_Nomemory();

		return pres;
	}


	inline void __cdecl operator delete( void *ptr, int type, const char *file, int line, int	dummy )
	{ 
		_free_dbg(ptr, type);
	}
*/
#else //VC6
	//common2 version
	inline void* __cdecl operator new( unsigned int size, int type, const char *file, int line, int	dummy )
	{ 
		void *pres = ::operator new(size, type, file, line); 

		if( size == 12 )
		{
			char	sz[100];
			sprintf( sz, "Allocated %d bytes [%08x] - %s %d\n", size, pres, file, line );
			OutputDebugString( sz );
		}
		return pres;
	}

	inline void __cdecl operator delete( void *ptr, int type, const char *file, int line, int	dummy )
	{ 
		char	sz[100];
		sprintf( sz, "Free [%08x] - %s %d\n", ptr, file, line );
		OutputDebugString( sz );
		::operator delete(ptr, type, file, line); 
	}
	#define new			::new(_NORMAL_BLOCK, __FILE__, __LINE__, 0)
#endif
#else

void AFX_CDECL operator delete(void *ptr, LPCSTR lpszFileName, int nLine);
void AFX_CDECL operator delete(void *ptr, void *p1, LPCSTR lpszFileName, int nLine);
void* AFX_CDECL operator new(size_t nSize, LPCSTR lpszFileName, int nLine);

inline void* __cdecl operator new( unsigned int size, int type, const char *file, int line, int	dummy )
{ 
	void *pres = _malloc_dbg(size, _NORMAL_BLOCK, file, line);
	//void *pres = ::operator new(size, file, line); 
	char	sz[100];
	sprintf( sz, "Alloc [%08x] - %d bytes  - %s %d\n", pres, size, file, line );
	OutputDebugString( sz );

	return pres;
}

inline void __cdecl operator delete( void *ptr, int type, const char *file, int line, int	dummy )
{ 
	char	sz[100];
	sprintf( sz, "Free [%08x] - %s %d\n", ptr, file, line );
	OutputDebugString( sz );
	::operator delete(ptr, file, line); 
}

inline void __cdecl operator delete( void *ptr, void *p1, int type, const char *file, int line, int	dummy )
{ 
	char	sz[100];
	sprintf( sz, "Free [%08x/%08x] - %s %d\n", ptr, p1, file, line );
	OutputDebugString( sz );
	::operator delete(ptr, p1, file, line); 
}




#define new			::new(_NORMAL_BLOCK, __FILE__, __LINE__, 0)
#define delete		::delete

#endif //__AFX

#endif	//debug

#endif //__misc_new_h__