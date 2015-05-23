#ifndef __utils_h__FOR_VT_CONTROLS
#define __utils_h__FOR_VT_CONTROLS



#include <comutil.h>
#include <atlconv.h>



BOOL InvokeByName(LPDISPATCH pDispatch, CString strFuncName);




#ifndef FOR_HOME_WORK


#include "..\\..\\ifaces\\data5.h"
#endif



#ifndef FOR_HOME_WORK
BOOL GetSectionEntry(const CString strKeyValue, CString& strSection, CString& strEntry);

inline bool _CheckInterface( IUnknown *punkn, const GUID guid )
{
	try{
	if( punkn )
	{
		IUnknown	*ptest = 0;

		if( punkn->QueryInterface( guid, (void**)&ptest ) ==S_OK )
		{
			ptest->Release();
			return true;
		}
		return false;
	}
	else
		return false;
	}catch( ... ){};
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////

//Main Set
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );

//String
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue );
//Color
void _SetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF cr );

void _SetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue );

void _SetValueDouble( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue );


/////////////////////////////////////////////////////////////////////////////////////

//Main Get
_variant_t _GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );

//String
CString _GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault = 0 );
//Color
COLORREF _GetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crDefault );

long _GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault = 0 );

double _GetValueDouble( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fDefault = 0 );

/////////////////////////////////////////////////////////////////////////////////////
//2 Helper function for visible & enable control status

BOOL _IsWindowVisible( CWnd* pWnd );
BOOL _IsWindowEnable( CWnd* pWnd );

/////////////////////////////////////////////////////////////////////////////////////
//3 Get the application's unknown pointer
IUnknown* GetAppUnknown( bool bAddRef = false );


/////////////////////////////////////////////////////////////////////////////////////

#endif

#endif 