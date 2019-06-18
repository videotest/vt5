#ifndef __misc_list_h__
#define __misc_list_h__

void FreeEmpty( void *ptr );
void FreeReleaseUnknown( void *ptr );

template <class DATA>
class _list_item_t
{
public:
	DATA		m_data;
	_list_item_t<DATA>	*m_next;
	_list_item_t<DATA>	*m_prev;
	byte	dummy[1000];
};

template <class DATA, void (*pfuncFree)( void * )=FreeEmpty>
class _list_t2
{
public:
	typedef _list_item_t<DATA>	Item;
public:
	_list_t2();
	virtual ~_list_t2();
public:
	void deinit();

	long	head()	{return (long)m_phead;}
	long	tail()	{return (long)m_ptail;}
	long			Count()	{return m_nCounter;}

	DATA	next( long &lpos );
	DATA	prev( long &lpos );
	DATA	get( long lpos );

	long	insert_before( DATA data, long lpos = 0 );
	long	insert( DATA data, long lpos = 0 );
	void	remove( long lpos );
protected:
	virtual void _free( Item *p ){pfuncFree( p->m_data );delete p;}
protected:
	Item	*m_phead;
	Item	*m_ptail;
	long	m_nCounter;
};


#include "misc_list.inl"

#endif // __misc_list_h__