#pragma once

#include "stat_types.h"
#include "\vt5\awin\misc_string.h"
#include "data_main.h"
#include "statistics.h"
#include "stat_consts.h"
#include "clone.h"
#include "statdata_int.h"
#include <ColorsNames.h>
#include "impl_event.h"


//template<class key, class val> class pmap : public map<key,val*>
//{
//public:
//	~pmap(){
//		clear();
//	}
//	void clear(){
//		for(const_iterator it=begin(); it!=end(); ++it) delete it->second;
//		map<key,val*>::clear();
//	}
//	val*& operator[](key parKey){
//		std::pair<iterator,bool> ib=insert(value_type(parKey,(val*)NULL));
//		return ib.first->second;
//	}
//};

//map col number to stat_col_info 
typedef map<long,stat_col_info*> CMapColInfo;

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableObject
//
//////////////////////////////////////////////////////////////////////
class CStatTableObject :	public CObjectBase,
							public CEventListenerImpl,
							public IStatTable,
							public ICompatibleObject,
							public IStatTablePersist,
							public IStatTableDisp,
							public _dyncreate_t<CStatTableObject>,
							public CColorsNames
{
	route_unknown();
public:
	CStatTableObject();
	virtual ~CStatTableObject();	

	//init/deinit
	void				default_init();
	void				deinit();

	// Overrides
	void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

protected:
	//IPersist helper
	virtual GuidKey		GetInternalClassID();
	virtual IUnknown*	DoGetInterface( const IID &iid );

public:
	//ISerializableObject
	com_call			Load( IStream *pStream, SerializeParams *pparams );
	com_call			Store( IStream *pStream, SerializeParams *pparams );
	//INamedDataObject2
	com_call			IsBaseObject(BOOL * pbFlag);

	//INamedDataObject2
	com_call			GetFirstChildPosition(TPOS *plPos);
	com_call			GetNextChild(TPOS *plPos, IUnknown **ppunkChild);


	//IStatTable
	//parameter info
	com_call			GetFirstParamPos( TPOS* plpos );
	com_call			GetNextParam( TPOS* plpos, stat_param** ppparam );
	com_call			GetParamsCount( long* pl_count );
	com_call			AddParam( TPOS lpos_insert_after, stat_param* pparam, TPOS* pl_pos_new );
	com_call			DeleteParam( TPOS lpos );
	com_call			GetParamPosByKey( long lkey, TPOS* pl_pos );

	//group
	com_call			GetFirstGroupPos(TPOS* plpos);
	com_call			GetNextGroup(TPOS* plpos, stat_group** ppgroup);
	com_call			AddGroup(TPOS lpos_insert_after, stat_group* pgroup, TPOS* pl_pos_new);
	com_call			DeleteGroup(TPOS lpos, BOOL bsync_rows);
	com_call			GetGroupCount( long* pl_count );

	//row info
	com_call			GetFirstRowPos(TPOS* pl_pos);
	com_call			GetNextRow(TPOS* pl_pos, stat_row** pprow);
	com_call			GetRowCount( long* pl_count );
	com_call			AddRow(TPOS lpos_insert_after, stat_row* prow, TPOS* pl_pos_new);
	com_call			DeleteRow(TPOS lpos_row);

	//value by param
	com_call			GetValue(TPOS lpos_row, TPOS lpos_param, stat_value** ppvalue);
	com_call			SetValue(TPOS lpos_row, TPOS lpos_param, stat_value* pvalue);
	
	//value by key
	com_call			GetValueByKey(TPOS lpos_row, long lkey, stat_value** ppvalue);
	com_call			SetValueByKey(TPOS lpos_row, long lkey, stat_value* pvalue);

	com_call			ClearCache( );

	STDMETHOD(UpdateClassNames)(void);

	//helper func
	stat_param_ex*		get_param_by_key(long lkey, TPOS* pl_pos);

	//ICompatibleObject
	com_call			CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );

	//IStatTablePersist
	com_call			LoadData( IStream *pStream, long lparam );
	com_call			StoreData( IStream *pStream, long lparam );

	list_param			m_list_params;
	map_param			m_map_params;
	list_group			m_list_group;

	list_row			m_list_row;
	CString				m_str_class_name;


	//IDispatch
    com_call GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo);
	com_call GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
	com_call GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
	com_call Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);

      com_call getFirstParamPos (
        /*[out,retval]*/ VARIANT * pv_pos );
      com_call getNextParamPos (
				/*[in]*/ LONG_PTR lpos,
        /*[out,retval]*/ VARIANT * pv_pos );
	  com_call getParamKey(
		  /*[in]*/ LONG_PTR lpos,
		  /*[out,retval]*/ VARIANT * pv_key);
      com_call getFirstRowPos (
        /*[out,retval]*/ VARIANT * pv_pos );
      com_call getNextRowPos (
				/*[in]*/ LONG_PTR lpos,
		/*[out,retval]*/ VARIANT * pv_pos );
      com_call getRowCount (
        /*[out,retval]*/ VARIANT * pv_count );
      com_call getRowGroup (
				/*[in]*/ LONG_PTR lpos,
        /*[out,retval]*/ VARIANT * pv_group );
      com_call getRowClass (
				/*[in]*/ LONG_PTR lpos,
        /*[in]*/ BSTR class_file,
        /*[out,retval]*/ VARIANT * pv_class );
      com_call deleteRow (
				/*[in]*/ LONG_PTR lpos);
      com_call getValueByKey (
				/*[in]*/ LONG_PTR lpos,
        /*[in]*/ long lkey,
        /*[out,retval]*/ VARIANT * pv_value );
      com_call getValueByParamPos (
				/*[in]*/ LONG_PTR lpos,
				/*[in]*/ LONG_PTR lparam_pos,
        /*[out,retval]*/ VARIANT * pv_value );
      com_call getGroupCount (
        /*[out,retval]*/ VARIANT * pv_count );
			com_call getGroup( /*[in]*/long lpos, /*[out, retval]*/VARIANT* pv_group);
      com_call updateObject ( long lflags );
	  com_call relloadSettings( long lflags );
      com_call GetPrivateNamedData( /*[out,retval]*/ IDispatch **ppDisp );
	  com_call get_Name(/*[out,retval]*/BSTR *pbstr );
	  com_call put_Name(/*[in]*/BSTR bstr );
	  // [vanek] BT2000:4076 - 08.12.2004
	  com_call IsEmpty( /*[out, retval]*/ VARIANT_BOOL *pvbIsEmpty );
	  // [vanek] BT2000:4077 - 08.12.2004
	  com_call GetType( /*[out, retval]*/ BSTR *pbstrType );
		com_call getValueByKeyInUnit( /*[in]*/LONG_PTR lpos, /*[in]*/long lkey, /*[out, retval]*/VARIANT* pv_value);

	ITypeLib*	m_pi_type_lib;
	ITypeInfo*	m_pi_type_info;

	long lNextManMeasure;
	long lNextUserMeasure;
	struct MeasParDscr{
		MeasParDscr(long aKey)
			: lKey(aKey){}
		MeasParDscr(long aKey,ParameterContainer* ppc)
			: lKey(aKey)
			, strName(ppc->pDescr->bstrName)
			, strUserName(ppc->pDescr->bstrUserName)
			, strDefFormat(ppc->pDescr->bstrDefFormat)
			, strUnit(ppc->pDescr->bstrUnit)
			, fCoeffToUnits(ppc->pDescr->fCoeffToUnits)
		{}
		long lKey;
    CStringW strName;
    CStringW strUserName;
    CStringW strDefFormat;
    CStringW strUnit;
    double fCoeffToUnits;
		virtual void Store(INamedData* pND)
		{
			_bstr_t section=_bstr_t( sMainMeasParamsSection "\\" )+strName;
			pND->SetEmptySection(&section.GetBSTR());
			pND->SetValue( _bstr_t(L"Key"), _variant_t(lKey) );
			if(strName.GetLength())
				pND->SetValue( _bstr_t(L"Name"), _variant_t( strName ) );
			if(strUserName.GetLength())
				pND->SetValue( _bstr_t(L"UserName"), _variant_t( strUserName ) );
			if(strDefFormat.GetLength())
				pND->SetValue( _bstr_t(L"Format"), _variant_t( strDefFormat ) );
		}
		virtual ParameterDescriptor_ex* match(map_meas_params& mapParams)
		{
			for(map_meas_params::const_iterator lpos=mapParams.begin(); lpos!=mapParams.end(); ++lpos)
			{
				ParameterDescriptor_ex* pShell=lpos->second;
				if( _bstr_t(strUserName)==_bstr_t(pShell->bstrUserName))
						//&& _bstr_t(strUnit)==_bstr_t(pShell->bstrUnit)
						//&& fCoeffToUnits==pShell->fCoeffToUnits)
				{
					return pShell;
				}
			}
			return 0;
		}
	};

	struct ManMeasParDscr:MeasParDscr{
 		ManMeasParDscr(long aKey)
			: MeasParDscr(aKey){}
		ManMeasParDscr(long aKey,ParameterContainer* ppc)
			: MeasParDscr(aKey,ppc),type(ppc->type){}
    enum ParamType type;
		void Store(INamedData* pND){
			__super::Store(pND);
			pND->SetValue( _bstr_t(L"ParamType"), _variant_t(type ) );
		}
	};
	struct UserMeasParDscr:MeasParDscr{
 		UserMeasParDscr(long aKey)
			: MeasParDscr(aKey),Enable(0),Format(L"%0.3"), 
			GreekSymbol(-1),TableOrder(0),AlwaysCalc(0){}
		UserMeasParDscr(long aKey,ParameterContainer* ppc)
			: MeasParDscr(aKey,ppc){}
		void Store(INamedData* pND){
			__super::Store(pND);
			pND->SetValue( _bstr_t(L"Unit"), _variant_t(strUnit) );
			pND->SetValue( _bstr_t(L"CoeffToUnit"), _variant_t(fCoeffToUnits) );
		}
	//[measurement\parameters\Изрезанность]
	short Enable;
	CStringW Graphics;				// :String=Изрезанность
	CStringW Format;					// :String=%0.3f
	CStringW _ExprString;			// :String=#{131076}=(1+Sqr((1-3.14159*{8}/{2})^2)-3.14159*{8}/{2})/2
	wchar_t GreekSymbol;
	long TableOrder;
	//UsedPanes:String=
	//UsedPhases:String=
	short AlwaysCalc;
};
	struct MapDscrs : public map<long,MeasParDscr*>
	{
		~MapDscrs(){
			for(const_iterator it=begin(); it!=end(); ++it) delete it->second;
		}
	}m_myParDescrs;

	CMapColInfo m_map_col;
private:
	bool SyncParamKeys(MapPrivateKeys& mapPrivateKeys);
};

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableObjectInfo
//
//////////////////////////////////////////////////////////////////////
class CStatTableObjectInfo : public CDataInfoBase,
					public _dyncreate_t<CStatTableObjectInfo>
{
public:
	CStatTableObjectInfo( );
	com_call GetUserName( BSTR *pbstr );
};

