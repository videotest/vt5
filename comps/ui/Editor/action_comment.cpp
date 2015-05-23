#include "stdafx.h"
#include "action_comment.h"
#include "EditComment.h"
#include "resource.h"
#include "actions.h"
#include "carioint.h"
#include <math.h>
#include "docmiscint.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionEditComment, CCmdTargetEx);

//olecreate 

// {2454A4F3-92B0-11d3-A52C-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionEditComment, "Editor.EditComment",
0x2454a4f3, 0x92b0, 0x11d3, 0xa5, 0x2c, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// guidinfo 

// {2454A4F1-92B0-11d3-A52C-0000B493A187}
static const GUID guidEditComment =
{ 0x2454a4f1, 0x92b0, 0x11d3, { 0xa5, 0x2c, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };

//[ag]6. action info

ACTION_INFO_FULL(CActionEditComment, ID_ACTION_EDIT_COMMENT, -1, -1, guidEditComment);
//[ag]7. targets

ACTION_TARGET(CActionEditComment, szTargetViewSite);
//[ag]8. arguments

ACTION_ARG_LIST(CActionEditComment)
	ARG_INT(_T("X"), -1)
	ARG_INT(_T("Y"), -1)
	ARG_INT(_T("xArrow"), -1)
	ARG_INT(_T("yArrow"), -1)
	ARG_INT(_T("nCommentID"), -1)
	ARG_STRING(_T("Text"), 0)
END_ACTION_ARG_LIST()

BEGIN_INTERFACE_MAP(CActionEditComment, CInteractiveActionBase)
	INTERFACE_PART(CActionEditComment, IID_IObjectAction, DrAction)
	INTERFACE_PART(CActionEditComment, IID_IScriptNotifyListner, ScriptNotify )
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CActionEditComment, DrAction)
IMPLEMENT_UNKNOWN_BASE(CActionEditComment, ScriptNotify )

HRESULT CActionEditComment::XDrAction::GetFirstObjectPosition(LONG_PTR *plpos)
{
	METHOD_PROLOGUE_EX(CActionEditComment, DrAction);
	(*plpos) = 1;
	return S_OK;
}
HRESULT CActionEditComment::XDrAction::GetNextObject(IUnknown **ppunkObject, LONG_PTR *plpos)
{
	METHOD_PROLOGUE_EX(CActionEditComment, DrAction);
	*ppunkObject = pThis->m_ptrComment;
	if( *ppunkObject )(*ppunkObject)->AddRef();
	(*plpos) = 0;
	return S_OK;
}

HRESULT CActionEditComment::XDrAction::AddObject( IUnknown *punkObject )
{
	METHOD_PROLOGUE_EX(CActionEditComment, DrAction);

	pThis->m_ptrComment = punkObject;
	pThis->m_state = asReady;

	return S_OK;
}






//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionEditComment implementation

//#pragma optimize("", off)

CActionEditComment::CActionEditComment():
	m_ptArrow(-1, -1), m_ptUndo(-1,-1)
{
	m_bPureOutsideComment = false;
	m_bMoveOutside = false;
	m_rcText = NORECT;
	m_rcPrevText= NORECT;
	m_strText = "";
	m_nID = -1;
	m_bUseBackColor = false;
	m_bEditText = false;
	m_bEditPlacement = false;
	m_punkDocument = 0;
	m_bCanTrackByRButton = false;
	m_pEditComment = 0;
	m_bFinalized = true;
	m_bReEditText = false;
	m_objects = 0;
	m_bDrawArrow = true;
	m_bPreventChangeCursor = false;
	m_punkEditedObj = 0;
	m_bEventRegistered = false;

	m_bFromScript = false;
}

bool CActionEditComment::IsRequiredInvokeOnTerminate()
{
	//return false;
//	if(m_bEditText||m_bReEditText)return true; // A.M. fix BT 5092
	//return false;
	return false;
}

CActionEditComment::~CActionEditComment()
{
	if(m_pEditComment) 
	{
		delete m_pEditComment;
		::SetValue(GetAppUnknown(), "Editor", "CommentText_HaveActiveObj", _variant_t((long)0));
	}
	m_pEditComment = 0;
	for(int i = 0; i <= m_pHotPoints.GetUpperBound(); i++)
	{
		delete m_pHotPoints[i];
	}
	m_pHotPoints.RemoveAll();
	/*if(m_objects)
	{
		m_objects->Release();
		m_objects = 0;
	}*/
	if(m_punkEditedObj)
	{
		m_punkEditedObj->Release();
		m_punkEditedObj = 0;
	}

	if (m_punkDocument)
		m_punkDocument->Release();
	m_punkDocument = 0;
}

