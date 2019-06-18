#pragma once

//#include "\vt5\awin\misc_map.h"
#include <params.h>
//#include "utils.h"


template<class key, class val> class pmap : public map<key,val*>
{
public:
	~pmap(){
		clear();
	}
	void clear(){
		for(const_iterator it=begin(); it!=end(); ++it) delete it->second;
		map<key,val*>::clear();
	}
	val*& operator[](key parKey){
		std::pair<iterator,bool> ib=insert(value_type(parKey,(val*)NULL));
		if(!ib.second)
			delete ib.first->second;
		return ib.first->second;
	}
	val* operator()(const key parKey){
		iterator it=find(parKey);
		if(it!=end())
			return it->second;
		else
			return (val*)0;
	}
};

class ParameterDescriptor_ex;
void init_parameter_descriptor( ParameterDescriptor* ppdescr );
void copy_parameter_descriptor( ParameterDescriptor* psrc, ParameterDescriptor* ptarget );
void release_parameter_descriptor( ParameterDescriptor* ppdescr );

//////////////////////////////////////////////////////////////////////
//
//	class ParameterDescriptor_ex
//
//////////////////////////////////////////////////////////////////////
class ParameterDescriptor_ex : public ParameterDescriptor
{
public:
	_bstr_t section;
	ParameterDescriptor_ex()
	{		init_parameter_descriptor( this );		}
	virtual ~ParameterDescriptor_ex()
	{		release_parameter_descriptor( this );	}
};


inline void ParameterDescriptor_ex_free( void* pdata )
{
	delete (ParameterDescriptor_ex*)pdata;
};

//typedef _list_map_t<ParameterDescriptor_ex*, long/*param key*/, cmp_long, ParameterDescriptor_ex_free>	map_meas_params;

struct MapPrivateKeys:map<long/*private key*/,long/*shell key*/>
{
	const mapped_type replace(const key_type key)
	{
		const_iterator it=find(key);
		if(it==end())
		{
			return key;
		}
		else
		{
			return it->second;
		}
	}
};

//ParameterDescriptor_ex helpres
//////////////////////////////////////////////////////////////////////
inline void init_parameter_descriptor( ParameterDescriptor* ppdescr )
{
	if( !ppdescr )	return;

	::ZeroMemory( ppdescr, sizeof(ParameterDescriptor) );
}

//////////////////////////////////////////////////////////////////////
inline void copy_parameter_descriptor( ParameterDescriptor* psrc, ParameterDescriptor* ptarget )
{
	if( !psrc || !ptarget )	return;
	ptarget->cbSize			= psrc->cbSize;
	if( psrc->bstrName )
		ptarget->bstrName		= _bstr_t( psrc->bstrName ).copy();
	//if( psrc->bstrName )
	//	ptarget->bstrName		= _bstr_t( psrc->bstrUserName ).copy();
	if( psrc->bstrUserName )
		ptarget->bstrUserName	= _bstr_t( psrc->bstrUserName ).copy();
	if( psrc->bstrDefFormat )
		ptarget->bstrDefFormat	= _bstr_t( psrc->bstrDefFormat ).copy();
	ptarget->lKey			= psrc->lKey;
	ptarget->lEnabled		= psrc->lEnabled;
	ptarget->pos			= psrc->pos;
	//must be filled on ReLoadState()
	if( psrc->bstrUnit )
		ptarget->bstrUnit		= _bstr_t( psrc->bstrUnit ).copy();
	ptarget->fCoeffToUnits	=	psrc->fCoeffToUnits;
	ptarget->lTableOrder = psrc->lTableOrder;
}

typedef pmap<long,ParameterDescriptor_ex> map_meas_params;
//////////////////////////////////////////////////////////////////////
inline void release_parameter_descriptor( ParameterDescriptor* ppdescr )
{
	if( ppdescr->bstrName )
		::SysFreeString( ppdescr->bstrName );		ppdescr->bstrName		= 0;
	if( ppdescr->bstrUserName )
		::SysFreeString( ppdescr->bstrUserName );	ppdescr->bstrUserName	= 0;
	if( ppdescr->bstrDefFormat )
		::SysFreeString( ppdescr->bstrDefFormat );	ppdescr->bstrDefFormat	= 0;
	if( ppdescr->bstrUnit )
		::SysFreeString( ppdescr->bstrUnit );		ppdescr->bstrUnit		= 0;
}

