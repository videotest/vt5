#include "StdAfx.h"
#include "Parser.h"
#include <float.h>
#include "misc_str.h"



CParser::CParser(void)
{
	m_iCurrIndex = -1;
	m_iTokeType = 0;
	m_iErrorCode = -1;
	m_iMode = OM_RELAX;		// режим работы по-умолчанию - отдыхаем
	m_pEntry = 0;
	m_hdc = 0;
	::SetRectEmpty( &m_DrawRect );

	m_lArrayValueIndex = -1;
	m_bWaitArrayParam = false;
	m_lArrayLength = INIT_ARRAY_LENGTH;
	m_bErrorEvalArrayFunc = FALSE;
    
	m_bSubExprProcessing = false;
	m_ptCursorPos.x = -1;
	m_ptCursorPos.y =-1;
	m_bReset = true;
	m_bCursor =false;
	m_pCursorOwner =0;
}

CParser::~CParser(void)
{
	free( );
	m_sExpression.Empty( );
	clear_dummi_stack();
	
}

void	CParser::SetString( CString str )
{
	m_iMode = OM_RELAX;
	m_bNewData = TRUE;
	if( str.IsEmpty( ) )
		return;
	m_sExpression = str;
}
RECT CParser::MoveCursor(HDC dc, MovingDirection dir)
{
	if(dir == mdReset) {
		m_bReset = true;
		init_cursor();
		return MoveCursor(dc, mdEnd);
	}
	m_hdc = dc;

	if(m_ExprElems.size()!=0 && m_SortedExprElems.size()==0) 
		init_cursor();
	XExprEntry** entry = m_SortedExprElems.ptr();
	int l = m_SortedExprElems.size();
	RECT r ={0};
	if(l==0) return r;
	m_pLastEntry = entry[l-1];
	m_pCursorOwner = entry[m_ptCursorPos.x];

	switch(dir)
	{
	case mdLeft:
		{
			if(m_pCursorOwner==*entry)//позиция курсора не изменяется 
			{
				m_pCursorOwner->get_rect(&m_rectCursor);
				m_rectCursor.right = m_rectCursor.left;
				m_rectCursor.left =0;
				return m_rectCursor;
			}
			else if(m_pCursorOwner->GetType()== ET_LBRACKET)
			{							
				if(entry[m_ptCursorPos.x-1]->GetType() == ET_FUNCTION) 
				{
					(m_pCursorOwner)->move_cursor(dir);
				}	
			}
		}
		break;
	case mdRight:
		{
			if(m_pCursorOwner==m_pLastEntry) //позиция курсора не изменяется 
			{
				m_ptCursorPos.y++;
				m_ptCursorPos.y = m_pCursorOwner->set_cursor_offset(m_ptCursorPos.y);
				m_pCursorOwner->get_rect(&m_rectCursor);

				return m_rectCursor;
			}
			else if (entry[m_ptCursorPos.x+1]->GetType()== ET_FUNCTION)
			{
				if(m_ptCursorPos.x+2<l) 
				{
					XExprEntry* e = entry[m_ptCursorPos.x+2];
					if(e->GetType() == ET_LBRACKET) 
					{
						m_rectCursor = m_pCursorOwner->move_cursor(dir);
						if(m_pCursorOwner->GetType()== ET_CONST) 
							return m_rectCursor;
						/*MoveCursor(dc, dir);*/
					}
				}
			}
		}
		break;
	case mdEnd:
		{
			{
				m_pCursorOwner = m_pLastEntry;
				m_ptCursorPos.y = m_pCursorOwner->set_cursor_offset(-1);
				m_pCursorOwner->get_rect(&m_rectCursor);
				m_ptCursorPos.x = m_SortedExprElems.size()-1;

				return m_rectCursor;
			}
		}
		break;
	case mdBegin:
		{
			int m=0;
			while(!strcmp((*entry)->get_graphics(),"="))
			{
				m++;
				entry++;
			}
			entry++;
			m_pCursorOwner = *entry;
			(*entry)->get_rect(&m_rectCursor);
			m_ptCursorPos.x =m+1;
			return m_rectCursor;
		}		
	}
	m_rectCursor = (m_pCursorOwner)->move_cursor(dir);

	m_hdc = 0;

	return m_rectCursor;
}
RECT CParser::GetCursorCoordinates(HDC dc)
{
	m_hdc = dc;
	RECT r ={0};
	if(m_pCursorOwner) m_pCursorOwner->get_rect(&r);
	m_hdc =0;
	return r;
}
RECT CParser::SetCoordinates(RECT rectDesired)
{
	XExprEntry** xe = m_SortedExprElems.ptr();
	RECT rect ={0};
	for(int i=0;i<m_SortedExprElems.size();i++)
	{
		
		xe[i]->get_rect(&rect);
		if(rect.right>= rectDesired.right) 
			break;
	}
	if(rect.right<rectDesired.right) 
		m_rectCursor = rect;
	else
		m_rectCursor = xe[i]->set_cursor_rect(rectDesired);
	return m_rectCursor;
}
void CParser::DeleteEntry(CString& s)
{
	s.Empty();
	if(m_pCursorOwner->get_graphics()== "=" || m_ptCursorPos.x<=0)
	{
		s = m_sExpression;
		return;
	}
	m_pCursorOwner->delete_entry();
	int n = m_SortedExprElems.size();
    for(int i=0;i<n;i++)
	{
		s+=(m_SortedExprElems.ptr()[i])->get_graphics();
	}
}

void CParser::GetDividedStrings(CString& strBegin, CString& strEnd)
{
	if(m_ExprElems.size()!=0 && m_SortedExprElems.size()==0) init_cursor();
	int n = m_SortedExprElems.size();
	if(n==0) return;

	for(int i=0;i<m_ptCursorPos.x;i++)
	{
		strBegin += (m_SortedExprElems.ptr()[i])->get_graphics();
	}
	CString temp;
	(m_SortedExprElems.ptr()[i])->get_divided_strings(temp, strEnd);
	strBegin+=temp;
	i++;
	for(i; i<n; i++)
	{
		strEnd+=(m_SortedExprElems.ptr()[i])->get_graphics();
	}
}


CString	CParser::GetString( )
{
	return m_sExpression;
}

long	CParser::GetNecessaryParamsNames( BSTR *pbstrParamsNames, int iParamType, int iMode )
{
	long lSize = m_Params.count( );

	if( iParamType == PT_ALL )
	{

		if( iMode == OM_CALCULATE )
			lSize -= m_lCountUnNecessaryParams;
	}
	else
		lSize = get_necessary_params_count( iParamType );

	if( !pbstrParamsNames )
		return lSize;
	
	BSTR *pbstr = pbstrParamsNames;

	for( long lPos = m_Params.head( ); lPos; lPos = m_Params.next( lPos ) )
	{
		ExprParam *pparam = m_Params.get( lPos );
		if( ( iMode == OM_CALCULATE && pparam->bNecessaryToValidate ) || iMode == OM_PAINT )
		{
			ParamKey key = m_Params.get_key( lPos );
			if( (iParamType == PT_SIMPLE && !key.bArray) || (iParamType == PT_ARRAY && key.bArray) || iParamType ==  PT_ALL)
			{
				*pbstr = key.sParamName.AllocSysString();
				pbstr++;
			}
		}
	}

	return (pbstr - pbstrParamsNames);
}

long	CParser::GetNecessaryParamsNamesEx( DrawParamInfo *pParamsInfos, int iParamType, int iMode )
{
    long lSize = m_Params.count( );

	if( iParamType == PT_ALL )
	{

		if( iMode == OM_CALCULATE )
			lSize -= m_lCountUnNecessaryParams;
	}
	else
		lSize = get_necessary_params_count( iParamType );

	if( !pParamsInfos )
		return lSize;
	
	DrawParamInfo *pinfos = pParamsInfos;

	for( long lPos = m_Params.head( ); lPos; lPos = m_Params.next( lPos ) )
	{
		ExprParam *pparam = m_Params.get( lPos );
		if( ( iMode == OM_CALCULATE && pparam->bNecessaryToValidate ) || iMode == OM_PAINT )
		{
			ParamKey key = m_Params.get_key( lPos );
			if( (iParamType == PT_SIMPLE && !key.bArray) || (iParamType == PT_ARRAY && key.bArray) || iParamType ==  PT_ALL)
			{
				pinfos->bstrParamName = key.sParamName.AllocSysString();
				pinfos->bIsLValue = !pparam->bNecessaryToValidate;
				pinfos ++;
			}
		}
	}

	return (pinfos - pParamsInfos);    
}

BOOL	CParser::SetParamsValues( double *pfValues, long	lSize )
{
	if( ( lSize != get_necessary_params_count( PT_SIMPLE ) ) || !pfValues )
		return FALSE;

	long lPos = 0;
	double *pdouble = pfValues;

	for( lPos = m_Params.head( ); lPos; lPos = m_Params.next( lPos ) )
	{
		ExprParam *pParam = 0;
		pParam = m_Params.get( lPos );
		ParamKey key = m_Params.get_key( lPos );
		if( pParam && pParam->bNecessaryToValidate && !key.bArray )
		{
			//pParam->dValue = *pdouble;
			pParam->set_values( pdouble, 1, false );
			pParam->bValidate = true;
			pdouble++;
		}
	}
	return TRUE;
}

BOOL	CParser::SetParamsVArrayalues( ArrayParamValues *pstValues, long	lSize )
{
	if( ( lSize != get_necessary_params_count( PT_ARRAY ) ) || !pstValues )
		return FALSE;

	long lPos = 0;
	ArrayParamValues *pstArrayParams = pstValues;

	for( lPos = m_Params.head( ); lPos; lPos = m_Params.next( lPos ) )
	{
		ExprParam *pParam = 0;
		pParam = m_Params.get( lPos );
		ParamKey key = m_Params.get_key( lPos );
		if( pParam && pParam->bNecessaryToValidate && key.bArray )
		{
			if( pParam->set_values( pstArrayParams->pfvalues, pstArrayParams->lsize, true ) )
				pParam->bValidate = true;

			pstArrayParams++;
		}
	}
	return TRUE;        
}

BOOL	CParser::SetParamsExInfo( ParamExInfo *pExInfo, long	lSize )
{
	if( ( lSize != m_Params.count( ) ) || !pExInfo )
		return FALSE;

	ParamExInfo *pCurrExInfo =	pExInfo;

	for( long lPos = m_Params.head( ); lPos; lPos = m_Params.next( lPos ) )
	{
		ExprParam *pParam = 0;
		pParam = m_Params.get( lPos );
		if( !pParam->pstParamExInfo )
			pParam->pstParamExInfo = new ParamExInfo;
		pParam->set_ex_info( pCurrExInfo );
		pCurrExInfo ++;
	}
	return TRUE;
}

void	CParser::free( )
{
	m_sToken.Empty( );
	m_iTokeType = 0;
	m_iCurrIndex = - 1;
	m_Params.clear( );
	m_UnkFuncs.clear( );
	m_UnkSymbols.clear( );
	
	long lSize = m_ExprElems.size( );
	for( long l = 0; l < lSize; l++ )
	{
		delete m_ExprElems.ptr()[l];
		m_ExprElems.ptr()[l]=0;
	}

	m_ExprElems.free( ); 
	m_pCursorOwner = 0;
	m_SortedExprElems.free();
	clear_dummi_stack();
	//pElem = 0;	// указатель на элемент выражения
	m_pEntry = 0;
	m_lArrayValueIndex = -1;


}


// разбор выражения, на выходе код ошибки
//	код ошибки	|	описание ошибки
//		-1		|	ошибок нет
//		0		|	неизветсная ошибка
//		1		|	исходная строка - пустая
//		2		|	отсутствует '}'
//		3		|	отсутствует '{'
//		4		|	неизвестный символ
//		5		|	деление на ноль
//		6		|	отсутвует '(' или '[' после имени функции - ОТСУТСТВУЕТ!
//		7		|	неизвестная функция
//		8		|	отсутсвует ')' или ']'
//		9		|	отсутствует '(' или '[', или обнаружен неизвестный символ
//		10		|	обнаружен неизвестный символ или отсутствует операнд операции или обнаружена лишняя ')'(']')
//		11		|	отсутсвует операция
//		12		|	неоперделено значение параметра	 (для eval_expr)
//		13		|	ошибка при отрисовке формулы
//		15		|	неверное количество входных аргументов функции
int	CParser::ParseExpr( )
{
	m_iErrorCode = -1;	// OK
	m_bWaitArrayParam = false;
	m_bNewData = TRUE;
	try
	{
		free( );
		get_token( );
		if( m_sToken.IsEmpty( ) )
			return 1;	// string is empty

		double res = 0.;
	eval_expr1( &m_ExprElems );
	}
	catch( int iError )
	{
		free( );
		return iError;	// return code of error
	}
	catch(...)
	{
		free( );
		return 0;	// unknown error
	}

	return m_iErrorCode;
}

