#ifndef __cario_impl_h__
#define __cario_impl_h__


#include "carioint.h"
#include "idio_int.h"
#include "chromosome.h"
#include "drag_drop_defs.h"
#include "\vt5\awin\misc_map.h"
#include "\vt5\awin\misc_list.h"
#include "charts_int.h"

#define _MASTER_KEY_ VK_SHIFT

enum AlignChromo
{
	acTop			= 0,
	acCenterLine	= 1,
	acBottom		= 2,
};

struct FRECT
{
	double    left;
	double    top;
	double    right;
	double    bottom;

	FRECT()
	{
		::ZeroMemory( this, sizeof( FRECT ) );
	}

	FRECT( const FRECT &rc )
	{
		::ZeroMemory( this, sizeof( FRECT ) );

		*this = rc;
	}

	FRECT( double _left, double _top, double _right, double _bottom )
	{
		left   = _left;
		top    = _top;
		right  = _right;
		bottom = _bottom;
	}

	void OffsetRect( POINT pt )
	{
		left += pt.x;
		top += pt.y;
		right += pt.x;
		bottom += pt.y;
	}
	double Width() { return right - left; }
	double Height() { return bottom - top; }

	void operator=( const FRECT &rc )
	{
		left   = rc.left;
		top    = rc.top;
		right  = rc.right;
		bottom = rc.bottom;
	}
};

struct FPoint : public FPOINT
{
	FPoint()
	{
		::ZeroMemory( this, sizeof( FPoint ) );
	}

	FPoint( double _x, double _y )
	{
		x = _x;
		y = _y;
	}

	FPoint( const FPoint &pt )
	{
		::ZeroMemory( this, sizeof( FPoint ) );

		*this = pt;
	}

	void operator=( const FPoint &pt )
	{
		x   = pt.x;
		y   = pt.y;
	}
	operator POINT() const { POINT pt = { x, y }; return pt; }
	operator CPoint() const { CPoint pt( x, y ); return pt; }
};

#define IDIOGRAM_POS_LEFT	0
#define IDIOGRAM_POS_RIGHT	1

//перекрыть хорошо бы дл€ многово, но самое нужное - восстанавливать после fragdrop
//guid исходных объектов
class CCarioFrame : public CFrame
{
	DECLARE_DYNCREATE(CCarioFrame)
protected:
	virtual bool SerializeObject( CStreamEx& ar, SerializeParams *pparams );	
public:
	CArray<GUID, GUID&>	m_arSrcGuids;
};


struct CInterval
{
	double a, b;
	CInterval()
	{
		a = b = 0;
	}
	CInterval( double s, double e )
	{
		a = s;
		b = e;
	}

	CInterval( const CInterval &in )
	{
		*this = in;
	}

	void operator=( const CInterval &in )
	{
		a = in.a;
		b = in.b;
	}
};

class CCarioImpl;
class chromo_object
{
public:
	CString m_strTextInfo;
	chromo_object();
	virtual ~chromo_object();
	void init_defaults(CCarioImpl *pImpl);
	void deinit( CCarioImpl *pImpl );
	void load( CStreamEx& stream );
	void save( CStreamEx& stream );
	void save_to_named_data( INamedData* pINamedData, CString strSection );
	void load_from_named_data( INamedData* pINamedData, CString strSection, CCarioImpl *pImpl );

	CRect	get_total_rect()	{return CRect( m_ptOffset, m_sizeTotal );}
	
	bool m_bErected;
	int m_nLinesCount;

	IUnknownPtr		m_ptrMeasureObject;
	GUID			m_guidMeasureObject;
	
	struct XAddLinesStruct
	{
		FRECT m_ptsLines;
		FPOINT m_pt;
	} *m_strAddLines, *m_strAddLinesFull;


	IChromosomePtr	m_ptrChromos;
	IIdiogramPtr	m_ptrIdio;

	IIdiogramPtr	m_sptrEditableIdio;

	IChartDrawerPtr m_sptrChart;
	IChartDrawerPtr m_sptrRatio;

