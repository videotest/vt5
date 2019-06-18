#pragma once

class CMethodStep;

interface IMethodData : public IUnknown
{
	com_call GetFirstStepPos( long *plPos ) = 0;
	com_call GetNextStep( long *plPos, CMethodStep *pStep ) = 0; // ���� plPos==0 - ���������� � plPos ������
	com_call GetNextStepPtr( long *plPos, CMethodStep **ppStep ) = 0; // ���� plPos==0 - ���������� � plPos ������
	com_call GetPrevStep( long *plPos, CMethodStep *pStep ) = 0;
	com_call GetStepCount( long *plCount ) = 0;
    com_call AddStep( CMethodStep *pStep, long lInsertBefore, long *plNewPos=0 ) = 0;
	com_call SetStep( long *plPos, CMethodStep *pStep, bool bDontClearCache=false ) = 0;
	com_call DeleteStep( long lPos ) = 0;
	com_call SetActiveStepPos( long lPos, BOOL bLoadCache=TRUE ) = 0;
	com_call GetActiveStepPos( long *plPos ) = 0;
	com_call GetModifiedFlag( BOOL *pbFlag ) = 0;
	com_call SetModifiedFlag( BOOL bFlag ) = 0;
	com_call GetStepIndexByPos( long lPos, long *plIndex ) = 0;
	com_call GetStepPosByIndex( long lIndex, long *plPos ) = 0;
	com_call UpdateMethod() = 0; // ��������� shell.data ��������
	com_call UpdateActiveStep() = 0; // ��������� ��������� ���� (\\StateAfterAction) ��������� ����
};

interface IMethod : public IUnknown
{
	com_call CreateCompatible(IUnknown **ppunkObject) = 0;
	com_call StoreFile(BSTR bstrFile) = 0;
	com_call LoadFile(BSTR bstrFile) = 0;
	com_call StoreToNamedData(IUnknown* punkND) = 0;
	com_call LoadFromNamedData(IUnknown* punkND) = 0;
	com_call SetName(BSTR bstrName) = 0;
	com_call GetName(BSTR *pbstrName) = 0;
	com_call GetFullScript(BSTR *pbstrScript) = 0;
};

enum MethodMoveStepCodes
{
	mmscMove, // ������ ����������� ���
	mmscContinueMove, // ���������� ����� ������� �������� (�� ���� ���� mmscMove ��� mmscContinueMove)
	mmscFinishMove, // ��������� ����� ������� �������� (�� ���� ���� mmscMove ��� mmscContinueMove)
};

interface IMethodChanges : public IUnknown
{
	com_call SetUndoDepth(long nDepth) = 0;
	com_call AddStep(long nStep, CMethodStep *pStep) = 0;
	com_call DeleteStep(long nStep) = 0;
	com_call MoveStep(long nStepFrom, long nStepTo, long nMoveCode=mmscMove) = 0;
	com_call ChangeStep(long nStep, CMethodStep *pStep, bool bDontClearCache=false) = 0;
	com_call Undo() = 0;
	com_call Redo() = 0;
	com_call ClearUndoRedo(bool bClearUndo=true, bool bClearRedo=true) = 0;
	com_call GetUndoAvailable( long *pnCount ) = 0; // ����������, ������� ����� Undo ��������
	com_call GetRedoAvailable( long *pnCount ) = 0; // ����������, ������� ����� Redo ��������
	com_call BeginGroupUndo() = 0; // ������ ������ Undo; ����������� ��������� ������
	com_call EndGroupUndo() = 0; // ��������� ������ Undo (��� ���� ��� ���� �� ���������� BeginGroupUndo �������� ������ ������� ���� GroupUndo)
	// ���� bKillEmpty - �� ������ ������ ����� �������� (� ������ Undo �� ���� BeginGroupUndo/EndGroupUndo ������ �� �������)
};

declare_interface( IMethodData,	"723EDE87-A61D-40de-A8BC-94131167005D" ) 
declare_interface( IMethod,	"0F4DD0F0-8CA4-45e2-8A3F-F29162EFC4EE" )
declare_interface( IMethodChanges,	"20F132F8-BC3E-4cd2-83BE-06E47A65ED29" )

