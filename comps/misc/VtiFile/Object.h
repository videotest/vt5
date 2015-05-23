#ifndef __Object_h__
#define __Object_h__

#include "AppDefs.h"
#include "ObNative.h"
#include "Image.h"
#include "RectHand.h"

//some def's for filling
#define FRAME_BODY			0xFF
#define FRAME_HOLES_RAW		0x80
#define FRAME_HOLES_FILL	0x81
#define FRAME_BODY_FILL		0x82
#define FRAME_BODY_RAW		0x83
#define FRAME_MARK_INT		0x84
#define FRAME_MARK_EXT		0x85
#define FRAME_INT_WALKED	0x86
#define FRAME_CONT			0x87

#define BYTE_FREESPACE	0x00
#define BYTE_INTRNSPACE	0x02
#define BYTE_FILLEDBODY	0x03
#define BYTE_INTMARKER	0x04
#define BYTE_EXTMARKER	0x05
#define BYTE_CONTOUR	0x06
#define BYTE_RAWBODY	0xFF
#define BYTE_RAWHOLE	0xFE

enum OVERLAY_KIND
{
	OV_1X0 = 0,
	OV_3X1 = 1,
	OV_1X1 = 2,
	OV_1X3 = 3,
	OV_0X1 = 4,
	OV_S_3X1 = -1,
	OV_S_1X1 = -2,
	OV_S_1X3 = -3,
	OV_S_0X1 = -4,
};

//
//Object classes
//
//CObjNative
// |-CObjMove
//   |-CObjMeas
//     |-CObjAutoMeas
//	   |-CObjHandyMeas
//	     |-CObjCircle
//		 |-CObjRect
//		 |-...
//
//
//Contour classes
//
//CCntrNative
// |-CContour

const	CshortPoint ptDirect[8] = 
{
	CshortPoint( 0, -1), 
	CshortPoint( 1, -1), 
	CshortPoint( 1,  0), 
	CshortPoint( 1,  1), 
	CshortPoint( 0,  1), 
	CshortPoint(-1,  1), 
	CshortPoint(-1,  0), 
	CshortPoint(-1, -1)
};

class CContour : public CCntrNative
{
	DECLARE_DYNCREATE(CContour);
protected:
	BYTE	m_byteFill, m_byteBody;//fill value
	BYTE	m_byteBack, m_byteHole;
	BYTE	m_byteCont;	//contour
	int		m_iMinSize;	//minimal size of internal holes
	bool	m_bCycled;

public:
	CContour();


	//virtuals
	virtual void Realloc();
	virtual void AddSegment( CPoint );		//добавить сегмент
	//functions
	virtual void Walk(C8Image &img, CPoint ptStart, int=-1);		//обойти контур
	virtual void Map(C8Image &img, BYTE byte);			//Нарисовать контур на изображении
	virtual void FillFast( C8Image &img, BYTE byteExtMask, BYTE byteIntMask, CContourArray &array, C8Image *pimgMeas=0 );
	virtual void FillFast( C8Image &img, BYTE byteExtMask );
	void SetExternalMarks( C8Image &img, BYTE byteMarks, BYTE *p1stCol = 0 );
	virtual void Serialize( CArchive &ar );

	virtual void Walk( BITMAPINFOHEADER *pbi, CPoint ptStart, int idir=-1);
	virtual void FillFast( BITMAPINFOHEADER *pbi, BYTE byteExtMask, BYTE byteIntMask, CContourArray &parray );

public://interface
	virtual void SetBody( BYTE byte ){ m_byteBody = byte;};
	virtual void SetHole( BYTE byte ){ m_byteHole = byte;};
	virtual void SetBack( BYTE byte ){ m_byteBack = byte;};
	virtual void SetFill( BYTE byte ){ m_byteFill = byte;};
	virtual void SetCont( BYTE byte ){ m_byteCont = byte;};
	void SetMinSize( int iS ){m_iMinSize = iS;}
	int GetSingPoint(int i) {return m_pSingulPts[i];};
	int GetSingPtsNum() {return m_nSingulPtsNum;};
public:
	void DoPaint1st(CDC &theDC, double iZoom);		//Отрисовка на Paint
	void DoPaintNext(CDC &theDC, double iZoom);	//Отрисовка на WM_TIMER
	CPoint *CreateArray( int iZoom, CPoint ptOfs );
public:
	void DoAddPoint( CPoint &pt );
	void ApplySmoothFilter( DWORD dwNum, double *x, double *y );
	void SetSize(DWORD dwSize);
	void SetRect(CRect rc) {m_rc = rc;}
	virtual DWORD CalcBytesSize() const;
public://operators
	CContour &operator+=(CPoint pt);
	CContour &operator+=(CRect rc);
	CContour &operator-=(CPoint pt);
	CContour &operator-=(CRect rc);
	CContour &operator = (CContour c);
public://???
	BOOL m_bNotRemove;
	BOOL m_bFork;
	int  m_nSupLen;