	//рассчитывает все, кроме m_ptObjectOffset
	void			layout_object( bool bShowChromoCenterLine, bool m_bShowChromoCentromere, bool bShowMainIdio, AlignChromo ac, int nCurrPane, CCarioImpl *pImpl, int nClassNum );
	//рассчитывает оставшийс€ m_ptObjectOffset. nLine - лини€ прив€зки.
	//если по верху, это верх. если по центромере, это центромера
	void			calculate_offsets( int nLine, AlignChromo ac );
	//void			calculate_dimension( bool bShowChromoCenterLine, bool m_bShowChromoCentromere, AlignChromo ac );
	

	//все позиции вычисл€ютс€ в общей системе координат, точка начала отсчета - левый верхний
	//угол объекта
	//все координаты с учетом увеличени€, но без учета скроллинга
	CRect			m_rectIdio,		//позици€ идиограммы 
					m_rectChromo,	//позици€ ц. линии, центромеры и вс€кой дури
					m_rectImage,	//позици€ изображени€
					m_rectCGHGraph,
					m_rectCGHRatio,
					m_rectEditableIdio,
					m_rectFull,
					m_rectText;

	
	CString			*m_pstrRatioTextUP,
					m_strRatioTextDOWN;

	_list_t<CInterval> m_listCGHAmpl_L; 
	_list_t<CInterval> m_listCGHAmpl_R; 


	//информаци€ наружу - позици€ центромеры и размер под объект
	CSize			m_sizeTotal;
	int				m_nCentromereY;
	double			m_fLength;
	//глобальна€ позици€ объекта
	CPoint			m_ptOffset;
	
	//задаемт размер идиограммы без текста - высота рассчитываетс€ исход€ из размера хромосомы,
	//ширина - фиксированна€
	CSize			m_sizeIdio, m_sizeEditableIdio;

	bool			m_bShowIdiogramm;

	bool m_bHasCGHAmplL, m_bHasCGHAmplR;

	double			m_fZoom;
	long			m_nYOffset;
	void init_cgh_graph( const CCarioImpl *pImpl );
	void init_cgh_ratio( const CCarioImpl *pImpl );
	static double _calc_cgh_value( const IImage3Ptr &ptrImage, const FPoint &pt1, const FPoint &pt2, const int &nPaneID, FPOINT *ret_pt1, FPOINT *ret_pt2 );
protected:
	void _update_rects( int nChromoMinX, int nChromoMaxX, int nChromoMinY, int nChromoMaxY, bool bShowMainIdio, const CCarioImpl *pImpl  );
};

/////////////////////////////////////////////////////////////////////////////
class chromo_cell
{
public:
	chromo_cell();
	virtual ~chromo_cell();
	void init_defaults();
	void deinit();
	void load( CStreamEx& stream );
	void save( CStreamEx& stream );
	void save_to_named_data( INamedData* pINamedData, CString strSection );
	void load_from_named_data( INamedData* pINamedData, CString strSection, CCarioImpl *pImpl );

	int		m_nCellWidth;
	int		m_nCellOffset;
	int		m_nClassNum;
	CString	m_strClassName;

	CArray<chromo_object*, chromo_object*>	m_arChromos;

	CRect	m_rectCell;
};

/////////////////////////////////////////////////////////////////////////////
class chromo_line
{
public:
	chromo_line();
	virtual ~chromo_line();
	void init_defaults();
	void deinit();
	void load( CStreamEx& stream );
	void save( CStreamEx& stream );
	void save_to_named_data( INamedData* pINamedData, CString strSection );
	void load_from_named_data( INamedData* pINamedData, CString strSection, CCarioImpl *pImpl );

	BOOL		m_bCellar;
	CArray<chromo_cell*, chromo_cell*>	m_arCell;
	int			m_nCaptionHeight;	

	int			m_nCenterLine;//cached
	CRect		m_rectLine;

	long		m_nLineHeight;
};

