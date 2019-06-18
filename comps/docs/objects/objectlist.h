#ifndef __objectlist_h__
#define __objectlist_h__


#include "DataBase.h"
#include "ClassBase.h"
#include "Classes5.h"
#include "Measure5.h"
#include "Object5.h"
#include "PropBagImpl.h"
#include "SimilarityHelper.h"

#include <AfxTempl.h>
#include "calibrint.h"
#include <stack>

/////////////////////////////////////////////////////////////////////////////
// manual params defines

class CCalcObjectContainerImpl : public CImplBase
{
public:
	CCalcObjectContainerImpl();
	virtual ~CCalcObjectContainerImpl();
public:
	virtual INamedDataObject2	*GetList() = 0;
	virtual bool Serialize( CStreamEx &ar );
	void DeInit();

	BEGIN_INTERFACE_PART(CalcCntr, ICalcObjectContainer)
		com_call GetCurentPosition( long *plpos, long *plKey );
		com_call SetCurentPosition( long lpos );
		com_call Move( long lDirection );

		com_call GetFirstParameterPos( long *plpos );
		com_call GetLastParameterPos( long *plpos );
		com_call GetNextParameter( long *plpos, struct ParameterContainer **ppContainer );
		com_call GetPrevParameter( long *plpos, struct ParameterContainer **ppContainer );
		com_call MoveParameterAfter( long lpos, struct ParameterContainer *p );	//InsertAfter, if pos=zero, 

		com_call DefineParameter( long lKey, enum ParamType type, IMeasParamGroup *pGroup, long *plpos );
		com_call DefineParameterFull( struct ParameterContainer *p, long *plpos );	//component should copy container
		com_call RemoveParameter( long lKey );
		com_call RemoveParameterAt( long lpos );
		com_call Reset();
		com_call ParamDefByKey( long lKey, struct ParameterContainer * * ppContainer );

		com_call GetParameterCount( long *pnCount );
	END_INTERFACE_PART(CalcCntr)
protected:
	ParameterContainer*	find( long lKey );
	void			container_notify( long lCode, long lKey );
public:
	long									m_lCurrentPosition;
	CTypedPtrList<CPtrList, ParameterContainer*>	m_params;
	POSITION AddParameter(ParameterContainer *pi);
};
using namespace std;
enum Purpose
{
	Flat =		0,
	Tree =		1
};
namespace kir
{

class CTreeNotion;
class CTreeNode
{
	IUnknown* data;
	CTreeNode* parent;

	int level;
	POSITION pos;
	POSITION& position_in_parent(){return pos;};
	void remove_child(CTreeNode* node, POSITION pos, BOOL del );
	void truly_adopt_children( BOOL del);//recursive set 'this' as parent ndo for all childs branches
	void plant_tree(CTreeNotion* tn, IUnknown* o_list);
	void find_children_in_parent(CTreeNode* object, double cf);
public:
	CTreeNode(){data = 0;pos =0;parent =0;};
	~CTreeNode(){};
	//Free resources of branch for which this node is root
	void KillChildren();
	//Adds a child 
	void AddChild(CTreeNode* child);
	//Sets a parent
	void SetParent(CTreeNode* parent);
	void GetNextChild(POSITION& pos, CTreeNode** tn)
	{
		*tn=0;
		if(pos)
		{
			*tn = children.GetNext(pos);
		}
	};
	void GetFirstChildPosition(POSITION& pos)
	{
		if(children.GetCount()==0) pos=0;
		else pos = children.GetHeadPosition();
	};

