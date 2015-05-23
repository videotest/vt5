#ifndef __sewLI_action_h__
#define __sewLI_action_h__

#include "sewLI.h"
#include "\vt5\ifaces\sewLI_int.h"
#include "resource.h"
#include "action_filter.h"
#include "action_interactive.h"
#include "image5.h"
#include "iSewLI.h"
#include "action_misc.h"
#include "atltypes.h"


/////////////////////////////////////////////////////////////////////////////////////////
//manual sew

/*class CDIBImage
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
};*/

class CSewLI : public CInteractiveAction, 
					public CUndoneableActionImpl,
					public IInteractiveSewLIAction,
					public _dyncreate_t<CSewLI>
{
	route_unknown();
public:
	CSewLI();
	virtual ~CSewLI();


	//CDIBImage	*GetWorkImage();
	//void		MoveTo( _point point );
	//void		SwitchZoom( _point point, bool bForce );

	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual bool Initialize();
	virtual void DeInit();
	virtual bool Finalize();
	com_call TerminateInteractive();

protected:
	//virtual bool DoChar( int nChar, bool bKeyDown );
	//virtual bool DoLButtonDown( _point point );
	//virtual bool DoLButtonDblClick( _point point );
	//virtual bool DoStartTracking( _point point );
	//virtual bool DoFinishTracking( _point point );
	//virtual bool DoTrack( _point  point );
	//virtual bool DoRButtonUp( _point point );
	//virtual void EnsureVisible( _point point );
	//virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	//virtual void DoActivateObjects( IUnknown *punkObject )	{TerminateInteractive();}

	com_call DoInvoke();
	//com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	//com_call SetMode( UINT nChar );
	//com_call GetMode( UINT *pnChar );
	//com_call NextImage();
	com_call GetActionState( DWORD *pdwState );
public:
	/*_list_ptr_t<CDIBImage*, FreeEmpty>	
					m_images;
	IImage2Ptr		m_result;
	long			m_lCurrentPos;
	_rect			m_rectActiveImage;
	_point			m_pointOffset;
	_size			m_sizeScroll, m_sizeScrollOld;
	int				m_xOfs1, m_xOfs2, m_yOfs1, m_yOfs2;
	_point			m_pointOldMove;

	bool			m_bTrackFirstTime;
	double			m_fOriginalZoom;
	bool			m_bFullZoom;

	_bstr_t			m_bstrCCName;
	int				m_colors;*/
	IOptionsPagePtr	m_ptrPage;
};

/////////////////////////////////////////////////////////////////////////////////////////
//info 

class CSewLIInfo : public _ainfo_t<ID_ACTION_SEWLI, _dyncreate_t<CSewLI>::CreateObject, 0>,
							public _dyncreate_t<CSewLIInfo>
{
	route_unknown();
public:
	//arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	//static arg	s_pargs[];
};


class CSewLIObjectAction :	public CFilter,
			   				public _dyncreate_t<CSewLIObjectAction>
{
public:
	route_unknown();
public:
	virtual IUnknown*	DoGetInterface( const IID &iid );

	CSewLIObjectAction();
	virtual ~CSewLIObjectAction();
	
	/*virtual bool*/com_call DoInvoke();//InvokeFilter();
};