// evaluate
// рассчет выражения
// возвращает код ошибки (описание см. функцию parse_expr)
// специфические ошибки: 
// 12 - не определено значение параметра
// 14 - ошибка при вычислении выражения (результат выражения не определен)
int		CParser::eval_expr( double &dResult, RECT *prect )
{
	dResult = 0.;
	if (m_iErrorCode == 5) // Деление на ноль. Код ошибки остался от другого объекта.
		m_iErrorCode = -1;
	
	try
	{
		if( m_sExpression.IsEmpty( ) )
			return 1;
		if(  !m_ExprElems.ptr() )
		{
            // parse the m_sExpression
			int iError = ParseExpr( );
			if( m_iMode == OM_CALCULATE && iError != -1 )
				return iError;
		}
	
		m_pEntry = m_ExprElems.ptr();

		//if( !pElem )
		if( !m_pEntry )
			return 0;	// unknown error
		
		// calculate results
		//dResult = f_calc( prect );
		switch( m_iMode )
		{
		case OM_CALCULATE:
			dResult = (*m_pEntry)->calc( );
			if( 0 == _finite( dResult ) )
				m_iErrorCode = 14;	// ошибка при вычислении выражения
            break;

		case OM_CALCRECT:
			(*m_pEntry)->calc_rect( prect );
			/*::CopyRect( &m_DrawRect, prect );*/
			/*if(m_bCursor) init_cursor();*/
			break;

		case OM_PAINT:
			//if( ::IsRectEmpty( &m_DrawRect ) )

			::CopyRect( &m_DrawRect, prect );  
			(*m_pEntry)->calc_rect( &m_DrawRect );
			m_pEntry = m_ExprElems.ptr();

			if( !(*m_pEntry)->draw( ) )
			{
				
				throw 13;
			}
			break;

		}
	}
	catch( int iError )
	{
		dResult = 0.;
		m_iErrorCode = iError;
	}
	catch(...)
	{
		m_iErrorCode = 0;	// unknown error
	}
	/*m_bNewData = FALSE;*/
	return m_iErrorCode;	// return code error
}

/*int		CParser::CalcNecessaryRect( HDC hdc, RECT *prect )
{
	if( !hdc || !prect)
		return 13;
	m_hdc = hdc;	
	m_iMode = OM_CALCRECT;
	double dResult;
	int i = eval_expr( dResult, prect );
	m_iMode = OM_RELAX;
	m_hdc = 0;
	return i;
}  */

// возвращает код ошибки (описание см. функцию parse_expr)
//еще одна ошибка:	13 - ошибка при отрисовке формулы
int		CParser::Draw( HDC hdc, RECT *pRect, TextDrawOptions tFlags )							
{	
	sPaint.Empty( );
	
	if( !hdc || !pRect)
		return 13;

	switch( tFlags )
	{
	case tdoPaint:
		m_iMode = OM_PAINT;
		break;
	case tdoCalcRect:
		m_iMode = OM_CALCRECT;
		break;
	default:
		m_iMode = OM_RELAX;
	}

	double dResult;
	m_hdc = hdc;

	//int iOldMode = ::SetMapMode( m_hdc, MM_TEXT );

	char str[] = "A";
	RECT rtTemp = {0};
	calc_rect( str, &rtTemp, FALSE);
	pRect->bottom = pRect->top + (int)( (pRect->bottom - pRect->top) / 2. + (rtTemp.bottom - rtTemp.top) / 2. + 0.5);
    int iError = eval_expr( dResult, pRect );
	if(m_iMode==OM_PAINT)m_bNewData = FALSE;
	
	if( m_iMode == OM_CALCRECT )
		::CopyRect( &m_DrawRect, pRect );
	/*else
		::SetRectEmpty( &m_DrawRect );*/

	//::SetMapMode( m_hdc, iOldMode );
	//iOldMode = 0;

	m_hdc = 0;	
	if(m_iMode==OM_CALCRECT && m_bCursor && m_bNewData)
		init_cursor();
	m_iMode = OM_RELAX;
	

	return iError;
}

BOOL	CParser::Redraw( HDC hdc )
{
	if( ::IsRectEmpty( &m_DrawRect ) || !hdc )
        return FALSE;

	m_hdc = hdc;
	//int iOldMode = ::SetMapMode( m_hdc, MM_TEXT );

    m_pEntry = m_ExprElems.ptr();
    if( !m_pEntry )
		return FALSE;
    
	BOOL bRes = (*m_pEntry)->draw( );

	//::SetMapMode( m_hdc, iOldMode );
	m_hdc = 0;
	m_pEntry = 0;

	return bRes;
}

void	CParser::GetExprRect( RECT *prect )
{
	if( !prect )
		return;

	::CopyRect( prect, &m_DrawRect );
}



int		CParser::Evaluate( double &dResult )
{
	m_iMode = OM_CALCULATE;
	int i = eval_expr( dResult );
	m_pLastEntry = *m_pEntry;
	m_iMode = OM_RELAX;
	return i;
}

void	CParser::get_token( )
{
	m_iTokeType = 0;
	m_sToken.Empty( );

	if( m_sExpression.IsEmpty( ) )
		return;
	else if( m_iCurrIndex == -1 )
		m_iCurrIndex = 0;

	if( m_iCurrIndex >= m_sExpression.GetLength( ) )
		return;
	
	while( m_sExpression.GetAt( m_iCurrIndex ) == ' ' ) m_iCurrIndex++;

	if( is_delim( ) )
	{
		m_sToken = m_sExpression.GetAt( m_iCurrIndex );
		m_iTokeType = DELIMITER;
		m_iCurrIndex ++;
	}
	else if( is_letter( ) )
	{
		int i = 0;
		while( is_letter( ) ) 
			m_sToken.Insert( i++, m_sExpression.GetAt( m_iCurrIndex++ ) );

		m_iTokeType = is_named_const() ? NAMED_CONST : FUNCTION;

	}
	else if( is_digit( ) )
	{
		int i = 0;
		while( is_digit( ) ) 
			m_sToken.Insert( i++ , m_sExpression.GetAt( m_iCurrIndex++ ) );
		if( is_point( ) )
		{
			m_sToken.Insert( i++, m_sExpression.GetAt( m_iCurrIndex++ ) );
			while( is_digit( ) ) 
				m_sToken.Insert( i++, m_sExpression.GetAt( m_iCurrIndex++ ) );
		}
		m_iTokeType = NUMBER;
	}
	else if( is_variable_begin( ) )
	{
		m_iCurrIndex++;
		int i = 0;
		do
		{
			if( m_iCurrIndex >= m_sExpression.GetLength( ) )
			{
				m_iErrorCode = 2;
				break;
			}
			m_sToken.Insert( i++, m_sExpression.GetAt( m_iCurrIndex++ ) );
		}
		while( !is_variable_end( ) );
		m_iCurrIndex++;
		m_iTokeType = VARIABLE;		
	}
	else if( is_variable_end( ) )
		m_iErrorCode = 3;
	else if( is_subexpr_delim( ) )
	{
		m_sToken = m_sExpression.GetAt( m_iCurrIndex );
		m_iTokeType = SUB_EXPR_DELIMITER;
		m_iCurrIndex ++;
	}
	else
	{
		m_sToken = m_sExpression.GetAt( m_iCurrIndex++ );
		m_iTokeType = UNKNOWN;
		m_iErrorCode = 4;
	}
}

//////////////////////////////////////////////////////

// функция обработки пустой операции
void	CParser::eval_expr1( _ptr_t<XExprEntry*> *pelems, BOOL bBreakOnRBracket /*= FALSE*/, BOOL bUseSubExpression /*= FALSE*/ )
{
	CString op;
	_ptr_t<XExprEntry*> temp_elems;

	eval_expr1_5( pelems );

    while( m_iTokeType )
	{
		temp_elems.free( );

		// учет подвыражений
		if( bUseSubExpression )
			// если обрабатываем подвыражения, то окончанием подвыражения является либо их разделитель, 
			// либо любой другой символ, в частности ')', что означает и окончание списка подвыражения,
			// поэтому просто вываливаемся на уровень выше - на обработку подвыражений
			return;
		
		// имитация неизвестного символа для символов ')' или ']'
		if( m_sToken == ')' || m_sToken == ']' )
		{
			if( bBreakOnRBracket )
				return;
			else
				m_iTokeType = UNKNOWN;
		}

		m_iErrorCode = 11;
	
		
				
		int iOld = m_iCurrIndex;
		// eval_expr2( &temp_elems );
		eval_expr1_5( &temp_elems );	
		if( iOld == m_iCurrIndex )
			get_token( );
				
		op = '?';
		//add_to_begin( pelems, f_err_empty_operation );
		XEmptyOperation *pEl = new XEmptyOperation;
		pEl->pParser = this;
		add_to_begin( pelems, pEl );
		merge_elems( pelems, &temp_elems );
	}
}

// оператор =
void	CParser::eval_expr1_5( _ptr_t<XExprEntry*> *pelems )
{
	CString op;
	_ptr_t<XExprEntry*> temp_elems;

	eval_expr2( pelems );
	
	while( ( (op = m_sToken) == '=' ) )
	{
		temp_elems.free( );
		// сохранение позиции в массиве параметров
		m_lCountUnNecessaryParams = m_Params.count( );
		for( long lPos = m_Params.head( ); lPos; lPos = m_Params.next( lPos ) )
			m_Params.get( lPos )->bNecessaryToValidate = false;
		
		get_token( );
        eval_expr2( &temp_elems );
		XEqual *pEl = new XEqual;
		pEl->pParser = this;
		add_to_begin( pelems, pEl );
		//add_to_begin( pelems, f_equal );			
        merge_elems( pelems, &temp_elems );
	}
}

// сложение и вычитание
void	CParser::eval_expr2( _ptr_t<XExprEntry*> *pelems )
{
	CString op;
	_ptr_t<XExprEntry*> temp_elems;

	eval_expr3( pelems );

	while( (op = m_sToken) == '+' || m_sToken == '-' )
	{
		temp_elems.free( );
		get_token( );
		eval_expr3( &temp_elems );

		XExprEntry *pExEnt = 0;
		if( op == '+' )
		{
			pExEnt =  new XAdd;
			//add_to_begin( pelems, &el );
			//add_to_begin( pelems, f_add );	
		}
		else if( op == '-' )
		{
			pExEnt =  new XSub;
			//add_to_begin( pelems, &el );
			//add_to_begin( pelems, f_sub );
		}

		if( pExEnt )
		{
			pExEnt->pParser = this;
			add_to_begin( pelems, pExEnt );
			pExEnt = 0;
		}

		merge_elems( pelems, &temp_elems );
	}
}

// Умножение и деление
void	CParser::eval_expr3( _ptr_t<XExprEntry*> *pelems )
{
	CString op;
	_ptr_t<XExprEntry*> temp_elems;

	eval_expr4( pelems );
	while( (op = m_sToken) == '*' || m_sToken == '/' || m_sToken == '%' )
	{
		temp_elems.free( );
		get_token( );
		eval_expr4( &temp_elems );

		XExprEntry *pExEnt = 0;
		if( op == '*' )
			pExEnt =  new XMul;
		else if( op == '/' )	
			pExEnt =  new  XDiv;
		else if( op == '%' )	
			pExEnt =  new XMod;
					
		if( pExEnt )
		{
			pExEnt->pParser = this;
			add_to_begin( pelems, pExEnt );
			pExEnt = 0;
		}

		merge_elems( pelems, &temp_elems );
	}
}

// Возведение в степень
void	CParser::eval_expr4( _ptr_t<XExprEntry*> *pelems )
{
	_ptr_t<XExprEntry*> temp_elems;
	
	eval_expr5( pelems );
	while( m_sToken == '^' )
	{
		get_token( );
		eval_expr5( &temp_elems );
        
		XPow *pEl = new XPow;
		pEl->pParser = this;
		add_to_begin( pelems, pEl );
		merge_elems( pelems, &temp_elems );
		temp_elems.free();
	}
}

// Унарный + или -
void	CParser::eval_expr5( _ptr_t<XExprEntry*> *pelems )
{
	CString op;
	if( ( m_iTokeType == DELIMITER ) && m_sToken == '+' || m_sToken == '-' )
	{
		op = m_sToken;
		get_token( );
	}
	eval_expr6( pelems );

	XExprEntry *pExEnt = 0;
	if( op == '-')
		pExEnt = new XUMinus;
	else if( op == '+' )
		pExEnt = new XUPlus;
	
	if( pExEnt )
	{
		pExEnt->pParser = this;
		add_to_begin( pelems, pExEnt );
		pExEnt = 0;
	}

}