enum MouseMode
{
	mmNone				= 0,
	mmMoveCell			= 1,
	mmMoveChromo		= 2,
//	mmResizeLineUp		= 3,
	mmResizeLineDown	= 4,
//	mmResizeCellLeft	= 5,
	mmResizeCellRight	= 6,
	mmSetChromoOffset   = 7,
};

enum AlignVert
{
	avTop		= 0,
	avCenter	= 1,
	avBottom	= 2,
};

enum AlignHorz
{
	ahLeft		= 0,
	ahCenter	= 1,
	ahRight		= 2,
};

//флаги дл€ synchronize_dimmension
#define	SDF_RECALC_ALL			0
#define SDF_RECALC_OBJECT		1
#define SDF_NORECALC			2
//флаги дл€ update_chromosome
#define CHROMO_OBJECT_ADD		0
#define CHROMO_OBJECT_DELETE	1
#define CHROMO_OBJECT_CHANGED	2
/*
class CCheck
{
	CString m_str;
public:
	CCheck( CString str )
	{
		FILE *fl = fopen( "c:\\filelog.txt", "at" );
		fprintf( fl, "Start %s\n", m_str = str );
		fclose( fl );
	}
	~CCheck()
	{
		FILE *fl = fopen( "c:\\filelog.txt", "at" );
		fprintf( fl, "End %s\n", m_str );
		fclose( fl );
	}
};
*/

/////////////////////////////////////////////////////////////////////////////
class CCarioFrame;
class CCarioImpl : public CImplBase
{
	int m_nShowNums, m_nShowEditableNums;
	COLORREF m_clRatioMin, m_clRatioMax, m_clRatioMid, m_clBright, m_clBrightBorder, m_clBrightBorder2, m_clRatioBorder, m_clRatioGrad, m_clRatioProb, m_clBright2, m_clAddLinesColor, m_clGradient, m_clGradient2;
	int m_nPosID_I,m_nPosID_G, m_nPosID_R, m_nPosID_C, m_nPosID_E;
	double m_fNormalMin,m_fNormalMid,m_fNormalMax, m_fRatioStepVal;
	int m_CGHGRAPH_CX,m_CGHRATIO_CX;
	int m_CGHAMPL_CX;
	UINT m_nRatioTextRCCount;
	double m_fProbability;
	LOGFONT		m_lpRatioText;

	bool m_bShowProb, m_bShowClasses, m_bShowGradient;
	bool m_bShowLineOnRatio;
	double m_fMinRatioVal, m_fMaxRatioVal;
	bool m_bUseAutoScaleRatio;
protected:
	IUnknownPtr		m_drawobjects;
	bool m_bFromPrint;
	bool m_bErectAfterDrop,m_bShowEditableIdio,m_bShowChromosome,
					m_bShowCGHGraph,
					m_bShowCGHRatio,
					m_bShowCGHAmpl, m_bShowAdditionLines;
	BOOL m_bSkipNotify;
public:
	friend class chromo_object;
	CCarioImpl();
	virtual ~CCarioImpl();
	
	virtual IUnknownPtr		GetViewPtr() = 0;
	virtual HWND			GetHwnd() = 0;
	
	CarioViewMode			GetCarioViewMode();

	bool					is_printing();

protected:
	IIdiogramPtr _get_editable_idio( IUnknown* punkMeasure);

	CarioViewMode			m_carioViewMode;
	IUnknownPtr				m_ptrActiveObjectList;
	COLORREF m_clTargetLine;

	BEGIN_INTERFACE_PART(CarioObject, ICarioObject)
		com_call InitFromObjectList( IUnknown* punkObjectList );
	END_INTERFACE_PART(CarioObject)

	CString get_idio_hint( int nLine, int nCell, int nChromo, CPoint pt );
	//chache unknown pointers
	void			set_class_num( );	
	bool			synchronize_with_object_list( IUnknown* punkObjectList );	
	//without addref
	void			fill_cromo_array_from_object_list( IUnknown* punkObjectList, CArray<IUnknown*, IUnknown*>& arCromoObj );