bool CActionEditComment::DoUndo()
{
	if(m_bEditPlacement)
	{
		_ModifyCurrentObj(m_ptUndo);
	}
	else if(m_bReEditText)
	{
		m_strText = m_strUndo;
		_ModifyCurrentObj(CPoint(0,0));
	}
	else
		m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionEditComment::DoRedo()
{
	if(m_bEditPlacement)
	{
		_ModifyCurrentObj(m_ptRedo);
	}
	else if(m_bReEditText)
	{
		m_strText = m_strRedo;
		_ModifyCurrentObj(CPoint(0,0));
	}
	else
		m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionEditComment::DoUpdateSettings()
{
	
	m_bDrawArrow = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_DrawArrow", 1) == 1;

	COLORREF color;
	color = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", RGB(0,255,0));	
	m_pColors[0] = GetRValue(color);
	m_pColors[1] = GetGValue(color);
	m_pColors[2] = GetBValue(color);
	
	_update_arrow();
	double Angle = ArrAngle*PI/180;
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	CRect rc = m_rcPrevText;
	rc.left = m_ptArrow.x;
	rc.top = m_ptArrow.y;
	rc.NormalizeRect();
	int arrWidth = (int)2*ArrHeight*tan(Angle/2)+10;
	rc.InflateRect(arrWidth,arrWidth,arrWidth,arrWidth);
	rc = ConvertToWindow( m_punkTarget, rc );
	pwnd->InvalidateRect(&rc);
	//pwnd->UpdateWindow();
	_Draw();

	
	sptrICommentObj sptrObj(m_punkEditedObj);
	if(sptrObj == 0)
	{
		IViewPtr sptrV(m_punkTarget);
		IUnknown* punkMF = 0;
		if(sptrV != 0)
			sptrV->GetMultiFrame(&punkMF, FALSE);
		IMultiSelectionPtr sptrMF(punkMF);
		if(punkMF)
			punkMF->Release();

		if(sptrMF != 0)
		{
			DWORD nCount = 0;
			sptrMF->GetObjectsCount(&nCount);
			for(DWORD i = 0; i < nCount; i++)
			{
				IUnknown* punkObj = 0;
				sptrMF->GetObjectByIdx(i, &punkObj);
				sptrObj = punkObj;
				if(punkObj)
					punkObj->Release();
				if(sptrObj != 0)
					break;
			}
		}
	}
	if(sptrObj != 0)
	{
		IUnknown *punkFontPage;
		CString strFontPage;
		strFontPage = c_szCCommentFontPropPage;
		punkFontPage = GetPropertyPageByTitle(strFontPage, 0);
		sptrIFontPropPage	sptrFontPage(punkFontPage);
		punkFontPage->Release();
		if(sptrFontPage)
		{
			LOGFONT	logFont;
			int nAlign;
			BOOL bBack;
			BOOL bArrow;
			OLE_COLOR colorText;
			OLE_COLOR colorBack;
			sptrFontPage->GetColorsAndStuff(&colorText, &colorBack, &bBack, &bArrow);
			sptrFontPage->GetFont(&logFont, &nAlign);
			sptrIFontPropPage	sptrObjFont(sptrObj);
			if(sptrObjFont)
			{
				sptrObjFont->SetFont(&logFont, nAlign);
				sptrObjFont->SetColorsAndStuff(colorText, colorBack, bBack, bArrow);
			}
		}
		sptrObj->UpdateObject();
	}

	
	/*IUnknown* punkList = GetActiveList();
	sptrINamedDataObject2 sptrList = 0;
	if(punkList)
	{
		sptrList = punkList;
		punkList->Release();
	}
	sptrICommentObj sptrObj = 0;
	IUnknown* punk = 0;
	if(sptrList != 0)
	{
		long POS = 0;
		sptrList->GetFirstChildPosition( &POS );
		CRect	rcHot;
		int idx = 0;
		while(POS)
		{
			sptrList->GetNextChild( &POS, &punk );
			if(idx == m_editedObj)
			{
				sptrObj = punk;
				punk->Release();
				if(sptrObj == 0)continue;
				IUnknown *punkFontPage;
				CString strFontPage;
				strFontPage = c_szCCommentFontPropPage;
				punkFontPage = GetPropertyPageByTitle(strFontPage, 0);
				sptrIFontPropPage	sptrFontPage(punkFontPage);
				punkFontPage->Release();
				if(sptrFontPage)
				{
					BOOL bBack;
					BOOL bArrow;
					OLE_COLOR colorText;
					OLE_COLOR colorBack;

					sptrFontPage->GetColorsAndStuff(&colorText, &colorBack, &bBack, &bArrow);

					sptrIFontPropPage	sptrObjFont(sptrObj);
					if(sptrObjFont)
						sptrObjFont->SetColorsAndStuff(colorText, colorBack, bBack, bArrow);
				}
				sptrObj->UpdateObject();
				break;
			}
			else
				punk->Release();
			idx++;
		}
	}*/	
	

	CString strVal;
	strVal = ::GetValueString(GetAppUnknown(), "Editor", "CommentText_WriteText", "");
	if(!strVal.IsEmpty())
	{
		if(m_pEditComment)
		{
			m_pEditComment->SetWindowText(strVal);
			//move caret to the end
			long nLength = strVal.GetLength();
			m_pEditComment->SetSel(nLength, nLength);
		}
	}

	if(!m_pEditComment)return true;
	int nStartSel = 0;
	int nEndSel = 0;
	m_pEditComment->GetSel(nStartSel, nEndSel);
	m_pEditComment->GetWindowText(m_strText);
	if(strVal.IsEmpty())
		_ResetEditControl();

	rc = m_rcText;
	rc = ConvertToWindow( m_punkTarget, rc );
	{
	CWindowDC	dc(0);
	CRect rc2=NORECT;
	CFont* pOldFont = dc.SelectObject(&m_pEditComment->m_fontText);
	dc.DrawText(m_strText.IsEmpty()?"A":m_strText, &rc2, DT_CALCRECT);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);
	//m_rcText.right += 2*tm.tmMaxCharWidth;
	rc.right = rc.left + rc2.Width() + tm.tmMaxCharWidth;
	//rc.bottom = rc.top + tm.tmHeight; //rc2.Height();
	rc.bottom = rc.top + rc2.Height();
	//rc2 = rc;
	//rc2.right -= tm.tmMaxCharWidth;
	//rc2 = ConvertToClient( m_punkTarget, rc2 );
	m_rcText.right = m_rcText.left + rc2.Width();
	m_rcText.bottom = m_rcText.top + rc2.Height();
	}
	m_pEditComment->MoveWindow(&rc);
	m_pEditComment->SetWindowText(m_strText);
	m_pEditComment->SetFocus();

	m_pEditComment->SetSel(nStartSel, nEndSel);
	return true;
}

bool CActionEditComment::Invoke()
{
	//it's not a simple arrow (w/o text) and not simple text (w/o arrow)
 	if(m_strText.IsEmpty() && m_ptArrow == CPoint(-1,-1) && m_ptrComment == 0 )
		return false;

	if( m_bPureOutsideComment )
		return false;

	if (m_punkDocument)
		m_punkDocument->Release();
	sptrIView	sptrV( m_punkTarget );
	sptrV->GetDocument( &m_punkDocument );
	if( !m_punkDocument	)
	{
		SetError( IDS_ERR_NODOCUMENT );
		return false;
	}
	m_lTargetKey = ::GetObjectKey( m_punkDocument );

	
	if(m_bEditPlacement)
	{
		return true; 
	}

	if(m_bReEditText)
	{
			//_ResetPunkEditedObj();
			sptrICommentObj sptrObj(m_punkEditedObj);

			IUnknown *punkFontPage;
			CString strFontPage;
			strFontPage = c_szCCommentFontPropPage;
			punkFontPage = GetPropertyPageByTitle(strFontPage, 0);
			sptrIFontPropPage	sptrFontPage(punkFontPage);
			if(punkFontPage == 0)return false;
			punkFontPage->Release();
			if(sptrFontPage)
			{
				LOGFONT	logFont;
				int nAlign;
				BOOL bBack;
				BOOL bArrow;
				OLE_COLOR colorText;
				OLE_COLOR colorBack;

				sptrIFontPropPage	sptrFont(sptrObj);
				sptrFontPage->GetFont(&logFont, &nAlign);
				sptrFontPage->GetColorsAndStuff(&colorText, &colorBack, &bBack, &bArrow);

				if(sptrFont)
				{
					sptrFont->SetFont(&logFont, nAlign);
					sptrFont->SetColorsAndStuff(colorText, colorBack, bBack, bArrow);
				}

				_ModifyCurrentObj(CPoint(0,0));


				sptrIView sptrV(m_punkTarget);
				IUnknown* punkMF = 0;
				sptrV->GetMultiFrame(&punkMF, false);
				sptrIMultiSelection sptrMF(punkMF);
				if(punkMF)
					punkMF->Release();
				if(sptrMF != 0)
					sptrMF->SelectObject(m_punkEditedObj, false);
			}
		return true;
	}
	_try(CActionEditComment, Invoke)
	{

		IViewDataPtr ptr_view_data( m_punkTarget );
		if( ptr_view_data != 0 )
		{
			IUnknown* punk_draw_list = 0;
			ptr_view_data->GetObject( &punk_draw_list, _bstr_t( szDrawingObjectList ) );
			if( !punk_draw_list )
				return false;

			IUnknown* punk_comment = CreateCommentObject();
			if( !punk_comment )
				return false;

			_fill_comment_properties( punk_comment );

			INamedDataObject2Ptr	ptrN( (IUnknown*)punk_comment );
			ptrN->SetParent( punk_draw_list, 0 );

			//[ay] тут мы ничего не делаем, просто прописываемся на будущее
			//undo/redo
			m_changes.SetToDocData( m_punkDocument, punk_comment );
			
			{
				sptrIView sptrV(m_punkTarget);
				IUnknown* punkMF = 0;
				sptrV->GetMultiFrame(&punkMF, false);
				sptrIMultiSelection sptrMF(punkMF);
				if(punkMF)
					punkMF->Release();
				if(sptrMF != 0)
					sptrMF->SelectObject(punk_comment, false);
			}
			punk_draw_list->Release();	punk_draw_list	= 0;
			punk_comment->Release();	punk_comment	= 0;


			return true;
		}

		/*//re-assign the target key to the document
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &m_punkDocument );

		if( !m_punkDocument	)
		{
			SetError( IDS_ERR_NODOCUMENT );
			return false;
		}

		//m_lTargetKey = ::GetObjectKey( m_punkDocument );
		*/
		//init the m_objects variable

		//if(m_objects)
		//{
			//m_objects->Release();
			//m_objects = 0;
		//}


		IUnknown* punkList = GetActiveList();
		m_objects.Attach(punkList , false );

		if( m_objects == 0 )
		{
			//create an object list
			IUnknown	*punkObjects = ::CreateTypedObject( szDrawingObjectList );
	
			if( !punkObjects )
			{
				SetError( IDS_ERR_CANTCREATEOBJECTLIST );
				if (m_punkDocument)
					m_punkDocument->Release();
				m_punkDocument = 0;
				return false;
			}

			m_objects.Attach( punkObjects, false );

			IUnknown	*punkImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
			if( punkImage )
			{
				SetBaseObject( m_objects, punkImage, m_punkDocument );
				punkImage->Release();
			}

			m_changes.SetToDocData( m_punkDocument, punkObjects );

			_bstr_t	bstr( ::GetObjectKind( punkObjects ) );
			IDataContextPtr	ptrC( m_punkTarget );
			ptrC->SetActiveObject( bstr, punkObjects, 0 );
		}
		ASSERT( m_objects != 0 );
		
		IUnknown	*punk = 0;

		if( m_ptrComment !=0 )
		{
			punk = m_ptrComment;
			punk->AddRef();
			sptrINamedDataObject2	sptrN( punk );
			sptrN->SetParent(m_objects, 0);
		}
		else
		{
			punk = CreateCommentObject();
			_fill_comment_properties( punk );
			ASSERT( punk );

		
			m_ptrComment = punk;
		}

		
		m_changes.SetToDocData( m_punkDocument, punk );
 		punk->Release();
		if (m_punkDocument)
			m_punkDocument->Release();
		m_punkDocument = 0;

		{
			sptrIView sptrV(m_punkTarget);
			IUnknown* punkMF = 0;
			sptrV->GetMultiFrame(&punkMF, false);
			sptrIMultiSelection sptrMF(punkMF);
			if(punkMF)
				punkMF->Release();
			if(sptrMF != 0)
				sptrMF->SelectObject(m_ptrComment, false);
		}
		return true;
	}
	_catch;
	{
		return false;
	}
}

bool CActionEditComment::_fill_comment_properties( IUnknown* punk )
{
	if( CheckInterface( punk, IID_ICommentObj ) )
	{
		sptrICommentObj  sptrObj(punk);
		sptrObj->LockDraw(TRUE);
		ASSERT(sptrObj != 0);
		{
		sptrIDrawObject sptrDraw(sptrObj);
		sptrDraw->SetTargetWindow(m_punkTarget);
		}


		IUnknown *punkFontPage;
		CString strFontPage;
		strFontPage = c_szCCommentFontPropPage;
		punkFontPage = GetPropertyPageByTitle(strFontPage, 0);
		sptrIFontPropPage	sptrFontPage(punkFontPage);
		if(punkFontPage == 0)
		{
			AfxMessageBox( "Comment tool can't be used withut font property page" );
			if (m_punkDocument)
				m_punkDocument->Release();

			m_punkDocument = 0;
			return false;
		}
		punkFontPage->Release();
		LOGFONT	logFont;
		int nAlign;
		BOOL bBack;
		BOOL bArrow;
		OLE_COLOR colorText;
		OLE_COLOR colorBack;

		if(sptrFontPage != 0 && !m_bFromScript)
		{
			sptrFontPage->GetFont(&logFont, &nAlign);
			sptrFontPage->GetColorsAndStuff(&colorText, &colorBack, &bBack, &bArrow);
		}
		else if(m_bFromScript)
		{
			nAlign = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Align", 0); //0-left; 1-center; 2-right
			colorText = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", RGB(0,255,0));
			colorBack = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Back", RGB(127,255,127));
			bBack = ::GetValueInt(GetAppUnknown(), "Editor", "TransparentCommentText", 0) == 0;
			bArrow = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_DrawArrow", 1) == 1;

			CString strName = ::GetValueString(GetAppUnknown(), "Editor", "CommentText_Name", "Arial");
			logFont.lfHeight = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Height", 12);
			logFont.lfWidth = 0;
			memcpy((char*)logFont.lfFaceName, strName.GetBuffer(strName.GetLength()), strName.GetLength());
			strName.ReleaseBuffer();
			logFont.lfEscapement = 0;
			logFont.lfOrientation = 0;
			logFont.lfWeight = (::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Bold", 0) == 1)?FW_BOLD:FW_NORMAL;
			logFont.lfItalic = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Italic", 0) == 1;
			logFont.lfUnderline = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Underline", 0) == 1;
			logFont.lfStrikeOut = FALSE;
			logFont.lfCharSet = DEFAULT_CHARSET;
			byte family = FF_DONTCARE;
			family = family << 4;
			family |= DEFAULT_PITCH;
			logFont.lfPitchAndFamily = family;
			logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
			logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			logFont.lfQuality = DEFAULT_QUALITY;	

		}
		sptrIFontPropPage	sptrFont(sptrObj);
		sptrFont->SetFont(&logFont, nAlign);
		sptrFont->SetColorsAndStuff(colorText, colorBack, bBack, bArrow);


		sptrObj->SetText(_bstr_t(m_strText));
		sptrObj->LockDraw(FALSE);
//		if( m_strText.Find( "\r\n" ) != -1 )
//			m_rcText.OffsetRect( 0, -m_rcText.Height() / 2 );
		sptrObj->SetCoordinates(m_rcText, m_ptArrow);
		sptrObj->GetCoordinates(&m_rcText, &m_ptArrow);
		InvalidateObjectRect();
	}

	return true;
}

void CActionEditComment::InvalidateObjectRect()
{
	CRect rcInvalid = NORECT;
	if(m_ptArrow != CPoint(-1, -1))
	{
		rcInvalid.UnionRect(&m_rcText, CRect(m_ptArrow, m_ptArrow));
		double Angle = ArrAngle*PI/180;
		int arrWidth = (int)2*ArrHeight*tan(Angle/2)+10;
		rcInvalid.InflateRect(arrWidth,arrWidth,arrWidth,arrWidth);
	}
	else
		rcInvalid = m_rcText;

	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	rcInvalid = ConvertToWindow( m_punkTarget, rcInvalid );
	pwnd->InvalidateRect(&rcInvalid);
}

bool CActionEditComment::DoTerminate()		//this virtual called when user terminate the action
											//return true if target window should be repainted
{
	return true;
}

bool CActionEditComment::IsAvaible()
{
	bool	bAvaible = false;
	IUnknown *punk = GetTargetWindow();

	if (CheckInterface( punk, IID_IImageView ))
	{
		IUnknown* punk_img = ::GetActiveObjectFromContext( punk, szTypeImage );
		if( punk_img )
		{
			punk_img->Release();	punk_img = 0;
			bAvaible = true;
		}
	}
	else if (CheckInterface( m_punkTarget, IID_ICarioView ))
		bAvaible = true;

	return bAvaible;
}

bool CActionEditComment::IsChecked()
{
	return false;
}

bool CActionEditComment::DoRButtonDown( CPoint pt )
{
	//pt = m_pointCurrent; //set window coordinates

	m_bUseBackColor = true;
	return true;
}

void CActionEditComment::_ResetPunkEditedObj()
{
	if(m_punkEditedObj)
	{
		m_punkEditedObj->Release();
		m_punkEditedObj = 0;
	}

	IUnknown* punkList = GetActiveList();		
	sptrINamedDataObject2 sptrList( punkList );

	if(punkList)
		punkList->Release();
	else
		return;	
	

	ASSERT(sptrList != 0);
	POSITION POS = 0;
	sptrList->GetFirstChildPosition( &POS );
	int idx = 0;
	while(POS)
	{
		sptrList->GetNextChild( &POS, &m_punkEditedObj );
		if(CheckInterface(m_punkEditedObj, IID_ICommentObj))
		{
			if(idx == m_editedObj)
			{
				break;
			}
			else
			{
				m_punkEditedObj->Release();
				m_punkEditedObj = 0;
			}
			idx++;
		}

		if(m_punkEditedObj)
		{
			m_punkEditedObj->Release();
			m_punkEditedObj = 0;
		}
	}
}

bool CActionEditComment::DoLButtonDown( CPoint pt )
{
	m_bPureOutsideComment = false;
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	
	if(m_bEditText || m_bReEditText)
	{
		if(_IsPtInEditControl(pt))
			return false;
		pt = ConvertToWindow( m_punkTarget, pt );
		Finalize();
		pwnd->PostMessage(WM_LBUTTONDOWN, WPARAM(0), LPARAM(MAKELONG(pt.x, pt.y)));
		return true;
	}
	
	sptrIView sptrV(m_punkTarget);
	IUnknown* punkMF = 0;
	sptrV->GetMultiFrame(&punkMF, false);
	sptrIMultiSelection sptrMF(punkMF);
	if(punkMF)
		punkMF->Release();

	_DeterminateHotPt(pt,  true);
	if(m_editedObj >= 0 && m_punkEditedObj)
	{
		m_bPreventChangeCursor = true;
		//_ResetPunkEditedObj();			
		
		if(sptrMF != 0)
			sptrMF->SelectObject(m_punkEditedObj, false);
		
		sptrICommentObj sptrObj(m_punkEditedObj);
		if(sptrObj != 0)
		{

			IUnknown *punkFontPage;
			CString strFontPage;
			strFontPage = c_szCCommentFontPropPage;
			punkFontPage = GetPropertyPageByTitle(strFontPage, 0);
			sptrIFontPropPage	sptrFontPage(punkFontPage);
			punkFontPage->Release();
			if(sptrFontPage)
			{
				LOGFONT	logFont;
				int nAlign;
				BOOL bBack;
				BOOL bArrow;
				OLE_COLOR colorText;
				OLE_COLOR colorBack;

				sptrIFontPropPage	sptrObjFont(sptrObj);
				sptrObjFont->GetFont(&logFont, &nAlign);
				sptrObjFont->GetColorsAndStuff(&colorText, &colorBack, &bBack, &bArrow);

				sptrFontPage->SetColorsAndStuff(colorText, colorBack, bBack, bArrow);
				sptrFontPage->SetFont(&logFont, nAlign);
				
			}

			if(m_hotType == hptTextEdit)
			{	
				if(sptrMF != 0)
					sptrMF->EmptyFrame();

				_ResetEditControl();
				

				CRect rc = ConvertToWindow( m_punkTarget, m_rcText );
				
				{
					CWindowDC	dc(pwnd);
					CFont* pOldFont = dc.SelectObject(&m_pEditComment->m_fontText);
					TEXTMETRIC tm;
					dc.GetTextMetrics(&tm);
					dc.SelectObject(pOldFont);
					m_rcText.right += 2*tm.tmMaxCharWidth;
					rc.right += tm.tmMaxCharWidth;
				}

		
				m_pEditComment->MoveWindow(&rc);
				
				BSTR bstrText = 0;
				sptrObj->GetText(&bstrText);

				m_strText = "";
				_ModifyCurrentObj(CPoint(0,0));
				m_strText = bstrText;
				m_strUndo = m_strText;
				::SysFreeString(bstrText);

				if( m_strText.GetLength() == 1 && m_strText[0] == ' ' )
					m_pEditComment->SetWindowText("");
				else
					m_pEditComment->SetWindowText(m_strText);

				m_pEditComment->SetFocus();

				m_bReEditText = true;
				pt = ConvertToWindow( m_punkTarget, pt );
				m_pEditComment->SendMessage(WM_LBUTTONDOWN, WPARAM(MK_LBUTTON), LPARAM(MAKELONG(pt.x, pt.y)));
				m_pEditComment->SendMessage(WM_LBUTTONUP, WPARAM(MK_LBUTTON), LPARAM(MAKELONG(pt.x, pt.y)));
				m_pEditComment->PostMessage(EM_SETSEL, WPARAM(-1), LPARAM(0));
				//m_pEditComment->SetSel(-1, 0);//remove selection
			}
		}
	}
	else
	{
		if(sptrMF != 0)
			sptrMF->EmptyFrame();
	}
	return false;
}


bool CActionEditComment::_IsPtInEditControl(CPoint pt)
{
	if(m_bEditText || m_bReEditText)
	{
		CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
		CRect rc = NORECT;
		m_pEditComment->GetWindowRect(&rc);
		pwnd->ScreenToClient(&rc);
		rc = ConvertToClient( m_punkTarget, rc );		
		if(rc.PtInRect(pt))
		{
			return true;
		}
	}
	return false;
}

bool CActionEditComment::CanStartTracking( CPoint pt )
{
	return !_IsPtInEditControl(pt);
}

bool CActionEditComment::DoRButtonUp( CPoint pt )
{
	//pt = m_pointCurrent; //set window coordinates

	//_Begin();
	//m_rcText = CRect(pt, pt);
	//m_bEditText = true;
	return true;
}

bool CActionEditComment::DoLButtonUp( CPoint pt )
{
	//pt = m_pointCurrent; //set window coordinates

 	m_bPreventChangeCursor = false;
	if(m_editedObj >= 0)return false;
	if(m_bEditText)
	{
		return false;
	}
	if(m_bReEditText)
	{
		//if(m_pEditComment)
		//{
		//	CRect rc;
		//	m_pEditComment->GetWindowRect(&rc);
		//	if(rc.PtInRect(pt))
		//	{
				return false;
		//	}
		//}
		//Finalize();
	}
	if(m_bEditPlacement)
	{
		m_bEditPlacement = false;
		return false;
	}

	// [vanek] BT2000:3475 - 13.11.2003
	//if(m_rcTargetImage.PtInRect(pt) == FALSE)
	//	return false;
	{
		//POINT _pt = ConvertToClient( m_punkTarget, m_ptArrow );
		//RECT rc = ConvertToClient( m_punkTarget, CRect(pt, pt) );
		//double fZoom = GetZoom( m_punkTarget );

		POINT _pt = m_ptArrow;
		IScrollZoomSitePtr sptrZ = m_punkTarget;
	
		SIZE sz = {0};
		sptrZ->GetClientSize( &sz );
		
		RECT rc2 = { 0, 0, sz.cx /*/ fZoom*/, sz.cy /*/ fZoom*/ };
		
		if( !::PtInRect( &rc2, _pt ) && !::PtInRect( &rc2, /**((LPPOINT)&rc )*/ pt ) )
		{
			CWnd *pWnd = GetWindowFromUnknown( m_punkTarget );

			if( pWnd )
				::InvalidateRect( pWnd->GetSafeHwnd(), 0, true );

			m_bPureOutsideComment = true;
		}
	}

	_Begin();
	_ResetEditControl();
	m_rcText = CRect(pt, pt);
	_update_arrow();
	//m_ptArrow = CPoint(-1, -1);//pt;

	if( m_bPureOutsideComment )
		return false;
	m_bEditText = true;




	CRect rc = m_rcText;
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	rc = ConvertToWindow( m_punkTarget, rc );
	{
	CWindowDC	dc(pwnd);
	CFont* pOldFont = dc.SelectObject(&m_pEditComment->m_fontText);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_rcText.right += tm.tmMaxCharWidth;
	m_rcText.bottom += tm.tmHeight;
	dc.SelectObject(pOldFont);
	rc.right = rc.left + m_rcText.Width();
	rc.bottom = rc.top + m_rcText.Height();
	m_rcText.right -= tm.tmMaxCharWidth;

	}
	
	m_rcText  = ConvertToClient( m_punkTarget, rc );
//	rc.OffsetRect( 0, -rc.Height() / 2 );
//	m_rcText.OffsetRect( 0, -m_rcText.Height() / 2 );

	m_pEditComment->MoveWindow(&rc, TRUE);
	m_pEditComment->SetFocus();
	pwnd->Invalidate();


	return false;
}

void CActionEditComment::_ResetEditControl()
{
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	if(m_pEditComment)
	{
		delete m_pEditComment;
		::SetValue(GetAppUnknown(), "Editor", "CommentText_HaveActiveObj", _variant_t((long)0));
		m_pEditComment = 0;
	}
	if( m_bPureOutsideComment )
		return;

	IUnknown *punkFontPage;
	CString strFontPage;
	strFontPage = c_szCCommentFontPropPage;
	punkFontPage = GetPropertyPageByTitle(strFontPage, 0);
	sptrIFontPropPage	sptrFontPage(punkFontPage);
	if(punkFontPage)
		punkFontPage->Release();
	if(sptrFontPage != 0)
	{
		LOGFONT	logFont;
		int nAlign;
		BOOL bBack;
		BOOL bArrow;
		OLE_COLOR colorText;
		OLE_COLOR colorBack;

		sptrFontPage->GetFont(&logFont, &nAlign);
		sptrFontPage->GetColorsAndStuff(&colorText, &colorBack, &bBack, &bArrow);

		_variant_t var;
		var = (long)nAlign;
		::SetValue(GetAppUnknown(), "Editor", "EditCommentText_Align", var); //0-left; 1-center; 2-right

		var = (long)((bBack==TRUE)?0:1);
		::SetValue(GetAppUnknown(), "Editor", "EditTransparentCommentText", var);

		::SetValueColor(GetAppUnknown(), "Editor", "EditCommentText_Fore", colorText);	
		::SetValueColor(GetAppUnknown(), "Editor", "EditCommentText_Back", colorBack);	
	

		var = logFont.lfHeight;
		::SetValue(GetAppUnknown(), "Editor", "EditCommentText_Height", var);
		var = (long)((logFont.lfWeight == FW_BOLD)?1:0);
		::SetValue(GetAppUnknown(), "Editor", "EditCommentText_Bold", var);
		var = (long)((logFont.lfUnderline == TRUE)?1:0);
		::SetValue(GetAppUnknown(), "Editor", "EditCommentText_Underline", var);
		var = (long)((logFont.lfItalic == TRUE)?1:0);
		::SetValue(GetAppUnknown(), "Editor", "EditCommentText_Italic", var);
		
		CString strName = logFont.lfFaceName;
		::SetValue(GetAppUnknown(), "Editor", "EditCommentText_Name", _variant_t(strName));
	}


	m_pEditComment = new CEditComment(pwnd, this, GetZoom(m_punkTarget));
	
	m_pEditComment->m_ptLastScrollPos = GetScrollPos(m_punkTarget);

	::SetValue(GetAppUnknown(), "Editor", "CommentText_HaveActiveObj", _variant_t((long)1));
}

bool CActionEditComment::DoStartTracking( CPoint pt )
{
	//pt = m_pointCurrent; //set window coordinates

	//if(m_bEditText || m_bReEditText)return false;

	if(_IsPtInEditControl(pt))return false;

	_DeterminateHotPt(pt, true);
	if(m_editedObj >= 0)
	{
		//need to save state for undo
		//m_ptUndoText = CPoint(m_rcText.left, m_rcText.top);
		//m_ptUndoArrow = m_prArrow;
		m_ptUndo = pt;
		m_bEditPlacement = true;
		//DoSetCursor(CPoint(0,0));
		return false;
	}
	else
		m_bEditPlacement = false;

	_Begin();
	//_ResetEditControl();
	m_ptArrow = pt;
	m_rcText = CRect(pt, pt);
	m_rcPrevText = CRect(pt, pt);
	_Draw();
	return false;
}

void CActionEditComment::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CInteractiveActionBase::OnNotify(pszEvent, punkHit, punkFrom, pdata, cbSize );
	_try(CActionEditComment, OnNotify)
	{
		if( !strcmp( pszEvent, szEventChangeObjectList ) )
		{
			long	lHint=*(long*)pdata;
			if(  lHint == cncRemove )
			{
				if(::GetObjectKey( punkHit ) == ::GetObjectKey( m_punkEditedObj ))
				{
					m_punkEditedObj->Release();
					m_punkEditedObj = 0;
					if(m_pEditComment)
					{
						delete m_pEditComment;
						::SetValue(GetAppUnknown(), "Editor", "CommentText_HaveActiveObj", _variant_t((long)0));
						m_pEditComment = 0;
						m_bEditText = false;
						m_bReEditText = false;
					}
				}
				_SetHotPointsArray(punkHit);
				//Finalize();
			}
		}
		else if( !strcmp( pszEvent, szEventChangeProperties ) )
		{
			CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
			pwnd->Invalidate();
		}
	}
	_catch;
}


void CActionEditComment::_ModifyCurrentObj(CPoint pt)
{
	
	IUnknown* punkList = GetActiveList();
	sptrINamedDataObject2 sptrList(punkList);
	if(punkList)
		punkList->Release();
	sptrICommentObj sptrObj = 0;
	IUnknown* punk = 0;
	if(sptrList == 0)return;
	POSITION POS = 0;
	sptrList->GetFirstChildPosition( &POS );
	CRect	rcText;
	CPoint	ptArrow;
	CRect	rcHot;
	int idx = 0;
	int nWidth = 0;
	int nHeight = 0;
	while(POS)
	{
		sptrList->GetNextChild( &POS, &punk );

		sptrObj = punk;
		punk->Release();
		if(sptrObj == 0)continue;

		CRect rcComment = NORECT;
		{
			CRect	rcText = NORECT;
			CPoint	ptArr = CPoint(0,0);
			sptrObj->GetCoordinates(&rcText, &ptArr );
			if(ptArr == CPoint(-1,-1))
				ptArr = rcText.TopLeft();
			CRect rcArr(ptArr.x-1, ptArr.y-1, ptArr.x+1, ptArr.y+1);
			rcArr.NormalizeRect();
			rcComment.UnionRect(&rcText, &rcArr);
			rcComment.NormalizeRect();
			rcComment.InflateRect( 5, 5, 5, 5 );
			rcComment.right += 3;
			rcComment.bottom += 3;
		}
		CSize szClient;
		bool bMoveOutsize = 1;

		if( IImageViewPtr( m_punkTarget ) )
			bMoveOutsize = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "EnableMoveOutside", 1 ) != 0;

		IScrollZoomSitePtr sprtZ = m_punkTarget;
		sprtZ->GetClientSize( &szClient );

		if(idx == m_editedObj)
		{
			sptrObj->GetCoordinates(&rcText, &ptArrow);
			switch(m_hotType)
			{
			case hptArrowMove:
				//pt  = ConvertToWindow( m_punkTarget, pt );
				//pt.Offset(hotPoint/2, hotPoint/2);
				//pt  = ConvertToClient( m_punkTarget, pt );
				ptArrow = pt;
				sptrObj->SetCoordinates(rcText, ptArrow);
				if(!m_bFinalized)
					m_ptRedo = pt;
				break;
			case hptTextMove:
			{	
				//pt  = ConvertToWindow( m_punkTarget, pt );
				//pt.Offset(hotPoint/2, hotPoint/2);
				//pt  = ConvertToClient( m_punkTarget, pt );
				nWidth = rcText.Width();
				nHeight = rcText.Height();
				
				int	dx = pt.x - rcText.left;
				int dy = pt.y - rcText.top;

				if( m_bMoveOutside )
				{
					dx = pt.x - ptArrow.x;
					dy = pt.y - ptArrow.y;
				}

				
				if( ptArrow != CPoint( -1, -1 ) )
				{
					if (ptArrow.x+dx<0) dx = -ptArrow.x;
					if (ptArrow.y+dy<0) dy = -ptArrow.y;
				}

				if( !bMoveOutsize )
				{
  					if( dx < 0 )
					{
						if( rcComment.left > 0 )
						{
							if( rcComment.left + dx < 0 )
								dx -= rcComment.left + dx;
						}
						else
							dx = 0;
					}

  					if( dy < 0 )
					{
						// [vanek] BT2000: 3525 - copy/paste :)
						if( rcComment.top > 0 )
						{
                            /*if( rcComment.top + dx < 0 )
								dx -= rcComment.top + dx;*/
							if( rcComment.top + dy < 0 )
								dy -= rcComment.top + dy;
						}
						else
							//dx = 0;
							dy = 0;

					}

 					if( dx > 0 )
					{
						if( rcComment.right < szClient.cx )
						{
							if( rcComment.right + dx > szClient.cx )
								dx -= ( rcComment.right + dx ) - szClient.cx;
						}
						else
							dx = 0;
					}

 					if( dy > 0 )
					{
						if( rcComment.bottom < szClient.cy )
						{
							if( rcComment.bottom + dy > szClient.cy )
								dy -= ( rcComment.bottom + dy ) - szClient.cy;
						}
						else
							dy = 0;
					}
				}


				rcText.left += dx;
				rcText.top += dy;
	
				if( ptArrow.x != -1 && ptArrow.y != -1 )
				{
					ptArrow.x += dx;
					ptArrow.y += dy;
				}
				rcText.right = rcText.left + nWidth;
				rcText.bottom = rcText.top + nHeight;
				sptrObj->SetCoordinates(rcText, ptArrow);
				if(!m_bFinalized)
					m_ptRedo = pt;
				//rcText.OffsetRect(pt);
				break;
			}
			case hptTextEdit:
				//nWidth = m_rcText.Width();
				//nHeight = m_rcText.Height();
				//rcText.right = rcText.left + nWidth;
				//rcText.bottom = rcText.top + nHeight;
				sptrObj->SetText(_bstr_t(m_strText));
				{
  					CRect rc = m_rcText;
					CPoint pt;

					sptrObj->GetCoordinates( &rc, &pt );
 					rc = m_rcText;
 					if( m_strText.IsEmpty() )
					{
//						rc.OffsetRect( 0, rc.Height() / 2 );
					}

					sptrObj->SetCoordinates( rc, pt );
				}
				break;
			default:
				return;
			}
				
			return;
		}
		idx++;
	}
}

