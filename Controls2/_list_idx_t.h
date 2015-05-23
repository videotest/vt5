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
	TPOS	operator[](long lIdx)
	{
		TPOS lPos = 0; long lCurIdx = 0;
		for( lPos = head( ); lPos; lPos = next( lPos ) )
		{
			if( lCurIdx == lIdx )
				break;
			lCurIdx++;
		}
		return lPos;
  }
};