	IUnknown* GetUnknown(){return data;};
	CTreeNode* GetParent(){
		return parent;
	};
	//Init node element whith real object
	void SetData(IUnknown* unk)
	{
		data = unk;
		ICompositeSupportPtr cs(data);
		cs->SetLevel(level);
	};
	IUnknown* GetData(){return data;}
	CTypedPtrList<CPtrList,CTreeNode*> children;
	int GetLevel(){return level;};
	void SetLevel(int l){level = l;};
	friend class CTreeNotion;
	friend class CTreeLevel;
};
/*
CTreeLevel - класс имитирующий объектный лист и INamedDataObject2
Позволяет получить виртуальный список подобъектов, полученных с одной бинарной плоскости
*/
class CTreeLevel: public CTypedPtrList<CPtrList,CTreeNode*>,
	public INamedDataObject2
{
	CPtrList m_listDeleted;
	ULONG m_lRef;
	long m_lActive;
	BOOL m_bDelay;
	int level;
	int m_nClass;
	bool m_bIgnoreRemove;
	IUnknown* m_oList;
	CTreeNode* find_parent_for(CTreeNode* , double CoverFactor =1.);
    void adopt_children(CTreeNode* node, CTreeNotion* tn, IUnknown* o_list);
	void find_children_in_deleted(CTreeNode* object, double cf);
	void add_node_to_object_list(CTreeNode* node);//with all children
	void find_children_in_parent(CTreeNode* object, double cf);

friend class CTreeNotion;
public:
	CTreeLevel(IUnknown* list)
	{
 		m_lRef =0;
		m_oList = list;
		AddRef();
		m_lActive=0;
		m_bIgnoreRemove =false; 
		m_bDelay =FALSE; 

		m_nClass = -1;
		m_listDeleted.RemoveAll();
	};
	~CTreeLevel()
	{
			/*CString s;
			s.Format("Deleted Tree Level %i by delete\n", level);
			OutputDebugStr(s);*/
	};
	void SetActiveChild( long lPos, CTreeNode* childToSet );
	void SetLevel(int l){level = l;};
	int GetLevel(){return level;};
	//Добавляет объект и его подобъекты во временный список (см. DelayRemove(BOOL delay)) 
	void AddToDeleted(CTreeNode* node);
	//Получает номер класса в соответствии с бинарной плоскостью, ктр. принадлежит объект 
	int GetClass();
	//Устанавливает номер класса в соответствии с бинарной плоскостью, с ктр. получен объект 
	void SetClass(int c);
	//delay = TRUE -- удаляемые объекты (и их подобъекты) будут помещены в буфер для хранения до вызова с delay = FALSE
	//Удобно для удаления и немедленного добавления редактируемых объектов
	void DelayRemove(BOOL delay);
	////////////////////////INamedDataObject
	com_call GetType( BSTR *pbstrType );
	com_call GetDataInfo( IUnknown **ppunk );	
	com_call IsModified( BOOL *pbModified );
	com_call SetModifiedFlag( BOOL bSet ) ;
	com_call SetTypeInfo( INamedDataInfo *pinfo );

	////////////////////////INamedDataObject2
	com_call CreateChild( IUnknown **ppunkChild ) ;
	com_call GetParent( IUnknown **ppunkParent ) ;
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags  ) ;
	com_call StoreData( DWORD dwDirection ) ;
	com_call RemoveChild( IUnknown *punkChild ) ;
	//Called by framework
	//Caution! Do not use for tree building, use CTreeNotion::AddElement instead
	com_call AddChild( IUnknown *punkChild ) ;
	com_call GetChildsCount( long *plCount ) ;
	com_call GetFirstChildPosition( long *plPos ) ;
	com_call GetNextChild( long *plPos, IUnknown **ppunkChild ) ;
	com_call GetLastChildPosition(long *plPos) ;
	com_call GetPrevChild(long *plPos, IUnknown **ppunkChild) ;
	com_call AttachData( IUnknown *punkNamedData ) ;
	com_call GetObjectFlags( DWORD *pdwObjectFlags ) ;
	com_call SetActiveChild( long lPos ) ;
	com_call GetActiveChild( long *plPos ) ;
	com_call SetUsedFlag( BOOL bSet ) ;	
	com_call SetHostedFlag( BOOL bSet ) ;			
	com_call GetData(IUnknown **ppunkNamedData ) ;
	com_call GetObjectPosInParent( long *plPos ) ; 
	com_call SetObjectPosInParent( long lPos ) ; 
	com_call GetChildPos(IUnknown *punkChild, long *plPos) ;
	com_call InitAttachedData() ;
	com_call GetBaseKey(GUID * pBaseKey) ;
	com_call SetBaseKey(GUID * pBaseKey) ;
	com_call IsBaseObject(BOOL * pbFlag) ;
	//////////////////////////////////////////////////
	//Поддерживает все требуемые интерфейсы CMeasureObjectList
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject)
	{
		*ppvObject = 0;
		if( riid == IID_IUnknown ||
			riid == IID_INamedDataObject)
		{
			*ppvObject = (INamedDataObject*)this;
			((IUnknown*)this)->AddRef();
			return S_OK;
		}
		if( riid == IID_INamedDataObject2 )
		{
			*ppvObject = (INamedDataObject2*)this;
			((IUnknown*)this)->AddRef();
			return S_OK;
		}
		if( riid == IID_ICalcObjectContainer)
		{

			ICalcObjectContainerPtr coc(m_oList);
			*ppvObject = coc.Detach();
			if(*ppvObject) return S_OK;
		}
		if( riid == IID_IDataObjectList)
		{

			IDataObjectListPtr dol(m_oList);
			*ppvObject = dol.Detach();
			if(*ppvObject) return S_OK;
		}
		if( riid == IID_INumeredObject)
		{

			INumeredObjectPtr no(m_oList);
			*ppvObject = no.Detach();
			if(*ppvObject) return S_OK;
		}
		if( riid == IID_ICompositeObject)
		{
			ICompositeObjectPtr co(m_oList);
			*ppvObject = co.Detach();
			if(*ppvObject) return S_OK;
		}
		
		return E_NOINTERFACE; 
	}
	ULONG STDMETHODCALLTYPE AddRef( void)
	{
		/*m_oList->AddRef();*/
		return ++m_lRef; 
	};
	ULONG  STDMETHODCALLTYPE Release( void)
	{
		/*m_oList->Release();*/
		m_lRef--;
		if( !m_lRef )
		{
			/*CString s;
			s.Format("Deleted Tree Level %i by release\n", level);
			OutputDebugStr(s);*/
			delete this;
			return 0;
		}
		return m_lRef;
	};

};

