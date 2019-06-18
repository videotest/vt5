#pragma once

#include "data_main.h"
#include "statistics.h"
#include "clone.h"
#include "action_main.h"
#include "resource.h"
#include "action_undo.h"
#include "impl_event.h"
#include "\vt5\common\measure_misc.h"
#include "impl_long.h"
#include <ColorsNames.h>

#include "param.h"
#include "idispatchimpl.h"

#include "PropBagImpl.h"

#include "\vt5\common\misc_str.h"

#ifdef _DEBUG
	#import "\vt5\vt5\debug\comps\statobject.tlb" no_namespace raw_interfaces_only named_guids 
#else
	#import "\vt5\vt5\release\comps\statobject.tlb" no_namespace raw_interfaces_only named_guids 
#endif


using namespace ObjectSpace::ParamSpace;

//void _delete_param( CStatParamBase *pObject );

//typedef _list_t<CStatParamBase *, _delete_param> PARAMS_LIST;

//void _delete_list( PARAMS_LIST *pObject );

interface ILongController : public IUnknown
{
	com_call SendNotify() = 0;
};

declare_interface( ILongController, "56575FA5-B9F1-4e96-AC99-FBE827F9FC7D" );
namespace ObjectSpace
{
	
	struct LessGUID{
		bool operator()(const GUID&g1,const GUID&g2)const{
			return memcmp(&g1,&g2,sizeof GUID)<0;
		}
	};

	typedef map<GUID, NumImg, LessGUID> MapGUID;

	struct MapDscrs:map<INamedData*,ParDscr*>{
		MapDscrs();
		~MapDscrs(){
			for(const_iterator it=begin(); it!=end(); ++it){
				it->first->Release();
				delete it->second;
			}
		}
		ParDscr*& operator[](INamedData* pNamedData){
			_ASSERTE(pNamedData);
			pNamedData->AddRef();
			std::pair<iterator,bool> ib=insert(value_type(pNamedData,(ParDscr*)NULL));
			return ib.first->second;
		}
	};

	class CStatisticObject :	public CObjectBase,
								public CEventListenerImpl,
								public ICompatibleObject,
								public CNamedPropBagImpl,
								public ::IDispatchImpl<IStatObjectDisp>,
								public IStatObject2,
							  public CUndoneableActionImpl,
								public _dyncreate_t<CStatisticObject>,
								public CColorsNames
	{
		route_unknown();
		enum Warning {
			UserParamRecursion=20101,	
			Calculated=0,
			NotCalculated=-1
		};
		map_meas_params m_map_meas_params;

		bool m_bLockValidate;

		set<NumImg> m_setImages;
		set<NumImg> m_setForGroups;
		set<long> m_setClassesForAll;
		set<long> m_setClassesRelative;
		
		_OBJECT_LIST* m_pObjectList;
		ParDscr parDscrs;
		ListClass	m_listParams;
		ListClass m_listFull;

		ListClass m_listParamsUser;
		ListClass m_listFullUser;

		vector<long> m_vecClasses;
		long m_lclassCount;

		long *m_arrClassesFull;
		long m_lclassCountFull;

//		long m_lParamsCount;
		set<long> m_plUserKeys;
		char m_strClassFile[MAX_PATH];
		
//		typedef map<ClassID, CClass*,lessClassID> ClassMap;
		struct ClassMap:public map<ClassID, CClass*,lessClassID>
		{
			CStatisticObject& m_Stat;
			ClassMap(CStatisticObject& stat):m_Stat(stat){}
			CClass& operator[](ClassID classId)
			{
				NumImg img=classId.img;
				Class nClass=classId.cls;

				if( CalcForRelativeClasses==nClass 
					&& 0==m_Stat.m_setClassesRelative.size() )
				{
					classId.cls=AllClasses;
				}
				else if( CalcForAllClasses==nClass
					&& 0==m_Stat.m_setClassesForAll.size() )
				{
					classId.cls=AllClasses;
				}
				return CClass::TrieveClass(m_Stat,classId);
			}
		};
		ClassMap m_ClassMap;
		
		ILongControllerPtr m_sptrLongController;
	public:

		CStatisticObject();
		virtual ~CStatisticObject();	

		virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
		CStatParamBase* CreateStatParam(CClass& cls, INamedDataPtr sptrND, long lKey );
    long index2group_image( long lidx );
		HRESULT GetParamVal2(ClassID aClass , long lKey, long CalcFrom, double& paramVal);
	protected:
		void deinit();

		//IPersist helper
		virtual GuidKey		GetInternalClassID();
		virtual IUnknown*	DoGetInterface( const IID &iid );
		PARAMS_LIST& _create_param_list( CClass& cls, INamedDataPtr sptrND, PARAMS_LIST *params_list = 0 );

		PARAMS_LIST& _create_param_list_user( CClass& cls, INamedDataPtr sptrND, PARAMS_LIST *params_list = 0 );
		CStatParamBase *_create_param_by_key( CClass& cls, INamedDataPtr sptrND, long lKey );
		CStatParamBase* _create_param_by_key_user(CClass& cls, INamedDataPtr sptrND, long lKey );

		void	_store_settings( INamedDataPtr sptrND );
		void	_load_settings( INamedDataPtr sptrND );
		double	_get_coef( long lKey, BSTR *bstr );
		bool	_has_group( const NumImg lgroup_idx );
	public:
		//ISerializableObject
		com_call			Load( IStream *pStream, SerializeParams *pparams );
		com_call			Store( IStream *pStream, SerializeParams *pparams );

		//ICompatibleObject
		com_call			CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );

