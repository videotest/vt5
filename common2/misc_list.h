#ifndef __misc_list_h__
#define __misc_list_h__

void FreeEmpty( void *ptr );
void FreeReleaseUnknown( void *ptr );

//struct __POSITION {};
//typedef __POSITION* POSITION;
typedef POSITION TPOS;

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

	TPOS	head()	{return (TPOS)m_phead;}
	TPOS	tail()	{return (TPOS)m_ptail;}
	long			Count()	{return m_nCounter;}

	DATA	next( TPOS &lpos );
	DATA	prev( TPOS &lpos );
	DATA	get( TPOS lpos );

	TPOS	insert_before( DATA data, TPOS lpos = 0 );
	TPOS	insert( DATA data, TPOS lpos = 0 );
	void	remove( TPOS lpos );
protected:
	virtual void _free( Item *p ){pfuncFree( p->m_data );delete p;}
protected:
	Item	*m_phead;
	Item	*m_ptail;
	long	m_nCounter;
};


#include "misc_list.inl"

#endif // __misc_list_h__