#include "StdAfx.h"
#include "drawparam.h"
#include "Params.h"
#include "works_modes.h"
#include "\vt5\common2\class_utils.h"
#include "resource.h"
#include "statui_int.h"

// [vanek] - 26.03.2005
static CString LoadString(LPCTSTR sz_id, LPCTSTR sz_sect="Strings") 
{
	CString strResult( _T("") );

	CString str_id( sz_id );
	str_id.TrimLeft();
	str_id.TrimRight();
	if( !str_id.IsEmpty() )
	{
		char	sz_string_file[MAX_PATH];
		GetModuleFileName( 0, sz_string_file, sizeof( sz_string_file ) );
		strcpy( strrchr( sz_string_file, '\\' )+1, "resource." );

		CString	strLang = GetValueString( GetAppUnknown(), "\\General", "Language", "en" );
		strcat( sz_string_file, strLang );
		
		char	*pbuffer = strResult.GetBuffer( 4096 );
		char sz_default[] = "";
		::GetPrivateProfileString( sz_sect, sz_id, sz_default, pbuffer, 4096, sz_string_file );
		if(0==*pbuffer)
		{
			strcpy(pbuffer, sz_id);
			::WritePrivateProfileString( sz_sect, sz_id, pbuffer, sz_string_file );
		}

		char	*pout = pbuffer;

		while( *pbuffer )
		{
			if( *pbuffer == '\\' )
			{
				pbuffer++;
				if( *pbuffer == 'n' || *pbuffer == 'N' )
					*pbuffer = '\n';
				if( *pbuffer == 'r' || *pbuffer == 'R' )
					*pbuffer = '\r';
			}
			*pout = *pbuffer;
			pout++;pbuffer++;
		}
		*pout = 0;

		strResult.ReleaseBuffer();
	}

	return strResult;
}


CDrawParam::CDrawParam(void)
{
	IUnknown *punk = 0;
	if( S_OK == ::CoCreateInstance( clsidCalculatingMachine, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk ) )
		m_iDrawPtr = punk;
	else
		m_iDrawPtr = 0;
	
	if( punk )
	{
		punk->Release( );
		punk = 0;	
	}

	m_bStrExprExist = FALSE;
	m_bInit = FALSE;
	m_lWorksMode = MIN_WORKSMODE;
	if(m_iDrawPtr ==0) m_nErrorCode = -1;
	else m_nErrorCode = 0;
}

CDrawParam::~CDrawParam(void)
{
	if(	m_iDrawPtr )
		m_iDrawPtr = 0;
}

BOOL	CDrawParam::Init( CMap<long, long&, CString, CString&> *pmapKeyToSection )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bparsed = FALSE;
	if( !_init( &bparsed, pmapKeyToSection ) )
	{
		// [vanek] SBT:304 - если выражение разобрано, но инициализация не прошла - 03.06.2004
        if( bparsed )
		{
            m_sErrorDescription.LoadString( IDS_ERR_INIT_EXPR );
			m_nErrorCode = 30;
		}

		return FALSE;
	}
	/*m_nErrorCode = 0;*/
    return TRUE;    
}

BOOL	CDrawParam::ReInit( CMap<long, long&, CString, CString&> *pmapKeyToSection )
{
	m_bInit = FALSE;
	return Init( pmapKeyToSection );
}