	bool			build( IUnknown* punkObjectList );
	void			synchronize_dimmension( unsigned flags, IUnknown *punkToSync = 0 );

	//conversion
	CPoint			convert_from_window_to_object( CPoint point );	

	CRect			convert_from_wnd_to_object( CRect rc );
	CRect			convert_from_object_to_wnd( CRect rc );
	CPoint			convert_from_wnd_to_object( CPoint rc );
	CPoint			convert_from_object_to_wnd( CPoint rc );

	double			get_zoom( )			{return m_fCachedZoom;}
	CPoint			get_scroll_pos( )	{return m_ptCachedScrollPos;}

	void			update_zoom_scroll();


	
	//occuped area
	virtual CRect	get_chromo_line_rect(	int nLineIdx, 
											chromo_line** ppline, bool bAddCaptionHeight = true );
	virtual CRect	get_chromo_cell_rect(	int nLineIdx, int nCellIdx, 
											chromo_line** ppline, chromo_cell** ppcell, bool bAddCaptionHeight = true );
	virtual CRect	get_chromo_object_rect( int nLineIdx, int nCellIdx, int nObjectIdx,
											chromo_line** ppline, chromo_cell** ppcell, chromo_object** ppobject );

	virtual	CCarioFrame	*GetDragDropFrame()			{return 0;}

	//helpers	
	// [vanek] - 13.11.2003 : punkExcludedObject - объект,который не учитываетс€ при подсчете результирующего размера
	CSize			get_occuped_size( IUnknown *punkExcludedObject = 0 );
	

	void			get_pointers( int nLineIdx, int nCellIdx, int nChromoIdx,
									chromo_line** ppline, chromo_cell** ppcell, chromo_object** ppobject );

	void			lookup_class( long lclass, int* pnLineIdx, int* pnCellIdx, 
									chromo_line** ppline, chromo_cell** ppcell );

	void			set_chromo_class( chromo_object *pchromo, long lclass );
	//Add remove objects

	
	IChromosomePtr	get_chromo_object_from_measure( IUnknown* punkMeasure );
	IIdiogramPtr	get_idio_object_from_measure( IUnknown* punkMeasure, bool bCellar = false );
	

	//object add
	bool			on_objects_drop( drag_n_drop_data* pdata );	
	bool			create_chromo_array_from_dd( drag_n_drop_data* pdata, CArray<GUID, GUID>& arGuidChromo );

	//эта функци€ должна вызыватьс€ всегда, когда что-то мен€етс€ дл€ хромосомы
	//передаваемый параметр - "большой" объект
	bool			update_chromosome( IUnknown* punk, long lCode );
	//ищем внутренний объект дл€ MeasureObject
	//передаваемый параметр - "большой" объект
	bool			lookup_chromo_in_array( IUnknown* punkChromo, int* pnline, int* pncell, int* pnobj );
	//активизируем €чейку по позиции
	bool			on_activate_object( IUnknown* punk );
	//проверить на валидность и вернуть объект по координатам
	chromo_object	*get_chromo_object( long line, long cell, long lpos );
	//если есть measure_object и chromo_jbject, инициализировать все пол€ последнего
	bool			init_chromo_by_measure( chromo_object *pchromo, IUnknown *punkMeasure );
	//просканить все кроме подвала и обновить классы
	void			update_classes();
	//переместить все объекты в соответствии с номерами их классов (за исключением подвала)
	bool			arrage_by_classes();
	//загрузить все идиограммы в карту идиограмм	
	void			load_idiograms();
	
	//drawing
	int				_get_text_caption_height();

	void			draw_chromo_line( CDC* pdc, int nLineIdx, CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits );
	void			draw_chromo_cell( CDC* pdc, int nLineIdx, int nCellIdx, CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits );
	void			draw_chromo_object( CDC* pdc, int nLineIdx, int nCellIdx, int nObjectIdx, CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits );
	
	void			draw_chromosome_image( CDC* pdc, chromo_object* pobj, CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits );

	void			invalidate_rect( CRect* prect );	

