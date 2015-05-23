#if !defined(__GalHlp_H__)
#define __GalHlp_H__

enum DrawFlags
{
	dfShow = 0,
	dfEdit = 1,
	dfActive = 2,
	dfOLEShow = 4,
	dfPrint = 8
};

enum ObjAlign
{
	OA_TOP,
	OA_BOTTOM,
	OA_CENTER,
	OA_CHROM,
};

enum NameAlign
{
	NA_TOP,
	NA_BOTTOM,
	NA_HIDDEN
};

interface IObject;
interface ICellInfo
{
	virtual CRect GetRect() = 0; // return m_rect
	virtual BOOL IsShowFrame() = 0;// return CGalView::s_bShowFrame
	virtual int  GetObjNumsCount() = 0;
	virtual BOOL IsObjVisible(int i) = 0;
	virtual CRect GetObjRect(int i) = 0;
	virtual __int64 GetObjNum(int i) = 0; // return m_arrObNums[i]
	virtual IObject *GetObj(__int64 i64) = 0;// return m_pparams->GetDocument()->GetObj( iObjNo )
	virtual int GetObjAlign() = 0; // return CGalView::s_ObjAlign
	virtual int GetNameAlign() = 0; // return CGalView::s_NameAlign
	virtual CString GetClassName() = 0;
	virtual int  GetClassNo() = 0; // return m_iClassNo
	virtual CSize GetObjSize() = 0; // return m_pparams->GetObjSize()
	virtual void DrawObjIcon(int nObj, CDC *pDC, RECT rcDib, RECT rcPaint, BOOL bForceStretch) = 0;
	virtual int GetObjNoByPoint(CPoint pt) = 0;	//return -1 if no object under given point
	virtual int GetPositionByPoint(CPoint pt) = 0;
};

interface IGalleryView
{
	virtual int GetCellByPoint(POINT ptPos) = 0; // Used inside OnLButtonDown
	virtual ICellInfo *GetCell(int nCell) = 0;
	virtual void InvalidateObjRect(int nCell, int nObj) = 0;
	virtual void SetActiveCell(int nCell) = 0;
	virtual void SetActiveObject(__int64 nObj) = 0;
	virtual void SetModifiedFlag(BOOL bFlag) = 0;
};

interface IGalleryHelper : public IUnknown
{
	virtual HRESULT __stdcall Draw(HDC hDC, double fZoom, POINT ptScroll, DWORD dwFlags, ICellInfo *pCellInfo) = 0;
	virtual SIZE __stdcall CalcMaxObjectSize(SIZE szMax, int nObjAlign) = 0;
	virtual BOOL __stdcall OnLButtonDown(int nFrameMode, double fZoom, POINT ptScroll, POINT ptPos, UINT uFlags, IGalleryView *pGalView) = 0;
	virtual BOOL __stdcall OnLButtonUp(int nFrameMode, double fZoom, POINT ptScroll, POINT ptPos, UINT uFlags, IGalleryView *pGalView) = 0;
	virtual BOOL __stdcall OnMouseMove(int nFrameMode, double fZoom, POINT ptScroll, POINT ptPos, UINT uFlags, IGalleryView *pGalView) = 0;
};


extern GUID IID_IGalleryHelper;


#endif