#pragma once

#include <atlstr.h>
#include <math.h>
#include <vector>
#include <stack>
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_ptr.h"
#include "\vt5\awin\misc_map.h"
#include "calc_int.h"

#pragma warning( disable : 4244)

#define INIT_ARRAY_LENGTH	1

inline void	_vert_centering_rect( RECT *pRect, long ly )
{
    if( !pRect )
		return;

	long cy=(pRect->bottom - pRect->top);
	long lhalf = (long)(cy / 2. + 0.5);
	pRect->top = ly - lhalf;
	pRect->bottom = pRect->top + cy;
}

inline void	_horz_centering_rect( RECT *pRect, long lx )
{
	if( !pRect )
		return;
	
	long lhalf = (long)( (pRect->right - pRect->left) / 2. + 0.5 );
	pRect->left = lx - lhalf;
	pRect->right = lx + lhalf;
}

inline long cmp_cstring( CString s1, CString s2 )
{	return s1.Compare( s2 ); };

struct ParamKey
{
	CString sParamName;
	bool	bArray;	// флаг множества значений параметра

	ParamKey() { init(); }

	void init()
	{
		sParamName = _T("");
		bArray = false;
	}
};

inline long cmp_paramkeys( ParamKey param_key1, ParamKey param_key2 )
{
	int cmp = param_key1.sParamName.Compare( param_key2.sParamName ); 
	if( !cmp )
		return !(param_key1.bArray == param_key2.bArray);
	else
		return cmp;
};

// Параметр выражения
struct ExprParam
{
	//double		dValue;
	
	double		*pfValues;
	long		lsize;
	
	bool		bValidate;
	bool		bNecessaryToValidate;
	ParamExInfo	*pstParamExInfo;

	ExprParam()	{ init(); }
	~ExprParam()
	{
		if( pfValues )
			delete[] pfValues; pfValues = 0;
        
		if( pstParamExInfo )
		{
			pstParamExInfo->free( );
			delete pstParamExInfo;
			pstParamExInfo = 0;
		}
	}

	void init()
	{
		//dValue = 0.;
		pfValues = 0;
		lsize = 0;
		
		bValidate = false;
		bNecessaryToValidate = true;
		pstParamExInfo = 0;
	}

	BOOL	set_values( double *pfNewValues, long lArraySize, bool bIsArray )
	{
        if( !pfNewValues || lArraySize == 0)
			return FALSE;					   

		if( pfValues )
			delete[] pfValues; pfValues = 0;

		lsize = lArraySize;
		pfValues = new double[ lsize ];

		::memcpy( pfValues, pfNewValues, sizeof( double ) * lArraySize );
		//bArray = bIsArray;
		return TRUE;
	}

	double get_value( long lIdx = 0 )
	{
		if( !pfValues )
			return 0.;

		/*if( lIdx > 0 && !bArray )
			lIdx = 0;*/

		return (lIdx >= lsize || lIdx < 0)? 0. : pfValues[ lIdx ];
	}

	void set_ex_info( ParamExInfo	*pstExInfo )
	{
		pstParamExInfo->free();		
		if( pstExInfo->psGraphics )
		{
			pstParamExInfo->psGraphics = new char[ strlen( pstExInfo->psGraphics ) + 1];
			strcpy( pstParamExInfo->psGraphics, pstExInfo->psGraphics );
		}
		if( pstExInfo->psUpIndex )
		{
			pstParamExInfo->psUpIndex = new char[ strlen( pstExInfo->psUpIndex ) + 1];
			strcpy( pstParamExInfo->psUpIndex, pstExInfo->psUpIndex );
		}
		if( pstExInfo->psLoIndex )
		{
			pstParamExInfo->psLoIndex = new char[ strlen( pstExInfo->psLoIndex ) + 1];
			strcpy( pstParamExInfo->psLoIndex, pstExInfo->psLoIndex );
		}
		pstParamExInfo->lGreekSmb = pstExInfo->lGreekSmb;
	}
};