	void SetCycled( bool bCycled ){m_bCycled = bCycled;}
	bool IsCycled(){return m_bCycled;}
};


class CDragState
{
	friend class CObjMove;
protected:
	C8Image	*m_pimgToMove;
	C8Image	*m_pimgToDraw;
	C8Image	*m_pimgBinToMove;
	C8Image	*m_pimgBinToDraw;
public:
	void operator=(CObjMove &);
	CDragState()
	{
		m_pimgToMove = NULL;
		m_pimgToDraw = NULL;
		m_pimgBinToMove = NULL;
		m_pimgBinToDraw = NULL;
	}
};

//object-derived classes 
class CBaseDoc;
class CObjArray;
class CObjMove : public CObjNative
{
	friend class CDragState;
	friend class CFrame;
	DECLARE_SERIAL( CObjMove );
protected:
	C8Image		*m_pimgBin; // Small binary image, containing this object.
	C8Image		*m_pimgHostBin; //Pointer to large binary image, containing all objects.
	C8Image		*m_pimgIcon;
	CImageBase	*m_pimg;
	// This members is to draw active zones and handle scaling and rotations.
	CRectHandler m_RectHandle;
	CContourArray m_OrigConts;
	// This members is to handle OLE Drag and Drop operations
	C8Image	*m_pimgToMove;
	C8Image	*m_pimgToDraw;
	C8Image	*m_pimgBinToMove;
	C8Image	*m_pimgBinToDraw;
	// for smart image rotating
	double m_fSavedAngle;
	double m_fSavedKx;
	double m_fSavedKy;
	C8Image m_imgToMoveRotated;
	C8Image m_imgBinToMoveRotated;

	OVERLAY_KIND	m_overlay;
	BOOL	m_bDragging;
	CPoint	m_ptOffset;
	BOOL	m_bDirectUnknown;
	BOOL	m_bNeedToSerializeIcon;

	CRect   CalcRect(); // calculates rect ( to use as m_rc member) by contours.
	CRect   CalcOrigRect(); // calculates rect ( to use as m_rc member) by contours.
private:
		//counter of ussage
	int	*m_plUsage;
public:
	static BOOL	s_bDrawSmoothed;
	static BOOL s_bDrawClass;
	static BOOL s_bDrawUnknown;
	BOOL	m_bFrame; //Is frame rather than object.
//	BOOL    m_bUndo;  // Is the object is created specially for undo.
	BOOL    m_bNotDivided; // This object couldn'tbe divided (used in DataSeparate2).
	CPoint &GetCenter() {return m_ptInitCen;}
private:
	double m_fInitAngle;
	double m_fInitKx;
	double m_fInitKy;
	CPoint m_ptInitCen;
public:
	CObjMove();
	virtual ~CObjMove();
	CObjNative *Clone();
	void AddArray( CObjNativeArray &arr );
public:
	virtual BOOL IsPointInObject( CPoint pt, BOOL bActive = FALSE );
	virtual BOOL IsImportedToGallery();
	BOOL IsEmpty(){return m_rc == NORECT;}
public:
	//interface
	virtual void AttachImg( void * );
	virtual void AttachHostBin( CImageBase * );
	virtual void AttachBin( CImageBase * );
	virtual void AttachCntr( CContourArray & );
	void operator=(CDragState &);
	void AttachImg1( CImageBase *pimg ) {m_pimg = pimg;}

	virtual void FindContours( BOOL bFindHoles = FALSE );

	virtual void CreateBIN( CSize size, BOOL bMapContours = FALSE );
	virtual void CreateMask( CCntrNative *p );
	virtual void CreateIcon( COLORREF crBack = RGB(255, 255, 255), BOOL bForceCreate = FALSE  );

	C8Image *GetBIN(){return m_pimgBin;}
	CImageBase *GetImg(){return m_pimg;}
	C8Image *GetIcon(){return m_pimgIcon;}

	virtual void AttachData( CObjNative & );
	virtual void Attach( CObjNative & );
	virtual void AttachMoveImage( CObjNative & );
	virtual void DeInit();
	virtual void DoFree();
	virtual CCntrNative	*CreateContour(){return new CContour;};
	virtual void DoPaint( CDC &theDC, double iZoom, int iType, BOOL bActive );