		com_call GetValue2( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ double *pfValue );
		com_call GetValueGlobal2( /*[in]*/ long lKey, /*[out]*/ double *pfValue );

		com_call GetValueInUnit2( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ double *pfValue );
		com_call GetValueGlobalInUnit2( /*[in]*/ long lKey, /*[out]*/ double *pfValue );

		//IStatObjectDisp
		com_call GetValue( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue );
		com_call GetValueGlobal( /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue );

		com_call GetValueInUnit( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue );
		com_call GetValueGlobalInUnit( /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue );

		com_call GetClassCount( long *plSz );
		com_call GetClassValue( long lClassID, long *plClassValue );
		
		com_call GetKeyCount( long *plSz );
		com_call GetKeyValue( long lKeyIndex, long *plKeyValue );
		
		com_call get_Name(BSTR *pbstr );
		com_call put_Name(BSTR bstr );

		com_call LoadSettingsFromSD( BSTR bstrSection );
		com_call StoreSettingsToSD( BSTR bstrSection );

		com_call GetPrivateNamedData( /*[out,retval]*/ IDispatch **ppDisp );
		com_call isEmpty( /*[out,retval]*/ char *pchRes );

		com_call SetBagProperty( /*[in]*/ BSTR bstrName, /*[in]*/ VARIANT var );
		com_call GetBagProperty( /*[in]*/ BSTR bstrName, /*[out, retval]*/ VARIANT *var );

		com_call GetStatParamInfo(/*[in]*/ LONG lKey, /*[out, retval]*/ VARIANT* pParamInfo);

		com_call UpdateClassNames( void);

		//IStatObject
		com_call Calculate( IUnknown *sptrO, long lClassDivision );
		com_call GetParamInfo( /*[in] */long lKey, /*[out] */double *pfCoef, /*[out] */BSTR *pbstrUnit );
		com_call SetLongAction( IUnknown *punkAction );
	
		com_call SetValidateLock( BOOL bLOCK  );
		com_call GetValidateLock( BOOL *pbLOCK  );

		//IStatObject2
		com_call Calculate2( IUnknown *punkO, long lClassDivision, DWORD dwFlags );

