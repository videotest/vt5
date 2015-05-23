#pragma once
#include "clone.h"
#include <data_main.h>
#include <impl_event.h>
#include "idispatchimpl.h"
#include "PropBagImpl.h"
#include <ColorsNames.h>

#import <statobject.tlb> no_namespace raw_interfaces_only named_guids 

enum TableStatus{
	comparable=0,
	noTable=1,
	noStatParam=2,
	noClassMatch=4,
	noClassifyParam=8
};
inline TableStatus& operator &= (TableStatus& s, const short bits)
	{ return (TableStatus&)((short&)s &= bits);}
inline TableStatus& operator |= (TableStatus& s, const short bits)
{ return (TableStatus&)((short&)s |= bits);}
inline TableStatus operator | (const TableStatus s1, const TableStatus s2)
	{return (TableStatus)(short(short(s1)|short(s2)));}

namespace ObjectSpace
{

	class CCmpStatObjectInfo : public CDataInfoBase,
		public _dyncreate_t<CCmpStatObjectInfo>
	{
	public:
		CCmpStatObjectInfo( );
		com_call GetUserName( BSTR *pbstr );
	};

	class CCmpStatObject :
		public CObjectBase,
		public CEventListenerImpl,
		public ICompatibleObject,
		public CNamedPropBagImpl,
		public ::IDispatchImpl<ICmpStatObjectDisp>,
	//	public ICmpStatObject,
//		public CUndoneableActionImpl,
		public _dyncreate_t<CCmpStatObject>,
		public CColorsNames
	{
		route_unknown();
		CCmpStatObject(void);
		~CCmpStatObject(void);
		void deinit();

		// CObjectBase
		GuidKey GetInternalClassID();
		IUnknown* DoGetInterface( const IID &iid );

		// ICompatibleObject
		com_call CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );

		// ISerializableObject
		STDMETHOD(Load)( IStream *pStream, SerializeParams *pparams );
		STDMETHOD(Store)( IStream *pStream, SerializeParams *pparams );

		// ICmpStatObjectDisp
		STDMETHOD(get_Statuses)(VARIANT* pVal);
		STDMETHOD(put_Statuses)(VARIANT newVal);
		//STDMETHOD(get_Standard)(VARIANT* pVal);
		//STDMETHOD(put_Standard)(VARIANT newVal);
		STDMETHOD(Compare)(LONG* rc);
		STDMETHOD(GetPrivateNamedData) ( /*[out,retval]*/ IDispatch **ppDisp );
//	private:
		struct StatTable{
			IStatTablePtr _pTable;
			IStatObjectPtr _pStat;
			TableStatus _status;
			bool _bCompare;
			COLORREF _clrCmpStat;
			int _iHatch;
			StatTable (){}
			StatTable(IStatTablePtr pTable, const TableStatus status, const bool bCompare)
				: _pTable(pTable), _status(status), _bCompare(bCompare){}
		};
		typedef map<CComBSTR,StatTable> StatTables;
		StatTables _statTables;
		typedef vector<std::pair<CComBSTR,StatTable> > StatObjects;
		StatObjects _statObjects;

		struct  
		{
			VARIANT_BOOL _bShow;
			double _mx;
			double _div;
		} _standard;
	private:
		double _valMin,_valMax;
		bool _bMinMaxTrue;
		HRESULT ClassifyTables(const vector<IStatTable*>& tablesNew);
	};

}

#define szTypeCmpStatObject "CmpStatObject"
