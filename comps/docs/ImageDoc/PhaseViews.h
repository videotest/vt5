#pragma once
#include "BinaryViews.h"

class CPhaseViewBase : public CBinaryViewBase
{
protected:
	void InitPhaseColors();
	CArray<COLORREF,COLORREF&> m_arrColors;
	COLORREF m_clrSync;
	int m_nTransparency;
public:
	CPhaseViewBase(void);
	~CPhaseViewBase(void);
	virtual void DrawLayers(CDC* pdc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint, bool bUseScrollZoom);
	virtual void	_AttachObjects( IUnknown *punkContext );
};

class CPhaseView : public CPhaseViewBase
{
	DECLARE_DYNCREATE(CPhaseView);
	GUARD_DECLARE_OLECREATE(CPhaseView);
	PROVIDE_GUID_INFO( )
public:
	CPhaseView();
public:
	virtual const char* GetViewMenuName();
};


