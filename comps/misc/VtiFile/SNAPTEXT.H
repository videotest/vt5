#if !defined(__SnapText_H__)
#define __SnapText_H__

#include "image.h"

class __declspec(uuid("74E2A864-32A6-11D3-BBEF-000000000000")) CTextArea : public CObject, public IUnknown
{
	friend class CTextArray;
	friend class CTextPropsOperation;
	DECLARE_DYNCREATE(CTextArea);
protected:
	CTextArray *m_pArray;
//	BOOL     m_fDrag;
//	CPoint   m_ptBeginDrag;
	CFont    m_Font;
	LOGFONT  m_LogFont;
	int      m_nHeight;
	COLORREF m_Color;
	CString  m_sData;
	CRectTracker m_Tracker;
	int      m_nJustify;
	static BOOL s_bDrawHandles;
	CTextArea() {m_nRefs = 0;}; // Only for use in CTextArray::Serialize
public:
	CTextArea(LOGFONT *pLogFont, COLORREF Color, const char * pszData, CRect Rect, int nJustify);
	CTextArea(const char *pszFaceName, int nSize, COLORREF Color, const char * pszData, CRect Rect, int nJustify);
	virtual ~CTextArea();
	static void StaticInit();

	// initializing
	void Init(LOGFONT *pLogFont, COLORREF Color, const char * pszData, CRect Rect, int nJustify);

	// attributes
//	int GetSize() const { return m_LogFont.lfHeight;}
	int GetSize() const { return m_nHeight;}
	const char *GetFaceName() const { return m_LogFont.lfFaceName;}
	BOOL GetItalic() const { return m_LogFont.lfItalic;}
	BOOL GetUnderline() const { return m_LogFont.lfUnderline;}
	BOOL GetStrikeout() const { return m_LogFont.lfStrikeOut;}
	CRect GetRect() const { return m_Tracker.m_rect;}
	const char *GetData() const { return m_sData;}
	int GetWeight() const { return m_LogFont.lfWeight;}
	COLORREF GetColor() const { return m_Color;}
	virtual BOOL SetSize(int nNewSize);
	virtual BOOL SetFaceName(const char *pszFaceName);
	virtual BOOL SetItalic(BOOL fItalic);
	virtual BOOL SetUnderline(BOOL fUnderline);
	virtual BOOL SetStrikeout(BOOL fStrikeout);
	virtual BOOL SetRect(CRect Rect);
	virtual BOOL SetData(const char *pszData);
	virtual BOOL SetWeight(int nWeight);
	virtual BOOL SetColor(COLORREF Color);
	virtual int  GetJustify() {return m_nJustify;}
	virtual int  SetJustify(int nJustify);
	virtual void Resize(double dX, double dY);

	// redraw
	virtual void OnDraw(CDC *pDC, double fZoomX, double fZoomY, int fsOpts, BOOL fDrawRect, CPoint ptOffs = CPoint(0,0));

	// when undo changes font
	virtual void OnChange();


	// handlers
	virtual BOOL OnLButtonDown(UINT nFlags, double fZoom, CPoint ptOrg,	CPoint ptDev, CRect rcLimits);
	BOOL OnMouseMove(UINT nFlags, double fZoom, CPoint ptOrg,
		CPoint ptDev);
	virtual BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message,
		CPoint ptOrg, double fZoom);

	//saving into bitmap
	void Save(CImageBase *pimgDst, CImageBase *pimgSrc, double fZoomX = 1.0, double fZoomY = 1.0);
	void Serialize(CArchive &Arch);

	// for undo and other purposes
	virtual void Attach(CTextArea &area);

private:
	int m_nRefs;
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		if (riid == IID_IUnknown)
		{
			*ppvObject = (IUnknown *)this;
			return S_OK;
		}
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef() {return (ULONG)++m_nRefs;}
	virtual ULONG STDMETHODCALLTYPE Release()
	{
		if (--m_nRefs <= 0)
		{
			ASSERT(m_nRefs == 0);
			delete this;
		}
		return m_nRefs;
	}
};

