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

#ifndef __misc_map_h__
#define __misc_map_h__

#include "misc_dbg.h"

#pragma warning(disable:4786)
typedef POSITION TPOS;

template <typename T> long cmp(T a, T b)
{
	return b>a?1:(a>b?-1:0);
}

inline long cmp_long( long l1, long l2 )
{	if( l1 == l2 )return 0;else return (l2>l1)?1:-1;};
inline long cmp_unsigned( unsigned l1, unsigned l2 )
{	if( l1 == l2 )return 0;else return (l2>l1)?1:-1;};
inline long cmp_string( const _char *psz1, const _char *psz2 )
{	return _tcscmp( psz1, psz2 );};
inline long cmp_string_nocase( const _char *psz1, const _char *psz2 )
{	return _tcsicmp( psz1, psz2 );};

inline long cmp_double( double f1, double f2 )
{	
	if( f1-f2 > 0 )return 1;
	if( f1-f2 < 0 )return -1;
	return 0;
};

#define MAX_DEEP	100


template <class data_type, class key_type, long (*ptr_compare)(key_type, key_type), void *ptr_free = 0>
class _map_t
{
public:
#pragma pack(push)
#pragma pack(1)
	struct item
	{
		item	*left;
		item	*right;
		signed char	factor;
		data_type	data;
		key_type	key;

		item	*&child( int dir )
		{	
			_assert( dir != 0 );
			return ( dir > 0 )?right:left;
		}
	};
#pragma pack(pop)

