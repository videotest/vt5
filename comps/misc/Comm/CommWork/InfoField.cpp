#include "StdAfx.h"
#include "InfoField.h"
#include "misc_utils.h"
#include "nameconsts.h"
#include "EditorInt.h"
#include "docview5.h"
#include "Comm.h"
#include "InfoFieldInt.h"

void SetBaseObject(IUnknown *punk, IUnknown *punkBase, IUnknown *punkDocument)
{
	if( punk == 0 || punkBase == 0 )
		return;
	INamedDataObject2Ptr ptrBase = punkBase;

//only parent object can be base
	IUnknown	*punkParent = 0;
	ptrBase->GetParent( &punkParent );

	if( punkParent )
	{
		ptrBase = punkParent;
		punkParent->Release();
	}

	BOOL bFlag = false;
	ptrBase->IsBaseObject(&bFlag);

	if (bFlag)
	{
		GuidKey BaseKey;
		INamedDataObject2Ptr ptrN = punk;
		if( SUCCEEDED( ptrBase->GetBaseKey(&BaseKey)) && BaseKey != INVALID_KEY )
			ptrN->SetBaseKey(&BaseKey);
	}
}


_bstr_t GetObjectKind( IUnknown *punkObj )
{
	sptrINamedDataObject	sptrO( punkObj );
	BSTR	bstrKind = 0;
	if (sptrO == 0 || FAILED(sptrO->GetType( &bstrKind )) || !bstrKind)
		return "";
	_bstr_t strKind = bstrKind;
	::SysFreeString( bstrKind );
	return strKind;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionClassify
////////////////////////////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CActionInfoFieldInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};


CActionInfoField::CActionInfoField()
{
}

IUnknown *CActionInfoField::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CAction::DoGetInterface( iid );
}


HRESULT CActionInfoField::DoInvoke()
{
	BOOL bMessages = ::GetValueInt(::GetAppUnknown(), "CommWork", "Messages", 1);
	try
	{
		BOOL bReopen = ::GetValueInt(::GetAppUnknown(), "CommWork", "Reopen", 0);;
		if (bReopen && g_Comm.IsOpened())
			g_Comm.Close();
		g_Comm.Open();
		InfoFieldData Info;
		g_Comm.ReadInfoField(Info);
		if (bReopen)
			g_Comm.Close();
		// Find (or create comment object list).	
		IUnknownPtr punkImage(::GetActiveObjectFromContext(m_ptrTarget, szTypeImage), false);
		int nWidth,nHeight;
		IImage3Ptr sptrImage(punkImage);
		sptrImage->GetSize(&nWidth, &nHeight);
		IUnknownPtr punkList(::GetActiveObjectFromDocument(m_ptrTarget, szDrawingObjectList), false);
		if (punkList == 0)
		{
			//create an object list
			punkList = IUnknownPtr(::CreateTypedObject(_bstr_t(szDrawingObjectList)),false);
			if (punkList == 0)
				throw new CMessageException("Error creating comment objects list");
			if (punkImage != 0)
				SetBaseObject(punkList, punkImage, m_ptrTarget);
			SetToDocData(m_ptrTarget, punkList);
			_bstr_t	bstr( ::GetObjectKind(punkList));
			IUnknownPtr punkView;
			IDocumentSitePtr ptrDocSite(m_ptrTarget);
			ptrDocSite->GetActiveView(&punkView);
			if (punkView != 0)
			{
				sptrIDataContext sptrDC(punkView);
				sptrDC->SetActiveObject(bstr, punkList, 0);
			}
		}
		IUnknownPtr punkObj;
		// Find object (if any).
		INamedDataObject2Ptr sptrNDO2(punkList);
		POSITION pos;
		sptrNDO2->GetFirstChildPosition(&pos);
		while (pos)
		{
			IUnknownPtr punkChild;
			sptrNDO2->GetNextChild(&pos, &punkChild);
			IInfoFieldPtr sptrIField(punkChild);
			if (sptrIField != 0)
			{
				punkObj = punkChild;
				break;
			}
		}
		if (punkObj == 0)
			punkObj = IUnknownPtr(CreateTypedObject(_bstr_t(szTypeInfoFieldObject)),false);
		sptrICommentObj  sptrComment(punkObj);	
		if (sptrComment != 0)
		{
			sptrComment->LockDraw(TRUE);
			LOGFONT	logFont;
			memset(&logFont, 0, sizeof(logFont));
			int nAlign = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Align", 0); //0-left; 1-center; 2-right
			OLE_COLOR colorText = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", RGB(0,255,0));
			OLE_COLOR colorBack = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Back", RGB(127,255,127));
			BOOL bBack = ::GetValueInt(GetAppUnknown(), "CommWork", "TransparentInfoField", 1) == 0;
			BOOL bArrow = FALSE;
			_bstr_t strName = ::GetValueString(GetAppUnknown(), "Editor", "CommentText_Name", "Arial");
			logFont.lfHeight = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Height", 12);
			logFont.lfWidth = 0;
			memcpy((char*)logFont.lfFaceName, (const char *)strName, strName.length());
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
			sptrIFontPropPage	sptrFont(sptrComment);
			sptrFont->SetFont(&logFont, nAlign);
			sptrFont->SetColorsAndStuff(colorText, colorBack, bBack, bArrow);
			IInfoFieldPtr sptrIField(sptrComment);
			sptrIField->SetData(Info);
			SIZE sz;
			sptrIField->CalcSize(&sz);
			sptrComment->LockDraw(FALSE);
			sptrComment->SetCoordinates(_rect(nWidth-sz.cx,nHeight-sz.cy,nWidth,nHeight), _point(-1,-1));
		}
		SetToDocData(m_ptrTarget, punkObj);
	}
	catch(CMessageException *e)
	{
		if (bMessages)
			e->ReportError();
		delete e;
		return E_UNEXPECTED;
	}
	return S_OK;
}