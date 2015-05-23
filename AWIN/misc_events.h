///////////////////////////////////////////////////////////////////////////////
//This file is part of AWIN class library
//Copyright (c) 2001-2002 Andy Yamov
//
// Permission to use, copy, modify, distribute, and sell this software and
// its documentation for any purpose is hereby granted without fee
// 
// THE SOFTWARE IS PROVIDED_T("AS-IS") AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//

#ifndef __events_h__
#define __events_h__


/*
class event_listener
{
};

typedef long	event_id;
typedef long (event_listener::*event_handler)( event_id event, ... );

#pragma warning(disable:4786)
#include "list"
#include "map"

class event_source_impl
{
	struct handler_info
	{
		event_listener	*plistener;
		event_handler	phandler;
	};
	struct handlers : public std::list<handler_info>
	{
		event_id	event;
	};
	typedef std::map<event_id, handlers>handlers_map;
public:
	void register_handler( event_id event, event_listener *plistener, event_handler handler)
	{
		handler_info		h;
		h.phandler = handler;
		h.plistener = plistener;

		handlers	&hs = m_map[event];
		hs.event = event;
		hs.insert( hs.end(), h );
	}
	void unregister_handler( event_id event, event_listener *plistener, event_handler handler)
	{
		handlers_map::iterator	r = m_map.find( event );

		if( r != m_map.end() )
		{
			handlers	hs = (*r).second;
			handlers::iterator	hi;

			for( hi = hs.begin(); hi != hs.end(); hi++ )
			{
				handler_info	h = *hi;
				if( h.plistener == plistener )
				{
					hs.erase( hi );
					break;
				}
			}
		}
	}	
	void fire( event_listener *psource, event_id event, int sizeof_args = 0, ... )
	{
		long	args_offset = (long)&sizeof_args+sizeof_args*sizeof( long );
		handlers_map::iterator	r = m_map.find( event );

		if( r != m_map.end() )
		{
			handlers	hs = (*r).second;
			handlers::iterator	hi;

			for( hi = hs.begin(); hi != hs.end(); hi++ )
			{
				//fire event
				handler_info	h = *hi;
				if( h.plistener != psource )
				{
					__asm
					{
						//store old ecx
						push	ecx;
						//loop - move args to stack again
						mov		ecx, sizeof_args;
						mov		eax, args_offset;
						cmp		ecx, 0
start_loop:
						
						je		end_loop
						push	DWORD	PTR[eax];
						sub		eax, 4
						dec		ecx
						jmp		start_loop
end_loop:
						//push def parameters
						mov		eax, event;
						push	eax;
						mov		ecx, DWORD PTR h.plistener;

						//call method
						call	h.phandler;

						//restore
						pop		ecx;
					}
					
				}
			}

		}
	}

	handlers_map	m_map;
};



//sample event listener class
class my_listener : public event_listener
{
public:
	my_listener()
	{identy = 777;}
	long sample_event_handler( event_id event )
	{
		_char	sz[100];
		_sprintf( sz, _T("Event Listener %p(%d), Event %d"), this, identy, event );
		MessageBox( 0, sz, _T("sample_event_handler"), MB_OK );
		return 0;
	}
	long sample_event_handler1( event_id event, long lParam, byte byte1, const _char *psz, double f )
	{
		_char	sz[200];
		_sprintf( sz, _T("Event Listener Param %p(%d), Event %d(%d), string = %s, byte = %d, double = %f"), 
			this, identy, event, lParam, psz, byte1, f );
		MessageBox( 0, sz, _T("sample_event_handler"), MB_OK );
		return 0;
	}
	long	identy;
};
//sample ussage 

	event_source	source;
	my_listener		listener;
	event_id		id_sample=101;
	event_id		id_sample_params=102;

	source.register_handler( id_sample, &listener, (event_handler)my_listener::sample_event_handler );
	source.register_handler( id_sample_params, &listener, (event_handler)my_listener::sample_event_handler1 );
	source.fire( 0, id_sample_params, 5, 1025, 128, _T("qqqqq"), 0.123 );
	source.fire( 0, id_sample  );

*/


#endif //__events_h__