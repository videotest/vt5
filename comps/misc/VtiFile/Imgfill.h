#if !defined(__ImgFill_H__)
#define __ImgFill_H__


#include "Image.h"
#include "SPoint.h"
#include "math.h"

class CInteger : public CObject
{
	int m_n;
	DECLARE_DYNAMIC(CInteger);
public:
	CInteger(int n) {m_n = n;}
	operator int() {return m_n;}
	CInteger &operator=(int n) {m_n = n; return *this;}
	int GetInt() {return m_n;}
	void SetInt(int n) {m_n = n;}
};

// Floating-point coordinates for the precious calculations
class CDblPoint
{
public:
	double x;
	double y;
	CDblPoint()
	{
		x = y = 0;
	}
	CDblPoint(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	CDblPoint(CPoint pt)
	{
		this->x = pt.x;
		this->y = pt.y;
	}
	CDblPoint(POINT pt)
	{
		this->x = pt.x;
		this->y = pt.y;
	}
	CDblPoint &operator=(CPoint pt)
	{
		this->x = pt.x;
		this->y = pt.y;
		return *this;
	}
	CDblPoint &operator=(CDblPoint pt)
	{
		this->x = pt.x;
		this->y = pt.y;
		return *this;
	}
	CDblPoint &operator+=(CPoint pt)
	{
		this->x += pt.x;
		this->y += pt.y;
		return *this;
	}
	CDblPoint &operator+=(CDblPoint pt)
	{
		this->x += pt.x;
		this->y += pt.y;
		return *this;
	}
	operator CPoint()
	{
		return CPoint(int(x),int(y));
	}
};

class CDblSize
{
public:
	double cx;
	double cy;
// Constructors
	CDblSize()
	{
		cx = cy = 0.;
	}
	CDblSize(double initCX, double initCY)
	{
		cx = initCX;
		cy = initCY;
	}
	CDblSize(SIZE initSize)
	{
		cx = initSize.cx;
		cy = initSize.cy;
	}
	CDblSize(POINT initPt)
	{
		cx = initPt.x;
		cy = initPt.y;
	}
	CDblSize(CDblSize &initSize)
	{
		cx = initSize.cx;
		cy = initSize.cy;
	}
	CDblSize(CDblPoint &initPt)
	{
		cx = initPt.x;
		cy = initPt.y;
	}

// Operations
	BOOL operator==(CDblSize size) const
	{
		return size.cx == cx && size.cy == cy;
	}
	BOOL operator!=(CDblSize size) const
	{
		return size.cx != cx || size.cy != cy;
	}
	void operator+=(CDblSize size)
	{
		cx += size.cx;
		cy += size.cy;
	}
	void operator-=(CDblSize size)
	{
		cx -= size.cx;
		cy -= size.cy;
	}
	BOOL operator==(SIZE size) const
	{
		return size.cx == cx && size.cy == cy;
	}
	BOOL operator!=(SIZE size) const
	{
		return size.cx != cx || size.cy != cy;
	}
	void operator+=(SIZE size)
	{
		cx += size.cx;
		cy += size.cy;
	}
	void operator-=(SIZE size)
	{
		cx -= size.cx;
		cy -= size.cy;
	}

// Operators returning CDblSize values
	CDblSize operator+(CDblSize size) const
	{
		return	CDblSize(cx+size.cx, cy+size.cy);
	}
	CDblSize operator-(CDblSize size) const
	{
		return	CDblSize(cx-size.cx, cy-size.cy);
	}
	CDblSize operator+(SIZE size) const
	{
		return	CDblSize(cx+size.cx, cy+size.cy);
	}
	CDblSize operator-(SIZE size) const
	{
		return	CDblSize(cx-size.cx, cy-size.cy);
	}
	CDblSize operator-() const
	{
		return	CDblSize(-cx, -cy);
	}

// Operators returning CDblPoint values
	CDblPoint operator+(CDblPoint point) const
	{
		return	CDblPoint(cx+point.x, cy+point.y);
	}
	CPoint operator-(CDblPoint point) const
	{
		return	CDblPoint(cx-point.x, cy-point.y);
	}
	CDblPoint operator+(POINT point) const
	{
		return	CDblPoint(cx+point.x, cy+point.y);
	}
	CPoint operator-(POINT point) const
	{
		return	CDblPoint(cx-point.x, cy-point.y);
	}

