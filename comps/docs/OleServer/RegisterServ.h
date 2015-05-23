#ifndef __registerserv_h__
#define __registerserv_h__

BOOL _RegisterServer( CLSID clsid, char* pszComponentName, char* pszProgID, char* pszProgIDVer );
BOOL _UnRegisterServer( CLSID clsid, char* pszComponentName, char* pszProgID, char* pszProgIDVer );

#endif //__registerserv_h__