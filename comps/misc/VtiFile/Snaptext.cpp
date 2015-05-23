#include "StdAfx.h"
#include "AppDefs.h"
#include "SnapText.h"
#include "resource.h"
#include "Image.h"

#pragma warning ( disable: 4244 )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WORD CTextArray::g_wLoadVersion;

static int font_height(int nHeight)
{
	CDC dc;
	VERIFY(dc.CreateIC("Display", NULL, NULL, NULL));;
	int nNewHeight =  -MulDiv(nHeight, dc.GetDeviceCaps(LOGPIXELSY), 72);
	ASSERT(abs(nNewHeight) > nHeight);
	return nNewHeight;
}

BOOL CTextArea::s_bDrawHandles = FALSE;


IMPLEMENT_DYNCREATE(CTextArea,CObject);

CTextArea::CTextArea(LOGFONT *pLogFont, COLORREF Color,
	const char *pszData, CRect Rect, int nJustify)
{
	m_nRefs = 0;
	Init(pLogFont,Color,pszData,Rect,nJustify);
}

CTextArea::CTextArea(const char *pszFaceName, int nHeight,
	COLORREF Color,	const char *pszData, CRect Rect, int nJustify)
{
	m_nRefs = 0;
	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	m_nHeight = nHeight;
	lf.lfHeight = font_height(nHeight);
	strncpy(lf.lfFaceName,pszFaceName,sizeof(lf.lfFaceName));
	Init(&lf,Color,pszData,Rect,nJustify);
}

CTextArea::~CTextArea()
{
//	if ((CTextArea *)CTextArray::m_pTopText == this)
//		CTextArray::m_pTopText = NULL;
}

void CTextArea::StaticInit()
{
	s_bDrawHandles = AfxGetApp()->GetProfileInt("Settings", "DrawTextHandles", 0);
}

void CTextArea::Init(LOGFONT *pLogFont, COLORREF Color,
	const char *pszData, CRect Rect, int nJustify)
{
	memcpy(&m_LogFont,pLogFont,sizeof(LOGFONT));
	m_nHeight = m_LogFont.lfHeight;
	m_LogFont.lfHeight = font_height(m_LogFont.lfHeight);
	m_Color = Color;
	m_sData = pszData;
	m_Tracker.m_nHandleSize = 20;
	m_Tracker.m_rect  = Rect;
	m_Tracker.m_nStyle = CRectTracker::dottedLine;
	VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
	m_pArray = NULL;
	m_nJustify = nJustify;
}

BOOL CTextArea::SetSize(int nNewSize)
{
	if( !m_pArray )
		return FALSE;
	if (nNewSize!=m_LogFont.lfHeight)
	{
		ASSERT(m_pArray);
		ASSERT(m_pArray->m_pDoc);
		m_nHeight = nNewSize;
		m_LogFont.lfHeight = font_height(nNewSize);
		VERIFY(m_Font.DeleteObject());
		VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
		return TRUE;
	}
	else return FALSE;
}

BOOL CTextArea::SetFaceName(const char *pszFaceName)
{
	if( !m_pArray )
		return FALSE;
	if (strcmp(m_LogFont.lfFaceName,pszFaceName))
	{
		ASSERT(m_pArray);
		ASSERT(m_pArray->m_pDoc);
		memcpy(m_LogFont.lfFaceName,pszFaceName,sizeof(m_LogFont.lfFaceName));
		VERIFY(m_Font.DeleteObject());
		VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
		return TRUE;
	}
	else return FALSE;
}

BOOL CTextArea::SetItalic(BOOL fItalic)
{
	if( !m_pArray )
		return FALSE;
	if (fItalic!=m_LogFont.lfItalic)
	{
		ASSERT(m_pArray);
		ASSERT(m_pArray->m_pDoc);
		m_LogFont.lfItalic = fItalic;
		VERIFY(m_Font.DeleteObject());
		VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
		return TRUE;
	}
	else return FALSE;
}