// Обработка функций sin, cos и т.п.
void	CParser::eval_expr6( _ptr_t<XExprEntry*> *pelems, int *pnExpressionCount/* = 0*/, BOOL bGetdArgs/*= FALSE*/ )
{
	CString op;
	
	if( m_iTokeType == FUNCTION )
	{
		op = m_sToken;
		XFunc *pfunc = 0;
		bool	bIsArrayFunc = false;

		// необходимое количество аргументов функции
		int		nNecessaryArgsCount = -1;	// произвольное количество аргументов функции
						
		if( !op.CompareNoCase( _T("sin") ) ) 
			pfunc = new XSin; 
        else if( !op.CompareNoCase( _T("cos") ) ) 
			pfunc = new XCos; 
		else if( !op.CompareNoCase( _T("asin") )  )	
			pfunc = new XAsin;
		else if( !op.CompareNoCase( _T("acos") ) )	
			pfunc = new XAcos;
		else if( !op.CompareNoCase( _T("exp") ) )	
			pfunc = new XExp; 
		else if( !op.CompareNoCase( _T("lg") ) )	
			pfunc = new XLog10;
		else if( !op.CompareNoCase( _T("log") ) || !op.CompareNoCase( _T("ln") ) )
			pfunc = new XLog;
		else if( !op.CompareNoCase( _T("tan") ) )
			pfunc = new XTan;
		else if( !op.CompareNoCase( _T("atan") ) )
			pfunc = new XAtan;
		else if( !op.CompareNoCase( _T("sqr") ) || !op.CompareNoCase( _T("sqrt") ) )
		    pfunc = new XSqrt;
		// функции с параметром, имеющим множество значений
		else if( !op.CompareNoCase( _T("sum") ) )
		{	// 
			bIsArrayFunc = true;
			pfunc = new XSum;
		}
		else if( !op.CompareNoCase( _T("avg") ) )
		{	// 
			bIsArrayFunc = true;
			pfunc = new XAvg;
		}
        // функции с неизвестным числом параметров
		else if( !op.CompareNoCase( _T("min") ) )
		    pfunc = new XMin;
		else if( !op.CompareNoCase( _T("max") ) )
		    pfunc = new XMax;		 
		// неизвестная функция
		else
		{
			m_iErrorCode = 7;
			pfunc = new XUnkFunction;
			((XUnkFunction *)pfunc)->m_lPos = m_UnkFuncs.add_tail( op );
		}

		// store
		bool	bOldWaitArrayParam = m_bWaitArrayParam,
				bOldSubExprProcessing = m_bSubExprProcessing;

		m_bWaitArrayParam = bIsArrayFunc;
		m_bSubExprProcessing = true;

		int nArgsCount = 0;
		
		get_token( );
		eval_expr6( pelems, &nArgsCount, TRUE );
		pfunc->set_curr_args_count( nArgsCount );

		if( (0 < pfunc->get_actual_args_count()) && (nArgsCount != pfunc->get_actual_args_count()) )
			m_iErrorCode = 15; //ошибка: количество аргументов не соответствует количеству аргументов функции
        
		// restore
		m_bWaitArrayParam = bOldWaitArrayParam;
		m_bSubExprProcessing = bOldSubExprProcessing;
		
		if( pfunc )
		{
			pfunc->pParser = this;
			add_to_begin( pelems, pfunc );
			pfunc = 0;
		}

	}
	else
		eval_expr7( pelems, pnExpressionCount, bGetdArgs );
}

// Обработка выражения в скобках
void	CParser::eval_expr7( _ptr_t<XExprEntry*> *pelems, int *pnExpressionCount/*= 0*/, BOOL bGetdArgs/*= FALSE*/ )
{
	if( m_sToken == '(' || m_sToken == '[' )
	{
		int	iLCode = m_sToken.GetAt( 0 ), // код левой скобки
			iRCode = 0;	// код правой скобки

		_ptr_t<XExprEntry*> temp_elems;
		get_token( );
		if( bGetdArgs )
			eval_expr7_5( &temp_elems, pnExpressionCount );
		else
		{
         	eval_expr1( &temp_elems, TRUE );
			if( temp_elems.size() && pnExpressionCount )
				 *pnExpressionCount = 1;
		}
			
		if( ( m_sToken != ')' ) && ( m_sToken != ']' ) )
			m_iErrorCode = 8;
		else
			iRCode = m_sToken.GetAt( 0 );
		
		XBrackets *pbrackets = 0;
		if( bGetdArgs )
		{
			XFuncBrackets *pfbrackets = new XFuncBrackets;
			pfbrackets->set_curr_args_count( *pnExpressionCount );
			pbrackets = pfbrackets;
		}
		else
			pbrackets = new XBrackets;

		pbrackets->pParser = this;
		pbrackets->m_iLBracket = iLCode;
		pbrackets->m_iRBracket = iRCode;
		add_to_begin( &temp_elems, pbrackets ); 

		merge_elems( pelems, &temp_elems );
		//TODO
		if(m_sToken==')' || m_sToken==']') get_token( );
	}
	/* закоментированно, т.к. если этот код включить после имени функции будет ожидаться подвыражение, что 
	приводит к тому, что выражение вида lg10-5 будет вычисляться как lg(10-5), т.е. меняется приоритет операций
	 else if( bGetdArgs )
	{
		eval_expr7_5( pelems, pnExpressionCount );
	} */
	else
	{
		atom( pelems );
		if( pnExpressionCount ) *pnExpressionCount = 1;
	}
}

// Обработка списка аргументов
void	CParser::eval_expr7_5( _ptr_t<XExprEntry*> *pelems, int *pnArgsCount )
{
    if( !pnArgsCount || !pelems )
		return;
	
	*pnArgsCount = 0;
	XSubExprList *plist = new XSubExprList;
	plist->pParser = this;
	add_to_begin( pelems, plist );
    do
	{
		(*pnArgsCount) ++;
		if( (*pnArgsCount) > 1 )
		{
			plist->m_strSeparator = m_sToken; // если встретили первый разделитель - запоминаем его
			get_token( );
		}

		XSubExpr *pexpr = new XSubExpr;
        eval_expr1( &(pexpr->m_entries), FALSE, TRUE);
		plist->m_sub_exprs.alloc( (*pnArgsCount) );
		plist->m_sub_exprs.ptr()[(*pnArgsCount) - 1] = pexpr;
	}
	while( m_iTokeType == SUB_EXPR_DELIMITER );
}

void	CParser::atom( _ptr_t<XExprEntry*> *pelems )
{
	pelems->free( );
	switch( m_iTokeType )
	{
	case NUMBER:
		{
			XConst *pEl = new XConst;
			pEl->pParser = this;
			if(m_sToken.Replace( ',', '.' ) || m_sToken.Find('.')!=-1 ) 
			{
				pEl->m_bDouble =true;
				pEl->m_Const.m_dConst = atof(m_sToken);
				

			}
			else 
			{
				pEl->m_bDouble =false;
				pEl->m_Const.m_nConst = atoi(m_sToken);
			}
			pEl->str = m_sToken;
			add_to_begin( pelems, pEl ); 
			get_token( );
		}
		break;
	case VARIABLE:
		{
			ExprParam *pParam = new ExprParam;
			pParam->init( );

			ParamKey key;
			key.bArray = m_bWaitArrayParam;
			key.sParamName = m_sToken;
			
			XVar *pEl = new XVar;
			pEl->pParser = this;
			pEl->m_lPos = m_Params.set( pParam, key );
			
			add_to_begin( pelems, pEl ); 
			get_token( );
		}
		break; 
	case NAMED_CONST:
		{
			XNamedConst *pEl = new XNamedConst;
			pEl->pParser = this;
			pEl->m_nCode = getCodeNamedConst( m_sToken );
			add_to_begin( pelems, pEl ); 
			get_token( );
		}
		break;
	case SUB_EXPR_DELIMITER:
		if( !m_bSubExprProcessing )
		{	// если обработки подвыражений в данный момент не происходит - счтаем, что символ разделитель 
			// подвыражений является неизветным для нас символом
			m_iTokeType = UNKNOWN;
            atom( pelems );
		}
		//иначе ни чего не делаем - обработка подвыражений - eval_expr7_5 на верхнем уровне
		break;	
	case UNKNOWN:
		{
			XUnkSymbol *pEl = new XUnkSymbol;
			pEl->pParser = this;
			pEl->m_lPos = m_UnkSymbols.add_tail( m_sToken );
			add_to_begin( pelems, pEl );
			m_iErrorCode = 4;
			get_token( );
		}
		break;
	default:
		{
			XMissingOperand *pEl = new XMissingOperand;
			pEl->pParser = this;
			add_to_begin( pelems, pEl );
			m_iErrorCode = 10;
		}
	}
}

void	CParser::merge_elems( _ptr_t<XExprEntry*> *preceiver, _ptr_t<XExprEntry*> *psource )
{
	long	lOldSize = (*preceiver).size( ),
			lSourceSize = (*psource).size( );
	(*preceiver).alloc( lOldSize + lSourceSize );
	for( long l = 0; l < lSourceSize; l++ ) 
		*((*preceiver).ptr() + lOldSize + l) = *((*psource).ptr( ) + l);
}

void	CParser::add_to_begin( _ptr_t<XExprEntry*> *preceiver, XExprEntry *pelem )
{
	_ptr_t<XExprEntry*> temp;
	merge_elems( &temp, preceiver );
	preceiver->free( );
	preceiver->alloc( 1 );
	*(preceiver->ptr( )) = pelem;
	merge_elems( preceiver, &temp );
	temp.free( );
}

BOOL CParser::is_delim(void) 
{ 
	return ( strchr( "+-*/%^()[]=", m_sExpression.GetAt( m_iCurrIndex ) ) ? TRUE : FALSE );
}

BOOL CParser::is_letter(void) 
{ 
	char c = m_sExpression.GetAt( m_iCurrIndex );
	return ( ( c >='a' && c <='z') || ( c >='A' && c <= 'Z' ) );
}

BOOL CParser::is_digit(void) 
{ 
	char c = m_sExpression.GetAt( m_iCurrIndex );
	return ( c >= '0' && c <= '9');
}

BOOL CParser::is_point(void) 
{
	return (m_sExpression.GetAt( m_iCurrIndex ) == '.' || m_sExpression.GetAt( m_iCurrIndex ) == ',' ); 
}

BOOL CParser::is_variable_begin(void)
{
	return (m_sExpression.GetAt( m_iCurrIndex ) == '{' );
}

BOOL CParser::is_variable_end(void)
{
	return ( m_sExpression.GetAt( m_iCurrIndex ) == '}' );
}

BOOL	CParser::is_named_const(void)
{
	return getCodeNamedConst( m_sToken ) != NCONST_UNKNOWN;
}

BOOL	CParser::is_subexpr_delim(void)
{
    return (m_sExpression.GetAt( m_iCurrIndex ) == ';');
}

int		CParser::getCodeNamedConst( CString sToken )
{
	if( !sToken.Compare( _T("pi")) )
		return NCONST_PI;
	else if( !sToken.Compare( _T("e")) )
		return NCONST_EXP;
	else
		return NCONST_UNKNOWN;
}


BOOL CParser::calc_rect( LPCSTR pstr, RECT *prect, BOOL OffsetByBottom /*= TRUE*/ )
{
	if( !m_hdc || !prect )
		return FALSE;
	long lBottom = prect->bottom,
		lLeft = prect->left;
	
	::SetRectEmpty( prect );

	Graphics graphics( m_hdc );
	PointF point_origin( (REAL)(prect->left), (REAL)(prect->top) );
	RectF boundRect;
	
	LOGFONT lf = {0};
	::GetObject( ::GetCurrentObject( m_hdc, OBJ_FONT ), sizeof(lf), &lf );
	/*INT nFontStyle = FontStyleRegular;*/
	lf.lfOutPrecision =OUT_TT_ONLY_PRECIS;
	/*if ( lf.lfWidth == FW_BOLD ) nFontStyle |= FontStyleBold;  
	if ( lf.lfItalic ) nFontStyle |= FontStyleItalic;
	if ( lf.lfUnderline ) nFontStyle |= FontStyleUnderline;
	if ( lf.lfStrikeOut ) nFontStyle |= FontStyleStrikeout;*/
	//Font font( _bstr_t(lf.lfFaceName), (REAL)(abs(lf.lfHeight) * 72.f / ::GetDeviceCaps(m_hdc, LOGPIXELSY)), nFontStyle, UnitPoint );
	//Font font(L"Times New Roman",16.f);
	Gdiplus::Font font(m_hdc, &lf);
	/*CString str; 
	str.Format("--                                   ------- font:  %s\t%i \n", lf.lfFaceName, lf.lfOutPrecision);	
	OutputDebugStr(str);*/
	// [vanek] SBT:1086 - 23.07.2004
	// get codepage from charset
    UINT uicodepage = CP_ACP;
	if( lf.lfCharSet )
	{
		DWORD dw = {0};
		dw =	MAKEWORD( lf.lfCharSet, 0 );
		CHARSETINFO chsi;
		::ZeroMemory( &chsi, sizeof(CHARSETINFO) );
		if( ::TranslateCharsetInfo( (DWORD*)(dw), &chsi, TCI_SRCCHARSET ) && ::IsValidCodePage( chsi.ciACP ) )
			uicodepage = chsi.ciACP;
	}

	int nlen_unicod = 0;
	nlen_unicod = ::MultiByteToWideChar( uicodepage, MB_PRECOMPOSED, pstr, -1, NULL, NULL );
	LPWSTR pwstr = 0;
	pwstr = new WCHAR[ nlen_unicod ];
	BOOL retval = FALSE;
	if( ::MultiByteToWideChar( uicodepage, MB_PRECOMPOSED, pstr, -1, pwstr, nlen_unicod) )	
		graphics.MeasureString( pwstr, -1, &font, point_origin, &boundRect );
	
	if( pwstr  )
		delete[] pwstr; pwstr = 0;
	
	
	prect->left = (int)(boundRect.X > 0 ? ceil( boundRect.X ) : floor( boundRect.X ) );
	prect->top = (int)(boundRect.Y > 0 ? ceil( boundRect.Y ) : floor( boundRect.Y ) );
	prect->right = (int)(prect->left + ceil(boundRect.Width) );
	prect->bottom = (int)(prect->top + ceil(boundRect.Height) ); 

	if( !::IsRectEmpty( prect ) )
	{
		::OffsetRect( prect, lLeft, OffsetByBottom ? (lBottom - prect->bottom) : 0 );	
        return TRUE;
	}

	// for empty string
	prect->right = prect->left + 1; 
	if( OffsetByBottom )
		prect->top = prect->bottom;

	return FALSE;  
}

