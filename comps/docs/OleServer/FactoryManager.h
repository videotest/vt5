// FactoryManager.h: interface for the CFactoryManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORYMANAGER_H__84274872_2AFD_46B6_88D8_33AE871F4DA1__INCLUDED_)
#define AFX_FACTORYMANAGER_H__84274872_2AFD_46B6_88D8_33AE871F4DA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constant.h"
#include "OleServInt.h"
#include "misc_list.h"


class CFactoryManager : public ComAggregableBase,
						public IServerFactoryManager
{
	route_unknown();

public:
	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IServerFactoryManager )return (IServerFactoryManager*)this;		
		else return ComAggregableBase::DoGetInterface( iid );
	}

public:
	CFactoryManager();
	virtual ~CFactoryManager();

	void Init();
	void DeInit();




public:	

	//IServerFactoryManager
	com_call CanCloseServer( BOOL* pbCanClose );
	com_call GetServerObjectsInfo( BSTR* pbstr );

	com_call InitApp( );
	com_call ShowApp( BOOL bShow );


	HRESULT OpenFactories();
	HRESULT CloseFactories();

	HRESULT RegisterFactories();
	HRESULT UnRegisterFactories();


protected:
	_list_t2 <IUnknown*> m_comps;	

	bool	m_bFactoryOpen;
	BOOL RegisterServer( CLSID clsid, BSTR bstrComponentName, BSTR bstrProgID, BSTR bstrProgIDVer );
	BOOL UnRegisterServer( CLSID clsid, BSTR bstrComponentName, BSTR bstrProgID, BSTR bstrProgIDVer );
};




#endif // !defined(AFX_FACTORYMANAGER_H__84274872_2AFD_46B6_88D8_33AE871F4DA1__INCLUDED_)