bool CActionEditComment::DoSetCursor( CPoint point )
{
	//point = m_pointCurrent; //set window coordinates
	/*if(m_bPreventChangeCursor)
	{
		::SetCursor( m_hcurActive );
		return true;
	}*/
	if(/*(!m_bEditPlacement)  && (!m_bPreventChangeCursor)*/true)
	{
		hotPointType hotType = (hotPointType)_DeterminateHotPt(point, false);
		if(hotType > 0)
		{
			if(hotType == hptTextEdit)
				m_hcurActive = LoadCursor(NULL, IDC_IBEAM);
			else
				m_hcurActive = AfxGetApp()->LoadCursor(IDC_MOVE);
		}
		else
			m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDIT_TEXT);
	}
	else
	{
		m_hcurActive = AfxGetApp()->LoadCursor(IDC_ED_EDIT_TEXT);
		//return true;
	}

	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );

	/*//TRACE("DoSetCursor, point=(%d, %d)\n", point.x, point.y);
	if(!m_bEditPlacement)// && !m_bReEditText)
		int Obj = _DeterminateHotPt(point, m_hotType);
	if(m_editedObj >= 0)
	{
		if(m_hotType == hptTextEdit)
			::SetCursor( LoadCursor(NULL, IDC_IBEAM) );
		else
			::SetCursor( AfxGetApp()->LoadCursor(IDC_MOVE) );
		return true;
	}
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
	*/
}

