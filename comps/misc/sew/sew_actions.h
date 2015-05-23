#ifndef __sew_actions_h__
#define __sew_actions_h__

#include "sew.h"
#include "resource.h"
#include "action_filter.h"
#include "action_interactive.h"
#include "image5.h"

interface IInteractiveSewAction : public IUnknown
{
	com_call SetMode( UINT nChar ) = 0;
	com_call GetMode( UINT *pnChar ) = 0;
	com_call NextImage() = 0;
};

declare_interface( IInteractiveSewAction, "DE2ADA4A-8B50-4560-A61C-54103A6B1A1B" );


/////////////////////////////////////////////////////////////////////////////////////////
//auto sew filters
class CSewImage
{
	byte	*m_pbytes;
	int		m_x, m_y, m_cx, m_cy;
public:
	CSewImage();
	~CSewImage();
	void	SetImage( IImage3	*pimg, IColorConvertor *pcnv, int x, int y, int x1, int y1 );
public:	
	byte *GetRow( int x, int y )
	{	x-=m_x;	y-=m_y;	return m_pbytes+x+y*m_cx;}

public:
	void	Contrast();
	void	Median();
	void	Kirsch();
};

class CAutoSewFilter : public CFilter
{
public:
	struct ImageHolder
	{
		int	x, y;
		int	cx, cy;
		IImage3Ptr	image;
		CSewImage	left, right, top, bottom;
	};

	CAutoSewFilter();

	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	com_call GetActionState(DWORD *pdwState);

	void _merge_h( ImageHolder *p1, ImageHolder *p2 );
	void _merge_v( ImageHolder *p1, ImageHolder *p2 );
protected:
	int		m_nMaxHOvrX, m_nMaxHOvrY, m_nMinHOvrX, m_nMinHOvrY;
	int		m_nMaxVOvrX, m_nMaxVOvrY, m_nMinVOvrX, m_nMinVOvrY;
	int		m_lStep;
	int		m_nSmoothEdges;

	int		m_cx, m_cy, m_colors;
	bool	m_bHorz;
};

class CAutoSewHFilter : public CAutoSewFilter, public _dyncreate_t<CAutoSewHFilter>
{
	route_unknown();
public:
	CAutoSewHFilter()
	{	m_bHorz = true;}
	com_call GetActionState(DWORD *pdwState) {return CAutoSewFilter::GetActionState(pdwState);}
};

class CAutoSewVFilter : public CAutoSewFilter, public _dyncreate_t<CAutoSewVFilter>
{
	route_unknown();
public:
	CAutoSewVFilter()
	{	m_bHorz = false;}
	com_call GetActionState(DWORD *pdwState) {return CAutoSewFilter::GetActionState(pdwState);}
};

/////////////////////////////////////////////////////////////////////////////////////////
//manual sew

class CDIBImage
{
public:
	enum DrawMode
	{		add = '+', sub = '-', copy = '='	};
public:
	CDIBImage();
	~CDIBImage();

	bool	IsInitialized();
public:
	void	DrawRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, RECT rectDest, _point  ptBmpOfs, double fZoom, _point  ptScroll );
	bool	AttachImage( IUnknown *punkImage );
public:
	void		SetOffset( const _point 	&point )			{m_point = point;}
	_point		GetOffset() const							{return m_point;}
	void		SetVisible( bool bSet )					{m_bVisible = bSet;}
	bool		GetVisible() const							{return m_bVisible;}
	_rect		GetRect()								{return _rect( m_point, m_size );}
	IImage3Ptr	&GetImage()						{return m_image;}
	void		SetDrawMode( DrawMode mode )		{m_mode = mode;}
	DrawMode	GetDrawMode() const					{return m_mode;}
protected:
	IImage3Ptr			m_image;
	_point 				m_point;
	_size				m_size;
	BITMAPINFOHEADER	*m_pbi;
	byte				*m_pbits;
	byte				*m_pmask;
	bool				m_bVisible;
	DrawMode			m_mode;
};

class CSewManual : public CInteractiveAction, 
					public CUndoneableActionImpl,
					public IInteractiveSewAction,
					public _dyncreate_t<CSewManual>
{
	route_unknown();
public:
	CSewManual();
	virtual ~CSewManual();


	CDIBImage	*GetWorkImage();
	void		MoveTo( _point point );
	void		SwitchZoom( _point point, bool bForce );

	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	com_call TerminateInteractive();

protected:
	virtual bool DoChar( int nChar, bool bKeyDown );
	virtual bool DoLButtonDown( _point point );
	virtual bool DoLButtonDblClick( _point point );
	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point  point );
	virtual bool DoRButtonUp( _point point );
	virtual void EnsureVisible( _point point );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void DoActivateObjects( IUnknown *punkObject )	{TerminateInteractive();}

	com_call DoInvoke();
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call SetMode( UINT nChar );
	com_call GetMode( UINT *pnChar );
	com_call NextImage();
	com_call GetActionState( DWORD *pdwState );
public:
	_list_ptr_t<CDIBImage*, FreeEmpty>	
					m_images;
	IImage2Ptr		m_result;
	TPOS			m_lCurrentPos;
	_rect			m_rectActiveImage;
	_point			m_pointOffset;
	_size			m_sizeScroll, m_sizeScrollOld;
	int				m_xOfs1, m_xOfs2, m_yOfs1, m_yOfs2;
	_point			m_pointOldMove;

	bool			m_bTrackFirstTime;
	double			m_fOriginalZoom;
	bool			m_bFullZoom;

	_bstr_t			m_bstrCCName;
	int				m_colors;
	IOptionsPagePtr	m_ptrPage;
};

/////////////////////////////////////////////////////////////////////////////////////////
//info 


class CAutoSewHorzInfo : public _ainfo_t<ID_ACTION_AUTOSEWHORZ, _dyncreate_t<CAutoSewHFilter>::CreateObject, 0>,
							public _dyncreate_t<CAutoSewHorzInfo>
{
	route_unknown();
public:
	CAutoSewHorzInfo();
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];

};


class CAutoSewVertInfo : public _ainfo_t<ID_ACTION_AUTOSEWVERT, _dyncreate_t<CAutoSewVFilter>::CreateObject, 0>,
							public _dyncreate_t<CAutoSewVertInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];

};


class CSewManualInfo : public _ainfo_t<ID_ACTION_SEW, _dyncreate_t<CSewManual>::CreateObject, 0>,
							public _dyncreate_t<CSewManualInfo>
{
	route_unknown();
public:
	//arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	//static arg	s_pargs[];

};

#endif //__sew_actions_h__
