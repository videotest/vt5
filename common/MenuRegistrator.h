#ifndef __menuergistrator_h__
#define __menuergistrator_h__

#include "defs.h"
#include "menuconst.h"

//common && common2 suppot

#ifdef _COMMON_LIB
#define menu_exp_func std_dll
#else
#define menu_exp_func
#endif


_bstr_t GetRegistryPath();

class menu_exp_func CMenuRegistrator
{
	
public:
	CMenuRegistrator( );
	~CMenuRegistrator();
	
	bool RegisterMenu( const char* szViewName, const char* szObjectName, const char* szUserMenuName );
	bool UnRegisterMenu( const char* szViewName, const char* szObjectName );			

	bool GetMenuSectionContent( long* plCount, _bstr_t* pNames, _bstr_t* pUserNames );

	_bstr_t GetMenu( const char* szViewName, const char* szObjectName );

protected:
	bstr_t GetKeyName( const char* szViewName, const char* szObjectName );		


public:	
	_bstr_t	GetMenuRootName();

protected:	
	HKEY	m_hKey;
	bool Init( );
	void DeInit();

};


class menu_exp_func CFileRegistrator
{
public:
	CFileRegistrator( );
	void Register(	bstr_t strExt, bstr_t strRegisterName, int nIconIndex, HINSTANCE hIconInst );
	void Unregister( 	bstr_t strExt );
	~CFileRegistrator();

protected:
	bstr_t GetShortName( bstr_t strExt );

public:
	static bool RegisterInSystem();

};

#endif//__menuergistrator_h__

