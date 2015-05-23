#ifndef __contourapi_h__
#define __contourapi_h__

#include "defs.h"
#include "image5.h"
#include "imagebase.h"

#include "dibsectionlite.h"

/*struct FillInfo
{
	long			nNumber;
	unsigned short	nX;
	unsigned short	nCount;
	color*			pColors;
	FillInfo*		pPrev;
};*/
class std_dll FillInfo
{
public:
	static FillInfo* Create();

	void CreateColors(int nSize);
	void Destroy();
	void KillColors();
	unsigned short	nX;
	unsigned short	nCount;
	color*			pColors;
	bool			bFill;
	FillInfo*		pNext;
};

struct Contour;

/////////////////////////////////////////////////////////////////////////////////////////
//CWalkFillInfo
//the helper class for image operation (selection, drawing etc)
class std_dll CWalkFillInfo
{
//constructor
public:
	CWalkFillInfo( IUnknown *punkImage );
	CWalkFillInfo( const CRect rect );
	virtual ~CWalkFillInfo();
//attributes
public:
//fill attributes ( colours )
	byte GetFillBody()	const	{return m_byteFillBody;}
	byte GetRawBody()	const	{return m_byteRawBody;}
	CRect GetRect() const		{return m_rect;}
	CDC *GetDC()				{return &m_dc;}
public:
//pixel exchange
	inline byte GetPixel( int x, int y ) const;		//get the pixel by location
	inline void SetPixel( int x, int y, byte b );	//set the pixel at given location
	inline byte *GetRowMask( int row );	//in relative coord
//mask exchange
	void AddImageMasks( IUnknown *punkImage );		//info <- image
	void AttachMasksToImage( IUnknown *punkImage );	//info -> image
	void ClearMasks();				//empty(zero) the area

//contours functions
	bool InitContours(bool bOnlyOne = false, CPoint pointStart = CPoint( -1, -1 ), CPoint *ppointFound=0, CRect rectBounds = NORECT, IUnknown* pimgBase = 0,  IUnknown** ppimgOut = 0, bool bBaseImageIsObject = false);
									//walk the contours
	void InitFillContours();		//walk the external contours
	void DetachContours();			//"detach" contours - clear list without free memory
	CRect CalcContoursRect();		//get the bounds contour rect
	CRect CalcNoEmptyRect();		//get the bounds "no empty" rect

	int GetContoursCount();			//get the contour count
	Contour	*GetContour( int nPos );//get the contour by position
	void AddContour( Contour *p );	//add the contour to the contour list
	void ClearContours();			//empty the contour list


	void AttachFillInfoToImage(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage,  CRect rcDraw, color* pColors, color* pBackColors, IUnknown *punkView = NULL);
	void AttachFillInfoToImageBin( CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo, IUnknown* punkImage,  CRect rcDraw, color* pColors, color* pBackColors, IUnknown *punkView = NULL);
	
	//void AttachFillInfoToImageAdd( IUnknown* punkImage,  CRect rcDraw, color* pColor, long nLineNumber = -1);
	//void AttachFillInfoToImage( IUnknown* punkImage,  CRect rcDraw, color* pColor, color* pBackColor);
	//void AttachFillInfoToImage( IUnknown* punkImage,  CRect rcDraw, color* pColors);
	//void RestoreFromPixArray(IUnknown* punkImage,CRect rcInvalid, bool bDeleteArray = true);
	//void AttachArrayToImage(IUnknown* punkImage, CRect rcInvalid, color* pColors);
	//void DeletePixArray(CRect rc = NORECT,long nLineNumber = -1);
	//long m_nLinesCount;
	bool InitFillInfo(CTypedPtrArray<CPtrArray, FillInfo*> *pptrFillInfo);

protected:
	void InitDIBSection();
protected:
	byte	m_byteFillBody;
	byte	m_byteRawBody;
	byte	m_byteContourMark;
	byte	m_byteExternalMark;
	byte	m_byteInternalMark;
	byte	m_byteHoleBody;

	byte	*m_pbytes;
	byte	**m_pprows;
	CRect	m_rect;
	CPtrArray	m_contours;
	CDIBSectionLite	m_dibs;
	CBitmap* m_pbmp_old;
	CDC		m_dc;
	int		m_iRowMemSize;