void CActionEditComment::_OnTrack(CPoint pt)
{
	m_rcText = CRect(pt, pt);
 	double Angle = ArrAngle*PI/180;
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	CRect rc = m_rcPrevText;
	rc.left = m_ptArrow.x;
	rc.top = m_ptArrow.y;
	rc.NormalizeRect();
	//int arrWidth = (int)2*ArrHeight*tan(Angle/2)*GetZoom(m_punkTarget);
	int arrWidth = (int)2*ArrHeight*tan(Angle/2);
	
	rc.InflateRect(arrWidth,arrWidth,arrWidth,arrWidth);
	rc = ConvertToWindow( m_punkTarget, rc );
	pwnd->InvalidateRect(&rc);
	pwnd->UpdateWindow();
	_Draw();
}

bool CActionEditComment::DoTrack( CPoint pt )
{
	//pt = m_pointCurrent; //set window coordinates

	if(m_bEditText || m_bReEditText)return false;
	if(m_bEditPlacement)
	{
		_ModifyCurrentObj(pt);
		return false;
	}
	_OnTrack(pt);
	return false;
}

void CActionEditComment::_update_arrow()
{
	double x = m_rcText.left - m_ptArrow.x;
	double y = m_rcText.top - m_ptArrow.y;
	double fLen = _hypot( x, y );

	if(fLen<0.5)
	{
		x += 1.0;
		fLen = _hypot( x, y );
	}

	double fMinLen = GetValueInt( ::GetAppUnknown(), "Editor", "MinCommentWidth", 20 ) ;
	if( fLen < fMinLen )
	{
		x /= fLen;
		y /= fLen;

		x *= fMinLen;
		y *= fMinLen;

		m_rcPrevText.OffsetRect( m_ptArrow.x - m_rcText.left, m_ptArrow.y - m_rcText.top );
		m_rcPrevText.OffsetRect( (int)( x + 0.5 ), (int)( y + 0.5 ) );

		m_rcText.OffsetRect( m_ptArrow.x - m_rcText.left, m_ptArrow.y - m_rcText.top );
		m_rcText.OffsetRect( (int)( x + 0.5 ), (int)( y + 0.5 ) );
	}
}

