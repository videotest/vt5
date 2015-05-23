#pragma once
#include "gdiplusmanager.h"

// vanek : drawing disabled icons - 27.02.2005
#include "\vt5\comps\misc\glib\glib.h"

enum	FigureType
{
	eftRectangle = 0,
	eftCircle = 1,
};

class CFigureButton : 
	public CButton
{
	static CGDIPlusManager *m_pGDIPlusManager;

	HBITMAP	m_hbitmap_pushed;
	HICON	m_hicon_pushed;

	// support three state button
	BOOL	m_bthree_state_btn;
    BOOL	m_bpushed;

public:
	// Construction
	CFigureButton(void);
	CFigureButton(FigureType ft);
    
	virtual ~CFigureButton(void);

   	// operations
	void	SetThreeState( BOOL buse_three_state = TRUE );
	BOOL	GetThreeState( );
	BOOL	SetPressedState( BOOL bnew_state = TRUE );
	BOOL	IsPressed();

	BOOL	SetPushedBitmap( HBITMAP hbitmap );
	BOOL	SetPushedIcon( HICON hicon );

    // tooltip
	BOOL	SetToolTipText( CString str_toolip );   	

	void	EnableButton( BOOL benable = TRUE );
	BOOL	IsEnableButton( );

	void	SetSemiFlat( BOOL bnew );
	BOOL	GetSemiFlat( );

	void	SetFlatButton( BOOL bnew );
	BOOL	GetFlatButton( );

	void	SetDrawBorder( BOOL bnew );
	BOOL	GetDrawBorder( );


// Overrides
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
protected:
	virtual void PreSubclassWindow( );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT OnNcHitTest(CPoint point); 
	afx_msg BOOL	OnClicked();
	afx_msg BOOL	OnDblClicked();
	afx_msg BOOL	OnNeedTooltipText( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg BOOL	OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

	void	init();

	void	init_gdi_plus( );
	void	deinit_gdi_plus( );

	void	update_region( );

	RectF	draw_circle( Graphics *pgraphics, RectF rc, Color color1, Color color2, REAL fwidth );
    
protected:
	CRgn			m_current_region;
	FigureType		m_eftype;
	CToolTipCtrl	m_tooltip;
	BOOL			m_bdisable;
	BOOL			m_bmouse_on_button;
	BOOL			m_bsemiflat;
	BOOL			m_bdraw_border;
	BOOL			m_bflat_button;

public:
	// vanek : drawing disabled icons - 27.02.2005
	static CImageDrawer m_ImgDrawer;
};