BOOL	CParser::draw_text( LPCSTR pstr, RECT *prect, UINT uFormat /*= 0*/ )
{
	if( !m_hdc || !prect )
		return FALSE;

	Graphics graphics( m_hdc );	  
    LOGFONT lf = {0};
	::GetObject( ::GetCurrentObject( m_hdc, OBJ_FONT ), sizeof(lf), &lf );
	/*INT nFontStyle = FontStyleRegular;*/
	lf.lfOutPrecision =OUT_TT_ONLY_PRECIS;
	/*if ( lf.lfWidth == FW_BOLD ) nFontStyle |= FontStyleBold;
	if ( lf.lfItalic ) nFontStyle |= FontStyleItalic;
	if ( lf.lfUnderline ) nFontStyle |= FontStyleUnderline;
	if ( lf.lfStrikeOut ) nFontStyle |= FontStyleStrikeout;*/
	//Font font( _bstr_t(lf.lfFaceName), (REAL)(abs(lf.lfHeight) * 72.f / ::GetDeviceCaps(m_hdc, LOGPIXELSY)), nFontStyle, UnitPoint );
	/*Font font(L"Times New Roman",16.f);*/
	Gdiplus::Font font(m_hdc, &lf);
	/*CString str;
	str.Format("--                                   ------- font:  %s\t%i \n", lf.lfFaceName, lf.lfOutPrecision);	
	OutputDebugStr(str);*/


    StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetTrimming( StringTrimmingNone );
	format.SetLineAlignment(/*StringAlignmentCenter*/StringAlignmentNear);
	format.SetFormatFlags( StringFormatFlagsNoWrap );
	RectF layoutRect( (REAL)(prect->left), (REAL)(prect->top), (REAL)(prect->right - prect->left), 
		(REAL)(prect->bottom - prect->top) );
	Color gdipColor;
	gdipColor.SetFromCOLORREF( ::GetTextColor( m_hdc ) );
	SolidBrush brush( gdipColor );

	
	if(0)
	{ 	// for debug - Draw layoutRect.
		Color clr;
		clr.SetFromCOLORREF( ::GetTextColor( m_hdc ) );
		graphics.DrawRectangle(&Pen(clr, 1), layoutRect);
	}
    
	// [vanek] SBT:1086 - 23.07.2004
	// get codepage from charset
    UINT uicodepage = CP_ACP;
	if( lf.lfCharSet )
	{
		DWORD dw = {0};
		dw =	MAKEWORD( lf.lfCharSet, 0 );
		CHARSETINFO chsi;
		::ZeroMemory( &chsi, sizeof(CHARSETINFO) );
		if( ::TranslateCharsetInfo( (DWORD*)(dw), &chsi, TCI_SRCCHARSET ) && ::IsValidCodePage( chsi.ciACP ) )
			uicodepage = chsi.ciACP;
	}

	



	int nlen_unicod = 0;
	nlen_unicod = ::MultiByteToWideChar( uicodepage, MB_PRECOMPOSED, pstr, -1, NULL, NULL );
	LPWSTR pwstr = 0;
	pwstr = new WCHAR[ nlen_unicod ];
	BOOL retval = FALSE;
	if( ::MultiByteToWideChar( uicodepage, MB_PRECOMPOSED, pstr, -1, pwstr, nlen_unicod) )	
		retval = (Ok == graphics.DrawString( pwstr, -1, &font, layoutRect, &format, &brush ));
	
	if( pwstr  )
		delete[] pwstr; pwstr = 0;
    
	return retval ;
}

void	CParser::init_cursor()
{
	if(!m_bCursor) return;

	m_SortedExprElems.free();
	clear_dummi_stack();

	int n = get_expression_count();
	if(!n) return;



	m_SortedExprElems.alloc(n);
	int from =0;
	for(int i=0;i<m_ExprElems.size();i++)
	{
		copy_next(m_SortedExprElems.ptr(),from,i);
	}
	for(i=0;i<m_SortedExprElems.size();i++)
	{
		(m_SortedExprElems.ptr()[i])->set_cursor_offset(-1);
	}
	sort_entries();
	XExprEntry** entry = m_SortedExprElems.ptr();
	int m = m_SortedExprElems.size()-1;
	if(m_ptCursorPos.x==-1 || m_bReset || m_ptCursorPos.x>m) m_ptCursorPos.x = m;
	if(m)
	{
		//last & current entry init
		if(m>=0)
		{
			m_pLastEntry = entry[m];
			m_pCursorOwner = entry[m_ptCursorPos.x];
			m_pCursorOwner->set_cursor_offset(m_ptCursorPos.y);
			m_pCursorOwner->get_rect(&m_rectCursor);
		}
		
	}
	m_bReset =false;
	/**/
	//int d = m_rectCursor.bottom - m_DrawRect.bottom;
	/*OffsetRect(&m_rectCursor, 0, d);*/
	/*CString s, s1;
	OutputDebugStr("-----------------------\n");
	for(int i=0;i<n;i++)
	{
		XExprEntry* entry = m_SortedExprElems.ptr()[i];
		RECT rect;
		entry->get_rect(&rect);
		s.Format("Entry rect: left = %i\tright = %i\t%s\n", rect.left, rect.right, entry->get_graphics());
		OutputDebugStr(s);
		s1+=entry->get_graphics();
		continue;
	}
	OutputDebugStr(s1);
	OutputDebugStr("\n-----------------------\n");*/
 	/*m_pCursorOwner->get_rect(&m_rectCursor);*/
}


int		CParser::get_next_non_empty_expr(int n)
{
	if(n<0) return -1;
	RECT r;
	while(n<m_SortedExprElems.size())
	{
		(m_SortedExprElems.ptr()[n])->get_rect(&r);
		if(!IsRectEmpty(&r)) break;
		n++;
	}
	if(n >= m_SortedExprElems.size()) return -1;
	return n;
}

int		CParser::get_prev_non_empty_expr(int n)
{
	if(m_SortedExprElems.size() <=  n) return -1;
	RECT r;
	while(n>=0)
	{
		(m_SortedExprElems.ptr()[n])->get_rect(&r);
		if(!IsRectEmpty(&r)) break;
		n--;
	}
	return n;
}
long	CParser::get_necessary_params_count( int iParamType )
{
	long lcount = 0;

	for( long lpos = m_Params.head(); lpos; lpos = m_Params.next( lpos ) )
	{
		ExprParam *pparam = 0;
		pparam = m_Params.get( lpos );
		ParamKey key = m_Params.get_key( lpos );
		if( pparam )
		{
			if( pparam->bNecessaryToValidate &&	( (iParamType == PT_SIMPLE && !key.bArray) || 
				(iParamType == PT_ARRAY && key.bArray) || iParamType == PT_ALL ) )
				lcount++;
		}
	}

	return lcount;
}


//// XExprEntry Class Implementation 
void	CParser::XExprEntry::next( )
{
	if( !pParser || !(pParser->m_pEntry) )
		throw -2;
	++(pParser->m_pEntry);
	return;
}

double	CParser::XExprEntry::calc( )
{
	next();
	return (*pParser->m_pEntry)->calc( );
}

BOOL	CParser::XExprEntry::draw( )
{
	next( );
	return (*pParser->m_pEntry)->draw( );
}

void	CParser::XExprEntry::calc_rect( RECT *prect )
{
	next( );
	(*pParser->m_pEntry)->calc_rect( prect );
}

void	CParser::XExprEntry::get_rect(RECT* rect)
{
	*rect = m_rtEntry;
	//rect++;
	//next();
	//(*pParser->m_pEntry)->get_rect(rect);
}
void CParser::ActivateCursor()
{
	m_bCursor = true;
	init_cursor();
}

void	CParser::XExprEntry::_calc_rect_for_dyadic_operation( char *sOp, RECT *prtOp, RECT *prect )
{
	if( !pParser )
		throw -2;
	
	long	lBottom = prect->bottom;
    
	CParser::XExprEntry::calc_rect( prect );
	
	if( sOp && prtOp )
	{
		prtOp->top = prtOp->bottom = lBottom;
		prtOp->left = prtOp->right = prect->right;

		pParser->calc_rect( sOp, prtOp );

		::UnionRect( prect, prtOp, prect );
	}

	RECT rtTemp = {0};
	rtTemp.left = rtTemp.right = prect->right;
	rtTemp.top = rtTemp.bottom = lBottom;
	
	CParser::XExprEntry::calc_rect( &rtTemp );

	::UnionRect( prect, &rtTemp, prect );
}

void	CParser::XExprEntry::_calc_rect_for_unary_operation( char *sOp, RECT *prtOp, RECT *prect )
{
	if( !sOp )
		return;

	if( !pParser )
		throw -2;

	long	lBottom = prect->bottom;

	pParser->calc_rect( sOp, prect );
        
	::CopyRect( prtOp, prect );
	
	prect->top = prect->bottom = lBottom;
	prect->left = prect->right;
		
	CParser::XExprEntry::calc_rect( prect );
    
	::UnionRect( prect, prtOp, prect );
}

BOOL	CParser::XExprEntry::_draw__dyadic_operation( char *sOp, RECT *prtOp )
{
	/*if( !sOp )
		return FALSE;*/

	if( !pParser )
		throw -2;

	 if( !CParser::XExprEntry::draw( ) )
		 return FALSE;

	 /*if( !pParser->draw_text( sOp, prtOp ) )
		  return FALSE;*/
	 if( sOp && prtOp )
	 {
         if( !pParser->draw_text( sOp, prtOp ) )
		  return FALSE;
	 }

	 if( !CParser::XExprEntry::draw( ) )
		 return FALSE;

	 return TRUE;
}

BOOL	CParser::XExprEntry::_draw_unary_operation( char *sOp, RECT *prtOp )
{
	if( !sOp )
		return FALSE;

	if( !pParser )
		throw -2;

	if( !pParser->draw_text( sOp, prtOp ) )
		return FALSE;

	return CParser::XExprEntry::draw( );
}
RECT	CParser::XExprEntry::move_cursor(MovingDirection dir)
{
	RECT rect = this->m_rtEntry;
	if( !pParser ) return rect;
	int offset = -1;
	switch(dir)
	{
	case mdLeft:
		pParser->m_ptCursorPos.x--;
        break;
	case mdRight:
		pParser->m_ptCursorPos.x++;
		offset =1;
		break;
	}
	pParser->m_pCursorOwner =pParser->m_SortedExprElems.ptr()[pParser->m_ptCursorPos.x];
	pParser->m_ptCursorPos.y = pParser->m_pCursorOwner->set_cursor_offset(offset);
	pParser->m_pCursorOwner->get_rect(&rect);
	
	return rect;
}

//// XConst Class Implementation 
void	CParser::XConst::get_string( char **pstr  )
{
	if( *pstr )
		return;
	*pstr = new char[MAX_PATH];
	ZeroMemory(*pstr, MAX_PATH);
	str.CopyChars(*pstr,str,str.GetLength());

}
EntryType 	CParser::XConst::GetType()
{
	return ET_CONST;
}

void CParser::XConst::get_divided_strings(CString& s1, CString& s2)
{

	int n = pParser->m_ptCursorPos.y;
	if(n<0) return;
	//CString str = get_graphics();
	int length = str.GetLength();
	if(n>length) n = length;
	s1 = str.Left(n);
	s2 = str.Right(length-n);
}

void CParser::XConst::delete_entry()
{
	if(m_nCursorPos ==0 ) m_nCursorPos=1;
    str.Delete(--m_nCursorPos);
	pParser->m_ptCursorPos.y = m_nCursorPos;

	if(str.GetLength()==0 || m_nCursorPos==0){
		pParser->m_ptCursorPos.x--;
		pParser->m_ptCursorPos.y =-1;
	}
}