	operator SIZE()
	{
		SIZE sz;
		sz.cx = int(cx);
		sz.cy = int(cy);
		return sz;
	}
	CDblSize operator=(SIZE sz)
	{
		cx = sz.cx;
		cy = sz.cy;
		return *this;
	}
};

class CDblRect
{
public:
	double left;
	double top;
	double right;
	double bottom;
	CDblRect()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}
	CDblRect(double l, double t, double r, double b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	};
	CDblRect(CDblRect &rc)
	{
		left   = rc.left;
		top    = rc.top;
		right  = rc.right;
		bottom = rc.bottom;
	}
	CDblRect(CRect rc)
	{
		left   = rc.left;
		top    = rc.top;
		right  = rc.right;
		bottom = rc.bottom;
	}
	operator CRect();
	CDblRect &operator=(CDblRect rc)
	{
		left   = rc.left;
		top    = rc.top;
		right  = rc.right;
		bottom = rc.bottom;
		return *this;
	}
	CDblRect &operator=(CRect rc)
	{
		left   = rc.left;
		top    = rc.top;
		right  = rc.right;
		bottom = rc.bottom;
		return *this;
	}
	CDblRect &operator+=(CDblPoint pt)
	{
		left   += pt.x;
		top    += pt.y;
		right  += pt.x;
		bottom += pt.y;
		return *this;
	}
	CDblRect &operator+=(CPoint pt)
	{
		left   += pt.x;
		top    += pt.y;
		right  += pt.x;
		bottom += pt.y;
		return *this;
	}
	double Width()
	{
		return right-left;
	}
	double Height()
	{
		return bottom-top;
	}
	CDblPoint TopLeft()
	{
		return CDblPoint(left,top);
	}
	CDblPoint TopRight()
	{
		return CDblPoint(right,top);
	}
	CDblPoint BottomRight()
	{
		return CDblPoint(right,bottom);
	}
	CDblPoint BottomLeft()
	{
		return CDblPoint(left,bottom);
	}
	CDblPoint CenterPoint()
	{
		return CDblPoint((right+left)/2,(bottom+top)/2);
	}
};

// Calculates the distance from the point to the another point
double FDist(CDblPoint pt1, CDblPoint pt2);
#define Dist(x,y) int(FDist(x,y))
// Calculates the distance from the point to the lene represented
//by two points
double FDistLine(CDblPoint pt, CDblPoint ptLin1, CDblPoint ptLin2);
void FDistLine(CDblPoint pt, CDblPoint ptLin1, CDblPoint ptLin2, double *);
#define DistLine(x,y,z) int(FDistLine(x,y,z))
// Various rotations
void Rotate(CDblPoint ptCen, CPoint *pt, int nPoints, double sinf, double cosf);
void Rotate(CDblPoint ptCen, CshortPoint *pt, int nPoints, double sinf, double cosf);
inline void Rotate(CDblPoint ptCen, CPoint *pt, int nPoints, double a)
{ Rotate(ptCen,pt,nPoints,sin(a),cos(a));}
inline void Rotate(CDblPoint ptCen, CshortPoint *pt, int nPoints, double a)
{ Rotate(ptCen,pt,nPoints,sin(a),cos(a));}
void Resize(CPoint TopLeft, CshortPoint *pt, int nPoints, double fScaleX, double fScaleY);
// Asserts what quadrilateral, represented by 5 points(pt[4] == pt[0])
// is inside bordering square.
void CheckRect(CPoint *pt, CRect rcInit);
// Represents the sqare by 5 points(pt[4] == pt[0]).
void InitPoints(CRect rc, CPoint *pt);
// Calcultates the angle of vector with 'X' axis.
double AngleFromPt(CDblPoint pt);
// Calculate the angle using sine and cosine values.
double AngleFromSin(double fSin, double fCos);
// Whether the rotation from pt1 to pt2 around ptCen is clockwise.
BOOL IsClockwise(CDblPoint ptCen, CDblPoint pt1, CDblPoint pt2);
// Finds out the angle from pt1 to pt2, represented by sine and cosine.
void CalcAngle(CDblPoint ptCen, CPoint pt1, CPoint pt2, double &sinf, double &cosf);
// Calculetes the center of the quadriliteral, repesented by 5 points.
CPoint CalcCenter(CPoint *pt);

class CFill
{
protected:
	void DoFill(C8Image &img, CRect &rcFill, int x, int yc, int nTestCol,
		int nSetCol);
	void DoBordFill(C8Image &img, CRect &rcFill, int x, int yc, int nTestCol, int nSetCol);
	CPoint FindFirstPoint(C8Image &img, CRect rcFill, int nTestCol);
public:
	CRect m_rcMark;
	void FastFloodFill(C8Image &img, CRect &rcFill, int x, int y, int nTestClr, int nSetClr);
	void FastBordFill(C8Image &img, CRect &rcFill, int nTestClr, int nSetClr);
};

class CContour;
void FillContour(C8Image &img, CContour *c, CPoint ptOffs, BYTE nFill = 255, BYTE nContour = 255);
void FillByMask(CImageBase *pimg, C8Image &imgMask, C8Image &imgBigMask, COLORREF Color,
	CRect rc, CPoint ptOffsMask);

class CObserver
{
public:
	virtual void OnUpdateObserver(CObject *pObservable, int nHint = 0, CObject *pObjHint = NULL) = 0;
};

class IObservable
{
	virtual void AddObserver(CObserver *pObserver) = 0;
	virtual void RemoveObserver(CObserver *pObserver) = 0;
	virtual void Update(int nHint = 0, CObject *pObjHint = NULL) = 0;
};

class CObservable : public CPtrArray, public IObservable
{
	DECLARE_DYNAMIC(CObservable);
public:
	void AddObserver(CObserver *pObserver)
		{Add(pObserver);}
	void RemoveObserver(CObserver *pObserver);
	void Update(int nHint = 0, CObject *pObjHint = NULL);
};

class CAccumulate
{
	C16Image m_img;
	int m_nImagesNum;
public:
	CAccumulate();
	bool AddImage(C8Image &NewImg);
	bool GetImage(C8Image &NewImg);
};

extern CString _GetProfileString(const char *pSect, const char *pEntry, const char *pDef);
extern int _GetProfileInt(const char *pSect, const char *pEntry, int nDef);



#endif