inline void ExprParam_ex_free( void* pdata )
{
	delete (ExprParam*)pdata;
}
enum EntryType
{
	ET_UNDEFINED =0, ET_FUNCTION, ET_BRACKET, ET_RBRACKET, ET_LBRACKET, ET_EQUAL, ET_SUBEX, ET_CONST 
};

enum ModesOfOperation{ OM_RELAX = 1, OM_CALCULATE, OM_PAINT , OM_CALCRECT };
enum ParamTypes{ PT_ALL = 1, PT_SIMPLE, PT_ARRAY };
class CParser
{

	// [vanek] : используем GDI+ для коректного отображения (посчета прямоугольников) наклонного текста
	CGDIPlusManager m_gdi_plus_mng;

	enum XTypes{ DELIMITER = 1, FUNCTION, VARIABLE, NUMBER, NAMED_CONST, SUB_EXPR_DELIMITER, UNKNOWN };
	enum XNamedConsts{ NCONST_UNKNOWN = 1, NCONST_PI, NCONST_EXP };
		
	class	XExprEntry
	{
		
	protected:

		int m_nCursorPos;
		bool m_bEnable;


	public:
		RECT m_rtEntry;
		XExprEntry() { ::SetRectEmpty( &m_rtEntry ); m_nCursorPos=-1; m_strType = ""; m_bEnable =true;}
		virtual ~XExprEntry() { }

		CParser *pParser;
		
		void	next( );
		       
		CString m_strType;
		CString			get_type_info(){return m_strType;};
		virtual void*	get_this(){return this;};
		virtual void get_divided_strings(CString& s1, CString& s2)
		{
			s1 = get_graphics();
			s2 ="";
		}
		       
		virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect(  RECT *prect );

		void	_calc_rect_for_dyadic_operation( char *sOp, RECT *prtOp, RECT *prect ); 
		void	_calc_rect_for_unary_operation( char *sOp, RECT *prtOp, RECT *prect ); 

		BOOL	_draw__dyadic_operation( char *sOp, RECT *prtOp ); 
		BOOL	_draw_unary_operation( char *sOp, RECT *prtOp ); 
		virtual void	get_rect(RECT*);
		virtual RECT	move_cursor(MovingDirection dir);
		virtual CString	get_graphics(void);
		virtual RECT	set_cursor_rect(RECT rectDesired);
		virtual int		get_cursor_pos(void);
		virtual int		set_cursor_offset(int offset);
		virtual EntryType GetType(){return ET_UNDEFINED;};
		virtual void	delete_entry()
		{
			m_strType.Empty(); 
			pParser->m_ptCursorPos.x--;
			pParser->m_ptCursorPos.y =(pParser->m_SortedExprElems.ptr()[pParser->m_ptCursorPos.x])->set_cursor_offset(-1);

			m_bEnable=false;
		}
		
	};

	class	XFunc : public virtual XExprEntry
	{
		int m_nCurrArgsCount;
	public:
		XFunc( ) { m_nCurrArgsCount = 0; m_strType = "Func";}

        // функция возвращает требуемое ей количество аргументов,
		// если возвращает (-1) - функция может иметь различное количество аргументов, которое определяется
		// текущим значением переменной m_nСurrArgsCount
		virtual int		get_actual_args_count( ) { return 0; /*нет аргументов*/ }

		int		get_curr_args_count( ) { return m_nCurrArgsCount; }
		void	set_curr_args_count( int ncount ) { m_nCurrArgsCount = ncount; }
		EntryType GetType(){return ET_FUNCTION;}
	};

	// класс подвыражения
	class	XSubExpr
	{
	public:
		_ptr_t<XExprEntry*> m_entries;
       
	public:
		XSubExpr( );
		~XSubExpr( );
    };

	// класс списка подвыражений
	class	XSubExprList : public XExprEntry
	{
	protected:
		int m_ncurr_expr_idx;			// индекс текущего подвыражения

	public:
        _ptr_t<XSubExpr*> m_sub_exprs;	// массив указателей на подвыражения
		_ptr_t<RECT> m_separator_rects;	// массив прямоугольников разделителей подвыражений