BOOL CTextArea::SetUnderline(BOOL fUnderline)
{
	if( !m_pArray )
		return FALSE;
	if (fUnderline!=m_LogFont.lfUnderline)
	{
		ASSERT(m_pArray);
		ASSERT(m_pArray->m_pDoc);
		m_LogFont.lfUnderline = fUnderline;
		VERIFY(m_Font.DeleteObject());
		VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
		return TRUE;
	}
	else return FALSE;
}

BOOL CTextArea::SetStrikeout(BOOL fStrikeout)
{
	if( !m_pArray )
		return FALSE;
	if (fStrikeout!=m_LogFont.lfStrikeOut)
	{
		ASSERT(m_pArray);
		ASSERT(m_pArray->m_pDoc);
		m_LogFont.lfStrikeOut = fStrikeout;
		VERIFY(m_Font.DeleteObject());
		VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
		return TRUE;
	}
	else return FALSE;
}

BOOL CTextArea::SetWeight(int nWeight)
{
	if( !m_pArray )
		return FALSE;
	if (nWeight!=m_LogFont.lfWeight)
	{
		ASSERT(m_pArray);
		m_LogFont.lfWeight = nWeight;
		VERIFY(m_Font.DeleteObject());
		VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
		return TRUE;
	}
	else return FALSE;
}

BOOL CTextArea::SetJustify(int nJustify)
{
	if( !m_pArray )
		return FALSE;
	if (m_nJustify != nJustify)
	{
		ASSERT(m_pArray);
		m_nJustify = nJustify;
		return TRUE;
	}
	else return FALSE;
}


BOOL CTextArea::SetRect(CRect Rect)
{
	CRect rcOld(m_Tracker.m_rect);
	if (m_Tracker.m_rect!=Rect)
	{
		m_Tracker.m_rect = Rect;
		ASSERT(m_pArray);
		return TRUE;
	}
	else return FALSE;
}

