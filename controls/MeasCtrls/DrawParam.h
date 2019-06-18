#pragma once
#include "calc_int.h"
#include "\vt5\comps\misc\Calculator\Calculator.h"
#include "Params.h"
#include "works_modes.h"
#include "data5.h"

inline long GetValueIntWithoutSet( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault )
{
	try
	{
		_variant_t	var( lDefault );
		if( !CheckInterface( punkDoc, IID_INamedData ) )
			return var;

		_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

		sptrINamedData	sptrData( punkDoc );
		sptrData->SetupSection( bstrSection );
		
		long lEntriesCount = 0;
		sptrData->GetEntriesCount( &lEntriesCount );
        sptrData->GetValue( bstrEntry, &var );
		long lNewEntriesCount = 0;
		sptrData->GetEntriesCount( &lNewEntriesCount );
		if( lNewEntriesCount > lEntriesCount )
			sptrData->DeleteEntry( bstrEntry );

		_VarChangeType( var, VT_I4 );
		return var.lVal;
	}
	catch( ... )
	{
		return lDefault;
	}
}

inline CString GetValueStringWithoutSet( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault )
{
	try
	{
		_variant_t	var( pszDefault );
		if( !CheckInterface( punkDoc, IID_INamedData ) )
			return var;

		_bstr_t	bstrSection( pszSection ), bstrEntry( pszEntry );

		sptrINamedData	sptrData( punkDoc );
		sptrData->SetupSection( bstrSection );

		long lEntriesCount = 0;
		sptrData->GetEntriesCount( &lEntriesCount );
        sptrData->GetValue( bstrEntry, &var );
		long lNewEntriesCount = 0;
		sptrData->GetEntriesCount( &lNewEntriesCount );
		if( lNewEntriesCount > lEntriesCount )
			sptrData->DeleteEntry( bstrEntry );
	
		_VarChangeType( var, VT_BSTR );

		return CString( var.bstrVal );
	}
	catch( ... )
	{
		return CString( pszDefault );
	}
}

inline BOOL search_section_by_key( /*CString *psMainSection*/ long lWorksMode, long lSearchKey, CString *psSection )
{
	if( !psSection /*|| !psMainSection*/)
		return FALSE;

	//CString stMainSection;
								   
	switch( lWorksMode )
	{
	case WKM_MEASUSER:
		{
		//stMainSection = _T( sMainMeasParamsSection );
			long lCount = ::GetEntriesCount( ::GetAppUnknown(), _T( sMainMeasParamsSection ) );

			for( long lPos = 0; lPos < lCount; lPos++ )
			{
				*psSection = sMainMeasParamsSection;
				CString sEntryName = ::GetEntryName( ::GetAppUnknown(), _T( sMainMeasParamsSection ), lPos );

				*psSection += "\\" + sEntryName;
				// vanek : corrected for the case when lSearchKey is zero - 29.03.2005
				long lKey = GetValueIntWithoutSet( ::GetAppUnknown(), *psSection, sKey, lSearchKey + 1 ); 
				if( lKey == lSearchKey )
					return TRUE;
			}
			psSection->Empty( );
		}
		break;
	case WKM_STATUSER:
		{
		//stMainSection = _T( sMainStatParamsSection );
			long lCount = ::GetEntriesCount( ::GetAppUnknown(), sMainStatParamsSection );

			for( long lPos = 0; lPos < lCount; lPos++ )
			{
				*psSection = sMainStatParamsSection;
				CString sEntryName = ::GetEntryName( ::GetAppUnknown(), sMainStatParamsSection, lPos );
				long lKey = atol( sEntryName ); 
				if( lKey == lSearchKey )
				{
                    // vanek - 29.03.2005
					*psSection += "\\" + sEntryName;		
					return TRUE;
				}
			}
			psSection->Empty( );
		}
		break;
	} 

	return FALSE;
}