//////////////////////////////////////////////////////////////////////
inline bool CreateParamDescrCache( map_meas_params& map_meas_params )
{
	map_meas_params.clear();

	IUnknown* punk_man = ::_GetOtherComponent( ::GetAppUnknown(), IID_IMeasureManager );
	if( !punk_man )			return false;

	ICompManagerPtr ptr_meas_man = punk_man;
	punk_man->Release();

	if( ptr_meas_man == 0 )	return false;

	int ncount = 0;
	ptr_meas_man->GetCount( &ncount );
	for( int i=0;i<ncount;i++ )
	{
		IUnknownPtr punk;
		ptr_meas_man->GetComponentUnknownByIdx( i, &punk );

		if(IMeasParamGroupPtr ptr_group = punk){
		long lparam_pos = 0;
		ptr_group->GetFirstPos( &lparam_pos );
		while( lparam_pos )
		{
			ParameterDescriptor* pdescr = 0;
			ptr_group->GetNextParam( &lparam_pos, &pdescr );

			ParameterDescriptor_ex* pnew_param = new ParameterDescriptor_ex;
			copy_parameter_descriptor( pdescr, pnew_param );
				map_meas_params[pdescr->lKey]=pnew_param;
		}
	}
		//if(ICalcObjectContainerPtr ptr_cntr = punk)
		//{
		//	long lparam_pos = 0;
		//	ptr_cntr->GetFirstParameterPos( &lparam_pos );
		//	while( lparam_pos )
		//	{
		//		ParameterContainer* pCntr=0;
		//		ptr_cntr->GetNextParameter ( &lparam_pos, &pCntr );
		//		ParameterDescriptor* pdescr = pCntr->pDescr;
		//		if(pdescr->lKey >= MANUAL_PARAMS_FIRST_KEY && pdescr->lKey < MANUAL_PARAMS_LAST_KEY)
		//		{
		//		ParameterDescriptor_ex* pnew_param = new ParameterDescriptor_ex;
		//		copy_parameter_descriptor( pdescr, pnew_param );
		//		map_meas_params.set( pnew_param, pdescr->lKey );
		//	}
		//}
	}
	return true;
}

inline GUID GetExternalGuid( GUID guidInternal )
{
	//PROFILE_TEST( "common.dll\\utils.cpp\\GetExternalGuid" );

	GUID guidExternal = INVALID_KEY;
	IVTApplicationPtr ptrApp( ::GetAppUnknown() );

	if( ptrApp == 0 )
	{
//		ASSERT( false );
		return guidExternal; 
	}

	if( S_OK != ptrApp->GetEntry( 1, (DWORD*)&guidInternal, (DWORD*)&guidExternal, 0 ) )
	{
//		ASSERT( false );
		guidExternal = INVALID_KEY;		
		return guidExternal;		
	}

	return guidExternal;
}

inline IUnknown* GetMeasManualGroup()
		{
	IUnknown* punk = ::_GetOtherComponent( ::GetAppUnknown(), IID_IMeasureManager );
	if( !punk )
		return 0;

	ICompManagerPtr ptrGroupMan( punk );
	punk->Release();	punk = 0;

	if( ptrGroupMan == 0 )
		return 0;

	int	nCount = 0;
	ptrGroupMan->GetCount( &nCount );

	//std_dll GUID GetExternalGuid( GUID guidInternal );

	const GUID CManualMeasGroupGuid = 								
	{0xe3985d92, 0x5c77, 0x4644, {0x8e, 0xa5, 0x97, 0x7, 0x36, 0xf, 0x13, 0x3d}};
	//	{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; 		

	GUID guidManualMeasGroup = GetExternalGuid( CManualMeasGroupGuid );


	for( int idx = 0; idx < nCount; idx++ )
	{
		punk = 0;
		ptrGroupMan->GetComponentUnknownByIdx( idx, &punk );

		if( !punk )
			continue;

		IPersistPtr ptrPersist( punk );
		punk->Release(); punk = 0;

		if( ptrPersist == 0 )
			continue;

		//get external CLSID
		CLSID clsid;
		::ZeroMemory( &clsid, sizeof(CLSID) );
		ptrPersist->GetClassID( &clsid );

		if( guidManualMeasGroup == clsid )
			{
			ptrPersist->AddRef();
			return (IUnknown*)ptrPersist;

			}

	}

	return 0;
}

