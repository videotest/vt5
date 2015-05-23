#pragma once

struct ParamExInfo		// param's extension info
{  
	char	*psGraphics;	// param caption 
	char	*psUpIndex;		// upper index
	char	*psLoIndex;		// lower index
	long	lGreekSmb;		// code of greek symbol

	ParamExInfo( ){	init();	}

	void init()
	{
		psGraphics = 0;
		psUpIndex = 0;
		psLoIndex = 0;
		lGreekSmb = -1;
	}

	void free()
	{
		if( psGraphics )
		{
			delete[] psGraphics;
			psGraphics = 0;
		}
		if( psUpIndex )
		{
			delete[] psUpIndex;
			psUpIndex = 0;
		}
		if( psLoIndex )
		{
			delete[] psLoIndex;
			psLoIndex = 0;
		}
		lGreekSmb = -1;
	}
};

// Значениея параметра множества
struct ArrayParamValues
{
	double *pfvalues;	// массив значений
	long	lsize;		// размерность массива значений
};

enum ObjectState
{
	osParsed = 1 << 0,
};

interface ICalcExpression : public IUnknown
{
	com_call	GetState( /*[out]*/ DWORD *pdwState ) = 0;

	com_call	SetString( /*[in]*/ BSTR bstrExpression ) = 0;
	com_call	GetString( /*[out]*/ BSTR *pbstrExpression ) = 0;
	com_call	Parse( /*[out]*/ BSTR *pbstrErrDesc ) = 0;
	
	// simple params
	com_call	ReturnNecessaryCalcParams( /*[out]*/ BSTR *pbstrParamsNames, /*[out]*/ long *plSize ) = 0;
	com_call	SetParamsValues( /*[in]*/ double *pfValues, /*[in]*/ long	lSize ) = 0;
	// array params
	com_call	ReturnNecessaryCalcArrayParams( /*[out]*/ BSTR *pbstrArrayParamsNames, /*[out]*/ long *plSize ) = 0;
	com_call	SetArrayParamsValues( /*[in]*/ ArrayParamValues *pstValues, /*[in]*/ long	lSize ) = 0;

	com_call	Calculate( /*[out]*/ double *pfResult, /*[out]*/ BSTR *pbstrErrDesc ) = 0;
};

enum	TextDrawOptions
{
	tdoPaint = 1,
	tdoCalcRect
};

struct DrawParamInfo
{
	BSTR	bstrParamName;
    BOOL	bIsLValue;		// слева от =

	DrawParamInfo() { init(); }
    
	void init(void)
	{
		bstrParamName = 0;
		bIsLValue = FALSE;
	}

	void free(void)
	{
		if( bstrParamName )
			::SysFreeString( bstrParamName ); bstrParamName = 0;
		bIsLValue = FALSE;
	}
};

interface IDrawExpression : public IUnknown
{
	com_call	SetString( /*[in]*/ BSTR bstrExpression ) = 0;
	com_call	GetString( /*[out]*/ BSTR *pbstrExpression ) = 0;
	com_call	Parse( /*[out]*/ BSTR *pbstrErrDesc ) = 0;
			
	com_call	ReturnNecessaryDrawParams( /*[out]*/ BSTR *pbstrParamsNames, /*[out]*/ long *plSize ) = 0;
	com_call	ReturnNecessaryDrawParamsEx( /*[out]*/ DrawParamInfo *pParamsInfos, /*[out]*/ long *plSize ) = 0;
	com_call	SetParamsExInfo( /*[in]*/ ParamExInfo *pExInfo, /*[in]*/ long	lSize ) = 0;
	com_call	Draw( /*[in]*/ DWORD hDC, /*[in][out]*/ RECT *pDrawRect,/*[in]*/ TextDrawOptions tFlags,/*[out]*/ BSTR *pbstrErrDesc ) = 0;
	com_call	Redraw( /*[in]*/ DWORD hDC ) = 0;
	com_call	GetRectangle( /*[out]*/ RECT *pRect ) = 0;
	com_call	GetLastErrorCode(int* code) = 0;


};										

enum	ExprColors
{ 
	ecForeColor = 1,
	ecBackColor 
};

interface IDrawAttributes : public IUnknown
{
	com_call	SetColor( /*[in]*/ DWORD dwColor, /*[in]*/ ExprColors eFlags ) = 0;
	com_call	GetColor( /*[out]*/ DWORD *pdwColor, /*[in]*/ ExprColors eFlags ) = 0;
	com_call	SetFont( /*[in]*/ LOGFONT lf ) = 0;
	com_call	GetFont( /*[out]*/ LOGFONT *plf ) = 0;
};
enum	MovingDirection
{
	mdNone = 0,
	mdLeft,
	mdRight,
	mdBegin,
	mdEnd,
	mdReset
};

interface ICursorPosition : public IUnknown
{
	com_call	MoveCursor(IN HDC dc, IN MovingDirection dir, OUT RECT* rect) =0;
	com_call	GetCursorCoordinates(IN HDC dc,OUT RECT* rect) =0;
	com_call	SetCoordinates(IN RECT rectDesired, OUT RECT* rectFixed) =0;
	com_call	GetDividedStrings(BSTR* bstrBegin, BSTR* bstrEnd) =0;
	com_call	ActivateCursor()=0;
	com_call	DeleteEntry(OUT BSTR* bstrNew)=0;
};

declare_interface( ICalcExpression, "1BABEDCA-67A7-42ec-BACD-FEB1DC6C2BC3" );
declare_interface( IDrawExpression, "CBF77F6D-38BF-426d-B94F-985FCFA0E444" );
declare_interface( IDrawAttributes, "9E2D41E0-F300-43bf-AFEF-784B2551133E" );
declare_interface( ICursorPosition, "02F785EB-74DD-470f-A2D2-297BBD535848" );