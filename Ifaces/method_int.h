#pragma once

class CMethodStep;

interface IMethodData : public IUnknown
{
	com_call GetFirstStepPos( long *plPos ) = 0;
	com_call GetNextStep( long *plPos, CMethodStep *pStep ) = 0; // если plPos==0 - возвращает в plPos первый
	com_call GetNextStepPtr( long *plPos, CMethodStep **ppStep ) = 0; // если plPos==0 - возвращает в plPos первый
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
	com_call UpdateMethod() = 0; // обновляет shell.data методики
	com_call UpdateActiveStep() = 0; // обновляет состояние вьюх (\\StateAfterAction) активного шага
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
	mmscMove, // просто передвинуть шаг
	mmscContinueMove, // продолжить ранее начатое движение (до того было mmscMove или mmscContinueMove)
	mmscFinishMove, // завершить ранее начатое движение (до того было mmscMove или mmscContinueMove)
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
	com_call GetUndoAvailable( long *pnCount ) = 0; // возвращает, сколько шагов Undo доступно
	com_call GetRedoAvailable( long *pnCount ) = 0; // возвращает, сколько шагов Redo доступно
	com_call BeginGroupUndo() = 0; // Начать группу Undo; допускаются вложенные группы
	com_call EndGroupUndo() = 0; // Завершить группу Undo (при этом все шаги от последнего BeginGroupUndo попадают внутрь единого шага GroupUndo)
	// если bKillEmpty - то пустая группа сразу удалится (в список Undo по паре BeginGroupUndo/EndGroupUndo ничего не попадет)
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
	lcfLoadShellData	= 1<<0, // восстановить shell.data
	lcfLoadImage		= 1<<1, // восстановить image
	lcfRestoreView		= 1<<2, // восстановить view;
	lcfRestoreSelection	= 1<<3, // восстановить выделение; использовать только в паре с lcfLoadImage
	lcfRestoreZoom		= 1<<4, // восстановить зум; использовать только в паре с lcfRestoreView

	lcfLoadAll			= 0xFFFFFFFF // все подряд
};

interface IMethodMan : public IUnknown
{
	com_call Run() = 0; // Выполнить методику целиком
	com_call RunFromActive() = 0;  // [vanek] BT2000: 4003, 4004: запуск методики с активного шага - 17.09.2004
	com_call Stop(BOOL bStopRun=true, BOOL bStopRecord=true) = 0;
	com_call Record() = 0;
	com_call IsRunning(BOOL *pbVal) = 0;
	com_call IsRecording(BOOL *pbVal) = 0;
	com_call AddMethod(IUnknown* punkMethod, long lInsertBefore, long *plNewPos = 0 ) = 0;
	com_call DeleteMethod(long lPos) = 0;
	com_call SetActiveMethodPos( long lPos ) = 0;
	com_call GetActiveMethodPos( long *plPos ) = 0;
	com_call GetFirstMethodPos( long *plPos ) = 0;
	com_call GetNextMethod( long *plPos, IUnknown **pMethod ) = 0;  // если plPos==0 - возвращает в plPos первый
	com_call Reload( ) = 0;

	com_call StoreCache( IUnknown *punkMethod, long lStepPos ) = 0;
	com_call LoadCache( IUnknown *punkMethod, long lStepPos, DWORD dwFlags=lcfLoadAll ) = 0;
	com_call DropCache( IUnknown *punkMethod, long lStepPos ) = 0;
	com_call IsCached( IUnknown *punkMethod, long lStepPos, BOOL *pbCached ) = 0;

	// переместить или скопировать кэш с одной позиции на другую (поддержка для MethodDoer::Undo())
	com_call MoveCache( IUnknown *punkMethod, long lOldStepPos, long lNewStepPos, BOOL bCopy=FALSE ) = 0;
};
declare_interface( IMethodMan,	"F9202E72-E816-4574-B609-46725B8555AD" )

interface IMethodMan2 : public IMethodMan
{
	com_call LockNotification() = 0; // заблокировать нотификации
	com_call UnlockNotification() = 0; // разблокировать нотификации
	// есть счетчик блокировок, если заблокировать 2 раза - нотификаций не будет, пока не разблокируем 2 раза
	//com_call Step() = 0; // Выполнить один шаг методики
};
declare_interface( IMethodMan2,	"BB39BE10-8F6B-4ae9-A783-033B7138CA15" )



#define szEventMethodRecord "MethodRecord"
#define szEventMethodRun "MethodRun"
#define szEventMethodStop "MethodStop"

#define szBeginMethodLoop	"BeginMethodLoop"
#define szEndMethodLoop		"EndMethodLoop"
#define szRecordHere		"RecordHere"

// [vanek] : свойста методики, хранящиеся в ее PropBag - 27.10.2004
#define szMtdPropCanDelete	"CanDelete"	// тип значения - long
#define szMtdPropCanCopy	"CanCopy"	// тип значения - long
#define szMtdPropCanEdit	"CanEdit"	// тип значения - long
#define szMtdPropCanRun		"CanRun"	// тип значения - long
#define szMtdPropAutoSave	"AutoSave"	// тип значения - long
#define szMtdPropSortPrefix	"SortPrefix"	// тип значения - BSTR