BOOL CTextArea::SetData(const char *pszData)
{
	if (m_sData!=pszData)
	{
		ASSERT(m_pArray);
		if( !m_pArray )
			return FALSE;
		m_sData = pszData;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CTextArea::SetColor(COLORREF Color)
{
	if (m_Color!=Color)
	{
		ASSERT(m_pArray);
		ASSERT(m_pArray->m_pDoc);
		m_Color = Color;
		return TRUE;
	}
	else return FALSE;
}

void CTextArea::Resize(double dX, double dY)
{
	CRect rc = m_Tracker.m_rect;
	m_Tracker.m_rect.left   = long(m_Tracker.m_rect.left*dX);
	m_Tracker.m_rect.right  = long(m_Tracker.m_rect.right*dX);
	m_Tracker.m_rect.top    = long(m_Tracker.m_rect.top*dY);
	m_Tracker.m_rect.bottom = long(m_Tracker.m_rect.bottom*dY);
	m_nHeight = (int)(m_nHeight*dY);
	m_LogFont.lfHeight = font_height(m_nHeight);
	VERIFY(m_Font.DeleteObject());
	VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
	rc.left   = min(rc.left,m_Tracker.m_rect.left);
	rc.right  = min(rc.right,m_Tracker.m_rect.right);
	rc.top    = max(rc.top,m_Tracker.m_rect.top);
	rc.bottom = max(rc.bottom,m_Tracker.m_rect.bottom);
	m_pArray->m_pDoc->Update(rc);
}

void CTextArea::OnChange()
{
	VERIFY(m_Font.DeleteObject());
	VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
}

void CTextArea::OnDraw(CDC *pDC, double fZoomX, double fZoomY,
	int fsOpts, BOOL fDrawRect, CPoint ptOffs)
{
}


BOOL CTextArea::OnLButtonDown(UINT nFlags, double fZoom, CPoint ptOrg, CPoint ptDev, CRect rcLimits)
{
	return FALSE;
}


BOOL CTextArea::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message,
	CPoint ptOrg, double fZoom)
{
	CRect rcSave(m_Tracker.m_rect);
	if (fZoom != 1.0)
	{
		m_Tracker.m_rect.left *= fZoom;
		m_Tracker.m_rect.right *= fZoom;
		m_Tracker.m_rect.top *= fZoom;
		m_Tracker.m_rect.bottom *= fZoom;
	}
	m_Tracker.m_rect.OffsetRect(-ptOrg.x,-ptOrg.y);
	BOOL bRet = m_Tracker.SetCursor(pWnd,nHitTest);
	m_Tracker.m_rect = rcSave;
	return bRet;
}

void CTextArea::Save(CImageBase *pimgDst, CImageBase *pimgSrc,
	double fZoomX, double fZoomY)
{
}

void CTextArea::Serialize(CArchive &Arch)
{
	if (Arch.IsLoading())
	{
		CString s;
		int nHeight;
		// Restore LOGFONT structure and current font
		memset(&m_LogFont,0,sizeof(LOGFONT));
		Arch >> s;
		strncpy(m_LogFont.lfFaceName,s,sizeof(m_LogFont.lfFaceName));
		Arch >> nHeight;
		m_nHeight = nHeight;
		m_LogFont.lfHeight = font_height(nHeight);
		Arch >> m_LogFont.lfItalic;
		Arch >> m_LogFont.lfUnderline;
		Arch >> m_LogFont.lfStrikeOut;
		Arch >> m_LogFont.lfWeight;
		VERIFY(m_Font.CreateFontIndirect(&m_LogFont));
		// Restore data
		Arch >> m_Color;
		Arch >> m_sData;
		// Restore rectangle
		Arch >> m_Tracker.m_rect.left;
		Arch >> m_Tracker.m_rect.right;
		Arch >> m_Tracker.m_rect.top;
		Arch >> m_Tracker.m_rect.bottom;
		if (CTextArray::g_wLoadVersion >= 3)
			Arch >> m_nJustify;
		else
			m_nJustify = 2;
	}
	else
	{
		// Restore log font
		CString s(m_LogFont.lfFaceName);
		Arch << s;
		Arch << m_nHeight;
		Arch << m_LogFont.lfItalic;
		Arch << m_LogFont.lfUnderline;
		Arch << m_LogFont.lfStrikeOut;
		Arch << m_LogFont.lfWeight;
		// Save data
		Arch << m_Color;
		Arch << m_sData;
		// Restore rectangle
		Arch << m_Tracker.m_rect.left;
		Arch << m_Tracker.m_rect.right;
		Arch << m_Tracker.m_rect.top;
		Arch << m_Tracker.m_rect.bottom;
		if (CTextArray::g_wLoadVersion >= 3)
			Arch << m_nJustify;
	}
}

void CTextArea::Attach(CTextArea &area)
{
	m_LogFont = area.m_LogFont;
	m_nHeight = area.m_nHeight;
	m_Color   = area.m_Color;
	m_sData   = area.m_sData;
	m_Tracker = area.m_Tracker;
	m_nJustify= area.m_nJustify;
}

TextAreaPtr CTextArray::m_pTopText;

CTextArray::CTextArray(CDocTextInterface *pDoc, CWnd *pWnd)
{
	m_pDoc = pDoc;
	m_pWnd = pWnd;
	DefaultFont(&m_DefLogFont);
	m_DefColor = GetDefaultColor();
}

CTextArray::~CTextArray()
{
	RemoveAndDeleteAll();
}


void CTextArray::Add(TextAreaPtr pTextArea)
{
	pTextArea->m_pArray = this;
	m_pTopText = pTextArea;
	CArray<TextAreaPtr,TextAreaPtr&>::Add(pTextArea);
}

TextAreaPtr& CTextArray::GetAt(int i) const
{
	return const_cast<TextAreaPtr&>(CArray<TextAreaPtr,TextAreaPtr&>::GetAt(i));
}

void CTextArray::Remove(TextAreaPtr p)
{
	int nSize = GetSize() - 1;
	for (; nSize>=0; nSize--)
		if ((CTextArea *)p == (CTextArea *)GetAt(nSize))
			RemoveAt(nSize);
}

void CTextArray::RemoveAt(int i)
{
	TextAreaPtr pTextArea = CArray<TextAreaPtr,TextAreaPtr&>::GetAt(i);
	CArray<TextAreaPtr,TextAreaPtr&>::RemoveAt(i);
	pTextArea->m_pArray = NULL;
}

void CTextArray::RemoveAndDeleteAt(int i)
{
	RemoveAt(i);
	/*
	CTextAreaPtr pTextArea = (CTextArea *)CObArray::GetAt(i);
	CArray<TextAreaPtr,TextAreaPtr&>::RemoveAt(i);
	delete pTextArea;
	*/
}

void CTextArray::RemoveAndDeleteAll()
{
	int nSize = GetSize() - 1;
	for (; nSize>=0; nSize--)
		RemoveAndDeleteAt(nSize);
}

void CTextArray::OnDelete()
{
	int nPos = GetSize() - 1;
	if (nPos >= 0)
	{
		CRect rc1,rc2 = NORECT;
		TextAreaPtr p = GetAt(nPos);
		rc1 = p->m_Tracker.m_rect;
		RemoveAndDeleteAt(nPos);
		nPos = GetSize() - 1;
		if (nPos >= 0)
		{
			PutOnTop(nPos);
			p = GetAt(nPos);
		}
		rc2 = p->m_Tracker.m_rect;
		m_pDoc->Update(rc1);
	}
}

void CTextArray::SetTextSize(int nNewSize)
{
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetSize(nNewSize))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::SetFaceName(const char *pszFaceName)
{
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetFaceName(pszFaceName))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::SetItalic(BOOL fItalic)
{
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetItalic(fItalic))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::SetUnderline(BOOL fUnderline)
{
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetUnderline(fUnderline))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::SetStrikeout(BOOL fStrikeout)
{
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetStrikeout(fStrikeout))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::SetColor(COLORREF Color)
{
	if (GetSize() <= 0) return;
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetColor(Color))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::SetRect(CRect Rect)
{
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetRect(Rect))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::SetData(const char *pszData)
{
	if (!GetSize()) return;
	TextAreaPtr p = GetAt(GetSize()-1);
	if (p->SetData(pszData))
	{
		m_pDoc->Update(p->m_Tracker.m_rect);
	}
}

void CTextArray::Resize(double dX, double dY)
{
	for ( int i = 0; i < GetSize(); i++)
		GetAt(i)->Resize(dX, dY);
}

void CTextArray::OnDraw(CDC *pDC, double fZoom, int fsOpts, CPoint ptOffs)
{
	for ( int i = 0; i < GetSize(); i++)
		GetAt(i)->OnDraw(pDC,fZoom,fZoom,i<GetSize()-1?fsOpts&~Focused:fsOpts,TRUE,ptOffs);
}

BOOL CTextArray::OnLButtonDown(UINT nFlags, double fZoom, CPoint ptOrg, CPoint ptDev, CRect rcLimits)
{
	for (int nSize = GetSize()-1; nSize>=0; nSize--)
		if (GetAt(nSize)->OnLButtonDown(nFlags,fZoom,ptOrg,ptDev,rcLimits))
		{
			PutOnTop(nSize);
			return TRUE;
		}
	return AddText(fZoom,ptOrg,ptDev,rcLimits);
}

BOOL CTextArray::OnMouseMove(UINT nFlags, double fZoom,
	CPoint ptOrg, CPoint ptDev)
{
/*
	for (int nSize = GetSize()-1; nSize>=0; nSize--)
		if (GetAt(nSize)->OnMouseMove(nFlags,fZoom,ptOrg,ptDev))
		{
			return TRUE;
		}
*/
	return FALSE;
}

BOOL CTextArray::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message,
	CPoint ptOrg, double fZoom)
{
	int nSize = GetSize();
	if (nSize > 0)
	{
		return GetAt(nSize - 1)->OnSetCursor(pWnd, nHitTest,
			message, ptOrg, fZoom);
	}
	return FALSE;
}