void	CParser::clear_dummi_stack()
{
	while(!m_tempEntries.empty())
	{
		XExprEntry* e = m_tempEntries.top();
		delete e;
		m_tempEntries.pop();
	}
}
void CParser::create_dummy_bracket_entry(XBrackets* e, XExprEntry** el, XExprEntry** er)
{
	RECT rect = e->get_left_rect();
	*el = new XDummyBracket();
	((XDummyBracket*)(*el))->ref = e;
	((XDummyBracket*)(*el))->md = mdLeft;
	(*el)->m_rtEntry = rect;
	(*el)->pParser = this;
	if(IsRectEmpty(&rect)) (*el)->m_strType ="";		
	else (*el)->m_strType ="(";

	m_tempEntries.push(*el);
//----
	rect = e->get_right_rect();
	*er = new XDummyBracket();
	((XDummyBracket*)(*er))->ref = e;
	((XDummyBracket*)(*er))->md = mdRight;
	(*er)->m_rtEntry = rect;
	(*er)->pParser = this;
	if(IsRectEmpty(&rect)) (*er)->m_strType ="";		
	else (*er)->m_strType =")";

	m_tempEntries.push(*er);
}

void	CParser::measure_substring(char* str, int subCount ,RECT& rect)
{
	if( !m_hdc ) return;

	/*MeasureCharRanges(m_hdc);*/
	Graphics graphics( m_hdc );
	//PointF point_origin( 0.f, 0.f );
	/*rect.right =1000;*/
	RectF boundRect(rect.left, rect.top, 10000, 10000);

	
	LOGFONT lf = {0};
	::GetObject( ::GetCurrentObject( m_hdc, OBJ_FONT ), sizeof(lf), &lf );
	/*INT nFontStyle = FontStyleRegular;*/
	lf.lfOutPrecision =OUT_TT_ONLY_PRECIS;
	/*if ( lf.lfWidth == FW_BOLD ) nFontStyle |= FontStyleBold;
	if ( lf.lfItalic ) nFontStyle |= FontStyleItalic;
	if ( lf.lfUnderline ) nFontStyle |= FontStyleUnderline;
	if ( lf.lfStrikeOut ) nFontStyle |= FontStyleStrikeout;*/
	//Font font( _bstr_t(lf.lfFaceName), (REAL)(abs(lf.lfHeight) * 72.f / ::GetDeviceCaps(m_hdc, LOGPIXELSY)), nFontStyle, UnitPoint );
	Gdiplus::Font font(m_hdc, &lf);

	/*CString s;
	s.Format("--                                   ------- font: %s\t%i \n", lf.lfFaceName, lf.lfOutPrecision);
	OutputDebugStr(s);*/

    UINT uicodepage = CP_ACP;
	if( lf.lfCharSet )
	{
		DWORD dw = {0};
		dw =	MAKEWORD( lf.lfCharSet, 0 );
		CHARSETINFO chsi;
		::ZeroMemory( &chsi, sizeof(CHARSETINFO) );
		if( ::TranslateCharsetInfo( (DWORD*)(dw), &chsi, TCI_SRCCHARSET ) && ::IsValidCodePage( chsi.ciACP ) )
			uicodepage = chsi.ciACP;
	}

	CharacterRange charRanges[1] = { CharacterRange(0, subCount)};

//const StringFormat* pGenericFormat = StringFormat::GenericTypographic();
//StringFormat strFormat = pGenericFormat;


	StringFormat strFormat;
	/*strFormat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces | StringFormatFlagsNoFontFallback);*/

	strFormat.SetMeasurableCharacterRanges(1, charRanges);
	INT count = strFormat.GetMeasurableCharacterRangeCount();

	Region* pRegion;
	pRegion = ::new Region[count];


	int nlen_unicod = 0;
	nlen_unicod = ::MultiByteToWideChar( uicodepage, MB_PRECOMPOSED, str, -1, NULL, NULL );
	LPWSTR pwstr = 0;
	pwstr = new WCHAR[ nlen_unicod ];
	BOOL retval = FALSE;
	if( ::MultiByteToWideChar( uicodepage, MB_PRECOMPOSED, str, -1, pwstr, nlen_unicod) )	
		graphics.MeasureCharacterRanges(pwstr, -1, &font, boundRect, &strFormat, count, pRegion);
	
	if( pwstr  )
		delete[] pwstr; pwstr = 0;

	pRegion[0].GetBounds(&boundRect, &graphics);
	::delete[] pRegion;

	rect.left = boundRect.X; 
	rect.top  = boundRect.Y;
	rect.right= rect.left+boundRect.Width; 
	rect.bottom=rect.top+ boundRect.Height;

}

double	CParser::XConst::calc( )
{
	if(m_bDouble)
		return m_Const.m_dConst;
	else
		return (double)m_Const.m_nConst;
}
RECT CParser::XConst::move_cursor(MovingDirection dir)
{
	XExprEntry** entry = pParser->m_SortedExprElems.ptr();
	entry += pParser->m_ptCursorPos.x;
	char *str =0;
	get_string(&str);
	CString s = str;
	delete[] str;
	int length = s.GetLength();


	switch(dir)
	{
	case mdLeft:
		if(m_nCursorPos == 1)
		{
			if(pParser->m_ptCursorPos.x ==0)
			{
				return this->m_rtEntry;
			}
			else
			{
				pParser->m_ptCursorPos.x--;
				entry--;
				pParser->m_pCursorOwner = *entry;
				RECT r;
				pParser->m_SortedExprElems.ptr()[pParser->m_ptCursorPos.x]->get_rect(&r);
				return r;
			}
		}
		else
		{
			this->m_nCursorPos--;
			pParser->m_ptCursorPos.y = m_nCursorPos;
			RECT r = m_rtEntry; 
			pParser->measure_substring(s.GetBuffer(),m_nCursorPos,r);
			return r;
		}
		break;
	case mdRight:
		if(m_nCursorPos >= length)
		{
			if(pParser->m_ptCursorPos.x ==pParser->m_SortedExprElems.size()-1)
			{
				return this->m_rtEntry;
			}
			else
			{
				pParser->m_ptCursorPos.x++;
				
				entry++;
				pParser->m_pCursorOwner = (*entry);
				RECT r;
				(*entry)->set_cursor_offset(1);
				(*entry)->get_rect(&r);
				return r;
			}
			
		}
		else
		{
			m_nCursorPos++;
			pParser->m_ptCursorPos.y =m_nCursorPos;
			RECT r = m_rtEntry; 
			pParser->measure_substring(s.GetBuffer(),m_nCursorPos ,r);
			return r;

		}
		break;
	}
	return this->m_rtEntry;
}
void	CParser::XConst::calc_rect( RECT *prect )
{
	if( !pParser )
		throw -2;

    if( !pParser->m_hdc )
		throw 13;
	char *pvalue = 0;
	get_string( &pvalue );
	if( pvalue )
	{
		pParser->calc_rect( pvalue, prect );
	}
	delete[] pvalue;
	::CopyRect( &m_rtEntry, prect );
}

BOOL	CParser::XConst::draw( )
{
	//if( ::IsRectEmpty( &m_ConstRect ) )
	//	return FALSE;

	if( !pParser )
		throw -2;

	if(	!pParser->m_hdc )
		throw 13;

	char *pvalue = 0;
	get_string( &pvalue );
	pParser->draw_text( pvalue, &m_rtEntry );
	if( pvalue )
	{
		delete[] pvalue;
		pvalue = 0;
	}

	return TRUE;
}


//// XNamedConst Class Implementation 
double	CParser::XNamedConst::calc( )
{
    return get_value( );
}

void	CParser::XNamedConst::calc_rect( RECT *prect )
{
	if( !pParser )
		throw -2;

    if( !pParser->m_hdc )
		throw 13;

	switch( m_nCode )
	{
	case NCONST_PI:
		{
			HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
			if( !hFont )						 
				throw 13;
			LOGFONT lf = {0};
			::GetObject( hFont, sizeof( lf ), &lf);
			lf.lfCharSet = GREEK_CHARSET;
			hFont = ::CreateFontIndirect( &lf );
			if( !hFont )
				throw 13;
			hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
			char str[2] = {0};
			str[0] = (char)0xF0;		
			pParser->calc_rect( str, prect );		
			::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
			hFont = 0;
		}
		break;
	case NCONST_EXP:
		pParser->calc_rect( _T("e"), prect );			
		break;
	}
	::CopyRect( &m_rtEntry, prect );
}
CString CParser::XNamedConst::get_graphics()
{
	return m_strType;
}

BOOL	CParser::XNamedConst::draw( )
{
	//if( ::IsRectEmpty( &m_ConstRect ) )
	//	return FALSE;

	if( !pParser )
		throw -2;

	if(	!pParser->m_hdc )
		throw 13;

	switch( m_nCode )
	{
	case NCONST_PI:
		{
			HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
			if( !hFont )						 
				throw 13;

			LOGFONT lf = {0};
			::GetObject( hFont, sizeof( lf ), &lf);
			lf.lfCharSet = GREEK_CHARSET;
			hFont = ::CreateFontIndirect( &lf );
			if( !hFont )
				throw 13;
			hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
			char str[2] = {0};
			str[0] = (char)0xF0;		
			pParser->draw_text( str, &m_rtEntry );
			::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
			hFont = 0;
		}
		break;
	case NCONST_EXP:
		pParser->draw_text( _T("e"), &m_rtEntry );
		break;
	}

	return TRUE;
}

double	CParser::XNamedConst::get_value( )
{				 
    switch( m_nCode )
	{
	case NCONST_PI:
		return 3.1415926535897932384626433832795;
	case NCONST_EXP:
		return 2.7182818284590452354;
	default:
		return 0.;
	}
}


//// XVar Class Implementation 
CParser::XVar::XVar( ) 
{ 
	m_strType = "Var";
	::SetRectEmpty( &m_MainRect );
	m_UpRect = m_LoRect = m_MainRect; 
}

ExprParam *CParser::XVar::get_ExprParam( )
{
    if( !pParser ) 
		throw -2;

	ExprParam *pParam = 0;
	pParam = pParser->m_Params.get( m_lPos );
	return pParam;
}

ParamKey	CParser::XVar::get_ParamKey( )
{
    if( !pParser ) 
		throw -2;

	return pParser->m_Params.get_key( m_lPos );
}
CString CParser::XVar::get_graphics()
{
	if(m_bEnable)
	{
		ParamKey key = get_ParamKey( );
		return "{"+ key.sParamName + "}";
	}
	else return "";

}

double	CParser::XVar::calc( )
{
    double dRes = 0.;
	ExprParam *pParam = 0;
	pParam = get_ExprParam( );
	ParamKey key = get_ParamKey( );

	if( pParam && pParam->bNecessaryToValidate )
		{
			if( pParam->bValidate )
			{
				if( key.bArray )
				{
					if( !pParser->m_bErrorEvalArrayFunc )
					{
						if( pParam->lsize != pParser->m_lArrayLength && pParser->m_lArrayLength != 1 )
							pParser->m_bErrorEvalArrayFunc = TRUE;
						else
						{
							pParser->m_lArrayLength = pParam->lsize;
							dRes = pParam->get_value( pParser->m_lArrayValueIndex );
						}
					}
				}
				else
					dRes = pParam->get_value( );
			}
			else
				throw 12;
		}
		else
			dRes = 0.;

	return dRes;
}

