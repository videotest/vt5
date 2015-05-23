#pragma once

extern long g_block_mem_hook;
////////////////////////////////////////////////////////////////////////////
//heap_info
class heap_info
{
public:
	heap_info(){
		m_laddress		= 0;
		m_dw_size		= 0;
		m_hheap			= 0;		
		m_pstack		= 0;
		m_size_stack	= 0;

		m_dw_time		= ::GetTickCount();
	}
	
	~heap_info(){
		if( m_pstack )	delete []m_pstack;
	}

	void copy_from( heap_info* p_src )
	{
		if( !p_src )	return;

		m_laddress		= p_src->m_laddress;
		m_dw_size		= p_src->m_dw_size;
		m_hheap			= p_src->m_hheap;
		m_dw_time		= p_src->m_dw_time;
		m_size_stack	= p_src->m_size_stack;
		if( p_src->m_pstack )
		{
			m_pstack = new unsigned[m_size_stack];
			memcpy( m_pstack, p_src->m_pstack, sizeof(unsigned)*m_size_stack );
		}
	}

	static void free( void* p){
		delete(heap_info*)p;
	}

	//mem info
	long		m_laddress;
	DWORD		m_dw_size;
	HANDLE		m_hheap;

	//stack
	unsigned*	m_pstack;
	BYTE		m_size_stack;
	//time
	DWORD		m_dw_time;
};

////////////////////////////////////////////////////////////////////////////
//lock
class lock
{
public:
	lock( long* pbvalue, long bnew )
	{
		m_bold		= *pbvalue;
		::InterlockedExchange( pbvalue, bnew );
		m_pbvalue	= pbvalue;
	}
	virtual ~lock()
	{
		::InterlockedExchange( m_pbvalue, m_bold );
	}
	
	long*	m_pbvalue;
	long	m_bold;	
};

//////////////////////////////////////////////////////////////////////
//
//	Critical section enter/leave
//
//////////////////////////////////////////////////////////////////////
class enter_cc
{public:
	enter_cc( CRITICAL_SECTION*	pcs )
	{
//		lock l( &g_block_mem_hook, true );
		m_pcs = pcs;
		if( m_pcs )
			::EnterCriticalSection( m_pcs );			
	}
	virtual ~enter_cc()	
	{	
//		lock l( &g_block_mem_hook, true );
		if( m_pcs )
			::LeaveCriticalSection( m_pcs );		
	}
protected:
	CRITICAL_SECTION*	m_pcs;
};