void CTextArray::PutOnTop(int i)
{
	ASSERT(i>=0);
	ASSERT(i<GetSize());
//	if (i != GetSize()-1)
//	{
		TextAreaPtr pTextArea;
		CRect rcOld(0,0,0,0);
		ASSERT(m_pDoc);
		if (GetSize())
		{
			pTextArea = GetAt(GetSize()-1);
			rcOld = pTextArea->m_Tracker.m_rect;
			m_pDoc->Update(rcOld);
		}
		pTextArea = GetAt(i);
		RemoveAt(i);
		Add(pTextArea);
		m_pDoc->Update(pTextArea->m_Tracker.m_rect);
		m_pTopText = pTextArea;
//	}
}

void CTextArray::DefaultFont(LOGFONT *pLogFont)
{
	memset(pLogFont,0,sizeof(*pLogFont));
	strcpy(pLogFont->lfFaceName,"System");
}

COLORREF CTextArray::GetDefaultColor()
{
	if (m_pDoc)
		return m_pDoc->GetDefaultColor();
	else
		return RGB(0,0,0);
}


BOOL CTextArray::AddText(double fZoom, CPoint ptOrg, CPoint ptDev, CRect rcLimits)
{
	return FALSE;
}

void CTextArray::Save(CImageBase *pimgDst, CImageBase *pimgSrc,
	double fZoomX, double fZoomY)
{
}

