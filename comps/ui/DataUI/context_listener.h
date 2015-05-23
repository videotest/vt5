#pragma once

/////////////////////////////////////////////////////////////////////////////
class context_listener
{
public:
	context_listener();
	virtual ~context_listener();

protected:
	virtual void	register_on_controller( IUnknown* punk_ctrl ) = 0;
	virtual void	unregister_on_controller( IUnknown* punk_ctrl ) = 0;

	//active
	IUnknownPtr		m_ptr_active_doc;
	IUnknownPtr		m_ptr_active_view;

	virtual void	register_doc( IUnknown* punk_doc );
	virtual void	unregister_doc();

	virtual void	register_view( IUnknown* punk_view );
	virtual void	unregister_view();


	//notification
	virtual void	on_notify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//notifications handlers
	virtual bool	_on_activate_view( IUnknown* punk_doc, IUnknown* punk_view );
	virtual bool	_on_close_document( IUnknown* punk_doc );
	virtual bool	_on_destroy_view( IUnknown* punk_view );
	virtual bool	_on_data_context_change( IUnknown* punk_view, IUnknown* punk_obj, NotifyCodes nc );
};                  

/////////////////////////////////////////////////////////////////////////////	
inline context_listener::context_listener()
{
	//register_on_controller( 0 );
}

/////////////////////////////////////////////////////////////////////////////	
inline context_listener::~context_listener()
{
	//unregister_on_controller( 0 );
}

/////////////////////////////////////////////////////////////////////////////	
inline void context_listener::register_doc( IUnknown* punk_doc )
{
	unregister_doc( );

	m_ptr_active_doc = punk_doc;
	
	if( m_ptr_active_doc )
		register_on_controller( m_ptr_active_doc );
}

/////////////////////////////////////////////////////////////////////////////	
inline void context_listener::unregister_doc()
{
	if( m_ptr_active_doc )
		unregister_on_controller( m_ptr_active_doc );
	
	m_ptr_active_doc = 0;
}

/////////////////////////////////////////////////////////////////////////////	
inline void context_listener::register_view( IUnknown* punk_view )
{
	unregister_view( );

	m_ptr_active_view = punk_view;
	
	if( m_ptr_active_view )
		register_on_controller( m_ptr_active_view );
}

/////////////////////////////////////////////////////////////////////////////	
inline void context_listener::unregister_view()
{
	if( m_ptr_active_view )
		unregister_on_controller( m_ptr_active_view );
	
	m_ptr_active_view = 0;
}


//notification
/////////////////////////////////////////////////////////////////////////////	
inline void context_listener::on_notify(	const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, 
									void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szAppActivateView ) )
	{	
		if( punkFrom && punkHit )
		{
			_on_activate_view( punkHit, punkFrom );
		}
	}
	else if( !strcmp( pszEvent, "AfterClose" ) )
	{
		_on_close_document( punkFrom );
	}
	else if( !strcmp( pszEvent, szAppDestroyView ) )
	{
		_on_destroy_view( punkFrom );
	}
	else if( !strcmp( pszEvent, szEventActiveContextChange ) )
	{		
		if( cbSize == sizeof( NotifyCodes ) )
		{			
			if( ::GetObjectKey( punkFrom ) == ::GetObjectKey( m_ptr_active_view ) )
			{
				NotifyCodes* pnc = (NotifyCodes*)(pdata);
				_on_data_context_change( punkFrom, punkHit, *pnc );
			}
		}
	}
	else if( !strcmp( pszEvent, szEventActivateObject ) )
	{
	}	

}

//notifications handlers
/////////////////////////////////////////////////////////////////////////////	
inline bool context_listener::_on_activate_view( IUnknown* punk_doc, IUnknown* punk_view )
{
	if( !CheckInterface( punk_doc, IID_IDocument ) )
		return false;

	if( !CheckInterface( punk_view, IID_IView ) )
		return false;

	if( ::GetObjectKey( punk_doc ) == GetObjectKey( m_ptr_active_doc ) &&
		::GetObjectKey( punk_view ) == GetObjectKey( m_ptr_active_view ) )
		return false;

	register_doc( punk_doc );
	register_view( punk_view );

	return true;
}

/////////////////////////////////////////////////////////////////////////////	
inline bool context_listener::_on_close_document( IUnknown* punk_doc )
{
	if( !CheckInterface( punk_doc, IID_IDocument ) )
		return false;

	if( ::GetObjectKey( m_ptr_active_doc ) != ::GetObjectKey( punk_doc ) )
		return false;

	unregister_doc( );
	return true;
}

/////////////////////////////////////////////////////////////////////////////	
inline bool context_listener::_on_destroy_view( IUnknown* punk_view )
{
	if( !CheckInterface( punk_view, IID_IView ) )
		return false;

	if( ::GetObjectKey( m_ptr_active_view ) != ::GetObjectKey( punk_view ) )
		return false;

	unregister_view( );
	return true;
}

/////////////////////////////////////////////////////////////////////////////	
inline bool context_listener::_on_data_context_change( IUnknown* punk_view, IUnknown* punk_obj, NotifyCodes nc )
{
	if( !punk_view || !punk_obj )
		return false;

	if( ::GetObjectKey( punk_view ) != ::GetObjectKey( m_ptr_active_view ) )
		return false;

	if( nc == ncAddObject )
	{

	}
	else if( nc == ncRemoveObject )
	{

	}

	return true;
}