//typedef CTypedPtrList<CPtrList,CTreeNode*>  CTreeLevel;
typedef CTypedPtrList<CObList,CTreeLevel*> CLevels;

//Дерево объектов 
//Хранит иерархию объектов родительского объектного листа, сам лист остается плоским 
class CTreeNotion
{
	CLevels levels;
	double cf;
	IMeasParamGroupPtr m_group;
	CTreeNode* find(IUnknown* punkToFind, int level=-1);
	CTreeNode* find_on_level(IUnknown* punkToFind, int level)
	{
		CTreeNode* node =0;
		CTreeLevel* tl = GetLevel(level);
		POSITION pos = tl->GetHeadPosition();
		while(pos)
		{
			node = tl->GetNext(pos);
			INamedDataObject2Ptr nd(punkToFind);
			if(nd == node->GetData()) return node;
		}
		return 0;
	}
	
	CTreeNode* find_on_level(IUnknown* punkToFind, POSITION levelPosition)
	{
		CTreeLevel* tl = levels.GetAt(levelPosition);
		POSITION pos = tl->GetHeadPosition();
		CTreeNode* node;
		while(pos)
		{
			node = tl->GetNext(pos);
			INamedDataObject2Ptr ndo(punkToFind);
			if(node->GetData() == ndo) return node;
		}
		return 0;
	}


public:
	CTreeNotion();
	~CTreeNotion();
	//Производит вычисления только CCompositeMeasGroup
	void CalcCompositeObject(IUnknown* object);
	//cf - cover factor, см. MeasureComposite
	double GetCF(){return cf;};
	void SetCF(double f){cf=f;};
	//Возвращает список, содержащий уровни дерева
	CLevels* GetLevels(){return &levels; };
	//Поиск узла дерева по ключу
	CTreeNode* FindElementByKey(GUID key);
	//удаляет ветвь начиная с 'root', если bAddToDelay - добавляет в отложенные удаленные
	void root_out_branch(CTreeNode* root, bool bAddToDelay , bool bRecursively );
	//adds new tree level, returns levels count
	int AddLevel(IUnknown* list);
	//adds tree node to the tail of specified level
	CTreeNode* AddElement(int level);
	//finds and deletes specified element with children
	bool DeleteElement(IUnknown* punkToDelete, int level=-1);
	//returns specified tree level
	CTreeLevel* CTreeNotion::GetLevel(int level)
	{
		if(level>=levels.GetCount()) return 0;
		POSITION p = levels.GetHeadPosition();
		CTreeLevel* tl;

		while(level>=0)
		{
			tl = levels.GetNext(p);
			level--;

		}
		return tl;

	}
	//вызов DelayRemove(BOOL delay) для всех CTreeLevel's
	void DelayRemove(BOOL delay);
	//returns base tree level
	CTreeLevel* GetBaseLevel();
	//очистка ресурсов
	void RemoveAll();
	//кол-во уровней
	int GetLevelsCount();
	//Search tree node by node IUnknown 
	CTreeNode* Find(IUnknown* punkToFind, int level=-1)
	{
		CTreeNode* node =0;
		if(level==-1)
		{
			POSITION pos = levels.GetHeadPosition();
			while(pos)
			{
				if(node = find_on_level(punkToFind, pos)) return node;
				levels.GetNext(pos);
			}
		}
		else
		{
			return find_on_level(punkToFind, level);
		}
		return 0;
	};
};
class CCompositeInfoWrp: public CCompositeInfo
{
	//int m_nCount;
	//int* p_nClass;
	//int m_nVersion;
	//bool m_bActual;
public:
	CCompositeInfoWrp(){ m_nCount=0; p_nClass =0; m_nVersion =1; m_bActual = false;} ; 
	~CCompositeInfoWrp(){delete[] p_nClass;};
	void Init(int count)
	{
		m_nCount = count;
		delete[] p_nClass;
		p_nClass = new int[count];
	};
	void Set(int lvl, int cls)
	{
		ASSERT(lvl<m_nCount);
		p_nClass[lvl] = cls;
	};
	int Get(int lvl)
	{
		ASSERT(lvl<m_nCount);
		return p_nClass[lvl];
	};
	void Copy(const CCompositeInfo& cinfo)
	{
		delete[] p_nClass;
		m_nCount = cinfo.m_nCount;
		p_nClass = new int[m_nCount];
		for(int i=0;i<m_nCount;i++)
		{
			p_nClass[i] = cinfo.p_nClass[i];
		}
		m_bActual =cinfo.m_bActual;
	};
	void Serialize(CStreamEx &ar)
	{
		if(ar.IsStoring())
		{
			ar<<m_nVersion;
			ar<<m_nCount;
			for(int i=0; i<m_nCount;i++)
			{
				ar<<p_nClass[i];
			}
		}
		else
		{
			ar>>m_nVersion;
			ar>>m_nCount;
			Init(m_nCount);
			for(int i=0; i<m_nCount;i++)
			{
				ar>>p_nClass[i];
			}
			m_bActual = true;
		}
	};
	long& actual()
	{
		return m_bActual;
	};
	void clear()
	{
		delete[] p_nClass;
		p_nClass=0;
		m_nCount =0;
		m_bActual =FALSE;
	};

};
}
using namespace kir;
class CMeasureObjectList :	public CDataObjectListBase,
							public CCalcObjectContainerImpl,
							public CNamedPropBagImpl