	void ResetIcon();

	virtual BOOL CanDraw(){return m_rc != NORECT;}

	virtual LPBITMAPINFOHEADER GetBinBI();
	virtual LPBITMAPINFOHEADER GetImgBI();
	virtual LPBITMAPINFOHEADER GetIconBI();

	virtual void Serialize( CArchive &ar );
	void SerializeIcon( CArchive &ar );
public:
	virtual void PaintFrame( CDC &theDC, double iZoom, BOOL bActive );
	virtual void PaintFrameNext( CDC &theDC, double iZoom );
	void MapToBin( C8Image &imgBin );
public:
	virtual void SetRect( CRect rc );
	operator CRect(){return m_rc;}
	CObjMove &operator +=( CObjMove & );
	CObjMove &operator -=( CObjMove & );
	virtual CObjNative& operator = (CObjNative &obj);


	void AttachNoCopy( CObjNative &obj )
	{ (*this) = obj;}
	void CreateOrigImage(BOOL  bBin);

public:
	// OLE Drag and Drop operations.
	void InitMoveImage( OVERLAY_KIND ok, CBaseDoc * );
	void DeInitMoveImage();
	void Overlay();
	void Map( CBaseDoc *pDoc, CRect rc = NORECT );
	virtual void MapBackground(CBaseDoc *pDoc, CObjNative *pExc = NULL);

	BOOL IsReadyToDrag()
	{	return m_pimgToMove != 0 ;}
	BOOL IsDraggingNow()
	{	return m_bDragging;}
	CPoint GetOffset()
	{	return m_ptOffset;}
	void SetOffset( CPoint pt )
	{	m_ptOffset = pt;}
	CImageBase *GetMoveImage() {return m_pimgToMove;}
	CImageBase *GetBinMoveImage() {return m_pimgBinToMove;}
	void SetMoveImage( C8Image *p )
	{	safedelete( m_pimgToMove );
		m_pimgToMove = p; }
	void SetBinMoveImage( C8Image *p )
	{	safedelete( m_pimgBinToMove );
		m_pimgBinToMove = p; }
	void SetDrawImage( C8Image *p )
	{	m_pimgToDraw = p; }
	void SetOverlay( OVERLAY_KIND ok )
	{	m_overlay = ok;}
	C8Image	*GetImgToDraw()
	{	return m_pimgToDraw;	}
	// If FALSE, object can be moved with content and not pertains
	// the form. If TRUE, the object can not be moved with content and
	// pertains the form.
	virtual BOOL IsPermanentAngle() {return FALSE;}

public:
	// This functions processes object moving, scaling and rotations.
	virtual BOOL OnBeginMove(CPoint pt, CPoint ptBmp, int MoveMode,
		double fZoom, CView *pView);
	virtual void OnMove(CPoint ptStart, CPoint ptEnd, double fZoom);
	virtual BOOL OnEndMove(CPoint pt, CPoint ptBmpStart, double fZoom,
		CRect &rcInv1, CRect &rcInv2, CView *pView);
	void MoveTo(CPoint pt, CView *pView, CDocument *pDoc);
	int GetMoveMode();
	// This function allows the object to set cursor for active zones
	virtual BOOL OnSetCursor(CPoint pt = CPoint(-1,-1), double fZoom = 1);
	virtual CRect CalcInvalidateRect();
	void PaintMoveImage(CDC &theDC, double fZoom, int iType, BOOL bActive, CObjArray *pArr, CPoint ptOffset);
	CRectHandler &GetRectHandle() {return m_RectHandle;}
	void MoveContour(POINT ptOffs);
	virtual void OnUpdate(CDocument *pDoc, CObjNative* pSender, LPARAM lHint, CObject* pHint);
	void Rotate(double fSin, double fCos, double fAngle);
	void FinalizeData(CView *pView);

	BOOL NeedToSerializeIcon( BOOL bNeed )
	{BOOL bold = m_bNeedToSerializeIcon;
	m_bNeedToSerializeIcon = bNeed;
	return bold;}
	virtual DWORD CalcBytesSize() const;
	double GetInitAngle() { return m_fInitAngle; }
	virtual HRESULT __stdcall MoveTo(int x, int y);

protected:
	void SetLine(double fFract, CView *pView);
	void ScaleInitContour(POINT *pt);
	void Resize();
	void PaintHandles(CDC &theDC, double fZoom, int iType, BOOL bActive );
	void AllocCopy();
	virtual void DoAllocCopy();
};