// get params sections by works mode 
inline long	get_sections_by_worksmode( long lWorksMode, BSTR **ppbstrSections, long **pplKeys )
{
	if( !ppbstrSections && !pplKeys ) 
		return -1;

	long lParamsCount = 0;
	_bstr_t *pbstrt = 0;
	long	*plong = 0;
	switch( lWorksMode )
	{
	case WKM_MEASUSER:
		{
			if( pbstrt )
				break;

			long lCount = ::GetEntriesCount( ::GetAppUnknown(), _T( sMainMeasParamsSection ) );
			pbstrt = new _bstr_t[ lCount ];
			plong = new long[ lCount ];

			for( long lPos = 0; lPos < lCount; lPos++ )
			{
				_bstr_t	bstrEntryName = ::GetEntryName( ::GetAppUnknown(), _T( sMainMeasParamsSection ), lPos );
				_bstr_t bstrParamsSection( _T( sMainMeasParamsSection ) );
				bstrParamsSection += _T("\\") + bstrEntryName;

				_bstr_t bstrExpr = GetValueStringWithoutSet( ::GetAppUnknown(), bstrParamsSection, sExprString, "");
				if( ((char*)(bstrExpr))[0] == '#' )
					continue; // если ExprString начинается с # - игнорируем, секретный параметр

				long lKey = GetValueIntWithoutSet( ::GetAppUnknown(), bstrParamsSection, sKey, MEAS_USER_PARAMS_FIRST_KEY - 1); 
				if( ( lKey >= MEAS_USER_PARAMS_FIRST_KEY ) && ( lKey <= MEAS_USER_PARAMS_LAST_KEY ) )
				{
					*(pbstrt + lParamsCount) = bstrParamsSection;
					*(plong + lParamsCount) = lKey;
					lParamsCount ++;
				}
			}
		}
		break;
	case WKM_STATUSER:
		{
			if( pbstrt )
				break;

			long lCount = ::GetEntriesCount( ::GetAppUnknown(), _T( sMainStatParamsSection ) );
			pbstrt = new _bstr_t[ lCount ];
			plong = new long[ lCount ];
						
			for( long lPos = 0; lPos < lCount; lPos++ )
			{
				_bstr_t	bstrEntryName = ::GetEntryName( ::GetAppUnknown(), _T( sMainStatParamsSection ), lPos );
				_bstr_t bstrParamsSection( _T( sMainStatParamsSection ) );
				bstrParamsSection += _T("\\") + bstrEntryName;

				_bstr_t bstrExpr = GetValueStringWithoutSet( ::GetAppUnknown(), bstrParamsSection, sExprString, "");
				if( ((char*)(bstrExpr))[0] == '#' )
					continue; // если ExprString начинается с # - игнорируем, секретный параметр

				long lKey = atol( bstrEntryName );
				if( ( lKey >= STAT_USER_PARAMS_FIRST_KEY ) && ( lKey <= STAT_USER_PARAMS_LAST_KEY ) )
				{
					*(pbstrt + lParamsCount) = bstrParamsSection;
					*(plong + lParamsCount) = lKey;
					lParamsCount ++;
				}
			}

		}
		break;
	default:
		lParamsCount = 0;
	}
	
	if( lParamsCount > 0 )
	{
		if( ppbstrSections )
		{
			*ppbstrSections = new BSTR[ lParamsCount ];
			for( long l = 0; l < lParamsCount; l++ )
				*(*ppbstrSections + l) = (pbstrt +l)->Detach( );
		}

		if( pplKeys )
		{
			*pplKeys = new long[ lParamsCount ];
			for( long l = 0; l < lParamsCount; l++ )
				*(*pplKeys + l) = *(plong + l);
		}
	}

	if( pbstrt )
	{
		delete[] pbstrt;
		pbstrt = 0;
	}

	if( plong )
	{
		delete[] plong;
		plong = 0;
	}
	
	return lParamsCount;	// return size of sections array
}


class CDrawParam
{
	CString				m_sSection;
	IDrawExpressionPtr	m_iDrawPtr;
	BOOL				m_bStrExprExist;
	BOOL				m_bInit;
	long				m_lWorksMode;	// режим работы
        
public:
	CString				m_sErrorDescription;
	int					m_nErrorCode;
	CDrawParam(void);
	virtual ~CDrawParam(void);

	BOOL	Init( CMap<long, long&, CString, CString&> *pmapKeyToSection = 0 );
	BOOL	ReInit( CMap<long, long&, CString, CString&> *pmapKeyToSection = 0 );
	BOOL	SetBackColor( COLORREF clrBack );
	BOOL	SetForeColor( COLORREF clrFore );
	BOOL	DrawExpr( HDC hDC, RECT *pRect );

	void	SetWorksMode( long lNewMode );
	long	GetWorksMode( );

	void	GetSection( CString *pString );
	void	SetSection( const CString *pString );

	void	GetExpr( CString *pString );
	void	SetExpr( CString *pString, CMap<long, long&, CString, CString&> *pmapKeyToSection = 0 );



	void	GetUserName( CString *pString );
    //long	GetGreekSymbol( );

	long	GetHeight( HDC hDC = 0, BOOL bRecalc = FALSE );
	long	GetWidth( HDC hDC = 0, BOOL bRecalc = FALSE );
	void	GetExprRects(TCHAR** &str, RECT **pRects ,int* count);
	ICursorPosition* GetCursorInterface();
protected:
	BOOL	_init( BOOL *pbparsed_expr, CMap<long, long&, CString, CString&> *pmapKeyToSection = 0 );
};
