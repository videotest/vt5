#if !defined(__ChrUtils_H__)
#define __ChrUtils_H__

#include <math.h>
#include "Woolz.h"
#include "Chromosome.h"

#define CMAX 1000000.

#define safedelete(x) delete x; x = 0
#define INVCOORD  -10000
#define INVPOINT  CPoint(INVCOORD,INVCOORD)
#define INVFPOINT CFPoint(INVCOORD,INVCOORD)
#define INVANGLE  -10000.


BOOL GetPointDir( CImageWrp& ImgWrp,  double oX, double oY,	 double fAngle, CPoint &pt);
inline double GetLen( double dx, double dy ) {	return ::sqrt( dx*dx+dy*dy );};
double check_angle(double a);
Contour *max_contour(CImageWrp &pImgWrp);

class CIntervalImage;
class CAxisContext
{
	double fWidth; // Used in PrimaryAxis and SecondaryAxis
	int iWCount;

	void CorrectEnds(double daDef);
	void CorrectEnd1(double daDef, double daLim, bool bFirst);
	void CorrectEnd2(double daDef, double daLim, bool bFirst); // Using Assymetry
	void CorrectEnds3();
	void CorrectEnds4();
	void CorrectEnds5();
public:
	~CAxisContext();

	CImageWrp pImgWrp;
	double dLen;
	int nPoints;   // Number of points for axis
	int nPtsDist;  // Distance between this points
	double fAngle; // Object angle
	double fCos,fSin; // Its cosine and sine
	double *pfSegX; // Axis points coordinates
	double *pfSegY;
	Contour *pCntr; // Contour of object
	int nCount; // Number of points in contour
	CIntervalImage *pIntvl; // Image in form of interval
	bool m_bCurve;

	bool IsCurve();

	void Init(CImageWrp &pImgWrp, Contour *pCntr, CIntervalImage *pIntvl, int nPoints, int nPtsDist, double fAngle, double dLen);
	void PrimaryAxis();
	void SecondaryAxis();
	void SkeletonAxis();
	enum CorrectEndsStage
	{
		CEPrimary,
		CESecondary,
		CESkeleton
	};
	void CorrectEnds(int nStage);
};

VARIANT _GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry);
void _SetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, VARIANT var);
int _GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault, bool bSave);
int _SetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue);

class CIniValue
{
	static CIniValue *s_pFirst;
	CIniValue *m_pNext;
protected:
	int m_nInit;
	DWORD m_dwLastUpdated;
public:
	CIniValue();
	static void Clear();
};

class CIntIniValue : public CIniValue
{
	LPCTSTR m_lpEntry;
	LPCTSTR m_lpSection;
	int m_nValue;
public:
	CIntIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault = 0, bool bSave = false);
	operator int();
	int operator =(int nValue);
	void Save(int nValue);
};

class CColorIniValue : public CIniValue
{
	LPCTSTR m_lpEntry;
	LPCTSTR m_lpSection;
	COLORREF m_clrValue;
	int m_nInit;
public:
	CColorIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF nDefault);
	operator COLORREF();
	int operator =(COLORREF clrValue);
};

class CIntervalImage : public INTERVALIMAGE
{
public:
	CIntervalImage();
	~CIntervalImage();
	void Free();
	void MakeByImage(CImageWrp& ImgWrp);
};

