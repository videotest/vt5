#include "stdafx.h"
#include "data.h"
#include "actions.h"
#include "resource.h"
#include "dialogs.h"
#include "ContentDlg.h"
#include "dbase.h"
#include "undoint.h"
#include "docview5.h"
#include "binimageint.h"
#include "ImageSignature.h"
#include "SendToManager.h"

#include "notifyspec.h"
#include "docmiscint.h"

/*BOOL IsGoodExt( CString &strExt )
{

	if( strExt.IsEmpty() )
		return true;


	BOOL bUseFilterTemplate = ::GetValueInt( GetAppUnknown(), "\\FileOpenDialog", "UseTemplate", 0 );
	
	if( bUseFilterTemplate )
	{
		CString strFilterTemplate = ::GetValueString( GetAppUnknown(), "\\FileOpenDialog", "FilterTemplate", "" );

		if( strFilterTemplate.IsEmpty() )
			return true;

		if( strExt.Find( "\n", 0 ) == -1 )
		{
			if( strFilterTemplate.Find( strExt.Right( strExt.GetLength() - 1 ), 0 ) == - 1 )
				return false;
		}
		else
		{
			CString ResultStr = "";
			int nSz = strExt.GetLength();
			int nBrID = -1;
			for(int i = 0; i < nSz;i++ )
			{
				if( strExt[i] == '\n' )
				{
					CString Res;
					
					if( nBrID >= 0 )
					{
						Res = strExt.Right( nBrID );
						Res = Res.Left( i - nBrID );
					}
					else
						Res = strExt.Left( i );
					
					nBrID = i;

					if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
					{
						if( !ResultStr.IsEmpty() )
							ResultStr += "\n" + Res;
						else
							ResultStr = Res;
					}
				}
			}

			{
				CString Res;
				
				if( nBrID >= 0 )
					Res = strExt.Right( strExt.GetLength() - nBrID - 1 );
				else
					Res = strExt.Left( i );
				
				nBrID = i;

				if( strFilterTemplate.Find( Res.Right( Res.GetLength() - 1 ), 0 ) != - 1 )
				{
					if( !ResultStr.IsEmpty() )
						ResultStr += "\n" + Res;
					else
						ResultStr = Res;
				}
			}


			if( ResultStr.IsEmpty() )
				return false;

			strExt = ResultStr;

		}
	}

	return true;

}*/

bool CorrectFileName( char *szFileName )
{
	int	c = 0;
	bool	bChanged = false;
	char	szInvalidChars[] = "|/;\":\\`?*.";

	for( int i = 0; szFileName[i]; i++ )
	{
		bool	bFound = false;
		for( int j = 0; szInvalidChars[j]; j++ )
			if( szFileName[i] == szInvalidChars[j] )
				bFound = true;
		if( bFound )
			bChanged = true;
		else
			szFileName[c++]=szFileName[i];
	}
	szFileName[c] = 0;
	return bChanged;
}

static void __RedrawAllViews(IUnknown *punkTarget)
{
	sptrIDocumentSite	sptrS(punkTarget);
	TPOS lViewPos;
	sptrS->GetFirstViewPosition(&lViewPos);
	while (lViewPos)
	{
		IUnknownPtr punkView;
		sptrS->GetNextView(&punkView, &lViewPos);
		IWindowPtr sptrWnd(punkView);
		if (sptrWnd != 0)
		{
			HWND hwnd = NULL;
			if (sptrWnd->GetHandle((HANDLE *)&hwnd) == S_OK && hwnd != NULL)
				::InvalidateRect(hwnd, NULL, TRUE);
		}
	}
}


/*
qq="Geterk"
ActionManager.ObjectSave qq, "c:\"+qq+".bmp"
*/

#define szObjectFileOpen "FileOpenObject"
//append file suffix helper
static void _AppendFileSuffix(CString &strFilter, LPCTSTR szName, LPCTSTR szExt);
//append all avaible extension for given document template, including file filters
static void AppendFilterSuffix(CString &strFilter, OPENFILENAME *pofn, CString *pstrDefault, CCompManager * pmanFilters, bool bOpenFile, CString strTemplateName);//, IDocTemplate* punkTemplate);

bool PromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, IUnknown *punkFilter = 0);
bool PromtSaveFileName( const char *pszType, const char *pszObjectName, CString &strReturnPath );

IUnknown * GetContextViewer(bool bAddRef = true);

//bool PromptFileName(CString& fileName, LPCTSTR szTitle, DWORD lFlags, BOOL bOpenFileDialog, IUnknown *punkFilter = 0);
// for multiple selection
//bool PromptFileName(CStringArray& arrfileNames, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, IUnknown *punkFilter = 0);
//bool PromptFileName(CStringArray& arrfileNames, LPCTSTR szTitle, DWORD lFlags, BOOL bOpenFileDialog, IUnknown *punkFilter = 0);

IMPLEMENT_DYNCREATE(CActionShowContext,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectUnselectAll, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDataShowContent, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDeleteObject,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectDrop,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectDrag,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditCopy,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEditCut,			CActionEditCopy);
IMPLEMENT_DYNCREATE(CActionEditPaste,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDataMoveToDoc,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDataMoveToApp,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDataOrganizer,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectSave,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectLoad,		CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectProperties, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectRename,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectDelete,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectSelect,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionObjectCreate,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCreateImageObjectByView,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionClipboardAvail,	CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionTouchObject,	CCmdTargetEx);

