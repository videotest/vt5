#ifndef __CellInfo_h__
#define __CellInfo_h__

#include "GalHlp.h"
#include "CellInfo.inl"

//class CImgDoc;
class CGalParams;
class CGalChangeInfo;


inline int SHORT_CODE( __int64 nCode ){return (int)((nCode)&0x0FFFFFFFF);}

//trackler
class CMyRectTracker : public CRectTracker
{
	int	m_iZoom;
public:
	CMyRectTracker(){ m_iZoom = 1; };
	CMyRectTracker( LPCRECT lpSrcRect, UINT nStyle ):CRectTracker( lpSrcRect, nStyle )
	{	
		m_iZoom = 1;	
		m_nHandleSize = 10;
	};


	void SetZoom( int iZoom ){m_iZoom = iZoom; };
	virtual void OnChangedRect( const CRect& rectOld );
};



class CCellInfo : public CObject, public ICellInfo
{
	DECLARE_SERIAL(CCellInfo)
protected:
	CDWordArray	m_arrObNums;
	CRect	m_rect;
	int		m_iClassNo;
	CGalParams	*m_pparams;
	CPoint	m_ptScroll;
public:
	CCellInfo();
	~CCellInfo();
public:
	virtual void Serialize( CArchive &ar );
public:
	virtual void Draw( CDC &theDC, double fZoom, CPoint ptScroll, UINT uFlags );
public:	//members access
	void AttachParams( CGalParams	*pparams ){ m_pparams = pparams;}

	CCellInfo &SetClass( int n );
	CCellInfo &SetRect( CRect rc );
	int	GetClass(){return m_iClassNo;}
//	CRect GetRect(){return m_rect;}
//vizible area properties
//	BOOL IsObjVisible( int iObjPos );
//	CRect GetObjRect( int iObjPos );

	int AddObject( __int64 iObjNo, int iPos = -1 );
	void RemoveObject( int iPos );

//	int GetObjNoByPoint( CPoint pt );	//return -1 if no object under given point
		//return ObjNo if some object is
		//pt - in zoom 1:1
//	int GetPositionByPoint( CPoint pt );
		//return position

	BOOL IsObjectInPos( int iPos );
	int GetObjPos( __int64 iObjNo );

	int GetObjCount(){return m_arrObNums.GetSize();}
	int GetFreePosition();
	void EmptyCell();
	CSize GetMaxSize();

//some for scrolling inside cell
	BOOL NeedScrollBar( UINT nBar );
	void SetScrollPos( UINT nBar, int iPos );
	int GetScrollPos( UINT nBar );
	int GetScrollSize( UINT nBar );

	CPoint GetScrollPoint()
	{return CPoint( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ) );	};

	BOOL EnsureVisible( __int64 iObjNo );

	CDWordArray	&GetObjArray(){return m_arrObNums;};

	// Methods for ICellInfo
public:
	virtual CRect GetRect() {return m_rect;}
	virtual BOOL IsShowFrame();
	virtual int  GetObjNumsCount() {return m_arrObNums.GetSize();}
	virtual BOOL IsObjVisible(int i);
	virtual CRect GetObjRect(int i);
	virtual __int64 GetObjNum(int i) {return m_arrObNums[i];}
	virtual IObject *GetObj(__int64 i64);
	virtual int GetObjAlign();
	virtual int GetNameAlign();
	virtual CString GetClassName();
	virtual int  GetClassNo() {return m_iClassNo;}
	virtual CSize GetObjSize();// {return m_pparams->GetObjSize();}
	virtual void DrawObjIcon(int nObj, CDC *pDC, RECT rcDib, RECT rcPaint, BOOL bForceStretch);
	virtual int GetObjNoByPoint(CPoint pt);	//return -1 if no object under given point
		//return ObjNo if some object is
	virtual int GetPositionByPoint(CPoint pt);
};

class CGalParams : public CTypedPtrArray<CObArray, CCellInfo*>
{
	DECLARE_SERIAL(CGalParams)
protected:
//	CImgDoc	*m_pDocument;
	int	m_iActiveCell;
	BOOL	m_bLockedUpdate;
	BOOL	m_bWriteToRegistry;
	CGXFont	m_font;
	CSize	m_sizeObj;
public:
	CGalParams();
	~CGalParams();
		//create default 
	void Init();
	void DeInit();
		//configuration interface
	void ReadState();
	void SaveState();

	void PlaceObjects();
	void DeleteContents();
public:
//	void AttachDocument( CImgDoc *pdoc );
//	CImgDoc *GetDocument();
public:
	virtual void Serialize( CArchive &ar );
public:
	virtual void Draw( CDC &theDC, double fZoom, CPoint ptScroll, UINT uFlags );
	virtual CSize GetDrawSize();

	virtual int Add( CCellInfo *pinfo );
//interface to active object
	int GetCellInfoByPoint( CPoint pt );
	int GetCellInfoByObjNo( __int64 iObjNo );
	int GetCellInfoByClass( int iClass );
	__int64 GetObjNoByPoint( CPoint pt );

	void DeleteCell();
	void InsertCell();
	void SetClass( int iPos, int iClass );

	CSize GetObjSize(){return m_sizeObj;}
	void UpdateObjSize();
	void UpdatePosition();

	CCellInfo *GetActiveCell();
	int GetActiveCellNo(){return m_iActiveCell;}
	void SetActiveCell( int idx );

	void ZoomCells( double fZoom );
	void ArrageCells();
	void AddCells( int iCount, int iCols = 5 );

	void LockUpdate( BOOL bLock ){m_bLockedUpdate = bLock;};
	void WriteRegistry( BOOL bWrite ){m_bWriteToRegistry = bWrite;}

	void UpdateAllViews( CGalChangeInfo *p );
	LOGFONT *GetFont();
};

#endif //__CellInfo_h__