	_map_t()
	{	m_ptr = 0;m_count = 0;	}
	virtual ~_map_t()
	{	clear();}
/*
	void		remove( key_type key );
	long head();
	long next( long lpos );
	long tail();
	long prev( long lpos );
	data_type	get( long lpos );
	key_type	get_key( long lpos );
	long		set( key_type key, data_type data );
	long		find( key_type key );

	void clear();
	long count();
*/
	long count()	{return m_count;};
protected:
	signed char _cmp( key_type k1, key_type k2 )
	{
		long (*pfn)(key_type k1, key_type k2) = (long (*)(key_type, key_type))ptr_compare;
		int	cmp = ptr_compare( k1, k2 );
		
		if( cmp > 0 )return 1;
		else if( cmp < 0 )return -1;
		else return 0;

	}
	void _free( item *pitem )
	{
		void (*pfn)(data_type data)=(void (*)(data_type))ptr_free;
		if( pfn )pfn( pitem->data );
	}
public:
//установить значение для ключа
	TPOS set(data_type data, key_type key)
	{
		signed char	pdirs[MAX_DEEP], cmp=0;
		long	ndeep = 0;
//ищем ветку куда будем складывать результат
//заодно ищем ветку которую потенциально может быть лучше баллансировать
//это - последняя ветка с фактором != 0 
//(фактор не может быть больше 1, так как это уже отбаллансировалось
//бы при предъидущих вызвовах, а 0 - не баллансируется а тольео меняется фактор)
		item	*pitem, *pitem_parent, *pitem_ret;
		item	*pballance, *pballance_parent;
		
		for( pitem = m_ptr, 
			 pitem_parent = 0,
			 pballance = m_ptr, 
			 pballance_parent = 0; 
			 pitem!=0; 
			 pitem = pitem->child( cmp ) )
		{
		//нашли - уже есть в дереве. заменяем.
			cmp = _cmp( pitem->key, key );
			if( !cmp )break;
		//рассматриваем этот элемент как кандидат для баллансировки
			if( pitem->factor )
				{
				pballance = pitem;
				pballance_parent = pitem_parent;
				ndeep = 0;
			}
		//идем дальше
			pdirs[ndeep++] = cmp;			
			pitem_parent = pitem;
		}

		//так и не нашли - размезаем новый элемент
		if( !pitem )
		{
			pitem = new item;
			pitem->left = pitem->right = 0;
			pitem->factor = 0;

			if( !pitem_parent )m_ptr = pitem;
			else pitem_parent->child( cmp ) = pitem;
			m_count++;
		}
		else
			_free( pitem );

		pitem->key = key;
		pitem->data = data;

		pitem_ret = pitem;
		//если мы заместили уже существующий элемент, то ничего больше делеть
		//не надо. иначе - попробуем сбалланисироваться.
		if( cmp )
		{
			//есть кандидат на баллансировку
			if( pballance )
			{
				item	*pnew = 0;
				//сместили балланс у всех промежуточных  элементов
				int	idx = 0;
				for( pitem = pballance; pitem != pitem_ret; pitem = pitem->child(pdirs[idx]), idx++ )
					pitem->factor += pdirs[idx];

				//баллансируем
				if( pballance->factor == -2 )
				{
					pitem = pballance->left;
				
					//поворот направо
					if( pitem->factor == -1 )
					{
						pnew = pitem;
						
						pballance->left = pitem->right;
						pitem->right = pballance;
						pitem->factor = 0;
						pballance->factor = 0;
					}
					else 
					{
						_assert( pitem->factor == +1 );
						//поворот налево - указатель не может быть 0 так как фактор
						//перекошен в эту сторону
						pnew = pitem->right;
						pitem->right = pnew->left;
						pballance->left = pnew->right;
						pnew->left = pitem;
						pnew->right = pballance;
						if( pnew->factor == 1 )
							pballance->factor = 0, pitem->factor = -1;
						else if( pnew->factor == 0 )
							pballance->factor = pitem->factor = 0;
						else //if( pnew->ballance== -1 )
							pballance->factor = 1, pitem->factor = 0;
						pnew->factor = 0;
					}
				}
				else if( pballance->factor == 2 )
				{
					pitem = pballance->right;
				
					//поворот направо
					if( pitem->factor == +1 )
					{
						pnew = pitem;
						
						pballance->right = pitem->left;
						pitem->left = pballance;
						pitem->factor = 0;
						pballance->factor = 0;
					}
					else 
					{
						_assert( pitem->factor == -1 );
						//поворот налево - указатель не может быть 0 так как фактор
						//перекошен в эту сторону
						pnew = pitem->left;
						pitem->left = pnew->right;
						pballance->right = pnew->left;
						pnew->right = pitem;
						pnew->left = pballance;
						if( pnew->factor == -1 )
							pballance->factor = 0, pitem->factor = 1;
						else if( pnew->factor == 0 )
							pballance->factor = pitem->factor = 0;
						else //if( pnew->factor== +1 )
							pballance->factor = -1, pitem->factor = 0;
						pnew->factor = 0;
					}
				}

				//обновляем указатель родителя балансируемого элемента на 
				//новый элемент, только если производили баллансировку
				if( pnew != 0 )
				{
					if( pballance_parent )
					{
						if( pballance_parent->left == pballance )
							pballance_parent->left = pnew;
						else 
							pballance_parent->right = pnew;
					}
					else
						m_ptr = pnew;
				}
			}
		}
	
		return (TPOS)pitem_ret;
	}

