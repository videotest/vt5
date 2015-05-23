
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//	Copyright Llew S. Goodstadt 1998
//		http://www.lg.ndirect.co.uk    mailto:lg@ndirect.co.uk
//		you are hereby granted the right tofair use and distribution
//		including in both commercial and non-commercial applications.

#include <afxwin.h>
#include <math.h>
void EllipseToBezier(CRect& r, CPoint* cCtlPt);
struct LDPoint
{
	double x, y;	

	// Constructors
		LDPoint(){}
		LDPoint(double _x, double _y): x(_x), y(_y)	{}
		LDPoint(const CPoint& p): x(p.x), y(p.y)	{}
		LDPoint(const LDPoint& p): x(p.x), y(p.y)	{}
		LDPoint(const POINT& p): x(p.x), y(p.y)		{}

	// equality operators
		bool		operator ==(const LDPoint& other) const;
		bool		operator !=(const LDPoint& other) const;

	// Functions/binary-operators that return points or sizes
		LDPoint		OffsetBy(double dx, double dy) const;
		LDPoint		OffsetBy(const LDPoint sz) const;			//		should be LDSize
		LDPoint		operator +(const LDPoint& other) const;		//		should be LDSize
		LDPoint		operator -(const LDPoint& other) const;		//		should be LDSize * 2
		LDPoint		operator -() const;
		double		DistFrom2(const LDPoint&) const;
		double		DistFrom(const LDPoint&) const;
		LDPoint		ScaledBy(double f) const;
		LDPoint		operator*(double f) const;
		CPoint		GetCPoint() const;


	// Functions/assignement-operators that modify this point
		LDPoint&	Offset(double dx, double dy);
		LDPoint&	Offset(const LDPoint& sz);					//		should be LDSize
		LDPoint&	operator +=(const LDPoint& other);			//		should be LDSize
		LDPoint&	operator -=(const LDPoint& other);			//		should be LDSize
		LDPoint&	Scale(double f);
		LDPoint&	operator*=(double f);
};


struct LBezier
{
	LDPoint	p[4];
	void GetCPoint(CPoint* out);
	void TSplit(LBezier& Output, double t);
	void Split(LBezier& Left, LBezier& Right) const;
	double TAtLength(unsigned int& len) const;
	double TAtLength(double& len, double error = 0.5) const;
	double Length(double error = 0.5) const;
};

// equality operators
inline bool	LDPoint::operator ==(const LDPoint& other) const
	{	return other.x == x && other.y == y;}
inline bool LDPoint::operator !=(const LDPoint& other) const
	{	return other.x != x || other.y != y;}

	// Functions/binary-operators that return points or sizes
inline LDPoint LDPoint::OffsetBy(double dx, double dy) const
	{	return LDPoint(dx+x, dy+y);}
inline LDPoint LDPoint::OffsetBy(const LDPoint sz) const
	{	return LDPoint(sz.x + x, sz.y + y);}
inline LDPoint LDPoint::operator +(const LDPoint& sz) const
	{	return LDPoint(sz.x + x, sz.y + y);}
inline LDPoint LDPoint::operator -(const LDPoint& other) const
	{	return LDPoint(x-other.x, y-other.y);}
inline LDPoint LDPoint::operator -() const
	{	return LDPoint(-x, -y);}
inline double LDPoint::DistFrom2(const LDPoint& o) const
	{	return ((x - o.x) * (x - o.x) + (y - o.y) * (y - o.y));}
inline double LDPoint::DistFrom(const LDPoint& o) const
	{	return(sqrt(DistFrom2(o)));}
inline LDPoint LDPoint::ScaledBy(double f) const
	{return LDPoint(x * f, y * f);}
inline LDPoint LDPoint::operator*(double f) const
	{return LDPoint(x * f, y * f);}
inline CPoint LDPoint::GetCPoint() const
	{	return CPoint(static_cast<int>(x + 0.5), static_cast<int>(y + 0.5));}

	// Functions/assignement-operators that modify this point
inline LDPoint&	LDPoint::Offset(double dx, double dy)
	{x += dx; y += dy; return *this;}
inline LDPoint&	LDPoint::Offset(const LDPoint& sz)
	{x += sz.x; y += sz.y; return *this;}
inline LDPoint&	LDPoint::operator +=(const LDPoint& sz)
	{x += sz.x; y += sz.y; return *this;}
inline LDPoint&	LDPoint::operator -=(const LDPoint& sz)
	{x -= sz.x; y -= sz.y; return *this;}
inline LDPoint&	LDPoint::Scale(double f)
	{x *= f; y *= f; return *this;}
inline LDPoint&	LDPoint::operator*=(double f)
	{x *= f; y *= f; return *this;}