bool CActionEditComment::DoFinishTracking( CPoint pt )
{
	//pt = m_pointCurrent; //set window coordinates

	if(m_bEditText || m_bReEditText)return false;
	if(m_bEditPlacement)
	{
		//m_bEditPlacement = false;
		Finalize();
		return false;
	}
	_OnTrack(pt);

	_ResetEditControl();

	m_rcText = CRect(pt, pt);

	_update_arrow();

	_Draw();
	//m_bEditText = true;
	CRect rc = m_rcText;
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	rc = ConvertToWindow( m_punkTarget, rc );
	{
	CWindowDC	dc(pwnd);
	CFont* pOldFont = dc.SelectObject(&m_pEditComment->m_fontText);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_rcText.right += tm.tmMaxCharWidth;
	m_rcText.bottom += tm.tmHeight;
	dc.SelectObject(pOldFont);
	}
	rc.right = rc.left + m_rcText.Width();
	rc.bottom = rc.top + m_rcText.Height();

	m_rcText  = ConvertToClient( m_punkTarget, rc );
//	rc.OffsetRect( 0, -rc.Height() / 2 );
//	m_rcText.OffsetRect( 0, -m_rcText.Height() / 2 );

	_ResetEditControl();

	m_pEditComment->MoveWindow(&rc, TRUE);
	m_pEditComment->SetFocus();
	return false;
}