class CFPoint : public FPOINT
{
public:
	CFPoint(int x, int y)
	{
		this->x = x;
		this->y = y;
	};
	CFPoint(double x, double y)
	{
		this->x = x;
		this->y = y;
	};
	CFPoint(POINT pt)
	{
		x = pt.x;
		y = pt.y;
	}
	CFPoint(FPOINT pt)
	{
		x = pt.x;
		y = pt.y;
	}
	CFPoint operator=(FPOINT pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
	CFPoint operator=(CPoint pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
	bool operator==(FPOINT pt)
	{
		return x==pt.x&&y==pt.y;
	}
	operator CPoint()
	{
		return CPoint((int)x,(int)y);
	}
};

class CStreamEx;
inline void operator>>(CStreamEx &arch, FPOINT &fp)
{
	arch >> fp.x;
	arch >> fp.y;
}

inline void operator<<(CStreamEx &arch, FPOINT fp)
{
	arch << fp.x;
	arch << fp.y;
}

inline void operator>>(CStreamEx &arch, CFPoint &fp)
{
	arch >> fp.x;
	arch >> fp.y;
}

inline void operator<<(CStreamEx &arch, CFPoint fp)
{
	arch << fp.x;
	arch << fp.y;
}

class CCentromere : public CENTROMERE
{
public:
	CCentromere()
	{
		ptCen = aptCenDraw[0] = aptCenDraw[1] = INVFPOINT;
		ptOffset = INVPOINT;
		nCentromere = -1;
		dAngle = INVANGLE;
	};
	void Offset(POINT ptOffset);
	void MirrorX(int x);
};

/*class CRotateParams : public ROTATEPARAMS2
{
public:
	CRotateParams()
	{
		dwSize = sizeof(ROTATEPARAMS2);
		dAngle = dAngleVisible = 0.;
		ptCenter = ptOffs = ptOffsR = ptOffsRM = CPoint(0,0);
		bManual = bMirrorV = bMirrorH;
	};
};*/

class CChromoAxis : public CHROMOAXIS, public IChromoAxis2
{
	bool m_bDelete;
	union
	{
		POINT  *m_iptAxisAlloc;
		FPOINT *m_fptAxisAlloc;
		int    *m_pnProfAlloc;
	};
	unsigned m_nAllocSize;
public:
	CChromoAxis();
	CChromoAxis(CChromoAxis &S);
	~CChromoAxis();
	CChromoAxis& operator=(CChromoAxis &S);
//	void Init(AxisType Type, unsigned nSize, void *pData = NULL); // Profile.cpp
	void Scale(int nScale);
	void Scale(double dScale);
	void Offset(CPoint ptOffs);
	FPOINT GetFPoint(int i);

	void Serialize(CStreamEx& ar);

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {return E_NOINTERFACE;}
	ULONG STDMETHODCALLTYPE AddRef(void) {return 1;}
	ULONG STDMETHODCALLTYPE Release(void) {return 1;}
	void __stdcall Init(AxisType Type, unsigned long nSize, void *pData = NULL);
	void __stdcall Init(IChromoAxis2 *pCopyFrom);
	AxisType __stdcall GetType() {return m_Type;}
	long __stdcall GetSize() {return m_nSize;}
	void *__stdcall GetData();
	FPOINT __stdcall GetOffset() {return CFPoint(m_ptOffset.x,m_ptOffset.y);}
	void __stdcall SetOffset(FPOINT pt) {m_ptOffset = CPoint(int(pt.x),int(pt.y));}
	virtual int __stdcall GetScale() {return m_nScale;}
	virtual void __stdcall SetScale(int nScale) {m_nScale = nScale;}
	void __stdcall Offset(FPOINT pt); // Offsets data poits and changes ptOffset
	void __stdcall Rotate(FPOINT fptOrg, double dAngle);
	virtual void __stdcall MirrorX(double dxMirror);
	virtual void __stdcall MirrorY(double dyMirror);
	virtual void __stdcall Smooth(int nIterations, bool bScaled);
	virtual void __stdcall Convert(AxisType NewType);
	virtual void __stdcall Shorten(int nBeg, int nEnd);
};

void SliceAxis(IChromoAxis2 *pAxis, CImageWrp &img);


struct CChord
{
//	int sig;	/* non-zero if judged to be significant */
	int acon;	/* chord equation is acon*x - bcon*y = ccon */
	int bcon;	/* in object-central coordinates */
//	int ccon;
	int cl;		/* chord length, *8 */
//	int bl;	/* line number of bay bottom or bulge top */
//	int bk;	/* column number of bay bottom or bulge top */
//	int barea;	/* bay or bulge area, *8 */
//	int bd;		/* bay maximum depth or bulge max height, *8 */
};


class CChromoPerimeter
{
public:
	unsigned m_nSize;
	POINT *m_ppt;
	CChord *m_pChords;
	CChromoPerimeter();
	~CChromoPerimeter();
	void MakeByInterval(CIntervalImage *pIntvls);
	int  CalcPerim();
	CChord CalcAngle(int &nn1, int &nn2, int &s, int &c);
};

void rottrans(CImageWrp &pImgWrp, CIntervalImage *pIntImg, double radians, CImageWrp &pImgWrp1,
	CIntervalImage *&pIntImg1, POINT *pptOrg, bool bBlackOnWhite = false, bool bMakeGray = true);
CImageWrp _rotateimage(CImageWrp pImgWrp);
POINT rotatepoint(CIntervalImage *pIntImg, POINT pt, double angle);
POINT rotatepoint(POINT ptOrg, POINT pt, double angle);
FPOINT rotatepoint(FPOINT ptOrg, FPOINT pt, double angle);
CENTROMERE _rotatecentromere(POINT ptOrg, CENTROMERE Cen, double angle);
bool _calcdrawpoints(CENTROMERE *pCen, CChromoAxis *pLongAxis, CImageWrp &pImgWrp);
int _FindCentromereByPoint(CFPoint ptCentromere, CChromoAxis &pLongAxis);

inline bool IsInvPoint(POINT pt) {return pt.x <= INVCOORD || pt.y <= INVCOORD;}
inline bool IsInvPoint(FPOINT pt) {return pt.x <= INVCOORD || pt.y <= INVCOORD;}
inline bool IsInvAngle(double dAngle) {return dAngle <= INVANGLE;}

#endif
