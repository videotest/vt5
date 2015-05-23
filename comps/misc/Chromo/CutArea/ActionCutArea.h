// ActionCutArea.h: interface for the CActionCutArea class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONCUTAREA_H__7F640A4E_2BA1_11D6_BA8D_0002B38ACE2C__INCLUDED_)
#define AFX_ACTIONCUTAREA_H__7F640A4E_2BA1_11D6_BA8D_0002B38ACE2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "vector.h"
#include "matrix.h"

CVector _inverse_transform(CVector vPtInWorld,CMatrix mtSys);
CVector _transform(CVector vPtInSys,CMatrix mtSys);
CVector _cross_product(CVector vA,CVector vB);

CVector operator*(CMatrix &matr, CVector &vect);
CVector operator*(CVector &vect,CMatrix &matr);

class CActionCutArea : public CInteractiveActionBase  
{
	DECLARE_DYNCREATE(CActionCutArea)
	GUARD_DECLARE_OLECREATE(CActionCutArea)

	ENABLE_UNDO();
	std::vector<CPoint>m_Points;
	std::vector<CPoint>m_DispPoints;
	std::vector<int>m_MinInter;
	std::vector<int>m_MaxInter;

	CObjectListUndoRecord	m_changes;	//undo
	BOOL m_bLBtnDown, m_bNeedToContinue;
	CPen *m_pPen;
	CPoint m_ptMax,m_ptMin;
	int m_Type;
	HCURSOR m_hcurScissors, m_hcurOld;
public:
	BOOL _same_direction( Contour *cFirstCont, Contour *cSecondCont );
	BOOL _clockwise_direction( Contour *pContour );
	CActionCutArea();
	virtual ~CActionCutArea();

	virtual bool Initialize();
	virtual void Finalize();
	virtual bool Invoke();

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual void DoDraw(CDC &dc); 

	virtual bool DoRButtonDown( CPoint pt );

	virtual bool DoRedo();
	virtual bool DoUndo(); 
	
	virtual bool IsAvaible();
	virtual bool DoMouseMove(CPoint point);
protected:	
//	int _line_fill( LPBYTE *pSrcMask, int w, int h, int x, int y, int dir, int PrevXl, int PrevXr, BYTE Color, BYTE Bk );
//	void _fill_contour( LPBYTE *pSrcMask, LPBYTE *pDestMask, int w, int h, LPPOINT Pts, int nSz, RECT &rcImage, BYTE Val, bool bIncludeBorder = true );
	void _check_contour( std::vector<CPoint> &PPoints );
	BOOL _fill_points(std::vector<CPoint>&Points,Contour *pContour, int *nIDs,  BOOL bClockwise, BOOL bIn = true, BOOL bFirst = true );
	BOOL _is_point_on_section(POINT pt, POINT ptA, POINT ptB);
	BOOL _is_pt_on_countor(POINT pt, LPPOINT Pts, int nSz);
	double _canonic(double x1, double y1,double x2, double y2,double x, double y,int sign);

	void _fill_points(std::vector<CPoint>&Points,POINT ptLast, POINT ptThis);
	BOOL _copy_image_data(IUnknown *pNewImage, IUnknown *pImage, LPBYTE *pDestImgMask, RECT &rcImage );
	IUnknown *_create_image( int w,int h, POINT Offset, _bstr_t ColorConvertorName, IUnknown *punkDevided );

	BOOL fill_dests_img(LPBYTE **pDestMask, LPBYTE *pSrcMask, int w, int h, RECT &rcImage, IUnknown *pImage, int &nTopID, IUnknown **pNewImage);

	void Invalidate(CPoint Pt1,CPoint Pt2);

	bool cut_area(sptrIDocument sptrD);
	void _get_img_rect(IUnknown *pImage,RECT &rc);
	BOOL _is_true_object(RECT &rc);

	double _get_y_by_x(double x,double x1,double y1,double x2, double y2);
	_bstr_t _GetCCName(IUnknown *pImage);
	double _get_x_by_y(double y,double x1,double y1,double x2, double y2);
	BOOL _point_in_polygon(LPPOINT Points, int Count, int x, int y, bool bIncludeBorder = true);
	bool _run();
	BOOL _is_contour_correct(LPBYTE *pDestMask, int w, int h, Contour *pContour, int sx, int sy  );
	void _activate_object( CPoint pt );
	bool _is_virtual( IUnknown *punkImage );
	long _get_square( IUnknown *punkImage );
};

#endif // !defined(AFX_ACTIONCUTAREA_H__7F640A4E_2BA1_11D6_BA8D_0002B38ACE2C__INCLUDED_)