{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CMeasureObjectList);
	GUARD_DECLARE_OLECREATE_PROGID(CMeasureObjectList);
	DECLARE_INTERFACE_MAP();
public:
	ENABLE_MULTYINTERFACE();



	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	};

	BEGIN_INTERFACE_PART(Composite, ICompositeObject)
		com_call SetPurpose(int purpose);
		com_call GetPurpose(int* purpose);
		//Построение дерева объектов
		com_call BuildTree(LONG biCount);
		//Is object list composite
		com_call IsComposite(long* bC);
		//Установка признака см. IsComposite()
		com_call SetCompositeFlag();
		//Получить CTreeLevel's IUnknown (только базовые объекты) ! без AddRef !
		com_call GetMainLevelObjects(IUnknown** unk);
		//Получить представление списка в виде дерева
		com_call GetTreeNotion(void** tn);
		//Получить CTreeLevel's IUnknown 
		com_call GetLevel(int level, IUnknown** unkLevel);
		//Установить задержку удаления объектов
		com_call DelayRemove(long delay, IUnknown* pUnkLevel);
		//Получить CTreeLevel's IUnknown (объекты заданного класса) !AddRef!
		com_call GetLevelbyClass(int cls, IUnknown** unkLevel);
		//Получить CTreeLevel's IUnknown (объекты уровнем ниже, чем cls)		
		com_call GetElderLevel(int cls, IUnknown** unkLevel);
		//Построить CTreeNotion на базе соответствия ключей объектов 
		com_call RestoreTreeByGUIDs();
		//Кол-во бинарных плоскостей, из ктр. построен объект 
		com_call GetPlaneCount(long* count);
		com_call SetPlaneCount(long count);
		com_call CalculateComposite();
		com_call SetCompositeInfo(CCompositeInfo* ci);
		com_call GetCompositeInfo(CCompositeInfo** ci);
	END_INTERFACE_PART(Composite)