void	CParser::XVar::calc_rect( RECT *prect )
{
	if( !pParser )
		throw -2;

	if( !(pParser->m_hdc) )
		throw 13;

	ExprParam *pParam = 0;
	pParam = get_ExprParam( );
	if( !pParam || !(pParam->pstParamExInfo))
		throw 13;

	if( pParam->pstParamExInfo->lGreekSmb != -1 )
	{
		HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
		if( !hFont )						 
			throw 13;
		LOGFONT lf = {0};
		::GetObject( hFont, sizeof( lf ), &lf);
		lf.lfCharSet = GREEK_CHARSET;
		hFont = ::CreateFontIndirect( &lf );
		if( !hFont )
			throw 13;
		hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
		char str[2] = {0};
		str[0] = (char)pParam->pstParamExInfo->lGreekSmb;
		
		pParser->calc_rect( str, prect );
				
		::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
		hFont = 0;
	}
	else if( pParam->pstParamExInfo->psGraphics ) 
		pParser->calc_rect( pParam->pstParamExInfo->psGraphics, prect );
	else
		throw 13;	// not set lGreekSmb and psGraphics
	::CopyRect( &m_MainRect, prect );
	
	m_LoRect.left = m_LoRect.right = m_UpRect.left = m_UpRect.right = prect->right /*+ 1*/;
	m_UpRect.top = m_UpRect.bottom = prect->top;
	m_LoRect.top = m_LoRect.bottom = prect->bottom;	
	
	HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
	if( !hFont )
		throw 13;
	LOGFONT lf = {0};
	::GetObject( hFont, sizeof( lf ), &lf);
	lf.lfHeight = (LONG)(lf.lfHeight * 10. / 15. + 0.5); // 10/15
	lf.lfWidth = 0;
	hFont = ::CreateFontIndirect( &lf );
	if( !hFont )
		throw 13;
	hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
    
	if( pParam->pstParamExInfo->psUpIndex )
		pParser->calc_rect( pParam->pstParamExInfo->psUpIndex, &m_UpRect, FALSE );
	if( pParam->pstParamExInfo->psLoIndex )
		pParser->calc_rect( pParam->pstParamExInfo->psLoIndex, &m_LoRect, FALSE );

	::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
	hFont = 0;
    
	/*_vert_centering_rect( &m_UpRect, prect->top );
	_vert_centering_rect( &m_LoRect, prect->bottom );

	// [vanek] : выдерживаем сииметрию относительно центра - 01.06.2004
	RECT TempRect = {0};
	//::UnionRect( &TempRect, &m_UpRect, &m_LoRect );
	int nmax_dy = 0;
	nmax_dy = max( max( m_MainRect.top - m_UpRect.top, m_LoRect.bottom - m_MainRect.bottom ) , 0);
	TempRect.left = min( m_LoRect.left, m_UpRect.left );
	TempRect.right = max( m_LoRect.right, m_UpRect.right );
	TempRect.top = m_MainRect.top - nmax_dy;
	TempRect.bottom = m_MainRect.bottom + nmax_dy;
	::UnionRect( prect, prect, &TempRect );*/

	//
	int ncenter_y = 0;
	ncenter_y = (int)( m_MainRect.top + (m_MainRect.bottom - m_MainRect.top) / 2. + 0.5);
	int rect_height = 0;    
	// up rect
	rect_height = m_UpRect.bottom - m_UpRect.top;
	m_UpRect.bottom = ncenter_y;
	m_UpRect.top = m_UpRect.bottom - rect_height;
	// lo rect
	rect_height = m_LoRect.bottom - m_LoRect.top;
	m_LoRect.top = ncenter_y;
	m_LoRect.bottom = m_LoRect.top + rect_height;

	// [vanek] : выдерживаем сииметрию относительно центра - 01.06.2004
	RECT TempRect = {0};
	int nmax_dy = 0;
	nmax_dy = max( max( m_MainRect.top - m_UpRect.top, m_LoRect.bottom - m_MainRect.bottom ) , 0);
	TempRect.left = min( m_LoRect.left, m_UpRect.left );
	TempRect.right = max( m_LoRect.right, m_UpRect.right );
	TempRect.top = m_MainRect.top - nmax_dy;
	TempRect.bottom = m_MainRect.bottom + nmax_dy;
	::UnionRect( prect, prect, &TempRect );
	//

    ::CopyRect( &m_rtEntry, prect );

}

BOOL	CParser::XVar::draw( )
{
	//if( ::IsRectEmpty( &m_MainRect ) )//|| ::IsRectEmpty( &m_LoRect ) || ::IsRectEmpty( &m_UpRect ) )
	//	return FALSE;

	if( !pParser )
		throw -2;

	if( !(pParser->m_hdc) )
		throw 13;

	ExprParam *pParam = 0;
	pParam = get_ExprParam( );
	if( !pParam || !(pParam->pstParamExInfo))
		throw 13;

	if( pParam->pstParamExInfo->lGreekSmb != -1 )
	{
		HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
		if( !hFont )						 
			throw 13;
		LOGFONT lf = {0};
		::GetObject( hFont, sizeof( lf ), &lf);
		lf.lfCharSet = GREEK_CHARSET;   
		hFont = ::CreateFontIndirect( &lf );
		if( !hFont )
			throw 13;
		hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
		char str[2] = {0};
		str[0] = (char)pParam->pstParamExInfo->lGreekSmb;
		pParser->draw_text( str, &m_MainRect );
		::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
		hFont = 0;
	}
	else if( pParam->pstParamExInfo->psGraphics ) 
		pParser->draw_text( pParam->pstParamExInfo->psGraphics, &m_MainRect );
	else
		throw 13;	// not set lGreekSmb and psGraphics

    HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
	if( !hFont )
		throw 13;
	LOGFONT lf = {0};
	::GetObject( hFont, sizeof( lf ), &lf);
	lf.lfHeight = (LONG)(lf.lfHeight * 10. / 15. + 0.5); // 10/15
	lf.lfWidth = 0;
	hFont = ::CreateFontIndirect( &lf );
	if( !hFont )
		throw 13;
	hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
    
	if( pParam->pstParamExInfo->psUpIndex )
		pParser->draw_text( pParam->pstParamExInfo->psUpIndex, &m_UpRect );
	if( pParam->pstParamExInfo->psLoIndex )
		pParser->draw_text( pParam->pstParamExInfo->psLoIndex, &m_LoRect );

	::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
	hFont = 0;
    
	return TRUE;
}


//// XUnkSymbol Class Implementation 
double	CParser::XUnkSymbol::calc( )
{
	if( !pParser )
		throw 0;

	throw pParser->m_iErrorCode;	
}

BOOL	CParser::XUnkSymbol::draw( )
{
	if( !pParser )
		throw -2;

	CString *pString = &(pParser->m_UnkSymbols.get( m_lPos ));
	if( pString )
	{
		pParser->draw_text( pString->GetBuffer( pString->GetLength( ) ), &m_rtEntry );
		pString->ReleaseBuffer( );
		pString = 0;
	}
	
    return TRUE;
}

void	CParser::XUnkSymbol::calc_rect( RECT *prect )
{
    if( !pParser )
		throw -2;

	CString *pString = &(pParser->m_UnkSymbols.get( m_lPos ));
	if( pString )
	{
		pParser->calc_rect( pString->GetBuffer( pString->GetLength( ) ), prect );
		pString->ReleaseBuffer( );
		pString = 0;
		::CopyRect( &m_rtEntry, prect );
	}	
}


//// XEmptyOperation Class Implementation 
double	CParser::XEmptyOperation::calc( )
{
	if( !pParser )
		throw -2;

	throw pParser->m_iErrorCode;
	return 0.;	
}

BOOL	CParser::XEmptyOperation::draw( )
{
	//return TRUE;
	return _draw__dyadic_operation( 0, 0 );	
}

void	CParser::XEmptyOperation::calc_rect( RECT *prect )
{
	_calc_rect_for_dyadic_operation( 0, 0, prect );	
    return;
}


//// XMissingOperand Class Implementation 
double	CParser::XMissingOperand::calc( )
{
	if( !pParser )
		throw -2;

	throw pParser->m_iErrorCode;
	return 0.;
}

BOOL	CParser::XMissingOperand::draw( )
{
	return TRUE;
}

void	CParser::XMissingOperand::calc_rect( RECT *prect )
{
	// doing nothing
	return;
}


//// XUnkFunction Class Implementation 
double	CParser::XUnkFunction::calc( )
{
	if( !pParser )
		throw -2;

	throw pParser->m_iErrorCode;
	return 0.;
}

BOOL	CParser::XUnkFunction::draw( )
{
	if( !pParser )
		throw -2;

	BOOL	bRes = FALSE;
	CString *pString = &(pParser->m_UnkFuncs.get( m_lPos ));
	if( pString )
	{
		bRes = _draw_unary_operation( pString->GetBuffer( pString->GetLength( ) ), &m_rtEntry );
		pString->ReleaseBuffer( );
		pString = 0;
	}

	return bRes;
}

void	CParser::XUnkFunction::calc_rect( RECT *prect )
{
	if( !pParser )
		throw -2;

	CString *pString = &(pParser->m_UnkFuncs.get( m_lPos ));
	if( pString )
	{
		_calc_rect_for_unary_operation( pString->GetBuffer( pString->GetLength( ) ), &m_rtEntry, prect );
		pString->ReleaseBuffer( );
		pString = 0;
	}
}


//// XBrackets Class Implementation 
double	CParser::XBrackets::calc( )
{
	if( !is_left_bracket( ) )
		throw 9; // no '(' or '['

	double res = __super::calc( );

	if( !is_right_bracket( ) )
		throw 8;	// no ')' or ']'

	return res;
}

BOOL	CParser::XBrackets::draw( )
{
	if( !pParser )
		throw -2;

	if( !draw_left_bracket( ) )
		return FALSE;

	if( !__super::draw( ) )
		return FALSE;

	if( !draw_right_bracket( ) )
		return FALSE;
	
	return TRUE;
}

void	CParser::XBrackets::calc_rect( RECT *prect )
{
    if( !pParser )
		throw -2;

	long lbottom = prect->bottom;

	calc_rect_for_left_bracket( prect );
	
	__super::calc_rect( prect );

	::UnionRect( prect, &m_rtLBracket, prect );
	calc_rect_for_right_bracket( prect, lbottom );

	::UnionRect( prect, prect, &m_rtRBracket );
	::CopyRect( &m_rtEntry, prect );
}

BOOL	CParser::XBrackets::is_left_bracket( ) 
{ 
	return (m_iLBracket == '(' || m_iLBracket == '['); 
}

BOOL	CParser::XBrackets::is_right_bracket( ) 
{
	return (m_iRBracket == ')' || m_iRBracket == ']'); 
}

void	CParser::XBrackets::calc_rect_for_left_bracket( RECT *prect )
{
	if( !pParser )
		throw -2;

    if( prect && is_left_bracket( ) )
	{
        char str[2] = {0};
		str[0] = m_iLBracket;
		pParser->calc_rect( str, prect );
		::CopyRect( &m_rtLBracket, prect );
		::OffsetRect( prect, (m_rtLBracket.right - m_rtLBracket.left), 0 );
	}
	else
		::SetRectEmpty( &m_rtLBracket );
}

void	CParser::XBrackets::calc_rect_for_right_bracket( RECT *prect, long lbottom )
{
	if( !pParser )
		throw -2;

    if( prect && is_right_bracket( ) )
	{
        char str[2] = {0};
		str[0] = m_iRBracket;
		m_rtRBracket.top = m_rtRBracket.bottom = lbottom;
		m_rtRBracket.left = m_rtRBracket.right = prect->right;
		pParser->calc_rect( str, &m_rtRBracket );
	}
	else
		::SetRectEmpty( &m_rtRBracket );
}

BOOL	CParser::XBrackets::draw_left_bracket( )
{
	if( !pParser )
		throw -2;
	
	char str[2] = {0};

	if( ::IsRectEmpty( &m_rtLBracket ) )
		return FALSE;
	
	str[0] = m_iLBracket;
	pParser->draw_text( str, &m_rtLBracket );
	return TRUE;        
}

BOOL	CParser::XBrackets::draw_right_bracket( )
{
	if( !pParser )
		throw -2;
	
	char str[2] = {0};

	if( ::IsRectEmpty( &m_rtRBracket ) )
		return FALSE;

	str[0] = m_iRBracket;
	pParser->draw_text( str, &m_rtRBracket );
	return TRUE;
}


//// XFuncBrackets Class Implementation 
CParser::XFuncBrackets::XFuncBrackets( ) 
{
	 m_nidx_arg = 0; 
	 m_lbottom = 0;
}

double CParser::XFuncBrackets::calc( )
{
	if( m_nidx_arg >= get_curr_args_count() )
		m_nidx_arg = 0;

	if( !m_nidx_arg && !is_left_bracket( ) )
		throw 9; // no '(' or '['

	double fResult = calc_arg( m_nidx_arg );

	if( (m_nidx_arg == (get_curr_args_count() - 1)) && !is_right_bracket( ) )
		throw 8;	// no ')' or ']'

	m_nidx_arg ++;

	return fResult;
}

void	CParser::XFuncBrackets::calc_rect( RECT *prect )
{
	if( !pParser )
		throw -2;

    if( m_nidx_arg >= get_curr_args_count() )
		m_nidx_arg = 0;

	if( !m_nidx_arg )
	{
		::SetRectEmpty( &m_rtEntry );
		m_rtLBracket = m_rtRBracket = m_rtEntry;
	}

	if( !m_nidx_arg )
	{	// считаем открывающую скобку
		m_lbottom = prect->bottom;
		calc_rect_for_left_bracket( prect );
	}

	calc_arg_rect( m_nidx_arg, prect );
	::UnionRect( &m_rtEntry, prect, &m_rtEntry );

	if( (m_nidx_arg == (get_curr_args_count() - 1)) )
	{	//считаем закрывающую скобку и формируем результирующий прямоугольник  (по аналогии с XBrackets::calc_rect)
    	::UnionRect( prect, &m_rtLBracket, prect );
        calc_rect_for_right_bracket( prect, m_lbottom );
        ::UnionRect( prect, prect, &m_rtRBracket );
		::CopyRect( &m_rtEntry, prect );
	}

	m_nidx_arg ++;
}