interface IMethodTab : public IUnknown
{
	com_call AddPane( BSTR bstrName ) = 0;
	com_call Remove( long lIndex ) = 0;
	com_call GetActivePane( long *plIndex ) = 0;
	com_call GetPaneCount( long *plCount ) = 0;
	com_call LoadContent( BSTR bstrFile ) = 0;
	com_call StoreContent( BSTR bstrFile ) = 0;

	com_call GetContextPoint( POINT *lpptContext ) = 0;

	com_call PanePaneItemByPoint( POINT ptPoint ) = 0;

	com_call OpenPane( BSTR bstrPath, BOOL bOpen ) = 0;
};
declare_interface( IMethodTab,	"F5F8D5C1-A866-45f9-B4A1-71FCCAD09991" )

enum LoadCacheFlags
{
	lcfLoadShellData	= 1<<0, // ������������ shell.data
	lcfLoadImage		= 1<<1, // ������������ image
	lcfRestoreView		= 1<<2, // ������������ view;
	lcfRestoreSelection	= 1<<3, // ������������ ���������; ������������ ������ � ���� � lcfLoadImage
	lcfRestoreZoom		= 1<<4, // ������������ ���; ������������ ������ � ���� � lcfRestoreView

	lcfLoadAll			= 0xFFFFFFFF // ��� ������
};

interface IMethodMan : public IUnknown
{
	com_call Run() = 0; // ��������� �������� �������
	com_call RunFromActive() = 0;  // [vanek] BT2000: 4003, 4004: ������ �������� � ��������� ���� - 17.09.2004
	com_call Stop(BOOL bStopRun=true, BOOL bStopRecord=true) = 0;
	com_call Record() = 0;
	com_call IsRunning(BOOL *pbVal) = 0;
	com_call IsRecording(BOOL *pbVal) = 0;
	com_call AddMethod(IUnknown* punkMethod, long lInsertBefore, long *plNewPos = 0 ) = 0;
	com_call DeleteMethod(long lPos) = 0;
	com_call SetActiveMethodPos( long lPos ) = 0;
	com_call GetActiveMethodPos( long *plPos ) = 0;
	com_call GetFirstMethodPos( long *plPos ) = 0;
	com_call GetNextMethod( long *plPos, IUnknown **pMethod ) = 0;  // ���� plPos==0 - ���������� � plPos ������
	com_call Reload( ) = 0;

	com_call StoreCache( IUnknown *punkMethod, long lStepPos ) = 0;
	com_call LoadCache( IUnknown *punkMethod, long lStepPos, DWORD dwFlags=lcfLoadAll ) = 0;
	com_call DropCache( IUnknown *punkMethod, long lStepPos ) = 0;
	com_call IsCached( IUnknown *punkMethod, long lStepPos, BOOL *pbCached ) = 0;

	// ����������� ��� ����������� ��� � ����� ������� �� ������ (��������� ��� MethodDoer::Undo())
	com_call MoveCache( IUnknown *punkMethod, long lOldStepPos, long lNewStepPos, BOOL bCopy=FALSE ) = 0;
};
declare_interface( IMethodMan,	"F9202E72-E816-4574-B609-46725B8555AD" )

interface IMethodMan2 : public IMethodMan
{
	com_call LockNotification() = 0; // ������������� �����������
	com_call UnlockNotification() = 0; // �������������� �����������
	// ���� ������� ����������, ���� ������������� 2 ���� - ����������� �� �����, ���� �� ������������ 2 ����
	//com_call Step() = 0; // ��������� ���� ��� ��������
};
declare_interface( IMethodMan2,	"BB39BE10-8F6B-4ae9-A783-033B7138CA15" )



#define szEventMethodRecord "MethodRecord"
#define szEventMethodRun "MethodRun"
#define szEventMethodStop "MethodStop"

#define szBeginMethodLoop	"BeginMethodLoop"
#define szEndMethodLoop		"EndMethodLoop"
#define szRecordHere		"RecordHere"

// [vanek] : ������� ��������, ���������� � �� PropBag - 27.10.2004
#define szMtdPropCanDelete	"CanDelete"	// ��� �������� - long
#define szMtdPropCanCopy	"CanCopy"	// ��� �������� - long
#define szMtdPropCanEdit	"CanEdit"	// ��� �������� - long
#define szMtdPropCanRun		"CanRun"	// ��� �������� - long
#define szMtdPropAutoSave	"AutoSave"	// ��� �������� - long
#define szMtdPropSortPrefix	"SortPrefix"	// ��� �������� - BSTR