public:
	CMeasureObjectList();
	~CMeasureObjectList();
public:
	virtual INamedDataObject2	*GetList(){return &m_xDataObj;};
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );				
	virtual void OnFinalRelease( );
	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CMeasureObjectList)
	afx_msg long GetParametersCount();
	afx_msg long GetFirstParameterPosition();
	afx_msg long GetNextParameter(long lpos, VARIANT FAR* refName, VARIANT FAR* refUnits, VARIANT FAR* refKey, VARIANT FAR* refCalibr);

	afx_msg bool SetBagProperty( LPCTSTR, const VARIANT FAR& );
	afx_msg VARIANT GetBagProperty( LPCTSTR );

	afx_msg LPDISPATCH GetSimilarObject(LPDISPATCH pdispObject);

	afx_msg void InitSimilarityHelper();
	afx_msg void DeInitSimilarityHelper();

	afx_msg void SimilarityHelper_ProcessObject(LPDISPATCH pdispObject); // обсчитать все для объекта; должен быть уже вызван Init
	afx_msg void SimilarityHelper_ProcessObjectError(); // обсчитать для объекта статистику значимости параметров; необязательное дополнение к ProcessObject
	afx_msg void SimilarityHelper_TestObjectList(LPDISPATCH pdispList); // обсчитать по всем объектам листа - для каждого ProcessObject и ProcessObjectError
	// можно вызвать для нескольких листов подряд - накопится общая статистика
	afx_msg void SimilarityHelper_TestObjectList2(LPDISPATCH pdispList); // посчитать ошибки и найти оптимальные параметры для вкл/выкл
	// вызывать только для одного листа за раз

	afx_msg long SimilarityHelper_GetClass(); // вернуть класс наиболее похожего
	afx_msg long SimilarityHelper_Get(); // вернуть номер наиболее похожего
	afx_msg long SimilarityHelper_GetInClass(long nClass); // вернуть номер наиболее похожего в данном классе
	afx_msg long SimilarityHelper_GetInOwnClass(); // вернуть номер наиболее похожего в собственном классе
	afx_msg double SimilarityHelper_GetParamDistance(int nParam); // вернуть "расстояние" по указанному параметру

	afx_msg void SimilarityHelper_WriteObjectReport(LPCWSTR fileName); // сохранить отчет по объекту; должен быть уже вызван Init и Process
	afx_msg void SimilarityHelper_WriteErrorReport(LPCWSTR fileName); // сохранить отчет по тестированию

	afx_msg double CMeasureObjectList::SimilarityHelper_GetError2(); // получить ошибку, которая была при TestObjectList2
	afx_msg double CMeasureObjectList::SimilarityHelper_Improve2(); // один шаг оптимизации, возвращает, на сколько прооптимизировали; только после TestObjectList2

	//}}AFX_DISPATCH
	//{{AFX_MSG(CMeasureObjectList)
	//}}AFX_MSG
	//{{AFX_VIRTUAL(CMeasureObjectList)
	//}}AFX_VIRTUAL

	virtual bool IsObjectEmpty();
	/*void RegisterCompositeMeasgroup(void);*/