BOOL	CParser::XFuncBrackets::draw( )
{ 
	if( !pParser )  
		throw -2;

    if( m_nidx_arg >= get_curr_args_count())
		m_nidx_arg = 0;

	if( !m_nidx_arg )
	{
		// отрисовываем открывающую скобку
		if( !draw_left_bracket( ) )
			return FALSE;
	}
	
	if( !draw_arg( m_nidx_arg ) )
		return FALSE; 
	
	if( (m_nidx_arg == (get_curr_args_count() - 1)) )
	{	// отрисовываем закрывающую скобку
        if( !draw_right_bracket( ) )
			return TRUE;
	}

	m_nidx_arg ++;
	return TRUE;
}


//// XEqual Class Implementation 
double	CParser::XEqual::calc( )
{
	__super::calc( );
	return __super::calc( );
}

BOOL	CParser::XEqual::draw( )
{
	return _draw__dyadic_operation( "=", &m_rtEntry );
}

CString CParser::XEqual::get_graphics()
{
	return m_strType;
}


void	CParser::XEqual::calc_rect( RECT *prect )
{
	_calc_rect_for_dyadic_operation( "=", &m_rtEntry, prect );
}
RECT CParser::XEqual::move_cursor(MovingDirection dir)
{
	RECT r;
	XExprEntry** entry = pParser->m_SortedExprElems.ptr();
	entry+=pParser->m_ptCursorPos.x;
	switch (dir)
	{
	case mdLeft:
		return this->m_rtEntry;
	case mdRight:
		entry++;
		pParser->m_ptCursorPos.x++;
		pParser->m_ptCursorPos.y = (*entry)->set_cursor_offset(1);
		(*entry)->get_rect(&r);
		pParser->m_pCursorOwner = *entry;
		return r;
	}
	return this->m_rtEntry;
}

//// XAdd Class Implementation 
double	CParser::XAdd::calc( )
{
	double	c1 = __super::calc( ),
			c2 = __super::calc( );	
	return ( c1 + c2 );	
}

BOOL	CParser::XAdd::draw( )
{
	return _draw__dyadic_operation( "+", &m_rtEntry );	
}

void	CParser::XAdd::calc_rect( RECT *prect )
{
	_calc_rect_for_dyadic_operation( "+", &m_rtEntry, prect );
}


//// XSub Class Implementation 
double	CParser::XSub::calc( )
{
	double c1 = __super::calc( ), c2 = __super::calc( ); 
	return ( c1 - c2 );  	
}

BOOL	CParser::XSub::draw( )
{
	return _draw__dyadic_operation( "-", &m_rtEntry );	
}

void	CParser::XSub::calc_rect( RECT *prect )
{
    _calc_rect_for_dyadic_operation( "-", &m_rtEntry, prect );	
}


//// XMul Class Implementation 
double	CParser::XMul::calc( )
{
	double c1 = __super::calc( ), c2 = __super::calc( );
	return ( c1 * c2 );  	
}

BOOL	CParser::XMul::draw( )
{
    return _draw__dyadic_operation( "*", &m_rtEntry );	
}

void	CParser::XMul::calc_rect( RECT *prect )
{
	_calc_rect_for_dyadic_operation( "*", &m_rtEntry, prect );		
}


//// XDiv Class Implementation 
double	CParser::XDiv::calc( )
{
	double c1 = __super::calc( ), c2 = __super::calc( );

	if( !c2 )
		throw 5;	// ZERO

	return ( c1 / c2 );	
}

BOOL	CParser::XDiv::draw( )
{
	return _draw__dyadic_operation( "/", &m_rtEntry );	
}

void	CParser::XDiv::calc_rect( RECT *prect )
{
    _calc_rect_for_dyadic_operation( "/", &m_rtEntry, prect );				
}


//// XMod Class Implementation 
double	CParser::XMod::calc( )
{
	double c1 = __super::calc( ), c2 = __super::calc( );
	if( !c2 )
		throw 5;	// ZERO
	return fmod( c1, c2 ); 	
}

BOOL	CParser::XMod::draw( )
{
	return _draw__dyadic_operation( "%", &m_rtEntry );	
}

void	CParser::XMod::calc_rect( RECT *prect )
{
	_calc_rect_for_dyadic_operation( "%", &m_rtEntry, prect );				
}


//// XPow Class Implementation 
double	CParser::XPow::calc( )
{
	double	c1 = __super::calc( ), 
			c2 = __super::calc( );
	//return ( exp( c2 * log( c1 ) ) );
	return pow( c1 , c2 );
}

BOOL	CParser::XPow::draw( )
{
	return _draw__dyadic_operation( "^", &m_rtEntry );	
}

void	CParser::XPow::calc_rect( RECT *prect )
{
	_calc_rect_for_dyadic_operation( "^", &m_rtEntry, prect );			
}


//// XUMinus Class Implementation 
double	CParser::XUMinus::calc( )
{
	return ( (double) -1. * __super::calc( ) );	
}

BOOL	CParser::XUMinus::draw( )
{
	return _draw_unary_operation( "-", &m_rtEntry );
}

void	CParser::XUMinus::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "-", &m_rtEntry, prect );	
}


//// XUPlus Class Implementation
double	CParser::XUPlus::calc( )
{
	return ( (double) __super::calc( ) ); 
}

BOOL	CParser::XUPlus::draw( )
{
	return _draw_unary_operation( "+", &m_rtEntry );
}

void	CParser::XUPlus::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "+", &m_rtEntry, prect );
}


//// XSin Class Implementation
double	CParser::XSin::calc( )
{
	return sin( __super::calc( ) );
}

BOOL	CParser::XSin::draw( )
{
	return _draw_unary_operation( "sin", &m_rtEntry );	
}

void	CParser::XSin::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "sin", &m_rtEntry, prect );
}


//// XCos Class Implementation
double	CParser::XCos::calc( )
{
	return cos( __super::calc( ) );	
}

BOOL	CParser::XCos::draw( )
{
	return _draw_unary_operation( "cos", &m_rtEntry );	
}

void	CParser::XCos::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "cos", &m_rtEntry, prect );	
}


//// XAsin Class Implementation
double	CParser::XAsin::calc( )
{
	return asin( __super::calc( ) );
}

BOOL	CParser::XAsin::draw( )
{
	return _draw_unary_operation( "asin", &m_rtEntry );	
}

void	CParser::XAsin::calc_rect( RECT *prect )
{
    _calc_rect_for_unary_operation( "asin", &m_rtEntry, prect );	
}


//// XAcos Class Implementation
double	CParser::XAcos::calc( )
{
	return acos( __super::calc( ) );
}

BOOL	CParser::XAcos::draw( )
{
	return _draw_unary_operation( "acos", &m_rtEntry );	
}

void	CParser::XAcos::calc_rect( RECT *prect )
{
    _calc_rect_for_unary_operation( "acos", &m_rtEntry, prect );	
}


//// XExp Class Implementation
double	CParser::XExp::calc( )
{
	return exp( __super::calc( ) );
}

BOOL	CParser::XExp::draw( )
{
	return _draw_unary_operation( "exp", &m_rtEntry );	
}

void	CParser::XExp::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "exp", &m_rtEntry, prect );
}


//// XLog10 Class Implementation
double	CParser::XLog10::calc( )
{
	return log10( __super::calc( ) );
}

BOOL	CParser::XLog10::draw( )
{
	return _draw_unary_operation( "lg", &m_rtEntry );	
}

void	CParser::XLog10::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "lg", &m_rtEntry, prect );	
}


//// XLog Class Implementation
double	CParser::XLog::calc( )
{
	return log( __super::calc( ) );
}

BOOL	CParser::XLog::draw( )
{
	return _draw_unary_operation( "ln", &m_rtEntry );	
}

void	CParser::XLog::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "ln", &m_rtEntry, prect );	
}


//// XTan Class Implementation
double	CParser::XTan::calc( )
{
	return tan( __super::calc( ) );
}

BOOL	CParser::XTan::draw( )
{
	return _draw_unary_operation( "tg", &m_rtEntry );	
}

void	CParser::XTan::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "tg", &m_rtEntry, prect );	
}


//// XAtan Class Implementation
double	CParser::XAtan::calc( )
{
	return atan( __super::calc( ) );
}

BOOL	CParser::XAtan::draw( )
{
	return _draw_unary_operation( "atg", &m_rtEntry );	
}

void	CParser::XAtan::calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "atg", &m_rtEntry, prect );	
}


//// XSqrt Class Implementation
double	CParser::XSqrt::calc( )
{
	return sqrt( __super::calc( ) );
}

BOOL	CParser::XSqrt::draw( )
{
	if( ::IsRectEmpty( &m_rtEntry ) )
		return FALSE;

	if( !pParser )
		throw -2;

	HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
	if( !hFont )						 
		throw 13;
	LOGFONT lf = {0};
	::GetObject( hFont, sizeof( lf ), &lf);
	strcpy( lf.lfFaceName, "Symbol" );
	hFont = ::CreateFontIndirect( &lf );
	if( !hFont )
		throw 13;
	hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
	char str[2] = {0};
	str[0] = (char)0xD6;
	BOOL bRes = pParser->draw_text( str, &m_rtEntry );
    ::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
	hFont = 0;

	return bRes ? __super::draw( ) : FALSE;
}

void	CParser::XSqrt::calc_rect( RECT *prect )
{
	if( !pParser )
		throw -2;

	HFONT hFont = (HFONT) ::GetCurrentObject( pParser->m_hdc, OBJ_FONT );
	if( !hFont )						 
		throw 13;
	LOGFONT lf = {0};
	::GetObject( hFont, sizeof( lf ), &lf);
	strcpy( lf.lfFaceName, "Symbol" );
	hFont = ::CreateFontIndirect( &lf );
	if( !hFont )
		throw 13;
	hFont = (HFONT) ::SelectObject( pParser->m_hdc, hFont );
	char str[2] = {0};
	str[0] = (char)0xD6;
	
	long	lBottom = prect->bottom;
	pParser->calc_rect( str, prect );  


	::CopyRect( &m_rtEntry, prect );
	prect->top = prect->bottom = lBottom;
	prect->left = prect->right;
	::DeleteObject( ::SelectObject( pParser->m_hdc, hFont ) );
	hFont = 0;
		
	__super::calc_rect( prect ); 
	::UnionRect( prect, &m_rtEntry, prect );
}


//// XSum Class Implementation
void	CParser::XSum::on_calc_element( double fResult )
{
    m_fResult += fResult;	
	__super::on_calc_element( fResult );
}

BOOL	CParser::XSum::on_draw( )
{
	return _draw_unary_operation( "sum", &m_rtEntry );
}

void	CParser::XSum::on_calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "sum", &m_rtEntry, prect );
}


//// XAvg Class Implementation
void	CParser::XAvg::on_calc_element( double fResult )
{
    m_fResult += fResult;	
	m_lCount ++;
	__super::on_calc_element( fResult );
}

void CParser::XAvg::on_begin_calc( )
{
	m_lCount = 0;
	__super::on_begin_calc();
}

void  CParser::XAvg::on_end_calc( )
{
    if( m_lCount == 0 )  
		throw 5;	

	m_fResult /= m_lCount;
}


BOOL	CParser::XAvg::on_draw( )
{
	return _draw_unary_operation( "avg", &m_rtEntry );
}

void	CParser::XAvg::on_calc_rect( RECT *prect )
{
	_calc_rect_for_unary_operation( "avg", &m_rtEntry, prect );
}

//// XSubExpr Class Implementation
CParser::XSubExpr::XSubExpr( )
{
}

CParser::XSubExpr::~XSubExpr( )
{
    long lsize = m_entries.size( );
	for( long l = 0; l < lsize; l++ )
		delete *(m_entries.ptr() + l);
    m_entries.free( ); 
}
EntryType CParser::XSubExprList::GetType() 
{ 
	return ET_SUBEX;
}

//// XSubExprList Class Implementation

CParser::XSubExprList::XSubExprList( ) 
{ 
	m_ncurr_expr_idx = 0; 
	m_strSeparator = _T(" "); 
	m_strType = "SubExprList";
}

CParser::XSubExprList::~XSubExprList( ) 
{ 
	long lsize = m_sub_exprs.size( );
	for( long l = 0; l < lsize; l++ )
		delete *(m_sub_exprs.ptr() + l);
	m_sub_exprs.free( );
	m_separator_rects.free( ); 
}

int CParser::XSubExprList::get_expr_count_ex( )
{
	XSubExpr* sub;
	XExprEntry* entry;

	int n = this->m_sub_exprs.size();

	int nRet =0;
	for(int i =0;i<n;i++)
	{
		sub = m_sub_exprs.ptr()[i];	
		int m = sub->m_entries.size();
		for(int j=0;j<m;j++)
		{
			entry = sub->m_entries.ptr()[j];
			if(entry->GetType()==ET_SUBEX)
			{
				nRet+=((XSubExprList*)entry)->get_expr_count_ex();
			}
			else if(entry->GetType()==ET_BRACKET)
				nRet+=2;
			else
				nRet++;
		}

	}
	return nRet;
}
void CParser::XSubExprList::copy(XExprEntry** entries, int& from)
{
	XSubExpr* sub;
	XExprEntry* entry;

	int n = this->m_sub_exprs.size();
	for(int i =0;i<n;i++)
	{
		sub = m_sub_exprs.ptr()[i];	
		int m = sub->m_entries.size();
		for(int j=0;j<m;j++)
		{
			entry = sub->m_entries.ptr()[j];
			if(entry->GetType()==ET_SUBEX)
			{
				((XSubExprList*)entry)->copy(entries,from);
			}
			else if(entry->GetType()==ET_BRACKET)
			{
				XExprEntry* e1, *e2;
				pParser->create_dummy_bracket_entry((XBrackets*)entry->get_this(),&e1,&e2);
				entries[from]=e1;
				from++;
				entries[from]=e2;
				from++;
			}
			else
			{
				entries[from] = entry;
				from++;
			}
		}

	}


}

