#ifndef __imgfilterbase_h__
#define __imgfilterbase_h__

#include "filterbase.h"


class CImageFilterBase : public CFilterBase
{
public:
	struct MaskCol
	{
		int	xPos, yPos;
		int	nPointCount;
		bool	bAdd;
		MaskCol	*pNext;
	};
public:
	CImageFilterBase();
	virtual ~CImageFilterBase();

	bool InitImageFilter( int nStargeCount = 1 );
	bool InitHistMask( int nMaskSize );
	void InitColorCache( int nColor );
	void FinalizeImageFilter();
	void NotifyImage( int yPos, int cPos );
	
	void InitMask( int nRow );
	void InitApprHistMask( int nRow );
	void MoveMaskRight( bool bInit = false );
	void MoveApprHistMaskRight( bool bInit = false );

	virtual void InitImageResult();
	virtual void DoDrawMaskShape( CDC *pdc, int xCenter, int yCenter, int nMaskSize );
public:
	CImageWrp	m_image;
	CImageWrp	m_imageNew;
	int			m_cx, m_cy, m_colors;

	bool		m_bImageInitialized;
	bool		m_bMaskInitialized;

	long		m_nCurrentHistSum;		//the current mask state
	long		m_nCurrentHistArea;
	long		m_nCurrentMaskSize;
	color		m_nCurrentMedian;

	int			m_nMedianPos;

	long		m_nCurrentX, m_nCurrentY;
	long		*m_pCurrentHist;
	long		*m_pCurrentHistAppr;
	int			*m_pCurrentJumpUp;
	int			*m_pCurrentJumpDown;
	int			m_nCurrentMin, m_nCurrentMax;
protected:
	virtual bool IsAvaible();
	void _DeleteMask( MaskCol *pMask );
protected:
	inline color CalcMin();
	inline color CalcMax();
	inline color CalcMedian();


	MaskCol		*m_pMask;
	MaskCol		*m_pMaskFull;

	color		**m_pRowCache;	//the row cache. It is expanded - don't worry about negative values

	void _check();
};

//the specific class
class CThresholdingActionBase : public CFilterBase, 
			public CDrawObjectImpl
{
	ENABLE_MULTYINTERFACE();

	DECLARE_INTERFACE_MAP();
public:
	CThresholdingActionBase();
	~CThresholdingActionBase();
public:	//overrided
	virtual void DoDraw( CDC &dc );
	virtual void DoFinalizePreview();
	virtual void DoInitializePreview();
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;}
public:	//overrideable
	virtual bool MakeBinaryImage();
	virtual COLORREF GetFillColor();
	virtual bool MeasureBinaryImage();

	void InvalidateTargetWindow();
	
protected:
	IViewPtr		m_ptrTargetView;
	bool			m_bPreviewAttached;

	CImageWrp		m_image;
	CObjectListWrp	m_objects;
	CRect			m_rect;
	CWalkFillInfo	*m_pwfi;

	DWORD			m_dwOldFlags;
	int			m_nPane;
};


inline color CImageFilterBase::CalcMedian()
{
	int		nSum;
	long	*plHist;
	int	n = 0;

	nSum = 0;
	plHist = m_pCurrentHistAppr;


	while( 	nSum+*plHist < m_nMedianPos )
	{
		nSum+=*plHist;
		plHist++; 
		n++;
	}

	return ByteAsColor( n );
}

inline color CImageFilterBase::CalcMin()
{
	int		n = 0;
	long	*plHist = m_pCurrentHistAppr;


	while( 	!*plHist )
	{
		plHist++;
		n++;
	}
	return ByteAsColor( n );
}

inline color CImageFilterBase::CalcMax()
{
	int		n = 255;
	long	*plHist = m_pCurrentHistAppr+255;


	while( 	!*plHist )
	{
		plHist--;
		n--;
	}

	return ByteAsColor( n );
}


#endif //__imgfilterbase_h__