protected:
	virtual void OnAddChild( IUnknown *punkNewChild, POSITION pos );
	virtual void OnDeleteChild( IUnknown *punkChildToDelete, POSITION pos );
	virtual void OnAfterSerialize( CStreamEx& ar );
	virtual void OnSetBaseObject(GuidKey & NewKey);
	virtual bool DoSetActiveChild(POSITION pos);
	virtual POSITION DoGetActiveChild();
	//ICompatible object interface
	HRESULT CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );
	INamedDataObject2Ptr m_ptrActiveObject;
	Purpose purpose;

public:
	static const char *c_szType;
	private:
		bool store_child(IUnknown* unk, int level, CTreeNode* parent =0);
		POSITION find_child(IUnknown* punkChild);
		CTreeNotion m_lObjects;
		BOOL m_bDirty;
		POSITION m_posActive;
		BOOL bComposite;
		int m_binaryCount;
		bool m_bLockRemoveNotification;
		bool m_bDelay;
		bool m_bFinalRelease;
		CCompositeInfoWrp m_ci;

	private:
		CSimilarityHelper* m_pSimilarityHelper;

protected:
	// восстанавливает структуру дерева после сериализации
	void restore_tree_from_archive(void);

};




class CMeasureObjectLight : public  CDataObjectBase,
							public  CCalcObjectImpl,
							public  CNamedPropBagImpl
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CMeasureObjectLight);
	GUARD_DECLARE_OLECREATE(CMeasureObjectLight);
	ENABLE_MULTYINTERFACE();					//

public:
	CMeasureObjectLight();
	~CMeasureObjectLight();

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}

	virtual DWORD GetNamedObjectFlags();

	virtual IUnknown* Clone();

	DECLARE_INTERFACE_MAP();

	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CMeasureObject)
		afx_msg double GetValueDisp(long nKey);
		afx_msg double GetValueInUnitsDisp(long nKey);
		afx_msg void SetValueDisp(long nKey, double fValue);
		afx_msg void SetValueInUnitsDisp(long nKey, double fValue);
		afx_msg long GetValuesCount();
		afx_msg long GetClassDisp(LPCTSTR strClassifierName);
		afx_msg void SetClassDisp(long lClass, LPCTSTR strClassifierName);
		afx_msg double GetValueExDisp(long nKey, long lPane, long lPhase);
		afx_msg void SetValueExDisp(long nKey, long lPane, long lPhase, double fValue);
	//}}AFX_DISPATCH
