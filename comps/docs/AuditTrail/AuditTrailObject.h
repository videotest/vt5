#if !defined(AFX_AUDITTRAILOBJECT_H__02B70DC4_7462_4665_BE16_327F3C9D22D5__INCLUDED_)
#define AFX_AUDITTRAILOBJECT_H__02B70DC4_7462_4665_BE16_327F3C9D22D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuditTrailObject.h : header file
//
#include "auditint.h"
#include "StreamEx.h"

#define MAX_FROM_PURE_BLACK	10	//through every MAX_FROM_PURE_BLACK we generate new difference from pure black DIB
#define TEXT_INTERLINE_HEIGHT 5

#define MANAGE_HEAD														\
	if(!pHead)															\
	{																	\
		pHead = pCurDiff;												\
		pLast = pCurDiff;												\
	}																	\
	else																\
	{																	\
		pLast->pNext = pCurDiff;										\
		pLast = pCurDiff;												\
	}																	\
	pCurDiff = 0;

#define CLOSE_SEQ														\
	if(bColorSeq)														\
	{																	\
		bColorSeq = false;												\
		if(pCurDiff)													\
		{																\
			int nCount  = (pCurDiff->bPixelCount & 0x7f)*3;				\
			pCurDiff->pbData = new byte[nCount];						\
			memcpy(pCurDiff->pbData, pColorSeq, nCount);				\
			delete pColorSeq;											\
			pColorSeq = 0;												\
			MANAGE_HEAD													\
		}																\
	}																	\
	else																\
	{																	\
		bGraySeq = false;												\
		if(pCurDiff)													\
		{																\
			pCurDiff->pbData = new byte[pCurDiff->bPixelCount];			\
			memcpy(pCurDiff->pbData, pGraySeq, pCurDiff->bPixelCount);	\
			delete pGraySeq;											\
			pGraySeq = 0;												\
			MANAGE_HEAD													\
		}																\
	}
			
#define SET_GRAY_PIX													\
	pGraySeq[pCurDiff->bPixelCount] = R;								\
	pCurDiff->bPixelCount++;											
			
#define SET_COLOR_PIX													\
	int nCount  = pCurDiff->bPixelCount & 0x7f;							\
	pColorSeq[nCount*3] = R;											\
	pColorSeq[nCount*3+1] = G;											\
	pColorSeq[nCount*3+2] = B;											\
	pCurDiff->bPixelCount++;

#define FILL_BITMAPBITMAPINFOHEADER(header, size)						\
	ZeroMemory(&(header), sizeof(BITMAPINFOHEADER));					\
	(header).biBitCount = 24;											\
	(header).biHeight = (size).cy;										\
	(header).biWidth = (size).cx;										\
	(header).biSize = sizeof(BITMAPINFOHEADER);							\
	(header).biPlanes = 1;

struct DifferenceUnit
{
	byte bPixelCount; //highest bit - is flag of grayscale/truecolor (0/1) data; 7 other bits - counter
	byte* pbData; //difference data
	WORD nX;
	WORD nY;
	DifferenceUnit* pNext;
};

struct ThumbnailInfo
{
	CString strActionName;
	CString strParams;
	CString strActionTime;
	GuidKey keyAction;
	GuidKey keyImage;
};	

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailObject command target

class CAuditTrailObject : public CDataObjectBase,
						  public CPrintImpl
{
	PROVIDE_GUID_INFO()
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CAuditTrailObject)
	GUARD_DECLARE_OLECREATE(CAuditTrailObject)
	DECLARE_INTERFACE_MAP()

public:
	CAuditTrailObject();           // protected constructor used by dynamic creation
	virtual ~CAuditTrailObject();

	BEGIN_INTERFACE_PART(ATobj, IAuditTrailObject)
		com_call CreateThumbnail(IUnknown* punkDoc, IUnknown* punkImage, IUnknown* punkAction, BSTR bstrParams, BOOL bRender);
		com_call GetDocumentSize(SIZE* psizeDoc);
		com_call Draw(BITMAPINFOHEADER *pbi, BYTE *pdibBits, RECT rectDest);
		com_call GetThumbnailFromPoint(POINT ptCoord, long* pnIndex);
		com_call SetActiveThumbnail(long nIndex);
		com_call GetActiveThumbnail(long* pnIndex);
		com_call GetThumbnailRect(long nIndex, RECT* prcBound);
		com_call UndoAction(GuidKey* pKeyAction, GuidKey* pKeyImage);
		com_call SetFlags(DWORD dwFlags);
		com_call GetThumbnailsCount(long* pnCount);
		com_call GetThumbnailInfo(long nIdx, BSTR* pbstrActionName, BSTR* pbstrActionParams, BSTR* pbstrActionTime);
		com_call SetGridData( BYTE* pBuf, long lSize );
		com_call GetGridData( BYTE** ppBuf, long* plSize );
		com_call ReloadSettings( );
	END_INTERFACE_PART(ATobj)

	BYTE*	m_pGridData;
	long	m_lGridDataSize;
	void	_deinit_grid_data();

	int GetThumbnailsCount();
	CSize GetDocumentSize();

	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );				


//print functions
	bool GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
	bool GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
	void Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags);

	
protected:
	void CreateThumbnail(IUnknown* punkDoc, IUnknown* punkImage, IUnknown* punkAction, CString strParams, bool bRender);
	void Draw(BITMAPINFOHEADER *pbi, BYTE *pdibBits, CRect rectDest, bool bDrawSelection = true);
	long GetThumbnailFromPoint(CPoint ptCoord);
	void SetActiveThumbnail(long nIndex);
	CRect GetThumbnailRect(long nIndex);
	long GetTextLineHeight();
	bool UndoAction(GuidKey key, GuidKey* pKeyImage);
	void CleanUpDifferences(long nIndex = -1);
	void CleanUpInfos(long nIndex = -1);
	long GetDifferenceSize(DifferenceUnit* pHead);
	void RegenerateCurrentDIB();

public:
	static const char *c_szType;
	CSize	m_sizeThumbnail;
	long m_nBorder;

	CArray<ThumbnailInfo*, ThumbnailInfo*> m_arrThumbnailsInfo;

private:
	CArray<DifferenceUnit*, DifferenceUnit*> m_arrDifferences;
	CArray<CSize, CSize&> m_arrSizes;

		 
	byte*	m_pCurrentDIB;
	CSize	m_sizeCurrentDIB;
	byte	m_nFromPureBlackCount;
	long	m_nActiveThumbnail;
	CFont	m_fontText;
	bool	m_bOnlyText;
	bool	m_bShowActionParams;

	COLORREF	m_clrActive;
	COLORREF	m_clrInactive;
	COLORREF	m_clrActiveText;
	COLORREF	m_clrInactiveText;

	DWORD		m_dwFlags;

};

class DrawDib
{
public:
	DrawDib()
	{		m_hDrawDib = ::DrawDibOpen();	}
	~DrawDib()
	{		::DrawDibClose( m_hDrawDib );	}
	operator HDRAWDIB()
	{	return m_hDrawDib;}
protected:
	HDRAWDIB	m_hDrawDib;
};
extern DrawDib	g_DrawDibInstance;



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDITTRAILOBJECT_H__02B70DC4_7462_4665_BE16_327F3C9D22D5__INCLUDED_)