inline bool CreateNDParamDescrCache(map_meas_params& map_meas_params, IUnknown* pUnk )
{
	if(INamedDataPtr pND=pUnk)
	{
		IUnknownPtr punkMMG(GetMeasManualGroup(),false);
		IMeasParamGroupPtr ptr_group(punkMMG);
		if(ptr_group)
		{
		_bstr_t bstrParamsSection( _T( sMainMeasParamsSection ) );
		long lCount=0;
		HRESULT hr=pND->SetupSection(bstrParamsSection);
		pND->GetEntriesCount(&lCount);
		for( long idx = 0; idx < lCount;
			++idx, hr=pND->SetupSection(bstrParamsSection))
		{
				_bstr_t	section;
				pND->GetEntryName( idx, section.GetAddress());
				HRESULT hr=pND->SetupSection(bstrParamsSection+L"\\"+section);
			_variant_t var;
			pND->GetValue(_bstr_t(sKey),&var); 
				if(VT_EMPTY==var.vt)
					continue;
			hr=VariantChangeType(&var,&var,0,VT_I4);
			if( S_OK==hr )
			{
				long lKey=var.lVal;
				if(lKey >= MANUAL_PARAMS_FIRST_KEY && lKey <= MANUAL_PARAMS_LAST_KEY)
				{
					ParameterDescriptor_ex* pdescr = new ParameterDescriptor_ex;
					pdescr->lKey=lKey;

						var.Clear();
						pND->GetValue( _bstr_t(L"Name"), &var );
						if(VT_BSTR==var.vt){
							pdescr->bstrName=var.Detach().bstrVal;
						}

					var.Clear();
					pND->GetValue( _bstr_t(L"UserName"), &var );
					if(VT_BSTR==var.vt){
						pdescr->bstrUserName=var.Detach().bstrVal;
					}

					var.Clear();
					pND->GetValue( _bstr_t(L"Enable"), &var );
						if(VT_I4==var.vt){
						pdescr->lEnabled=var.Detach().iVal;
					}

					var.Clear();
					pND->GetValue( _bstr_t(L"Format"), &var );
					if(VT_BSTR==var.vt){
						pdescr->bstrDefFormat=var.Detach().bstrVal;
					}

					var.Clear();
					pND->GetValue( _bstr_t(L"ParamType"), &var );

					ptr_group->GetUnit(var,&pdescr->bstrUnit,&pdescr->fCoeffToUnits);

						pdescr->section=section;
						map_meas_params[pdescr->lKey]=pdescr;
				}
					else if(MEAS_USER_PARAMS_FIRST_KEY<=lKey && lKey<=MEAS_USER_PARAMS_LAST_KEY)
				{
					ParameterDescriptor_ex* pdescr = new ParameterDescriptor_ex;
					pdescr->lKey=lKey;

						var.Clear();
						pND->GetValue( _bstr_t(L"Name"), &var );
						if(VT_BSTR==var.vt){
							pdescr->bstrName=var.Detach().bstrVal;
						}

					var.Clear();
					pND->GetValue( _bstr_t(L"UserName"), &var );
					if(VT_BSTR==var.vt){
						pdescr->bstrUserName=var.Detach().bstrVal;
					}

					var.Clear();
					pND->GetValue( _bstr_t(L"Format"), &var );
					if(VT_BSTR==var.vt){
						pdescr->bstrDefFormat=var.Detach().bstrVal;
					}

					var.Clear();
					pND->GetValue( _bstr_t(L"Unit"), &var );
					if(VT_BSTR==var.vt){
						pdescr->bstrUnit=var.Detach().bstrVal;
					}

					var.Clear();
					pND->GetValue( _bstr_t(L"CoeffToUnit"), &var );
					pdescr->fCoeffToUnits=1.;
					if(var.vt!=VT_EMPTY){
						hr=VariantChangeType(&var,&var,0,VT_R8);
						if( S_OK==hr ){
							if(VT_R8==var.vt){
								pdescr->fCoeffToUnits=var.dblVal;
							}
						}
					}
						pdescr->section=section;
						map_meas_params[pdescr->lKey]=pdescr;
				}
			}
		}
	}
	return true;
}
	_ASSERTE("Not found parameter description!");
	return false;
}

