#include "StdAfx.h"
#include "calculatingmachine.h"
#include "resource.h"


CCalculatingMachine::CCalculatingMachine(void)
{
	m_pParser = new CParser( );
	m_crBackColor = 0;
	m_crForeColor = 0;
	ZeroMemory( &m_lfFont, sizeof( m_lfFont ) );
	m_bFontSet	=	false;
	m_bBkClrSet	=	false;
	m_bForeClrSet =	false;
	m_dwStateFlag = 0;
	m_nErrorCode = 0;
	m_bCursor =false;
}

CCalculatingMachine::~CCalculatingMachine(void)
{
	if( m_pParser )
		delete m_pParser;
}

IUnknown *CCalculatingMachine::DoGetInterface( const IID &iid )
{
	if( iid == IID_ICalcExpression )
		return (ICalcExpression *) this;
	if( iid == IID_IDrawExpression ) 
		return (IDrawExpression *) this;
	if( iid == IID_IDrawAttributes ) 
		return (IDrawAttributes *) this;
	if( iid == IID_ICursorPosition ) 
		return (ICursorPosition *) this;
	
	return ComObjectBase::DoGetInterface( iid );
}

// ICalcExpression
HRESULT	CCalculatingMachine::GetState( /*[out]*/ DWORD *pdwState )
{
	if( !pdwState )
		return S_FALSE;

	*pdwState = m_dwStateFlag;

	return S_OK;
}

HRESULT	CCalculatingMachine::Parse( /*[out]*/ BSTR *pbstrErrDesc )
{
	int iError = m_pParser->ParseExpr( );
	m_nErrorCode = iError;
	CString sError;
	sError.LoadString( IDS_UNKNOWNERROR + iError ); // load description from resorces
	*pbstrErrDesc = sError.AllocSysString( );
	if( iError != -1 )
		return S_FALSE;

	m_dwStateFlag |= osParsed;
	return S_OK;
}

HRESULT	CCalculatingMachine::SetString( /*[in]*/ BSTR bstrExpression )
{

	m_dwStateFlag &= ~osParsed;
	m_pParser->SetString( CString( bstrExpression ) );
	return S_OK;
}

HRESULT	CCalculatingMachine::GetString( /*[out]*/ BSTR *pbstrExpression )
{
	*pbstrExpression = m_pParser->GetString( ).AllocSysString( );
	return S_OK;
}

HRESULT	CCalculatingMachine::ReturnNecessaryCalcParams( /*[out]*/ BSTR *pbstrParamsNames, /*[out]*/ long *plSize )
{
	long lsize = m_pParser->GetNecessaryParamsNames( pbstrParamsNames, PT_SIMPLE, OM_CALCULATE );   // calculate mode;
	if( plSize )
		*plSize = lsize;

	if( *plSize != -1 )
		return S_OK;
	else
		return S_FALSE;
}

HRESULT	CCalculatingMachine::SetParamsValues( /*[in]*/ double *pfValues, /*[in]*/ long	lSize )
{
	if( m_pParser->SetParamsValues( pfValues, lSize) )
        return S_OK;
	else
		return S_FALSE;
}

HRESULT	CCalculatingMachine::ReturnNecessaryCalcArrayParams( /*[out]*/ BSTR *pbstrArrayParamsNames, /*[out]*/ long *plSize )
{
    long lsize = m_pParser->GetNecessaryParamsNames( pbstrArrayParamsNames, PT_ARRAY, OM_CALCULATE );   // calculate mode;
	if( plSize )
		*plSize = lsize;

	if( *plSize != -1 )
		return S_OK;
	else
		return S_FALSE;
}

HRESULT	CCalculatingMachine::SetArrayParamsValues( /*[in]*/ ArrayParamValues *pstValues, /*[in]*/ long	lSize )
{
	if( m_pParser->SetParamsVArrayalues( pstValues, lSize) )
        return S_OK;
	else
		return S_FALSE;
}

HRESULT	CCalculatingMachine::Calculate( /*[out]*/ double *pfResult, /*[out]*/ BSTR *pbstrErrDesc )
{
	int iError = m_pParser->Evaluate( *pfResult );
	m_nErrorCode = iError;
	CString sError;
	sError.LoadString( IDS_UNKNOWNERROR + iError ); // load description from resources
	*pbstrErrDesc = sError.AllocSysString( );
	if( iError == -1 )
		return S_OK;
	else
		return S_FALSE; 
}

// IDrawExpression
HRESULT	CCalculatingMachine::ReturnNecessaryDrawParams( /*[out]*/ BSTR *pbstrParamsNames, /*[out]*/ long *plSize )
{
	long lsize =  m_pParser->GetNecessaryParamsNames( pbstrParamsNames, PT_ALL, OM_PAINT );	// paint mode
	if( plSize )
		*plSize = lsize;

	if( *plSize != -1 )
		return S_OK;
	else
		return S_FALSE;
}

HRESULT	CCalculatingMachine::ReturnNecessaryDrawParamsEx( /*[out]*/ DrawParamInfo *pParamsInfos, /*[out]*/ long *plSize )
{
    long lsize =  m_pParser->GetNecessaryParamsNamesEx( pParamsInfos, PT_ALL, OM_PAINT );	// paint mode
	if( plSize )
		*plSize = lsize;

	if( *plSize != -1 )
		return S_OK;
	else
		return S_FALSE;    
}

HRESULT	CCalculatingMachine::SetParamsExInfo( /*[in]*/ ParamExInfo *pExInfo, /*[in]*/ long	lSize )
{
	if( m_pParser->SetParamsExInfo( pExInfo, lSize ) )
		return S_OK;
	else
		return S_FALSE;
}

