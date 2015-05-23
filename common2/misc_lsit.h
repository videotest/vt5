#ifndef __misc_list_h__
#define __misc_list_h__

template <class DATA>
class TListItem	
{
	Data	pData;
protected:
	CListItem	*m_next;
	CListItem	*m_prev;
};

template <class DATA>
class TListBase
{
public:
	virtual ~TListBase();

	CListItem<DATA>	*Head()	{return m_pHead;}
	CListItem<DATA>	*Tail()	{return m_pTail;}

	DATA	*Next( CListItem<DATA> *&rpitem );
	DATA	*Prev( CListItem<DATA> *&rpitem );

	void DeInit();
};


#endif // __misc_list_h__