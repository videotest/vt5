#ifndef __OLESERVINT_H__
#define __OLESERVINT_H__


#include "defs.h"

#define szServerFactorySection	"Server Factory"


interface IServerFactoryManager : public IUnknown
{
	/*
	com_call RegisterFactories() = 0;
	com_call UnRegisterFactories() = 0;

	com_call OpenFactories() = 0;
	com_call CloseFactories() = 0;
	*/

	com_call CanCloseServer( BOOL* pbCanClose ) = 0;
	com_call GetServerObjectsInfo( BSTR* pbstr ) = 0;

	com_call InitApp( ) = 0;
	com_call ShowApp( BOOL bShow ) = 0;

};

interface IServerFactory : public IUnknown
{
	com_call OpenFactory() = 0;
	com_call CloseFactory() = 0;

	com_call GetRegisterInfo( CLSID* pCLSID, BSTR* pbstrComponentName, BSTR* pbstrProgID, BSTR* pbstrProgIDVer ) = 0;

	com_call GetObjectsCount( long* plCount ) = 0;
	com_call GetObjectRefCount( long lObjectIndex, long* plCount ) = 0;
	com_call GetServerLocks( long* plCount ) = 0;
};


declare_interface( IServerFactoryManager, "28A5365C-A2B0-4255-A8B9-517B6138A102" );
declare_interface( IServerFactory, "641A1B46-8B15-4663-9A6C-5C4F6B7B4836" );





#endif// __OLESERVINT_H__