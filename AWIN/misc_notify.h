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

#ifndef __misc_notify_h__
#define __misc_notify_h__

#include "misc_list.h"
class event_listener
{
friend class notifier;
protected:
	virtual void handle_notify( int code, void *pdata )	{}
};


class locker
{
public:
	locker( long *p, long old )
	{
		plong = p;
		old_lock = old;
		*plong = true;
	}
	~locker()
	{
		*plong = old_lock;
	}
protected:
	long	old_lock;
	long	*plong;
};


class notifier
{
public:
	notifier()
	{		lock = false;	}

	TPOS attach_listener( event_listener *p )
	{			return listeners.insert_before( p, 0 );		}
	void detach_listener(event_listener *p, TPOS lpos = 0)
	{			if( !lpos )lpos = listeners.find( p );listeners.remove( lpos );	}
	virtual void on_unlock_notify()	{};
	void notify( int code, void *pdata, event_listener *psource = 0 )
	{
		if( lock )return;

		TPOS	lpos = listeners.head();

		while( lpos )
		{
			event_listener	*p = listeners.get( lpos );
			if( p != psource )
				p->handle_notify( code, pdata );
			lpos = listeners.next( lpos );
		}
	}
public:
	long	lock;
protected:
	_list_t<event_listener*> listeners;
	
};


#endif //__misc_notify_h__