void CActionEditComment::_SetHotPointsArray(IUnknown* punkExceptThis)
{
	_KillHotPointsArray();
	//fill array
	IUnknown* punkList = GetActiveList();
	sptrINamedDataObject2 sptrList(punkList);
	if(punkList)
		punkList->Release();
	sptrICommentObj sptrObj = 0;
	IUnknown* punk = 0;
	if(sptrList == 0)return;
	POSITION POS = 0;
	sptrList->GetFirstChildPosition( &POS );
	CRect	rcText;
	CPoint	ptArrow;

	CRect	rcHot;
	while(POS)
	{
		sptrList->GetNextChild( &POS, &punk );
		sptrObj = punk;
		punk->Release();
		if(sptrObj == 0 || (punkExceptThis && GetObjectKey(punkExceptThis) == GetObjectKey(sptrObj)))continue;
		sptrObj->GetCoordinates(&rcText, &ptArrow);
		CRect	rcTextTmp  = rcText;
		CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
		
		ptArrow = ConvertToWindow( m_punkTarget, ptArrow );
		rcText = ConvertToWindow( m_punkTarget, rcText );

		CRect* rc;

		if(ptArrow == CPoint(-1, -1))
			rc = new CRect(NORECT);
		else
			rc = new CRect(ptArrow.x-hotPoint/2, ptArrow.y-hotPoint/2, ptArrow.x+hotPoint, ptArrow.y+hotPoint);
		*rc = ConvertToClient( m_punkTarget, *rc );
		m_pHotPoints.Add(rc);
		rc = new CRect(rcText.left-hotPoint/2, rcText.top-hotPoint/2, rcText.left+hotPoint, rcText.top+hotPoint);
		*rc = ConvertToClient( m_punkTarget, *rc );
		m_pHotPoints.Add(rc);

		rc = new CRect(rcTextTmp);
		//*rc = ConvertToClient( m_punkTarget, *rc );
		m_pHotPoints.Add(rc);
		sptrObj->SetEditMode(TRUE);
	}

}

