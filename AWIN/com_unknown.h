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

#ifndef __com_unknown_h__
#define __com_unknown_h__

#include "unknwn.h"


#define com_call virtual HRESULT __stdcall
#define com_ref virtual ULONG __stdcall

#define implement_unknown()\
	com_call QueryInterface( const IID &iid, void **ppv ){return _qi( iid, ppv );}\
    com_ref AddRef( void ){return _addref();}\
    com_ref Release( void ){return _release();}

class com_unknown : public IUnknown
{
public:
	com_unknown();
	virtual ~com_unknown()				{};

	virtual void on_finalrelease()		{};
	virtual IUnknown	*unknown()		{return this;}
	virtual IUnknown *get_interface( const IID &iid );
protected:
    com_call _qi( const IID &iid, void **ppv );
    com_ref _addref( void );
    com_ref _release( void );
protected:
	long	m_nRefCounter;
};

inline com_unknown::com_unknown()
{
	m_nRefCounter = 1;
}
inline IUnknown *com_unknown::get_interface( const IID &iid )
{
	if( iid == IID_IUnknown )return (IUnknown*)this;
	else return 0;
}
inline HRESULT com_unknown::_qi( const IID &iid, void **ppv )
{
	if( IUnknown *p = get_interface( iid ) )
	{
		(*ppv) = p;p->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

inline ULONG com_unknown::_addref( void )
{
	m_nRefCounter++;
	return m_nRefCounter;
}

inline ULONG com_unknown::_release( void )
{
	m_nRefCounter--;
	if( m_nRefCounter )return m_nRefCounter;
	on_finalrelease();
	delete this;
	return 0;
}

#endif //__com_unknown_h__