//		afx_msg void SetClassKeyDisp(VARIANT Key);
//		afx_msg VARIANT GetClassKeyDisp();


	BEGIN_INTERFACE_PART(Clone, IClonableObject)
		com_call Clone(IUnknown** ppunkCloned);
	END_INTERFACE_PART(Clone)

protected:
	
	virtual void OnAddChild(IUnknown *punkNewChild, POSITION pos);
	virtual void OnDeleteChild(IUnknown *punkChildToDelete, POSITION pos);
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );

	void		ParentUpdate();
	//void		SetListowner(IUnknown*);

public:
	static const char *c_szType;

protected:
	POSITION	m_posActiveChild;
	
};


class CMeasureObject : public  CMeasureObjectLight,
					   public  CClipboardImpl
{
	PROVIDE_GUID_INFO()
	DECLARE_DYNCREATE(CMeasureObject);
	GUARD_DECLARE_OLECREATE(CMeasureObject);
	ENABLE_MULTYINTERFACE();					//

public:

	CMeasureObject();
	~CMeasureObject();

//ovverrided virtuals
	virtual DWORD GetNamedObjectFlags();
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );
	virtual bool ExchangeDataWithParent( DWORD dwFlags );

	virtual IUnknown* Clone();
	
	virtual bool Copy(UINT nFormat, HANDLE* pHandle);


	BEGIN_INTERFACE_PART(MeasObj, IMeasureObject2)
		com_call SetImage( IUnknown *punkImage );
		com_call GetImage( IUnknown **ppunkImage );
		com_call UpdateParent();
		com_call GetChildByParam(long lParamKey, IUnknown **ppunkChild);
		com_call SetDrawingNumber(long   nNum);
		com_call GetDrawingNumber(long* pnNum);
		com_call SetNumberPos( POINT point );
		com_call GetNumberPos( POINT *ppoint );
		// IMeasureObject2
		com_call SetZOrder( int nZOrder );
		com_call GetZOrder( int *pnZOrder );
	END_INTERFACE_PART(MeasObj)

	BEGIN_INTERFACE_PART(Calibr, ICalibr)
		com_call GetCalibration( double *pfPixelPerMeter, GUID *pguidC );
		com_call SetCalibration( double fPixelPerMeter, GUID *guidC );
	END_INTERFACE_PART(Calibr)

	BEGIN_INTERFACE_PART(Rect, IRectObject)
		com_call SetRect(RECT rc);
		com_call GetRect(RECT* prc);
		com_call HitTest( POINT	point, long *plHitTest );
		com_call Move( POINT point );
	END_INTERFACE_PART(Rect)

	BEGIN_INTERFACE_PART(Composite, ICompositeSupport)
	com_call SetSyncronizedFlag(long flag);
	com_call GetPixCount(long *pixCount);
	com_call SetPixCount(long pixCount);
	com_call IsSyncronized(long* flag);
	com_call GetParentKey(GUID* key);
	com_call SetParentKey(GUID key);
	com_call GetTreeKey(GUID* key);
	com_call SetTreeKey(GUID key);
	com_call GetLevel(long* lvl);
	com_call SetLevel(long lvl);
	com_call GetGrandParent(IUnknown** unkParent);
	com_call CopyAllCompositeData(IUnknown* destObject);

	END_INTERFACE_PART(Composite)


	IUnknown *	GetObjectByParamKey(long lParamKey);

	DECLARE_INTERFACE_MAP();

	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CMeasureObject)
		afx_msg LPDISPATCH GetImageDisp();
	//}}AFX_DISPATCH

protected:
	CImageWrp	m_image;
	long		m_nDrawingNumber;
	CPoint		m_point_num;
	long		m_nZOrder;
	GUID m_guidParentKey;
	GUID m_guidTreeKey;
	long m_lLevel;
	long m_lPixCount;
	long m_lSync;
public:
	static const char *c_szType;

};


extern void CopyCalcVals(ICalcObjectPtr sptrIn, ICalcObjectPtr sptrOut);

#endif //__objectlist_h__