class CSewLIObjectActionInfo : public _ainfo_t<ID_ACTION_SEWLI_OBJECT, _dyncreate_t<CSewLIObjectAction>::CreateObject, 0>,
					   public _dyncreate_t<CSewLIObjectActionInfo>
{
	route_unknown();
public:
	CSewLIObjectActionInfo()
	{
		return;
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CShowSewLI : public CActionShowView,
				public _dyncreate_t<CShowSewLI>
{
public:
	route_unknown();	
public:
	virtual _bstr_t GetViewProgID();
};


class CShowSewLIInfo : public _ainfo_t<IDS_SHOW_SEW_LI, _dyncreate_t<CShowSewLI>::CreateObject, 0>,
					public _dyncreate_t<CShowSewLIInfo>
{
	route_unknown();
public:
	CShowSewLIInfo(){}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetFrame;}	
};



class CActionSewImport : public CAction, public CUndoneableActionImpl,
	public _dyncreate_t<CActionSewImport>
{
	IUnknownPtr m_punkImage;
	POINT m_ptPos;
	ISewImageListPtr m_sptrSLI;
public:
	route_unknown();
	CActionSewImport(void);
	~CActionSewImport(void);
	virtual IUnknown *DoGetInterface( const IID &iid );
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
};

class CActionSewImportInfo : 
public _ainfo_t<ID_ACTION_SEWIMPORT, _dyncreate_t<CActionSewImport>::CreateObject, 0>,
public _dyncreate_t<CActionSewImportInfo>
{
	route_unknown();
public:
	CActionSewImportInfo() { }
	arg *args() {return s_pargs;}
	static arg s_pargs[];
	virtual _bstr_t target()			{return "anydoc";}
};


class CActionSewExport : public CAction, public CUndoneableActionImpl,
	public _dyncreate_t<CActionSewExport>
{
	void SendToDoc(ISewFragment *pFrag, BSTR bstrImg);
public:
	route_unknown();
	CActionSewExport(void);
	~CActionSewExport(void);
	virtual IUnknown *DoGetInterface( const IID &iid );
	com_call DoInvoke();
};

class CActionSewExportInfo : 
	public _ainfo_t<ID_ACTION_SEWEXPORT, _dyncreate_t<CActionSewExport>::CreateObject, 0>,
	public _dyncreate_t<CActionSewExportInfo>
{
	route_unknown();

public:
	CActionSewExportInfo() { }
	arg *args() {return s_pargs;}
	static arg s_pargs[];
	virtual _bstr_t target()			{return "anydoc";}
};


class CActionSewMoveFragment : public CInteractiveAction, public CUndoneableActionImpl,
	public _dyncreate_t<CActionSewMoveFragment>
{
	POINT m_ptFragOrig;
	void MoveFragment(POINT point, bool bFire);
	ISewImageListPtr m_sptrSLI;
	ISewFragmentPtr m_sptrFrag;
	ISewLIViewPtr m_sptrView;
	POINT m_ptFragPos1;
	POINT m_ptCursPos1;
//	double m_fZoom;
	bool m_bEnableMove;
	bool m_bMoved;
	bool m_bMoveLV;
	CRect m_rcMoveFrag;
	bool GetMoveableFragmentRect(RECT &rcMoveFrag);
	bool m_bKeyPress;
	void ProcessKeyboardMove(WPARAM wParam, bool bFinally);
public:
	route_unknown();
	CActionSewMoveFragment(void);
	~CActionSewMoveFragment(void);
	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual bool Initialize();
	virtual bool Finalize();
	com_call DoInvoke();
	virtual bool DoLButtonDown( _point point );
	virtual bool DoStartTracking( _point point );
	virtual bool DoFinishTracking( _point point );
	virtual bool DoTrack( _point  point );
	virtual bool DoLButtonUp( _point point );
	virtual bool DoRButtonUp( _point point );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual bool DoSetCursor();
	com_call DoUndo();
	com_call DoRedo();
};

class CActionSewMoveFragmentInfo : 
	public _ainfo_t<ID_ACTION_SEWMOVEFRAGMENT, _dyncreate_t<CActionSewMoveFragment>::CreateObject, 0>,
	public _dyncreate_t<CActionSewMoveFragmentInfo>
{
	route_unknown();

public:
	CActionSewMoveFragmentInfo() { }
	arg *args() {return s_pargs;}
	static arg s_pargs[];
	virtual _bstr_t target()			{return "view site";}
};


class CActionSewDeleteFragment : public CAction, public CUndoneableActionImpl,
	public _dyncreate_t<CActionSewDeleteFragment>
{
	IUnknownPtr m_punkImage;
	POINT m_ptPos;
	ISewImageListPtr m_sptrSLI;
public:
	route_unknown();
	CActionSewDeleteFragment(void);
	~CActionSewDeleteFragment(void);
	virtual IUnknown *DoGetInterface( const IID &iid );
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
};

class CActionSewDeleteFragmentInfo : 
	public _ainfo_t<ID_ACTION_SEWDELETEFRAGMENT, _dyncreate_t<CActionSewDeleteFragment>::CreateObject, 0>,
	public _dyncreate_t<CActionSewDeleteFragmentInfo>
{
	route_unknown();

public:
	CActionSewDeleteFragmentInfo() { }
	arg *args() {return s_pargs;}
	static arg s_pargs[];
	virtual _bstr_t target()			{return "anydoc";}
};



class CActionSewSaveImage : public CFilter,
	public _dyncreate_t<CActionSewSaveImage>
{
public:
	route_unknown();
	CActionSewSaveImage(void);
	~CActionSewSaveImage(void);
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ) {return false;}
};

class CActionSewSaveImageInfo : 
	public _ainfo_t<ID_ACTION_SAVE_SEW_IMAGE, _dyncreate_t<CActionSewSaveImage>::CreateObject, 0>,
	public _dyncreate_t<CActionSewSaveImageInfo>
{
	route_unknown();

public:
	CActionSewSaveImageInfo() { }
	arg *args() {return s_pargs;}
	static arg s_pargs[];
	virtual _bstr_t target()			{return "anydoc";}
};


#endif //__sewLI_actions_h__
