#pragma once
#include "..\awin\misc_ptr.h"
#include "ActionHelper.h"
#include "vector"
#include "list"

class CViewsLock
{
public:
	void AddView(IUnknown* pView);
	void Clear();
protected:
	struct XViewLock
	{
		IDataContext3Ptr sptrView; // вьюха
		BOOL bPrevLock; // ее предыдущее состояние
	};
	list<XViewLock> m_locks;
};

class CViewState
{ // состояние конкретной вьюхи - что в ней заселекчено
public:
	void Push(IDataContext3Ptr sptrDC);
	void Pop(IDataContext3Ptr sptrDC);
	vector<IUnknownPtr> m_SelectionList;
};

class CDocumentState
{
public:
	void CreateEmpty(); // создать пустой
	void GetFrom(INamedDataPtr sptrND, IDataContext3Ptr sptrDC);
	void PutTo(INamedDataPtr sptrND, IDataContext3Ptr sptrDC, bool bClearUndo=false);
	INamedDataPtr m_sptrND; // NamedData (документа)
	IUnknownPtr m_ptrUndoPos; // позиция в Undo List
	list<CViewState> m_ViewsStates;
	//CViewState m_ViewState;
};

// Impl для CActionHelper2: обеспечивает функции, необходимые FormManager
// для соответствующий функций диспатча
class CActionHelper2
{
public:
	CActionHelper2(void);
	~CActionHelper2(void);
	void	Init( CActiveXSite *psite, IUnknownPtr ptrDoc, int nStates=2 );
	void	DeInit();

	// работа с состояниями:
	bool SaveState(int index); // low level - сохранить State
	bool LoadState(int index, bool bClearUndo); // low level - загрузить State
	bool Invalidate(int index); // пометить State как Invalid
	void Cancel(); // отменить выполнение
	void Ok(); // завершить выполнение
	bool UpdateStates(); // выполнить все Invalid States
	inline int GetStatesCount() { return m_States.size(); };
	inline IDocumentSitePtr GetDocument() { return m_sptrDoc; };

protected:
	CActiveXSite			*m_psite;
	IDocumentSitePtr	m_sptrDoc;

	CViewsLock m_ViewsLock;
	void LockAllViews();
	void UnlockAllViews();

	_ptr_t<CDocumentState> m_States; // состояния обрабатываемого документа
	// m_States[0] - начальное
	// m_States[m_States.size()-1] - конечное
	int m_nFirstInvalidState; // номер первого необработанного этапа;
	// может меняться внутри OnProcess
	
	enum { ah2Cancel, ah2Ok, ah2Continue } m_Action; // может меняться внутри OnProcess
	// на Ok и Cancel Preview останавливается, соответственно завершив работу или отменив ее результат

	bool m_bRunning; // флаг, установленный во время выполнения.
	// пока true - можно только менять m_Action и m_nFirstInvalidState

	class XLock
	{
	public:
		XLock( bool* pbLock )
		{ m_pbLock = pbLock; m_bStore = *pbLock; *pbLock = true; }
		~XLock()
		{ *m_pbLock = m_bStore;}
	protected:
		bool *m_pbLock;
		bool m_bStore;
	};
};
