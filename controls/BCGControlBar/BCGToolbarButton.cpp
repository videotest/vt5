//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

// BCGToolbarButton.cpp: implementation of the CBCGToolbarButton class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "BCGToolbar.h"
#include "BCGToolbarButton.h"
#include "BCGToolBarImages.h"
#include "BCGCommandManager.h"
#include "globals.h"
#include "BCGFrameWnd.h"
#include "BCGMDIFrameWnd.h"
#include "BCGOleIPFrameWnd.h"
#include "BCGUserToolsManager.h"
#include "BCGUserTool.h"
#include "BCGToolbarMenuButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CBCGToolbarButton, CObject, VERSIONABLE_SCHEMA | 1)

CLIPFORMAT CBCGToolbarButton::m_cFormat = (CLIPFORMAT)::RegisterClipboardFormat (_T("BCGToolbarButton"));

static const int nTextMargin = 3;
static const int nSeparatorWidth = 8;

CList<UINT, UINT> CBCGToolbarButton::m_lstProtectedCommands;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGToolbarButton::CBCGToolbarButton()
{
	Initialize ();
}
//*********************************************************************************
CBCGToolbarButton::CBCGToolbarButton(UINT uiID, int iImage, LPCTSTR lpszText, BOOL bUserButton,
									 BOOL bLocked)
{
	Initialize ();

	m_bLocked = bLocked;

	m_nID = uiID;
	m_bUserButton = bUserButton;
	SetImage (iImage);

	m_strText = (lpszText == NULL) ? _T("") : lpszText;

	if (m_nID != 0 && !m_bLocked)
	{
		if (m_bUserButton)
		{
			if (m_iUserImage != -1)
			{
				CMD_MGR.SetCmdImage (m_nID, m_iUserImage, TRUE);
			}
			else
			{
				m_iUserImage = CMD_MGR.GetCmdImage (m_nID, TRUE);
			}
		}
		else
		{
			if (m_iImage != -1)
			{
				CMD_MGR.SetCmdImage (m_nID, m_iImage, FALSE);
			}
			else
			{
				m_iImage = CMD_MGR.GetCmdImage (m_nID, FALSE);
			}
		}
	}
}
//*********************************************************************************
void CBCGToolbarButton::Initialize ()
{
	m_nID = 0;
	m_nStyle = TBBS_BUTTON;
	m_iImage = -1;
	m_iUserImage = -1;
	m_bUserButton = FALSE;
	m_bDragFromCollection = FALSE;
	m_bText = FALSE;
	m_bImage = TRUE;
	m_bWrap = FALSE;
	m_bWholeText = TRUE;
	m_bLocked = FALSE;
	m_bIsHidden = FALSE;
	m_bTextBelow = FALSE;

	m_rect.SetRectEmpty ();
	m_sizeText = CSize (0, 0);
}
//*********************************************************************************
CBCGToolbarButton::~CBCGToolbarButton()
{
}
//*********************************************************************************
void CBCGToolbarButton::CopyFrom (const CBCGToolbarButton& src)
{
	m_nID			= src.m_nID;
	m_bLocked		= src.m_bLocked;
	m_bUserButton	= src.m_bUserButton;
	m_nStyle		= src.m_nStyle;
	SetImage (src.m_bUserButton ? src.m_iUserImage : src.m_iImage);
	m_strText		= src.m_strText;
	m_bText			= src.m_bText;
	m_bImage		= src.m_bImage;
	m_bWrap			= src.m_bWrap;
					
	m_bDragFromCollection = FALSE;
}					
//***************************************************************************************
void CBCGToolbarButton::Serialize (CArchive& ar)
{
	CObject::Serialize (ar);

	if (ar.IsLoading ())
	{
		int iImage;

		ar >> m_nID;
		ar >> m_nStyle;	
		ar >> iImage;	
		ar >> m_strText;	
		ar >> m_bUserButton;
		ar >> m_bDragFromCollection;
		ar >> m_bText;
		ar >> m_bImage;

//!!!andy modification
		if( m_bUserButton || !s_bInitProgMode )
			SetImage (iImage);
//end modification
		// A.M. fix (SBT 838)
		m_nStyle &= ~TBBS_PRESSED;
	}
	else
	{
		ar << m_nID;		
		ar << m_nStyle;	
		ar << GetImage();
		ar << m_strText;
		ar << m_bUserButton;
		ar << m_bDragFromCollection;
		ar << m_bText;
		ar << m_bImage;
	}
}
//***************************************************************************************
CLIPFORMAT CBCGToolbarButton::GetClipboardFormat ()
{
	return m_cFormat;
}
//***************************************************************************************
CBCGToolbarButton* CBCGToolbarButton::CreateFromOleData  (COleDataObject* pDataObject)
{
	ASSERT (pDataObject != NULL);
	ASSERT (pDataObject->IsDataAvailable (CBCGToolbarButton::m_cFormat));

	CBCGToolbarButton* pButton = NULL;

	try
	{
		//-------------------------------------
		// Get file refering to clipboard data:
		//-------------------------------------
		CFile* pFile = pDataObject->GetFileData (GetClipboardFormat ());
		if (pFile == NULL)
		{
			return FALSE;
		}

		//-------------------------------------------------------
		// Connect the file to the archive and read the contents:
		//-------------------------------------------------------
		CArchive ar (pFile, CArchive::load);

		//----------------------------------------
		// First, read run-time class information:
		//----------------------------------------
		CRuntimeClass* pClass = ar.ReadClass ();
		ASSERT (pClass != NULL);

		pButton = (CBCGToolbarButton*) pClass->CreateObject ();
		ASSERT (pButton != NULL);

		pButton->Serialize (ar);
		ar.Close ();
		delete pFile;

		return pButton;
	}
	catch (COleException* pEx)
	{
		TRACE(_T("CBCGToolbarButton::CreateFromOleData. OLE exception: %x\r\n"),
			pEx->m_sc);
		pEx->Delete ();
		return NULL;
	}
	catch (CArchiveException* pEx)
	{
		TRACE(_T("CBCGToolbarButton::CreateFromOleData. Archive exception\r\n"));
		pEx->Delete ();
		return FALSE;
	}
	catch (CNotSupportedException *pEx)
	{
		TRACE(_T("CBCGToolbarButton::CreateFromOleData. \"Not Supported\" exception\r\n"));
		pEx->Delete ();
		return FALSE;
	}

	if (pButton != NULL)
	{
		delete pButton;
	}

	return NULL;
}
//***************************************************************************************
void CBCGToolbarButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
								BOOL bHorz, BOOL bCustomizeMode, BOOL bHighlight,
								BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (this);

	BOOL bHot = bHighlight;
	CSize sizeImage = (pImages == NULL) ? CSize (0, 0) : pImages->GetImageSize (TRUE);

	CBCGUserTool* pUserTool = NULL;
	if (g_pUserToolsManager != NULL && !m_bUserButton)
	{
		pUserTool = g_pUserToolsManager->FindTool (m_nID);
	}

	if (!bCustomizeMode && !bHighlight &&
		(m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE)))
	{
		CRect rectDither = rect;
		rectDither.InflateRect (-afxData.cxBorder2, -afxData.cyBorder2);

		CBCGToolBarImages::FillDitheredRect (pDC, rectDither);
	}

	int x = rect.left;
	int y = rect.top;

	int iTextLen = 0;

	if (IsDrawText () && !(m_bTextBelow && bHorz))
	{
		CString strWithoutAmp = m_strText;
		strWithoutAmp.Remove (_T('&'));

		iTextLen = pDC->GetTextExtent (strWithoutAmp).cx + 2 * nTextMargin;
	}

	int dx = 0;
	int dy = 0;

	if (m_bTextBelow && bHorz)
	{
		ASSERT (bHorz);

		dx = rect.Width ();
		dy = sizeImage.cy + 2 * nTextMargin;
	}
	else
	{
		//AY
		dx = bHorz ? rect.Height()-sizeImage.cy+sizeImage.cx/*rect.Width () - iTextLen */: rect.Width ();
		dy = bHorz ? rect.Height () : rect.Height () - iTextLen;
	}

	// determine offset of bitmap (centered within button)
	CPoint ptImageOffset;
	ptImageOffset.x = (dx - sizeImage.cx - 1) / 2;
	ptImageOffset.y = (dy - sizeImage.cy + 1) / 2;

	CPoint ptTextOffset (nTextMargin, nTextMargin);

	if (IsDrawText () && !(m_bTextBelow && bHorz))
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics (&tm);

		if (bHorz)
		{
			ptTextOffset.y = (dy - tm.tmHeight - 1) / 2;
		}
		else
		{
			ptTextOffset.x = (dx - tm.tmHeight + 1) / 2;
		}
	}

	CPoint ptImageOffsetInButton (0, 0);
	BOOL bPressed = FALSE;

	if ((m_nStyle & (TBBS_PRESSED | TBBS_CHECKED)) && !bCustomizeMode)
	{
		// pressed in or checked
		ptImageOffset.Offset (1, 1);
		bPressed = TRUE;

		ptTextOffset.y ++;

		if (bHorz)
		{
			ptTextOffset.x ++;
		}
		else
		{
			ptTextOffset.x --;
		}
	}

	BOOL bImageIsReady = FALSE;

	if ((m_nStyle & TBBS_PRESSED) || !(m_nStyle & TBBS_DISABLED) ||
		bCustomizeMode)
	{
		if (IsDrawImage () && pImages != NULL)
		{
			if (pUserTool != NULL)
			{
				pUserTool->DrawToolIcon (pDC,
					CRect (CPoint (x + ptImageOffset.x, y + ptImageOffset.y),
					sizeImage));
			}
			else
			{
				pImages->Draw (pDC, x + ptImageOffset.x, y + ptImageOffset.y, GetImage ());
			}
		}

		if (bCustomizeMode || m_nStyle & TBBS_PRESSED)
		{
			bImageIsReady = TRUE;
		}
	}

	BOOL bDisabled = (bCustomizeMode && !IsEditable ()) ||
		(!bCustomizeMode && (m_nStyle & TBBS_DISABLED));

	if (!bImageIsReady)
	{
		if (!bHighlight)
		{
			bHighlight = !(m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE));
		}

		if (IsDrawImage () && pImages != NULL)
		{
			if (pUserTool != NULL)
			{
				pUserTool->DrawToolIcon (pDC,
					CRect (CPoint (x + ptImageOffset.x, y + ptImageOffset.y),
					sizeImage));
			}
			else
			{
				pImages->Draw (pDC, x + ptImageOffset.x, y + ptImageOffset.y, GetImage (), 
								!bHighlight, bDisabled && bGrayDisabledButtons);
			}
		}
	}

	if ((m_bTextBelow && bHorz) || IsDrawText ())
	{
		//--------------------
		// Draw button's text:
		//--------------------
		pDC->SetTextColor (bDisabled ?
							globalData.clrGrayedText : 
								(bHot) ? CBCGToolBar::GetHotTextColor () :
										globalData.clrBtnText);
		CString strText = m_strText;
		CRect rectText = rect;
		UINT uiTextFormat = 0;

		if (m_bTextBelow && bHorz)
		{
			ASSERT (bHorz);

			ptTextOffset.y += sizeImage.cy + nTextMargin;
			uiTextFormat = DT_CENTER | DT_WORDBREAK;

			rectText.left = (rect.left + rect.right - m_sizeText.cx) / 2 + ptTextOffset.x;
			rectText.right = (rect.left + rect.right + m_sizeText.cx) / 2;
		}
		else
		{
			if (IsDrawImage ())
			{
				if (bHorz)
				{
					ptTextOffset.x += nTextMargin + sizeImage.cx;
				}
				else
				{
					ptTextOffset.y += nTextMargin + sizeImage.cy;
				}
			}

			rectText.left = x + ptTextOffset.x + nTextMargin;
			uiTextFormat = DT_SINGLELINE|DT_END_ELLIPSIS;
		}

		if (bHorz)
		{
			rectText.top += ptTextOffset.y;
			pDC->DrawText (strText, &rectText, uiTextFormat|DT_NOCLIP);
		}
		else
		{
			rectText.left = rectText.right - ptTextOffset.x;
			rectText.top = rect.top + ptTextOffset.y + nTextMargin;
			rectText.bottom = rect.top + 3 * ptTextOffset.y;

			uiTextFormat |= DT_NOCLIP;

			int iAmpIndex = strText.Find (_T('&'));
			if (iAmpIndex >= 0)
			{
				strText.Remove (_T('&'));

				//-----------------------------------------
				// Calculate underlined character position:
				//-----------------------------------------
				CRect rectSubText;
				rectSubText.SetRectEmpty ();
				CString strSubText = strText.Left (iAmpIndex + 1);

				pDC->DrawText (strSubText, &rectSubText, uiTextFormat | DT_CALCRECT);
				int y1 = rectSubText.right;

				rectSubText.SetRectEmpty ();
				strSubText = strText.Left (iAmpIndex);

				pDC->DrawText (strSubText, &rectSubText, uiTextFormat | DT_CALCRECT);
				int y2 = rectSubText.right;

				pDC->DrawText (strText, &rectText, uiTextFormat);

				int x = rect.left + nTextMargin - 1;

				pDC->MoveTo (x, rectText.top + y1);
				pDC->LineTo (x, rectText.top + y2);
			}
			else
			{
				pDC->DrawText (strText, &rectText, uiTextFormat);
			}
		}
	}

	//--------------------
	// Draw button border:
	//--------------------
	if (!bCustomizeMode && HaveHotBorder () && bDrawBorder)
	{
		if (m_nStyle & (TBBS_PRESSED | TBBS_CHECKED))
		{
			//-----------------------
			// Pressed in or checked:
			//-----------------------
			pDC->Draw3dRect (&rect,
				afxData.clrBtnShadow,
				afxData.clrBtnHilite);
		}
		else if (bHot && !(m_nStyle & TBBS_DISABLED) &&
			!(m_nStyle & (TBBS_CHECKED | TBBS_INDETERMINATE)))
		{
			pDC->Draw3dRect (&rect, afxData.clrBtnHilite,
									afxData.clrBtnShadow);
		}
	}
}
//***********************************************************************************
SIZE CBCGToolbarButton::OnCalculateSize (
								CDC* pDC,
								const CSize& sizeDefault,
								BOOL bHorz)
{
	ASSERT_VALID (pDC);

	CSize size = sizeDefault;

	if (m_nStyle & TBBS_SEPARATOR)
	{
		if (bHorz)
		{
			size.cx = m_iImage > 0 ? m_iImage : nSeparatorWidth;
		}
		else
		{
			size.cy = nSeparatorWidth;
		}
	}
	else
	{
		if (!IsDrawImage () || GetImage () < 0)
		{
			if (bHorz)
			{
				size.cx = 0;
			}
			else
			{
				size.cy = 0;
			}
		}

		m_sizeText = CSize (0, 0);

		if (!m_strText.IsEmpty ())
		{
			if (m_bTextBelow && bHorz)
			{
				//----------------------------------------------------------
				// Try format text that it ocuppies no more tow lines an its
				// width less than 3 images:
				//----------------------------------------------------------
				CRect rectText (0, 0, 
					sizeDefault.cx * 3, sizeDefault.cy);
				pDC->DrawText (	m_strText, rectText, 
								DT_CENTER | DT_CALCRECT | DT_WORDBREAK);
				m_sizeText = rectText.Size ();
				m_sizeText.cx += 2 * nTextMargin;

				size.cx = max (size.cx, m_sizeText.cx) + 4 * nTextMargin;
				size.cy += m_sizeText.cy + 2 * nTextMargin;
			}
			else if (IsDrawText ())
			{
				CString strWithoutAmp = m_strText;
				strWithoutAmp.Remove (_T('&'));

				int iTextLen = pDC->GetTextExtent (strWithoutAmp).cx + 3 * nTextMargin;

				if (bHorz)
				{
					size.cx += iTextLen;
				}
				else
				{
					size.cy += iTextLen;
				}
			}
		}
	}

	return size;
}
//************************************************************************************
BOOL CBCGToolbarButton::PrepareDrag (COleDataSource& srcItem)
{
	if (!CanBeStored ())
	{
		return TRUE;
	}

	try
	{
		CSharedFile globFile;
		CArchive ar (&globFile,CArchive::store);

		//---------------------------------
		// Save run-time class information:
		//---------------------------------
		CRuntimeClass* pClass = GetRuntimeClass ();
		ASSERT (pClass != NULL);

		ar.WriteClass (pClass);

		//---------------------
		// Save button context:
		//---------------------
		Serialize (ar);
		ar.Close();

		srcItem.CacheGlobalData (GetClipboardFormat (), globFile.Detach());
	}
	catch (COleException* pEx)
	{
		TRACE(_T("CBCGToolbarButton::PrepareDrag. OLE exception: %x\r\n"),
			pEx->m_sc);
		pEx->Delete ();
		return FALSE;
	}
	catch (CArchiveException* pEx)
	{
		TRACE(_T("CBCGToolbarButton::PrepareDrag. Archive exception\r\n"));
		pEx->Delete ();
		return FALSE;
	}

	return TRUE;
}
//****************************************************************************************
void CBCGToolbarButton::SetImage (int iImage)
{
	if (m_nStyle & TBBS_SEPARATOR)
	{
		m_iImage = iImage;	// Actualy, separator width!
		return;
	}

	if (m_bUserButton)
	{
		m_iUserImage = iImage;
	}
	else
	{
		m_iImage = iImage;
	}

	if (!m_bLocked)
	{
		if (m_nID != 0 && iImage != -1)
		{
			CMD_MGR.SetCmdImage (m_nID, iImage, m_bUserButton);
		}
		else if (m_nID != 0)
		{
			m_iImage = CMD_MGR.GetCmdImage (m_nID, FALSE);
			m_iUserImage = CMD_MGR.GetCmdImage (m_nID, TRUE);

			if (m_iImage == -1 && !m_bUserButton)
			{
				m_bUserButton = TRUE;
			}
			else if (m_iImage == -1 && m_bUserButton)
			{
				m_bUserButton = FALSE;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBCGToolbarButton diagnostics

#ifdef _DEBUG
void CBCGToolbarButton::AssertValid() const
{
	CObject::AssertValid();
}
//******************************************************************************************
void CBCGToolbarButton::Dump(CDumpContext& dc) const
{
	CObject::Dump (dc);

	CString strId;
	strId.Format (_T("%x"), m_nID);

	dc << "[" << strId << " " << m_strText << "]";
	dc << "\n";
}

#endif

int CBCGToolbarButton::OnDrawOnCustomizeList (
	CDC* pDC, const CRect& rect, BOOL bSelected)
{
	int iWidth = 0;

	ASSERT_VALID (pDC);

	CBCGToolBarImages* pImages = m_bUserButton ?
		CBCGToolBar::GetUserImages () :
		CBCGToolBar::GetImages ();
	
	CBCGUserTool* pUserTool = NULL;
	if (g_pUserToolsManager != NULL && !m_bUserButton)
	{
		pUserTool = g_pUserToolsManager->FindTool (m_nID);
	}

	CBCGToolBarImages* pCommonImages = CBCGToolBar::GetImages ();
	
	CSize sizeButton (0, 0);
	if (pCommonImages != NULL)
	{
		sizeButton = CSize (pCommonImages->GetImageSize ().cx + 6,
							pCommonImages->GetImageSize ().cy + 6);
	}

	CRect rectRow = rect;
	CRect rectText = rect;
	rectText.left += sizeButton.cx;

	iWidth = sizeButton.cx;

	//-------------------
	// Draw button image:
	//-------------------
	if (GetImage () >= 0 && pImages != NULL)
	{
		CRect rectImage = rect;
		rectImage.right = rectImage.left + sizeButton.cx;

		if (pUserTool != NULL)
		{
			pUserTool->DrawToolIcon (pDC, rectImage);
		}
		else
		{
			CBCGDrawState ds;
			pImages->PrepareDrawImage (ds);

			OnDraw (pDC, rectImage, pImages, TRUE, TRUE, bSelected);
			pImages->EndDrawImage (ds);
		}

		rectRow.left = rectImage.right;
	}

	//-------------------
	// Draw button text:
	//-------------------
	if (!m_strText.IsEmpty ())
	{
		COLORREF clrTextOld;

		if (bSelected) 
		{
			clrTextOld = pDC->GetTextColor ();
			pDC->FillSolidRect (rectRow, ::GetSysColor (COLOR_HIGHLIGHT));
			pDC->SetTextColor (::GetSysColor (COLOR_HIGHLIGHTTEXT));
		} 
		else
		{
			pDC->FillRect (rectRow, &globalData.brBtnFace);
			clrTextOld = pDC->SetTextColor (::GetSysColor (COLOR_BTNTEXT));
		}

		pDC->SetBkMode (TRANSPARENT);
		pDC->DrawText (m_strText, rectText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->SetTextColor (clrTextOld);

		int iTextWidth = min (rectText.Width (), pDC->GetTextExtent (m_strText).cx);
		iWidth += iTextWidth;
	}

	if (bSelected)
	{
		pDC->DrawFocusRect (rectRow);
	}

	return iWidth;
}
//*************************************************************************************
BOOL CBCGToolbarButton::OnToolHitTest(const CWnd* pWnd, TOOLINFO* pTI)
{
	CFrameWnd* pTopFrame = (pWnd == NULL) ? 
		(CFrameWnd*) AfxGetMainWnd () : 
		pWnd->GetTopLevelFrame ();

	CBCGMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGMDIFrameWnd, pTopFrame);
	if (pMainFrame != NULL)
	{
		return pMainFrame->OnMenuButtonToolHitTest (this, pTI);
	}
	else	// Maybe, SDI frame...
	{
		CBCGFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGFrameWnd, pTopFrame);
		if (pFrame != NULL)
		{
			return pFrame->OnMenuButtonToolHitTest (this, pTI);
		}
		else	// Maybe, OLE frame...
		{
			CBCGOleIPFrameWnd* pOleFrame = 
				DYNAMIC_DOWNCAST (CBCGOleIPFrameWnd, pFrame);
			if (pOleFrame != NULL)
			{
				return pOleFrame->OnMenuButtonToolHitTest (this, pTI);
			}
		}
	}

	return FALSE;
}
//*************************************************************************************
BOOL CBCGToolbarButton::ExportToMenuButton (CBCGToolbarMenuButton& menuButton) const
{
	//-----------------------------------------------------
	// Text may be undefined, bring it from the tooltip :-(
	//-----------------------------------------------------
	if (m_strText.IsEmpty () && m_nID != 0)
	{
		CString strMessage;
		int iOffset;

		if (strMessage.LoadString (m_nID) &&
			(iOffset = strMessage.Find (_T('\n'))) != -1)
		{
			menuButton.m_strText = strMessage.Mid (iOffset + 1);
		}
	}

	return TRUE;
}
//*******************************************************************************
void CBCGToolbarButton::SetProtectedCommands (const CList<UINT, UINT>& lstCmds)
{
	m_lstProtectedCommands.RemoveAll ();
	m_lstProtectedCommands.AddTail ((CList<UINT,UINT>*) &lstCmds);
}
