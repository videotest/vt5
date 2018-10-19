#ifndef __commain_h__
#define __commain_h__

#include "GuardInt.h"
#include "com_defs.h"
#include "com_unknown.h"
#include "misc_templ.h"

class CWndClass;

//defs

void dbg_assert( bool bExpression, const char *pszErrorDescription = 0 );

#ifdef _DEBUG
#include <stdio.h> 
#include <stdarg.h>
inline void __trace(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[512];

	nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);

	OutputDebugString(szBuffer);

	va_end(args);
}
#else
inline void __trace(LPCTSTR lpszFormat, ...)
{
}
#endif


#define trace __trace



#define App	app

class app : public ComObjectBase,
			public _static_t<app>
{
	route_unknown();
public:
	class ComInfo
	{
	public:
		void	*(*m_pfn)();
		CLSID	m_clsid;
		CLSID	m_clsidExt;
		_bstr_t	m_bstrProgID;
		_bstr_t	m_bstrSect;

		ComInfo	*m_pnext;
	};
public:
	app();
	virtual ~app();

	virtual bool Init( HINSTANCE h );
	virtual bool Deinit();
public:
	static IUnknown *application()									{return instance()->m_punkAppUnknown;}
	static HINSTANCE handle()										{return instance()->m_hInstance;}
	static IMalloc	*malloc()										{return instance()->m_ptrMalloc;}
public:			//COM interface members
	virtual ComInfo			*FirstComInfo()							{return m_pFirstComInfo;}
	virtual ComObjectBase	*CreateInstance( const CLSID &clsid );
	virtual CWndClass		*FindWindowClass( const char *pszClassName );
	virtual bool ObjectInfo( void *(*p)(), const CLSID &clsid, const char *psz, char *pszAddProgIDSect = 0 );
	virtual bool WindowClass( CWndClass *pclass );

public:			//misc utils
	virtual void handle_error( dword dwErrorCode = DWORD(-1), char *pszErrorDescription = 0, bool bCriticalError = 0 );
	virtual const char *module_filename();

	operator HINSTANCE()	{return m_hInstance;}
protected:
	
public:
	HINSTANCE	m_hInstance;
	ComInfo		*m_pFirstComInfo;
	char		*m_pszModuleFileName;
	CWndClass	*m_pclass;
	IUnknown	*m_punkAppUnknown;
	IUnknown	*m_punkBitmapProvider;
	IMallocPtr	m_ptrMalloc;
	_bstr_t		m_bstrRegProgKey;
};




class ClassFactory : public ComObjectBase, 
					public _dyncreate_t<ClassFactory>,
					public IClassFactory2, 
					public IVTClass 
{
	route_unknown();
public:
	ClassFactory();
	bool RegisterAll( bool bRegister );
	void SetCLSID( const CLSID &clsid );
	void LoadObjectInfo();


public:
	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IClassFactory )
			return (IClassFactory*)this;
		else if ( iid == IID_IClassFactory2 )
			return (IClassFactory2*)this;
		else if ( iid == IID_IVTClass )
			return (IVTClass*)this;
		return ComObjectBase::DoGetInterface(iid);
	}
protected:
	bool _register( App::ComInfo *pi, bool bRegister );

	bool UpdateRegistry(bool bRegister, App::ComInfo * pInfo);
	bool RegisterThis(IVTApplication * pApp, BOOL bModeRegister, App::ComInfo * pInfo, _bstr_t bstrProgIDEx, DWORD dwData = 0);
	bool UnregisterThis(IVTApplication * pApp, BOOL bModeRegister, App::ComInfo * pInfo, _bstr_t bstrProgIDEx);

	void LoadInfo(App::ComInfo * pInfo);

	

//IClassFactory
	com_call CreateInstance( IUnknown * pUnkOuter, REFIID riid, void **ppvObject );
	com_call LockServer( BOOL fLock );

//IClassFactory2
    com_call GetLicInfo(LICINFO * pLicInfo);
	com_call RequestLicKey(DWORD dwReserved, BSTR * pBstrKey);
	com_call CreateInstanceLic(IUnknown * pUnkOuter, IUnknown * pUnkReserved, REFIID riid, BSTR bstrKey, PVOID * ppvObj);

//IVTClass
	com_call VTCreateObject(LPUNKNOWN pUnkOuter, LPUNKNOWN pUnkReserved, REFIID riid, BSTR bstrKey, LPVOID* ppvObject);
	com_call VTRegister(BOOL bRegister);
	com_call VTGetProgID(BSTR * pbstrProgID);
protected:
	CLSID	m_clsid;
};

#include "new.h"

#endif //__commain_h__