	public:
		XSubExprList ( );
		~XSubExprList( );
		void copy(XExprEntry** entries, int& from);
		virtual	double	calc( );
		virtual void	calc_rect( RECT *prect );
		virtual	BOOL	draw( );
		virtual bool	is_subexpr(void){return true;};			
		long get_expr_count( ) { return m_sub_exprs.size( ); }
		int get_expr_count_ex( ) ;
		EntryType GetType();
	public:
		CString m_strSeparator;
	};

	class XSingleArgFunc : public XFunc
	{
	public:
		int		get_actual_args_count( ) { return 1; /*один аргумент*/ }
	};

	class XMultiArgFunc : public XFunc
	{
	public:
		XMultiArgFunc( ) {};

		virtual int		get_actual_args_count( ) { return -1; /*различное количество аргументов*/ }
		
		double	calc_arg( int narg );
		void	calc_arg_rect( int narg, RECT *prect );
		BOOL	draw_arg( int narg );

		void	calc_rect_for_multiarg_func( char *sfunc, RECT *prtOp, RECT *prect );
		BOOL	draw_multiarg_func( char *sfunc, RECT *prtOp );
	};

	class	XConst : public XExprEntry
	{
	public:

		XConst( ) {m_strType = "Const"; }

		virtual CString get_graphics(){return str;};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
		virtual RECT move_cursor(MovingDirection dir);
		void	get_string( char **pstr  );
		virtual void get_divided_strings(CString& s1, CString& s2);
		virtual EntryType GetType();
		union
		{
		double	m_dConst;
		int		m_nConst;
		}m_Const;
		bool m_bDouble;
		CString str;
		//int get_cursor_pos(void);
		virtual int set_cursor_offset(int offset);
		virtual void get_rect(RECT*);
		virtual void	delete_entry();
	};

	class	XNamedConst : public XExprEntry
	{
	public:
 		XNamedConst( ) { m_nCode = NCONST_UNKNOWN; m_strType = ""; }

		virtual CString get_graphics();
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );

		double	get_value( );

		int	m_nCode;
	};
	
	class	XVar : public XExprEntry
	{
		ExprParam	*get_ExprParam( );
		ParamKey	get_ParamKey( );
		RECT m_MainRect, m_UpRect, m_LoRect;
	public:
		XVar( );
		virtual CString get_graphics();
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );

		TPOS	m_lPos;
	};

	class	XUnkSymbol : public XExprEntry
	{
	public:
		XUnkSymbol( ) {m_strType = "UnkSymbol"; }
		virtual CString get_graphics()
		{
			return m_bEnable ? (pParser->m_UnkSymbols.get( m_lPos )):"";
		};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );

		TPOS	m_lPos;
	};

	class	XEmptyOperation : public XExprEntry
	{
	public:
		XEmptyOperation() { m_strType = "";}
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XMissingOperand : public XExprEntry // f_no_operand
	{
	public:
		XMissingOperand() {m_strType = "Missing oper";};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XUnkFunction : public XMultiArgFunc
	{
	public:
		XUnkFunction( ) {m_strType = "f?"; };
		virtual CString get_graphics(){	 return m_bEnable ? pParser->m_UnkFuncs.get( m_lPos ):"";};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );

		TPOS	m_lPos;
	};

	class	XBrackets : virtual public XExprEntry
	{
	protected:
		RECT m_rtLBracket, m_rtRBracket;

	public:
		XBrackets( ) 
		{ 
			::SetRectEmpty( &m_rtLBracket );
			m_rtRBracket = m_rtLBracket;

		}
		RECT get_left_rect(){return m_rtLBracket;};
		RECT get_right_rect(){return m_rtRBracket;};
		virtual EntryType GetType(){return ET_BRACKET;};
		virtual void*	get_this(){return this;};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );

		BOOL	is_left_bracket( );
		BOOL	is_right_bracket( );

        void	calc_rect_for_left_bracket( RECT *prect );        
		void	calc_rect_for_right_bracket( RECT *prect, long lbottom  );        

		BOOL	draw_left_bracket( );
		BOOL	draw_right_bracket( );

		int m_iLBracket, m_iRBracket;
	};

	class XDummyBracket: public XExprEntry
	{
	public:
		XBrackets* ref;
		MovingDirection md;
		virtual void get_rect(RECT* r)
		{
			if(md==mdRight)
			{
				*r = ref->get_right_rect();
			}
			else
			{
				*r = ref->get_left_rect();
			}
		};
		EntryType GetType()
		{
			if(md==mdRight)
				return ET_RBRACKET;
			return ET_LBRACKET;
		};
		void delete_entry();
	};