class CObjArray : public CObjNativeArray
{
public:
	virtual CObjNative *CreateObjNative();
	virtual CObjNative *CreateObjMove();
	virtual CObjNative *CreateObjMeasure();
	virtual void Attach( CObjNativeArray *parr );
	void AddArray( CObjNativeArray &arr );
	void AttachCopy( CObjNativeArray *parr );
	virtual CObjNativeArray *OnClone(BOOL bCopyContent);
};



///////////////////////////////////////////////////////////////////////////////
//inline
inline void CContour::DoAddPoint( CPoint &pt )
{
	if(m_rc == NORECT)
	{
		m_rc.left = pt.x;
		m_rc.top = pt.y;
		m_rc.right= pt.x+1;
		m_rc.bottom = pt.y+1;
	}
	else
	{
		m_rc.left = min(m_rc.left,  pt.x);
		m_rc.top = min(m_rc.top,  pt.y);
		m_rc.right= max(m_rc.right,  pt.x+1);
		m_rc.bottom = max(m_rc.bottom,  pt.y+1);
	}
}


inline void CContour::SetSize(DWORD dwSize)
{
	m_iSize = dwSize;
	if (unsigned(m_iMaxSize) >= dwSize)
		return;
	m_iMaxSize = dwSize;
	m_ptArray = (CshortPoint *)AllocMem(m_iMaxSize*sizeof(CshortPoint));
}

inline CContour &CContour::operator+=(CPoint pt)
{
	for(DWORD dw= 0; dw<unsigned(m_iSize); dw++)
		m_ptArray[dw]+=pt;
	m_rc+=pt;
	return *this;
}

inline CContour &CContour::operator+=(CRect rc)
{
	for(DWORD dw= 0; dw<unsigned(m_iSize); dw++)
		m_ptArray[dw]+=CshortPoint(rc.left, rc.top);
	m_rc+=CPoint(rc.left, rc.top);;
	return *this;
}


inline CContour &CContour::operator-=(CPoint pt)
{
	for(DWORD dw= 0; dw<unsigned(m_iSize); dw++)
		m_ptArray[dw]-=pt;
	m_rc-=pt;
	return *this;
}
inline CContour &CContour::operator-=(CRect rc)
{
	for(DWORD dw= 0; dw<unsigned(m_iSize); dw++)
		m_ptArray[dw]-=CshortPoint(rc.left, rc.top);
	m_rc-=CPoint(rc.left, rc.top);;
	return *this;
}

inline CContour &CContour::operator  = (CContour c)
{
	if(m_ptArray)
		FreeMem(m_ptArray);
	m_iSize = c.m_iSize;
	m_iMaxSize = c.m_iMaxSize;
	m_rc = c.m_rc;
	if(m_iSize)
	{
		m_ptArray = (CshortPoint *)AllocMem(m_iSize*sizeof(CshortPoint));
		memcpy(m_ptArray, c.m_ptArray, m_iSize*sizeof(CshortPoint));
	}
	else
		m_ptArray = 0;

	return *this;
}

void inline CContour::ApplySmoothFilter( DWORD dwNum, double *x, double *y )
{
	int	iSize = (int)GetSize();
	int	iNum  = (int)dwNum;


	if( IsEmpty() )
	{
		*x = -1;
		*y = -1;
		return;
	}

	if( iSize < 3 )
	{
		*x = m_ptArray[iNum].x;
		*y = m_ptArray[iNum].y;
		return;
	}
	int	dw1, dw2, dw3, dw4, dw5;

	if( m_bCycled )
	{
		dw1 = (iNum>1)?iNum-2:iSize+iNum-2;
		dw2 = (iNum>0)?iNum-1:iSize+iNum-1;
		dw3 = iNum;
		dw4 = (iNum<(iSize-1))?iNum+1:iNum-iSize+2;
		dw5 = (iNum<(iSize-2))?iNum+2:iNum-iSize+3;
	}
	else
	{
		dw1 = max( iNum-2, 0 );
		dw2 = max( iNum-1, 0 );
		dw3 = iNum;
		dw4 = min( iNum+1, iSize-1 );
		dw5 = min( iNum+2, iSize-1 );
	}

	*x = (m_ptArray[dw1].x+2*m_ptArray[dw2].x+3*m_ptArray[dw3].x+2*m_ptArray[dw4].x+m_ptArray[dw5].x)/9.;
	*y = (m_ptArray[dw1].y+2*m_ptArray[dw2].y+3*m_ptArray[dw3].y+2*m_ptArray[dw4].y+m_ptArray[dw5].y)/9.;
}



#endif //__Object_h__