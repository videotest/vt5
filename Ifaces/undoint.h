#ifndef __undolistint_h__
#define __undolistint_h__

#include "defs.h"


#define GROUP_CODE_OK		1
#define GROUP_CODE_CANCEL	0

interface IUndoList : public IUnknown
{
	com_call DoUndo() = 0;
	com_call DoRedo() = 0;
	com_call GetUndoStepsCount( int *pnStepsCount ) = 0;
	com_call GetRedoStepsCount( int *pnStepsCount ) = 0;
	com_call AddAction( IUnknown *punkAction ) = 0;
	com_call RemoveAction( IUnknown *punkAction ) = 0;
	com_call GetLastUndoAction( IUnknown **ppunkAction ) = 0;
	com_call ClearUndoRedoList( ) = 0;

	com_call BeginGroup( BSTR bstr ) = 0;
	com_call EndGroup( long code ) = 0;
	com_call ClearRedoList( ) = 0;

	com_call GetEnableUndo( BOOL* pbenable ) = 0;
	com_call SetEnableUndo( BOOL benable ) = 0;
};

declare_interface(IUndoList, "5885BA43-199D-4a99-AC5F-995E3704F3D2");

interface IUndoList2 : public IUndoList
{
	com_call BeginGroup2( BSTR bstr, long lStepsBack ) = 0; // начать группу, включив в нее акции из lStepsBack последних групп
	com_call GetUndoAction( int nUndoStepPos, int nActionPos, IUnknown **ppunkAction ) = 0; // получить указатель на акцию в undo-листе
	// nUndoStepPos - номер шага Undo, 0 - последний
	// nActionPos - номер акции в шаге, 0 - последн€€
	com_call GetUndoActionName( int nUndoStepPos, int nActionPos, BSTR *pbstrActionName) = 0; // получить им€ акции в undo-листе
	com_call DeleteLastUndoAction( ) = 0;
	com_call EndGroup2( BSTR bstr,  long code ) = 0;
};

declare_interface(IUndoList2, "F6AA90EF-2D76-42e4-98AD-E8245BC13006");

//added by akm 26_10_k5
/*interface IUndoList3 : public IUndoList2
{
	//удалить из списка UndoList последнюю акцию, потребовалось дл€ исправлени€
	//ошибки 1428 из SBT
	com_call DeleteLastUndoAction( ) = 0;
};

declare_interface(IUndoList3, "C4C248E8-BDC2-425d-ACC7-804DEE177728");*/

#endif // __undolistint_h__