#pragma warning( disable : 4250)
	// класс скобок функций
	class	XFuncBrackets : public XBrackets, public XMultiArgFunc
	{	
		int		m_nidx_arg;
		long	m_lbottom;
	public:
		XFuncBrackets( );

		virtual	double	calc( );
		virtual void	calc_rect( RECT *prect );
		virtual	BOOL	draw( );
		EntryType GetType(){return ET_BRACKET;};
	};
#pragma warning( default : 4250)

	class	XEqual : public XExprEntry
	{
	public:
		XEqual( ) {m_strType = "=";}
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
		virtual RECT move_cursor(MovingDirection dir);
		virtual CString get_graphics();

	};

	class	XAdd : public XExprEntry
	{
	public:
		XAdd( ) {m_strType = "+";}
		virtual CString get_graphics(){return m_strType;};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XSub : public XExprEntry
	{
	public:
		XSub( ) {m_strType = "-";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XMul : public XExprEntry
	{
	public:
		XMul( ) {m_strType = "*";}

		virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XDiv : public XExprEntry
	{
	public:
		XDiv( ) {m_strType = "/";}
		
		CString get_graphics(){return m_strType;};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XMod : public XExprEntry
	{
	public:
		XMod( ) {m_strType = "%";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XPow : public XExprEntry
	{
	public:
		XPow( ) {m_strType = "^";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XUMinus : public XExprEntry
	{
	public:
		XUMinus( ) {m_strType = "-"; }

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XUPlus : public XExprEntry
	{
	public:
		XUPlus( ) {m_strType = "+";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XSin : public XSingleArgFunc
	{
	public:
		XSin( ) {m_strType = "Sin";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XCos : public XSingleArgFunc
	{
	public:
		XCos( ){m_strType = "Cos";} 

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );

	};

	class	XAsin : public XSingleArgFunc
	{
	public:
		XAsin( ) {m_strType = "Asin";}
		virtual CString get_graphics(){return m_strType;};
		virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XAcos : public XSingleArgFunc
	{
	public:
		XAcos( ) 
		{
			m_strType = "Acos";
		}
		virtual CString get_graphics(){return m_strType;};
        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XExp : public XSingleArgFunc
	{
	public:
		XExp( ) {m_strType = "Exp";}

		virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XLog10 : public XSingleArgFunc
	{
	public:
		XLog10( ) {m_strType = "Lg";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XLog : public XSingleArgFunc
	{
	public:
		XLog( ) {m_strType = "Ln";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XTan : public XSingleArgFunc
	{
	public:
		XTan( ) {m_strType = "Tan";}

		virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XAtan : public XSingleArgFunc
	{
	public:
		XAtan( ) {m_strType = "Atan";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};

	class	XSqrt : public XSingleArgFunc
	{
	public:
		XSqrt( ) {m_strType = "Sqrt";}

        virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};


	class	XArrayFunc : public XSingleArgFunc
	{
	protected:
		double m_fResult;
	public:
		XArrayFunc( ) {	m_fResult = 0.;m_strType = "ArrayFunc"; }

		double	calc( )
		{
			if( !pParser )
				throw -2;

			// store
			long	lOldArrayValueIndex = pParser->m_lArrayValueIndex,
					lOldArrayLength = pParser->m_lArrayLength;
			
			pParser->m_lArrayLength = INIT_ARRAY_LENGTH;
			long lcurrent_position = (long) pParser->m_pEntry;

			on_begin_calc( );
			for( pParser->m_lArrayValueIndex = 0; pParser->m_lArrayValueIndex < pParser->m_lArrayLength; pParser->m_lArrayValueIndex ++ )
			{
				pParser->m_pEntry = (/*CParser::*/XExprEntry **) lcurrent_position;
				on_calc_element( __super::calc() );
			}
			on_end_calc( );

			// restore
			pParser->m_lArrayValueIndex = lOldArrayValueIndex;
			pParser->m_lArrayLength = lOldArrayLength;

			return m_fResult;
		}		  

		BOOL	draw( ) { return on_draw( ); }
		void	calc_rect( RECT *prect ) { return on_calc_rect( prect ); }

		// calc
		virtual	void on_calc_element( double fResult ) { return; }
		virtual	void on_begin_calc( ) { m_fResult = 0.; return; }
		virtual	void on_end_calc( ) { return; }

		// draw
		virtual	BOOL on_draw( ) { return TRUE; }

		// calc_rect
		virtual void	on_calc_rect( RECT *prect ) { return; }

	};

	class	XSum : public XArrayFunc
	{
	public:

		// calc
		void on_calc_element( double fResult );
		// draw
        BOOL on_draw( );
        // calc_rect
		void	on_calc_rect( RECT *prect );

	};

	class	XAvg : public XArrayFunc
	{
		long m_lCount;
	public:
		XAvg( ) 
		{  m_lCount = 0;m_strType = "Avg";	}

		virtual CString get_graphics(){return m_strType;};
        // calc
		void on_calc_element( double fResult );
		void on_begin_calc( );
		void on_end_calc( );
		
		// draw
        BOOL on_draw( );

		// calc_rect
		void	on_calc_rect( RECT *prect );
	};


	class XMin : public XMultiArgFunc
	{
	public:
		virtual	double	calc( );
		virtual void	calc_rect( RECT *prect );
		virtual	BOOL	draw( );
		
	};

	class XMax : public XMultiArgFunc
	{
	public:
		virtual	double	calc( );
		virtual	BOOL	draw( );
		virtual void	calc_rect( RECT *prect );
	};
    

	_list_map_t< ExprParam*, ParamKey, cmp_paramkeys, ExprParam_ex_free> m_Params;	// ассоциативный массив параметров
	_list_t<CString> m_UnkSymbols;													// массив неизвестных символов
	_list_t<CString> m_UnkFuncs;													// массив неизвестных функций

	CString m_sExpression;
	CString m_sToken;
	int		m_iMode;					// режим работы (XModes)
	int		m_iTokeType;
	int		m_iCurrIndex;				
	int		m_iErrorCode;				// код ошибки
	long	m_lCountUnNecessaryParams;	// количество параметров, для которых не надо задавать значения (стоят слева от знака равенства)
	POINT		m_ptCursorPos;
	std::vector<RECT>	m_ExprRects;
	std::vector<CString> m_ExprStrings;
	std::stack<XExprEntry*> m_tempEntries;
	BOOL m_bNewData;

	_ptr_t<XExprEntry*>	m_ExprElems;	// массив элементов выражения
	XExprEntry **m_pEntry;				// указатель на элемент выражения
	bool m_bReset;
	bool m_bCursor;
	_ptr_t<XExprEntry*>	m_SortedExprElems;
	//cursor support
	//m_ptPosition.x here is a number of entries to the left from cursor
	//m_ptPosition.y - number of symbols of the m_ptPosition.x-th entry to the left from cursor - 
	//must be ignored by all entries except not named const
	//POINT m_ptPosition;
	RECT  m_rectCursor;
	XExprEntry *m_pCursorOwner;
	XExprEntry *m_pLastEntry;

	// for draw
    HDC		m_hdc;
	RECT	m_DrawRect;
	// 

	// array params
	bool	m_bWaitArrayParam;	// флаг ожидания параметра множества
	long	m_lArrayValueIndex;		// текущий индекс значения параметра в массиве значений
	long	m_lArrayLength;	
	BOOL	m_bErrorEvalArrayFunc;				// 
	//

	// sub expressions
	bool	m_bSubExprProcessing;	// флаг обработки подвыражений


	void	free( );

	void	merge_elems( _ptr_t<XExprEntry*> *preceiver, _ptr_t<XExprEntry*> *psource ); 
	void	add_to_begin( _ptr_t<XExprEntry*> *preceiver, XExprEntry *pelem );
	
	int		eval_expr( double &, RECT *prect = 0 );								// подсчет выражения
	void	eval_expr1( _ptr_t<XExprEntry*> *, BOOL bBreakOnRBracket = FALSE,
											BOOL bUseSubExpression = FALSE );	// пустая операция
	void	eval_expr1_5( _ptr_t<XExprEntry*> * );								// операция =
	void	eval_expr2( _ptr_t<XExprEntry*> * );									// сложение и вычитание
	void	eval_expr3( _ptr_t<XExprEntry*> * );									// умножение и деление
	void	eval_expr4( _ptr_t<XExprEntry*> * );									// возведение в степень
	void	eval_expr5( _ptr_t<XExprEntry*> * );									// унарный + или -
	void	eval_expr6( _ptr_t<XExprEntry*> *pelems, int *pnExpressionCount = 0,
						BOOL bGetdArgs = FALSE );									// обработка функций sin, cos и т.п.
	void	eval_expr7( _ptr_t<XExprEntry*> *pelems, int *pnExpressionCount = 0,
						BOOL bGetdArgs = FALSE );									// обработка выражения в скобках
	void	eval_expr7_5( _ptr_t<XExprEntry*> *pelems, int *pnArgsCount );


	void	get_token( );
	void	atom( _ptr_t<XExprEntry*> * );

	
	BOOL	is_delim(void);
	BOOL	is_letter(void);
	BOOL	is_digit(void);
	BOOL	is_point(void);
	BOOL	is_variable_begin(void);
	BOOL	is_variable_end(void);
	BOOL	is_named_const(void);
	BOOL	is_subexpr_delim(void);

	int		getCodeNamedConst( CString sToken );
	double	getValueNamedConst( int iCode );

	int		get_prev_non_empty_expr(int n);
	int		get_next_non_empty_expr(int n);
	void	init_cursor();

	BOOL calc_rect( LPCSTR pstr, RECT *prect, BOOL OffsetByBottom = TRUE );
	BOOL draw_text( LPCSTR pstr, RECT *prect, UINT uFormat = 0 );

	long	get_necessary_params_count( int iParamType );
	void	measure_substring(char* str,int subCount ,RECT& rect);
	void	clear_dummi_stack();
	void create_dummy_bracket_entry(XBrackets* e, XExprEntry**, XExprEntry**);

public:

	CParser(void);
	virtual ~CParser(void);
	void ActivateCursor();
	CString sPaint;

	void	SetString( CString str );		
	CString	GetString( );
	int		ParseExpr( );													// разбор выражения и возврат кода ошибки
	int		Evaluate( double & );											// подсчет выражения и возврат кода ошибки

	long	GetNecessaryParamsNames( BSTR *pbstrParamsNames, int iParamType, int iMode );	// возврат требуемых параметров для заданного режима работы
	long	GetNecessaryParamsNamesEx( DrawParamInfo *pParamsInfos, int iParamType, int iMode );	
	BOOL	SetParamsValues( double *pfValues, long	lSize );

	BOOL	SetParamsVArrayalues( ArrayParamValues *pstValues, long	lSize );	

	BOOL	SetParamsExInfo( ParamExInfo *pExInfo, long	lSize );
	

	//int		CalcNecessaryRect( HDC hdc, RECT *prect );			// 
	int		Draw( HDC hdc, RECT *pRect, TextDrawOptions tFlags );
	int		Redraw( HDC hdc );
	void	GetExprRect( RECT *prect );
	//void	GetExprRectangles( TCHAR** &str, RECT **prect, long* count );
	RECT MoveCursor(HDC dc, MovingDirection dir);
	RECT GetCursorCoordinates(HDC dc);
	RECT SetCoordinates(RECT rectDesired);
	void GetDividedStrings(CString& strBegin, CString& strEnd);
	void DeleteEntry(CString& s);
	POINT GetCursorPosition(void);
	
private:
	void sort_entries(void);
protected:
	// кол-во элементарных выражений, включая развернутые подвыражения
	int get_expression_count(void);
private:
	void copy_next(XExprEntry** entries, int& from, int curr);
};

