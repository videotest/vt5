#ifndef __misc_list_inl__
#define __misc_list_inl__

template <class DATA, void (*pfn)( void * )>
_list_t2<DATA, pfn>::_list_t2()
{
	m_phead = m_ptail = 0;
	m_nCounter = 0;
}

template <class DATA, void (*pfn)( void * )>
_list_t2<DATA, pfn>::~_list_t2()
{
	deinit();
}

template <class DATA, void (*pfn)( void * )>
void _list_t2<DATA, pfn>::deinit()
{
	Item	*p = m_phead;

	while( p != m_ptail )
	{
		p = p->m_next;
		_free( p->m_prev );
	}
	if( p )_free( p );
	m_ptail = m_phead = 0;
	m_nCounter = 0;
}

template <class DATA, void (*pfn)( void * )>
long _list_t2<DATA, pfn>::insert_before( DATA data, long lpos )
{
	Item	*pitem = (Item*)lpos;
	if( !pitem )pitem = (Item*)head();

	Item	*pnew = new Item;
	ZeroMemory( pnew, sizeof( Item ) );
	pnew->m_data = data;

	pnew->m_next = pitem;
	if( pitem )pnew->m_prev = pitem->m_prev;
	if( pnew->m_prev ) pnew->m_prev->m_next = pnew;
	if( pnew->m_next ) pnew->m_next->m_prev = pnew;

	if( !pnew->m_prev )m_phead = pnew;
	if( !pnew->m_next )m_ptail = pnew;

	m_nCounter++;

	return (long)pnew;
}

template <class DATA, void (*pfn)( void * )>
long _list_t2<DATA, pfn>::insert( DATA data, long lpos )
{
	Item	*pitem = (Item*)lpos;
	if( !pitem )pitem = (Item*)tail();

	Item	*pnew = new Item;
	ZeroMemory( pnew, sizeof( Item ) );
	pnew->m_data = data;

	pnew->m_prev = pitem;
	if( pitem )pnew->m_next = pitem->m_next;
	if( pnew->m_prev ) pnew->m_prev->m_next = pnew;
	if( pnew->m_next ) pnew->m_next->m_prev = pnew;

	if( !pnew->m_prev )m_phead = pnew;
	if( !pnew->m_next )m_ptail = pnew;

	m_nCounter++;

	return (long)pnew;
}

template <class DATA, void (*pfn)( void * )>
void _list_t2<DATA, pfn>::remove( long lpos )
{
	Item	*pitem = (Item*)lpos;
	Item	*pn = pitem->m_next;
	Item	*pp = pitem->m_prev;
	if( pn )pn->m_prev = pp;
	if( pp )pp->m_next = pn;

	if( !pn )m_ptail = pp;
	if( !pp )m_phead = pn;

	_free( pitem );

	m_nCounter--;
}

template <class DATA, void (*pfn)( void * )>
DATA _list_t2<DATA, pfn>::next( long &lpos )
{
	Item	*pitem = (Item*)lpos;
	lpos = (long)pitem->m_next;
	return pitem->m_data;
}

template <class DATA, void (*pfn)( void * )>
DATA _list_t2<DATA, pfn>::prev( long &lpos )
{
	Item	*pitem = (Item*)lpos;
	lpos = (long)pitem->m_prev;
	return pitem->m_data;
}

template <class DATA, void (*pfn)( void * )>
DATA _list_t2<DATA, pfn>::get( long lpos )
{
	Item	*pitem = (Item*)lpos;
	return pitem->m_data;
}

#define _list_ptr_t	_list_t2

#endif // __misc_list_inl__