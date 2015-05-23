#pragma once
///////////////////////////////////////////////////////////////////////////////
// This file is part of AWIN class library
// Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED_T("AS-IS") AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef __message_filter_h__
#define __message_filter_h__

#include "type_defs.h"

class message_filter 
{
public:
	message_filter();
	virtual ~message_filter(){}

	virtual LRESULT	handle_message( UINT m, WPARAM w, LPARAM l );
};

inline message_filter::message_filter()
{
}

inline LRESULT message_filter::handle_message(UINT m, WPARAM w, LPARAM l)
{
	return 0;
}



#endif //__message_filter_h__