	LONG_PTR dbg_check(item *p)
	{
		if( !p )return 0;

		if( p->parent )
		{
			int	cmp = _cmp( p->parent->key, p->key );
			_assert( cmp != 0 );
			if( cmp>0 )_assert( p->parent->right == p );
			else _assert( p->parent->left == p );
		}
		else
			_assert( p == m_ptr );

		LONG_PTR	right_h = _check(p->right);
		TPOS	left_h = _check(p->left);

		TPOS	factor = right_h - left_h;
		_assert( factor == p->factor );

		if( right_h > left_h )return right_h+1;
		else return left_h+1;
	}
	bool remove_key( key_type key )
	{
		signed char	pdirs[MAX_DEEP], cmp;
		item	*ppitems[MAX_DEEP];
		long	ndeep = 0;

	//найдем item, который соответствует key - попутно сохраним путь до него
		for( item	*pitem = m_ptr; pitem; )
		{
			cmp = _cmp( pitem->key, key );
			
			if( !cmp )break;
			pdirs[ndeep] = cmp;
			ppitems[ndeep] = pitem;
			ndeep++;
			pitem = pitem->child( cmp );
		}
	//ничего не нашли - неудачно
		if( !pitem )return false;

		item	*pparent = ndeep?ppitems[ndeep-1]:0;
	//у удаляемого итема нет правого поддерева
		if( !pitem->right )
		{
			if( pparent )pparent->child( pdirs[ndeep-1] ) = pitem->left;
			else m_ptr = pitem->left;
		}
		else
		{
			item	*pright = pitem->right;
		//по сути - ищем первый справа итем, у которого нет левого поддерева
		//и заменяем на него удаляемый item
			if( pright->left == 0 )
			{
				pright->left = pitem->left;
				pright->factor = pitem->factor;


				if( pparent )pparent->child( pdirs[ndeep-1] ) = pright;
				else m_ptr = pright;

				pdirs[ndeep] = 1;
				ppitems[ndeep] = pright;
				ndeep++;
			}
			else
			{
				item	*prepl = pright;
				item	*prepl_parent = pitem;

				int		cur_deep = ndeep;
				ndeep++;

				while( prepl->left )
				{
					pdirs[ndeep] = -1;
					ppitems[ndeep] = prepl;
					ndeep++;

					prepl_parent = prepl;
					prepl = prepl->left;
				}

				prepl->left = pitem->left;
				prepl_parent->left = prepl->right;
				prepl->right = pitem->right;
				prepl->factor = pitem->factor;

			//у нас один неправильный итем в списке
				ppitems[cur_deep] = prepl;
				pdirs[cur_deep] = 1;

			//обновим parent
				if( pparent )pparent->child( pdirs[cur_deep-1] ) = prepl;
				else m_ptr = prepl;
			}
		}
	
		_free( pitem );
		delete pitem;
		m_count--;

		//теперь - баллансировка

		if( ndeep )
		{
			ndeep--;
			while( ndeep>-1 )
			{
				item	*pitem_y = ppitems[ndeep];
				pitem_y->factor -= pdirs[ndeep];

				if( pitem_y->factor == 2 )
				{
					item	*pitem_x = pitem_y->right;

					//двойной поворот
					if( pitem_x->factor == -1 )
					{
						item	*pitem_w = pitem_x->left;
						pitem_x->left = pitem_w->right;
						pitem_w->right = pitem_x;
						pitem_y->right = pitem_w->left;
						pitem_w->left = pitem_y;

						if( pitem_w->factor == 1 )
							pitem_x->factor = 0, pitem_y->factor = -1;
						else if( pitem_w->factor == -1 )
							pitem_x->factor = 1, pitem_y->factor = 0;
						else
							pitem_x->factor = 0, pitem_y->factor = 0;
						
						pitem_w->factor = 0;

						if( !ndeep )m_ptr = pitem_w;
						else ppitems[ndeep-1]->child( pdirs[ndeep-1] ) = pitem_w;
					}
					else //if( pitem_x->factor == 1 )
					{
						pitem_y->right = pitem_x->left;
						pitem_x->left = pitem_y;
						
						if( !ndeep )m_ptr = pitem_x;
						else ppitems[ndeep-1]->child( pdirs[ndeep-1] ) = pitem_x;

						if( pitem_x->factor == 0 )
						{
							pitem_x->factor = -1; 
							pitem_y->factor = 1;
							break;
						}
						else
							pitem_x->factor = pitem_y->factor = 0;
					}
				}
				else if( pitem_y->factor == -2 )
				{
					item	*pitem_x = pitem_y->left;

					//двойной поворот
					if( pitem_x->factor == 1 )
					{
						item	*pitem_w = pitem_x->right;
						pitem_x->right = pitem_w->left;
						pitem_w->left = pitem_x;
						pitem_y->left = pitem_w->right;
						pitem_w->right = pitem_y;

						if( pitem_w->factor == -1 )
							pitem_x->factor = 0, pitem_y->factor = +1;
						else if( pitem_w->factor == +1 )
							pitem_x->factor = -1, pitem_y->factor = 0;
						else
							pitem_x->factor = 0, pitem_y->factor = 0;

						pitem_w->factor = 0;

						if( !ndeep )m_ptr = pitem_w;
						else ppitems[ndeep-1]->child( pdirs[ndeep-1] ) = pitem_w;
					}
					else //if( pitem_x->factor == -1 )
					{
						pitem_y->left = pitem_x->right;
						pitem_x->right = pitem_y;
						
						if( !ndeep )m_ptr = pitem_x;
						else ppitems[ndeep-1]->child( pdirs[ndeep-1] ) = pitem_x;

						if( pitem_x->factor == 0 )
						{
							pitem_x->factor = 1;
							pitem_y->factor = -1;
							break;
						}
						else
							pitem_x->factor = pitem_y->factor = 0;
					}
				}
				else if( pitem_y->factor == 1||pitem_y->factor == -1 )
					break;
				ndeep--;
			}
		}

		return true;
	}

//найти позицию по ключу. если 0 то не нашли
	TPOS	find( key_type key )
	{
		char	c;
		for( item	*p = m_ptr; p!=0;  )
		{
			c = _cmp( p->key, key );
			if (!c)return (TPOS)p;

			if( c > 0 )p = p->right;
			else p = p->left;
		}
		return 0;
	}
//убить все из карты
	void clear()
	{
		if( !m_ptr )return;
		
		item	*ppitems[MAX_DEEP];
		long	ndeep = 0;

		item	*p = m_ptr;
		while( true )
		{
			if( p->left )
			{
				ppitems[ndeep++] = p;
				p = p->left;
			}
			//мы можем сходить направо - идем туда
			else if( p->right )
			{
				ppitems[ndeep++] = p;
				p = p->right;
			}
			//мы направо уже сходили, смотрим где
			else if( !ndeep )
			{
				_free( p );
				delete p;
				break;
			}
			//мы возвращаемся слева - надо сходить направо
			else if( p == ppitems[ndeep-1]->left )
			{
				ndeep--;
				p = ppitems[ndeep];
				_free( p->left );
				delete p->left;
				p->left = 0;
			}
			//иначе - мы возвращаемся справа - сходить налево
			else if( p == ppitems[ndeep-1]->right )
			{
				ndeep--;
				p = ppitems[ndeep];
				_free( p->right );
				delete p->right;
				p->right = 0;
			}
		}
		m_ptr = 0;
		m_count = 0;
	}

