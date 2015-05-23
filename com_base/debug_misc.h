#ifndef __debug_misc_h__
#define __debug_misc_h__

//#ifdef _DEBUG
//#include <crtdbg.h>
//#define new	::new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif _DEBUG

#ifdef _DEBUG

#if _MSC_VER >= 1300
//#include <xdebug>	
#endif

#include <crtdbg.h>

inline void* __cdecl operator new(size_t size, const char* psz_file, int nline )
{
	//_lock(_HEAP_LOCK);
	
	void* p = ::operator new( size, _NORMAL_BLOCK, psz_file, nline );
	
	//_unlock(_HEAP_LOCK);

	return p;
}

inline void __cdecl operator delete(void* p)
{
	//_lock(_HEAP_LOCK);

	_free_dbg( p, _NORMAL_BLOCK );
	
	//_unlock(_HEAP_LOCK);
}


inline void __cdecl operator delete(void* p, const char* psz_file, int nline )
{
	::operator delete(p);
}


#define DBG_NEW new(__FILE__, __LINE__)

#endif//_DEBUG





#endif//__debug_misc_h__