#ifndef __auditint_h__
#define __auditint_h__

#include "defs.h"

interface IAuditTrailObject : public IUnknown
{
	com_call CreateThumbnail(IUnknown* punkDoc, IUnknown* punkImage, IUnknown* punkAction, BSTR bstrParams, BOOL bRender) = 0;
	com_call GetDocumentSize(SIZE* psizeDoc) = 0;
	com_call Draw(BITMAPINFOHEADER *pbi, BYTE *pdibBits, RECT rectDest) = 0;
	com_call GetThumbnailFromPoint(POINT ptCoord, long* pnIndex) = 0;
	com_call SetActiveThumbnail(long nIndex) = 0;
	com_call GetActiveThumbnail(long* pnIndex) = 0;
	com_call GetThumbnailRect(long nIndex, RECT* prcBound) = 0;
	com_call UndoAction(GuidKey* pKeyAction, GuidKey* pKeyImage) = 0;
	com_call SetFlags(DWORD dwFlags) = 0;
	com_call GetThumbnailsCount(long* pnCount) = 0;
	com_call GetThumbnailInfo(long nIdx, BSTR* pbstrActionName, BSTR* pbstrActionParams, BSTR* pbstrActionTime) = 0;	
	//grid data
	com_call SetGridData( BYTE* pBuf, long lSize ) = 0;
	com_call GetGridData( BYTE** ppBuf, long* plSize ) = 0;
	com_call ReloadSettings( ) = 0;

};


interface IAuditTrailMan : public IUnknown
{
	com_call ToggleOnOffFlag() = 0;
	com_call GetCreateFlag(BOOL* pbWillCreate) = 0;
	com_call GetAuditTrail(IUnknown* punkImage, IUnknown** ppunkAT) = 0;
	com_call RescanActionsList() = 0;
};

enum AuditTrailDrawFlags
{
	atdfVert = 1,			//vertical view (thumbnails)
	atdfHorz = 1<<1,		//horizontal view (thumbnails)
	//if niether vertical nor horizontal flag set, than draw single object view with original DIB size
};

declare_interface( IAuditTrailMan, "BDD0499B-438F-41fd-BEED-F4F0129358E9");
declare_interface( IAuditTrailObject, "6C589D28-7D44-4fde-9A7C-84A0237FD45A");

#endif //__auditint_h__