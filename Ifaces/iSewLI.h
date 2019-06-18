#ifndef __iSewLI_h__
#define __iSewLI_h__

#include "\vt5\awin\misc_list.h"

//////////////////////////////////////////////////////////////////////
interface IInteractiveSewLIAction : public IUnknown
{
};

interface ISewImageList;
//interface ITransparencyMap;
interface IDistanceMap;
interface ISewFragment : public IUnknown
{
	com_call GetSize(SIZE *psz) = 0;
	com_call SetSize(SIZE sz) = 0;
	com_call GetOffset(POINT *ptOffset) = 0;
	com_call SetOffset(POINT ptOffs) = 0;
	com_call GetImage(int *pnZoom, IUnknown **ppunkImage) = 0;
	com_call SetImage(IUnknown *punkImage) = 0;
//	com_call GetTransparencyMap(int *pnZoom, ITransparencyMap **ppTranspMap) = 0;
	com_call GetDistanceMap(int *pnZoom, IDistanceMap **ppDistanceMap) = 0;
//	com_call RebuildTransparencyMaps() = 0;
	com_call SetFragNum(long lNum) = 0;
	com_call GetFragNum(long *plNum) = 0;
	com_call LoadImage(int nZoom) = 0;
	com_call FreeImage(int nZoom) = 0;
	com_call SetListPtr(ISewImageList *pList) = 0;
	com_call RestoreImages() = 0;
};

enum EAddImageFlags
{
	addImageFindPos = 1,
};

enum EMoveFragmentFlags
{
	moveFragCalcOvr = 1,
	moveFragFire = 2,
	moveFragCorrect = 4,
};

interface ISewImageList : public IUnknown
{
	com_call OnActivateObject() = 0;
	com_call GetFirstFragmentPos(long *plPos) = 0;
	com_call GetNextFragment(long *plPos, ISewFragment **ppFragment) = 0;
	com_call GetPrevFragment(long *plPos, ISewFragment **ppFragment) = 0;
	com_call GetFragmentsCount(long *plCount) = 0;
	com_call GetOrigin(POINT *pOrg) = 0;
	com_call GetSize(SIZE *pSize) = 0;
	com_call GetPath(BSTR *pbstrPath, BOOL bSaving) = 0;
	com_call GetRangeCoef(int *pnRangeCoef) = 0;
	com_call AddImage(IUnknown *punkImage, POINT ptOffs, DWORD dwFlags) = 0;
	com_call GetActiveFragmentPosition(long *plPos) = 0;
	com_call SetActiveFragmentPosition(long lPos) = 0;
	com_call GetLastFragmentPosition(long *plPos) = 0;
	com_call AddFragment(ISewFragment *pFragment) = 0;
	com_call MoveFragment(ISewFragment *pFragment, POINT ptNewPos, DWORD dwFlags) = 0;
	com_call DeleteFragment(long lPosFrag) = 0;
	com_call GetOverlapPercent(int *pnOvrPerc) = 0;
	com_call CheckOverlapPercent() = 0;
	com_call CalcOrigFragOffset(ISewFragment **ppPrev, POINT *pptOffs) = 0;
	com_call IsSavingOrLoading(BOOL *pbSaving, BOOL *pbLoading) = 0;
	com_call GetTotalImage(IUnknown **ppunk) = 0;
	com_call GetFragmentsFormat(BSTR *pbstrFragFmt) = 0;
};

enum SewImageListEvents
{
	silevAddImage = 0,
	silevMoveImage = 1,
	silevDeleteImage = 2,
	silevActiveChanged = 3,
	silevImageChanged = 4,
};

declare_interface( IInteractiveSewLIAction, "907D2030-8652-4875-B38B-050D7EA892CA" );
declare_interface( ISewFragment, "F46130D0-DBCE-454d-AE9A-C853439D155E" );
declare_interface( ISewImageList, "A6179CAD-3DA2-4a72-99D4-A24B397A9E90" );

#endif//__iSewLI_h__
