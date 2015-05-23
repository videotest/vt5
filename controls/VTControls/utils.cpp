#include "stdafx.h"

#include "utils.h"


BOOL InvokeByName(LPDISPATCH pDispatch, CString strFuncName)
{
	DISPID dispID;

	BSTR bstrFuncName = strFuncName.AllocSysString();
																				 
	//Find func name by id
	HRESULT hr = pDispatch->GetIDsOfNames( 
		IID_NULL, &bstrFuncName, 1, 
		LOCALE_USER_DEFAULT, &dispID 
		);

	CString strError;

	if( hr != S_OK){
		strError.Format( "Can't find function ID:%s", strFuncName );
		AfxMessageBox( strError , MB_OK | MB_ICONSTOP );
		return FALSE;
	}


	VARIANT vt;

	DISPPARAMS dispparams = { NULL, NULL, 0, 0 };	

	hr = pDispatch->Invoke( 
					dispID, 
					IID_NULL, 
					LOCALE_SYSTEM_DEFAULT,
					DISPATCH_METHOD,
					&dispparams,
					&vt,
					NULL,
					NULL
					);

	if( hr == S_OK )
	{			
		return vt.boolVal;
	}
	else
	{
		strError.Format( "Can't execute function :%s\nError code:%u ", 
			strFuncName, hr );
		AfxMessageBox( strError , MB_OK | MB_ICONSTOP );
		return FALSE;
	}

	return TRUE;
}




#ifndef FOR_HOME_WORK


//Format for Key Value int Key: "Section\Value";

//Restore Section & Entry from KeyValue
BOOL GetSectionEntry(const CString strKeyValue, CString& strSection, CString& strEntry)
{

	if( strKeyValue == "" ) return FALSE;

	if( strKeyValue[0] != '\\' ) return FALSE;


	CString _strKeyValue = strKeyValue;
//	_strKeyValue.Delete( 0 );
	

	int nLength = _strKeyValue.GetLength();
	int nBackSlashPosition = _strKeyValue.Find( "\\", 1 );

	// verify that accept ("?\?")
	if( nLength < 3 ) return FALSE;

	//verify not ("\?")
	if( nBackSlashPosition  < 1) return FALSE;

	strSection	= _strKeyValue.Left( nBackSlashPosition );
	strEntry	= _strKeyValue.Right( nLength - nBackSlashPosition - 1 );

	if( strSection == "" ) return FALSE;
	if( strEntry == "" ) return FALSE;

		
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//		Main Set
//
//
/////////////////////////////////////////////////////////////////////////////////////
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	_variant_t	var = varDefault;
	if( !_CheckInterface( punkDoc, IID_INamedData ) )
		return;

	_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

	sptrINamedData	sptrData( punkDoc );
	sptrData->SetupSection( bstrSection );
	sptrData->SetValue( bstrEntry, var );
}

//Set String
void _SetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue )
{
	_variant_t	var( pszValue );
	_SetValue( punkDoc, pszSection, pszEntry, var );
}

//Set color
void _SetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF cr )
{
	int	r = GetRValue( cr );
	int	g = GetGValue( cr );
	int	b = GetBValue( cr );

	CString	strDef;
	strDef.Format( "(%d, %d, %d)", r, g, b );
	_SetValue( punkDoc, pszSection, pszEntry, strDef );
}


void _SetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue )
{
	_variant_t	var( lValue );
	_SetValue( punkDoc, pszSection, pszEntry, var );
}


void _SetValueDouble( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue )
{
	_variant_t	var( fValue );
	_SetValue( punkDoc, pszSection, pszEntry, var );

}

/////////////////////////////////////////////////////////////////////////////////////
//
//		Main Get
//
//
/////////////////////////////////////////////////////////////////////////////////////
_variant_t _GetValue( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	_variant_t	var = varDefault;
	if( !_CheckInterface( punkDoc, IID_INamedData ) )
		return var;

	_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

	sptrINamedData	sptrData( punkDoc );
	sptrData->SetupSection( bstrSection );
	sptrData->GetValue( bstrEntry, &var );
	sptrData->SetValue( bstrEntry, var );

	return var;

}

//Get String
CString _GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault )
{
	try
	{
		_variant_t	var( pszDefault );
		var = _GetValue( punkDoc, pszSection, pszEntry, var );

		
		var.ChangeType( VT_BSTR );

		return CString( var.bstrVal );
	}
	catch( ... )
	{
		return CString( pszDefault );
	}
}

