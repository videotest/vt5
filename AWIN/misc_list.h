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

#ifndef __awin_misc_list_h__
#define __awin_misc_list_h__

#pragma warning(disable:4786)


template<class data_type, void *ptr_free = 0>
class _list_t
{
public:
	_list_t()
	{
		m_phead = m_ptail = 0;
		m_count = 0;
	}
	virtual ~_list_t()
	{
		clear();
	}

	class list_item
	{
	public:
		data_type	data;
		list_item	*prev;
		list_item	*next;
	};


	long head()							{return (long)m_phead;};
	long tail()							{return (long)m_ptail;};
	long count()						{return m_count;}
	static data_type	&get( long lpos ){return ((list_item*)lpos)->data;}
	static long next( long lpos )				{return (long)((list_item*)lpos)->next;}
	static long prev( long lpos )				{return (long)((list_item*)lpos)->prev;}
	static void set( data_type	p, long lpos )	{((list_item*)lpos)->data = p;}
	long add_head( data_type p )		{return insert_after( p );}
	long add_tail( data_type p )		{return insert_before( p );}
	long insert_before( data_type p, long lpos = 0 )
	{
		list_item	*pnext = (list_item*)lpos;
		list_item	*pnew = new list_item;
		pnew->data = p;

		pnew->next = pnext;
		if( pnext )pnew->prev = pnext->prev; else pnew->prev = m_ptail;
		if( pnew->prev )pnew->prev->next = pnew;
		if( pnew->next )pnew->next->prev = pnew;

		if( !pnew->prev )m_phead = pnew;
		if( !pnew->next )m_ptail = pnew;

		m_count++;

		return (long)pnew;
	}

	long insert_after( data_type p, long lpos = 0 )
	{
		list_item	*pprev = (list_item*)lpos;
		list_item	*pnew = new list_item;
		pnew->data = p;

		pnew->prev = pprev;
		if( pprev )pnew->next = pprev->next; else pnew->next  = m_phead;
		if( pnew->prev )pnew->prev->next = pnew;
		if( pnew->next )pnew->next->prev = pnew;

		if( !pnew->prev )m_phead = pnew;
		if( !pnew->next )m_ptail = pnew;

		m_count++;

		return (long)pnew;
	}
	void remove( long lpos )
	{
		list_item	*premove = (list_item*)lpos;
		if( premove->next )premove->next->prev = premove->prev;else m_ptail = premove->prev;
		if( premove->prev )premove->prev->next = premove->next;else m_phead = premove->next;
		_free_item( premove->data );
		delete premove;

		m_count--;
	}
	void clear()
	{
		list_item	*p = m_phead;
		while( p )
		{
			m_phead = p->next;
			m_count--;
			_free_item( p->data );
			delete p;
			p = m_phead;
		}
		m_phead = m_ptail = 0;
		m_count = 0;
	}
	long	find( data_type &p )
	{
		list_item	*pitem = m_phead;
		while( pitem )
		{
			if( p == pitem->data )
				return long(pitem);
			pitem = pitem->next;
		}
		return 0;
	}
protected:
	void _free_item( data_type &data )
	{
		if( ptr_free )((void ( *)(data_type))ptr_free)( data );
	}
protected:
	list_item	*m_phead;
	list_item	*m_ptail;
	long		m_count;
};


#endif // __awin_misc_list_h__