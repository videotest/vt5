#ifndef __CLASSES5_H__
#define __CLASSES5_H__

#import <measure.tlb> exclude ("IUnknown", "GUID", "_GUID", "tagPOINT") no_namespace raw_interfaces_only named_guids 

//#include "defs.h"
//#include "misc5.h"

/*
// moveParam and insertParam has LPOS lPosAfter variable that can be 0 (it's means param lists's head) || -1 (it's means param lists's tail)
interface IMeasParamGroup2 : public IMeasParamGroup
{
	com_call GetLastPos(TPOS *plPos) = 0;
	com_call GetPrevPos(TPOS *plPos) = 0;
//	com_call AddParam(long lKey, BSTR bstrName, BSTR bstrFormat, DWORD dwFlag) = 0;
	com_call InsertParam(long lKey, BSTR bstrName, BSTR bstrFormat, DWORD dwFlag, DWORD dwType, BSTR bstrAction, LPOS lPosAfter) = 0;
	com_call RemoveParam(LPOS lPos) = 0;
//	com_call RemoveParamByKey(long lKey) = 0;
	com_call MoveParam(LPOS lPos, LPOS lPosAfter) = 0;

	com_call GetParamInfo2(LPOS lPos, long *plKey, BSTR *pbstrName, BSTR *pbstrFormat, DWORD *pdwFlag, DWORD * pdwType, BSTR * pbstrActionName) = 0;
	com_call SetParamInfo2(LPOS lPos, long *plKey, BSTR *pbstrName, BSTR *pbstrFormat, DWORD *pdwFlag, DWORD * pdwType, BSTR * pbstrActionName) = 0;

//	com_call IsManualParam(LPOS lPos, BOOL *bFlag) = 0;
//	com_call SetManualParam(LPOS lPos, BOOL bFlag) = 0;

//	com_call GetParamType(LPOS lPos, DWORD * pdwType) = 0;
//	com_call SetParamType(LPOS lPos, DWORD dwType) = 0;

//	com_call GetParamAction(LPOS lPos, BSTR * pbstrActionName) = 0;
//	com_call SetParamAction(LPOS lPos, BSTR bstrActionName) = 0;

	com_call GetActiveParamPos(TPOS *plPos) = 0;
	com_call SetActiveParamPos(LPOS lPos) = 0;

	com_call RestoreState() = 0;
	com_call SaveState() = 0;
};

interface IParamGroupManager : public IUnknown
{
	com_call ExcludeGroup(TPOS *plPos) = 0;
	com_call ExcludeGroup(IUnknown* punkGroup) = 0;
	com_call IncludeGroup(IUnknown* punkGroup, TPOS *plPos) = 0;

	com_call GetFirstPos(TPOS *plPos) = 0;
	com_call GetNextGroup(TPOS *plPos, IUnknown **ppunk) = 0;
	com_call GetGroupPos(IUnknown* punkGroup, TPOS *plPos) = 0; //???
	com_call GetGroupCount(int *pnCount) = 0;

	com_call SaveState(IUnknown* punkNamedData) = 0;
	com_call LoadState(IUnknown* punkNamedData) = 0;

	com_call SetCalcImageToGroups(IUnknown* punkImage) = 0;
};

interface IGridView : public IUnknown
{
	com_call GetActiveCell(long *plCol, long *plRow) = 0;
	com_call SetActiveCell(long lCol, long lRow) = 0;
};

interface IClassView : public IUnknown
{
	com_call GetObjectList(IUnknown ** ppunkList) = 0;
	com_call GetActiveObject(IUnknown ** ppunkObject) = 0;
	com_call SetActiveObject(IUnknown * punkObject) = 0;

};

declare_interface(IClassView,			"426DE2AE-A0CC-485e-A2A4-3C6E9056E4E4");
declare_interface(IGridView,			"419B5983-BACB-4fd3-8AE3-16709CE573EE");
declare_interface(IParamGroupManager,	"7CC08D53-2D0F-4613-BCF1-691D480718F8");
declare_interface(IClassObject,			"DE4A00ED-8CB5-4af5-96AF-913113B7925B");
declare_interface(IMeasParamGroup2,		"4B71077E-FB21-4a2a-B599-F5DA934C36EF");
*/

#endif// __CLASSES5_H__