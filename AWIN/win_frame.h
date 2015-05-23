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

#ifndef __frame_impl_h__
#define __frame_impl_h__

#include "win_base.h"

class frame_impl : public win_impl
{
public:
	frame_impl();
public:
	frame_impl	&set_mainwnd( bool bset ){ if(bset)m_flags_frame &= (1<<0);else m_flags_frame &= ~(1<<0);return *this;}
	bool	is_mainwnd() const	{return (m_flags_frame & (1<<0)) != 0;}
public:

	virtual long on_destroy();
protected:
	DWORD	m_flags_frame;
};

inline frame_impl::frame_impl()
{
	m_flags_frame = 0;
}

inline long	frame_impl::on_destroy()
{
	PostQuitMessage( 0 );
	return win_impl::on_destroy();
}


#endif //__frame_impl_h__