void CTextArray::Activate()
{
	if (GetSize() > 0)
	{
		m_pTopText = GetAt(GetSize()-1);
	}
}

void CTextArray::Serialize(CArchive &Arch)
{
	if (Arch.IsLoading())
	{
		int nItems;
		RemoveAndDeleteAll();
		Arch >> nItems;
		for (int i = 0; i < nItems; i++)
		{
			TextAreaPtr p = new CTextArea;
			p->Serialize(Arch);
			Add(p);
		}
		Activate();
	}
	else
	{
		int nItems = GetSize();
		Arch << nItems;
		for (int i = 0; i < nItems; i++)
		{
			TextAreaPtr p = (CTextArea *)GetAt(i);
			p->Serialize(Arch);
		}
	}
}

void CTextArray::AttachNoCopy(CTextArray &array)
{
	bool bThisActive = false;
	for ( int i = 0; i < GetSize(); i++)
	{
		if ((CTextArea *)m_pTopText == (CTextArea *)GetAt(i))
			bThisActive = true;
	}
	RemoveAndDeleteAll();
	for (i = 0; i < array.GetSize(); i++)
	{
		TextAreaPtr p1 = array.GetAt(i);
		CArray<TextAreaPtr,TextAreaPtr&>::Add(p1);
	}
	if (bThisActive)
		Activate();
}

void CTextArray::Attach(CTextArray &array)
{
	bool bThisActive = false;
	for ( int i = 0; i < GetSize(); i++)
	{
		if ((CTextArea *)m_pTopText == (CTextArea *)GetAt(i))
			bThisActive = true;
	}
	RemoveAndDeleteAll();
	for (i = 0; i < array.GetSize(); i++)
	{
		TextAreaPtr p1 = array.GetAt(i);
		TextAreaPtr p2 = (CTextArea *)p1->GetRuntimeClass()->CreateObject();
		p2->Attach(*p1);
		p2->m_pArray = this;
		CArray<TextAreaPtr,TextAreaPtr&>::Add(p2);
	}
	if (bThisActive)
		Activate();
}