HRESULT	CCalculatingMachine::Draw( /*[in]*/ DWORD hDC, /*[in][out]*/ RECT *pDrawRect,/*[in]*/ TextDrawOptions tFlags,/*[out]*/ BSTR *pbstrErrDesc )
{
	if( !( (HDC) hDC ) )
		return S_FALSE;

	HFONT		hFont = 0;
	COLORREF	crPrevBkColor = CLR_INVALID, crPrevForeColor = CLR_INVALID;
	
	/*if( !m_bFontSet )
	{
		HFONT	hf = (HFONT) ::GetCurrentObject( (HDC) hDC, OBJ_FONT );
		if( hf )
		{
			::GetObject( hf, sizeof( LOGFONT ) , &m_lfFont );
			m_bFontSet = true;
		}
		hf = 0;
	}
	if( !m_bBkClrSet )
		SetColor( ::GetBkColor( (HDC) hDC ), ecBackColor );
	if( !m_bForeClrSet )
		SetColor( ::GetTextColor( (HDC) hDC ), ecForeColor );*/

	if( m_bFontSet )
	{
		hFont = ::CreateFontIndirect( &m_lfFont );
		if( !hFont )
			return S_FALSE;

		hFont = (HFONT) ::SelectObject( (HDC) hDC, (HFONT) hFont );
	}

	if( m_bBkClrSet )
		crPrevBkColor = ::SetBkColor( (HDC) hDC, m_crBackColor );	
	if( m_bForeClrSet )
		crPrevForeColor = ::SetTextColor( (HDC) hDC, m_crForeColor );
	
    int iError = m_pParser->Draw( (HDC) hDC, pDrawRect, tFlags );
	m_nErrorCode = iError;
	if( crPrevBkColor != CLR_INVALID )
	{
		::SetBkColor( (HDC) hDC, crPrevBkColor );
		crPrevBkColor = CLR_INVALID;
	}
	if( crPrevForeColor != CLR_INVALID )
	{
		::SetBkColor( (HDC) hDC, crPrevForeColor );
		crPrevForeColor = CLR_INVALID;
	}
	if( hFont )
		::DeleteObject( ::SelectObject( (HDC) hDC, hFont ) );

	CString sError;
	if( iError == -2 ) // inner error
		sError = _T("!inner error!");
	else
		sError.LoadString( IDS_UNKNOWNERROR + iError ); // load description from resources
	*pbstrErrDesc = sError.AllocSysString( );
	return S_OK;
}

HRESULT CCalculatingMachine::Redraw( /*[in]*/ DWORD hDC )
{
   if( !( (HDC) hDC ) )
       return S_FALSE;

   return m_pParser->Redraw( (HDC) hDC ) ? S_OK : S_FALSE;
}

HRESULT CCalculatingMachine::GetRectangle( /*[out]*/ RECT *pRect )
{
	m_pParser->GetExprRect( pRect );
	return S_OK;
}

// IDrawAttributes
HRESULT CCalculatingMachine::SetColor( /*[in]*/ DWORD dwColor, /*[in]*/ ExprColors eFlags )
{
	switch( eFlags )
	{
	case ecForeColor:	
		m_crForeColor = dwColor;
		m_bForeClrSet = true;
		break;
	case ecBackColor:
        m_crBackColor = dwColor;
		m_bBkClrSet = true;
		break;
	default:
		return S_FALSE;
	}
	return S_OK;
}

HRESULT CCalculatingMachine::GetColor( /*[out]*/ DWORD *pdwColor, /*[in]*/ ExprColors eFlags )
{
	if( !pdwColor )
		return S_FALSE;

	switch( eFlags )
	{
	case ecForeColor:
		*pdwColor = m_crForeColor;
		break;
	case ecBackColor:
		*pdwColor = m_crBackColor;
		break;
	default:
		return S_FALSE;
	}
	return S_OK;
}

HRESULT CCalculatingMachine::SetFont( /*[in]*/ LOGFONT lf )
{
	m_lfFont = lf;
	m_bFontSet = true;
	return S_OK;
}

HRESULT CCalculatingMachine::GetFont( /*[out]*/ LOGFONT *plf )
{
	if( !plf )
		return S_FALSE;
	*plf = m_lfFont;
	return S_OK;
}

HRESULT CCalculatingMachine::GetLastErrorCode(/*[out]*/int* code)
{	
	*code = m_nErrorCode+1;
	return S_OK;
}



HRESULT	CCalculatingMachine::MoveCursor(IN HDC dc, IN MovingDirection dir, OUT RECT* rect)
{
	*rect = m_pParser->MoveCursor(dc, dir);
	return S_OK;
}
HRESULT	CCalculatingMachine::GetCursorCoordinates(IN HDC dc, OUT RECT* rect)
{
	*rect = m_pParser->GetCursorCoordinates(dc);
	return S_OK;
}
HRESULT	CCalculatingMachine::SetCoordinates(IN RECT rectDesired, OUT RECT* rectFixed)
{
	*rectFixed = m_pParser->SetCoordinates(rectDesired);
	return S_OK;
}
HRESULT	CCalculatingMachine::GetDividedStrings(BSTR* bstrBegin, BSTR* bstrEnd)
{
	CString sb, se;
	m_pParser->GetDividedStrings(sb, se);
	*bstrBegin = sb.AllocSysString();
	*bstrEnd = se.AllocSysString();
	return S_OK;
}
HRESULT	CCalculatingMachine::ActivateCursor()
{
	m_bCursor =true;
	m_pParser->ActivateCursor();
	return S_OK;
}
HRESULT	CCalculatingMachine::DeleteEntry(BSTR* bstrNew)
{
	CString s;
	m_pParser->DeleteEntry(s);
	*bstrNew = s.AllocSysString();
	return S_OK;
}

