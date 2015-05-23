#if !defined(__Array_H__)
#define __Array_H__

#include "misc_utils.h"



template <class TYPE> class _array_t : public _ptr_t2<TYPE>
{
public:
	_array_t ( TYPE *p ) : _ptr_t2<TYPE>( p )
	{
	}
	_array_t ( int nCount = 0 ) : _ptr_t2<TYPE>( nCount )
	{
	}

	void set_size(int n)
	{
		if (n > nsize)
		{
			TYPE *p = new[TYPE];
			memcpy(p, ptr, nsize*sizeof(TYPE));
			delete ptr;
			ptr = p;
			nsize = n;
		}
	}

};














#endif