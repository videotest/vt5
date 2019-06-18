#ifndef __com_unknown_h__
#define __com_unknown_h__

#include "com_defs.h"
//#include "misc_new.h"
					  //public Object, 
class ComObjectBase : public IUnknown
{
public:
	ComObjectBase();
	virtual ~ComObjectBase();

	virtual IUnknown *DoGetInterface( const IID &iid )
	{		return 0;	}
	virtual HRESULT DoQueryAggregates( const IID &iid, void **ppRet )
	{		return E_NOINTERFACE;	}
public:
	virtual void BeforeFinalRelease() {};
	long _addref()										{	return ++m_nRefCounter;	}
	long _release()
	{
		if(m_nRefCounter==1) BeforeFinalRelease();
		if( !--m_nRefCounter )
		{
			delete this;
			return 0;
		}return m_nRefCounter;	
	}
	HRESULT _qi( const IID &iid, void **pret );
public:
	long RefCounter()									{	return m_nRefCounter;	}
	virtual IUnknown *Unknown()							{	return this;			}
	virtual IUnknown *InnerUnknown()					{	return this;			}
	virtual bool SetOuterUnknown( IUnknown *punk )		{	return false;			}

	
protected:
	long	m_nRefCounter;
	bool	m_bLockApp;
};


class ComAggregableBase : public ComObjectBase
{
protected:
	class XInnerUnknown : public IUnknown
	{
	public:
		XInnerUnknown( ComAggregableBase *pa )
		{
			m_pHost = pa;
		}
		com_call_ref AddRef()
		{	return m_pHost->ComObjectBase::_addref();	}
		com_call_ref Release()
		{	return m_pHost->ComObjectBase::_release();	}
		com_call QueryInterface( const IID &riid, void **pp )
		{	return m_pHost->_qi_own( riid, pp );	}
	public:
		ComAggregableBase	*m_pHost;
	};
public:
	ComAggregableBase()
	{
		m_punkInner = 0;
 		m_punkOuter = 0;
	}
	virtual ~ComAggregableBase()
	{
		if( m_punkInner )delete m_punkInner;
		m_punkInner = 0;
		m_punkOuter = 0;
	}
public:
	virtual bool SetOuterUnknown( IUnknown *punk )
	{
		m_punkInner = new XInnerUnknown( this );
		m_punkOuter = punk;
		return true;
	}
public:
	long _addref()
	{
		if( m_punkOuter )
			return m_punkOuter->AddRef();
		return ComObjectBase::_addref();
	}
	long _release()
	{
		if( m_punkOuter )
			return m_punkOuter->Release();
		return ComObjectBase::_release();
	}
	virtual IUnknown *Unknown()							
	{	
		return this?(m_punkOuter?m_punkOuter:this):0;			
	}
	virtual IUnknown *InnerUnknown()
	{
		return this?(m_punkInner?m_punkInner:this):0;			
	}

	HRESULT _qi( const IID &iid, void **pret );
	HRESULT _qi_own( const IID &iid, void **pret );
protected:
	IUnknown	*m_punkOuter;
	IUnknown	*m_punkInner;
};




#endif //__com_unknown_h__