// {6F95376A-2E96-4b17-A539-DB686DC1EDFB}
GUARD_IMPLEMENT_OLECREATE(CActionShowContext, "Data.ShowContext",
0x6f95376a, 0x2e96, 0x4b17, 0xa5, 0x39, 0xdb, 0x68, 0x6d, 0xc1, 0xed, 0xfb);
// {EBB67684-89EB-48bf-993F-E08216594BCF}
GUARD_IMPLEMENT_OLECREATE(CActionObjectUnselectAll, "Data.ObjectUnselectAll",
0xebb67684, 0x89eb, 0x48bf, 0x99, 0x3f, 0xe0, 0x82, 0x16, 0x59, 0x4b, 0xcf);
// {122029A3-B7E6-40b2-A48E-63E3F64C9AC3}
GUARD_IMPLEMENT_OLECREATE(CActionDataShowContent, "data.DataShowContent",
0x122029a3, 0xb7e6, 0x40b2, 0xa4, 0x8e, 0x63, 0xe3, 0xf6, 0x4c, 0x9a, 0xc3);
// {45C90AF3-D32B-11d3-A090-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CActionDeleteObject, "data.DeleteObject",
0x45c90af3, 0xd32b, 0x11d3, 0xa0, 0x90, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);
// {A624B387-7CA1-11d3-A66A-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectDrop, "Data.ObjectDrop",
0xa624b387, 0x7ca1, 0x11d3, 0xa6, 0x6a, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {A624B383-7CA1-11d3-A66A-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectDrag, "Data.ObjectDrag",
0xa624b383, 0x7ca1, 0x11d3, 0xa6, 0x6a, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {078653AF-6F1B-11d3-A659-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionEditCopy, "Data.EditCopy",
0x78653af, 0x6f1b, 0x11d3, 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {078653AB-6F1B-11d3-A659-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionEditCut, "Data.EditCut",
0x78653ab, 0x6f1b, 0x11d3, 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {078653A7-6F1B-11d3-A659-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionEditPaste, "Data.EditPaste",
0x78653a7, 0x6f1b, 0x11d3, 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {9A43318F-6A8F-11d3-A650-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionDataMoveToDoc, "Data.DataMoveToDoc",
0x9a43318f, 0x6a8f, 0x11d3, 0xa6, 0x50, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {9A43318B-6A8F-11d3-A650-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionDataMoveToApp, "Data.DataMoveToApp",
0x9a43318b, 0x6a8f, 0x11d3, 0xa6, 0x50, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {9A433187-6A8F-11d3-A650-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionDataOrganizer, "Data.DataOrganizer",
0x9a433187, 0x6a8f, 0x11d3, 0xa6, 0x50, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {ACB48D77-5AE9-11d3-8817-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionObjectSave, "Data.ObjectSave",
0xacb48d77, 0x5ae9, 0x11d3, 0x88, 0x17, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {ACB48D73-5AE9-11d3-8817-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionObjectLoad, "Data.ObjectLoad",
0xacb48d73, 0x5ae9, 0x11d3, 0x88, 0x17, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {6C0FB617-5ABC-11d3-8817-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionObjectProperties, "Data.ObjectProperties",
0x6c0fb617, 0x5abc, 0x11d3, 0x88, 0x17, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {2B19CE73-53D8-11d3-880F-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionObjectRename, "Data.ObjectRename",
0x2b19ce73, 0x53d8, 0x11d3, 0x88, 0xf, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {3F65E40B-4037-11d3-A60C-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectDelete, "Data.ObjectDelete",
0x3f65e40b, 0x4037, 0x11d3, 0xa6, 0xc, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {3F65E407-4037-11d3-A60C-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectSelect, "Data.ObjectSelect",
0x3f65e407, 0x4037, 0x11d3, 0xa6, 0xc, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {3F65E403-4037-11d3-A60C-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionObjectCreate, "Data.ObjectCreate",
0x3f65e403, 0x4037, 0x11d3, 0xa6, 0xc, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

// {C46140E1-F907-4165-9244-481B8732C33C}
GUARD_IMPLEMENT_OLECREATE(CActionCreateImageObjectByView, "Data.CreateImageByView",
0xc46140e1, 0xf907, 0x4165, 0x92, 0x44, 0x48, 0x1b, 0x87, 0x32, 0xc3, 0x3c);

// {1CBEE600-38CC-447d-AD19-D8B9D5836838}
GUARD_IMPLEMENT_OLECREATE(CActionClipboardAvail, "Data.ClipboardAvail",
0x1cbee600, 0x38cc, 0x447d, 0xad, 0x19, 0xd8, 0xb9, 0xd5, 0x83, 0x68, 0x38);

// {728A6305-8627-41ab-9F8A-47C3AECF51EB}
GUARD_IMPLEMENT_OLECREATE(CActionTouchObject, "Data.TouchObject", 
0x728a6305, 0x8627, 0x41ab, 0x9f, 0x8a, 0x47, 0xc3, 0xae, 0xcf, 0x51, 0xeb);


// {DF90919E-7370-4d94-B7E2-2C7BFD9129A5}
static const GUID guidShowContext =
{ 0xdf90919e, 0x7370, 0x4d94, { 0xb7, 0xe2, 0x2c, 0x7b, 0xfd, 0x91, 0x29, 0xa5 } };
// {D24117B8-73A6-4889-8F3F-DACA3771A433}
static const GUID guidObjectUnselectAll =
{ 0xd24117b8, 0x73a6, 0x4889, { 0x8f, 0x3f, 0xda, 0xca, 0x37, 0x71, 0xa4, 0x33 } };
// {D7DCC9DA-090F-470e-BC6C-6161548B7AFB}
static const GUID guidDataShowContent =
{ 0xd7dcc9da, 0x90f, 0x470e, { 0xbc, 0x6c, 0x61, 0x61, 0x54, 0x8b, 0x7a, 0xfb } };
// {45C90AF1-D32B-11d3-A090-0000B493A187}
static const GUID guidDeleteObject =
{ 0x45c90af1, 0xd32b, 0x11d3, { 0xa0, 0x90, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };
// {A624B385-7CA1-11d3-A66A-0090275995FE}
static const GUID guidObjectDrop =
{ 0xa624b385, 0x7ca1, 0x11d3, { 0xa6, 0x6a, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {A624B381-7CA1-11d3-A66A-0090275995FE}
static const GUID guidObjectDrag =
{ 0xa624b381, 0x7ca1, 0x11d3, { 0xa6, 0x6a, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {078653AD-6F1B-11d3-A659-0090275995FE}
static const GUID guidEditCopy =
{ 0x78653ad, 0x6f1b, 0x11d3, { 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {078653A9-6F1B-11d3-A659-0090275995FE}
static const GUID guidEditCut =
{ 0x78653a9, 0x6f1b, 0x11d3, { 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {078653A5-6F1B-11d3-A659-0090275995FE}
static const GUID guidEditPaste =
{ 0x78653a5, 0x6f1b, 0x11d3, { 0xa6, 0x59, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {9A43318D-6A8F-11d3-A650-0090275995FE}
static const GUID guidDataMoveToDoc =
{ 0x9a43318d, 0x6a8f, 0x11d3, { 0xa6, 0x50, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {9A433189-6A8F-11d3-A650-0090275995FE}
static const GUID guidDataMoveToApp =
{ 0x9a433189, 0x6a8f, 0x11d3, { 0xa6, 0x50, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {9A433185-6A8F-11d3-A650-0090275995FE}
static const GUID guidDataOrganizer =
{ 0x9a433185, 0x6a8f, 0x11d3, { 0xa6, 0x50, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {ACB48D75-5AE9-11d3-8817-0000B493FF1B}
static const GUID guidObjectSave =
{ 0xacb48d75, 0x5ae9, 0x11d3, { 0x88, 0x17, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {ACB48D71-5AE9-11d3-8817-0000B493FF1B}
static const GUID guidObjectLoad =
{ 0xacb48d71, 0x5ae9, 0x11d3, { 0x88, 0x17, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {6C0FB615-5ABC-11d3-8817-0000B493FF1B}
static const GUID guidObjectProperties =
{ 0x6c0fb615, 0x5abc, 0x11d3, { 0x88, 0x17, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {2B19CE71-53D8-11d3-880F-0000B493FF1B}
static const GUID guidObjectRename =
{ 0x2b19ce71, 0x53d8, 0x11d3, { 0x88, 0xf, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {3F65E409-4037-11d3-A60C-0090275995FE}
static const GUID guidObjectDelete =
{ 0x3f65e409, 0x4037, 0x11d3, { 0xa6, 0xc, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {3F65E405-4037-11d3-A60C-0090275995FE}
static const GUID guidObjectSelect =
{ 0x3f65e405, 0x4037, 0x11d3, { 0xa6, 0xc, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {3F65E401-4037-11d3-A60C-0090275995FE}
static const GUID guidObjectCreate =
{ 0x3f65e401, 0x4037, 0x11d3, { 0xa6, 0xc, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {45B42C4E-351F-46ee-951A-2392EFC9CB73}
static const GUID guidCreateImageByView = 
{ 0x45b42c4e, 0x351f, 0x46ee, { 0x95, 0x1a, 0x23, 0x92, 0xef, 0xc9, 0xcb, 0x73 } };
// {AAFA6247-2155-42c4-99DC-A3CDA6717F1A}
static const GUID guidClipboardAvail =
{ 0xaafa6247, 0x2155, 0x42c4, { 0x99, 0xdc, 0xa3, 0xcd, 0xa6, 0x71, 0x7f, 0x1a } };
// {D26E03A0-8F7D-46dc-94FC-C55696185FAE}
static const GUID guidTouchObject = 
{ 0xd26e03a0, 0x8f7d, 0x46dc, { 0x94, 0xfc, 0xc5, 0x56, 0x96, 0x18, 0x5f, 0xae } };




ACTION_INFO_FULL(CActionShowContext, IDS_ACTION_SHOWCONTEXT, -1, IDS_TB_DATA, guidShowContext);
ACTION_INFO_FULL(CActionObjectUnselectAll, IDS_ACTION_UNSELECT, -1, -1, guidObjectUnselectAll);
ACTION_INFO_FULL(CActionDataShowContent, IDS_ACTION_CONTENT, -1, -1, guidDataShowContent);
ACTION_INFO_FULL(CActionDeleteObject, IDS_ACTION_OBJECT_DELETE, -1, -1, guidDeleteObject);
ACTION_INFO_FULL(CActionObjectDrop, IDS_ACTION_OBJECT_DROP, -1, -1, guidObjectDrop);
ACTION_INFO_FULL(CActionObjectDrag, IDS_ACTION_OBJECT_DRAG, -1, -1, guidObjectDrag);
ACTION_INFO_FULL(CActionEditCopy, IDS_ACTION_EDITCOPY, IDS_MENU_EDIT_DUC, -1, guidEditCopy);
ACTION_INFO_FULL(CActionEditCut, IDS_ACTION_EDITCUT, IDS_MENU_EDIT_DUC, -1, guidEditCut);
ACTION_INFO_FULL(CActionEditPaste, IDS_ACTION_EDITPASTE, IDS_MENU_EDIT_DUC, -1, guidEditPaste);
ACTION_INFO_FULL(CActionDataMoveToDoc, IDS_ACTION_TODOC, -1, -1, guidDataMoveToDoc);
ACTION_INFO_FULL(CActionDataMoveToApp, IDS_ACTION_TOAPP, -1, -1, guidDataMoveToApp);
ACTION_INFO_FULL(CActionDataOrganizer, IDS_ACTION_ORGANIZE, -1, -1, guidDataOrganizer);
ACTION_INFO_FULL(CActionObjectSave, IDS_ACTION_SAVEOBJECT, IDS_MENU_EDIT, IDS_TB_DATA, guidObjectSave);
ACTION_INFO_FULL(CActionObjectLoad, IDS_ACTION_LOADOBJECT, IDS_MENU_EDIT, IDS_TB_DATA, guidObjectLoad);
ACTION_INFO_FULL(CActionObjectProperties, IDS_ACTION_OBJECTPROPS, IDS_MENU_EDIT, IDS_TB_DATA, guidObjectProperties);
ACTION_INFO_FULL(CActionObjectRename, IDS_ACTION_RENAMEOBJECT, IDS_MENU_EDIT, IDS_TB_DATA, guidObjectRename);
ACTION_INFO_FULL(CActionObjectDelete, IDS_ACTION_DELETEOBJECT, IDS_MENU_EDIT, IDS_TB_DATA, guidObjectDelete);
ACTION_INFO_FULL(CActionObjectSelect, IDS_ACTION_SELECTOBJECT, IDS_MENU_VIEW, IDS_TB_DATA, guidObjectSelect);
ACTION_INFO_FULL(CActionObjectCreate, IDS_ACTION_NEWOBJECT, IDS_MENU_EDIT, IDS_TB_DATA, guidObjectCreate);
ACTION_INFO_FULL(CActionCreateImageObjectByView, IDS_ACTION_CREATE_IMAGE_BY_VIEW, -1, -1, guidCreateImageByView);
ACTION_INFO_FULL(CActionClipboardAvail, IDS_ACTION_CLIPBOARDAVAIL, IDS_MENU_EDIT_DUC, -1, guidClipboardAvail);
ACTION_INFO_FULL(CActionTouchObject, IDS_ACTION_TOUCHOBJECT, -1, -1, guidTouchObject);


// targets


//ACTION_TARGET(CActionShowContext,	szTargetFrame);
ACTION_TARGET(CActionObjectUnselectAll, szTargetViewSite);
//ACTION_TARGET(CActionDataShowContent, "app");
ACTION_TARGET(CActionDeleteObject,	szTargetAnydoc);
ACTION_TARGET(CActionObjectDrop,	szTargetAnydoc);
ACTION_TARGET(CActionObjectDrag,	szTargetAnydoc);
ACTION_TARGET(CActionEditCopy,		szTargetAnydoc);
ACTION_TARGET(CActionEditCut,		szTargetAnydoc);
ACTION_TARGET(CActionEditPaste,		szTargetAnydoc);
ACTION_TARGET(CActionDataMoveToDoc,	szTargetAnydoc);
ACTION_TARGET(CActionDataMoveToApp,	szTargetAnydoc);
ACTION_TARGET(CActionDataOrganizer,	szTargetAnydoc);
ACTION_TARGET(CActionObjectSave,	szTargetAnydoc);
ACTION_TARGET(CActionObjectLoad,	szTargetAnydoc);
ACTION_TARGET(CActionObjectProperties, szTargetAnydoc);
ACTION_TARGET(CActionObjectRename,	szTargetAnydoc);
ACTION_TARGET(CActionObjectDelete,	szTargetAnydoc);
ACTION_TARGET(CActionObjectSelect,	szTargetViewSite);
ACTION_TARGET(CActionObjectCreate,	szTargetAnydoc);
ACTION_TARGET(CActionCreateImageObjectByView,	szTargetAnydoc);
ACTION_TARGET(CActionClipboardAvail,		szTargetApplication);
ACTION_TARGET(CActionTouchObject,	szTargetAnydoc);

// arguments

ACTION_ARG_LIST(CActionEditCopy)
	ARG_STRING("ObjectName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionEditCut)
	ARG_STRING("ObjectName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionDataMoveToDoc)
	ARG_STRING("ObjectName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionDataMoveToApp)
	ARG_STRING("ObjectName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectSave)
	ARG_STRING("ObjectName", 0)
	ARG_STRING("Path", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectLoad)
	ARG_STRING("Type", 0)
	ARG_STRING("Path", 0)
	ARG_INT("ActivateObject", 1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectProperties)
	ARG_STRING("Object", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectRename)
	ARG_STRING("OldName", 0)
	ARG_STRING("NewName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectDelete)
	ARG_STRING("Object", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectSelect)
	ARG_STRING("Object", 0)
	ARG_INT("ActivateType", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectDrop)
	ARG_INT("PositionX", 0)
	ARG_INT("PositionY", 0)
	ARG_INT("Copy", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionObjectDrag)
	////ARG_INT("ObjectName", 0)
	ARG_INT("xOffset", 0)
	ARG_INT("yOffset", 0)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionObjectCreate)
	ARG_STRING("Type", 0)
	ARG_STRING("Name", 0)
	ARG_INT("ActivateObject", 1)
END_ACTION_ARG_LIST()


ACTION_ARG_LIST(CActionCreateImageObjectByView)
	ARG_STRING("ImageName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionTouchObject)
	ARG_STRING("Object", 0)
END_ACTION_ARG_LIST()


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//CActionShowContext implementation
CActionShowContext::CActionShowContext()
{
}

CActionShowContext::~CActionShowContext()
{
}

bool CActionShowContext::Invoke()
{
	IUnknownPtr sptrCV = GetContextViewer(false);

	IWindowPtr sptrW = sptrCV;
	if (sptrW == 0)
		return false;

	HANDLE handle = 0;
	if (FAILED(sptrW->GetHandle(&handle)) || !handle)
		return false;

	//1. get the main window
	IUnknownPtr		sptrUnkMainWnd(::_GetOtherComponent(::GetAppUnknown(false), IID_IMainWindow), false);
	IMainWindowPtr	sptrM = sptrUnkMainWnd;
	if (sptrM == 0)
		return 0;

	HWND hWnd = (HWND)handle;

	sptrM->ShowControlBar( hWnd, true );
	
	if (::IsWindow(hWnd) && ::IsWindowVisible(hWnd))
		SetFocus(hWnd);

	return true;
}

//extended UI
bool CActionShowContext::IsAvaible()
{
	return true;
}

bool CActionShowContext::IsChecked()
{
	IUnknownPtr sptrCV = GetContextViewer(false);

	IWindowPtr sptrW = sptrCV;
	if (sptrW == 0)
		return false;

	HANDLE handle = 0;
	if (FAILED(sptrW->GetHandle(&handle)) || !handle)
		return false;

	HWND hWnd = (HWND)handle;
	BOOL bVisible = ::IsWindowVisible(hWnd);
	return bVisible == TRUE;
}

//return the propertysheet dockbar unknown 
IUnknown * GetContextViewer(bool bAddRef)
{
	//1. get the main window
	IUnknownPtr		sptrUnkMainWnd(::_GetOtherComponent(::GetAppUnknown(false), IID_IMainWindow), false);
	IMainWindowPtr	sptrM = sptrUnkMainWnd;
	if (sptrM == 0)
		return 0;

	//find the context_view window between dockbars
	POSITION lPos = 0;
	sptrM->GetFirstDockWndPosition(&lPos);
	IUnknownPtr	sptrDockBar;

	while (lPos)
	{
		sptrM->GetNextDockWnd(&sptrDockBar, &lPos);

		if (CheckInterface(sptrDockBar, IID_IContextView))
		{
			if (bAddRef)
				sptrDockBar.AddRef();
			return sptrDockBar;
		}
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////
//CActionObjectUnselectAll implementation
CActionObjectUnselectAll::CActionObjectUnselectAll()
{
}

CActionObjectUnselectAll::~CActionObjectUnselectAll()
{
}

bool CActionObjectUnselectAll::Invoke()
{
	IViewPtr	ptrView( m_punkTarget );
	IUnknown	*punkFrame = 0;
	ptrView->GetMultiFrame( &punkFrame, false );

	if( !punkFrame )return false;
	IMultiSelectionPtr	ptrFrame( punkFrame );
	punkFrame->Release();
	ptrFrame->EmptyFrame();


	return true;
}

//extended UI
bool CActionObjectUnselectAll::IsAvaible()
{
	IViewPtr	ptrView( m_punkTarget );
	IUnknown	*punkFrame = 0;
	ptrView->GetMultiFrame( &punkFrame, false );
	if( !punkFrame )return false;
	IMultiSelectionPtr	ptrFrame( punkFrame );
	punkFrame->Release();

	DWORD nObjectCount = 0;
	ptrFrame->GetObjectsCount( &nObjectCount );

	return nObjectCount >= 1;
}


//////////////////////////////////////////////////////////////////////
//CActionDataShowContent implementation
CActionDataShowContent::CActionDataShowContent()
{
}

CActionDataShowContent::~CActionDataShowContent()
{
}

bool CActionDataShowContent::Invoke()
{
	CString sPath( _T("") );
	long ldata_source = 0;
	ldata_source = GetValueInt( GetAppUnknown(), "\\DataShowContents", "DataSource", ldata_source );
	sPath = GetValueString( GetAppUnknown(), "\\DataShowContents", "SelectedPath", sPath );

	CContentDlg	dlg;
	// [vanek] SBT:898 restore selection - 05.04.2004
	dlg.SetLastDataSource( (int)(ldata_source) );
	dlg.SetLastSelectedPath( sPath );
	
	dlg.DoModal();

	// [vanek]: save selection 13.02.2004
	dlg.GetLastSelectedPath( &sPath );
	SetValue( GetAppUnknown(), "\\DataShowContents", "DataSource", (long)dlg.GetLastDataSource( ) );
	SetValue( GetAppUnknown(), "\\DataShowContents", "SelectedPath", sPath );

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionDeleteObject implementation
CActionDeleteObject::CActionDeleteObject()
{
}

CActionDeleteObject::~CActionDeleteObject()
{
}

bool CActionDeleteObject::Invoke()
{
	sptrIDocumentSite	sptrS( m_punkTarget );
	IUnknown* punkV = 0;
	sptrS->GetActiveView( &punkV );
	if( !punkV )return false;
	sptrIView	sptrV( punkV );
	punkV->Release();
	if(sptrV)
	{
		IUnknown* punkMF = 0;
		sptrV->GetMultiFrame(&punkMF, false);
		sptrIMultiSelection sptrMF(punkMF);
		if(punkMF)
			punkMF->Release();
		if(sptrMF)
		{
			//store objects to list
			CPtrArray	arrToDelete;
			DWORD nObjects = 0;	
			sptrMF->GetObjectsCount(&nObjects);

			arrToDelete.SetSize( nObjects );

			for(int i = nObjects-1; i >= 0; i--)
			{
				IUnknown* punk = 0;
				sptrMF->GetObjectByIdx(i, &punk);				

				INotifyObjectPtr ptrNObj( punk );
				if( ptrNObj != 0 && ptrNObj->NotifyDestroy() != S_OK )
				{
					arrToDelete[i] = 0;
					punk->Release();
				}
				else
				{
					arrToDelete[i] = punk;
				}
			}

			sptrMF->EmptyFrame();

			for(i = nObjects-1; i >= 0; i--)
			{
				IUnknown	*punk = (IUnknown*)arrToDelete[i];
				if( punk == 0 )
					continue;

				m_changes.RemoveFromDocData( m_punkTarget, punk, false );	
				punk->Release();
			}
			// Image has changed inside CImage::ExchangeWithParent, invalidate all views excluding
			// active - active view was already invalidated.
			if (nObjects > 0)
				__RedrawAllViews(m_punkTarget);
			
			return true;
		}
	}
	return false;
}

//undo/redo
bool CActionDeleteObject::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionDeleteObject::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//extended UI
bool CActionDeleteObject::IsAvaible()
{
	sptrIDocumentSite	sptrS( m_punkTarget );
	IUnknown* punkV = 0;
	sptrS->GetActiveView( &punkV );
	if( !punkV )
		return false;
	sptrIView	sptrV( punkV );
	punkV->Release();
	IUnknown* punkMF = 0;
	sptrV->GetMultiFrame(&punkMF, false);
	sptrIMultiSelection sptrMF(punkMF);
	if(!punkMF)
		return false;
	punkMF->Release();

	DWORD nObjects = 0;	
	sptrMF->GetObjectsCount( &nObjects );


	return nObjects > 0;
}

bool CActionDeleteObject::IsChecked()
{
	return false;
}



CArray<GuidKey, GuidKey&> m_arrCurrentParentKeys;
GuidKey	g_keyLastDrop;
//////////////////////////////////////////////////////////////////////
//CActionObjectDrop implementation
CActionObjectDrop::CActionObjectDrop()
{
	m_point = CPoint( 0, 0 );
}

CActionObjectDrop::~CActionObjectDrop()
{
}

bool CActionObjectDrop::Invoke()
{	
	//return false;
	m_point.x = GetArgumentInt( _T("PositionX") );
	m_point.y = GetArgumentInt( _T("PositionY") );

	//get the active view from the document target
	IUnknown	*punkV = 0;
	sptrIDocumentSite	sptrS( m_punkTarget );
	sptrS->GetActiveView( &punkV );

	if( !punkV )return false;
	sptrIView	sptrV( punkV );
	punkV->Release();

	//get base object
	INamedDataObject2Ptr sptrNDOBase;
	BOOL bBase = FALSE;
	TPOS POS = 0;
	sptrV->GetFirstVisibleObjectPosition(&POS);
	while(POS)
	{
		IUnknown* punkObject = 0;
		sptrV->GetNextVisibleObject(&punkObject, &POS);
		sptrNDOBase = punkObject;
		if(punkObject)
			punkObject->Release();
		if(sptrNDOBase != 0)
		{
			sptrNDOBase->IsBaseObject(&bBase);
			if(bBase == TRUE)
				break;
			else
				sptrNDOBase = 0;
		}
	}

	IUnknownPtr	sptrParent;

	bool	bOnlyOneObject = true;						//possible only one objects of this type
	bool	bSameParent = true;							//move to the different parent
	bool	bMove = GetArgumentInt( _T("Copy") )==0;	//is object moved?

	CPoint	pointDropOffset;
	DWORD numObjects = 0;
	sptrIMultiSelection sptrMF = 0;
	
	//init the new data object
	{
		IUnknown	*punkObjectToPaste = 0;	


		/*IDocumentSitePtr sptrDoc(m_punkTarget);
		IUnknown *punkView = 0;
		sptrDoc->GetActiveView(&punkView);
		IViewPtr sptrV(punkView);
		if(punkView)
			punkView->Release();
		IUnknown	*punkDataObject = 0;	
		sptrV->GetDroppedDataObject(&punkDataObject);

		IDataObjectPtr	sptrDO(punkDataObject);

		if(sptrDO == 0)
			return false;

		FORMATETC fmt;
		fmt.cfFormat = ::GetClipboardFormat();
		fmt.ptd = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex = -1;
		fmt.tymed = TYMED_HGLOBAL;
		STGMEDIUM stgMedium;
		sptrDO->GetData(&fmt, &stgMedium);
		HGLOBAL	hData = stgMedium.hGlobal;

		if(stgMedium.pUnkForRelease)
			stgMedium.pUnkForRelease->Release();
			
		//COleDataObject	oleObject;
		//oleObject.AttachClipboard();
		//HGLOBAL	hData = oleObject.GetGlobalData( ::GetClipboardFormat() );

		if( !hData )
			return false;

		
		//punkObjectToPaste = ::RestoreObjectFromHandle( hData, &pointDropOffset );
		*/

		IUnknown* punkFrame = 0;
		sptrV->GetMultiFrame(&punkFrame, true);
	
		//::RestoreMultiFrameFromHandle( hData,  punkFrame, &pointDropOffset);

		//::GlobalFree(hData);

		sptrMF = punkFrame;
		if(sptrMF)
			sptrMF->SetFrameOffset(m_point);
		
		if( punkFrame )
			punkFrame->Release();
		
		

		if( sptrMF )
			sptrMF->GetObjectsCount(&numObjects);
		////if( !punkObjectToPaste )
		/////	return false;

		/////m_sptrDrop = punkObjectToPaste;
		////punkObjectToPaste->Release();
	}
	
	ASSERT(numObjects > 0);

	//paul [10.08.2002] if CDesctopFrame, problem with ensure_visible
	//if( sptrMF != 0 && numObjects )
		//sptrMF->RedrawFrame();


	// paul 28.05.2002
	/*

	{
		bool bneed_terminate = false;
		IViewDataPtr ptr_view_data( sptrV );
		if( ptr_view_data )
		{
			for( int i = 0; i < numObjects; i++ )
			{
				IUnknown* punkObj = 0;
				sptrMF->GetObjectByIdx(i, &punkObj);
				INamedDataObject2Ptr ptr_drop = punkObj;
				punkObj->Release();	punkObj = 0;

				if( ptr_drop == 0 )		continue;

				IUnknown	*punkType = 0;
				ptr_drop->GetDataInfo( &punkType );
				if( !punkType )			continue;

				sptrINamedDataInfo	ptrTI( punkType );
				punkType->Release();	punkType = 0;
	
				BSTR	bstrContainer = 0;
				ptrTI->GetContainerType( &bstrContainer );
	
				CString	strContainer = bstrContainer;
				::SysFreeString( bstrContainer );	bstrContainer = 0;
	
				if( strContainer.IsEmpty() )
					continue;

				IUnknown* punk_parent = 0;
				ptr_view_data->GetObject( &punk_parent, _bstr_t( (LPCSTR)strContainer ) );
				if( !punk_parent )		continue;

				ptr_drop->SetParent( punk_parent, apfNone );				
				punk_parent->Release();	punk_parent = 0;
				bneed_terminate = true;
			}
		}

		if( bneed_terminate )
		{
			sptrV->MoveDragFrameToFrame();
			return true;
		}
	}
	*/
					
			
	g_keyLastDrop = m_key;


	for(unsigned i = 0; i < numObjects; i++)
	{
		IUnknown* punkObj = 0;
		sptrMF->GetObjectByIdx(i, &punkObj);
		m_sptrDrop = punkObj;
		punkObj->Release();
		//get the active object
		{
			IUnknown	*punkActive = 0;		
			_bstr_t	bstrObjectType = ::GetObjectKind( m_sptrDrop );
			sptrIDataContext	sptrC( sptrV );
			sptrC->GetActiveObject( bstrObjectType, &punkActive );
			m_sptrActive = punkActive;
	
			if( punkActive )
				punkActive->Release();
		}
	
		//check the view can present only one sub-object of this type
		{
			_bstr_t	bstrObjectType = ::GetObjectKind( m_sptrDrop );
			DWORD	dwObjectsFlags = 0;
			sptrV->GetObjectFlags( bstrObjectType, &dwObjectsFlags );
	
			bOnlyOneObject = (dwObjectsFlags & ofOnlyOneChild) != 0;
		}

		bool bLiveInContainer = false;
		//check the objects has the same parents
		{
			GuidKey	lPasteObjectParentKey;
			GuidKey	lActiveObjectParentKey;
	
			//get the pasted object parent key
			lPasteObjectParentKey = m_arrCurrentParentKeys[i];
			//get the active object parent key
			
			if( m_sptrActive )
			{
				IUnknown	*punkParent = 0;
				m_sptrActive->GetParent( &punkParent );
	
				lActiveObjectParentKey = ::GetObjectKey( punkParent );

				if( punkParent )
				{
					sptrParent = punkParent;
					punkParent->Release();
				}
				else
					sptrParent = m_sptrActive;
			}
			else
			{
				//possible parent is container type
				IUnknown	*punkType = 0;
				m_sptrDrop->GetDataInfo( &punkType );
				ASSERT( punkType );

				sptrINamedDataInfo	sptrTI( punkType );
				punkType->Release();
	
				BSTR	bstrContainer;
				sptrTI->GetContainerType( &bstrContainer );
	
				CString	strContainer = bstrContainer;
				::SysFreeString( bstrContainer );
	
				if( !strContainer.IsEmpty() )
				{
					bLiveInContainer = true;

					//[ay] - раньше брали из документа и соответственно обламывались в случае когда объект жил в
					//named data другого объекта - теперь пытаемся найти в видимых объектах и их data
					IUnknown *punkParent = 0;

					//paul 13.11.2002. Одно но: кол-во дровинигов в кариограмме равно кол-ву режимов, в каждом свой.
					IViewDataPtr ptr_view_data( sptrV );
					if( ptr_view_data )
						ptr_view_data->GetObject( &punkParent, _bstr_t( (LPCSTR)strContainer ) );
					//paul 13.11.2002. Поэтому хватать первый попавшийся не пойдет

					/*
					if( !punkParent )
					{
						TPOS	lpos = 0;

						sptrV->GetFirstVisibleObjectPosition( &lpos );
						
						while( lpos )
						{
							IUnknown	*punk_o = 0;
							sptrV->GetNextVisibleObject( &punk_o, &lpos );

							if( punk_o )
							{
								bstr_t	bstr_kind = GetObjectKind( punk_o );

								if( !strcmp( bstr_kind, strContainer ) )
								{
									//нашли среди видимых
									punkParent = punk_o;
									break;
								}

								//пробуем найти в NamedData объекта
								IDataTypeManagerPtr	ptrTM( punk_o );

								if( ptrTM )
								{
									long	types_count = 0;
									ptrTM->GetTypesCount( &types_count );

									for( long type = 0; type < types_count; type++ )
									{
										BSTR	bstr = 0;
										ptrTM->GetType( type, &bstr );

										CString	strT( bstr );
										::SysFreeString( bstr );

										if( !strcmp( strT, strContainer ) )
										{
											LPOS lpos_o;
											ptrTM->GetObjectFirstPosition( type, &lpos_o );

											if( lpos_o )
											{
												ptrTM->GetNextObject( type, &lpos_o, &punkParent );
												break;
											}

										}
									}

								}
								punk_o->Release();
								if( punkParent )
									//нашли в named data видимого объекта
									break;
							}
						}
					}
					*/

					//[ay] а так было до меня
					//IUnknown *punkParent = ::GetActiveObjectFromDocument( m_punkTarget, strContainer );

/*					if( !punkParent && bMove ) // Maxim (23.04.02) Creating new object instead assignment to old parent
					{
						if( strContainer == szTypeObject )
						{
							punkParent = ::GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );
							if( punkParent )
							{
								IUnknown *pChild = ::GetChildObjectByKey( punkParent, lPasteObjectParentKey );

								if( punkParent )
									punkParent->Release();

								punkParent = pChild;
							}

						}
					}
*/

					lActiveObjectParentKey = ::GetObjectKey( punkParent );
					sptrParent = punkParent;
					if( punkParent )
						punkParent->Release();
				}
			}
	
			if( lActiveObjectParentKey != INVALID_KEY )
				bSameParent = lActiveObjectParentKey == lPasteObjectParentKey;
			else
				bSameParent = false;
	
			m_lActiveParent = lActiveObjectParentKey;
		}
	
		//attach the current dataobject to the NamedData
		IUnknown	*punkObject = m_sptrActive;
		
	
		/*//!!! images only - set coords
		if( CheckInterface( m_sptrDrop, IID_IImage ) )
		{
			CImageWrp	image( m_sptrDrop );
			image.SetOffset( m_point );
		}
		else
		if( CheckInterface( m_sptrDrop, IID_IMeasureObject ) )
		{
			CObjectWrp	object( m_sptrDrop );
			CImageWrp	image( object.GetImage(), false );
			image.SetOffset( m_point );
		}
		*/
	
		//attach the new parent to the specified data object
		m_sptrDrop->SetParent( sptrParent, apfNone );

		bool bWaitForCreateContainer = bLiveInContainer && sptrParent == 0;
		if(!bWaitForCreateContainer)
		{
			if(sptrNDOBase != 0 && m_sptrDrop != 0)
			{
				GUID BaseKey;
				sptrNDOBase->GetBaseKey(&BaseKey);
				INamedDataObject2Ptr sptrNDODrop = m_sptrDrop;
				if(sptrNDODrop != 0)
					sptrNDODrop->SetBaseKey(&BaseKey);
			}
		}
				
		//map the object data to the parent
		if( ( !bMove ||	( bMove && !bSameParent ) ) && bOnlyOneObject && m_lActiveParent != INVALID_KEY )
		{
			//move the current object data to the parent
			m_changes.RemoveFromDocData( m_punkTarget, m_sptrActive );
		}
		
		//check unique name
		{
			BSTR	bstrName;
			INamedObject2Ptr	ptrNamed( m_sptrDrop );

			ptrNamed->GetName( &bstrName );

			CString	strOldName = bstrName;
			if( strOldName.IsEmpty() )
			{
				strOldName = ::GetObjectKind( m_sptrDrop );
				::SysFreeString( bstrName );
				bstrName = strOldName.AllocSysString(); 
			}

			INamedDataPtr	ptrND( m_punkTarget );

			long	lExist = false;
			ptrND->NameExists( bstrName, &lExist );

			if( lExist )
			{
				CString	strNewName = GenerateNameForArgument( strOldName, ptrND );
				::SysFreeString( bstrName );
				bstrName = strNewName.AllocSysString();
			}
			ptrNamed->SetName( bstrName );

			::SysFreeString( bstrName );


		}
		m_changes.SetToDocData( m_punkTarget, m_sptrDrop );

		if(bWaitForCreateContainer)
		{
			if(sptrNDOBase != 0 && m_sptrDrop != 0)
			{
				GUID BaseKey;
				sptrNDOBase->GetBaseKey(&BaseKey);
				INamedDataObject2Ptr sptrNDODrop;
				IUnknown	*punkParent = 0;
				m_sptrDrop->GetParent( &punkParent );
				if(punkParent)
				{
					sptrNDODrop = punkParent;
					punkParent->Release();
					if(sptrNDODrop != 0)
						sptrNDODrop->SetBaseKey(&BaseKey);

					IDataContextPtr sptrContext(sptrV);
					if(sptrContext != 0)
						sptrContext->SetActiveObject(0, sptrNDODrop, aofActivateDepended);
				}
			}
		}
	
	}

	sptrV->MoveDragFrameToFrame();
	return true;
}

bool CActionObjectDrop::DoUndo()
{
//remove dropped object
	m_changes.DoUndo( m_punkTarget );

	return true;
}

bool CActionObjectDrop::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectDrag implementation
//long	CActionObjectDrag::s_lCurrentParentKey = -1;

CActionObjectDrag::CActionObjectDrag()
{
	m_arrCurrentParentKeys.RemoveAll();
}

CActionObjectDrag::~CActionObjectDrag()
{
	m_arrCurrentParentKeys.RemoveAll();
}

bool CActionObjectDrag::Invoke()
{
	////CString	strObjectName = GetArgumentString( _T("ObjectName") );
	int		xOffset = GetArgumentInt( _T("xOffset") );
	int		yOffset = GetArgumentInt( _T("yOffset") );

	DWORD	dwFlags = 0;
	if( GetValueInt( GetAppUnknown(), "DragDrop", "OnlyImageMask", 0 ) )
		dwFlags |= sf_OnlyMaskOfImage;


	//get the active view
	IUnknown	*punkV = 0;
	sptrIDocumentSite	sptrS( m_punkTarget );
	sptrS->GetActiveView( &punkV );
	//no active view for dragdrop
	if( !punkV )
		return false;
	sptrIView	sptrV( punkV );
	punkV->Release();


	IUnknown* punkMF;
	sptrV->GetMultiFrame(&punkMF, false);
	sptrIMultiSelection	sptrMF(punkMF);
	if(punkMF)
		punkMF->Release();
	if(sptrMF == 0)
		return false;


	DWORD numObjects = 0;
	sptrMF->GetObjectsCount(&numObjects);
	if(numObjects == 0)
		return false;

	CPtrArray	arrDragObjects;

	//store frame's objects keys
	m_arrCurrentParentKeys.SetSize(numObjects);
	IUnknown* punkObj = 0;
	int n = numObjects;
	CPtrList unselect;
	for(unsigned i = 0; i < numObjects; i++)
	{
		sptrMF->GetObjectByIdx(i, &punkObj);

		//Composite support - dragging not allowed
		long bC = 0;;
		if(CheckInterface(punkObj, IID_IMeasureObject) && !CheckInterface(punkObj, IID_IManualMeasureObject))
		{
			INamedDataObject2Ptr nd(punkObj);
			IUnknown* parent;
			nd->GetParent(&parent);
			if(parent)
			{
				ICompositeObjectPtr co(parent);
				if(co)
				{
					co->IsComposite(&bC);
					if(bC) unselect.AddTail(punkObj);
				}
				parent->Release();
			}
		}
		if(!bC)
		{
			arrDragObjects.Add( punkObj );
			GuidKey nParentKey = ::GetParentKey( punkObj );
			//TRACE( "Key before drag %d\n", nParentKey );
			m_arrCurrentParentKeys[i] = nParentKey;
		}
	}
	numObjects -= unselect.GetSize();
	while(unselect.GetSize())
	{
		IUnknown* unk = (IUnknown*)unselect.RemoveTail();
		sptrMF->UnselectObject( unk );
		unk->Release();
	}


	CPoint	pointDragOffset( xOffset, yOffset );

	SerializeParams	params;
	ZeroMemory( &params, sizeof( params ) );
	params.flags = dwFlags;

	HGLOBAL	h = ::StoreMultiFrameObjectsToHandle( sptrMF, pointDragOffset, 0, &params );

	//preform ole dragdrop
	DROPEFFECT	effect = DROPEFFECT_NONE;
	COleDataSource	*pdataSource = new COleDataSource();
	bool bEnable = 	CMouseImpl::Enable( false );
	try
	{
		//pdataSource->GetControllingUnknown()->AddRef();
		pdataSource->CacheGlobalData( GetClipboardFormat(), h );
		//pdataSource->SetClipboard();

		//s_lCurrentParentKey = lParentKey;
		effect = pdataSource->DoDragDrop( DROPEFFECT_COPY|DROPEFFECT_MOVE );
		CMouseImpl::Enable( bEnable );
		//s_lCurrentParentKey = -1;
		m_arrCurrentParentKeys.RemoveAll();

		//pdataSource->GetControllingUnknown()->Release();

		//pdataSource->Empty();

		if(h)
		{
			HGLOBAL hh = ::GlobalFree(h);
			if(hh != NULL)
			{
				DWORD dw = GetLastError();
				ASSERT(FALSE);
			}
		}
	
		delete pdataSource;


//try to find drop action in list 
		IUndoListPtr	ptrUndoList( m_punkTarget );
		if( ptrUndoList != 0 )
		{
			IUnknown	*punk = 0;
			ptrUndoList->GetLastUndoAction( &punk );

			if( punk )
			{
				//_ReportCounter( punk );
				GuidKey	key = ::GetObjectKey( punk );

				if( key == g_keyLastDrop )
				{
					//_ReportCounter( punk );
					m_ptrDropUndo = punk;
					m_ptrDropAction = punk;
					//_ReportCounter( punk );
					ptrUndoList->RemoveAction( punk );
				}

				punk->Release();
			}
		}
	}
	catch( CException *pe )
	{
		effect = DROPEFFECT_NONE;
		pe->Delete();
		delete pdataSource;
	}

	
	//TRACE0("Drag action comlete!\n");

	//if object moved, delete it
	for(i = 0; i < numObjects; i++)
	{
		IUnknown *punk = (IUnknown *)arrDragObjects[i];

		GuidKey nParentKey = ::GetParentKey( punk );
		//TRACE( "Key after drag %d\n", nParentKey );
		if( effect == DROPEFFECT_MOVE )
			m_changes.RemoveFromDocData( m_punkTarget, punk, false, (dwFlags & sf_OnlyMaskOfImage)==0 );	
		punk->Release();
	}

	// Image has changed inside CImage::ExchangeWithParent, invalidate all views excluding
	// active - active view was already invalidated.
	if (numObjects && (dwFlags & sf_OnlyMaskOfImage)==0)
		__RedrawAllViews(m_punkTarget);

	if( effect == DROPEFFECT_NONE )
		return false;
	
	return true;
}

bool CActionObjectDrag::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	if( m_ptrDropUndo != 0 )
		m_ptrDropUndo->Undo();

	return true;
}

bool CActionObjectDrag::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	if( m_ptrDropUndo != 0 )
		m_ptrDropUndo->Redo();

	return true;
}

//CActionEditCopy implementation
CActionEditCopy::CActionEditCopy()
{
	m_punkDataObject = 0;
}

CActionEditCopy::~CActionEditCopy()
{
	if(m_punkDataObject)
		m_punkDataObject->Release();
	m_punkDataObject = 0;
}


void CActionEditCopy::PutObjectToClipboard(IUnknown* punkDataObject)
{

	IStreamPtr ptrStream;
	{
		IStream* pStream = 0;
		CreateStreamOnHGlobal( 0, FALSE, &pStream );

		if( pStream == 0 )
			return;

		ptrStream = pStream;
		if( pStream )
			pStream->Release(); pStream = 0;
	}



	CString strObjectName = ::GetObjectName( punkDataObject );
	CString	strObjectType = ::GetObjectKind( punkDataObject );

	//CSharedFile	file;
	{
		//CArchive	ar( &file, CArchive::store );
		CStreamEx ar( ptrStream, false );

		ar<<(CString)"";
		ar<<strObjectName;
		ar<<strObjectType;
		ar.Flush();

		//CArchiveStream	stream( &ar );

		IUnknown	*punkParentObject = 0;
		sptrINamedDataObject2	sptrN( punkDataObject );
		sptrN->GetParent( &punkParentObject );
		DWORD	dwFlags = 0;
		sptrN->GetObjectFlags( &dwFlags );

		IUnknownPtr punkND;
		sptrN->GetData(&punkND);

		if( dwFlags & nofHasData )sptrN->SetParent( 0, apfNone );
		else sptrN->SetParent( 0, sdfCopyParentData );

		SerializeParams	params;
		ZeroMemory( &params, sizeof( params ) );
		params.flags = sf_DetachParent;

		sptrISerializableObject	sptrS( punkDataObject );
		sptrS->Store( ptrStream, &params );

		if( dwFlags & nofHasData ) sptrN->SetParent( punkParentObject, apfNone );
		else sptrN->SetParent( punkParentObject, sdfAttachParentData );

		// A.M. fix SBT1786
		if (punkND != 0)
			sptrN->AttachData(punkND);

		if( punkParentObject )punkParentObject->Release();

	}

	//file.Flush();
	/*long	nBufferSize = file.GetLength();


	HGLOBAL	hGlobal = ::GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, nBufferSize );
	void	*pdata = ::GlobalLock( hGlobal );

	if( !pdata )
		return;


	byte	*pdataFile = file.Detach();
	::memcpy( pdata, pdataFile, nBufferSize );

	::free( pdataFile );

	::GlobalUnlock( hGlobal );*/

	HGLOBAL	hGlobal = 0;//file.Detach();

	GetHGlobalFromStream( ptrStream, &hGlobal );

	if(hGlobal)
		::SetClipboardData( ::GetClipboardFormat(), hGlobal );
}

void CActionEditCopy::PutNativeFormatToClipboard(IUnknown* punkDataObject)
{
	IClipboardPtr sptrClip(punkDataObject);
	if(sptrClip != 0)
	{
		HANDLE hGlobal = 0;
		long nPos = 0;
		sptrClip->GetFirstFormatPosition(&nPos);
		while(nPos)
		{
			UINT nRegFormat = 0;
			sptrClip->GetNextFormat(&nRegFormat, &nPos);
			if(nRegFormat)
			{
				sptrClip->Copy(nRegFormat, &hGlobal);
				if(hGlobal)
					::SetClipboardData(nRegFormat, hGlobal );
			}
		}
	}
}


bool CActionEditCopy::Invoke()
{
	if(m_punkDataObject)
		m_punkDataObject->Release();
	m_punkDataObject = 0;

	CString	strObjectName = GetArgumentString( _T("ObjectName") );

	SetArgument( _T("ObjectName"), _variant_t( strObjectName ) );

	HWND	hWnd = 0;
	sptrIApplication	sptrA( GetAppUnknown() );
	sptrA->GetMainWindowHandle( &hWnd );

	IUnknown* punkDataObject = 0;
	
	if(!strObjectName.IsEmpty())
	{
		m_punkDataObject = ::FindObjectByName( m_punkTarget, strObjectName );
		if(m_punkDataObject)
		{
			::OpenClipboard( hWnd );
			::EmptyClipboard();
			PutObjectToClipboard(m_punkDataObject);
			PutNativeFormatToClipboard(m_punkDataObject);
			::CloseClipboard();
			//punkDataObject->Release();
		}
		return true;
	}


	IDocumentSitePtr sptrDoc(m_punkTarget);
	
	IUnknown *punkView = 0;
	sptrDoc->GetActiveView(&punkView);

	IViewPtr sptrView(punkView);
	if(punkView)
		punkView->Release();

	if(sptrView == 0)
		return false;

	IMeasureViewPtr sptrMeasVw(punkView);
	if (sptrMeasVw != 0)
	{ // special treatment for measure view
		sptrMeasVw->CopyToClipboard(cf_grid);
		return true;
	}

	IUnknown* punkMF = 0;
	sptrView->GetMultiFrame(&punkMF, FALSE);
	sptrIMultiSelection sptrMF(punkMF);
	if(punkMF)
		punkMF->Release();


	bool bUseADO = true;
	if(sptrMF != 0)
	{
		DWORD nObjects = 0;	
		sptrMF->GetObjectsCount(&nObjects);
		if(nObjects)
			bUseADO = false;
	}
	
	::OpenClipboard( hWnd );
	::EmptyClipboard();

	bool bcopied = true;

	if(bUseADO)		
	{
		IClipboardProviderPtr ptrCP( sptrView );
		if( ptrCP )
			ptrCP->GetActiveObject( &m_punkDataObject );

		if( !m_punkDataObject )
			m_punkDataObject = ::GetActiveDataObject( m_punkTarget );

		if(m_punkDataObject)
		{
			PutObjectToClipboard(m_punkDataObject);
			PutNativeFormatToClipboard(m_punkDataObject);
			//punkDataObject->Release();
		}

		bcopied = true;
	}
	else
	{
		m_punkDataObject = sptrMF;
		m_punkDataObject->AddRef();

		//sptrMF->CleanVirtualsBack(TRUE);
		SerializeParams	params;
		ZeroMemory( &params, sizeof( params ) );

		HGLOBAL	hGlobal = ::StoreMultiFrameObjectsToHandle(sptrMF, CPoint(0, 0), 0, &params );
		//sptrMF->CleanVirtualsBack(FALSE);

		if(hGlobal)
        {
			::SetClipboardData( ::GetClipboardFormat(), hGlobal );


			sptrMF->GetObjectByIdx(0, &punkDataObject);
			if(punkDataObject)
			{
				PutNativeFormatToClipboard(punkDataObject);
				punkDataObject->Release();
			}

			bcopied = true;
		}
		else
			bcopied = false;
	}

	
	::CloseClipboard();

	return bcopied;
}

//undo/redo
//extended UI
bool CActionEditCopy::IsAvaible()
{
	IUnknownPtr punkDataObject(::GetActiveDataObject( m_punkTarget ), false);
	if( punkDataObject == 0 )
		return false;
	
	// A.M. fix BT5037.
	CString	strObjectName = GetArgumentString( _T("ObjectName") );
	if (strObjectName.IsEmpty())
	{
		IDocumentSitePtr sptrDoc(m_punkTarget);
		IUnknownPtr punkView;
		sptrDoc->GetActiveView(&punkView);
		CString sViewName = ::GetObjectName(punkView);
		if (!sViewName.IsEmpty())
		{
			if (sViewName == _T("BlankView"))
			{
				short nFieldType;
				CString sVTObjType;
				if (GetDBActiveField(m_punkTarget, nFieldType, sVTObjType) &&
					nFieldType == ftVTObject && !sVTObjType.IsEmpty())
				{
					BOOL bEnable = GetValueInt(::GetAppUnknown(), _T("\\EditCopy\\Enable\\BlankView"), sVTObjType, TRUE);
					if (!bEnable)
						return false;
				}
			}
			BOOL bEnable = GetValueInt(::GetAppUnknown(), "\\EditCopy\\Enable", sViewName, TRUE);
			if (!bEnable)
				return false;
		}
		if (punkView != 0)
		{
			IViewPtr sptrView(punkView);
			IMeasureViewPtr sptrMeasVw(punkView);
			if (sptrMeasVw == 0 && sptrView != 0)
			{
				IUnknownPtr punkMF;
				sptrView->GetMultiFrame(&punkMF, FALSE);
				sptrIMultiSelection sptrMF(punkMF);
				bool b1Obj = true;
				DWORD nObjects = 0;	
				if (sptrMF != 0)
				{
					sptrMF->GetObjectsCount(&nObjects);
					if (nObjects)
						b1Obj = false;
				}
				if (b1Obj)
				{
					IUnknownPtr punkObj;
					IClipboardProviderPtr ptrCP(punkView);
					if (ptrCP)
						ptrCP->GetActiveObject(&punkObj);
					if (punkObj == 0)
						punkObj = IUnknownPtr(::GetActiveDataObject(m_punkTarget), false);
					DWORD dwObjectFlags = 0;
					INamedDataObject2Ptr ptrNDO2(punkObj);
					if (ptrNDO2 != 0)
						ptrNDO2->GetObjectFlags(&dwObjectFlags);
					if ((dwObjectFlags&nofCantSendTo) != 0)
						return false;
				}
				else
				{
					bool bFound = false;
					for (int i = 0; i < nObjects; i++)
					{
						IUnknownPtr punkObj;
						DWORD dwObjectFlags = 0;
						INamedDataObject2Ptr ptrNDO2(punkObj);
						if (ptrNDO2 != 0)
							ptrNDO2->GetObjectFlags(&dwObjectFlags);
						if ((dwObjectFlags&nofCantSendTo) == 0)
						{
							bFound = true;
							break;
						}
					}
					if (!bFound)
						return false;
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionEditCut implementation
CActionEditCut::CActionEditCut()
{
}

CActionEditCut::~CActionEditCut()
{
}

bool CActionEditCut::Invoke()
{
	if( !CActionEditCopy::Invoke() )
	{
		return false;
	}

	sptrIMultiSelection sptrMF(m_punkDataObject);
	if(sptrMF != 0)
	{
		DWORD nObjects = 0;	
		sptrMF->GetObjectsCount(&nObjects);
		for(int i = nObjects-1; i >= 0; i--)
		{
			IUnknown* punk = 0;
			sptrMF->GetObjectByIdx(i, &punk);
			if(punk)
			{
				m_changes.RemoveFromDocData( m_punkTarget, punk, false, true );	
				punk->Release();
			}
		}
		sptrMF->EmptyFrame();
	}
	else
		m_changes.RemoveFromDocData( m_punkTarget, m_punkDataObject );

	return true;
}

//undo/redo
bool CActionEditCut::DoUndo()
{
	//::SetValue( m_punkTarget, 0, 0, _variant_t( m_punkDataObject ) );
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionEditCut::DoRedo()
{
	//::DeleteObject( m_punkTarget, m_punkDataObject );
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionEditPaste implementation
CActionEditPaste::CActionEditPaste()
{
	
}

CActionEditPaste::~CActionEditPaste()
{

}


IUnknown* GetActiveObjectFromDBaseDocument( IUnknown* pUnkDocument, CString strObjectType )
{
////////////////////////////////////////////////////////////////
	// 13.07.2000 Paul reason : DBase Support
	sptrIDBaseDocument spDBDoc( pUnkDocument );
	sptrISelectQuery spQuery;	

	if( spDBDoc != NULL )
	{
		IUnknown* pUnkActiveQuery = NULL;
		spDBDoc->GetActiveQuery( &pUnkActiveQuery );
		if( pUnkActiveQuery != NULL )
		{
			spQuery = pUnkActiveQuery;
			pUnkActiveQuery->Release();
		}		
	}	


	if( spDBDoc != NULL && spQuery != NULL )
	{
		//sptrIDataContext spDC( pUnkDocument );
		//if( spDC )
		{
			IUnknown* pUnkActiveObject = NULL;
			//spDC->GetActiveObject( _bstr_t((LPCSTR)strObjectType), &pUnkActiveObject );
			pUnkActiveObject = ::GetActiveObjectFromDocument( spDBDoc, strObjectType );

			if( pUnkActiveObject != NULL )
			{
				INamedObject2Ptr spNO2( pUnkActiveObject );
				if( spNO2 )
				{
					BSTR bstrObjectName;
					spNO2->GetName( &bstrObjectName );
					
					CString strObjectName = bstrObjectName;					
					
					::SysFreeString( bstrObjectName );

					if( strObjectName.GetLength() > 2 )
					{
						int nPoint = strObjectName.Find( "." );
						if( nPoint != -1 )
						{
							CString strField, strTable;
							strTable = strObjectName.Left( nPoint );
							strField = strObjectName.Right( strObjectName.GetLength() - nPoint - 1 );

							BOOL bAvailable = FALSE;							
							spQuery->IsAvailableField( 
										strTable.AllocSysString(),
										strField.AllocSysString(),
										&bAvailable										
										);							

							if( bAvailable )
							{								
								return pUnkActiveObject;
							}
						}
					}										
				}						
				
				pUnkActiveObject->Release();
			}
		}
	}
	return NULL;

}	// 13.07.200 Paul EOC

////////////////////////////////////////////////////////////////



IUnknown* CActionEditPaste::CreateDataObject(CString strObjectType, CString strObjectName, CStreamEx*	par)
{
	IUnknown* punkDataObject = ::CreateTypedObject( strObjectType );

	CString	strOldName = ::GetObjectName( punkDataObject );

	if( !punkDataObject )
		return false;

	if(par)
	{
		//CArchiveStream	stream(par);
		SerializeParams	params;
		ZeroMemory( &params, sizeof( params ) );

		sptrISerializableObject	sptrS( punkDataObject );
		sptrS->Load( (IStream*)*par, &params );
	}

	
	_bstr_t	bstrName = GetObjectName( punkDataObject );
	INamedDataPtr	ptrData( m_punkTarget );
	long	NameExist = 0;
	ptrData->NameExists( bstrName, &NameExist );

	if( !NameExist )
		::SetObjectName( punkDataObject, strObjectName );
	else
	{
		::GenerateUniqueNameForObject( punkDataObject, m_punkTarget );
		INumeredObjectPtr	sptrN( punkDataObject );
		sptrN->GenerateNewKey( 0 );
	}

	//m_changes.SetToDocData(sptrD, punkDataObject);

	/*IDocumentSitePtr	sptrDoc(sptrD);
	if(sptrDoc != 0)
	{
		IUnknown* punkView = 0;
		sptrDoc->GetActiveView(&punkView);
		if(punkView)
		{
			IDataContextPtr sptrContext(punkView);
			if(sptrContext != 0)
			{
				sptrContext->SetActiveObject(_bstr_t(strObjectType), punkDataObject);
			}
			punkView->Release();
		}
	}*/


	return punkDataObject;
}


void CActionEditPaste::SetObjectToDoc(IUnknown* punkObj)
{
	sptrINamedDataObject2 sptrNO(punkObj);
	sptrINamedDataObject2 sptrActive;

	IUnknown* punkV = 0;
	sptrIDocumentSite	sptrS( m_punkTarget );
	sptrS->GetActiveView( &punkV );
	if( !punkV )return;
	sptrIView	sptrV( punkV );
	punkV->Release();

	IUnknownPtr	sptrParent;

	//get the active object
	{
		IUnknown	*punkActive = 0;		
		_bstr_t	bstrObjectType = ::GetObjectKind( sptrNO );
		sptrIDataContext	sptrC( sptrV );
		sptrC->GetActiveObject( bstrObjectType, &punkActive );
		sptrActive = punkActive;

		if( punkActive )
			punkActive->Release();
	}

	if( sptrActive )
	{
		IUnknown	*punkParent = 0;
		sptrActive->GetParent( &punkParent );
		sptrParent = punkParent;
		if( punkParent )
			punkParent->Release();
		else
			sptrParent = sptrActive;
	}
	else
	{
		{
			IActiveXCtrlPtr ptrAXCtrl( sptrNO );
			if( ptrAXCtrl )
			{
				CRect rc = NORECT;
				ptrAXCtrl->GetRect( &rc );
				rc.OffsetRect( 10, 10 );
				ptrAXCtrl->SetRect( rc );

			}
			
		}
		//possible parent is container type
		IUnknown	*punkType = 0;
		sptrNO->GetDataInfo( &punkType );
		ASSERT( punkType );

		sptrINamedDataInfo	sptrTI( punkType );
		punkType->Release();

		BSTR	bstrContainer;
		sptrTI->GetContainerType( &bstrContainer );

		CString	strContainer = bstrContainer;
		::SysFreeString( bstrContainer );

		if( !strContainer.IsEmpty() )
		{
			IUnknown *punkParent = ::GetActiveObjectFromDocument( m_punkTarget, strContainer );
			sptrParent = punkParent;
			if( punkParent )
				punkParent->Release();
		}
	}

	//attach the new parent to the specified data object
	sptrNO->SetParent( sptrParent, apfNone );

	//check is name unique
	m_changes.SetToDocData( m_punkTarget, sptrNO );
}


bool CActionEditPaste::Invoke()
{
	HWND	hWnd = 0;

	sptrIApplication	sptrA( GetAppUnknown() );
	sptrA->GetMainWindowHandle( &hWnd );


	::OpenClipboard( hWnd );
	HGLOBAL	hGlobal = ::GetClipboardData(::GetClipboardFormat());
	if(hGlobal)
	{
		//byte	*pData = (byte	*)::GlobalLock( hGlobal );

		IStreamPtr ptrStream;
		{
			IStream* pStream = 0;
			CreateStreamOnHGlobal( hGlobal, FALSE, &pStream );

			ptrStream = pStream;

			if( ptrStream == 0 )
				return false;


		}

		CStreamEx ar( ptrStream, true );


		//CMemFile	file( pData, ::GlobalSize( hGlobal ) );
		//CArchive	ar( &file,  CArchive::load );

		CString	strObjectName;
		CString	strObjectType;

				
		CString strSignature;
		ar>>strSignature;
		if(strSignature == szMultiFrameSignature)
		{
			//ar.Close();
			//file.Close();
			::GlobalUnlock( hGlobal );


			/*CPoint point(0, 0);
			IDocumentSitePtr sptrDoc(m_punkTarget);
								
			IUnknown *punkView = 0;
			sptrDoc->GetActiveView(&punkView);

			IViewPtr sptrView(punkView);
			if(punkView)
				punkView->Release();

			if(sptrView == 0)
			{
				return false;
			}

			IUnknown* punkMF = 0;
			sptrView->GetMultiFrame(&punkMF, TRUE);
			IMultiSelectionPtr sptrMF(punkMF);
			if(punkMF)punkMF->Release();*/

			//create dummy frame, if view doesn't support own
			CPoint point(0, 0);
			CFrame	*p = new 	CFrame;
			IUnknown	*punk = p->GetControllingUnknown();
			IMultiSelectionPtr sptrMF = punk;
			punk->Release();

			SerializeParams	params;
			ZeroMemory( &params, sizeof( params ) );

			::RestoreMultiFrameFromHandle( hGlobal, sptrMF, &point, &params );


			INotifyPlacePtr ptrDoc( m_punkTarget );


			DWORD numObjects = 0;

			sptrMF->GetObjectsCount(&numObjects);
			
			ASSERT(numObjects > 0);

			bool	bFirstObject = true;
			for(unsigned i = 0; i < numObjects; i++)
			{
				IUnknown* punkObj = 0;
				sptrMF->GetObjectByIdx(i, &punkObj);
				

				if(punkObj)
				{
					INamedDataObject2Ptr	ptrN( punkObj );
					punkObj->Release();	punkObj = 0;

					HRESULT hRes = S_OK;
					bool bProcessByNotify = false;
					bool bTerminate = false;
					if( ptrDoc )
					{
						IUnknown *punkUndoObj1 = NULL;
						hRes = ptrDoc->NotifyPutToDataEx( ptrN, &punkUndoObj1 );
						IUnknownPtr punkUndoObj(punkUndoObj1, FALSE);
						bProcessByNotify = ( S_OK == hRes );
						if ( E_FAIL == hRes )
						{							
							continue;
						}
						m_arrUndoObjects.Add(punkUndoObj);
					}

					if( !bProcessByNotify )
					{
						SetObjectToDoc( ptrN );					
						if( bFirstObject )
						{
							DWORD	dwFlags = 0;
							ptrN->GetObjectFlags( &dwFlags );

							if( (dwFlags & nofStoreByParent) || (dwFlags & nofHasHost ) )
								continue;

							IUnknown	*punkView = 0;
							IDocumentSitePtr	ptrDocSite = m_punkTarget;
							ptrDocSite->GetActiveView( &punkView );

							if( punkView )
							{
								::AjustViewForObject( punkView, ptrN );
								punkView->Release();
							}
							bFirstObject = false;
						}
					}
				}
			}

			//sptrView->MoveDragFrameToFrame();
		}
		else
		{
			ar>>strObjectName>>strObjectType;
			ar.Flush();


			INotifyPlacePtr ptrDoc( m_punkTarget );
			
			IUnknown* punkObj = CreateDataObject( strObjectType, strObjectName, &ar );

			HRESULT hRes = S_OK;
			bool bProcessByNotify = false;
			bool bTerminate = false;
			if( ptrDoc )
			{
				IUnknown *punkUndoObj1 = NULL;
				hRes = ptrDoc->NotifyPutToDataEx( punkObj, &punkUndoObj1 );
				IUnknownPtr punkUndoObj(punkUndoObj1, FALSE);
				bProcessByNotify = ( S_OK == hRes );
				bTerminate = ( E_FAIL == hRes );
				if (bProcessByNotify)
					m_arrUndoObjects.Add(punkUndoObj);
			}

			if( bTerminate )
			{
				if( punkObj )
					punkObj->Release();
				return false;
			}

			if( !bProcessByNotify )			
				m_changes.SetToDocData(m_punkTarget, punkObj);			


			if( !bProcessByNotify )
			{
				IUnknown	*punkView = 0;
				IDocumentSitePtr	ptrDocSite = m_punkTarget;
				ptrDocSite->GetActiveView( &punkView );

				if( punkView )
				{
					::AjustViewForObject( punkView, punkObj );
					punkView->Release();
				}
			}


			if(punkObj)
				punkObj->Release();

			::GlobalUnlock( hGlobal );
		}
	}
	else
	{

		POSITION pos = ::GetFirstClipboardFormatPos();
		while(pos)
		{
			UINT nFormat = 0;
			CString strType = "";
			::GetClipboardFormatByPos(pos, nFormat, strType);
			if(!::IsClipboardFormatAvailable(nFormat))
				continue;
			if(strType.IsEmpty())
				continue;
			else
			{
				hGlobal = ::GetClipboardData(nFormat);
				
				IUnknown* punkObj = CreateDataObject(strType);
				if(punkObj)
				{
					//SetObjectToDoc(punkObj);

					IClipboardPtr sptrClip(punkObj);
					punkObj->Release();
					if(sptrClip != 0)
					{
						if( sptrClip->Paste(nFormat, hGlobal) != S_OK )
						{
							AfxMessageBox( IDS_ERROR_PASTING );
							continue;
						}


						INotifyPlacePtr ptrDoc( m_punkTarget );

						HRESULT hRes = S_OK;
						bool bProcessByNotify = false;
						bool bTerminate = false;
						if( ptrDoc )
						{
							IUnknown *punkUndoObj1 = NULL;
							hRes = ptrDoc->NotifyPutToDataEx( sptrClip, &punkUndoObj1 );
							IUnknownPtr punkUndoObj(punkUndoObj1, FALSE);
							bProcessByNotify = ( S_OK == hRes );
							if ( E_FAIL == hRes )
								continue;
							m_arrUndoObjects.Add(punkUndoObj);
						}

						if( !bProcessByNotify )			
						{
							m_changes.SetToDocData(m_punkTarget, sptrClip);
							CheckImageSignature(sptrClip);

							IUnknown	*punkView = 0;
							IDocumentSitePtr	ptrDocSite = m_punkTarget;
							ptrDocSite->GetActiveView( &punkView );

							if( punkView )
							{
								::AjustViewForObject( punkView, sptrClip );
								punkView->Release();
							}
						}
					}
				}
			}
		}

	}
	::CloseClipboard();
	
	return true;
}

//undo/redo
bool CActionEditPaste::DoUndo()
{
	//::DeleteObject( m_punkTarget, m_punkDataObject );
	m_changes.DoUndo( m_punkTarget );
	INotifyPlacePtr ptrDoc( m_punkTarget );
	if (ptrDoc != 0)
	{
		for (int i = 0; i < m_arrUndoObjects.GetSize(); i++)
			ptrDoc->Undo(m_arrUndoObjects.GetAt(i));
	}
	return true;
}

bool CActionEditPaste::DoRedo()
{
	//::SetValue( m_punkTarget, 0, 0, _variant_t( m_punkDataObject ) );
	m_changes.DoRedo( m_punkTarget );
	INotifyPlacePtr ptrDoc( m_punkTarget );
	if (ptrDoc != 0)
	{
		for (int i = 0; i < m_arrUndoObjects.GetSize(); i++)
			ptrDoc->Redo(m_arrUndoObjects.GetAt(i));
	}
	return true;
}

//extended UI
bool CActionEditPaste::IsAvaible()
{
	if(::IsClipboardFormatAvailable(::GetClipboardFormat()))
		return true;


	POSITION pos = ::GetFirstClipboardFormatPos();
	while(pos)
	{
		UINT nFormat = 0;
		CString strType = "";
		::GetClipboardFormatByPos(pos, nFormat, strType);
		if(::IsClipboardFormatAvailable(nFormat))
			return true;
	}




	/*UINT nFormat = EnumClipboardFormats(0);
	while(nFormat)
	{
		if(!GetObjectTypeByClipboardFormat(nFormat).IsEmpty())
			return true;

		nFormat = EnumClipboardFormats(nFormat); 
	}
	*/

	return false;
}


//////////////////////////////////////////////////////////////////////
//CActionObjectMoveBase implementation
CActionObjectMoveBase::CActionObjectMoveBase()
{
	m_punkObjToMove = 0;
	m_punkFrom = 0;
	m_punkTo = 0;

}

CActionObjectMoveBase::~CActionObjectMoveBase()
{
	if( m_punkObjToMove )
		m_punkObjToMove->Release();
}

bool CActionObjectMoveBase::MoveObjects()
{
	sptrINamedDataObject2	sptrN( m_punkObjToMove );
	IUnknown *punkParent = 0;
	sptrN->GetParent( &punkParent );

	::DeleteObject( m_punkFrom, m_punkObjToMove );

	if( punkParent )
	{
		m_lParentKey = ::GetObjectKey( punkParent );
		sptrN->SetParent( 0, apfNotifyNamedData );

		punkParent->Release();
	}

	::SetValue( m_punkTo, 0, 0, _variant_t( m_punkObjToMove ) );

	return true;
}

bool CActionObjectMoveBase::Invoke()
{					  
	ASSERT( m_punkFrom );
	ASSERT( m_punkTo );

	m_strObjectName = GetArgumentString( _T("ObjectName") );

	if( m_strObjectName.IsEmpty() )
		return false;

	m_punkObjToMove = ::FindObjectByName( m_punkFrom, m_strObjectName );

	if( !m_punkObjToMove )
		return false;

	return MoveObjects();
}

bool CActionObjectMoveBase::DoUndo()
{
	ASSERT(m_punkObjToMove);

	if( m_lParentKey != INVALID_KEY )
	{
		IUnknown	*punkParent = ::GetObjectByKey( m_punkFrom, m_lParentKey );

		if( punkParent )
		{
			sptrINamedDataObject2	sptrN( m_punkObjToMove );
			sptrN->SetParent( punkParent, apfNotifyNamedData );
		}
	}
	::SetValue( m_punkFrom, 0, 0, _variant_t( m_punkObjToMove ) );
	::DeleteObject( m_punkTo, m_punkObjToMove );

	return true;
}

bool CActionObjectMoveBase::DoRedo()
{
	return MoveObjects();
}

//////////////////////////////////////////////////////////////////////
//CActionDataMoveToDoc implementation
bool CActionDataMoveToDoc::Invoke()
{
	m_punkFrom = ::GetAppUnknown();
	m_punkTo = m_punkTarget;

	return CActionObjectMoveBase::Invoke();
}

//////////////////////////////////////////////////////////////////////
//CActionDataMoveToApp implementation
bool CActionDataMoveToApp::Invoke()
{
	m_punkFrom = m_punkTarget;
	m_punkTo = ::GetAppUnknown();

	return CActionObjectMoveBase::Invoke();
}



//////////////////////////////////////////////////////////////////////
//CActionDataOrganizer implementation
CActionDataOrganizer::CActionDataOrganizer()
{
}

CActionDataOrganizer::~CActionDataOrganizer()
{
}

bool CActionDataOrganizer::Invoke()
{
	ASSERT( m_punkTarget );
	ASSERT( CheckInterface( m_punkTarget, IID_IDocumentSite ) );
	ASSERT( CheckInterface( m_punkTarget, IID_INamedData ) );


	COrganizerDlg	dlg( m_punkTarget );
	dlg.DoModal();

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectSave implementation
IUnknown *GetSingleObjectFromDocument( IUnknown *punkDoc )
{
	if( !punkDoc )return 0;
	IUnknown	*punkView = 0;

	IDocumentSitePtr	ptrSite( punkDoc );
	ptrSite->GetActiveView( &punkView );
	if( !punkView )return 0;

	IViewPtr			ptrV( punkView );
	punkView->Release();

	IUnknown	*punkFound = 0;

	TPOS lpos = 0;
	ptrV->GetFirstVisibleObjectPosition(&lpos);

	bool	bContinue = true;
	while (lpos)
	{
		if (punkFound)
		{
			punkFound->Release();
			punkFound = 0;

			return 0;
		}
		ptrV->GetNextVisibleObject(&punkFound, &lpos);

	}

	
/*
	ptrContext->GetObjectTypeCount( &nTypes );

	for( n = 0; n  < nTypes; n++ )
	{
		BSTR	bstr;
		ptrContext->GetObjectType( n, &bstr );



		while( bContinue )
		{
			IUnknown	*punkOldObject = punkFound;
			ptrV->GetNextVisibleObject( bstr, &punkFound );

			bContinue = punkFound != 0;

			if( punkFound && punkOldObject )
			{
				punkFound->Release();
				punkOldObject->Release();

				return 0;
			}

			if( punkOldObject )
			{
				ASSERT( !punkFound );
				punkFound = punkOldObject;
				punkOldObject = 0;
			}
		}

		::SysFreeString( bstr );
	}
*/
	return punkFound;
}

CActionObjectSave::CActionObjectSave()
{
}

CActionObjectSave::~CActionObjectSave()
{
}

bool CActionObjectSave::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	CString	strName = GetArgumentString(_T("ObjectName"));
	m_strPath = GetArgumentString(_T("Path"));
	CString strExt;

	// both names must be valid
	if (ForceNotShowDialog() && (m_strPath.IsEmpty() || strName.IsEmpty() ))
		return false;

	if( strName.IsEmpty() )
	{
		IUnknown	*punk = ::GetSingleObjectFromDocument( m_punkTarget );
		strName = ::GetObjectName( punk );
		m_ptrObj = punk;
		if( punk )punk->Release();
	}

	if( strName.IsEmpty()  )
	{
		if (strName.IsEmpty()) // type of object is undefined
		{
		// get object name 
			IUnknown * punk = 0;

			// get context
			sptrINamedData sptr(m_punkTarget);
			sptr->GetActiveContext(&punk);
			
			if (!punk)
				return false;
			////////////////////////////
			CActivateObjectDlg	dlg(0, pwndParent);
			dlg.m_tree.AttachData( punk, m_punkTarget );	
			punk->Release();

			dlg.SetDlgTitle("Save object");
			dlg.SetBoundTitle("Choose object to save");
			dlg.SetObjectTitle("Available objects");
	
			if (dlg.DoModal() != IDOK)
				return false;

			m_ptrObj = dlg.GetActiveObject();
			if( m_ptrObj == 0 )
				return false;

			strName = ::GetObjectName( m_ptrObj );
		}
	}
	IUnknown	*punk = ::FindObjectByName(m_punkTarget, strName);
	m_ptrObj = punk;
	if( !punk )return false;
	punk->Release();
	
	// get object type and filter
	CString strType = ::GetObjectKind(m_ptrObj);
	
	if( m_strPath.IsEmpty() )
	{
		// try to get filter extention
		BSTR bstrExt = 0, bstrFilter = 0, bstrTemplates = 0;

		/*if (SUCCEEDED(m_ptrFilter->GetFilterParams(&bstrExt, &bstrFilter, &bstrTemplates)))
		{
			strExt = bstrExt;

			::SysFreeString(bstrExt);
			::SysFreeString(bstrFilter);
			::SysFreeString(bstrTemplates);

			if (::GetFileExtention(strName) != strExt)
				m_strPath = strName + strExt;
		}
		else*/
			m_strPath = strName;

		bool bChanged = CorrectFileName( m_strPath.GetBuffer( 0 ) );
		m_strPath.ReleaseBuffer();

		if( !PromtSaveFileName( strType, strName, m_strPath ) )
			return false;
	}

	punk = ::GetFilterByFile(m_strPath);
	m_ptrFilter = punk;
	if (punk)punk->Release();

	if (m_ptrFilter == NULL)
	{
		AfxMessageBox( IDS_NO_FILE_FILTER );
		return false;
	}

	SetArgument(_T("Path"), _variant_t(m_strPath));
	SetArgument(_T("ObjectName"), _variant_t(strName));

	return true;
}


bool CActionObjectSave::Invoke()
{
	_try(CActionObjectSave, Invoke)
	{
		ASSERT(m_punkTarget);
		ASSERT(m_ptrObj != 0 );
		ASSERT(m_ptrFilter != 0);

		// both pointers must be valid
		if (!(m_ptrFilter.GetInterfacePtr() && m_punkTarget && (m_ptrObj!=0)))
			return false;

		m_ptrFilter->AttachNamedData(m_punkTarget, 0);
		m_ptrFilter->AttachNamedObject(m_ptrObj);
		// get file data object 
		sptrIFileDataObject sptr(m_ptrFilter);

		_bstr_t bstrPath(m_strPath);

		sptr->SaveFile(bstrPath);

		// get created objects
		m_ptrFilter->AttachNamedObject(0);
		m_ptrFilter->AttachNamedData(0, 0);
	}
	_catch
	{
		if (m_ptrFilter != NULL)
		{
			m_ptrFilter->AttachNamedObject(0);
			m_ptrFilter->AttachNamedData(0, 0);
		}
		return false;
	}
	return true;
}
/*
//extended UI
bool CActionObjectSave::IsAvaible()
{
	return true;
}
*/
//////////////////////////////////////////////////////////////////////
//CActionObjectLoad implementation
CActionObjectLoad::CActionObjectLoad()
{
}

CActionObjectLoad::~CActionObjectLoad()
{
	m_manObjects.FreeComponents();
}

bool CActionObjectLoad::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	CString	strType = GetArgumentString(_T("Type"));
	CString	strPath = GetArgumentString(_T("Path"));

	// both names must be valid
	if (ForceNotShowDialog() && strPath.IsEmpty())
		return false;

	if (strPath.IsEmpty() || ForceShowDialog())
	{
/*		if (!strType.IsEmpty()) // type of object is undefined
		{
			IUnknown* punk = ::GetFilterByType(strType);
			m_spFilter = punk;
			if (punk)
				punk->Release();

			if (!PromptFileName(strPath, IDS_FILE_OPEN_DLG_TITLE, 0, TRUE, (IUnknown*)m_spFilter.GetInterfacePtr()))
				return false;
		}
		else */if (!PromptFileName(strPath, IDS_FILE_OPEN_DLG_TITLE, 0, TRUE))
			return false;
	}

	IUnknown* punk = ::GetFilterByFile(strPath);
	m_spFilter = punk;
	
	if (punk)
		punk->Release();
	
	if( m_spFilter == 0 )
	{
		AfxMessageBox( IDS_OBJECT_LOAD_NO_FILTER, MB_ICONERROR );
		return false;
	}

	m_strPath = strPath;

	SetArgument(_T("Path"), _variant_t(strPath));
	::SetValue( ::GetAppUnknown(), "\\FileOpenObject", "LoadedObjectPath", m_strPath );

	return true;
}


/*
bool CActionObjectLoad::InsertToDBaseDocument()
{
	
	CString strDBaseActiveObjectName;	
	
	
	IUnknown* punkObject = NULL;
	punkObject = GetActiveObjectFromDBaseDocument( m_punkTarget, szTypeImage );
	if( punkObject == NULL )
		return false;

	IUnknownPtr ptrActiveDBaseObject = punkObject;
	punkObject->Release();

	//Load object to local NamedData

	// both pointers must be valid
	if (!(m_spFilter.GetInterfacePtr() ))
		return false;

	sptrIDBaseDocument sptrDBaseDoc( m_punkTarget );

	m_spFilter->AttachNamedData( m_punkTarget, 0 );
	

	// get file data object 
	sptrIFileDataObject sptr(m_spFilter);

	_bstr_t bstrPath(m_strPath);

	sptr->LoadFile(bstrPath);
	



	// get created objects
	int nCount = 0;
	m_spFilter->GetCreatedObjectCount(&nCount);

	// only one object can be created !!!!!!!!!!
	ASSERT(nCount == 1);

	if( nCount < 1 )
		return false;

	

	IUnknown * punkLoadObject = 0;
	m_spFilter->GetCreatedObject( 0, &punkLoadObject );
	if( punkLoadObject == NULL )
		return false;

	IUnknownPtr ptrLoadObject = punkLoadObject;
	punkLoadObject->Release();

	if( ptrLoadObject == NULL )
		return false;

		
		
	BSTR bstrType;
	m_spFilter->GetObjectType( 0, &bstrType );
	CString strObjectType = bstrType;
	::SysFreeString( bstrType );




	INamedObject2Ptr spNO2( ptrActiveDBaseObject );
	if( spNO2 )
	{
		BSTR bstrDBaseObjectName;
		spNO2->GetName( &bstrDBaseObjectName );
		
		strDBaseActiveObjectName = bstrDBaseObjectName;

		::SysFreeString( bstrDBaseObjectName );
	}


	{


		CMemFile fileObjectLoad;
		CArchive arObjectLoad( &fileObjectLoad, CArchive::store );
		CArchiveStream	streamObjectLoad( &arObjectLoad );

		sptrISerializableObject	spStreamObjectLoad( ptrLoadObject );
		spStreamObjectLoad->Store( &streamObjectLoad );

		fileObjectLoad.SeekToBegin();

		DWORD dwFileLen = fileObjectLoad.GetLength();

		BYTE* pData = new BYTE[dwFileLen];
		fileObjectLoad.Read( pData, dwFileLen );



		m_spFilter->AttachNamedData( 0, 0 );
		m_changes.RemoveFromDocData( m_punkTarget, ptrLoadObject );		
			
		


		CMemFile fileDBObject;
		fileDBObject.Write( pData, dwFileLen );
		fileDBObject.SeekToBegin();
		CArchive arDBObject( &fileDBObject, CArchive::load );
		CArchiveStream	streamDBObject( &arDBObject );

		sptrISerializableObject	spStreamDBObject( ptrActiveDBaseObject );
		spStreamDBObject->Load( &streamDBObject );

		delete pData;

		if( spNO2 )
			spNO2->SetName( _bstr_t((LPCTSTR)strDBaseActiveObjectName) );						

		m_changes.SetToDocData( m_punkTarget, ptrActiveDBaseObject );		

		::UpdateDataObject( m_punkTarget, ptrActiveDBaseObject );


	}



	return true;
}
*/

#include <stdio.h>
bool CActionObjectLoad::Invoke()
{
	int bActivateObject = ( GetArgumentInt( "ActivateObject" ) == 1 );
	
	try
	{		
		ASSERT(m_punkTarget);
		ASSERT(m_spFilter != NULL);
		
		/*
		if( CheckInterface( m_punkTarget, IID_IDBaseDocument )  )
		{
			return InsertToDBaseDocument();
		}
		*/

		// both pointers must be valid
		if (!(m_spFilter.GetInterfacePtr() && m_punkTarget))
			return false;

		INotifyPlacePtr ptrNPDoc( m_punkTarget );

		//m_spFilter->AttachNamedData( m_punkTarget, 0 );

		// get file data object 
		sptrIFileDataObject sptr(m_spFilter);

		_bstr_t bstrPath(m_strPath);

		if (FAILED(sptr->LoadFile(bstrPath)))
		{
			m_spFilter->AttachNamedData(0, 0);
			return false;
		}
		CString	strType = GetArgumentString(_T("Type"));

		// get created objects
		int nCount = 0;
		m_spFilter->GetCreatedObjectCount(&nCount);


		bool bProcessByNotify = false;
		bool bTerminate = false;
		HRESULT hRes;

		if( ptrNPDoc )
		{
			if( nCount > 0 )
			{
				IUnknown * punk = 0;
				m_spFilter->GetCreatedObject( 0, &punk );
				if( punk )
				{
					hRes = ptrNPDoc->NotifyPutToData( punk );
					bProcessByNotify = ( S_OK == hRes );
					bTerminate = ( E_FAIL == hRes );				

					punk->Release();	punk = 0;
				}				
			}
		}

		if( bProcessByNotify )
		{
			m_spFilter->AttachNamedData(0, 0);
			return true;
		}

		if( bTerminate )
		{
			m_spFilter->AttachNamedData(0, 0);
			return false;

		}

		// only one object can be created !!!!!!!!!!
		//ASSERT(nCount == 1);!!!!!!!!!!!!!!!!!!!!!!
		
		for (int i = 0; i < nCount; i++)
		{
			IUnknown * punk = 0;
			m_spFilter->GetCreatedObject(i, &punk);

			if( !punk )continue;

			if (!strType.IsEmpty())
			{
				CString s = GetObjectKind(punk);
				if (strType != s)
				{
					punk->Release();
					continue;
				}
			}

			m_changes.SetToDocData( m_punkTarget, punk, bActivateObject ? UF_ACTIVATE_DEPENDENT : UF_NOT_ACTIVATE_OBJECT );


			if( bActivateObject )
			{
				// get active context
				IDataContext2Ptr sptrContext;

				IDocumentSitePtr sptrDoc = m_punkTarget;
				if (sptrDoc != 0)
				{
					// get active view from document
					IUnknown * punkActiveView = 0;
					sptrDoc->GetActiveView(&punkActiveView);

					// and set it to context
					sptrContext = punkActiveView;
					if (punkActiveView)
					{
						::AjustViewForObject( punkActiveView, punk );
						punkActiveView->Release();
					}
				}
			}

// [Max] 
			CString strName = ::GetObjectName( punk );
			CString strKind = ::GetObjectKind( punk );

			::SetValue( ::GetAppUnknown(), "\\FileOpenObject", "LoadedObjectName", strName );
			::SetValue( ::GetAppUnknown(), "\\FileOpenObject", "LoadedObjectType", strKind );

			::FireEvent( GetAppUnknown(), szEventAfterLoadObject, punk, GetControllingUnknown() );

		INotifyObjectPtr ptrNotifyObj(  punk );
		if( ptrNotifyObj )
			ptrNotifyObj->NotifySelect( true );

			punk->Release();
		}

		m_spFilter->AttachNamedData(0, 0);
	}
	catch( ... )
	{
		if (m_spFilter != NULL)
			m_spFilter->AttachNamedData(0, 0);
		return false;
	}
	return true;
}

bool CActionObjectLoad::DoUndo()
{
/*	ASSERT(m_punkTarget);
	ASSERT(m_manObjects.GetComponentCount());

	if (!m_punkTarget)
		return false;

	bool ret = true;

	for (int i = 0; i < m_manObjects.GetComponentCount(); i++)
		ret = ::DeleteObject(m_punkTarget, m_manObjects.GetComponent(i, false));*/
	
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionObjectLoad::DoRedo()
{
/*	ASSERT(m_punkTarget);
	ASSERT(m_manObjects.GetComponentCount());

	if (!m_punkTarget)
		return false;

	for (int i = 0; i < m_manObjects.GetComponentCount(); i++)
	{
		_variant_t	var(m_manObjects.GetComponent(i, false));
		::SetValue(m_punkTarget, 0, 0, var);
	}*/

	m_changes.DoRedo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectProperties implementation
CActionObjectProperties::CActionObjectProperties()
{
	m_punkObj = 0;
}

CActionObjectProperties::~CActionObjectProperties()
{
}

bool CActionObjectProperties::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	CString	strObject = GetArgumentString(_T("Object"));

	// both names must be valid
	if (ForceNotShowDialog() && strObject.IsEmpty())
		return false;

	if (strObject.IsEmpty() || ForceShowDialog())
	{
		IUnknown * punk = 0;
		sptrINamedData sptr(m_punkTarget);
		sptr->GetActiveContext(&punk);
		
		if (!punk)
			return false;

		CActivateObjectDlg	dlg(punk, pwndParent);

		dlg.SetDlgTitle("Object's properties");
		dlg.SetBoundTitle("Choose object to show it's properties");
		dlg.SetObjectTitle("Available objects");

		if (punk)
			punk->Release();

		if (dlg.DoModal() != IDOK)
			return false;

		m_punkObj = dlg.GetActiveObject();
	}
	else
	{
		m_punkObj = ::FindObjectByName(m_punkTarget, strObject);

		if (!m_punkObj)
			return false;

		m_punkObj->Release();
	}

	SetArgument(_T("Object"), _variant_t(strObject));
	return true;
}

bool CActionObjectProperties::Invoke()
{
	ASSERT(m_punkTarget);
	ASSERT(m_punkObj);

	// both pointers must be valid
	if (!(m_punkObj && m_punkTarget))
		return false;

	return ::ObjectProperties(m_punkObj);
}

//////////////////////////////////////////////////////////////////////
//CActionObjectRename implementation
CActionObjectRename::CActionObjectRename()
{
	m_punkObjToRename = 0;
}

CActionObjectRename::~CActionObjectRename()
{
	if (m_punkObjToRename)
		m_punkObjToRename->Release();
}

bool CActionObjectRename::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	CString	strNew = GetArgumentString(_T("NewName"));
	CString	strOld = GetArgumentString(_T("OldName"));

	// both names must be valid
	if (ForceNotShowDialog() && (strOld.IsEmpty() || strNew.IsEmpty()))
		return false;

	m_punkObjToRename = ::FindObjectByName(m_punkTarget, strOld);

	if (!m_punkObjToRename || strOld.IsEmpty() || strNew.IsEmpty() || ForceShowDialog())
	{
		CRenameObjectDlg	dlg(m_punkTarget, pwndParent);
		dlg.SetOldObjectName(strOld);
		dlg.SetNewObjectName(strNew);

		if (dlg.DoModal() != IDOK)
			return false;

		m_punkObjToRename = dlg.GetRenamedObject();
	
		strNew = dlg.GetNewObjectName();
	}

	if (!m_punkObjToRename)
		return false;

	strOld = ::GetObjectName(m_punkObjToRename);
	m_strOldName = strOld;
	m_strNewName = strNew;

	SetArgument(_T("NewName"), _variant_t(strNew));
	SetArgument(_T("OldName"), _variant_t(strOld));

	return true;
}

bool CActionObjectRename::Invoke()
{
	ASSERT(m_punkTarget);
	ASSERT(m_punkObjToRename);


	if( ::IsDBaseRootObject( m_punkObjToRename, m_punkTarget ) )
		return false;

	// both pointers must be valid
	if (!(m_punkObjToRename && m_punkTarget))
		return false;
	
	sptrINamedData	sptrD(m_punkTarget);
	long	NameExists = 0;
	_bstr_t bstrName(m_strNewName);

	// if new name already exists
	sptrD->NameExists(bstrName, &NameExists);
	if (NameExists)
	{
		CString message, str;
		str.LoadString(IDS_ERR_OBJECTNAME_EXISTS_S);
		message.Format(str, m_strNewName);
		AfxMessageBox(message);
		return false;
	}


	INamedDataObjectPtr ptrNDO( m_punkObjToRename );
	if( ptrNDO )
	{
		ptrNDO->SetModifiedFlag( TRUE );
	}

	
	return ::RenameObject(m_punkTarget, m_punkObjToRename, m_strNewName);
}

bool CActionObjectRename::DoUndo()
{
	ASSERT( m_punkTarget );
	ASSERT( m_punkObjToRename );

	return ::RenameObject(m_punkTarget, m_punkObjToRename, m_strOldName);
}

bool CActionObjectRename::DoRedo()
{
	ASSERT( m_punkTarget );
	ASSERT( m_punkObjToRename );

	return ::RenameObject(m_punkTarget, m_punkObjToRename, m_strNewName);
}

//////////////////////////////////////////////////////////////////////
//CActionObjectDelete implementation
CActionObjectDelete::CActionObjectDelete()
{
}

CActionObjectDelete::~CActionObjectDelete()
{
}

/*bool CActionObjectDelete::ExecuteSettings( CWnd *pwndParent )
{
	if( !m_punkTarget )
		return false;

	CString	strObject = GetArgumentString( _T("Object") );

	if (ForceNotShowDialog() && strObject.IsEmpty())
		return false;

	m_punkObjToDelete = ::FindObjectByName(m_punkTarget, strObject);

	if (!m_punkObjToDelete || strObject.IsEmpty() || ForceShowDialog())
	{
//check ref counter

//		_ReportCounter( punkV );
//		_ReportCounter( sptrS );

		{
			CDeleteObjectDlg	dlg( m_punkTarget, pwndParent );
			dlg.SetSelectedObject( strObject );

			if( dlg.DoModal() != IDOK )
				return false;

			m_punkObjToDelete = dlg.GetSelectedObject();
		}

//		_ReportCounter( punkV );
//		_ReportCounter( sptrS );

		
	}

	if( !m_punkObjToDelete )
		return false;

	strObject = ::GetObjectName( m_punkObjToDelete );

	SetArgument( _T("Object"), _variant_t( strObject ) );

	return true;
}*/

bool CActionObjectDelete::Invoke()
{
	if( !m_punkTarget )
		return false;

	CString str_obj_name = GetArgumentString( "Object" );

	CPtrArray	arr;

	IUnknown	*punk = ::FindObjectByName( m_punkTarget, str_obj_name );
	if( punk )
	{
		arr.Add( punk );
	}
	else
	{
		//paul 25.04.2003 BT 3164, да и вообще тереть все подряд, если имя не корректно...
		if( str_obj_name.GetLength() )
			return false;

		IUnknown	*punkView = 0;
		IDocumentSitePtr	ptrDocSite = m_punkTarget;
		ptrDocSite->GetActiveView( &punkView );

		if( !punkView )
		{
			AfxMessageBox( IDS_NO_ACTIVEVIEW );
			return false;
		}
		IDataContext2Ptr	ptrContext( punkView );
		punkView->Release();

		long	nTypesCount = 0;
		ptrContext->GetObjectTypeCount( &nTypesCount );

		for( long nType = 0; nType < nTypesCount; nType++ )
		{
			BSTR	bstrType;
			ptrContext->GetObjectTypeName( nType, &bstrType );

			LONG_PTR	lpos= 0;
			ptrContext->GetFirstSelectedPos( bstrType, &lpos );

			while( lpos )
			{
				ptrContext->GetNextSelected( bstrType, &lpos, &punk );
				arr.Add( punk );
			}
			::SysFreeString( bstrType );
		}
	}

	for( int i  = 0; i < arr.GetSize(); i++ )
	{
		punk = (IUnknown*)arr[i];
		INotifyObjectPtr ptrNObj( punk );

		if( ptrNObj != 0 && ptrNObj->NotifyDestroy() != S_OK )
		{
			punk->Release();
			continue;
		}
		m_changes.RemoveFromDocData(m_punkTarget, punk);
		punk->Release();
	}
	// Image has changed inside CImage::ExchangeWithParent, invalidate all views excluding
	// active - active view was already invalidated.
	if (arr.GetSize() > 0)
		__RedrawAllViews(m_punkTarget);

	if( !arr.GetSize() )
	{
		AfxMessageBox( IDS_NO_OBJECT_SELECTED );
		return false;
	}
	return true;
}

bool CActionObjectDelete::DoUndo()
{
	ASSERT( m_punkTarget );
//	ASSERT( m_punkObjToDelete );

//	_variant_t	var( m_punkObjToDelete );
//	SetValue( m_punkTarget, 0, 0, var );
	
	m_changes.DoUndo( m_punkTarget );

/*	POSITION pos = m_changes.GetHeadPosition();
	while(pos)
	{
		CObjectUndoRecord *p = m_changes.GetNext(pos);
		if (!p->IsAdded())
		{
			INotifyObjectPtr ptrNObj(p->GetObject());
			if (ptrNObj != 0)
				ptrNObj->NotifySelect(true);
			break;
		}
	}*/
	return true;
}

bool CActionObjectDelete::DoRedo()
{
	ASSERT( m_punkTarget );
//	ASSERT( m_punkObjToDelete );

//	::DeleteObject( m_punkTarget, m_punkObjToDelete );

/*	POSITION pos = m_changes.GetHeadPosition();
	while(pos)
	{
		CObjectUndoRecord *p = m_changes.GetNext(pos);
		if (!p->IsAdded())
		{
			INotifyObjectPtr ptrNObj(p->GetObject());
			if (ptrNObj != 0)
				ptrNObj->NotifyDestroy();
			break;
		}
	}*/
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectSelect implementation
CActionObjectSelect::CActionObjectSelect()
{
	m_sptrObjToActivate = 0;
}

CActionObjectSelect::~CActionObjectSelect()
{
	m_sptrObjToActivate = 0;
}

bool CActionObjectSelect::ExecuteSettings( CWnd *pwndParent )
{
	if (!m_punkTarget)
		return false;

	CString	strObject = GetArgumentString(_T("Object"));

	if (ForceNotShowDialog() && strObject.IsEmpty())
		return false;

	if (strObject.IsEmpty() || ForceShowDialog())
	{
		CActivateObjectDlg	dlg(m_punkTarget, pwndParent);

		if(dlg.DoModal() != IDOK)
			return false;

		m_sptrObjToActivate = dlg.GetActiveObject(false);
		m_strType = dlg.GetObjectKind();
	}
	else
	{
		sptrIView sptrView(m_punkTarget);

		IUnknown * punk;
		if (FAILED(sptrView->GetDocument(&punk)) || !punk)
			return false;

		m_sptrObjToActivate.Attach(::FindObjectByName(punk, strObject), false);
		punk->Release();

		if (m_sptrObjToActivate == 0)
			return false;

		m_strType = ::GetObjectKind(m_sptrObjToActivate);

	}

	SetArgument( _T("Object"), _variant_t( strObject ) );

	return true;
}

bool CActionObjectSelect::Invoke()
{
	int nActivateType = GetArgumentInt( "ActivateType" );

	sptrIDataContext2	sptrC( m_punkTarget );
	sptrIView			sptrV( m_punkTarget );

	_bstr_t	bstrType = m_strType;

	bool bRet = true;
	if (m_sptrObjToActivate == 0)
		return false;

	INotifyObjectPtr ptrNotifyObj(  m_sptrObjToActivate );
	if( ptrNotifyObj )
		ptrNotifyObj->NotifySelect( true );

	bstrType = ::GetObjectKind(m_sptrObjToActivate);

	if( nActivateType == 1 )
	{
		sptrC->SetObjectSelect( m_sptrObjToActivate, TRUE );
		m_sptrObjToActivate = 0;
		return true;
	}
	else if( nActivateType == 2 )
	{
		sptrC->UnselectAll( _bstr_t(bstrType) );
		sptrC->SetObjectSelect( m_sptrObjToActivate, TRUE );
		m_sptrObjToActivate = 0;
		return true;
	}
	

	bRet = SUCCEEDED(sptrC->SetActiveObject(bstrType, m_sptrObjToActivate, aofActivateDepended));	
	::AjustViewForObject( sptrC, m_sptrObjToActivate );

	m_sptrObjToActivate = 0;
//	if( sptrC->SetActiveObject( bstrType, m_sptrObjToActivate ) != S_OK )
//		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionObjectCreate implementation
CActionObjectCreate::CActionObjectCreate()
{
	m_punkCreatedObject = 0;
}

CActionObjectCreate::~CActionObjectCreate()
{
	if( m_punkCreatedObject )
		m_punkCreatedObject->Release();
}

bool CActionObjectCreate::ExecuteSettings( CWnd *pwndParent )
{
	CString strType = GetArgumentString(_T("Type"));
	CString strName = GetArgumentString(_T("Name"));
	_bstr_t bstrName(strName);

	if (!m_punkTarget)
		return false;

	sptrINamedData	sptrD(m_punkTarget);
	long NameExists = 0;
	
	if (!strName.IsEmpty())
		sptrD->NameExists(bstrName, &NameExists);


	if (ForceNotShowDialog() && (strType.IsEmpty() || NameExists))
		return false;

	if (NameExists || strName.IsEmpty() || strType.IsEmpty() || ForceShowDialog())
	{
		CCreateObjectDlg	dlg(m_punkTarget, pwndParent);

		dlg.SetObjectKind(strType);
		dlg.SetObjectName(strName);

		if( dlg.DoModal() != IDOK )
			return false;

		strType = dlg.GetObjectKind();
		strName = dlg.GetObjectName();
	}
	// NOTE : new object's name in CreateDialog always will be checked for existence
	if (strType.IsEmpty()) 
		return false;

	m_bstrType = strType;
	m_bstrName = strName;

	SetArgument(_T("Type"), _variant_t(strType));
	SetArgument(_T("Name"), _variant_t(strName));

	return true;
}

bool CActionObjectCreate::DoUndo()
{
	ASSERT( m_punkTarget );
//	ASSERT( m_punkCreatedObject );

//	::DeleteObject( m_punkTarget, m_punkCreatedObject );

	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionObjectCreate::DoRedo()
{
	ASSERT( m_punkTarget );
//	ASSERT( m_punkCreatedObject );

//	_variant_t	var( m_punkCreatedObject );
//	::SetValue( m_punkTarget, 0, 0, var );

	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionObjectCreate::Invoke()
{
	ASSERT( m_punkTarget );
	ASSERT( CheckInterface( m_punkTarget, IID_INamedData ) );

	// vanek - 03.10.2003
	BOOL bActivateObject = ( GetArgumentInt( "ActivateObject" ) == 1 );

	if (!m_punkTarget)
		return false;


	sptrINamedData	sptrD( m_punkTarget );
	long NameExists = 0;

	if( m_bstrName.length() != 0 )
		sptrD->NameExists(m_bstrName, &NameExists);
	
	if (NameExists)
	{
		AfxMessageBox( IDS_NAME_EXIST );
		return false;
	}

	m_punkCreatedObject = ::CreateTypedObject( m_bstrType );

	if (!m_punkCreatedObject)
	{
		AfxMessageBox( IDS_ERROR_CREATE );
		return false;
	}

	//assign object name, if exist
	if( m_bstrName.length() != 0 )
	{
		INamedObject2Ptr	sptrN( m_punkCreatedObject );
		sptrN->SetName( m_bstrName );
	}

	INotifyObjectPtr ptrNotifyObj(  m_punkCreatedObject );
	if( ptrNotifyObj )
	{
		if( ptrNotifyObj->NotifyCreate( ) != S_OK)
		{
			m_punkCreatedObject->Release();
			m_punkCreatedObject = NULL;
			return false;
		}
	}



	int	cx = ::GetValueInt( GetAppUnknown(), "\\Information", "LastImageCX", 400 );
	int	cy = ::GetValueInt( GetAppUnknown(), "\\Information", "LastImageCY", 300 );


	if( CheckInterface( m_punkCreatedObject, IID_IImage ) )
	{
		CImageWrp	image( m_punkCreatedObject );
		if( !image.CreateNew( cx, cy, "RGB" ) )
			return false;
	}
	if( CheckInterface( m_punkCreatedObject, IID_IBinaryImage ) )
	{
		IBinaryImagePtr	ptrBinary( m_punkCreatedObject );
		if( ptrBinary->CreateNew( cx, cy ) != S_OK )
			return false;
	}

//	_variant_t	var( m_punkCreatedObject );

	
//place object to the name data
	m_changes.SetToDocData(sptrD, m_punkCreatedObject, bActivateObject ? 0 : UF_NOT_ACTIVATE_OBJECT );	// vanek BT

	// vanek - 03.10.2003
	if( bActivateObject )
	{	//activate it
		IUnknown	*punkView = 0;
		IDocumentSitePtr	ptrDocSite = sptrD;
		ptrDocSite->GetActiveView( &punkView );

		if( punkView )
		{
			::AjustViewForObject( punkView, m_punkCreatedObject );
			punkView->Release();
		}

		if( ptrNotifyObj )
			ptrNotifyObj->NotifySelect( true );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions for open/save dialog with filter


// get path and file name for selected filter
//bool PromptFileName(CString& fileName, LPCTSTR szTitle, DWORD lFlags, BOOL bOpenFileDialog, IUnknown *punkFilter)
bool PromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, IUnknown *punkFilter)
{
	CString strTitle;
	VERIFY(strTitle.LoadString(nIDSTitle));

	CWinApp* pApp = ::AfxGetApp();
	// get last path
	/*CString	strPath = pApp->GetProfileString("general", "LastOpenPath");

	CString	strExt = pApp->GetProfileString("general", "LastLoadExt");

	if (!strPath.IsEmpty()) 
		::SetCurrentDirectory(strPath);*/

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	lFlags &= ~OFN_ALLOWMULTISELECT;
	ofn.Flags |= lFlags | OFN_ENABLESIZING;

	CString strTemplName, strDefault, strFilterString;
	sptrIFileFilter2 sptrFilter = NULL;

	// get filter and template names
	if (CheckInterface(punkFilter, IID_IFileFilter2))
	{
		CString strExt, strFilter;
		BSTR bstrExt = 0, bstrFilter = 0, bstrTemplName = 0;
	
		sptrFilter = punkFilter;

		if (SUCCEEDED(sptrFilter->GetFilterParams(&bstrExt, &bstrFilter, &bstrTemplName)))
		{		
			strExt = bstrExt;
			strFilter = bstrFilter;
			strTemplName = bstrTemplName;

			::SysFreeString(bstrExt);
			::SysFreeString(bstrFilter);
			::SysFreeString(bstrTemplName);

			// add this filter first 
			if( IsGoodExt( strExt, bOpenFileDialog ) )
				_AppendFileSuffix(strFilterString, strFilter, strExt);
		}
	}
	
	IUnknown *punk = ::GetAppUnknown();
	sptrIApplication sptrApp(punk);
	if (punk)
		punk->Release();

	// do for all doc template
	LONG_PTR nPos = 0;
	sptrApp->GetFirstDocTemplPosition(&nPos);

	while (nPos)
	{
		BSTR bstrName = 0;
		IUnknown *punk = 0;
		sptrApp->GetNextDocTempl(&nPos, &bstrName, &punk);

		// get template name
		CString	strTemplate = bstrName;
		::SysFreeString(bstrName);
		// A.M., SBT 1336
		sptrIDocTemplate sptrDT(punk);
		BSTR bstr = NULL;
		HRESULT hr = sptrDT->GetDocTemplString(-1, &bstr);
		CString sDocString(bstr);
		::SysFreeString(bstr);
		CString	strFilterName;
		AfxExtractSubString(strFilterName, sDocString, CDocTemplate::filterName);

		if (punk)
			punk->Release();

		// append filter extension for all filters with exclude selected filter
		if (strTemplate != strTemplName || sptrFilter == NULL)
		{
			// load filters for template
			CCompManager manFilters(strTemplate + CString("\\") + CString(szPluginExportImport), sptrApp);
			// and append suffixes
			AppendFilterSuffix(strFilterString, &ofn, NULL, &manFilters, bOpenFileDialog == TRUE, strFilterName/*strTemplate*/);
		}
	}

	if( ::GetValueInt( ::GetAppUnknown(), "\\FileOpen", "UseShowAllFilter", 1 ) != 0 )
	{
		// append the "*.*" all files filter
		CString allFilter;
		VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));

		// add last filter
		strFilterString += allFilter;

		strFilterString += (TCHAR)'\0';   // next string please
		strFilterString += _T("*.*");
		strFilterString += (TCHAR)'\0';   // last string
		ofn.nMaxCustFilter++;
	}

	ofn.lpstrFilter = strFilterString;
	ofn.lpstrTitle = strTitle;
	ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
	ofn.nMaxFile = _MAX_PATH;


	// do dialog
	BOOL res;

	res = ExecuteFileDialog(bOpenFileDialog, ofn, szObjectFileOpen );

	fileName.ReleaseBuffer();

	//char	szPath[_MAX_PATH];
	//::GetCurrentDirectory(_MAX_PATH, szPath);

	// set latest path to application
	//pApp->WriteProfileString("general", "LastOpenPath", szPath);

	//pApp->WriteProfileString("general", "LastLoadExt", strExt);

	return res == TRUE;
}


//append all avaible extension for given document template, including file filters
static void AppendFilterSuffix(CString &strFilter, OPENFILENAME *pofn, CString *pstrDefault, CCompManager * pmanFilters, bool bOpenFile, CString strTemplateName)//, IDocTemplate* punkTemplate)
{
	CString	strExt;
	CString	strName;
	if (!pmanFilters)
		return;

	strTemplateName.MakeLower();
	
	if(bOpenFile)
	{
		//if( !strName.IsEmpty() && !strExt.IsEmpty() )
		{
			//make common filter fo template
			long nCount = pmanFilters->GetComponentCount();

			if(nCount)
			{
				CString strFilterName;
				CString strFilterExt;

				strFilterName.Format(IDS_ALLFILTER_NAME_FORMAT, (LPCTSTR)strTemplateName);
//				strFilterName = "All " + strTemplateName + " files (";
				//strFilterExt = '*' + strExt + ';';

				for(long i = 0; i < nCount; i++ )
				{
					IUnknown *punk = pmanFilters->GetComponent( i );
					sptrIFileFilter		spFilter( punk );
			
					punk->Release();
			
					BSTR	bstrFilter, bstrExt;
			
					spFilter->GetFilterParams( &bstrExt, &bstrFilter, 0 );
			
					strExt = bstrExt;
					strName = bstrFilter;
			
					::SysFreeString( bstrExt );
					::SysFreeString( bstrFilter );

					int	idx = 0;
					CString	strAll = strExt;
					while( idx != -1 )
					{
						idx = strAll.Find( "\n" );
						CString	strExt;
						if( idx == -1 )	 
							strExt = strAll;
						else
						{
							strExt = strAll.Left( idx );
							strAll = strAll.Right( strAll.GetLength()-1-idx );
						}

						if( IsGoodExt( strExt, bOpenFile, _T("ObjectLoad") ) )
							strFilterExt += '*' + strExt + ';';
					}
				}
			
				if( strFilterExt.GetLength() )
					strFilterExt.Delete(strFilterExt.GetLength()-1);
				
				if( strFilterExt.Find(";") != -1 )
				{
					strFilterName += strFilterExt + ")" + '\0' + strFilterExt + '\0'; 
					
					if( !strFilterExt.IsEmpty() )
					{
						//strFilter.Insert(0, strFilterName); don't work correctly 
						CString str = strFilter;
						strFilter = strFilterName;
						strFilter += str;

					
						if( pofn )
							pofn->nMaxCustFilter++;
					}
				}
			}
		}
	}



	if (pstrDefault)
		*pstrDefault = strExt;

	for (int i = 0; i < pmanFilters->GetComponentCount(); i++)
	{
		IUnknown *punk = pmanFilters->GetComponent(i);
		sptrIFileFilter		spFilter(punk);

		if (punk)
			punk->Release();

		BSTR	bstrFilter, bstrExt;

		spFilter->GetFilterParams(&bstrExt, &bstrFilter, 0);

		strExt = bstrExt;
		strName = bstrFilter;

		::SysFreeString(bstrExt);
		::SysFreeString(bstrFilter);

		int	idx = 0;
		CString	strAll = strExt;
		CString	strFilterExt = "";
		while( idx != -1 )
		{
			idx = strAll.Find( "\n" );
			CString	strExt;
			if( idx == -1 )	 
				strExt = strAll;
			else
			{
				strExt = strAll.Left( idx );
				strAll = strAll.Right( strAll.GetLength()-1-idx );
			}
			if( IsGoodExt( strExt, bOpenFile, _T("ObjectLoad") ) )
				strFilterExt += '*' + strExt + ';';
		}
		strFilterExt.Delete(0);	

		if( strFilterExt.GetLength() )
			strFilterExt.Delete(strFilterExt.GetLength()-1);	
		
		if( strFilterExt.Find(";") != -1 || IsGoodExt( strFilterExt, bOpenFile ) )
		{
			if( !strFilterExt.IsEmpty() )
			{
				_AppendFileSuffix(strFilter, strName, strFilterExt);

				if (pofn)
					pofn->nMaxCustFilter++;
			}
		}
	}
}

//append file suffix helper
static void _AppendFileSuffix(CString &strFilter, LPCTSTR szName, LPCTSTR szExt)
{
	CString	strExt = szExt;
	strExt.Replace( "\n", ";" );
	strFilter += szName;
	strFilter += " (*";
	strFilter += strExt;
	strFilter += ")";
	strFilter += '\0';
	strFilter += '*';
	strFilter += strExt;
	strFilter += '\0';

}

bool PromtSaveFileName( const char *pszType, const char *pszObjectName, CString &strReturnPath )
{
	IApplicationPtr	ptrApp( GetAppUnknown() );

	LONG_PTR	lPosTempl = 0;
    
	CMapStringToPtr	filters;

	CString	strFilterAll;
	int		nFilterCount = 0;

	ptrApp->GetFirstDocTemplPosition( &lPosTempl );
	while( lPosTempl )
	{
		BSTR	bstrDocTempl = 0;
		ptrApp->GetNextDocTempl( &lPosTempl, &bstrDocTempl, 0 );

		CString	str( bstrDocTempl );
		::SysFreeString( bstrDocTempl );

		CCompManager	r( str+"\\export-import" );

		for( int i = 0; i <r.GetComponentCount(); i++ )
		{
			IFileFilter2Ptr	ptrFilter( r.GetComponent( i, false ) );

			int	nFilterMaxCount = 0;
			ptrFilter->GetObjectTypesCount( &nFilterMaxCount );

			for( int j = 0; j < nFilterMaxCount; j++ )
			{
				BSTR	bstrType;
				ptrFilter->GetObjectType( j, &bstrType );
				CString	strType( bstrType );
				::SysFreeString( bstrType );

				if( strType != pszType )
					continue;

				BSTR	bstrFilterName;
				BSTR	bstrFilterExt;
				ptrFilter->GetFilterParams( &bstrFilterExt, &bstrFilterName, 0 );
				CString	strFName( bstrFilterName );
				CString	strFExt( bstrFilterExt );

				int nPos = strFExt.Find(10, 0);
				if(nPos >= 0)
					strFExt = strFExt.Left(nPos);

				
				::SysFreeString( bstrFilterName );
				::SysFreeString( bstrFilterExt );

				void	*pdummy;
				if( filters.Lookup( strFName, pdummy ) )
					continue;

				filters[strFName] = 0;

				if( IsGoodExt( strFExt, false) )
				{
					_AppendFileSuffix( strFilterAll, strFName, strFExt );
					nFilterCount++;
				}
			}
		}
	}

	if( !nFilterCount )
	{
		AfxMessageBox( IDS_NO_FILTER );
		return false;
	}
//2.
	CString strTitle;
	VERIFY(strTitle.LoadString(IDS_FILE_SAVE_DLG_TITLE));

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.Flags |= OFN_ENABLESIZING;

	ofn.nMaxCustFilter = nFilterCount;
	ofn.lpstrFilter = strFilterAll;
	ofn.lpstrTitle = strTitle;
	ofn.lpstrFile = strReturnPath.GetBuffer(_MAX_PATH);
	ofn.nMaxFile = _MAX_PATH;

	// do dialog
	BOOL res;

	res = ExecuteFileDialog( false, ofn, szObjectFileOpen );

	strReturnPath.ReleaseBuffer();

	return res == TRUE;
}



BEGIN_INTERFACE_MAP(CActionCreateImageObjectByView, CActionBase)
	INTERFACE_PART(CActionCreateImageObjectByView, IID_ILongOperation, Long)
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////
//CActionCreateImageObjectByView implementation
CActionCreateImageObjectByView::CActionCreateImageObjectByView()
{
}

//////////////////////////////////////////////////////////////////////
CActionCreateImageObjectByView::~CActionCreateImageObjectByView()
{
}
#define BIG_LEN	10000000
//////////////////////////////////////////////////////////////////////
bool CActionCreateImageObjectByView::Invoke()
{
	IPrintViewPtr ptrV( GetActivePrintView() );
	if( ptrV == NULL )
		return false;

	CString	strImageName = GetArgumentString( _T("ImageName") );

	IScrollZoomSitePtr ptrZ( ptrV );
	if( ptrZ == NULL )
		return false;

	ptrZ->SetZoom( 1. );	
	ptrZ->SetScrollPos( CPoint( 0, 0 ) );
	
	

	int nUserPosDX, nUserPosDY; 
	int nReturnWidth, nReturnHeight;

	nUserPosDX = nUserPosDY = 0;
	nReturnWidth = nReturnHeight = 0;

	BOOL bContinue = FALSE;

	ptrV->GetPrintWidth( BIG_LEN, &nReturnWidth, &bContinue, 0, &nUserPosDX );
	ASSERT( !bContinue );

	bContinue = FALSE;
	ptrV->GetPrintHeight( BIG_LEN, &nReturnHeight, &bContinue, 0, &nUserPosDY );
	ASSERT( !bContinue );

				/*
	nReturnWidth = (int)( (double)(nReturnWidth) / fZoom );
	nReturnHeight = (int)( (double)(nReturnHeight) / fZoom );		

	nUserPosDX = (int)( (double)(nUserPosDX) / fZoom );
	nUserPosDY = (int)( (double)(nUserPosDY) / fZoom );
	*/
	
	

	if( nReturnWidth < 1 || nReturnHeight < 1 )
		return false;

	CClientDC dc( 0 );
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );

	BITMAPINFOHEADER bmih;
	::ZeroMemory( &bmih, sizeof( bmih )  );
	bmih.biBitCount = 24;
	bmih.biHeight = nReturnHeight;
	bmih.biWidth = nReturnWidth;
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;

	BYTE* pDibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( 
						memDC.m_hDC, 
						(BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pDibBits, 0, 0 );

	if( pDibBits == NULL || hDIBSection == NULL )	
		return false;

	CBitmap *pSrcBitmap = CBitmap::FromHandle( hDIBSection  );
	CBitmap *pOldBmp = memDC.SelectObject( pSrcBitmap );

	memDC.FillSolidRect(CRect( 0, 0, nReturnWidth, nReturnHeight ), GetSysColor( COLOR_WINDOW ));

	{
		IViewPtr	sptrView( ptrV );
		IUnknownPtr	ptrFrame = 0;
		if(ptrV)
		{ // по возможности уберем выделение с вьюхи
			sptrView->GetMultiFrame( &ptrFrame, false );
			IMultiSelectionPtr	sptrFrame( ptrFrame );
			if(sptrFrame)
				sptrFrame->EmptyFrame();
		}
	}

	ptrV->Print(	
					memDC.m_hDC, 
					CRect( 0, 0, nReturnWidth, nReturnHeight ), 
					0, 0,
					nUserPosDX, nUserPosDY,
					FALSE, 1 );

	CImageWrp image;
	if( !image.CreateNew( nReturnWidth, nReturnHeight, "RGB" ) )
		return false;

	try
	{

		StartNotification( nReturnHeight );
		
		int cx4 = ( nReturnWidth * 3 + 3 ) / 4 * 4;
		for( int y = 0; y < nReturnHeight; y++ )
		{		
			for( int nPane=0;nPane<3;nPane++ )
			{
				int nOffset = nPane;

				if( nPane == 0 )
					nOffset = 2;
				else
				if( nPane == 2 )
					nOffset = 0;

				color* pColor = image.GetRow( y, nPane );
				for( int x = 0; x < nReturnWidth; x++ )
				{

					byte	*pRGB = pDibBits + ( nReturnHeight - y - 1 ) * cx4 + x * 3 + nOffset;
					pColor[ x ] = (*pRGB) * colorMax / 255;
				}
			}
			Notify( y );
		}
	}
	catch(...)
	{
		memDC.SelectObject( pOldBmp );		
		if( hDIBSection )
			::DeleteObject(hDIBSection);	hDIBSection = 0;
		return false;
	}

	memDC.SelectObject( pOldBmp );
	
	::DeleteObject(hDIBSection);	hDIBSection = 0;


	m_changes.SetToDocData( m_punkTarget, image );
	

	FinishNotification();

	

	return true;
}

//////////////////////////////////////////////////////////////////////
//undo/redo
bool CActionCreateImageObjectByView::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionCreateImageObjectByView::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//////////////////////////////////////////////////////////////////////
//extended UI
bool CActionCreateImageObjectByView::IsAvaible()
{
	IPrintViewPtr ptrV( GetActivePrintView() );
	
	return ptrV != NULL; 
}

//////////////////////////////////////////////////////////////////////
bool CActionCreateImageObjectByView::IsChecked()
{
	return false;
}

//////////////////////////////////////////////////////////////////////
IPrintViewPtr CActionCreateImageObjectByView::GetActivePrintView()
{
	sptrIDocumentSite	sptrS( m_punkTarget );
	IUnknown* punkV = 0;
	sptrS->GetActiveView( &punkV );
	if( !punkV )
		return NULL;

	IPrintViewPtr ptrV( punkV );
	punkV->Release();	punkV = 0;

	return ptrV;
}

//////////////////////////////////////////////////////////////////////
//CActionClipboardAvail implementation
CActionClipboardAvail::CActionClipboardAvail()
{
	
}

CActionClipboardAvail::~CActionClipboardAvail()
{

}

bool CActionClipboardAvail::Invoke()
{
	return false;
}


//extended UI
bool CActionClipboardAvail::IsAvaible()
{
	if(::IsClipboardFormatAvailable(::GetClipboardFormat()))
		return true;


	POSITION pos = ::GetFirstClipboardFormatPos();
	while(pos)
	{
		UINT nFormat = 0;
		CString strType = "";
		::GetClipboardFormatByPos(pos, nFormat, strType);
		if(::IsClipboardFormatAvailable(nFormat))
			return true;
	}




	/*UINT nFormat = EnumClipboardFormats(0);
	while(nFormat)
	{
		if(!GetObjectTypeByClipboardFormat(nFormat).IsEmpty())
			return true;

		nFormat = EnumClipboardFormats(nFormat); 
	}
	*/

	return false;
}


/****************************************************************************************************/
IMPLEMENT_DYNCREATE(CActionDataToExcelAvail,	CCmdTargetEx);

// {79BBFB7A-0864-4687-984D-D0D936E80CA4}
GUARD_IMPLEMENT_OLECREATE(CActionDataToExcelAvail, "Data.DataToExcelAvail", 
0x79bbfb7a, 0x864, 0x4687, 0x98, 0x4d, 0xd0, 0xd9, 0x36, 0xe8, 0xc, 0xa4);
// Action info
// {18F710EA-AB97-472e-89BF-03CF0C691DAC}
static const GUID guidDataToExcelAvail = 
{ 0x18f710ea, 0xab97, 0x472e, { 0x89, 0xbf, 0x3, 0xcf, 0xc, 0x69, 0x1d, 0xac } };


ACTION_INFO_FULL(CActionDataToExcelAvail, IDS_ACTION_DATA_TO_EXCEL_AVAIL, -1, -1, guidDataToExcelAvail );
ACTION_TARGET(CActionDataToExcelAvail, szTargetViewSite );

CActionDataToExcelAvail::CActionDataToExcelAvail()
{
	
}

CActionDataToExcelAvail::~CActionDataToExcelAvail()
{

}

bool CActionDataToExcelAvail::Invoke()
{
	return false;
}

bool CActionDataToExcelAvail::IsAvaible()
{
	IUnknown *ptrDocList = 0;
	ptrDocList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );

	if( ptrDocList )
	{
		INamedDataObject2Ptr ptrObjList = ptrDocList;
		ptrDocList->Release(); ptrDocList = 0;
		
		long lCount = 0;
		ptrObjList->GetChildsCount( &lCount );

		if( lCount > 0 )
			return true;
	}
	return false;
}
/****************************************************************************************************/


//////////////////////////////////////////////////////////////////////
//CActionTouchObject implementation
CActionTouchObject::CActionTouchObject()
{
}

CActionTouchObject::~CActionTouchObject()
{
}

bool CActionTouchObject::Invoke()
{
	CString	strObject = GetArgumentString(_T("Object"));
	IUnknownPtr punk(::FindObjectByName(m_punkTarget, strObject), false);
	if (punk != 0)
	{
//		INumeredObject2Ptr sptrNO2(punk);
//		if (sptrNO2 != 0)
//			sptrNO2->RenewCreateNum();
		INamedDataObject2Ptr sptrNDO(punk);
		DWORD dwFlags = 0;
		if (sptrNDO != 0)
			sptrNDO->GetObjectFlags(&dwFlags);
		if (dwFlags&nofCantSendTo)
			return false;
		IUnknown *punkNew = ::CloneObject(punk);
		if (punkNew)
		{
			CDisableAutoSelect DisableAutoSelect;
			m_changes.RemoveFromDocData( m_punkTarget, punk, false );
			m_changes.SetToDocData( m_punkTarget, punkNew );
			punkNew->Release();
		}
		return true;
	}
	return false;

/*	sptrIDocumentSite	sptrS( m_punkTarget );
	IUnknown* punkV = 0;
	sptrS->GetActiveView( &punkV );
	if( !punkV )return false;
	sptrIView	sptrV( punkV );
	punkV->Release();
	if(sptrV)
	{
		IUnknown* punkMF = 0;
		sptrV->GetMultiFrame(&punkMF, false);
		sptrIMultiSelection sptrMF(punkMF);
		if(punkMF)
			punkMF->Release();
		if(sptrMF)
		{
			//store objects to list
			CPtrArray	arrToDelete;
			DWORD nObjects = 0;	
			sptrMF->GetObjectsCount(&nObjects);

			arrToDelete.SetSize( nObjects );

			for(int i = nObjects-1; i >= 0; i--)
			{
				IUnknown* punk = 0;
				sptrMF->GetObjectByIdx(i, &punk);				

				INotifyObjectPtr ptrNObj( punk );
				if( ptrNObj != 0 && ptrNObj->NotifyDestroy() != S_OK )
				{
					arrToDelete[i] = 0;
					punk->Release();
				}
				else
				{
					arrToDelete[i] = punk;
				}
			}

			sptrMF->EmptyFrame();

			for(i = nObjects-1; i >= 0; i--)
			{
				IUnknown	*punk = (IUnknown*)arrToDelete[i];
				if( punk == 0 )
					continue;

				m_changes.RemoveFromDocData( m_punkTarget, punk, false );	
				m_changes.SetToDocData( m_punkTarget, punk );	
				punk->Release();
			}
			// Image has changed inside CImage::ExchangeWithParent, invalidate all views excluding
			// active - active view was already invalidated.
			if (nObjects > 0)
				__RedrawAllViews(m_punkTarget);
			
			return true;
		}
	}
	return false;*/
}

//undo/redo
bool CActionTouchObject::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionTouchObject::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