double CParser::XSubExprList::calc( )
{
	double dResult = 0.;

	if( !pParser )
		throw -2;

	if( !m_sub_exprs.size( ) )
		//throw 666; ///! внимание: установить нормальный код ошибки ! возвращаем код ошибки: список аргументов пуст
		return 0; // пока не будем считать это ошибкой

	if( m_ncurr_expr_idx >= m_sub_exprs.size( ) )
		m_ncurr_expr_idx = 0;

	// запоминаем текущий элемент выражения
	CParser::XExprEntry **pcurr_entry = pParser->m_pEntry;
	
	// делаем текущим соответствующее подвыражение 
	pParser->m_pEntry = (m_sub_exprs.ptr()[ m_ncurr_expr_idx])->m_entries.ptr();
	
	// вычисляем подвыражение
	dResult = (*pParser->m_pEntry)->calc( );
    
	// восстанавливаем текущий элемент выражения
    pParser->m_pEntry = pcurr_entry;

	// обновляем индекс текущего подвыражения
	m_ncurr_expr_idx ++;

	return dResult;
}

void	CParser::XSubExprList::calc_rect( RECT *prect )
{
	if( !pParser )
		throw -2;

    if( !m_sub_exprs.size( ) )
		//throw 666; ///! внимание: установить нормальный код ошибки ! возвращаем код ошибки: список аргументов пуст
		return; // пока не будем считать это ошибкой

	if( m_ncurr_expr_idx >= m_sub_exprs.size( ) )
		// идем заново
		m_ncurr_expr_idx = 0;

	if( !m_ncurr_expr_idx )
	{   // начало обхода выражений
		::SetRectEmpty( &m_rtEntry );
		m_separator_rects.free( );
		m_separator_rects.alloc( m_sub_exprs.size( ) - 1 );	// на один меньше, чем количество подвыражений
	}

	long	lBottom = prect->bottom;

	// запоминаем текущий элемент выражения
	CParser::XExprEntry **pcurr_entry = pParser->m_pEntry;
	
	// делаем текущим соответствующее подвыражение 
	pParser->m_pEntry = (m_sub_exprs.ptr()[ m_ncurr_expr_idx ])->m_entries.ptr();
	
	// обсчитываем результирующий прямоугольник подвыражения
	(*pParser->m_pEntry)->calc_rect( prect );

	if( m_ncurr_expr_idx < m_separator_rects.size( ) )
	{	
		RECT rtSeparator = {0};  
		rtSeparator.top = rtSeparator.bottom = lBottom;
		rtSeparator.left = rtSeparator.right = prect->right;
		pParser->calc_rect( m_strSeparator, &rtSeparator );
		*(m_separator_rects.ptr() + m_ncurr_expr_idx) = rtSeparator;
		::UnionRect( prect, &rtSeparator, prect );
	}
    
	// восстанавливаем текущий элемент выражения
    pParser->m_pEntry = pcurr_entry;

	// обновляем индекс текущего подвыражения
	m_ncurr_expr_idx ++;        

	// обновляем результирующий прямоугольник
    ::UnionRect( &m_rtEntry, prect, &m_rtEntry );
}

BOOL	CParser::XSubExprList::draw( )
{
	BOOL bResult = FALSE;

    if( !pParser )
		throw -2;

	if( !m_sub_exprs.size( ) )
		//throw 66; ///! внимание: установить нормальный код ошибки ! возвращаем код ошибки: список аргументов пуст
		return TRUE; // пока не будем считать это ошибкой

	if( m_ncurr_expr_idx >= m_sub_exprs.size( ) )
		m_ncurr_expr_idx = 0;

	// запоминаем текущий элемент выражения
	CParser::XExprEntry **pcurr_entry = pParser->m_pEntry;
	
	// делаем текущим соответствующее подвыражение 
	pParser->m_pEntry = (m_sub_exprs.ptr()[ m_ncurr_expr_idx ])->m_entries.ptr();
	
	// отрисовываем подвыражение
	bResult = (*pParser->m_pEntry)->draw( );

	if( m_ncurr_expr_idx < m_separator_rects.size( ) )
		// отрисовывам разделитель подвыражений
		bResult = pParser->draw_text( m_strSeparator, (m_separator_rects.ptr() + m_ncurr_expr_idx) );
	    
	// восстанавливаем текущий элемент выражения
    pParser->m_pEntry = pcurr_entry;

	// обновляем индекс текущего подвыражения
	m_ncurr_expr_idx ++;

	return bResult;
}


//// XMultiArgFunc Class Implementation
double	CParser::XMultiArgFunc::calc_arg( int narg )
{
	double fResult = 0.;

	if( narg < get_curr_args_count() )
	{
        if( narg != (get_curr_args_count() - 1) )
		{
			XExprEntry **ppfunc = pParser->m_pEntry;
			fResult = __super::calc( );
			pParser->m_pEntry = ppfunc;
		}
		else
			fResult = __super::calc( );
	}
	else
		throw 15;

	return fResult;
}

void	CParser::XMultiArgFunc::calc_arg_rect( int narg, RECT *prect_arg )
{
    if( narg < get_curr_args_count() )
	{
        if( narg != (get_curr_args_count() - 1) )
		{
			XExprEntry **ppfunc = pParser->m_pEntry;
			__super::calc_rect( prect_arg );
			pParser->m_pEntry = ppfunc;
		}
		else
			__super::calc_rect( prect_arg );
	}
	else
		throw 15; 
}

void	CParser::XMultiArgFunc::calc_rect_for_multiarg_func( char *sfunc,  RECT *prt_func, RECT *prect )
{
    if( !sfunc || !prt_func || !prect )     
		return;

    if( !pParser )
		throw -2;

	long	lBottom = prect->bottom;

	pParser->calc_rect( sfunc, prect );
        
	::CopyRect( prt_func, prect );
	
	prect->top = prect->bottom = lBottom;
	prect->left = prect->right;
	for( int i = 0; i < get_curr_args_count(); i ++ )
	{
		RECT rt_arg = *prect;
		rt_arg.top = rt_arg.bottom = lBottom;
		rt_arg.left = rt_arg.right;
		calc_arg_rect( i, &rt_arg );
        ::UnionRect( prect, prect, &rt_arg );   
	}

	::UnionRect( prect, prt_func, prect );   
}

BOOL	CParser::XMultiArgFunc::draw_multiarg_func( char *sfunc, RECT *prt_func )
{
    if( !sfunc || !prt_func )
		return FALSE;

	if( !pParser )
		throw -2;

	if( !pParser->draw_text( sfunc, prt_func ) )
		return FALSE;

	for( int i = 0; i < get_curr_args_count( ); i ++ )
	{
		if( !draw_arg( i ) )
			return FALSE;
	}

	return TRUE;
}

BOOL	CParser::XMultiArgFunc::draw_arg( int narg )
{
	BOOL bResult = FALSE;

    if( narg < get_curr_args_count() )
	{
        if( narg != (get_curr_args_count() - 1) )
		{
			XExprEntry **ppfunc = pParser->m_pEntry;
			bResult = __super::draw( );
			pParser->m_pEntry = ppfunc;
		}
		else
			bResult = __super::draw( );
	}
	else
		throw 15; 

	return bResult;
}


//// XMin Class Implementation
double CParser::XMin::calc( )
{
    double fresult = 0.;
	int nargs_count = get_curr_args_count( );
	for( int i = 0; i < nargs_count; i ++ )
	{
		double fcurr_res = calc_arg( i );
		if( !i )
			fresult = fcurr_res;
		else 
			fresult = min( fresult, fcurr_res);
	}

	return fresult;
}

void	CParser::XMin::calc_rect( RECT *prect )
{
    calc_rect_for_multiarg_func( "min", &m_rtEntry, prect );
}

BOOL	CParser::XMin::draw( )
{
	return draw_multiarg_func( "min", &m_rtEntry );
}


//// XMax Class Implementation
double CParser::XMax::calc( )
{
    double fresult = 0.;
	int nargs_count = get_curr_args_count( );
	for( int i = 0; i < nargs_count; i ++ )
	{
		double fcurr_res = calc_arg( i );
		if( !i )
			fresult = fcurr_res;
		else 
			fresult = max( fresult, fcurr_res);
	}

	return fresult;
}

void	CParser::XMax::calc_rect( RECT *prect )
{
    calc_rect_for_multiarg_func( "max", &m_rtEntry, prect );
}

BOOL	CParser::XMax::draw( )
{
	return draw_multiarg_func( "max", &m_rtEntry );
}
POINT CParser::GetCursorPosition(void)
{
	POINT p ={0};
	return p;
}

CString CParser::XExprEntry::get_graphics(void)
{
	return m_strType;
};


RECT CParser::XExprEntry::set_cursor_rect(RECT rectDesired)
{
	return m_rtEntry;
}

int CParser::XExprEntry::get_cursor_pos(void)
{
	return m_nCursorPos;
}

int CParser::XExprEntry::set_cursor_offset(int offset)
{
	return m_nCursorPos = offset;
}

int CParser::XConst::set_cursor_offset(int offset)
{
	char* s =0;
	get_string(&s);
	int length = strlen(s);
	delete[] s;
	if(offset ==-1 || offset>length)//to the end of entry string
	{
		offset = length;
		/*pParser->m_ptCursorPos.y = offset;*/
	}
	return __super::set_cursor_offset(offset);
}
void CParser::copy_next(XExprEntry** entries, int& from, int curr)
{
	XExprEntry* e = m_ExprElems.ptr()[curr];
	if(e->GetType()==ET_SUBEX)
	{
		((XSubExprList*)e)->copy(entries, from);
	}
	else if(e->GetType()==ET_BRACKET)
	{
		XExprEntry* e1, *e2;
		create_dummy_bracket_entry((XBrackets*)e->get_this(),&e1,&e2);
		entries[from]=e1;
		from++;
		entries[from]=e2;
		from++;
	}
	else
	{
		entries[from]= e;
		from++;
	}

}
void CParser::sort_entries(void)
{
	XExprEntry *temp, **e1,**e2;

	RECT r1,r2;
	int n = m_SortedExprElems.size();
	for(int i=0;i<n;i++)
	{
		for(int j=n-1;j>i;j--)
		{
			e1 = &(m_SortedExprElems.ptr()[j-1]);
			(*e1)->/*XExprEntry::*/get_rect(&r1);
			e2 =&(m_SortedExprElems.ptr()[j]);
			(*e2)->/*XExprEntry::*/get_rect(&r2);
			if((r2.right<r1.right && r2.right!=0) || r1.right==0) 
			{
				temp = *e2;
				*e2 =*e1;
				*e1 =temp;
			}
		}
	}
	r1.right=1;
	e1 = m_SortedExprElems.ptr();
	n=0;
	while(1)
	{
		(*e1)->/*XExprEntry::*/get_rect(&r1);
		if(r1.right ==0) break;
		e1++;
		n++; 
		if(n>=m_SortedExprElems.size()) break;
	}
	m_SortedExprElems.alloc(n);
}

void CParser::XConst::get_rect(RECT *r)
{
	*r = m_rtEntry;
	char* str =0;
	get_string(&str);
	int len = strlen(str);
	//if(len-1 == m_nCursorPos) return;
	pParser->measure_substring(str,m_nCursorPos , *r);
	delete[] str; 
}



// кол-во элементарных выражений, включая развернутые подвыражения
int CParser::get_expression_count(void)
{
	int n = m_ExprElems.size();
	int nRet =0;
	XExprEntry* entry;
	for(int i=0;i<n;i++)
	{
		entry = m_ExprElems.ptr()[i];
		if(entry->GetType()==ET_SUBEX)
		{
			nRet+=((XSubExprList*)entry)->get_expr_count_ex();
		}
		else if(entry->GetType()==ET_BRACKET)
			nRet+=2;
		else
			nRet++;
	}
	return nRet;
}
void CParser::XDummyBracket::delete_entry()
{
	if(GetType()!=ET_LBRACKET) return __super::delete_entry();
	XExprEntry** entry = pParser->m_SortedExprElems.ptr();
	XExprEntry* e = entry[pParser->m_ptCursorPos.x-1];
	if(e->GetType() == ET_FUNCTION)
	{
		__super::delete_entry();
		return e->delete_entry();
	}
	return __super::delete_entry();
}
#pragma warning( default : 4244 )