		protected:
			void _send_notify();
            HRESULT	_calculate_object( IUnknown *punkTable, long lClassDivision, DWORD dwFlags );
		public:
		struct XParamDescr{
			double fCoef;
			CStringW wsUnit;
				long m_lBaseKey;
		}m_XParamDescr;
	private:
		void LoadNewSettings(void);
		void LoadDscrs(INamedDataPtr sptrND);
		friend bool CStatParamUserDefined::Calculate();
public:
	enum Warning m_Warning;
private:
	bool SyncParamKeys(MapPrivateKeys& mapPrivateKeys);
	bool m_bVirtualClasses;
	long m_lVirtClasses;
public:
	void GetVirtualClasses(bool& bVirtualClasses, long& lVirtClasses);
	COLORREF get_class_color( long lpos );
	const char* get_class_name( long lpos );
};

	//////////////////////////////////////////////////////////////////////
	//
	//	class CStatisticObjectInfo
	//
	//////////////////////////////////////////////////////////////////////
	class CStatisticObjectInfo : public CDataInfoBase,
						public _dyncreate_t<CStatisticObjectInfo>
	{
	public:
		CStatisticObjectInfo( );
		com_call GetUserName( BSTR *pbstr );
	};
	namespace ObjectActions
	{
/**********************************************************************************/
		class CActionCreateStatObject : public CAction,
								public CUndoneableActionImpl,
								public _dyncreate_t<CActionCreateStatObject>,
								public CLongOperationImpl,
								public ILongController
		{
			long m_lNotify;
		public:
			route_unknown();	
			virtual IUnknown*	DoGetInterface( const IID &iid );
		public:
			CActionCreateStatObject();
			virtual ~CActionCreateStatObject();

			com_call Abort();

			
			//invoke
			com_call DoInvoke();
			com_call GetActionState( DWORD *pdwState );
			com_call SendNotify();
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionCreateStatObjectInfo : public _ainfo_t<IDS_CREATE_STAT_OBJECT, _dyncreate_t<CActionCreateStatObject>::CreateObject, 0>,
									public _dyncreate_t<CActionCreateStatObjectInfo>
		{
			route_unknown();
		public:
			CActionCreateStatObjectInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetAnydoc; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
		class CActionAssignVirtualClasses : public CAction,
								public _dyncreate_t<CActionAssignVirtualClasses>
		{		   
		public:
			route_unknown();	
		public:
			CActionAssignVirtualClasses();
			virtual ~CActionAssignVirtualClasses();
			
			//invoke
			com_call DoInvoke();
			com_call GetActionState( DWORD *pdwState );
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionAssignVirtualClassesInfo : public _ainfo_t<IDS_ASSIGN_VIRTUAL_CLASS, _dyncreate_t<CActionAssignVirtualClasses>::CreateObject, 0>,
									public _dyncreate_t<CActionAssignVirtualClassesInfo>
		{
			route_unknown();
		public:
			CActionAssignVirtualClassesInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetAnydoc; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
		class CActionShowVirtualClasses : public CAction,
								public _dyncreate_t<CActionShowVirtualClasses>
		{		   
		public:
			route_unknown();	
		public:
			CActionShowVirtualClasses();
			virtual ~CActionShowVirtualClasses();
			
			//invoke
			com_call DoInvoke();
			com_call GetActionState( DWORD *pdwState );
		};


		/////////////////////////////////////////////////////////////////////////////
		class CActionShowVirtualClassesInfo : public _ainfo_t<IDS_SHOW_VIRTUAL_CLASS, _dyncreate_t<CActionShowVirtualClasses>::CreateObject, 0>,
									public _dyncreate_t<CActionShowVirtualClassesInfo>
		{
			route_unknown();
		public:
			CActionShowVirtualClassesInfo(){}
			arg	*args()	{return s_pargs;}
			virtual _bstr_t target()			{return szTargetViewSite; }	
			static arg s_pargs[];
		};
/**********************************************************************************/
	}
	long _user_param_count( INamedDataPtr sptrData );
	long _standart_param_count( INamedDataPtr sptrData );
	long _standart_param_last( INamedDataPtr sptrData );
}

inline RECT ScaleRect( RECT rc, double fScale )
{
	if( fScale == 1 )
		return rc;

	rc.left = long( rc.left * fScale );
	rc.top = long( rc.top * fScale );
	rc.right = long( rc.right * fScale );
	rc.bottom = long( rc.bottom * fScale );

	return rc;
}
