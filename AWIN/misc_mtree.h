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

#ifndef __misc_mtree_h__
#define __misc_mtree_h__

#include "misc_list.h"
#include "misc_notify.h"

enum
{
	mtn_ItemChanged,		//pos_item
	mtn_ItemRemoved,		//pos_item
	mtn_ItemRemovedLink,	//pos_item, pos_parent
	mtn_ItemAdded,			//pos_item, pos_parent
	mtn_ResetAll,
	mtn_Empty,
	mtn_ItemChildChanged,	//pos_item, pos_parent
};

template<class data_type, void (*free_func)(void*) = 0>
class _multylink_tree_t: public notifier
{
	class multylink_item
	{
	public:
		_list_t<multylink_item*>	parents;
		_list_t<multylink_item*>	childs;
		data_type					data;
	};
public:
	_multylink_tree_t()
	{
		m_root.data = 0;
	}
	virtual ~_multylink_tree_t()
	{
		deinit();
	}


	long root()												{	return 0;	}
	long first_parent_pos( long lpos_item )					{	multylink_item *p = _item_from_pos( lpos_item );return p->parents.head();}
	long next_parent( long lpos_item, long lpos )			{	multylink_item *p = _item_from_pos( lpos_item );return p->parents.next( lpos );}
	long first_child_pos( long lpos_item )					{	multylink_item *p = _item_from_pos( lpos_item );return p->childs.head();}
	long next_child( long lpos_item, long lpos )			{	multylink_item *p = _item_from_pos( lpos_item );return p->childs.next( lpos );}
	data_type	get( long lpos_item )						{	multylink_item *p = _item_from_pos( lpos_item );return p->data;}
	long parents_count( long lpos_item )					{	multylink_item *p = _item_from_pos( lpos_item );return p->parents.size();}
	long childs_count( long lpos_item )						{	multylink_item *p = _item_from_pos( lpos_item );return p->childs.size();}

	virtual bool can_insert( data_type data, long lpos_parent, long lpos_before )		{return true;}
	virtual bool can_reference( long lpos_item, long lpos_parent, long lpos_before )	{return !is_parent( lpos_parent, lpos_item );}
	virtual bool can_remove_reference( long lpos_item, long lpos_parent )				{return lpos_item != root();}
	virtual bool can_remove( long lpos_item )											{return lpos_item != root();}

	virtual void deinit()
	{
		notify( mtn_Empty, 0, 0 );
		_remove_item_childs( &m_root );
		notify( mtn_ResetAll, 0, 0 );
	}

	virtual void on_unlock_notify()	{notify( mtn_ResetAll, 0, 0 );};

	virtual long insert( data_type data, long lpos_parent = 0, long lpos_before = 0 )
	{
		if( !can_insert( data, lpos_parent, lpos_before  ) )
			return 0;

		multylink_item *parent = _item_from_pos( lpos_parent );
		multylink_item *pnew = new multylink_item;

		pnew->data = data;
		pnew->parents.insert_before( parent, 0 );
		
		long	lpos_item = parent->childs.insert_before( pnew, lpos_before );;

		long	notify_params[2]={lpos_item, lpos_parent};
		notify( mtn_ItemAdded, notify_params, 0 );

		return lpos_item;
	}

	virtual long reference( long lpos_item, long lpos_parent, long lpos_before = 0 )
	{
		if( !can_reference( lpos_item, lpos_parent, lpos_before ) )
			return 0;

		multylink_item	*pitem = _item_from_pos( lpos_item );
		multylink_item	*pparent = _item_from_pos( lpos_parent );
		lpos_item = pparent->childs.insert_before( pitem, lpos_before );
		pitem->parents.insert_before( pparent, 0 );

		long	notify_params[2]={lpos_item, lpos_parent};
		notify( mtn_ItemAdded, notify_params, 0 );

		return lpos_item;
	}
	virtual bool remove_reference( long lpos_item, long lpos_parent )
	{
		if( !can_remove_reference( lpos_item, lpos_parent ) )
			return false;

		long	notify_params[2]={lpos_item, lpos_parent};
		notify( mtn_ItemRemovedLink, notify_params, 0 );

		multylink_item	*pitem = _item_from_pos( lpos_item );
		multylink_item	*pparent = _item_from_pos( lpos_parent );

		long	lpos_inparent = pitem->parents.find( pparent );
		pparent->childs.remove( lpos_item );
		pitem->parents.remove( lpos_inparent );

		if( pitem->parents.head() == 0 )
			_free_item( pitem );

		return true;
	}
	virtual bool remove( long lpos_item )
	{
		if( !can_remove( lpos_item ) )
			return false;

		notify( mtn_ItemRemoved, &lpos_item, 0 );

		multylink_item	*pitem = _item_from_pos( lpos_item );

		long lpos1 = pitem->parents.head();
		while( lpos1 )
		{
			multylink_item	*pparent = _item_from_pos( lpos1 );
			lpos1 = pitem->parents.next( lpos1 );
			long	lpos2 = pparent->childs.find( pitem );
			pparent->childs.remove( lpos2 );
		}
		_free_item( pitem );

		return true;
	}

	bool	is_parent( long lpos_item, long lpos_parent )
	{
		multylink_item	*pitem = _item_from_pos( lpos_item );
		multylink_item	*pparent = _item_from_pos( lpos_parent );

		return _is_parent( pitem, pparent );
	}

protected:
	multylink_item	*_item_from_pos( long lpos )
	{
		if( lpos == 0 )return &m_root;
		return _list_t<multylink_item*>::get( lpos );
	}
	bool _is_parent( multylink_item *pi, multylink_item *pp )
	{
		if( pp == pi )return true;

		long	lpos = pp->childs.head();
		while( lpos )
		{
			multylink_item *ptest = _item_from_pos( lpos );
			if( _is_parent( pi, ptest ) )return true;
			lpos = pp->childs.next( lpos );
		}
		return false;
	}
	void _remove_item_childs( multylink_item *p )
	{
		while( long lpos1 = p->childs.head() )
		{
			multylink_item *p_c = _item_from_pos( lpos1 );
			p->childs.remove( lpos1 );

			long	lpos_parent = p_c->parents.find( p );
			p_c->parents.remove( lpos_parent );

			if( p_c->parents.head() == 0 )
				_free_item( p_c );
		}
	}
	void _free_item( multylink_item *p )
	{
		_remove_item_childs( p );
		if( free_func )free_func( p->data );
		delete p;
	}
	
	
protected:
	multylink_item	m_root;
};

#endif //__misc_mtree_h__