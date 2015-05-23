#include "StdAfx.h"
#include "Periodic.h"
#include "InfoField.h"
#include "nameconsts.h"
#include "EditorInt.h"
#include "docview5.h"
#include "Comm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void _SetBaseObject(IUnknown *punk, IUnknown *punkBase, IUnknown *punkDocument)
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

CSize CalcSize(LOGFONT lf, LPCTSTR lpszString)
{
	CWindowDC dc(NULL);
	CRect rc(0,0,0,0);
	dc.DrawText(lpszString, -1, &rc, DT_CALCRECT|DT_SINGLELINE);
	rc.right+=10;
	return rc.Size();
}

void AddObject(IUnknown *punkDoc, LPCTSTR lpszString)
{
//	BOOL bMessages = ::GetValueInt(::GetAppUnknown(), "Periodic", "Messages", 1);
	{
		// Find (or create comment object list).	
		IUnknownPtr punkImage(::GetActiveObjectFromDocument(punkDoc, szTypeImage), false);
		int nWidth,nHeight;
		IImage3Ptr sptrImage(punkImage);
		sptrImage->GetSize(&nWidth, &nHeight);
		IUnknownPtr punkList(::GetActiveObjectFromDocument(punkDoc, szDrawingObjectList), false);
		if (punkList == 0)
		{
			//create an object list
			punkList = IUnknownPtr(::CreateTypedObject(_bstr_t(szDrawingObjectList)),false);
			if (punkList == 0)
			{
				AfxMessageBox("Error creating comment objects list");
				return;
			}
			if (punkImage != 0)
				_SetBaseObject(punkList, punkImage, punkDoc);
			variant_t var((IUnknown*)punkList);
			SetValue(punkDoc, NULL, NULL, var);
			_bstr_t	bstr( ::GetObjectKind(punkList));
			IUnknownPtr punkView;
			IDocumentSitePtr ptrDocSite(punkDoc);
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
			sptrICommentObj sptrComment(punkChild);
			if (sptrComment != 0)
			{
				punkObj = punkChild;
				break;
			}
		}
		if (punkObj == 0)
		{
			punkObj = IUnknownPtr(CreateTypedObject(_bstr_t(szDrawingObject)),false);
			INamedDataObject2Ptr sptrNDOCom(punkObj);
			sptrNDOCom->SetParent(punkList,0);
		}
		sptrICommentObj  sptrComment(punkObj);	
		if (sptrComment != 0)
		{
			sptrComment->LockDraw(TRUE);
			LOGFONT	logFont;
			memset(&logFont, 0, sizeof(logFont));
			int nAlign = ::GetValueInt(GetAppUnknown(), "Editor", "CommentText_Align", 0); //0-left; 1-center; 2-right
			OLE_COLOR colorText = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", RGB(0,255,0));
			OLE_COLOR colorBack = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Back", RGB(127,255,127));
			BOOL bBack = ::GetValueInt(GetAppUnknown(), "Periodic", "TransparentInfoField", 1) == 0;
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
			sptrComment->LockDraw(FALSE);
			CSize sz = CalcSize(logFont, lpszString);
			CRect rc(nWidth-sz.cx,nHeight-sz.cy,nWidth,nHeight);
			sptrComment->SetCoordinates(rc, CPoint(-1,-1));
			sptrComment->SetText(_bstr_t(lpszString));
		}
//		variant_t var((IUnknown*)punkObj);
//		SetValue(punkDoc, NULL, NULL, var);
	}
}


