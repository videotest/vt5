#pragma once

#include "\vt5\common2\com_unknown.h"
#include "calc_int.h"	
#include "Parser.h"

class CCalculatingMachine :
	public ComObjectBase,
	public ICalcExpression,
	public IDrawExpression,
	public IDrawAttributes,
	public ICursorPosition
{
	CParser		*m_pParser;

	LOGFONT		m_lfFont;
	COLORREF	m_crForeColor;	
	COLORREF	m_crBackColor;	
	bool		m_bFontSet;
	bool		m_bBkClrSet;
	bool		m_bForeClrSet;
	DWORD		m_dwStateFlag;
	int			m_nErrorCode;
	bool m_bCursor;
public:
	CCalculatingMachine(void);
	virtual ~CCalculatingMachine(void);

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

public:
	// ICalcExpression   &	IDrawExpression
	com_call	GetState( /*[out]*/ DWORD *pdwState );
	com_call	SetString( /*[in]*/ BSTR bstrExpression );
	com_call	GetString( /*[out]*/ BSTR *pbstrExpression );
	com_call	Parse( /*[out]*/ BSTR *pbstrErrDesc );
	
	// ICalcExpression 
	com_call	ReturnNecessaryCalcParams( /*[out]*/ BSTR *pbstrParamsNames, /*[out]*/ long *plSize );
	com_call	SetParamsValues( /*[in]*/ double *pfValues, /*[in]*/ long	lSize );
	com_call	ReturnNecessaryCalcArrayParams( /*[out]*/ BSTR *pbstrArrayParamsNames, /*[out]*/ long *plSize );
	com_call	SetArrayParamsValues( /*[in]*/ ArrayParamValues *pstValues, /*[in]*/ long	lSize );
	com_call	Calculate( /*[out]*/ double *pfResult, /*[out]*/ BSTR *pbstrErrDesc );

	// IDrawExpression
	com_call	ReturnNecessaryDrawParams( /*[out]*/ BSTR *pbstrParamsNames, /*[out]*/ long *plSize );
	com_call	ReturnNecessaryDrawParamsEx( /*[out]*/ DrawParamInfo *pParamsInfos, /*[out]*/ long *plSize );
	com_call	SetParamsExInfo( /*[in]*/ ParamExInfo *pExInfo, /*[in]*/ long	lSize );
	com_call	Draw( /*[in]*/ DWORD hDC, /*[in][out]*/ RECT *pDrawRect,/*[in]*/ TextDrawOptions tFlags,/*[out]*/ BSTR *pbstrErrDesc );
	com_call	Redraw( /*[in]*/ DWORD hDC );
	com_call	GetRectangle( /*[out]*/ RECT *pRect );
	com_call	GetLastErrorCode(/*[out]*/int* code);


	// IDrawAttributes
	com_call	SetColor( /*[in]*/ DWORD dwColor, ExprColors eFlags );
	com_call	GetColor( /*[out]*/ DWORD *pdwColor, ExprColors eFlags );
	com_call	SetFont( /*[in]*/ LOGFONT lf );
	com_call	GetFont( /*[out]*/ LOGFONT *plf );

	//ICursorPosition
	com_call	MoveCursor(IN HDC dc, IN MovingDirection dir, OUT RECT* rect);
	com_call	GetCursorCoordinates(IN HDC dc, OUT RECT* rect);
	com_call	SetCoordinates(IN RECT rectDesired, OUT RECT* rectFixed);
	com_call	GetDividedStrings(BSTR* bstrBegin, BSTR* bstrEnd);
	com_call	ActivateCursor();
	com_call	DeleteEntry(BSTR* bstrNew);
};


