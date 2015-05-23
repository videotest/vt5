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

#ifndef __render_window_h__
#define __render_window_h__

#include "render.h"

class render_window : public render
{
public:
	render_window( HWND hwnd )
	{
		m_hwnd = hwnd;
	}

	virtual void layout( const RECT &rect )
	{
		render::layout( rect );

		if( m_hwnd )
			::MoveWindow( m_hwnd, m_rect.left, m_rect.top, m_rect.width(), m_rect.height(), false );
	}
	virtual render_type	type()
	{
		return type_render_window;
	}
protected:
	HWND	m_hwnd;
};

#endif //__render_window_h__