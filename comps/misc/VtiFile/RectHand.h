#if !defined(__RectHand_H__)
#define __RectHand_H__

#include "ImgFill.h"

class CRectHandObserver : public CObserver, public CObservable
{
	DECLARE_DYNAMIC(CRectHandObserver);
public:
	CRectHandObserver();
	~CRectHandObserver();
	virtual void OnUpdateObserver(CObject *pObservable, int nHint = 0, CObject *pObjHint = NULL);
};

class CRectHandler : public CObject
{
	static BOOL s_bDrawCurve;
	friend class CRectHandObserver;
protected:

	double m_fAngle;
	CRect	m_rcText;
	CDblRect  m_rcInit;
	double m_fSin,m_fCos;
	double m_fScaleX;
	double m_fScaleY;
	double m_fFract;
	// This data calculated inside basic data above by CalcData.
	CDblPoint m_ptCen;
	CPoint m_pt[5];
	// Used for curve
	CArray<CPoint,CPoint&> m_Curve;
	CPoint m_ptMed[2];
public:
	double GetAngle() { return m_fAngle; }
	static CRectHandObserver *m_pObserver;
	DECLARE_SERIAL(CRectHandler);
	enum ObjRectZone
	{
		Outside = -1,
		Move,
		SizeTop,
		SizeLeft,
		SizeBottom,
		SizeRight,
		SizeSynch0,
		SizeSynch1,
		SizeSynch2,
		SizeSynch3,
		Rotate0,
		Rotate1,
		Rotate2,
		Rotate3,
		Line,
		MoveFrame
	};
	CRectHandler(double fFract = -1);
	static void Init();
	static void Done();
	void operator=(CRectHandler &Sample);
	CRectHandler &operator+=(CPoint ptOff);
	void Rotate(double fSin, double fCos, double fAngle );
	void Resize();
	void DrawBoxes( CDC &theDC, POINT *pt, BOOL bXor, double fZoom );
	void DrawLine( CDC &theDC, BOOL bXor, double fZoom );
	void DrawCurve( CDC &theDC, BOOL bXor, double fZoom);
	void SetInitRect(CRect rc);
	virtual void GetData(CPoint *pt, CPoint *ptMed, CRect &rcInit,
		CDblPoint &ptCen, double &fSin, double &fCos, double &fFract);
	int GetZone(CPoint pt, double fZoom);
	void SetContour(POINT *pt); // After moving or resizing
	void SetContour(double fSin, double fCos); // After rotating
	void SetContour(POINT pt); // After moving, argument - new center
	void SetLine(double fFract);
	BOOL OnSetCursor(CPoint ptPos, double fZoom);
	static BOOL SetZoneCursor(int nZone, double fSin);
	CPoint *GetPoints() {return m_pt;}
	CRect GetRectInit() {return m_rcInit;}
	double GetFract() {return m_fFract;}
	void SetFract(double fFract) {m_fFract = fFract;}
	CDblPoint GetCenter() {return m_ptCen;}
	double GetScaleX() {return m_fScaleX;}
	double GetScaleY() {return m_fScaleY;}
	CRect CalcInvalidateRect();
	CPoint CalcOffs(int nZone, CPoint ptBmp);
	double Sin() {return m_fSin;}
	double Cos() {return m_fCos;}
	CDblPoint PtCen() {return m_ptCen;}
	CPoint CalcNewScale(int nAngle, CPoint pt, CPoint ptOffs);
	CDblRect CalcRealRect();
	virtual void CalcData();
	virtual void Serialize(CArchive &ar);
	static  BOOL IsDrawCurve() {return s_bDrawCurve;}

	void SetTextRect( CRect rc = NORECT )
	{ m_rcText = rc; }
protected:
	virtual void ModifyInitRect(int l, int t, int r, int b);// {m_rcInit = rc;}
};

#endif