LONG s_bRun = false;
CommData g_CommData;
LONG s_bCapture = 0;
CRITICAL_SECTION g_cs;
BOOL CComThread::InitInstance()
{
	CComm Comm;
	Comm.Open();
	if (!Comm.IsOpened())
		return FALSE;
	int nCountFF = 0, nCountData = 0;
	CommData Data;
	while (s_bRun)
	{
		BYTE b;
		if (Comm.Read(1, &b))
		{
			if (nCountFF < 4) // Header with FF does not read
			{
				if (b == 0xFF)
					nCountFF++;
				else
					nCountFF = 0;
			}
			else // read data
			{
				if (nCountData == 0)
					Data.x = (WORD)b;
				else if (nCountData == 1)
					Data.x |= ((WORD)b)<<8;
				else if (nCountData == 2)
					Data.y = (WORD)b;
				else if (nCountData == 3)
					Data.y |= ((WORD)b)<<8;
				else if (nCountData == 4)
					Data.flag = (WORD)b;
				else if (nCountData == 5)
				{
					Data.flag |= ((WORD)b)<<8;
					EnterCriticalSection(&g_cs);
					g_CommData = Data;
					LeaveCriticalSection(&g_cs);
					nCountFF = 0;
					if (Data.flag == 0x9999)
						s_bCapture = 1;
				}
				if (nCountData < 5)
					nCountData++;
				else
					nCountData = 0;
			}
		}
	}
	return FALSE;
}
CComThread *g_pThread;

CString FormatCoordinates()
{
	CommData Data;
	EnterCriticalSection(&g_cs);
	Data = g_CommData;
	LeaveCriticalSection(&g_cs);
	CString s;
	s.Format(_T("%d : %d"), (int)Data.x, (int)Data.y);
	return s;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionReadCoords
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionReadCoords, CCmdTargetEx);

// {0FE387BA-78E5-4DDD-9906-652A465A5EF8}
GUARD_IMPLEMENT_OLECREATE(CActionReadCoords, _T("Periodic.ActionReadCoords"), 
0xfe387ba, 0x78e5, 0x4ddd, 0x99, 0x6, 0x65, 0x2a, 0x46, 0x5a, 0x5e, 0xf8);

// {89EB00C7-D7B1-4F42-862D-F1A95BD1237F}
static const GUID guidActionReadCoords = 
{ 0x89eb00c7, 0xd7b1, 0x4f42, { 0x86, 0x2d, 0xf1, 0xa9, 0x5b, 0xd1, 0x23, 0x7f } };

ACTION_INFO_FULL(CActionReadCoords, ID_ACTION_READCOORDS, -1, -1, guidActionReadCoords);
ACTION_TARGET(CActionReadCoords, szTargetAnydoc);
ACTION_ARG_LIST(CActionReadCoords)
END_ACTION_ARG_LIST();


CActionReadCoords::CActionReadCoords()
{
}

CActionReadCoords::~CActionReadCoords()
{
}

bool CActionReadCoords::Invoke()
{
	CString s;
	bool bOk;
	if (s_bRun)
	{
		s = FormatCoordinates();
		bOk = true;
	}
	else
	{
		CComm Comm;
		Comm.Open();
		if (Comm.IsOpened())
		{
			int nCountFF = 0, nCountData = 0;
			bool bOk = false;
			CommData Data;
			while (1)
			{
				BYTE b;
				if (Comm.Read(1, &b))
				{
					if (nCountFF < 4) // Header with FF does not read
					{
						if (b == 0xFF)
							nCountFF++;
						else
							nCountFF = 0;
					}
					else // read data
					{
						if (nCountData == 0)
							Data.x = (WORD)b;
						else if (nCountData == 1)
							Data.x |= ((WORD)b)<<8;
						else if (nCountData == 2)
							Data.y = (WORD)b;
						else if (nCountData == 3)
							Data.y |= ((WORD)b)<<8;
						else if (nCountData == 4)
							Data.flag = (WORD)b;
						else if (nCountData == 5)
						{
							Data.flag |= ((WORD)b)<<8;
							bOk = true;
							break;
						}
					}
				}
				else
					break;
			}
			if (bOk)
				s.Format(_T("%d : %d"), (int)Data.x, (int)Data.y);
		}
	}
	if (bOk)
	{
		AddObject(m_punkTarget,s);
		return true;
	}
	else
	{
		AfxMessageBox(IDS_DATA_CANNOT_BE_READ, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
}
