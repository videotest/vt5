// EditComment.cpp : implementation file
//

#include "stdafx.h"
#include "Editor.h"
#include "EditComment.h"
#include "action_comment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditComment

CEditComment::CEditComment(CWnd* pParent, CActionEditComment* pAction, double nFontZoom) : m_ptLastScrollPos(0,0)
{
	m_pParent = pParent;
	m_pAction = pAction;
	m_strText = "";
	int nAlign = ::GetValueInt(GetAppUnknown(), "Editor", "EditCommentText_Align", 0); //0-left; 1-center; 2-right
	UINT ES_TEXTALIGN = 0;
	switch(nAlign)
	{
	case 1:
		ES_TEXTALIGN = ES_CENTER;
		break;
	case 2:
		ES_TEXTALIGN = ES_RIGHT;
		break;
	default:
		ES_TEXTALIGN = ES_LEFT;
		break;
	}
	Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_TEXTALIGN, NORECT, pParent, 606);
	int nHeight = ::GetValueInt(GetAppUnknown(), "Editor", "EditCommentText_Height", 12);
	int nBold = ::GetValueInt(GetAppUnknown(), "Editor", "EditCommentText_Bold", 0);
	int nUnderline = ::GetValueInt(GetAppUnknown(), "Editor", "EditCommentText_Underline", 0);
	int nItalic = ::GetValueInt(GetAppUnknown(), "Editor", "EditCommentText_Italic", 0);
	CString strName = ::GetValueString(GetAppUnknown(), "Editor", "EditCommentText_Name", "Arial");
	int nStrikeOut = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_StrikeOut", 0);
	byte family = FF_DONTCARE;
	family = family << 4;
	family |= DEFAULT_PITCH;
	m_fontText.CreateFont((int)(nHeight*nFontZoom), 0, 0, 0,
							nBold==1?FW_BOLD:FW_NORMAL,
							nItalic==1?true:false,
							nUnderline==1?true:false,
							nStrikeOut==1?true:false,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
							CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
							family, strName );
	//m_fontText.CreateFont(10, 0, 0, 0, nBold==1?FW_BOLD:FW_NORMAL, nItalic==1?true:false, nUnderline==1?true:false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, family, strName);
	
	SetFont(&m_fontText);
}

CEditComment::~CEditComment()
{
}


BEGIN_MESSAGE_MAP(CEditComment, CEdit)
	//{{AFX_MSG_MAP(CEditComment)
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_KEYUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditComment message handlers

void CEditComment::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	//CFont* pFont = GetFont();
	//resize control

	CRect rcWindow = NORECT;
	GetWindowRect(&rcWindow);
	CString strCurEditText = "";
	::SetValue(GetAppUnknown(), "Editor", "CommentText_CurEditText", _variant_t(strCurEditText));

	
	if(!(/*(short)GetKeyState(VK_CONTROL) < 0 &&*/
 		nChar == 13))
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	CWindowDC	dc(this);
	CRect rcOld;
	//CFont font;
	//font.CreatePointFont(280, "Arial");
	//SetFont(&font);
	CFont* pOldFont = dc.SelectObject(&m_fontText);
	//dc.SelectObject(GetFont());

	CString	strText;
	CSize	size;
	GetWindowText(strText);
	CRect rc = NORECT;
	TEXTMETRIC tm;
	//if(!strText.IsEmpty())
	{
	//strText += nChar;
	//size = dc.GetTextExtent(strText);
	 int nStrPos = 0;
	 int nPrevPos = 0;
	 int nNumLines = 1;
	 int nWidth = 0;
	 CString	strSub;
	 while(nStrPos >= 0)
	 {
		 nStrPos = strText.Find("\n", nStrPos);
		 strSub = "";
		 if(nStrPos >= 0)
		 {
			 nNumLines++;
			 strSub = strText.Mid(nPrevPos, nStrPos-nPrevPos-1);
			 nStrPos++;
			 nPrevPos = nStrPos;
		 }
		 else
		 {
			 strSub = strText.Mid(nPrevPos);
		 }
		  size = dc.GetTextExtent( strSub );	
		  if(size.cx > nWidth)
			  nWidth = size.cx;
	 }
	 dc.GetTextMetrics(&tm);

	 size.cy = tm.tmHeight * nNumLines;
	 size.cx = nWidth;
	

	dc.SelectObject(pOldFont);
	//dc.DrawText(strText, &rc, DT_CALCRECT);
 	GetWindowRect(&rc);

//	if(nChar == 8 || nChar == 32 )//backspace
	{
		CRect rc2 = rc;
		m_pParent->ScreenToClient(&rc2);
		m_pParent->InvalidateRect(&rc2);
		rcOld = rc2;
//		m_pParent->UpdateWindow();
	}	
	HideCaret();
 	size.cx += tm.tmMaxCharWidth;
	size.cx = max(size.cx, rcWindow.Width());
	m_pParent->ScreenToClient(&rc);
	rc.right = rc.left + size.cx;
	rc.bottom = rc.top + size.cy;
	}
	//if((short)GetKeyState(VK_CONTROL) < 0)
	{
		if( nChar == 13 || nChar == 27 )// && !((short)GetKeyState(VK_CONTROL) < 0)) //enter
		{
			if( nChar == 27 && !m_pAction->m_strUndo.IsEmpty())
				m_pAction->m_bCancel = true;

			if(!strText.IsEmpty())
			{
				rc.right -= tm.tmMaxCharWidth;
				//rc.bottom -= tm.tmHeight;
				MoveWindow(&rc);
			}
			ShowCaret();
			m_pAction->Finalize();
			return;
		}

	}
	//if(!strText.IsEmpty())
	{
		/*
 		if( rcOld.Height() != rc.Height() )
		{
			if( rcOld.Height() < rc.Height() )
				rc.OffsetRect( 0, -abs(rcOld.Height() - rc.Height()) / 2 );
			else
				rc.OffsetRect( 0, abs(rcOld.Height() - rc.Height()) / 2 );
		}
		*/

		MoveWindow(&rc);
		//SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOZORDER|SWP_NOMOVE);
		ShowCaret();
		m_pParent->InvalidateRect(&rc);
//		m_pParent->UpdateWindow();
	}
	GetWindowText(strCurEditText);
	::SetValue(GetAppUnknown(), "Editor", "CommentText_CurEditText", _variant_t(strCurEditText));


	_SetTextInPage(strCurEditText);
}



void CEditComment::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetWindowRect( &rc );

	CWnd *pParent = GetParent();
	pParent->ScreenToClient( &rc );
	pParent->InvalidateRect( rc );

	
	CEdit::OnLButtonDown(nFlags, point);
}



void CEditComment::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	CString str = "";
	GetWindowText(str);
	if(!str.IsEmpty())
		_SetTextInPage(str);
}

void CEditComment::_SetTextInPage(CString strText)
{
	IUnknown *punkTextPage;
	CString strTextPage;
	strTextPage = c_szCTextPropPage;
	punkTextPage = GetPropertyPageByTitle(strTextPage, 0);
	sptrITextPage	sptrTextPage(punkTextPage);
	if(punkTextPage)
		punkTextPage->Release();
	if(sptrTextPage != 0)
	{
		sptrTextPage->SetText(_bstr_t(strText));
	}
}
void CEditComment::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar != VK_RETURN && nChar != VK_ESCAPE )
		OnChar( nChar, nRepCnt,nFlags );
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}