void CActionEditComment::_KillHotPointsArray()
{
	//clear array
	for(int i = 0; i <= m_pHotPoints.GetUpperBound(); i++)
	{
		delete m_pHotPoints[i];
	}
	m_pHotPoints.RemoveAll();

	IUnknown* punkList = GetActiveList();
	sptrINamedDataObject2 sptrList(punkList);
	if(punkList)
		punkList->Release();
	sptrICommentObj sptrObj = 0;
	IUnknown* punk = 0;
	if(sptrList == 0)return;
	POSITION POS = 0;
	sptrList->GetFirstChildPosition( &POS );
	while(POS)
	{
		sptrList->GetNextChild( &POS, &punk );
		sptrObj = punk;
		punk->Release();
		if(sptrObj == 0)continue;
		sptrObj->SetEditMode(FALSE);
	}
}

int CActionEditComment::_DeterminateHotPt(CPoint pt, bool bSetParams)
{
	if(m_bEditPlacement)return 0;
	bool bFound = false;
	m_bMoveOutside = false;
	for(int i = 0; i <= m_pHotPoints.GetUpperBound(); i++)
	{
		CRect rc = *m_pHotPoints[i];
		rc.InflateRect(0,0,1,1);
		if(rc.PtInRect(pt))
//		if(m_pHotPoints[i]->PtInRect(pt)) 
		{
			bFound = true;
			break;
		}
	}
	int nObjNum = -1;
	hotPointType hotType = (hotPointType)0;

	if(bFound)
	{
		if(*m_pHotPoints[i] == NORECT)
			i++;
		nObjNum = i / hotPointTypeCount;
		switch(i % hotPointTypeCount)
		{
		case 0:
			{
				IScrollZoomSitePtr sptrZ = m_punkTarget;

				CSize sz;
				sptrZ->GetClientSize( &sz );
				CRect rc( 0, 0, sz.cx, sz.cy );
				rc = ::ConvertToWindow( m_punkTarget, rc );
				CPoint pt2 = (*m_pHotPoints[i+1]).CenterPoint();
				pt2 = ::ConvertToWindow( m_punkTarget, pt2 );
				if( rc.PtInRect( pt2 ) )
					hotType = hptArrowMove;
				else
				{
					hotType = hptTextMove;
					m_bMoveOutside = true;
				}
				break;
			}
		case 1:
			hotType = hptTextMove;
			break;
		case 2:
			hotType = hptTextEdit;
			if(bSetParams)
				m_rcText = *m_pHotPoints[i];
			break;
		}
	}

	if(bSetParams)
	{
		m_editedObj = nObjNum;
		m_hotType = hotType; 
		if(m_editedObj >= 0)
			_ResetPunkEditedObj();			
	}

	return hotType;
}


bool CActionEditComment::Initialize()
{
	IViewDataPtr ptr_view_data( m_punkTarget );
	if( ptr_view_data != 0 )
	{
		IUnknown* punk_draw_list = 0;
		HRESULT hr = ptr_view_data->GetObject( &punk_draw_list, _bstr_t( szDrawingObjectList ) );
		if( punk_draw_list )punk_draw_list->Release();
		if( hr == S_FALSE )return false;
	}

	AddPropertyPageDescription( c_szCCommentFontPropPage );
	AddPropertyPageDescription( c_szCTextPropPage );

	if(!m_bFinalized) return false;
	m_bFinalized = false;
	m_bEditText = false;
	m_bEditPlacement = false;
	m_bReEditText = false;
	m_bCancel = m_bFromScript = false;
	_SetHotPointsArray();
	m_strText = GetArgumentString( _T("Text") );
	m_rcText.left = GetArgumentInt( _T("X") );
	m_rcText.top = GetArgumentInt( _T("Y") );
	m_ptArrow.x = GetArgumentInt( _T("xArrow") );
	m_ptArrow.y = GetArgumentInt( _T("yArrow") );
	m_nID = GetArgumentInt( _T("nCommentID") );
	m_rcPrevText = m_rcText;
	if( !CInteractiveActionBase::Initialize() )return false;
	if(m_rcText.left >=0 && m_rcText.top >= 0)
	{
		m_bFromScript = true;
		Finalize();
		return false;
	}

	//IImageViewPtr sptrIV(m_punkTarget);
	m_rcTargetImage = NORECT;
	
	/*if(sptrIV != 0)
	{
		IUnknown* punkImage = 0;
		sptrIV->GetActiveImage(&punkImage, 0);
		CImageWrp image(punkImage);
		if(punkImage)
			punkImage->Release();
		m_rcTargetImage = image.GetRect();
	}
	else*///[paul] 12.03.2002
	{
		IScrollZoomSitePtr ptrSZS( m_punkTarget );
 		if( ptrSZS )
		{
			CSize size;
			ptrSZS->GetClientSize( &size );
			m_rcTargetImage = CRect( 0, 0, size.cx, size.cy );

			{
				RECT rc = {0};
				ptrSZS->GetVisibleRect( &rc );
				m_rcTargetImage.UnionRect( &m_rcTargetImage, &rc );
			}
		}
	}

//	LockMovement(true, m_rcTargetImage);

	ASSERT(CheckInterface(m_punkTarget, IID_IView));
	IUnknown* punkDoc = 0;
	sptrIView sptrV(m_punkTarget);
	if(sptrV !=0)
	{
		sptrV->GetDocument(&punkDoc);
		Register( punkDoc );
		if(punkDoc)
			punkDoc->Release();
		m_bEventRegistered = true;
	}
	return true;
}

void CActionEditComment::Finalize()
{
	if(m_bEventRegistered)
	{
		ASSERT(CheckInterface(m_punkTarget, IID_IView));
		IUnknown* punkDoc = 0;
		sptrIView sptrV(m_punkTarget);
		if(sptrV !=0)
		{
			sptrV->GetDocument(&punkDoc);
			UnRegister( punkDoc );
			if(punkDoc)
				punkDoc->Release();
			m_bEventRegistered = false;
		}
	}


	if(m_bReEditText)
	{
		CString str;
		m_pEditComment->GetWindowText(str);

		if( m_bCancel )
			str = m_strUndo;

		if(!str.IsEmpty())
			m_strText = str;
		else
			m_strText = " ";

		//_ModifyCurrentObj(CPoint(0,0));it's go to invoke
		m_strRedo = m_strText;
		//CRect rect = NORECT;
		//m_pEditComment->GetWindowRect(&rect);
		//m_rcText.right = m_rcText.left + rect.Width();
		//m_rcText.bottom = m_rcText.top + rect.Height();
	}
	m_bFinalized = true;
	//hide all hot areas
	if(m_pEditComment)
	{
	CString str;
	m_pEditComment->GetWindowText(str);

	if( m_bCancel )
		str = m_strUndo;

	if(!str.IsEmpty())
		m_strText = str;
	CRect rc = NORECT;
	m_pEditComment->GetWindowRect(&rc);

	//CSize	size(rc.Width(), rc.Height());
	//size = ConvertToClient( m_punkTarget, size );

	//m_rcText.right = m_rcText.left + rc.Width();
	//m_rcText.bottom = m_rcText.top + rc.Height();
	//m_rcText.right = m_rcText.left + size.cx;
	//m_rcText.bottom = m_rcText.top + size.cy;

	/*CRect	rc1 = m_rcText;
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	rc1 = ConvertToWindow( m_punkTarget, m_rcText );
	rc1.right = rc1.left + rc.Width();
	rc1.bottom = rc1.top + rc.Height();
	m_rcText = ConvertToClient( m_punkTarget, rc1 );*/

//	m_rcText.top = rc.top; // Max
	CRect	rc1(0,0,rc.Width()-3,rc.Height());
	rc1 = ConvertToClient( m_punkTarget, rc1 );
	m_rcText.right = m_rcText.left + rc1.Width();
	m_rcText.bottom = m_rcText.top + rc1.Height();



	//if(m_pEditComment) 
	//{
		//m_pEditComment->GetWindowRect(&rc);
		//pwnd->ScreenToClient(&rc);
		//pwnd->InvalidateRect(&rc);
		//pwnd->Invalidate();
		//pwnd->UpdateWindow();
		delete m_pEditComment;
	}
	m_bCancel = false;
	m_pEditComment = 0;
	::SetValue(GetAppUnknown(), "Editor", "CommentText_HaveActiveObj", _variant_t((long)0));
	_KillHotPointsArray();
	_End();
	CInteractiveActionBase::Finalize();
	//m_bEditPlacement = false;
	//m_bReEditText = false;
}
	