	//CTypedPtrArray<CPtrArray, FillInfo*> m_ptrFillInfo;
};


inline byte *CWalkFillInfo::GetRowMask( int row )
{
	return m_pprows[row+1]+1;
}

//get the pixel by location
inline byte CWalkFillInfo::GetPixel( int x, int y ) const 
{
	x -= m_rect.left;
	y -= m_rect.top;

	ASSERT( x > -2 && y > -2 && x < m_rect.Width()+2 && y < m_rect.Height()+2 );
	if( ( x > -2 )&&( y > -2 )&&( x < m_rect.Width()+2 )&&( y < m_rect.Height()+2 ) )
		return (*(m_pprows[y+1]+x+1));
	return 0;
}

//set the pixel at given location
inline void CWalkFillInfo::SetPixel( int x, int y, byte b )
{
	x -= m_rect.left;
	y -= m_rect.top;

	if( x > -2 || x < m_rect.Width()+2 || y > -2 || y < m_rect.Height()+2 )
		(*(m_pprows[y+1]+x+1)) = b;
}

//ContourDraw flags
enum	ContourDrawFlags
{
	cdfFill = 1<<0,
	cdfSpline = 1<<1,
	cdfClosed = 1<<2,
	cdfPoint = 1<<4,
};


//draw a contour
std_dll void ContourDraw( CDC &dc, Contour *pc, double fZoom, CPoint pointScroll, DWORD dwFlags );
std_dll void ContourDraw( CDC &dc, Contour *pc, IUnknown *punkVS, DWORD dwFlags );
//create an empty contour
std_dll Contour *ContourCreate();
//create the copy of contour
std_dll Contour *ContourCopy(Contour *pContourSrc);
//make contour empty
std_dll void ContourEmpty(Contour *pc);
//make contour compact
std_dll void ContourCompact(Contour *pc);
//add a point to the contour
std_dll void ContourAddPoint( Contour *pcntr, int x, int y, bool bAddSamePoints = true);
//Функция Walk. обойти контур
std_dll Contour *ContourWalk( CWalkFillInfo &info, int iStartX, int iStartY, int iStartDir =-1 );
//delete a contour
std_dll void ContourDelete( Contour *pcntr );
//offset a contour
std_dll void ContourOffset( Contour *pcntr, CPoint pointOffset );
//mark contour to walkinfo in specified byte
std_dll void ContourMark( CWalkFillInfo &info, Contour *pcntr, byte byteMark );
//mark external area of contour in specified byte
std_dll void ContourExternalMark( CWalkFillInfo &info, Contour *pcntr, byte byteMark, byte byteMarkContour );
//fill marked area to the specified color
std_dll void ContourFillArea( CWalkFillInfo &info, byte byteCntrMark, byte byteExtMark, byte byteFill, CRect rect = NORECT );
//fills the single area in wfi
std_dll Contour *ContourFillAreas( CWalkFillInfo &info );
//get the direction
std_dll int ContourGetDirection( Contour *pcntr, int idx );
//calc the contour sizes
std_dll CRect ContourCalcRect( Contour *pcntr );
//fill the contour holes
std_dll Contour *ContourFillHoles( Contour *pcntr );
//fill the marked area and find holes
std_dll void ContourFillAreaWithHoles( CWalkFillInfo &info, byte byteRawBody, byte byteExtMark, byte byteFill, byte byteIntMark, CRect rect, IUnknown* punkimage );
//kill last point entry
std_dll void ContourKillLastPoint( Contour *pcntr);
//set point
std_dll void ContourSetPoint( Contour *pcntr, int nNumberPoint, int x, int y);
//get point
std_dll void ContourGetPoint( Contour *pcntr, int nNumberPoint, int& x, int& y);
//smooth
std_dll void ContourApplySmoothFilter( Contour *pcntr, DWORD dwNum, double *x, double *y );
//Measure Perimeter
std_dll double ContourMeasurePerimeter(Contour *p, bool bUseSmooth = false, double fXCalibr = 1., double fYCalibr = 1.);

#endif //__contourapi_h__