class TextAreaPtr
{
protected:
	CTextArea *p;
public:
	TextAreaPtr(CTextArea *p = NULL)
	{
		this->p = p;
		if (p) p->AddRef();
	}
	TextAreaPtr(const TextAreaPtr& t)
	{
		p = t.p;
		if (p) p->AddRef();
	}
	~TextAreaPtr()
	{
		if (p) p->Release();
	}
	TextAreaPtr &operator=(TextAreaPtr &t)
	{
		if (p) p->Release();
		p = t.p;
		if (p) p->AddRef();
		return *this;
	}
	TextAreaPtr &operator=(CTextArea *p)
	{
		if (this->p) this->p->Release();
		this->p = p;
		if (p) p->AddRef();
		return *this;
	}
	TextAreaPtr &operator=(int null)
	{
		if (p) p->Release();
		p = 0;
		return *this;
	}
	operator CTextArea*() const
	{
		return p;
	}
	CTextArea *operator->()
	{
		return p;
	}
	int operator==(TextAreaPtr &t)
	{
		return p == t.p;
	}
};

class COperation;
class CDocTextInterface
{
public:
	virtual void Update(CRect) = 0;
	virtual void AddUndo(COperation *pOperation) = 0;
	virtual COLORREF GetDefaultColor() = 0;
};

class CTextArray : public CArray<TextAreaPtr,TextAreaPtr&>
{
	friend class CTextArea;
protected:
	LOGFONT m_DefLogFont;
	COLORREF m_DefColor;
protected:
	COLORREF GetDefaultColor();
	CDocTextInterface *m_pDoc;
	CWnd      *m_pWnd;
	void PutOnTop(int i);
public:
	enum TextDrawOptions {
		Focused   = 1,
		GrayScale = 2,
		BmpDC     = 4,
	};

	static TextAreaPtr m_pTopText;
	static WORD g_wLoadVersion;

	void Activate();
	virtual BOOL AddText(double fZoom, CPoint ptOrg, CPoint ptDev, CRect rcLimits);
	// construction
	CTextArray(CDocTextInterface *pDoc = NULL, CWnd *pWnd = NULL);
	~CTextArray();
	void Attach(CWnd *pWnd) { m_pWnd = pWnd;}
	void Attach(CDocTextInterface *pDoc) { m_pDoc = pDoc;}

	// working with stored data
	virtual void Add(TextAreaPtr pTextArea);
	TextAreaPtr& GetAt (int i) const;
	void Remove(TextAreaPtr p);
	void RemoveAt(int i);
	void RemoveAndDeleteAt(int i);
	void RemoveAndDeleteAll();
	virtual void OnDelete();

	// attributes
	int GetTextSize() const { return GetAt(GetSize()-1)->GetSize();}
	const char *GetFaceName() const { return GetAt(GetSize()-1)->GetFaceName();}
	BOOL GetItalic() const { return GetAt(GetSize()-1)->GetItalic();}
	BOOL GetUnderline() const { return GetAt(GetSize()-1)->GetUnderline();}
	BOOL GetStrikeout() const { return GetAt(GetSize()-1)->GetStrikeout();}
	COLORREF GetColor() const { return GetAt(GetSize()-1)->GetColor();}
	CRect GetRect() const { return GetAt(GetSize()-1)->GetRect();}
	const char *GetData() const { return GetAt(GetSize()-1)->GetData();}
	void SetTextSize(int nNewSize);
	void SetFaceName(const char *pszFaceName);
	void SetItalic(BOOL fItalic);
	void SetUnderline(BOOL fUnderline);
	void SetStrikeout(BOOL fStrikeout);
	void SetColor(COLORREF Color);
	void SetRect(CRect Rect);
	void SetData(const char *pszData);
	void Resize(double dX, double dY);

	// redraw
	virtual void OnDraw(CDC *pDC, double fZoom, int fsOpts, CPoint ptOffs = CPoint(0,0));

	// handlers
	virtual BOOL OnLButtonDown(UINT nFlags, double fZoom, CPoint ptOrg, CPoint ptDev, CRect rcLimits);
	virtual BOOL OnMouseMove(UINT nFlags, double fZoom,	CPoint ptOrg, CPoint ptDev);
	virtual BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message, CPoint ptOrg, double fZoom);

	//saving into bitmap
	virtual void Save(CImageBase *pimgDst, CImageBase *pimgSrc,
		double fZoomX = 1.0, double fZoomY = 1.0);
	virtual void Serialize(CArchive &Arch);
	CDocTextInterface *GetDoc() {return m_pDoc;}
	void Attach(CTextArray &area);
	void AttachNoCopy(CTextArray &area);
private:
	static void DefaultFont(LOGFONT *pLogFont);
};

#endif