inline bool CreateAllParamDescrCache(map_meas_params& map_meas_params, IUnknown* pUnk )
{
		if(CreateParamDescrCache(map_meas_params))
			return CreateNDParamDescrCache(map_meas_params, pUnk);
		else
			return false;
}

inline HRESULT AppendParamsFromND(IUnknown* pUnk, MapPrivateKeys& mapPrivateKeys)
{
	if(INamedDataPtr pND=pUnk)
	{
		map_meas_params m_myParDescrs;
		if(!CreateNDParamDescrCache(m_myParDescrs, pUnk ))
			return S_FALSE;
		map_meas_params mapMeasParams;
		INamedDataPtr pShellND(::GetAppUnknown());
		if(!CreateNDParamDescrCache(mapMeasParams, pShellND))
			return S_FALSE;

		// find free keys for manual measurement and user measurement params
		long lNextManualParamKey=MANUAL_PARAMS_FIRST_KEY;
		long lNextMeasUserParamKey=MEAS_USER_PARAMS_FIRST_KEY;
		for(map_meas_params::const_iterator it=mapMeasParams.begin(); it!=mapMeasParams.end(); ++it)
		{
			ParameterDescriptor_ex*const pShell=it->second;
			long lKey=pShell->lKey;
			if( MANUAL_PARAMS_FIRST_KEY<=lKey && lKey <= MANUAL_PARAMS_LAST_KEY){
				lNextManualParamKey=lKey+1;
			}else if(MEAS_USER_PARAMS_FIRST_KEY<=lKey && lKey<=MEAS_USER_PARAMS_LAST_KEY){
				lNextMeasUserParamKey=lKey+1;
			}
		}	

		IUnknownPtr punkMMG(GetMeasManualGroup(),false);
		IMeasParamGroupPtr ptr_group(punkMMG);
//		IMeasParamGroupPtr ptr_group=GetMeasManualGroup();
		map<_bstr_t,ParamType> mapS2ParType;
		//enum ParamType parTypes {
		//{	etUndefined, etLinear, etCounter, etAngle };
		for(ParamType itType=etUndefined; itType<=etAngle; itType=ParamType((itType)+1) )
		{
			_bstr_t bstrUnit;
			double fCoef;
			HRESULT hr=ptr_group->GetUnit(itType,bstrUnit.GetAddress(),&fCoef);
			mapS2ParType[bstrUnit]=itType;
		}

		_bstr_t bstrParamsSection(_T(sMainMeasParamsSection));
		for(map_meas_params::const_iterator it=m_myParDescrs.begin(); it!=m_myParDescrs.end(); ++it)
		{
			ParameterDescriptor_ex* pNDPar=it->second;
			long lNDKey=pNDPar->lKey;
			
			_bstr_t bstrNameND;
			if(pNDPar->bstrName && *pNDPar->bstrName)
				 bstrNameND=pNDPar->bstrName;
			else{
				bstrNameND=pNDPar->section;
			}

			if( MANUAL_PARAMS_FIRST_KEY<=lNDKey && lNDKey <= MANUAL_PARAMS_LAST_KEY)
			{
				for(map_meas_params::const_iterator itShell=mapMeasParams.begin(); itShell!=mapMeasParams.end(); ++itShell)
				{
					ParameterDescriptor_ex* pShell=itShell->second;
					long lKey=pShell->lKey;
					if( MANUAL_PARAMS_FIRST_KEY<=lKey && lKey <= MANUAL_PARAMS_LAST_KEY
//						&& _bstr_t(pNDPar->bstrName)==_bstr_t(pShell->bstrName)
						&& _bstr_t(pNDPar->bstrUserName)==_bstr_t(pShell->bstrUserName)
						&& _bstr_t(pNDPar->bstrUnit)==_bstr_t(pShell->bstrUnit)
						&& pNDPar->fCoeffToUnits==pShell->fCoeffToUnits)
					{
						goto ManMeasParameterExist;
					}
				}
				{
					long lNewKey=lNextManualParamKey++;

					_bstr_t section=pNDPar->bstrName;
					if(0==section.length())
						section=pNDPar->bstrUserName;

					section=bstrParamsSection + L"\\" + section;
					HRESULT hr=pShellND->SetEmptySection(&section.GetBSTR());

					pShellND->SetValue( _bstr_t(L"Key"), _variant_t(lNewKey) );
					if(pNDPar->bstrName && *pNDPar->bstrName)
						pShellND->SetValue( _bstr_t(L"Name"), _variant_t( pNDPar->bstrName ) );
					if(pNDPar->bstrUserName && *pNDPar->bstrUserName)
						pShellND->SetValue( _bstr_t(L"UserName"), _variant_t( pNDPar->bstrUserName ) );
					if(pNDPar->bstrDefFormat && *pNDPar->bstrDefFormat)
						pShellND->SetValue( _bstr_t(L"Format"), _variant_t( pNDPar->bstrDefFormat ) );
					pShellND->SetValue( _bstr_t(L"ParamType"), _variant_t(mapS2ParType[_bstr_t(pNDPar->bstrUnit)]));
					ParameterDescriptor_ex* pShell=new ParameterDescriptor_ex;
					copy_parameter_descriptor(pNDPar,pShell);
					pShell->lKey=lNewKey;
					mapMeasParams[lNewKey]=pShell;
					mapPrivateKeys[pNDPar->lKey]=lNewKey;
				}
ManMeasParameterExist:;
			}
			else if(MEAS_USER_PARAMS_FIRST_KEY<=lNDKey && lNDKey<=MEAS_USER_PARAMS_LAST_KEY)
			{
				for(map_meas_params::const_iterator itShell=mapMeasParams.begin(); itShell!=mapMeasParams.end(); ++itShell)
				{
					ParameterDescriptor_ex* pShell=itShell->second;
					long lKey=pShell->lKey;
					if(MEAS_USER_PARAMS_FIRST_KEY<=lKey && lKey<=MEAS_USER_PARAMS_LAST_KEY
//						&& _bstr_t(pNDPar->bstrName)==_bstr_t(pShell->bstrName)
						&& _bstr_t(pNDPar->bstrUserName)==_bstr_t(pShell->bstrUserName)
//						&& _bstr_t(pNDPar->bstrUnit)==_bstr_t(pShell->bstrUnit)
//						&& pNDPar->fCoeffToUnits==pShell->fCoeffToUnits)
					){
						goto MeasUserParameterExist;
					}
				}
				{
					long lNewKey=lNextMeasUserParamKey++;

					_bstr_t section=pNDPar->bstrName;
					if(0==section.length())
						section=pNDPar->bstrUserName;

					section=bstrParamsSection + L"\\" + section;
					HRESULT hr=pShellND->SetEmptySection(&section.GetBSTR());

					pShellND->SetValue( _bstr_t(L"Key"), _variant_t(lNewKey) );
					if(pNDPar->bstrName && *pNDPar->bstrName)
						pShellND->SetValue( _bstr_t(L"Name"), _variant_t( pNDPar->bstrName ) );
					if(pNDPar->bstrUserName && *pNDPar->bstrUserName)
						pShellND->SetValue( _bstr_t(L"UserName"), _variant_t( pNDPar->bstrUserName ) );
					if(pNDPar->bstrDefFormat && *pNDPar->bstrDefFormat)
						pShellND->SetValue( _bstr_t(L"Format"), _variant_t( pNDPar->bstrDefFormat ) );
					ParameterDescriptor_ex* pShell=new ParameterDescriptor_ex;
					copy_parameter_descriptor(pNDPar,pShell);
					pShell->lKey=lNewKey;
					mapMeasParams[lNewKey]=pShell;
					mapPrivateKeys[pNDPar->lKey]=lNewKey;
				}
MeasUserParameterExist:;
			}
		}

		{
			::SetValue(pShellND,secMeasurement,MANUAL_NEXT_KEY, lNextManualParamKey);
			::SetValue(pShellND,secMeasurement,MEASUSER_NEXT_KEY, lNextMeasUserParamKey);
		}

		for(MapPrivateKeys::const_iterator it=mapPrivateKeys.begin(); it!=mapPrivateKeys.end(); ++it)
		{
			long keyPrivate=it->first;
			long keyShell=it->second;
			if(keyPrivate!=keyShell)
			{
				ParameterDescriptor_ex* pNDPar=m_myParDescrs(keyPrivate);
				pND->SetValue(pNDPar->section+_bstr_t(L"\\Key"), _variant_t(keyShell));
			}
		}
		return S_OK;
	}else{
//		_ASSERTE(!"Object hasn't INamedData interface");
		return S_FALSE;
	}
}