	//вернуть - таки значение по позиции
	data_type	get(TPOS lpos)
	{
		item	*p = (item*)lpos;
		return p->data;
	}

	//вернуть - ключ по позиции
	key_type	get_key(TPOS lpos)
	{
		item	*p = (item*)lpos;
		return p->key;
	}

	//отладочные функции
	TPOS root()					{ return (TPOS)m_ptr; }
	TPOS left(LONG_PTR lpos)		{ item *p = (item*)lpos; return (TPOS)p->left; }
	TPOS right(LONG_PTR lpos)		{ item *p = (item*)lpos; return (TPOS)p->right; }


protected:
	item	*m_ptr;
	long	m_count;
};

template <class data_type, class key_type, long (*ptr_compare)(key_type, key_type), void *ptr_free = 0>
class _list_map_t
{
public:
#pragma pack(push)
#pragma pack(1)
	struct item
	{
		item	*left;
		item	*right;
		item	*parent;
		signed char	factor;
		data_type	data;
		key_type	key;

		item	*&child( int dir )
		{	
			_assert( dir != 0 );
			return ( dir > 0 )?right:left;
		}
	};
#pragma pack(pop)

	_list_map_t()
	{	m_ptr = 0;m_count = 0;	}
	virtual ~_list_map_t()
	{	clear();}
/*
	void		remove( key_type key );
	long head();
	long next( long lpos );
	long tail();
	long prev( long lpos );
	data_type	get( long lpos );
	key_type	get_key( long lpos );
	long		set( key_type key, data_type data );
	long		find( key_type key );

	void clear();
	long count();
*/
	long count()	{return m_count;};
protected:
	signed char _cmp( key_type k1, key_type k2 )
	{
		long (*pfn)(key_type k1, key_type k2) = (long (*)(key_type, key_type))ptr_compare;
		int	cmp = ptr_compare( k1, k2 );
		
		if( cmp > 0 )return 1;
		else if( cmp < 0 )return -1;
		else return 0;

	}
	void _free( item *pitem )
	{
		void (*pfn)(data_type data)=(void (*)(data_type))ptr_free;
		if( pfn )pfn( pitem->data );
	}
public:
//установить значение для ключа
	TPOS set(data_type data, key_type key)
	{
		signed char	pdirs[MAX_DEEP], cmp=0;
		long	ndeep = 0;
//ищем ветку куда будем складывать результат
//заожно ищем ветку которую потенциально может быть лучше баллансировать
//это - последняя ветка с фактором != 0 
//(фактор не может быть больше 1, так как это уже отбаллансировалось
//бы при предъидущих вызвовах, а 0 - не баллансируется а тольео меняется фактор)
		item	*pitem, *pitem_parent, *pitem_ret;
		item	*pballance, *pballance_parent;
		
		for( pitem = m_ptr, 
			 pitem_parent = 0,
			 pballance = m_ptr, 
			 pballance_parent = 0; 
			 pitem!=0; 
			 pitem = pitem->child( cmp ) )
		{
		//нашли - уже есть в дереве. заменяем.
			cmp = _cmp( pitem->key, key );
			if( !cmp )break;
		//рассматриваем этот элемент как кандидат для баллансировки
			if( pitem->factor )
			{
				pballance = pitem;
				pballance_parent = pitem_parent;
				ndeep = 0;
			}
		//идем дальше
			pdirs[ndeep++] = cmp;			
			pitem_parent = pitem;
		}

		//так и не нашли - размезаем новый элемент
		if( !pitem )
		{
			pitem = new item;
			pitem->parent = pitem_parent;
			pitem->left = pitem->right = 0;
			pitem->factor = 0;

			if( !pitem_parent )m_ptr = pitem;
			else pitem_parent->child( cmp ) = pitem;
			m_count++;
		}
		else
			_free( pitem );

		pitem->key = key;
		pitem->data = data;

		pitem_ret = pitem;
		//если мы заместили уже существующий элемент, то ничего больше делеть
		//не надо. иначе - попробуем сбалланисироваться.
		if( cmp )
		{
			//есть кандидат на баллансировку
			if( pballance )
			{
				item	*pnew = 0;
				//сместили балланс у всех промежуточных  элементов
				int	idx = 0;
				for( pitem = pballance; pitem != pitem_ret; pitem = pitem->child(pdirs[idx]), idx++ )
					pitem->factor += pdirs[idx];

				//баллансируем
				if( pballance->factor == -2 )
				{
					pitem = pballance->left;
				
					//поворот направо
					if( pitem->factor == -1 )
					{
						pnew = pitem;
						
						pballance->left = pitem->right;	if( pballance->left )pballance->left->parent = pballance;
						pitem->right = pballance;		pballance->parent = pitem;
						pitem->factor = 0;
						pballance->factor = 0;
					}
					else 
					{
						_assert( pitem->factor == +1 );
						//поворот налево - указатель не может быть 0 так как фактор
						//перекошен в эту сторону
						pnew = pitem->right;
						pitem->right = pnew->left;		if( pitem->right )pitem->right->parent = pitem;
						pballance->left = pnew->right;	if( pballance->left )pballance->left->parent = pballance;
						pnew->left = pitem;				pitem->parent = pnew;
						pnew->right = pballance;		pballance->parent = pnew;
						if( pnew->factor == 1 )
							pballance->factor = 0, pitem->factor = -1;
						else if( pnew->factor == 0 )
							pballance->factor = pitem->factor = 0;
						else //if( pnew->ballance== -1 )
							pballance->factor = 1, pitem->factor = 0;
						pnew->factor = 0;
					}
				}
				else if( pballance->factor == 2 )
				{
					pitem = pballance->right;
				
					//поворот направо
					if( pitem->factor == +1 )
					{
						pnew = pitem;
						
						pballance->right = pitem->left;	if( pballance->right )pballance->right->parent = pballance;
						pitem->left = pballance;		pballance->parent = pitem;
						pitem->factor = 0;
						pballance->factor = 0;
					}
					else 
					{
						_assert( pitem->factor == -1 );
						//поворот налево - указатель не может быть 0 так как фактор
						//перекошен в эту сторону
						pnew = pitem->left;
						pitem->left = pnew->right;		if( pitem->left )pitem->left->parent = pitem;
						pballance->right = pnew->left;	if( pballance->right )pballance->right->parent = pballance;
						pnew->right = pitem;			pitem->parent = pnew;
						pnew->left = pballance;			pballance->parent = pnew;
						if( pnew->factor == -1 )
							pballance->factor = 0, pitem->factor = 1;
						else if( pnew->factor == 0 )
							pballance->factor = pitem->factor = 0;
						else //if( pnew->factor== +1 )
							pballance->factor = -1, pitem->factor = 0;
						pnew->factor = 0;
					}
				}

				//обновляем указатель родителя балансируемого элемента на 
				//новый элемент, только если производили баллансировку
				if( pnew != 0 )
				{
					if( pballance_parent )
					{
						if( pballance_parent->left == pballance )
							pballance_parent->left = pnew;
						else 
							pballance_parent->right = pnew;
					}
					else
						m_ptr = pnew;
					pnew->parent = pballance_parent;
				}
			}
		}
	
		return (TPOS)pitem_ret;
	}