void CActionEditComment::DoDraw( CDC &dc )
{
	_try(CActionEditComment, DoDraw)
	{
		//draw hot points
		if(m_pEditComment)
		{
			CPoint ptCurPos = GetScrollPos(m_punkTarget); 
			if(m_pEditComment->m_ptLastScrollPos != ptCurPos)
			{	
				//move editor
				/*CRect rcEdit = NORECT;
				m_pEditComment->GetWindowRect(&rcEdit);
				CSize sizeOffset = m_pEditComment->m_ptLastScrollPos - ptCurPos;
				rcEdit.OffsetRect(sizeOffset);
				m_pEditComment->MoveWindow(rcEdit);
				m_pEditComment->m_ptLastScrollPos = ptCurPos;*/
				Finalize();
				return;
			}
		}

		if(m_ptArrow == CPoint(-1,-1))
		{
			return;
		}

		double Angle = ArrAngle*PI/180;

		CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
		
		CRect rc = m_rcPrevText;
		rc.left = m_ptArrow.x;
		rc.top = m_ptArrow.y;
		rc.NormalizeRect();
		int arrWidth = (int)2*ArrHeight*tan(Angle/2);
		rc.InflateRect(arrWidth,arrWidth,arrWidth,arrWidth);
		rc = ConvertToWindow( m_punkTarget, rc );
		
		
		CPen pen(PS_SOLID, 1, RGB(m_pColors[0], m_pColors[1], m_pColors[2]));
		CBrush brush(RGB(m_pColors[0], m_pColors[1], m_pColors[2]));
		CPen* pOldPen = dc.SelectObject(&pen);
		CBrush* pOldBrush = dc.SelectObject(&brush);
		dc.SetROP2(R2_COPYPEN);
		CPoint ptArrow = m_ptArrow;
		ptArrow = ConvertToWindow( m_punkTarget, ptArrow );
		rc = m_rcText;
 		rc = ConvertToWindow( m_punkTarget, rc );
//		rc.OffsetRect( 0, rc.Height() / 2 );
		dc.MoveTo(ptArrow);
		dc.LineTo(rc.left, rc.top);
		//dc.LineTo(rc.CenterPoint());


		long X = ptArrow.x;
		long Y = ptArrow.y;
		long Xo = rc.left;
		long Yo = rc.top;
		double length = sqrt(pow(double(X-Xo),2)+pow(double(Y-Yo), 2));
		if(m_bDrawArrow && (ArrHeight <= length))
		{	
		
		//double ACOS = acos((X-Xo)/length);
		//double ASIN = asin((Y-Yo)/length);
		if(X != Xo || Y != Yo)
		{
			//double ATAN;
			//ATAN = Y==Yo?pi/2:atan((X-Xo)/(Y-Yo));
			double dZoom = GetZoom(m_punkTarget); 
			double dArrHeight = min(ArrHeight*dZoom,length);
			double ATAN = atan2(double(X-Xo), double(Y-Yo));
			POINT points[3];
			points[0].x = X;
			points[0].y = Y;
			points[1].x = X - (X-Xo)*dArrHeight/length + dArrHeight*tan(Angle/2)*cos(ATAN);
			points[1].y = Y - (Y-Yo)*dArrHeight/length - dArrHeight*tan(Angle/2)*sin(ATAN);
			points[2].x = X - (X-Xo)*dArrHeight/length - dArrHeight*tan(Angle/2)*cos(ATAN);
			points[2].y = Y - (Y-Yo)*dArrHeight/length + dArrHeight*tan(Angle/2)*sin(ATAN);
			dc.Polygon(points, 3);
		}
		}

		m_rcPrevText = m_rcText;
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}
	_catch;
}

bool CActionEditComment::_Begin()
{
	_try(CActionEditComment, _Begin)
	{
		//get colors
		//if(m_bUseBackColor)
		{
			COLORREF color;
			color = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", RGB(0,255,0));	
			m_pColors[0] = GetRValue(color);
			m_pColors[1] = GetGValue(color);
			m_pColors[2] = GetBValue(color);
		}
		/*else
		{
			m_pColors[0] = (byte)::GetValueInt(GetAppUnknown(), "Editor", "R", 0);
			m_pColors[1] = (byte)::GetValueInt(GetAppUnknown(), "Editor", "G", 0);
			m_pColors[2] = (byte)::GetValueInt(GetAppUnknown(), "Editor", "B", 0);
		}*/
		
		m_bDrawArrow = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_DrawArrow", 1) == 1;
		return true;
	}
	_catch;
	{
		return false;
	}
}

void CActionEditComment::_End()
{
	SetArgument( _T("Text"), _variant_t( m_strText ) );
	SetArgument( _T("X"), _variant_t( m_rcText.left ) );
	SetArgument( _T("Y"), _variant_t( m_rcText.top ) );
	SetArgument( _T("xArrow"), _variant_t( m_ptArrow.x ) );
	SetArgument( _T("yArrow"), _variant_t( m_ptArrow.y ) );
	SetArgument( _T("nCommentID"), _variant_t( m_nID ) );
}

IUnknown *CActionEditComment::CreateCommentObject()
{
	IUnknown	*punk = CreateTypedObject( szDrawingObject );
	ASSERT( punk );

	//sptrINamedDataObject2	sptrN( punk );
	//sptrN->SetParent( m_objects, 0 );

	//m_listAddedObjects.AddTail( punk );

	return punk;
}

void CActionEditComment::DeleteCommentObject( IUnknown *punkObject )
{
	m_changes.RemoveFromDocData( m_punkDocument, punkObject );
}

IUnknown	*CActionEditComment::GetActiveList()	//return the active object list
{
	IViewDataPtr ptr_view_data( m_punkTarget );
	if( ptr_view_data != 0 )
	{
		IUnknown* punk_draw_list = 0;
		ptr_view_data->GetObject( &punk_draw_list, _bstr_t( szDrawingObjectList ) );
		return punk_draw_list;
	}

	if( CheckInterface( m_punkTarget, IID_IDocument ) )
		return ::GetActiveObjectFromDocument( m_punkTarget, szDrawingObjectList );
	else
		return ::GetActiveObjectFromContext( m_punkTarget, szDrawingObjectList );
}

HRESULT  CActionEditComment::XScriptNotify::ScriptNotify( BSTR bstrNotifyDesc, VARIANT varValue )
{
	METHOD_PROLOGUE_BASE( CActionEditComment, ScriptNotify );

	CString strNotifyDesc = bstrNotifyDesc;

	if( strNotifyDesc == "ActionComment_PropertiesChanged" )
		pThis->DoUpdateSettings();

	return S_OK;
}

