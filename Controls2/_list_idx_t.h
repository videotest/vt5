#pragma once
#include "misc_list.h"


template<class data_type, void *ptr_free = 0>
class _list_idx_t :
	public _list_t<data_type, ptr_free>
{
public:

	_list_idx_t(void)
	{
	}

	virtual ~_list_idx_t(void)
	{
	}

	// get position by index
	long	operator[]( long lIdx )
	{
		long lPos = 0, lCurIdx = 0;
		for( lPos = head( ); lPos; lPos = next( lPos ) )
		{
			if( lCurIdx == lIdx )
				break;
			lCurIdx++;
		}
		return lPos;
    }
};