	LONG_PTR dbg_check(item *p)
	{
		if( !p )return 0;

		if( p->parent )
		{
			int	cmp = _cmp( p->parent->key, p->key );
			_assert( cmp != 0 );
			if( cmp>0 )_assert( p->parent->right == p );
			else _assert( p->parent->left == p );
		}
		else
			_assert( p == m_ptr );

		LONG_PTR	right_h = _check(p->right);
		LONG_PTR	left_h = _check(p->left);

		LONG_PTR	factor = right_h - left_h;
		_assert( factor == p->factor );

		if( right_h > left_h )return right_h+1;
		else return left_h+1;
	}
	bool remove(TPOS lpos)
	{
		key_type key = get_key( lpos );
		return remove_key( key );
	}
	bool remove_key( key_type key )
	{
		signed char	pdirs[MAX_DEEP], cmp;
		long	ndeep = 0;
		item	*ptop = 0;

	//найдем item, который соответствует key - попутно сохраним путь до него
		for( item	*pitem = m_ptr; pitem; )
		{
			cmp = _cmp( pitem->key, key );
			
			if( !cmp )break;
			pdirs[ndeep] = cmp;
			ndeep++;
			pitem = pitem->child( cmp );
		}
	//ничего не нашли - неудачно
		if( !pitem )return false;

		item	*pparent = pitem->parent;
	//у удаляемого итема нет правого поддерева
		if( !pitem->right )
		{
			ptop = pitem->parent;
			if( pparent )pparent->child( pdirs[ndeep-1] ) = pitem->left;
			else m_ptr = pitem->left;
		//обновим parent
			if( pitem->left )pitem->left->parent = pparent;
		}
		else
		{
			item	*pright = pitem->right;
		//по сути - ищем первый справа итем, у которого нет левого поддерева
		//и заменяем на него удаляемый item
			if( pright->left == 0 )
			{
				pright->left = pitem->left;			if( pright->left )pright->left->parent = pright;
				pright->factor = pitem->factor;


				if( pparent )pparent->child( pdirs[ndeep-1] ) = pright;
				else m_ptr = pright;
			//обновим parent
				pright->parent = pparent;

				ptop = pright;
				pdirs[ndeep] = 1;
				ndeep++;
			}
			else
			{
				item	*prepl = pright;
				item	*prepl_parent = pitem;

				int		cur_deep = ndeep;
				ndeep++;

				while( prepl->left )
				{
					pdirs[ndeep] = -1;
					ndeep++;

					prepl_parent = prepl;
					prepl = prepl->left;
				}

				ptop = prepl_parent;

				prepl->left = pitem->left;				if( prepl->left )prepl->left->parent = prepl;
				prepl_parent->left = prepl->right;		if( prepl_parent->left )prepl_parent->left->parent = prepl_parent;
				prepl->right = pitem->right;			prepl->right->parent = prepl;
				prepl->factor = pitem->factor;

			//у нас один неправильный итем в списке
				pdirs[cur_deep] = 1;

			//обновим parent
				if( pparent )pparent->child( pdirs[cur_deep-1] ) = prepl;
				else m_ptr = prepl;
				prepl->parent = pparent;
			}
		}
	
		_free( pitem );
		delete pitem;
		m_count--;

		//теперь - баллансировка

		if( ndeep )
		{
			ndeep--;
			while( ndeep>-1 )
			{
				item	*pitem_y = ptop;ptop = ptop->parent;//ppitems[ndeep];
				pitem_y->factor -= pdirs[ndeep];

				if( pitem_y->factor == 2 )
				{
					item	*pitem_x = pitem_y->right;

					//двойной поворот
					if( pitem_x->factor == -1 )
					{
						item	*pitem_w = pitem_x->left;
						pitem_x->left = pitem_w->right;			if( pitem_x->left )pitem_x->left->parent = pitem_x;
						pitem_w->right = pitem_x;				pitem_w->right->parent = pitem_w;
						pitem_y->right = pitem_w->left;			if( pitem_y->right )pitem_y->right->parent = pitem_y;
						pitem_w->left = pitem_y;				pitem_w->left->parent = pitem_w;

						if( pitem_w->factor == 1 )
							pitem_x->factor = 0, pitem_y->factor = -1;
						else if( pitem_w->factor == -1 )
							pitem_x->factor = 1, pitem_y->factor = 0;
						else
							pitem_x->factor = 0, pitem_y->factor = 0;
						
						pitem_w->factor = 0;

						if( !ndeep )m_ptr = pitem_w, pitem_w->parent = 0;
						else ptop->child( pdirs[ndeep-1] ) = pitem_w, pitem_w->parent = ptop;
					}
					else //if( pitem_x->factor == 1 )
					{
						pitem_y->right = pitem_x->left;			if( pitem_y->right)pitem_y->right->parent = pitem_y;
						pitem_x->left = pitem_y;					pitem_x->left->parent = pitem_x;
						
						if( !ndeep )m_ptr = pitem_x, pitem_x->parent = 0;
						else ptop->child( pdirs[ndeep-1] ) = pitem_x, pitem_x->parent = ptop;

						if( pitem_x->factor == 0 )
						{
							pitem_x->factor = -1; 
							pitem_y->factor = 1;
							break;
						}
						else
							pitem_x->factor = pitem_y->factor = 0;
					}
				}
				else if( pitem_y->factor == -2 )
				{
					item	*pitem_x = pitem_y->left;

					//двойной поворот
					if( pitem_x->factor == 1 )
					{
						item	*pitem_w = pitem_x->right;
						pitem_x->right = pitem_w->left;			if( pitem_x->right )pitem_x->right->parent = pitem_x;
						pitem_w->left = pitem_x;				pitem_w->left->parent = pitem_w;
						pitem_y->left = pitem_w->right;			if( pitem_y->left )pitem_y->left->parent = pitem_y;
						pitem_w->right = pitem_y;				pitem_w->right->parent = pitem_w;

						if( pitem_w->factor == -1 )
							pitem_x->factor = 0, pitem_y->factor = +1;
						else if( pitem_w->factor == +1 )
							pitem_x->factor = -1, pitem_y->factor = 0;
						else
							pitem_x->factor = 0, pitem_y->factor = 0;

						pitem_w->factor = 0;

						if( !ndeep )m_ptr = pitem_w, pitem_w->parent = 0;
						else ptop->child( pdirs[ndeep-1] ) = pitem_w, pitem_w->parent = ptop;
					}
					else //if( pitem_x->factor == -1 )
					{
						pitem_y->left = pitem_x->right;			if( pitem_y->left )pitem_y->left->parent = pitem_y;
						pitem_x->right = pitem_y;					pitem_x->right->parent = pitem_x;
						
						if( !ndeep )m_ptr = pitem_x, pitem_x->parent = 0;
						else ptop->child( pdirs[ndeep-1] ) = pitem_x, pitem_x->parent = ptop;

						if( pitem_x->factor == 0 )
						{
							pitem_x->factor = 1;
							pitem_y->factor = -1;
							break;
						}
						else
							pitem_x->factor = pitem_y->factor = 0;
					}
				}
				else if( pitem_y->factor == 1||pitem_y->factor == -1 )
					break;
				ndeep--;
			}
		}

		return true;
	}

//найти позицию по ключу. если 0 то не нашли
	TPOS	find(key_type key)
	{
		long	c;
		for( item	*p = m_ptr; p!=0;  )
		{
			c = _cmp( p->key, key );
			if (!c)return (TPOS)p;

			if( c > 0 )p = p->right;
			else p = p->left;
		}
		return 0;
	}
//убить все из карты
	void clear()
	{
		if( !m_ptr )return;

		item	*p = m_ptr;
		while( true )
		{
			if( p->left )p = p->left;
			//мы можем сходить направо - идем туда
			else if( p->right )p = p->right;
			//мы направо уже сходили, смотрим где
			else if( p->parent == 0 )
			{
				_free( p );
				delete p;
				break;
			}
			//мы возвращаемся слева - надо сходить направо
			else if( p == p->parent->left )
			{
				p = p->parent;
				_free( p->left );
				delete p->left;
				p->left = 0;
			}
			//иначе - мы возвращаемся справа - сходить налево
			else if( p == p->parent->right )
			{
				p = p->parent;	
				_free( p->right );
				delete p->right;
				p->right = 0;
			}
		}
		m_ptr = 0;
		m_count = 0;
	}
	//навигация - интерфейс списка
	TPOS head()
	{
		if( !m_ptr )return 0;
		item	*p = m_ptr;
		while( p->left )p = p->left;
		return (TPOS)p;
	}
	TPOS next(TPOS lpos)
	{
		item	*p = (item*)lpos;
		if( p->right )
		{
			p = p->right;
			while( p->left )p = p->left;
		}
		else if( p->parent && p == p->parent->left )
		{
			p = p->parent;
		}
		else
		{
			while( p->parent && p->parent->right == p )p = p->parent;
			p = p->parent;
		}
		return (TPOS)p;
	}

	TPOS tail()
	{
		if( !m_ptr )return 0;
		item	*p = m_ptr;
		while( p->right )p = p->right;
		return (TPOS)p;
	}
	TPOS prev(TPOS lpos)
	{
		item	*p = (item*)lpos;
		if( p->left )
		{
			p = p->left;
			while( p->right )p = p->right;
		}
		else if( p->parent && p == p->parent->right )
		{
			p = p->parent;
		}
		else
		{
			while( p->parent && p->parent->left == p )p = p->parent;
			p = p->parent;
		}
		return (TPOS)p;
	}

	//вернуть - таки значение по позиции
	data_type	get(TPOS lpos)
	{
		item	*p = (item*)lpos;
		return p->data;
	}

	//вернуть - ключ по позиции
	key_type	get_key(TPOS lpos)
	{
		item	*p = (item*)lpos;
		return p->key;
	}

	//отладочные функции
	TPOS root()					{ return (TPOS)m_ptr; }
	TPOS left(TPOS lpos)		{ item *p = (item*)lpos; return (TPOS)p->left; }
	TPOS right(TPOS lpos)		{ item *p = (item*)lpos; return (TPOS)p->right; }


protected:
	item	*m_ptr;
	long	m_count;
};






#endif //__misc_map_h__