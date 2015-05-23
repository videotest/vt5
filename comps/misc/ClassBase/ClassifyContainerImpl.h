#pragma once

#include <atlstr.h>
#include "classify_int.h"

#include "\vt5\awin\misc_ptr.h"
#include "\vt5\awin\misc_map.h"
#include "\vt5\awin\misc_list.h"
#include "measure5.h"
#include "misc_str.h"

class CClassifyContainerImpl : public IClassifyParams, public IClassifyProxy2, public IClassifyInfoProxy
{
protected:
	DWORD m_dwFlag;

	long *m_plKeys;
	long m_lSz;

	long *m_plClasses;
	long m_lSz2;

	CString m_strName, m_strPROGID;

	template <class TDataType> class XItem
	{
		TDataType *m_pfVals;
		long m_lSz;
	public:
		XItem()
		{
			m_pfVals = 0;
			m_lSz = 0;
		}
		
		XItem( long lSz )
		{
			if( lSz > 0 )
			{
				m_pfVals = new TDataType[lSz];
				m_lSz = lSz;
			}
		}

		~XItem()
		{
			if( m_pfVals )
				delete []m_pfVals;
			m_lSz = 0;
		}
		XItem( const XItem &item )
		{
			m_pfVals = 0;
			m_lSz = 0;

			*this = item;
		}

		TDataType *GetData() { return m_pfVals; }
		long GetSize() { return m_lSz; }

		void operator=( const XItem &item )
		{
			if( m_pfVals )
				delete []m_pfVals; 
			m_pfVals = 0;

			m_lSz = item.m_lSz;

			if( m_lSz > 0 )
			{
				m_pfVals = new TDataType[m_lSz];
				::CopyMemory( m_pfVals, item.m_pfVals, sizeof( TDataType ) * m_lSz );
			}
		}

		bool operator==( const XItem &item )
		{
			if( m_lSz != item->m_lSz )
				return false;

			if( m_pfVals == item->m_pfVals )
				return true;

			for( long i = 0; i < m_lSz; i++ )
			{
				if( m_pfVals[i] != item->m_pfVals[i] )
					return false;
			}
			return true;
		}

		bool operator!=( const XItem &item )
		{
			return !( *this == item );
		}

		TDataType &operator[]( long lID )
		{
			return m_pfVals[lID]; 
		}
		static void free( void* p ){
			delete (XItem*)p;}

	};

	typedef XItem<double> XKeyItem;
	typedef _map_t< XKeyItem*, long, cmp_long, XKeyItem::free > XKeyMap;
	typedef _map_t< CLASSINFO, long, cmp_long > XClassMap;

	XKeyMap	*m_mapKeyParams;
	XClassMap m_mapClassParams;
protected:	
	struct XObjectItem
	{
		IUnknownPtr sptrObject;
		long lClass;
		GuidKey guidKey;

		XObjectItem()
		{
			lClass = 0;
		}

		XObjectItem(  const XObjectItem &item  )
		{
			sptrObject = item.sptrObject;
			lClass = item.lClass;
			guidKey = item.guidKey;
		}

		void operator=( const XObjectItem &item )
		{
			sptrObject = item.sptrObject;
			lClass = item.lClass;
			guidKey = item.guidKey;
		}
		static void free( void* p ){
			delete (XObjectItem*)p;}
		static long cmp_guid_ptr( const GUID *p1, const GUID *p2 ){
			return memcmp( p1, p2, sizeof( GUID ) );}
	};
	_list_map_t<XObjectItem*, const GUID*, XObjectItem::cmp_guid_ptr, XObjectItem::free> m_arObjectList;
	_ptr_t<int>		m_quels;
	XKeyItem*		find_key_item( long lclass, long lkey );
	//_list_t<XObjectItem> m_arObjectList;

public:
	CClassifyContainerImpl()
	{
		m_plKeys = 0;
		m_lSz = 0;

		m_plClasses = 0;
		m_lSz2 = 0;

		m_dwFlag = 0;

		m_mapKeyParams = 0;

		m_lDescrClass = -1;
		m_strDescription = _T( "" );
	}
	~CClassifyContainerImpl()
	{
		if( m_plKeys )
			delete []m_plKeys;

		m_lSz = 0;

		if( m_plClasses )
			delete []m_plClasses;
		m_lSz2 = 0;

		if( m_mapKeyParams )
			delete []m_mapKeyParams; 
		m_mapKeyParams = 0;
	}
public:
// IClassifyParams
	com_call SetName( /*[in] */BSTR bstrName );
	com_call GetName(/*[out] */BSTR *pbstrName );

	com_call SetClasses( /*[in] */long *plClasses , /*[in]*/ long lSz );
	com_call GetClasses( /*[out] */long **pplClasses , /*[out]*/ long *plSz );
	com_call RemoveClasses();

	com_call SetClassParams( /*[in]*/ long lClass, /*[in]*/ CLASSINFO pfParams );
	com_call GetClassParams( /*[in]*/ long lClass, /*[out]*/CLASSINFO *ppParams );

	com_call SetKeys( /*[in] */long *plKeys , /*[in]*/ long lSz );
	com_call GetKeys( /*[out] */long **pplKeys , /*[out]*/ long *plSz );
	com_call RemoveKeys();

	com_call GetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[out]*/ double **ppfParams, /*[out]*/ long *plSz );
	com_call SetKeyParams( /*[in] */long lClass ,/*[in]*/ long lKey, /*[in]*/ double *pfParams, /*[in]*/ long plSz );

// IClassifyProxy
	com_call Load( /*[in]*/ BSTR bstrFile );
	com_call Store( /*[in]*/ BSTR bstrFile );

	com_call Start();
	com_call Process( /*[in]*/ IUnknown *punkObject );
	com_call Finish() { return S_OK; };
	com_call GetObjectClass( /*[in]*/ IUnknown *punkObject, /*out*/ long *plClass );

	com_call GetFlags( /*[out]*/ DWORD *pdwFlag );
	com_call SetFlags( /*[in]*/ DWORD dwFlag );
// IClassifyProxy2
	com_call Teach();

// vanek - 31.10.2003
// IClassifyInfoProxy 
	com_call PrepareObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass );
	com_call GetObjectDescrByClass( /*[out]*/ BSTR *pbstrDescription );
protected:
	IUnknownPtr m_sptrDescrObj;
	long		m_lDescrClass;
	CString		m_strDescription;
    
	BOOL		IsNeedDescrObj( IUnknown *punkCurrObj, long lCurrClass );
	void		InitDescription() { m_strDescription.Empty(); }

protected:
	CString _get_value_string( CString strFileName, CString strSection, CString strKey );
	void    _set_value_string( CString strFileName, CString strSection, CString strKey, CString strValue );
};

#include "ClassifyContainerImpl.inl"