//Set Color
COLORREF _GetValueColor( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, COLORREF crDefault )
{
	int	r = GetRValue( crDefault );
	int	g = GetGValue( crDefault );
	int	b = GetBValue( crDefault );

	CString	strDef;
	strDef.Format( "(%d, %d, %d)", r, g, b );

	CString	strValue = _GetValueString( punkDoc, pszSection, pszEntry, strDef );

	if( !strValue.GetLength() )
		return crDefault;

	if( ::sscanf( strValue, "(%d, %d, %d)", &r, &g, &b ) == 3 )
	{
		return RGB( r, g, b );
	}


	_SetValueColor( punkDoc, pszSection, pszEntry, crDefault );
	return crDefault;
}

long _GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault )
{
	try
	{
		_variant_t	var( lDefault );
		var = _GetValue( punkDoc, pszSection, pszEntry, var );

		var.ChangeType( VT_I4 );

		return var.lVal;
	}
	catch( ... )
	{
		return lDefault;
	}
}


double _GetValueDouble( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fDefault )
{
	try
	{
		_variant_t	var( fDefault );
		var = _GetValue( punkDoc, pszSection, pszEntry, var );

		var.ChangeType( VT_R8 );

		return var.dblVal;
	}
	catch( ... )
	{
		return fDefault;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//2 Helper function for visible & enable control status

BOOL _IsWindowVisible( CWnd* pWnd )
{
	if( !pWnd )
		return FALSE;

	if( !::IsWindow( pWnd->GetSafeHwnd() ) )
		return FALSE;

	return pWnd->IsWindowVisible( /*pWnd->GetSafeHwnd()*/ );
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL _IsWindowEnable( CWnd* pWnd )
{
	if( !pWnd )
		return FALSE;

	if( !::IsWindow( pWnd->GetSafeHwnd() ) )
		return FALSE;

	return pWnd->IsWindowEnabled();
}

/////////////////////////////////////////////////////////////////////////////////////
IUnknown* GetAppUnknown( bool bAddRef /*= false*/ )
{
	IUnknown * punkApp = 0;
	typedef IUnknown* (*PGUARDGETAPPUNKNOWN)(bool bAddRef /*= false*/);
	HMODULE hModule = GetModuleHandle( 0 );
	if (hModule)
	{
		PGUARDGETAPPUNKNOWN pfn = (PGUARDGETAPPUNKNOWN)GetProcAddress(hModule, "GuardGetAppUnknown");
		if (pfn)
			punkApp = pfn( bAddRef );
	}

	return punkApp;
}

/////////////////////////////////////////////////////////////////////////////////////


bool GetFontFromDispatch( COleControl *pOleCtrl, CFont& font, BOOL bUseSystemDefault )
{	
	if( !pOleCtrl )
		return false;

	if( bUseSystemDefault )
	{
		LOGFONT lf;
		::ZeroMemory( &lf, sizeof( LOGFONT ) );
		//CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );
		//[AY]
		CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(DEFAULT_GUI_FONT) );
		
		pFont->GetLogFont( &lf );

		return ( font.CreateFontIndirect( &lf ) == TRUE );
	}

	/*
	BSTR	bstrName = 0;
	CY		sizeCY;
	BOOL	bBold;
	BOOL	bItalic;
	BOOL	bUnderline;
	BOOL	bStrikethrough;
	SHORT	nWeight;
	SHORT	nCharset;
	*/

	LPFONTDISP plFontDisp = 0;
	plFontDisp = pOleCtrl->GetFont();
	if( !plFontDisp )
		return false;

	IFontPtr fontPtr( plFontDisp );	
	plFontDisp->Release();	plFontDisp = 0;

	if( fontPtr == 0 )
		return false;


	HFONT hFont = 0;
	fontPtr->get_hFont( &hFont );
	LOGFONT lf;
	CFont* _pfont = CFont::FromHandle( hFont );
	if( !_pfont )
		return false;

	_pfont->GetLogFont( &lf );
	lf.lfCharSet = DEFAULT_CHARSET;

	return ( font.CreateFontIndirect( &lf ) == TRUE );

	/*

	fontPtr->get_Name( &bstrName );
	fontPtr->get_Size( &sizeCY );
	fontPtr->get_Bold( &bBold );
	fontPtr->get_Italic( &bItalic );
	fontPtr->get_Underline( &bUnderline );
	fontPtr->get_Strikethrough( &bStrikethrough );
	fontPtr->get_Weight( &nWeight );
	fontPtr->get_Charset( &nCharset );                

	CString strFontName = bstrName;
	::SysFreeString( bstrName );

	return font.CreateFont( 
		-(int)( ((double)sizeCY.Lo)/(10000.0) ), 0, 
		0, 0, 
		nWeight, (BYTE)bItalic, 
		(BYTE)bUnderline, (BYTE)bStrikethrough, 
		(BYTE)DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH,						
		strFontName 
		) == TRUE;
		*/
}


#endif