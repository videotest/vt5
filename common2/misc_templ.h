#ifndef __misc_templ_h__
#define __misc_templ_h__

//templates
template <class Class>
class _dyncreate_t
{
public:
	static void *CreateObject()
	{	return new Class;	}
};

template <class Class>
class _static_t
{
public:
	_static_t()
	{		s_pclass = (Class*)this;	}
	virtual ~_static_t()
	{		s_pclass = 0;	}


	static Class	*s_pclass;
	static Class	*instance()	
	{
		return s_pclass;
	}
	static Class	*get_instance()	
	{
		if( !s_pclass )
			s_pclass = new Class;
		else
			s_pclass->AddRef();
		return s_pclass;
	}
};

template <class Class>
Class*	_static_t<Class>::s_pclass = 0;


template	<class TYPE>
class _ptr_t2
{
public:
	_ptr_t2( TYPE *p )
	{
		ptr = p;
		nsize = -1;//size is unknown
	}
	_ptr_t2( int nCount = 0 )
	{
		ptr = 0;
		nsize = 0;
		alloc( nCount );
	}

	virtual ~_ptr_t2()
	{
		free();
	}

	void zero()
	{
		if( ptr )
			memset( ptr, 0, nsize*sizeof( TYPE ) );
	}
	
	TYPE	*alloc( int nCount )
	{
		free();

		nsize = nCount;
		if( nsize )ptr = new TYPE[nsize];
		return ptr;
	}

	int size()
	{
		return nsize;
	}

	void free()
	{
		delete [] detach();
	}

	void attach( TYPE *p )
	{
		free();

		ptr = p;
		nsize = -1;
	}
	TYPE	*detach()
	{
		TYPE	*pret = ptr;
		ptr = 0;
		nsize = 0;
		return pret;
	}
	operator	TYPE*()	{return ptr;}
public:
	TYPE	*ptr;
	int		nsize;
};

template <class HTYPE>
class _gdi_t
{
public:
	_gdi_t( HTYPE h = 0 )
	{
		handle = 0;
		attach( h );
	}

	virtual ~_gdi_t()
	{
		free();
	}

	HTYPE detach()
	{
		HTYPE hret = handle;
		handle = 0;
		return hret;
	}

	void attach( HTYPE htype )
	{
		if( handle )free();
		handle = htype;
	}

	void free()
	{
		::DeleteObject( detach() );
	}

	operator HTYPE()			{return handle;}
	void operator = (HTYPE hType)	{attach( hType );}
	//operator HTYPE&()	{return handle;}

	HTYPE	handle;
};

typedef _gdi_t<HBRUSH>	_brush;
typedef _gdi_t<HPEN>	_pen;

#define _ptrKiller_t	_ptr_t2

//Class for automatic memory free
/*template	<class TYPE>
class _ptrKiller_t
{
public:
	_ptrKiller_t( TYPE* pPointer )
	{
		m_ptr = pPointer;
	}

	~_ptrKiller_t()
	{		
		if( m_ptr )
		{
			delete m_ptr;			
		}
	}

public:
	TYPE	*m_ptr;
};*/


#endif //__misc_templ_h__