BOOL	CDrawParam::SetBackColor( COLORREF clrBack )
{
	if( m_iDrawPtr == 0 )
		return FALSE;

	IDrawAttributesPtr iDrawAttrPtr = m_iDrawPtr;

	if( iDrawAttrPtr )
	{
		iDrawAttrPtr->SetColor( clrBack, ecBackColor  );
		iDrawAttrPtr = 0;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	CDrawParam::SetForeColor( COLORREF clrFore )
{
	if( m_iDrawPtr == 0 )
		return FALSE;

	IDrawAttributesPtr iDrawAttrPtr = m_iDrawPtr;

	if( iDrawAttrPtr )
	{
		iDrawAttrPtr->SetColor( clrFore, ecForeColor  );
		iDrawAttrPtr = 0;
		return TRUE;
	}
	else
		return FALSE;
}


BOOL	CDrawParam::DrawExpr( HDC hDC, RECT *pRect )
{
    if( m_iDrawPtr == 0 )
		return FALSE;

	BSTR bstr_error = 0;
	BOOL bRes = (S_OK ==  m_iDrawPtr->Draw( (DWORD) hDC, pRect, tdoPaint, &bstr_error ) );

	// [vanek]: не запоминаем ошибку при отрисовке выражения, т.к. она может "затереть" ошибку при разборе выражения
	//if( bRes )
	//	m_sErrorDescription = bstr_error;
		
	if( bstr_error )
	{
		::SysFreeString( bstr_error );
		bstr_error = 0;
	}

    return bRes;	
}

void	CDrawParam::SetWorksMode( long lNewMode )
{
	if( IS_WORKSMODE( lNewMode ) && ( m_lWorksMode != lNewMode ) )
		m_lWorksMode = lNewMode;
}

long	CDrawParam::GetWorksMode( )
{
	return m_lWorksMode;
}


void	CDrawParam::GetSection( CString *pString )
{
	if( pString )
		*pString = m_sSection;
}

void	CDrawParam::SetSection( const CString *pString )
{
	if( pString )
		m_sSection = *pString;
}

void	CDrawParam::GetExpr( CString *pString )
{
	if( !pString || (m_iDrawPtr == 0) )
		return;
	BSTR bstr = 0;
	if( S_OK != m_iDrawPtr->GetString( &bstr ) )
		return;
	if( bstr )
	{
		*pString = bstr;
		::SysFreeString( bstr );
		bstr = 0;
	}
}

void	CDrawParam::SetExpr( CString *pString, CMap<long, long&, CString, CString&> *pmapKeyToSection )
{
	if( !pString || (m_iDrawPtr == 0) )
		return;
	
	BSTR bstr = pString->AllocSysString( );
	if( S_OK == m_iDrawPtr->SetString( bstr ) )
		m_bStrExprExist = TRUE;
	if( bstr )
	{
		::SysFreeString( bstr );
		bstr = 0;
	}
	ReInit( pmapKeyToSection );
}

void	CDrawParam::GetUserName( CString *pString )
{
	if( pString )
		*pString = GetValueStringWithoutSet( ::GetAppUnknown( ), m_sSection, sUserName, "" );
}

long	CDrawParam::GetHeight( HDC hDC, BOOL bRecalc /*= FALSE*/ )
{
	if( m_iDrawPtr == 0 )
		return 0;

	RECT rect = {0};

	if( S_OK != m_iDrawPtr->GetRectangle( &rect ) )
		return 0;

	if( (::IsRectEmpty( &rect ) && hDC) || bRecalc )
	{
		BSTR bstr_error = 0;
		if( S_OK != m_iDrawPtr->Draw( (DWORD) hDC, &rect, tdoCalcRect, &bstr_error ) )
			return 0;
		if( bstr_error )
		{
			m_iDrawPtr->GetLastErrorCode(&m_nErrorCode);
			m_sErrorDescription = bstr_error;
			::SysFreeString( bstr_error );
			bstr_error = 0;
		}
	}

	return (rect.bottom - rect.top);
}

long	CDrawParam::GetWidth( HDC hDC, BOOL bRecalc /*= FALSE*/  )
{
	if( m_iDrawPtr == 0 )
		return 0;

	RECT rect = {0};

	if( S_OK != m_iDrawPtr->GetRectangle( &rect ) )
		return 0;

	if( (::IsRectEmpty( &rect ) && hDC) || bRecalc )
	{
		BSTR bstr_error = 0;
		if( S_OK != m_iDrawPtr->Draw( (DWORD) hDC, &rect, tdoCalcRect, &bstr_error ) )
			return 0;
		if( bstr_error )
		{
			m_iDrawPtr->GetLastErrorCode(&m_nErrorCode);
			m_sErrorDescription = bstr_error;
			::SysFreeString( bstr_error );
			bstr_error = 0;
		}
	}

	return (rect.right - rect.left);
}

BOOL	CDrawParam::_init( BOOL *pbparsed_expr, CMap<long, long&, CString, CString&> *pmapKeyToSection /*= 0*/ )
{
	if( m_bInit )
		return TRUE;

	if( m_iDrawPtr == 0 )
		return FALSE;

	CString stMainSection;			   
	switch( m_lWorksMode )
	{
	case WKM_MEASUSER:
		stMainSection = _T( sMainMeasParamsSection );
		break;
	case WKM_STATUSER:
		stMainSection = _T( sMainStatParamsSection );
		break;
	default:
		stMainSection.Empty( );
	}

	if( !m_sSection.IsEmpty( ) && !stMainSection.IsEmpty( ) )
	{
        // [vanek] SBT:334,282 проверка соответсвия секции текущему режиму работы - 08.06.2004
        if( -1 == m_sSection.Find( stMainSection ) )
			return FALSE;
	}

	BSTR bstr = 0;
	if( !m_bStrExprExist )
	{
		CString strExpr;
		strExpr = GetValueStringWithoutSet( ::GetAppUnknown( ), m_sSection, sExprString, "" );
		if(	strExpr.Left(1) == "#" )
		{ // если начинается с # - строка закодирована
			strExpr.Delete(0,1);
			// раскодирование
			//int n = strExpr.GetLength()/2;
			//CString str;
			//const char* sz = strExpr;
			//for(int i=0; i<n; i++)
			//{
			//	char c = sz[i*2];
			//	int n1 = ( c + (c>'9' ? 10-'A' : 0-'0') ) & 15;
			//	c = sz[i*2+1];
			//	int n2 = ( c + (c>'9' ? 10-'A' : 0-'0') ) & 15;
			//	int n = n1*16 + n2;
			//	n ^= 0x55;
			//	str += char(n);
			//}
			//strExpr = str;
		}
		if( strExpr.IsEmpty( ) )
			return FALSE;
	
		bstr = strExpr.AllocSysString( );
		if( S_OK != m_iDrawPtr->SetString( bstr ))
			return FALSE;
		if( bstr )
		{
			::SysFreeString( bstr );
			bstr = 0;
		}
	}
	/*else
	{
		if( S_OK != m_iDrawPtr->GetString( &bstr ) )
			return FALSE;

		::SysFreeString( bstr );
		bstr = 0;
	}  */


	// parse expression
	bstr = 0;
    HRESULT hr = S_FALSE;
	hr = m_iDrawPtr->Parse( &bstr );
	if( pbparsed_expr )
		*pbparsed_expr = (hr == S_OK);
	m_iDrawPtr->GetLastErrorCode(&m_nErrorCode);
	m_sErrorDescription = bstr;
	if( bstr )
		::SysFreeString( bstr ); bstr = 0;
	
	// get draw infos
	DrawParamInfo *pDrawParamsInfos = 0;
	long lSize = 0;
	if( S_OK != m_iDrawPtr->ReturnNecessaryDrawParamsEx( pDrawParamsInfos, &lSize ) )
		return FALSE;

	// init infos for drawing
	BOOL binit_failed = FALSE;
	pDrawParamsInfos = new DrawParamInfo[ lSize ];
    if( S_OK != m_iDrawPtr->ReturnNecessaryDrawParamsEx( pDrawParamsInfos, &lSize ) )
		binit_failed = TRUE;

	if( lSize && !binit_failed )
	{
		ParamExInfo *pExInfo = new ParamExInfo[ lSize ];
		for( long l = 0; l < lSize; l++ )
		{
			CString stSection( _T("") ), 
					stParam( _T("") ),
					stClassNumber( _T("") ),
					stClassName( _T("") ); // имя класса статистического параметра

			// [vanek] - 26.03.2005
            pExInfo[ l ].init( );

			long lKey = 0;
			stParam = pDrawParamsInfos[ l ].bstrParamName;
            if( m_lWorksMode == WKM_STATUSER )
			{	// статистичекий параметр
                int iPos = 0;
				stClassNumber = stParam;
				stParam = stClassNumber.Tokenize( _T(":"), iPos );
				stClassNumber.Delete( 0, iPos );
			}
			
			// [vanek] SBT:304 - проверка на корректность номера параметра - 03.06.2004
			TCHAR *ptstr_stopped = 0;
			lKey = _tcstol( stParam, &ptstr_stopped, 10 );
			if( _tcslen( ptstr_stopped ) )
			{   // строка stParam содержит не только номер параметра
                binit_failed = TRUE;
				break;
			}
            
			// [vanek] - 26.03.2005
			bool bparam_section_exist = true;

			if( pmapKeyToSection )
			{
				if( !pmapKeyToSection->Lookup( lKey, stSection) )
				{
					if( search_section_by_key( m_lWorksMode, lKey, &stSection ) )
						pmapKeyToSection->SetAt( lKey, stSection );
					else
					{
						// [vanek] - 26.03.2005
                        //binit_failed = TRUE;
						//break;
						bparam_section_exist = false;
					}
				}
			}
			else if( !search_section_by_key( m_lWorksMode, lKey, &stSection ) )
			{
				// [vanek] - 26.03.2005
				//binit_failed = TRUE;
				//break;
				bparam_section_exist = false;
			}
			
			if( stSection.IsEmpty( ) )
			{
				// [vanek] - 26.03.2005
				//binit_failed = TRUE;
				//break;
				bparam_section_exist = false;
			}

			// [vanek] - 26.03.2005
			if( bparam_section_exist )
			{
				if( ( m_lWorksMode == WKM_STATUSER ) )
				{   // статистичекий параметр

					int iClassNumber = -2;
					stClassName.Empty( );

					if( stClassNumber.IsEmpty() )				
					{
						// [vanek]: SBT:303 - 04.06.2004
						if( pDrawParamsInfos[ l ].bIsLValue )
							iClassNumber = (int)GetValueIntWithoutSet( ::GetAppUnknown(), stSection, CALC_METHOD, iClassNumber );
					}
					else
					{
						// [vanek] SBT:304 - проверка на корректность номера класса параметра - 03.06.2004
						TCHAR *ptstr_stopped = 0;
						iClassNumber = (int)( _tcstol( stClassNumber , &ptstr_stopped, 10 ) );
						if( _tcslen(ptstr_stopped) )
						{
							binit_failed = TRUE;
							break;
						}
					}

					if( iClassNumber < 0 )
					{
						if( iClassNumber == -1  )
							// [vanek] - 26.03.2005
							//stClassName.LoadString( IDS_STATPARAMS_CLASSNAME_TOTAL );
							stClassName = LoadString( _T("Total") );
					}
					else
					{
						CString stClassifierName( _T("") );
						stClassifierName = ::GetValueStringWithoutSet( ::GetAppUnknown( ), stSection, sClassifier, "" );

						stClassName = get_class_name( iClassNumber, stClassifierName.GetBuffer( stClassifierName.GetLength() ) );
						stClassifierName.ReleaseBuffer( );
					}

					if( !stClassName.IsEmpty() )
						stClassName = _T("(") + stClassName + _T(")");
				}

				CString value;
				char *pvalue = 0;
				
				value = GetValueStringWithoutSet( ::GetAppUnknown( ), stSection, sGraphics, "" );
				if( 0 < value.GetLength( ) )
				{
					pExInfo[ l ].psGraphics = new char[ value.GetLength( ) + 1 ];
					pvalue = value.GetBuffer( value.GetLength( ) ); 
					strcpy( pExInfo[ l ].psGraphics, pvalue );
					value.ReleaseBuffer( );
					pvalue = 0;
				}
				else
					pExInfo[ l ].psGraphics = 0;

				value = GetValueStringWithoutSet( ::GetAppUnknown( ), stSection, sUpperIndex, "" );
				if( 0 < value.GetLength( ) )
				{
					pExInfo[ l ].psUpIndex = new char[ value.GetLength( ) + 1 ];
					pvalue = value.GetBuffer( value.GetLength( ) ); 
					strcpy( pExInfo[ l ].psUpIndex, pvalue );
					value.ReleaseBuffer( );
					pvalue = 0;
				}
				else
					pExInfo[ l ].psUpIndex = 0;
	            
				pvalue = 0;
				value = GetValueStringWithoutSet( ::GetAppUnknown( ), stSection, sLowerIndex, "" );
				if( ( 0 < value.GetLength( ) ) || ( 0 < stClassName.GetLength( ) ) )	
				{
					value += stClassName;
					pExInfo[ l ].psLoIndex = new char[ value.GetLength( ) + 1 ];
					pvalue = value.GetBuffer( value.GetLength( ) ); 
					strcpy( pExInfo[ l ].psLoIndex, pvalue );
					value.ReleaseBuffer( );
					pvalue = 0;
				}
				else
					pExInfo[ l ].psLoIndex = 0;
				
				pExInfo[ l ].lGreekSmb = GetValueIntWithoutSet( ::GetAppUnknown( ), stSection, sGreekSymbol, -1 );
			}
			else
			{
				CString str_empty;
				str_empty = LoadString( _T("Empty") );
				str_empty = _T("{") + str_empty + _T("}");
				              
				
				pExInfo[ l ].psGraphics = new char[ str_empty.GetLength( ) + 1 ];
                strcpy( pExInfo[ l ].psGraphics, str_empty.GetBuffer( str_empty.GetLength( ) ) );				
				str_empty.ReleaseBuffer( );
			}
		}
		
		if( !binit_failed )
			m_iDrawPtr->SetParamsExInfo( pExInfo, lSize );
	
		for( long l = 0; l < lSize; l++ )
		{
			pExInfo[ l ].free( );
			pDrawParamsInfos[ l ].free( );
		}
		
		delete[] pExInfo;
		pExInfo = 0;
	}

	if( pDrawParamsInfos )
	{
		delete[] pDrawParamsInfos;
		pDrawParamsInfos = 0;
	}

	return m_bInit = !binit_failed ;	        
}

ICursorPosition* CDrawParam::GetCursorInterface()
{
	ICursorPosition* ci=0;
	if(m_iDrawPtr!=0) m_iDrawPtr->QueryInterface(IID_ICursorPosition, (void**) &ci);
	return ci;
}

//void CDrawParam::GetExprRects(TCHAR** &str, RECT **pRects ,int* count)
//{
//	m_iDrawPtr->GetExprRects(str, pRects ,count);
//}