	MouseMode		m_mouseMode;	
	MouseMode		get_mouse_mode_by_point( CPoint ptObj, int *pnLine, int *pnCell, int *pnPos );

	//location
	void			hit_test_area( CPoint ptObj, int* pnLineIdx, int* pnCellIdx, int* pnChromoIdx, bool bTakeLineHeight = false );
	
	//serialization
	bool			init_attached_object_named_data( IUnknown* punk );	
	bool			is_correct_object_list( IUnknown* punk );

	CString			get_lines_section();
	CString			get_view_shared_section();
	CString			get_view_settings_section();

	bool			load_cario_state_from_data( IUnknown* punkNamedData );
	bool			save_cario_state_to_data( IUnknown* punkNamedData );
	bool			reload_object_from_data( IUnknown* punkNamedData );

	bool			m_bHasBeenChange;
	bool			m_bCanSaveToObjectAndShellData;	
	bool			save_to_object_and_shell_data();
	
	bool			m_bCanFireEventDataChange;
	bool			fire_event_data_change();

	CString			get_drawing_path();

	chromo_object*	find_chromo_by_guid( GUID guidChromo, int* pnLineIdx = 0, int* pnCellIdx = 0, int* pnChromoIdx = 0 );
	//action
	chromo_line*	add_line_at_pos( int nLine );	
	chromo_cell*	add_cell_at_pos( int nLine, int nCell );

	
	bool			delete_line( int nLine );	
	bool			delete_cell( int nLine, int nCell );
	
	//active line & cell
	int				m_nActiveLine;
	int				m_nActiveCell;
	int				m_nActiveChromo;
	
	//settings

	COLORREF		m_clrBkg;

	COLORREF		m_clrLine;
	COLORREF		m_clrLineActive;

	COLORREF		m_clrCell;
	COLORREF		m_clrCellActive;

	COLORREF		m_clrLineBorder;
	COLORREF		m_clrCellBorder;

	COLORREF		m_clrObjMirror;

	COLORREF		m_clrAlignLine;
	COLORREF		m_clrs_cell[6];
	int				m_nMinLineWidth;	
	int				m_nMinLineHeight;	
	int				m_nMinCellWidth;
	CSize			m_sizeDefaultLine;

	int				m_nResizeSense;

	AlignChromo		m_chromoAlign;

	bool			m_bShowClassNumber;
	bool			m_bShowChromoContures;
	bool			m_bShowChromoCenterLine;
	bool			m_bShowChromoCentromere;
	bool			m_bNeedChromoAlignLine;
	
	bool			reload_view_settings( IUnknown* punkNamedData );	
	bool			save_view_settings( IUnknown* punkNamedData );	


	//cursors
	HCURSOR			get_cursor_by_mode( MouseMode mm );
	HCURSOR			m_hCursorArrow;
	HCURSOR			m_hCursorVertSize;
	HCURSOR			m_hCursorHorzSize;
	HCURSOR			m_hCursorMoveCell;
	HCURSOR			m_hCursorMoveChromo;


	//mouse coordinates
	CPoint			m_ptMousePrevPos;
	CPoint			m_ptOffset;
	CPoint			m_ptFirstClickPos;
	
	//init deinit
	chromo_line*	_get_cellar( );
	int				_get_cellar_idx();
	void			_kill_cario_array();
	
	CArray<chromo_line*, chromo_line*> m_arCarioLine;		
	
	CSize	m_sizeClient;

	double	m_fCachedZoom;
	CPoint	m_ptCachedScrollPos;
	bool	m_bLockObjectUpdate;

	LOGFONT		m_lfClassText;
	COLORREF	m_clrClassText;
	bool		m_bShowCellIdio;
	bool		m_bActionExecuted;
	GUID m_guidExclude;

	_map_t<IIdiogramPtr, long, cmp_long>	m_idiograms;
};

inline void _find_end_points( IImage3Ptr ptrImage, FPoint &pt1, FPoint &pt2, int nPaneID );


#endif//__cario_impl_h__