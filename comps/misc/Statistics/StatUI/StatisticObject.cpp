#include "StdAfx.h"
#include "resource.h"
#include "statisticobject.h"
#include "statui_consts.h"
#include "misc_utils.h"
#include "misc.h"

#include "statui_int.h"

void _delete_param( CStatParamBase *pObject ) {	delete pObject; }
//void _delete_list( PARAMS_LIST *pObject ) {	delete pObject; }

inline void FireEvent( IUnknown *punkCtrl, const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( punkCtrl );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}

inline double _geom_series( double first, double fStep, int n )
{
	if( n == 0 )
		return first;
	return fStep * _geom_series( first, fStep, n - 1 );
}

inline long _virtual_classify( INamedDataObject2Ptr sptrDNO, long lSuperClass )
{
	long lBaseKey = GetValueInt( ::GetAppUnknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, 0 );

	double fMin = 1e307, fMax = -1e307;
	TPOS lPos = 0;
	sptrDNO->GetFirstChildPosition( &lPos );
	while( lPos )
	{
		IUnknown *punkChild = 0;
		sptrDNO->GetNextChild( &lPos, &punkChild );
		
		if( punkChild == 0 )
			continue; 

		ICalcObjectPtr sptrCalc = punkChild;
		punkChild->Release();

		if( sptrCalc == 0 )
			continue;

		long lClass = get_object_class( sptrCalc );

		if( lClass != lSuperClass )
			continue;

		double fVal = 0;
		if( sptrCalc->GetValue( lBaseKey, &fVal ) != S_OK )
			continue;
		
		if( fVal < fMin )
			fMin = fVal;

		if( fVal > fMax )
			fMax = fVal;
	}


	long lDivClass   = ::GetValueInt(    ::GetAppUnknown(), SECT_STATUI_VIRTUAL_CLASSES_DIVISION_ROOT, DIVISION_CLASS_COUNT, 10 );
	long lDivType    = ::GetValueInt(    ::GetAppUnknown(), SECT_STATUI_VIRTUAL_CLASSES_DIVISION_ROOT, DIVISION_TYPE,        0 );
	double fDivValue = ::GetValueDouble( ::GetAppUnknown(), SECT_STATUI_VIRTUAL_CLASSES_DIVISION_ROOT, DIVISION_VALUE,       0 );

	long lRetClass = 0;
	
	double fStep = ( fMax - fMin ) / ( lDivClass ? lDivClass : 1 );

	if( lDivType == 0 && fDivValue != 0 )
		fStep = fDivValue;

	sptrDNO->GetFirstChildPosition( &lPos );
	while( lPos )
	{
		IUnknown *punkChild = 0;
		sptrDNO->GetNextChild( &lPos, &punkChild );
		
		if( punkChild == 0 )
			continue; 

		ICalcObjectPtr sptrCalc = punkChild;
		punkChild->Release();

		if( sptrCalc == 0 )
			continue;

		long lClass = get_object_class( sptrCalc );

		if( lClass != lSuperClass )
		{
			set_object_class( sptrCalc, -1, VIRTUAL_STAT_CLASSIFIER );
			continue;
		}

		double fVal = 0;
		if( sptrCalc->GetValue( lBaseKey, &fVal ) != S_OK )
			continue;
		
		if( lDivType == 0 ) // нормально
		{
			long i = 0;
			while( ( fMin + i * fStep ) < fMax )
			{
				if( fVal >= ( fMin + i * fStep ) && fVal <= ( fMin + (i+1) * fStep ) )
				{
					set_object_class( sptrCalc, i, VIRTUAL_STAT_CLASSIFIER );
					if( i > lRetClass )
						lRetClass = i + 1;
					break;
				}
				if( i > lRetClass )
					lRetClass = i + 1;
				i++;
			}
		}
		else
		{
			long i = 0;
			while( _geom_series( fMin, fDivValue, i ) < fMax )
			{
				if( fVal >= _geom_series( fMin, fDivValue, i ) && fVal < _geom_series( fMin, fDivValue, i + 1 ) )
				{
					set_object_class( sptrCalc, i, VIRTUAL_STAT_CLASSIFIER );
					if( i > lRetClass )
						lRetClass = i + 1;
					break;
				}
				if( i > lRetClass )
					lRetClass = i + 1;
				i++;
			}
		}
	}
	return lRetClass;
}

inline void _virtual_classify( INamedDataObject2Ptr sptrDNO, IStatObjectDispPtr sptrStat, long lSuperClass )
{
	long lBaseKey = GetValueInt( ::GetAppUnknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, 0 );

	POSITION lPos = 0;
	sptrDNO->GetFirstChildPosition( &lPos );
	while( lPos )
	{
		IUnknown *punkChild = 0;
		sptrDNO->GetNextChild( &lPos, &punkChild );
		
		if( punkChild == 0 )
			continue; 

		ICalcObjectPtr sptrCalc = punkChild;
		punkChild->Release();

		if( sptrCalc == 0 )
			continue;

		long lClass = get_object_class( sptrCalc );

		if( lClass != lSuperClass )
		{
			set_object_class( sptrCalc, -1, VIRTUAL_STAT_CLASSIFIER );
			continue;
		}

		double fVal = 0;
		if( sptrCalc->GetValue( lBaseKey, &fVal ) != S_OK )
			continue;

		long lSz = 0;
		sptrStat->GetClassCount( &lSz );

		bool bOK = false;
		for( long i = 0; i < lSz; i++ )
		{
			double fMin = 1e307, fMax = -1e307;

			HRESULT hr = sptrStat->GetValue2( i, PARAM_MIN, &fMin );
			HRESULT hr2 = sptrStat->GetValue2( i, PARAM_MAX, &fMax );

			if( hr == S_OK && hr2 == S_OK )
			{
				if( fVal >= fMin && fVal <= fMax )
				{
					long lClass = 0;
					sptrStat->GetClassValue( i, &lClass );
					set_object_class( sptrCalc, lClass, VIRTUAL_STAT_CLASSIFIER );
					bOK = true;
					break;
				}
			}
		}
		if( !bOK )
			set_object_class( sptrCalc, -1, VIRTUAL_STAT_CLASSIFIER );
	}
}


//////////////////////////////////////////////////////////////////////
//
//	class CStatisticObject
//
//////////////////////////////////////////////////////////////////////
namespace ObjectSpace
{
CStatisticObject::CStatisticObject() : IDispatchImpl<IStatObjectDisp>( "statobject.tlb" ),
	m_pObjectList(NULL), m_ClassMap(*this), m_Warning(NotCalculated)
	, m_bVirtualClasses(false)
	, m_lVirtClasses(-1)
{
	m_bLockValidate = false;
	m_vecClasses.clear();
	m_lclassCount = 0;

	m_arrClassesFull = 0;
	m_lclassCountFull = 0;
//	m_lParamsCount = 0;
	m_plUserKeys.clear();

	INamedDataPtr sptrDN = Unknown();

	if( sptrDN == 0 )
	{
		INamedDataObject2Ptr sptrND = Unknown();
		sptrND->InitAttachedData();
	}
//	LoadNewSettings();

	Register( ::GetAppUnknown(), true, szEventNewSettings );

}
//////////////////////////////////////////////////////////////////////
CStatisticObject::~CStatisticObject()
{
	deinit();

	Register( ::GetAppUnknown(), false, szEventNewSettings );
}

void CStatisticObject::deinit()
{
	m_listParams.clear();
	m_listFull.clear();

/*User 1*/
	m_listParamsUser.clear();
	m_listFullUser.clear();
/*User 2*/

	m_vecClasses.clear();
	m_lclassCount = 0;

	if( m_arrClassesFull )
		delete []m_arrClassesFull;
	m_arrClassesFull = 0;
	m_lclassCountFull = 0;

	m_plUserKeys.clear();

	// delete all params in all classes
	for(ClassMap::iterator it=m_ClassMap.begin(); it!=m_ClassMap.end(); ++it)
	{
		it->second->clear();	// kills parameters in class
		delete it->second;		// kills Classes
	}
	m_ClassMap.clear();
}

//////////////////////////////////////////////////////////////////////
//IPersist helper
//////////////////////////////////////////////////////////////////////
GuidKey CStatisticObject::GetInternalClassID()
{
	return clsidStatObject;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CStatisticObject::DoGetInterface( const IID &iid )
{
	if( iid == IID_ICompatibleObject ) 
		return (ICompatibleObject*)this;	
	else if( iid == __uuidof( IStatObjectDisp ) ) 
		return (IStatObjectDisp*)this;	
	else if( iid == IID_IStatObject ) 
		return (IStatObject*)this;	
	else if( iid == IID_IStatObject2 ) 
		return (IStatObject2*)this;	
	else if( iid == IID_IDispatch ) 
		return (IDispatch *)this;	
	else if( iid == IID_IEventListener )
		return (IEventListener*)this;
	else if( iid == IID_INamedPropBag ) 
		return (INamedPropBag*)this;	

	else if( iid == IID_INamedPropBagSer ) 
		return (INamedPropBag*)this;	
	else 
		return CObjectBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
//interface ISerializableObject
//////////////////////////////////////////////////////////////////////

DWORD GetStreamPos( IStream* pStream )
{	
	if( !pStream )
		return -1;
	LARGE_INTEGER	liPos={0};
	ULARGE_INTEGER	uiCurPos;	
	if( S_OK != pStream->Seek( liPos, STREAM_SEEK_CUR, &uiCurPos ) )
		return -1;
	return uiCurPos.LowPart;	
}

HRESULT CStatisticObject::Load( IStream *pStream, SerializeParams *pparams )
{
	TIME_TEST( "CStatisticObject::Load()" );

	deinit();

	HRESULT hr = CObjectBase::Load( pStream, pparams );
	if( S_OK != hr )
		return hr;

	ULONG nRead = 0;
	//At first version control
	long nVersion  = 1;
	pStream->Read( &nVersion, sizeof(long), &nRead );

	pStream->Read( &m_lclassCount, sizeof(long), &nRead );

	m_vecClasses.clear();

#ifdef _DEBUG
#define DBGString(a, b, c) \
	{CString s; s.Format("%d %s %d\n",(a),(b),(c)); OutputDebugString(s);}
DWORD base=GetStreamPos( pStream );
#else
#define DBGString(a, b, c) 
#endif
DBGString(GetStreamPos(pStream), "m_lclassCount ", m_lclassCount)

	for( long lC = 0; lC < m_lclassCount; lC++ ){
		long lClass;
		pStream->Read( &lClass, sizeof(long), &nRead );
		m_vecClasses.push_back(lClass);
	}
	_ASSERT(m_vecClasses.size()==m_lclassCount);

	CClass& cls=m_ClassMap[ClassID(AllClasses,AllImages)];
//	PARAMS_LIST& listMain = _create_param_list(cls, Unknown(), 0 );

	for( long lC = 0; lC < m_lclassCount; lC++ )
	{
		long lCount = 0;
		pStream->Read( &lCount, sizeof(long), &nRead );

//		PARAMS_LIST *pList = new PARAMS_LIST;

		ClassID classId(Class(m_vecClasses[lC]),AllImages);
		CClass& cls=m_ClassMap[classId];
		for( long i = 0; i < lCount; i++ )
		{
			long lKey = 0;
			pStream->Read( &lKey, sizeof(long), &nRead );
//			_ASSERTE(STAT_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_PARAMS_LAST_KEY);

			long lSize = 0;
			pStream->Read( &lSize, sizeof(long), &nRead );

//			PARAMS_LIST::const_iterator it=listMain.find(lKey);
//			pObject;
			if( CStatParamBase* pObject=&cls.Param(lKey) )
			{
//				pObject->CopyParams( it->second );
				pObject->Load( pStream, pparams );
			}
			else
			{
				LARGE_INTEGER li={lSize};
				pStream->Seek(li, STREAM_SEEK_CUR, NULL);
			}
		}
		m_listParams.insert(&cls);
	}

/*User 1*/
	if( nVersion >= 2 )
	{
		m_plUserKeys.clear();
		long lParamsCount;
		pStream->Read( &lParamsCount, sizeof(long), &nRead );

DBGString(GetStreamPos(pStream)-base, "lParamsCount ", lParamsCount)

		for( long lC = 0; lC < lParamsCount; lC++ ){
			long lUserKey=0;
			pStream->Read( &lUserKey, sizeof(long), &nRead );
			m_plUserKeys.insert(lUserKey);
		}

		if( m_plUserKeys.size() && m_lclassCount)
		{
			for( long lC = 0; lC < m_lclassCount; lC++ )
			{
				ClassID classId(Class(m_vecClasses[lC]),AllImages);
				CClass& cls=m_ClassMap[classId];
				long lCount = 0;
				pStream->Read( &lCount, sizeof(long), &nRead );

				for( long i = 0; i < lCount; i++ )
				{
					long lKey = 0;
					pStream->Read( &lKey, sizeof(long), &nRead );
					_ASSERTE(STAT_USER_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_USER_PARAMS_LAST_KEY);

					long lSize = 0;
					pStream->Read( &lSize, sizeof(long), &nRead );

					if( CStatParamBase *pObject=_create_param_by_key_user(cls,Unknown(),lKey ) )
					{
						pObject->Load( pStream, pparams );
					}
					else
					{
						LARGE_INTEGER li={lSize};
						pStream->Seek(li, STREAM_SEEK_CUR, NULL);
					}
				}

				m_listParamsUser.insert(&cls);
			}
		}
	}
/*User 2*/

	pStream->Read( &m_lclassCountFull, sizeof(long), &nRead );

DBGString(GetStreamPos(pStream)-base, "m_lclassCountFull ", m_lclassCountFull)

	if( m_arrClassesFull )
		delete []m_arrClassesFull;
	m_arrClassesFull = 0;

	if( m_lclassCountFull > 0 )
		m_arrClassesFull = new long[m_lclassCountFull];

	for( long lC = 0; lC < m_lclassCountFull; lC++ )
		pStream->Read( &m_arrClassesFull[lC], sizeof(long), &nRead );

	for( long lC = 0; lC < m_lclassCountFull; lC++ )
	{
		long lCount = 0;
		pStream->Read( &lCount, sizeof(long), &nRead );

		ClassID classId(CalcForAllClasses,AllImages);
		CClass& cls=m_ClassMap[classId];
		for( long i = 0; i < lCount; i++ )
		{
			long lKey = 0;
			pStream->Read( &lKey, sizeof(long), &nRead );

			long lSize = 0;
			pStream->Read( &lSize, sizeof(long), &nRead );

//			PARAMS_LIST::const_iterator it=listMain.find(lKey);
			if(CStatParamBase* pObject=&cls.Param(lKey))
			{
//				pObject->CopyParams( it->second );
				pObject->Load( pStream, pparams );
			}
			else
			{
				LARGE_INTEGER li={lSize};
				pStream->Seek(li, STREAM_SEEK_CUR, NULL);
			}
		}
		m_listFull.insert(&cls);
	}

/*User 1*/
	if( nVersion >= 2 )
	{

DBGString(GetStreamPos(pStream)-base, "m_lclassCountFull ", m_lclassCountFull)

		if( m_plUserKeys.size() )
		{
			for( long lC = 0; lC < m_lclassCountFull; lC++ )
			{
				long lCount = 0;
				pStream->Read( &lCount, sizeof(long), &nRead );

				if( lCount > 0 ) {

					ClassID classId(CalcForAllClasses,AllImages);
					CClass& cls=m_ClassMap[classId];
				for( long i = 0; i < lCount; i++ )
				{
					long lKey = 0;
					pStream->Read( &lKey, sizeof(long), &nRead );
					_ASSERTE(STAT_USER_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_USER_PARAMS_LAST_KEY);

					long lSize = 0;
					pStream->Read( &lSize, sizeof(long), &nRead );

					if( CStatParamBase *pObject=_create_param_by_key_user(cls,Unknown(),lKey ) )
					{
						pObject->Load( pStream, pparams );
					}
					else
					{
						LARGE_INTEGER li={lSize};
						pStream->Seek(li, STREAM_SEEK_CUR, NULL);
					}
				}
					m_listFullUser.insert(&cls);
				}
			}
		}
	}
/*User 2*/

DBGString(GetStreamPos(pStream)-base, "m_lclassCountFull ", m_lclassCountFull)

	if( nVersion >= 3 )
	{
		INamedPropBagSerPtr sptrSer = Unknown();
		if( sptrSer )
			sptrSer->Load( pStream );
	}

	m_Warning=Calculated;
	MapPrivateKeys mapPrivateKeys;
	hr=AppendParamsFromND(Unknown(),mapPrivateKeys);
	if(!hr){
		SyncParamKeys(mapPrivateKeys);
	}
	LoadNewSettings();
	LoadColorsNames();

	return S_OK;
}

bool CStatisticObject::SyncParamKeys(MapPrivateKeys& mapPrivateKeys)
{
	if(INamedDataPtr pND=Unknown())
	{
		long keyPrivate=::GetValueInt(Unknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY,0);
		long keyShell=mapPrivateKeys.replace(keyPrivate);
		if(keyPrivate!=keyShell){
			::SetValue(Unknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, keyShell);
		}
		keyPrivate=::GetValueInt(Unknown(), SECT_STATUI_CHART_AXIS_ROOT, Y_PARAM_KEY,0);
		keyShell=mapPrivateKeys.replace(keyPrivate);
		if(keyPrivate!=keyShell){
			::SetValue(Unknown(), SECT_STATUI_CHART_AXIS_ROOT, Y_PARAM_KEY, keyShell);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatisticObject::Store( IStream *pStream, SerializeParams *pparams )
{
	TIME_TEST( "CStatisticObject::Store()" );

	HRESULT hr = CObjectBase::Store( pStream, pparams );
	if( S_OK != hr )
		return hr;

	ULONG nWritten = 0;

	//At first version control
	long nVersion  = 3;
	pStream->Write( &nVersion, sizeof(long), &nWritten );

	pStream->Write( &m_lclassCount, sizeof(long), &nWritten );

DWORD base=GetStreamPos( pStream );
DBGString(GetStreamPos(pStream), "m_lclassCount ", m_lclassCount)

	for (vector<long>::const_iterator it=m_vecClasses.begin();it!=m_vecClasses.end();it++)
		pStream->Write( &(*it), sizeof(long), &nWritten );

	_ASSERTE(m_lclassCount==m_listParams.size());
	for( ListClass::iterator lpos = m_listParams.begin(); lpos!=m_listParams.end(); ++lpos )
	{
		PARAMS_LIST& list = **lpos;
		PARAMS_LIST::const_iterator itLastParamNext=list.upper_bound(STAT_PARAMS_LAST_KEY);
		long lCount=0; 
		for(PARAMS_LIST::const_iterator it=list.begin(); it!=itLastParamNext; ++it) ++lCount;
		pStream->Write( &lCount, sizeof(long), &nWritten );
		for(PARAMS_LIST::const_iterator it=list.begin(); it!=itLastParamNext; ++it)
		{
			CStatParamBase *pObject = it->second;

			long lKey = pObject->GetKey();
			_ASSERTE(STAT_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_PARAMS_LAST_KEY);
			pStream->Write( &lKey, sizeof(long), &nWritten );

			long lSize = pObject->GetSerializeSize();
			pStream->Write( &lSize, sizeof(long), &nWritten );

			pObject->Store( pStream, pparams );
		}
	}

/*User 1*/
	long lParamsCount=m_plUserKeys.size();
	pStream->Write( &lParamsCount, sizeof(long), &nWritten );

DBGString(GetStreamPos(pStream)-base, "lParamsCount ", lParamsCount)

	for( set<long>::const_iterator it=m_plUserKeys.begin(); it!=m_plUserKeys.end(); ++it){
		long lUserKey=*it;
		pStream->Write( &lUserKey, sizeof(long), &nWritten );
	}
	if( m_plUserKeys.size() > 0 )
	{
		_ASSERTE(m_lclassCount==m_listParamsUser.size());
		for( ListClass::iterator lpos = m_listParamsUser.begin(); lpos!=m_listParamsUser.end(); ++lpos )
		{
			PARAMS_LIST& list = **lpos;
			PARAMS_LIST::const_iterator itFirstUserParam=list.lower_bound(STAT_USER_PARAMS_FIRST_KEY);
			long lCount=0; 
			for(PARAMS_LIST::const_iterator it=itFirstUserParam; it!=list.end(); ++it) ++lCount;
			pStream->Write( &lCount, sizeof(long), &nWritten );
			for(PARAMS_LIST::const_iterator it=itFirstUserParam; it!=list.end(); ++it)
			{
				CStatParamBase *pObject = it->second;

				long lKey = pObject->GetKey();
				_ASSERTE(STAT_USER_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_USER_PARAMS_LAST_KEY);
				pStream->Write( &lKey, sizeof(long), &nWritten );

				long lSize = pObject->GetSerializeSize();
				pStream->Write( &lSize, sizeof(long), &nWritten );

				pObject->Store( pStream, pparams );
			}
		}
	}
/*User 2*/

	pStream->Write( &m_lclassCountFull, sizeof(long), &nWritten );

DBGString(GetStreamPos(pStream)-base, "m_lclassCountFull ", m_lclassCountFull)

	for( long lC = 0; lC < m_lclassCountFull; lC++ )
		pStream->Write( &m_arrClassesFull[lC], sizeof(long), &nWritten );

	for( ListClass::iterator lpos = m_listFull.begin(); lpos!=m_listFull.end(); ++lpos )
	for( long lC = 0; lC < m_lclassCountFull; lC++ )
	{
		PARAMS_LIST& list = **lpos;
		PARAMS_LIST::const_iterator itLastParamNext=list.upper_bound(STAT_PARAMS_LAST_KEY);;
		long lCount=0; 
		for(PARAMS_LIST::const_iterator it=list.begin(); it!=itLastParamNext; ++it) ++lCount;
		pStream->Write( &lCount, sizeof(long), &nWritten );
		for(PARAMS_LIST::const_iterator it=list.begin(); it!=itLastParamNext; ++it)
		{
			CStatParamBase *pObject = it->second;

			long lKey = pObject->GetKey();
			_ASSERTE(STAT_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_PARAMS_LAST_KEY);
			pStream->Write( &lKey, sizeof(long), &nWritten );

			long lSize = pObject->GetSerializeSize();
			pStream->Write( &lSize, sizeof(long), &nWritten );

			pObject->Store( pStream, pparams );
		}
	}

/*User 1*/

DBGString(GetStreamPos(pStream)-base, "m_lclassCountFull ", m_lclassCountFull)

	if( m_plUserKeys.size() > 0 )
	{
		for( ListClass::iterator lpos = m_listFullUser.begin(); lpos!=m_listFullUser.end(); ++lpos )
		{
			PARAMS_LIST& list = **lpos;
			PARAMS_LIST::iterator itFirstUserParam=list.lower_bound(STAT_USER_PARAMS_FIRST_KEY);
			long lCount=0;
			for(PARAMS_LIST::const_iterator it=itFirstUserParam; it!=list.end(); ++it) ++lCount;
			pStream->Write( &lCount, sizeof(long), &nWritten );
			for(PARAMS_LIST::const_iterator it=itFirstUserParam; it!=list.end(); ++it)
			{
				CStatParamBase *pObject = it->second;

				long lKey = pObject->GetKey();
				_ASSERTE(STAT_USER_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_USER_PARAMS_LAST_KEY);
				pStream->Write( &lKey, sizeof(long), &nWritten );

				long lSize = pObject->GetSerializeSize();
				pStream->Write( &lSize, sizeof(long), &nWritten );

				pObject->Store( pStream, pparams );
			}
		}
	}
/*User 2*/

DBGString(GetStreamPos(pStream)-base, "m_lclassCountFull ", m_lclassCountFull)

	INamedPropBagSerPtr sptrSer = Unknown();
	if( sptrSer )
		sptrSer->Store( pStream );

	return S_OK;
}

//ICompatibleObject
//////////////////////////////////////////////////////////////////////
HRESULT CStatisticObject::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableObjectInfo
//
//////////////////////////////////////////////////////////////////////
CStatisticObjectInfo::CStatisticObjectInfo():CDataInfoBase( clsidStatObject, szTypeStatObject, 0, IDI_STAT_OBJECT )
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatisticObjectInfo::GetUserName( BSTR *pbstr )
{
	if( !pbstr )
		return E_POINTER;

	if( !m_bstrUserName.length() )
	{
		
		CStringW sz_buf;
		sz_buf.LoadString( App::handle(), IDS_STAT_OBJECT_TYPE_NAME);
		m_bstrUserName	= sz_buf;
	}

	*pbstr = m_bstrUserName.copy();

	return S_OK;
}

//#include "\vt5\awin\profiler.h"
struct CImageInfo
{
	int cx, cy;
	double fCalibr;
	GuidKey imageG;
	double fCutArea;
	CImageInfo()
	{
		cx = cy = 0;
		fCalibr = 0;
		fCutArea = 0;
	}
	CImageInfo( const CImageInfo &info )
	{
		*this = info;
	}
	
	void operator=( const CImageInfo &info )
	{
		imageG = info.imageG;
		cx = info.cx;
		cy = info.cy;
		fCalibr = info.fCalibr;
		fCutArea = info.fCutArea;
	}
};

HRESULT CStatisticObject::Calculate( IUnknown *punkTable, long lClassDivision )
{
	return _calculate_object( punkTable, lClassDivision, 0 );     	
}

void CStatisticObject::_send_notify()
{
	if( m_sptrLongController )
		m_sptrLongController->SendNotify();
}

HRESULT	CStatisticObject::_calculate_object( IUnknown *punkTable, long lClassDivision, DWORD dwFlags )
{
	//	TIME_TEST( "CStatisticObject::Calculate - ALL" );
	INamedDataPtr pTableND(punkTable);
	if(!punkTable)
		_ASSERTE(!"It is not stat table");
	CopyObjectNamedData( punkTable, Unknown(), _T( sMainMeasParamsSection ), true );

	strcpy( m_strClassFile, (char*)::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", 0 ) );
	_load_settings( ::GetAppUnknown() );

	LoadNewSettings();


	IUnknownPtr sptrO = punkTable;
	
	// delete all params in all classes
	for(ClassMap::iterator it=m_ClassMap.begin(); it!=m_ClassMap.end(); ++it)
	{
		it->second->clear();	// kills parameters in class
		delete it->second;		// kills Classes
	}
	m_ClassMap.clear();
/*User 1*/
	m_listParamsUser.clear();
	m_listFullUser.clear();
/*User 2*/

	m_vecClasses.clear();
	m_lclassCount = 0;


	INamedDataObject2Ptr sptrDNO = sptrO;

	if( sptrDNO == 0 )
		return E_FAIL;

	// [vanek] : сваливаем, если не указано по каким изображениям считать (CalcForGroups = "-1") - 17.11.2004
	if( m_setForGroups.size() == 1 && *m_setForGroups.begin() == -1 )
		return E_FAIL;

	// [vanek] BT2000:4238 сваливаем, если не указано по каким классам считать (CalcForAllClasses = "-2") - 03.01.2005
	if( m_setClassesForAll.size() == 1 && *m_setClassesForAll.begin() == -2 )
		return E_FAIL;

	long lPos = 0;

	_bstr_t bstrClassFile = m_strClassFile;
	long lClassC = 0;
	if( lClassDivision != -1 )
	{
		lClassC = _virtual_classify( sptrDNO, lClassDivision );

		bstrClassFile = full_classifiername( VIRTUAL_STAT_CLASSIFIER );
		::SetValue( ::GetAppUnknown(), "\\Classes", "ClassFile", (char*)bstrClassFile );
		INamedPropBagPtr sptrProp = Unknown();
		if( sptrProp )
		{
			sptrProp->SetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), _variant_t( 1L ) );
			sptrProp->SetProperty( _bstr_t( VIRTUAL_MAIN_CLASS ), _variant_t( lClassDivision ) );
		}
	}
	else
	{
		INamedPropBagPtr sptrProp = Unknown();
		if( sptrProp )
			sptrProp->SetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), _variant_t( 0L ) );
	}

	{
		INamedPropBagPtr sptrProp = Unknown();
		if( sptrProp )
			sptrProp->SetProperty( _bstr_t( STAT_VERSION ), _variant_t( STAT_VERSION_NUM ) );
	}

//	_send_notify();

	set<long> arrClasses;
	m_pObjectList=new _OBJECT_LIST();
	{
//		TIME_TEST( "CStatisticObject::Calculate - fill list" );
		long lBaseKey = ::GetValueInt( ::GetAppUnknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, 0 );

		IStatTablePtr sptrTable = sptrDNO;

		TPOS lPos_BaseParam = 0;
		POSITION lPos_AreaParam = 0;
		POSITION lPos_LengthParam = 0;
		POSITION lPos_PerimParam = 0;

		sptrTable->GetParamPosByKey( 0, &lPos_AreaParam );	
		sptrTable->GetParamPosByKey( 12, &lPos_LengthParam );	
		sptrTable->GetParamPosByKey( 2, &lPos_PerimParam );	
		
		if( lBaseKey == 0 )
			lPos_BaseParam = lPos_AreaParam;
		else if( lBaseKey == 12 )
			lPos_BaseParam = lPos_LengthParam;
		else if( lBaseKey == 2 )
			lPos_BaseParam = lPos_PerimParam;
		else
			sptrTable->GetParamPosByKey( lBaseKey, &lPos_BaseParam );	

		if( !lPos_BaseParam )
			return E_FAIL;

		MapGUID map_indexes;
		map<GUID,NumImg,LessGUID> mapGuidGroup2Img;
		{
			TPOS lPosGroup = 0;
			sptrTable->GetFirstGroupPos( &lPosGroup );
			
			while( lPosGroup )
			{
				stat_group *pgroup = 0;
				sptrTable->GetNextGroup( &lPosGroup, &pgroup );
				NumImg iImg=map_indexes.insert(MapGUID::value_type(pgroup->guid_image,(NumImg)map_indexes.size())).first->second;
				mapGuidGroup2Img.insert(map<GUID,NumImg,LessGUID>::value_type(pgroup->guid_group,iImg));
			}
		}

		TPOS lPosRow = 0;
		sptrTable->GetFirstRowPos(&lPosRow); 
		while( lPosRow ) 
		{ 
			stat_row *pRow = 0; 
			TPOS lChild = lPosRow; 
			sptrTable->GetNextRow(&lPosRow, &pRow);

			IUnknown *punkO = 0;
			{
				TPOS lP = lChild;
				sptrDNO->GetNextChild(&lP, &punkO );
			}

			ICalcObjectPtr sptrCalc = punkO;
			if( punkO )
				punkO->Release(); punkO = 0;

			long lclass = get_object_class( sptrCalc, (char*)bstrClassFile ); 

			if( lclass == -1 )
				continue;

			double fVal = std::numeric_limits<double>::quiet_NaN(); 
			
			stat_value *pvalue = 0;
			sptrTable->GetValue( lChild, lPos_BaseParam, &pvalue );
			if( pvalue && pvalue->bwas_defined)
				fVal = pvalue->fvalue;

			double fArea = std::numeric_limits<double>::quiet_NaN();
			double fLength = std::numeric_limits<double>::quiet_NaN();
			double fPerim = std::numeric_limits<double>::quiet_NaN();

			if( lPos_AreaParam )
			{
				if( lPos_BaseParam == lPos_AreaParam )
					fArea = fVal;
				else
				{
					pvalue = 0;
					sptrTable->GetValue( lChild, lPos_AreaParam, &pvalue );
					if(pvalue && pvalue->bwas_defined)
						fArea = pvalue->fvalue;
				}
			}

			if( lPos_LengthParam )
			{
				if( lPos_BaseParam == lPos_LengthParam )
					fLength = fVal;
				else
				{
					pvalue = 0;
					sptrTable->GetValue( lChild, lPos_LengthParam, &pvalue );
					if(pvalue && pvalue->bwas_defined)
						fLength = pvalue->fvalue;
				}
			}

			if( lPos_PerimParam )
			{
				if( lPos_BaseParam == lPos_PerimParam )
					fPerim = fVal;
				else
				{
					pvalue = 0;
					sptrTable->GetValue( lChild, lPos_PerimParam, &pvalue );
					if(pvalue && pvalue->bwas_defined)
						fPerim = pvalue->fvalue;
				}
			}
			NumImg numImg=mapGuidGroup2Img[pRow->guid_group];
			if(
				!m_setClassesForAll.size()
				|| !m_setClassesRelative.size()
				|| m_setClassesForAll.find(lclass)!=m_setClassesForAll.end()
				|| m_setClassesRelative.find(lclass)!=m_setClassesRelative.end() )
			{
				if(!m_setForGroups.size() 
					|| m_setForGroups.find(numImg)!=m_setForGroups.end())
				{
					double QNan=std::numeric_limits<double>::quiet_NaN();
					if(!_isnan(fVal))
					{
						m_pObjectList->add_tail( new XObjectInfo(numImg, fVal, lclass, fArea, fLength, fPerim ) );
						arrClasses.insert(lclass);
					}
				}
			}
		}

		// correct list of images
		{
			m_setImages.clear();
			for(MapGUID::const_iterator it=map_indexes.begin(); it!=map_indexes.end(); ++it)
			{
				if(!m_setForGroups.size() || m_setForGroups.find((NumImg)it->second)!=m_setForGroups.end())
				{
					m_setImages.insert((NumImg)it->second);
				}
			}
		}
	}


	// [vanek] : нет ни одного отклассифицированного объекта и не установлен cfValidateIfNoClass, то выходим - 02.12.2004
	if( !arrClasses.size() && !(dwFlags & cfValidateIfNoClass) )
		return E_FAIL;

	if( lClassDivision == -1 )
	{
		lClassC = class_count( (char*)bstrClassFile );
	}

	for( long i = 0; i < lClassC; i++ )
	{
		arrClasses.insert(i);
	}
	m_vecClasses.insert(m_vecClasses.end(),arrClasses.begin(),arrClasses.end());
	m_lclassCount=m_vecClasses.size();
	
	if( lClassDivision != -1 )
	{
		INamedPropBagPtr sptrProp = Unknown();
		if( sptrProp )
			sptrProp->SetProperty( _bstr_t( VIRTUAL_CLASS_COUNT ), _variant_t( m_lclassCount ) );
	}

//	_send_notify();

	_list_t<CImageInfo> listImages;
	{
		IStatTablePtr sptrTable = sptrDNO;

		TPOS lPos = 0;
		sptrTable->GetFirstGroupPos( &lPos );

		NumImg lgroup_idx = AllImages;        		

		while( lPos )
		{
			stat_group *pgroup = 0;
			
			sptrTable->GetNextGroup( &lPos, &pgroup );
			lgroup_idx ++;

			bool bNew = true;
			for (TPOS lp = listImages.head(); lp; lp = listImages.next(lp))
			{
				CImageInfo info = listImages.get( lp );
				
				if( info.imageG == pgroup->guid_image )
				{
					bNew = false;
					break;
				}
			}

			// check group
      if( bNew && !_has_group( lgroup_idx ) )
          continue;	// skip image			

			if( bNew )
			{
				CImageInfo info;
				info.imageG = pgroup->guid_image;

				_bstr_t bstr_guid;
				{
					BSTR bstr = 0;
					::StringFromCLSID( pgroup->guid_image, &bstr );
					if( bstr )
					{
						bstr_guid = bstr;
						CoTaskMemFree(bstr); bstr = 0;
					}
				}					

				_bstr_t bstr_ol;
				{
					BSTR bstr = 0;
					::StringFromCLSID( pgroup->guid_object_list, &bstr );
					if( bstr )
					{
						bstr_ol = bstr;
						CoTaskMemFree(bstr); bstr = 0;
					}
				}					

				_bstr_t bstr_image_path=
					_T("\\"BASE_PARAMS_SECTION"\\"BASE_PARAMS_IMAGE"\\")+bstr_guid;
				_bstr_t bstr_ol_path=
					_T("\\"BASE_PARAMS_SECTION"\\"BASE_PARAMS_OBJECTLIST"\\")+bstr_ol;

				char sz_buf[20];

				sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_WIDTH );
				info.cx = ::GetValueInt( sptrDNO, bstr_image_path, sz_buf, 0 );

				sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_HEIGHT );
				info.cy = ::GetValueInt( sptrDNO, bstr_image_path, sz_buf, 0 );

				sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_CALIBR );
				info.fCalibr = ::GetValueDouble( sptrDNO, bstr_image_path, sz_buf, 1.);

				sprintf( sz_buf, "%d", BASE_PARAM_CUT_AREA );
				info.fCutArea = ::GetValueDouble( sptrDNO, bstr_ol_path, sz_buf, 0 );

				listImages.add_tail(info);
			}
		}
	}
	
	INamedPropBagPtr sptrProp = Unknown();
	if( sptrProp )
	{
		double fArea = 0;
		double fUsedArea = 0;

		long lCount = 0; for (TPOS lp = listImages.head(); lp; lp = listImages.next(lp), lCount++)
		{
			CImageInfo info = listImages.get( lp );
			double fA2 = info.cx * info.cy * info.fCalibr * info.fCalibr;
			double fA = fA2 - info.fCutArea * info.fCalibr * info.fCalibr;
	
			fArea += fA2;
			fUsedArea += fA;

			CString strName;
			// [vanek] BT2000:4176 - 16.12.2004
			strName.Format( "_used_area_%d", index2group_image(lCount) );

			sptrProp->SetProperty( _bstr_t( strName ), _variant_t( fA ) );

		}

		sptrProp->SetProperty( _bstr_t( "_used_area_" ), _variant_t( fUsedArea ) );
		sptrProp->SetProperty( _bstr_t( "_full_area_" ), _variant_t( fArea ) );
		sptrProp->SetProperty( _bstr_t( "_image_count_" ), _variant_t( listImages.count() ) );
	}

	// Fill and color and name classes
	SetClasses(m_vecClasses, pTableND);
	Save();

//	_send_notify();

	//{
	//	long lMeasKey = GetValueInt( ::GetAppUnknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, 0 ); 
	//	long lpos_map = m_map_meas_params.find( lMeasKey );
	//	if( lpos_map )
	//	{
	//		ParameterDescriptor_ex* pmp = m_map_meas_params.get( lpos_map );
	//		m_XParamDescr.fCoef = pmp->fCoeffToUnits;
	//		m_XParamDescr.wsUnit =pmp->bstrUnit; 
	//		m_XParamDescr.m_lBaseKey = pmp->lKey;
	//	}else{
	//		m_XParamDescr.fCoef = 1.;
	//		m_XParamDescr.wsUnit = L""; 
	//		m_XParamDescr.m_lBaseKey = 0;
	//	}
	//}

	// Make classes
	{
		{
//	create full list;
			CString str = (char*)::GetValueString( Unknown(), SECT_STATUI_STAT_PARAM_ROOT, CALC_FULL_STAT_CLASSES, "-1" );
			CString *strs = 0;
			split( str, ",", &strs, &m_lclassCountFull );
			if( m_lclassCountFull > 0 )
				m_arrClassesFull = new long[m_lclassCountFull];
			for( long lC = 0; lC < m_lclassCountFull; lC++ )
				m_arrClassesFull[lC] = atol( strs[lC] );
			if( strs ) { delete []strs; strs = 0; }

			ClassID classId(AllClasses,AllImages);
			CClass* pClass=&m_ClassMap[classId];
			_create_param_list(*pClass, Unknown());
			m_listFull.insert(pClass);
		}
//	create list
		{
			for( long lC = 0; lC < m_lclassCount; lC++ )
			{
				ClassID classId(Class(m_vecClasses[lC]),AllImages);
				CClass* pClass=&m_ClassMap[classId];
				if(!m_setClassesForAll.size() || 
					m_setClassesForAll.find(m_vecClasses[lC])!=m_setClassesForAll.end())
				{
					_create_param_list(*pClass, Unknown());
				}
				m_listParams.insert(pClass);
			}
		}

	}

//	_send_notify();
/*User 1*/
	{
		TIME_TEST( "CStatisticObject::Calculate - user calculate" );

		// [vanek] BT2000:4236 - 04.01.2005
		if( sptrProp )
		{
			sptrProp->SetProperty( _bstr_t( "_calc_user_params_in_curr_units_" ), 
				_variant_t( (dwFlags & cfCalcUserParamsInUnits) ? 1L : 0L ) );
		}

		{
			ClassID classId(AllClasses,AllImages);
			CClass* pClass=&m_ClassMap[classId];
			_create_param_list_user(*pClass, ::GetAppUnknown());
			m_listFullUser.insert(pClass);
		}
		for( long lC = 0; lC < m_lclassCount; lC++ )
		{
			ClassID classId(Class(m_vecClasses[lC]),AllImages);
			CClass* pClass=&m_ClassMap[classId];
			if(!m_setClassesForAll.size() || 
				m_setClassesForAll.find(m_vecClasses[lC])!=m_setClassesForAll.end())
			{
				_create_param_list_user(*pClass, ::GetAppUnknown());
			}
			m_listParamsUser.insert(pClass);
		}


		int iii=0;
		for(ClassMap::iterator it=m_ClassMap.begin(); it!=m_ClassMap.end(); ++it)
		{
			CClass& cClass=*it->second;
			int ii=0, iii=0;
			for( PARAMS_LIST::const_iterator lpos2 = cClass.begin(); lpos2!=cClass.end(); ++lpos2)
			{
				CStatParamBase *pObject = lpos2->second;
				long key=lpos2->first;
				if( pObject->IsAvailable() ){
					if( !pObject->Calculate() )
					{
						pObject->m_fValue=std::numeric_limits<double>::quiet_NaN();
					}
				}
				ii++;
			}
			iii++;
		}
	}

/*User 2*/
	{
	   INamedPropBagPtr sptrProp = Unknown();
		sptrProp->SetProperty( _bstr_t( "_has_data_" ), _variant_t( 1L ) );
	}

	if( lClassDivision != -1 )
		::SetValue( ::GetAppUnknown(), "\\Classes", "ClassFile", m_strClassFile );

	_store_settings( Unknown() );

	// [vanek] SBT:1231 store guid of statistic table in shell.data - 25.11.2004
	GuidKey guid_stattable;
	guid_stattable = ::GetKey( punkTable );
	if( guid_stattable != INVALID_KEY )
	{
        BSTR bstr = 0;
		::StringFromCLSID( guid_stattable, &bstr );
		if( bstr )
		{
			CString str = bstr;
			::CoTaskMemFree( bstr );
			bstr = 0;

			::SetValue( Unknown(), SECT_STATUI_ROOT, STATTABLE_OWN, str );
		}
	}

	for (_OBJECT_LIST::iterator it=m_pObjectList->begin(); it!=m_pObjectList->end(); ++it)
	{
		delete *it;
	}
	delete m_pObjectList; m_pObjectList=NULL;
	
//	_send_notify();

	{
		HRESULT hr=m_Warning;
		INamedDataPtr sptrND(::GetAppUnknown());
		::SetValue( sptrND, SECT_STATUI_ROOT, "Result", hr );
		m_Warning=Calculated;
		return S_OK;
	}
}

HRESULT CStatisticObject::GetValue( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue )
{
	double fVal = 0;
	 HRESULT hr = GetValue2( lClass, lKey, &fVal );
	*pvarValue = _variant_t( fVal );
	if( hr != S_OK )
		::VariantClear( pvarValue );
	return S_OK;
}

HRESULT CStatisticObject::GetValueGlobal( /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue )
{
	double fVal = 0;
	HRESULT hr = GetValueGlobal2( lKey, &fVal );
	*pvarValue = _variant_t( fVal );

	if( hr != S_OK )
		::VariantClear( pvarValue );

	return S_OK;
}

HRESULT CStatisticObject::GetValueInUnit( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue )
{
	double fVal = 0;
	HRESULT hr = GetValueInUnit2( lClass, lKey, &fVal );
	*pvarValue = _variant_t( fVal );
	if( hr != S_OK )
		::VariantClear( pvarValue );
	return S_OK;
}

HRESULT CStatisticObject::GetValueGlobalInUnit( /*[in]*/ long lKey, /*[out]*/ VARIANT *pvarValue )
{
	double fVal = 0;
	HRESULT hr = GetValueGlobalInUnit2( lKey, &fVal );
	*pvarValue = _variant_t( fVal );
	if( hr != S_OK )
		::VariantClear( pvarValue );
	return S_OK;
}


HRESULT CStatisticObject::GetValueGlobal2( /*[in]*/ long lKey, /*[out]*/ double *pfValue )
{
	if( !pfValue )
		return E_FAIL;

	if( lKey < PARAM_USERDEFINE_MIN_KEY )
	{
		for(ListClass::iterator it=m_listFull.begin(); it!=m_listFull.end(); ++it)
		{
			CClass& cClass=**it;
			PARAMS_LIST::const_iterator lpos2=cClass.find(lKey);
			if( lpos2 != cClass.end() )
				{
					CStatParamBase *pObject = lpos2->second;
					if( pObject->IsAvailable() && pObject->IsCalculated() )
					{
						*pfValue = pObject->GetValue();
						return S_OK;
					}
			}
		}
	}
	else
	{
		static int ss=0;
		for(ListClass::iterator it=m_listFullUser.begin(); it!=m_listFullUser.end(); ++it)
		{
			CClass& cClass=**it;
			PARAMS_LIST::const_iterator lpos2=cClass.find(lKey);
			if( lpos2 != cClass.end() )
			{
				CStatParamBase *pObject = lpos2->second;
				++ss;
				if( pObject->IsAvailable() && pObject->IsCalculated() )
				{
					*pfValue = pObject->GetValue();
					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}

HRESULT CStatisticObject::GetValue2( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ double *pfValue )
{
	if( lClass == -1 )
		return GetValueGlobal2( lKey, pfValue );

	if( !pfValue )
		return E_FAIL;
	static CClass cls(*this,ClassID(Class(lClass),AllImages));
	cls.m_lClass=ClassID(Class(lClass),AllImages);
	if( lKey < PARAM_USERDEFINE_MIN_KEY )
	{
		PARAMS_LIST* pParamList=NULL;
		ListClass::const_iterator itClass=m_listParams.find(&cls);
		if(itClass==m_listParams.end())
					return E_FAIL;
		CClass& cClass=**itClass;
		CClass::const_iterator lpos2=cClass.find(lKey);
		if( lpos2 != cClass.end() )
		{
			CStatParamBase *pObject = lpos2->second;
			if( pObject->IsAvailable() && pObject->IsCalculated() )
			{
				*pfValue = pObject->GetValue();
				return S_OK;
			}
		}
	}
	else
	{
		ListClass::const_iterator itClass=m_listParamsUser.find(&cls);
		if(itClass==m_listParamsUser.end())
		return E_FAIL;
		CClass& cClass=**itClass;
		PARAMS_LIST::const_iterator lpos2=cClass.find(lKey);
		if( lpos2 != cClass.end() )
		{
			CStatParamBase *pObject = lpos2->second;
			if( pObject->IsAvailable() && pObject->IsCalculated() )
			{
				*pfValue = pObject->GetValue();
				return S_OK;
			}
		}
	}

	return E_FAIL;
}
HRESULT CStatisticObject::GetParamInfo( /*[in] */long lKey, /*[out] */double *pfCoef, /*[out] */BSTR *pbstrUnit )
{
	if( !pfCoef && !pbstrUnit )
		return E_FAIL;

	double fc = _get_coef( lKey, pbstrUnit );
	
	if( pfCoef )
		*pfCoef = fc;

	return S_OK;
}

HRESULT CStatisticObject::SetLongAction( IUnknown *punkAction )
{
	m_sptrLongController = punkAction;
	return S_OK;
}


HRESULT CStatisticObject::LoadSettingsFromSD( BSTR bstrSection )
{
	CopyObjectNamedData( ::GetAppUnknown(), Unknown(), (char*)_bstr_t( bstrSection ) , 1 );

	::SetValue( Unknown(), SECT_STATUI_ROOT, "Loaded", 1L );
	return S_OK;
}

HRESULT CStatisticObject::StoreSettingsToSD( BSTR bstrSection )
{
	CopyObjectNamedData( Unknown(), ::GetAppUnknown(), (char*)_bstr_t( bstrSection ), 1 );
	return S_OK;
}

HRESULT CStatisticObject::GetPrivateNamedData( /*[out,retval]*/ IDispatch **ppDisp )
{
	if( !ppDisp )
		return E_FAIL;

	INamedDataPtr sptrND = Unknown();

	if( sptrND == 0 )
	{
		INamedDataObject2Ptr sptrND2 = Unknown();
		sptrND2->InitAttachedData();
		sptrND = Unknown();
	}

	if( m_punkAggrNamedData )
		m_punkAggrNamedData->QueryInterface( IID_IDispatch, (LPVOID *)ppDisp );

	return S_OK;
}


double CStatisticObject::_get_coef( long lKey, BSTR *bstrUnit = 0 )
{
	double fCoef=1.;

	switch(lKey){
	default:
		return 1.;
	case PARAM_DISP:
	case PARAM_DISP_LOGNORM:
		{
			if( bstrUnit ){
				_bstr_t str = _bstr_t(L"(") + m_XParamDescr.wsUnit + L")^2";
				*bstrUnit = str.Detach();
			}
			fCoef=m_XParamDescr.fCoef;
			return fCoef * fCoef;
		}
	case PARAM_SPECCOUNT:
	case PARAM_SPECCOUNT_ERR:
//	case PARAM_NORMAL_COUNT:
		{
			if(ParameterDescriptor_ex* pmp = m_map_meas_params(0))
			{
				fCoef = 1. / pmp->fCoeffToUnits;
				if( bstrUnit ){
					_bstr_t str = _bstr_t(L"1/(") +pmp->bstrUnit + L")";
					*bstrUnit = str.Detach();
				}
			}
			return fCoef;
		}
	case PARAM_AREA:
	case PARAM_USEDAREA:
	case PARAM_FULL_AREA:
	case PARAM_NORMAL_AREA:
		{
			if(ParameterDescriptor_ex* pmp = m_map_meas_params(0))
			{
				fCoef = pmp->fCoeffToUnits;
				if(bstrUnit){
					*bstrUnit = SysAllocString(pmp->bstrUnit);
				}
			}
			return fCoef;
		}
	case PARAM_SPECLENGTH:
	case PARAM_SURF_AREA:
	case PARAM_SPEC_SURF:
		{
			if(ParameterDescriptor_ex* pmp = m_map_meas_params(12))
			{
				fCoef = 1 / pmp->fCoeffToUnits;
				if( bstrUnit ){
					_bstr_t str= _bstr_t(L"1/")+pmp->bstrUnit;
					*bstrUnit = str.Detach();
				}
			}
			return fCoef;
		}
	case PARAM_LENGTH:
	case PARAM_PERIMETR:
	case PARAM_MDL_CHORD:
	case PARAM_MDL_DIST:
		{
			if(ParameterDescriptor_ex* pmp = m_map_meas_params(12))
			{
				fCoef = pmp->fCoeffToUnits;
				if( bstrUnit ){
					*bstrUnit=SysAllocString(pmp->bstrUnit);
				}
			}
			return fCoef;
		}				
	case PARAM_MX:
	case PARAM_MATHXLOG:
	case PARAM_MX_GEOM:
	case PARAM_MX_GORM:
	case PARAM_MIN:
	case PARAM_MAX:
	case PARAM_STDDEV:
		//case PARAM_STD_DEV_LOG:
	case PARAM_MX_ERR:
		//case PARAM_MX_ERR_LOG:
	case PARAM_VAR:
	case PARAM_MX_PROB:
		//case PARAM_MX_PROB_LOG:
	case PARAM_MODE:
	case PARAM_MODE_LOG:
	case PARAM_MEDIANE:
		//case PARAM_SUMLOG:
		//case PARAM_MXLOG:
	case PARAM_MX_LOGNORM:
	case PARAM_MODE_LOGNORM:
		//case PARAM_ASSIMETRY_LOGNORM:
		//case PARAM_EXCESS_LOGNORM:
		{
			if(bstrUnit)
				*bstrUnit = m_XParamDescr.wsUnit.AllocSysString();
			return m_XParamDescr.fCoef;
		}
	}

	return 1.;
}

// [vanek] : определяет принадлежность группы (посылка в таблице данных) к текущему объекту статистики
bool	CStatisticObject::_has_group( const NumImg lgroup_idx )
{
    if( !m_setImages.size() )
		return true;	// объект содержит все группы из таблицы данных (StatTable)

	if( *m_setImages.begin() == -1 )
		return false;	// объект не содержит ни одной группы

	return m_setImages.find(lgroup_idx)!=m_setImages.end();
}

// [vanek] BT2000:4176 получение индекса группы изображения; если групп нет - возвращаем входной индекс lidx - 16.12.2004
long CStatisticObject::index2group_image( long lidx )
{
	if(lidx >= 0 && lidx < (long)m_setImages.size()){
		set<NumImg>::const_iterator it=m_setImages.begin();
		for(long idx=0;
			idx!=lidx;
			++it, ++idx);
		return *it;
	}else{
		return lidx;
	}
}

HRESULT CStatisticObject::GetValueInUnit2( /*[in]*/ long lClass, /*[in]*/ long lKey, /*[out]*/ double *pfValue )
{
	HRESULT hr = GetValue2( lClass, lKey, pfValue );
	if( hr == S_OK )
		*pfValue *= _get_coef( lKey );	
	return hr;
}
HRESULT CStatisticObject::GetValueGlobalInUnit2( /*[in]*/ long lKey, /*[out]*/ double *pfValue )
{
	HRESULT hr = GetValueGlobal2( lKey, pfValue );
	if( hr == S_OK )
		*pfValue *= _get_coef( lKey );	
	return hr;
}

HRESULT CStatisticObject::GetClassCount( long *plSz )
{
	if( !plSz )
		return E_FAIL;

	*plSz = m_lclassCount;

	return S_OK;
}

HRESULT CStatisticObject::GetClassValue( /*[in]*/ long lClassID, /*[out, retval]*/ long *plClassValue )
{
	if( !plClassValue )
		return E_FAIL;

	if( lClassID >= m_lclassCount || lClassID < 0 )
		return E_FAIL;

	*plClassValue = m_vecClasses[lClassID];

	return S_OK;
}

HRESULT CStatisticObject::GetKeyCount( long *plSz )
{
	if( !plSz )
		return E_FAIL;

	*plSz = _standart_param_count( Unknown() ) + m_plUserKeys.size();

	return S_OK;
}

HRESULT CStatisticObject::isEmpty( /*[out,retval]*/ char *pchRes )
{
	if( !pchRes )
		return E_FAIL;


	_variant_t var( 0L );

	INamedPropBagPtr sptrProp = Unknown();
	sptrProp->GetProperty( _bstr_t( "_has_data_" ), &var  );

	if( var.vt != VT_I4 )
	{
		*pchRes = 1;
		return S_FALSE;
	}

	*pchRes = bool( var.lVal == 0 );
	return S_OK;
}

HRESULT CStatisticObject::GetKeyValue( long lKeyIndex, long *plKeyValue )
{
	if( !plKeyValue )
		return E_FAIL;

	if( lKeyIndex < ObjectSpace::_standart_param_count( Unknown() ) )
		*plKeyValue = PARAM_BASE + lKeyIndex;
	else
	{
		lKeyIndex -= _standart_param_count( Unknown() );

		if( lKeyIndex < 0 || lKeyIndex >= (long)m_plUserKeys.size() )
			return E_FAIL;
		set<long>::const_iterator it=m_plUserKeys.begin();
		for(long i=0; i<lKeyIndex; ++i) ++it;
		*plKeyValue = *it;
	}

	return S_OK;
}

HRESULT CStatisticObject::get_Name(BSTR *pbstr )
{
	INamedObject2Ptr sptrO = Unknown();

	if( sptrO )
		sptrO->GetName(	pbstr  );

	return S_OK;
}

HRESULT CStatisticObject::put_Name(BSTR bstr )
{
	INamedObject2Ptr sptrO = Unknown();

	if( sptrO )
		sptrO->SetName(	bstr  );

	return S_OK;
}

HRESULT CStatisticObject::SetBagProperty( /*[in]*/ BSTR bstrName, /*[in]*/ VARIANT var )
{
	INamedPropBagPtr sptrProp = Unknown();

	if( sptrProp == 0 )
		return E_FAIL;

	return sptrProp->SetProperty( bstrName, var );
}

HRESULT CStatisticObject::GetBagProperty( /*[in]*/ BSTR bstrName, /*[out, retval]*/ VARIANT *var )
{
	INamedPropBagPtr sptrProp = Unknown();

	if( sptrProp == 0 )
		return E_FAIL;

	return sptrProp->GetProperty( bstrName, var );
}

HRESULT CStatisticObject::GetStatParamInfo(/*[in]*/ LONG lKey, /*[out, retval]*/ VARIANT* pParamInfo)
{
	CStatParamDscr*& parDscr=(CStatParamDscr*&)parDscrs[lKey];
	if(NULL==parDscr)
		return E_INVALIDARG;
	
	CComSafeArray<VARIANT> array(2);
	array[0]=(BSTR)parDscr->m_szName;
	array[1]=(BSTR)parDscr->m_szModuleOfDesign;

	pParamInfo->vt = VT_ARRAY | VT_VARIANT;
	pParamInfo->parray=array.Detach();
	return S_OK;
}

STDMETHODIMP CStatisticObject::UpdateClassNames(void)
{
	try{
		if(!RestoreByClassifier(m_vecClasses))
		{
			LoadColorsNames();
		}
		return S_OK;
	}catch(_com_error& err){
		return err.Error();
	}
}


PARAMS_LIST& CStatisticObject::_create_param_list( CClass& cls, INamedDataPtr sptrND, PARAMS_LIST *params_list )
	{
	cls.add_tail( new CStatParam_MX(cls,sptrND) );
	cls.add_tail( new CStatParam_MXGeom(cls,sptrND) );
	cls.add_tail( new CStatParam_MXGorm(cls,sptrND) );
	cls.add_tail( new CStatParam_Min(cls,sptrND) );
	cls.add_tail( new CStatParam_Max(cls,sptrND) );
	cls.add_tail( new CStatParam_Var(cls,sptrND) );
	cls.add_tail( new CStatParam_Disp(cls,sptrND) );
	cls.add_tail( new CStatParam_StdDev(cls,sptrND) );
	cls.add_tail( new CStatParam_Mode(cls,sptrND) );
	cls.add_tail( new CStatParam_Mediane(cls,sptrND) );
	cls.add_tail( new CStatParam_Assimetry(cls,sptrND) );
	cls.add_tail( new CStatParam_Excess(cls,sptrND) );
	cls.add_tail( new CStatParam_VarCoeff(cls,sptrND) );

	cls.add_tail( new CStatParam_Count(cls,sptrND) );
	cls.add_tail( new CStatParam_Area(cls,sptrND) );
	cls.add_tail( new CStatParam_Length(cls,sptrND) );
	cls.add_tail( new CStatParam_UsedArea(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecificArea(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecificAreaError(cls,sptrND) );
	cls.add_tail( new CStatParam_AreaPerc(cls,sptrND) );
	cls.add_tail( new CStatParam_AreaPercError(cls,sptrND) );
	cls.add_tail( new CStatParam_NormalArea(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecificCount(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecificCountError(cls,sptrND) );
	cls.add_tail( new CStatParam_CountPerc(cls,sptrND) );
	cls.add_tail( new CStatParam_CountPercError(cls,sptrND) );
	cls.add_tail( new CStatParam_NormalCount(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecificLength(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecificLengthError(cls,sptrND) );
	cls.add_tail( new CStatParam_3DPart(cls,sptrND) );
	cls.add_tail( new CStatParam_MXError(cls,sptrND) );
	cls.add_tail( new CStatParam_MXProb(cls,sptrND) );
	cls.add_tail( new CStatParam_ChiSquare(cls,sptrND) );
	cls.add_tail( new CStatParam_ChiSquareLog(cls,sptrND) );
	cls.add_tail( new CStatParam_ImageCount(cls,sptrND) );
	cls.add_tail( new CStatParam_FullArea(cls,sptrND) );
	cls.add_tail( new CStatParam_Perimeter(cls,sptrND) );
	cls.add_tail( new CStatParam_SurfArea(cls,sptrND) );
	cls.add_tail( new CStatParam_MdlChord(cls,sptrND) );
	cls.add_tail( new CStatParam_MdlDist(cls,sptrND) );
	cls.add_tail( new CStatParam_SurfAreaPerc(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecSurf(cls,sptrND) );
	cls.add_tail( new CStatParam_SpecSurfPerc(cls,sptrND) );

	cls.add_tail( new CStatParam_StdDevLog(cls,sptrND) );
	cls.add_tail( new CStatParam_MXErrorLog(cls,sptrND) );
	cls.add_tail( new CStatParam_MXProbLog(cls,sptrND) );
	cls.add_tail( new CStatParam_ModeLog(cls,sptrND) );
	cls.add_tail( new CStatParam_AssimetryLog(cls,sptrND) );
	cls.add_tail( new CStatParam_ExcessLog(cls,sptrND) );
	cls.add_tail( new CStatParam_VarCoeffLog(cls,sptrND) );

	cls.add_tail( new CStatParam_MXLog2(cls,sptrND) );
	cls.add_tail( new CStatParam_DispLog2(cls,sptrND) );
	
	cls.add_tail( new CStatParam_MXLogNorm( cls,sptrND ) );
	cls.add_tail( new CStatParam_DispLogNorm( cls,sptrND ) );
	cls.add_tail( new CStatParam_ModeLogNorm( cls,sptrND ) );
	cls.add_tail( new CStatParam_AssimetryLogNorm( cls,sptrND ) );
	cls.add_tail( new CStatParam_ExcessLogNorm( cls,sptrND ) );
	cls.add_tail( new CStatParam_Chi2Critic( cls,sptrND ) );
	cls.add_tail( new CStatParam_WeightedAverage( cls,sptrND ) );
	cls.add_tail( new CStatParam_Chi2Prob(cls,sptrND) );
	cls.add_tail( new CStatParam_Chi2ProbLog(cls,sptrND) );

	return cls;
}

CStatParamBase *CStatisticObject::_create_param_by_key( CClass& cls, INamedDataPtr sptrND, long lKey )
{
	CStatParamBase* pParam;
	switch(lKey){
	case PARAM_MX:
		pParam=new CStatParam_MX(cls,sptrND); break;
	case PARAM_MX_GEOM:
		pParam= new CStatParam_MXGeom(cls,sptrND); break;
	case PARAM_MX_GORM:
		pParam= new CStatParam_MXGorm(cls,sptrND); break;
	case PARAM_MIN:
		pParam= new CStatParam_Min(cls,sptrND); break;
	case PARAM_MAX:
		pParam= new CStatParam_Max(cls,sptrND); break;
	case PARAM_VAR:
		pParam= new CStatParam_Var(cls,sptrND); break;
	case PARAM_DISP:
		pParam= new CStatParam_Disp(cls,sptrND); break;
	case PARAM_STDDEV:
		pParam= new CStatParam_StdDev(cls,sptrND); break;
	case PARAM_MODE:
		pParam= new CStatParam_Mode(cls,sptrND); break;
	case PARAM_MEDIANE:
		pParam= new CStatParam_Mediane(cls,sptrND); break;
	case PARAM_ASSIMETRY:
		pParam= new CStatParam_Assimetry(cls,sptrND); break;
	case PARAM_EXCESS:
		pParam= new CStatParam_Excess(cls,sptrND); break;
	case PARAM_COEF_VAR:
		pParam= new CStatParam_VarCoeff(cls,sptrND); break;

	case PARAM_COUNT:
		pParam= new CStatParam_Count(cls,sptrND); break;
	case PARAM_AREA:
		pParam= new CStatParam_Area(cls,sptrND); break;
	case PARAM_LENGTH:
		pParam= new CStatParam_Length(cls,sptrND); break;
	case PARAM_USEDAREA:
		pParam= new CStatParam_UsedArea(cls,sptrND); break;
	case PARAM_SPECAREA:
		pParam= new CStatParam_SpecificArea(cls,sptrND); break;
	case PARAM_SPECAREA_ERR:
		pParam= new CStatParam_SpecificAreaError(cls,sptrND); break;
	case PARAM_AREAPERC:
		pParam= new CStatParam_AreaPerc(cls,sptrND); break;
	case PARAM_AREAPERC_ERR:
		pParam= new CStatParam_AreaPercError(cls,sptrND); break;
	case PARAM_NORMAL_AREA:
		pParam= new CStatParam_NormalArea(cls,sptrND); break;
	case PARAM_SPECCOUNT:
		pParam= new CStatParam_SpecificCount(cls,sptrND); break;
	case PARAM_SPECCOUNT_ERR:
		pParam= new CStatParam_SpecificCountError(cls,sptrND); break;
	case PARAM_COUNTPERC:
		pParam= new CStatParam_CountPerc(cls,sptrND); break;
	case PARAM_COUNTPERC_ERR:
		pParam= new CStatParam_CountPercError(cls,sptrND); break;
	case PARAM_NORMAL_COUNT:
		pParam= new CStatParam_NormalCount(cls,sptrND); break;
	case PARAM_SPECLENGTH:
		pParam= new CStatParam_SpecificLength(cls,sptrND); break;
	case PARAM_SPECLENGTH_ERR:
		pParam= new CStatParam_SpecificLengthError(cls,sptrND); break;
	case PARAM_3DPART:
		pParam= new CStatParam_3DPart(cls,sptrND); break;
	case PARAM_MX_ERR:
		pParam= new CStatParam_MXError(cls,sptrND); break;
	case PARAM_MX_PROB:
		pParam= new CStatParam_MXProb(cls,sptrND); break;
	case PARAM_CHI_SQUARE:
		pParam= new CStatParam_ChiSquare(cls,sptrND); break;
	case PARAM_CHI_SQUARE_LOG:
		pParam= new CStatParam_ChiSquareLog(cls,sptrND); break;
	case PARAM_IMAGE_COUNT:
		pParam= new CStatParam_ImageCount(cls,sptrND); break;
	case PARAM_FULL_AREA:
		pParam= new CStatParam_FullArea(cls,sptrND); break;

	case PARAM_PERIMETR:
		pParam= new CStatParam_Perimeter(cls,sptrND); break;
	case PARAM_SURF_AREA:
		pParam= new CStatParam_SurfArea(cls,sptrND); break;
	case PARAM_MDL_CHORD:
		pParam= new CStatParam_MdlChord(cls,sptrND); break;
	case PARAM_MDL_DIST:
		pParam= new CStatParam_MdlDist(cls,sptrND); break;
	case PARAM_SURFAREAPERC:
		pParam= new CStatParam_SurfAreaPerc(cls,sptrND); break;
	case PARAM_SPEC_SURF:
		pParam= new CStatParam_SpecSurf(cls,sptrND); break;
	case PARAM_SPEC_SURF_PERC:
		pParam= new CStatParam_SpecSurfPerc(cls,sptrND); break;

	case PARAM_STD_DEV_LOG:
		pParam= new CStatParam_StdDevLog(cls,sptrND); break;
	case PARAM_MX_ERR_LOG:
		pParam= new CStatParam_MXErrorLog(cls,sptrND); break;
	case PARAM_MX_PROB_LOG:
		pParam= new CStatParam_MXProbLog(cls,sptrND); break;
	case PARAM_MODE_LOG:
		pParam= new CStatParam_ModeLog(cls,sptrND); break;
	case PARAM_ASSIMETRY_LOG:
		pParam= new CStatParam_AssimetryLog(cls,sptrND); break;
	case PARAM_EXCESS_LOG:
		pParam= new CStatParam_ExcessLog(cls,sptrND); break;
	case PARAM_COEF_VAR_LOG:
		pParam= new CStatParam_VarCoeffLog(cls,sptrND); break;

	case PARAM_DXLOG:
		pParam= new CStatParam_DispLog2(cls,sptrND); break;
	case PARAM_MATHXLOG:
		pParam= new CStatParam_MXLog2(cls,sptrND); break;

	case PARAM_MX_LOGNORM:
		pParam= new CStatParam_MXLogNorm( cls,sptrND ) ; break;
	case PARAM_DISP_LOGNORM:
		pParam= new CStatParam_DispLogNorm( cls,sptrND ); break;
	case PARAM_MODE_LOGNORM:
		pParam= new CStatParam_ModeLogNorm( cls,sptrND ) ; break;
	case PARAM_ASSIMETRY_LOGNORM:
		pParam= new CStatParam_AssimetryLogNorm( cls,sptrND ) ; break;
	case PARAM_EXCESS_LOGNORM:
		pParam= new CStatParam_ExcessLogNorm( cls,sptrND ) ; break;
	case PARAM_CHI2_CRITIC:
		pParam= new CStatParam_Chi2Critic( cls,sptrND ) ; break;
	case PARAM_WEIGHTED_AVERAGE:
		pParam= new CStatParam_WeightedAverage( cls,sptrND ) ; break;
	case PARAM_CHI2_PROB:
		pParam= new CStatParam_Chi2Prob(cls,sptrND); break;
	case PARAM_CHI2_PROB_LOG:
		pParam= new CStatParam_Chi2ProbLog(cls,sptrND); break;
	default:
		_ASSERT (!"Invalid Param key");
		return NULL;
	}
	cls.add_tail(pParam);
	return pParam;
}

CStatParamBase* CStatisticObject::CreateStatParam(CClass& cls, INamedDataPtr sptrND, long lKey )
{
	if(STAT_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_PARAMS_LAST_KEY)
	{
		return _create_param_by_key(cls, sptrND, lKey);
	}else if(STAT_USER_PARAMS_FIRST_KEY<=lKey && lKey<=STAT_USER_PARAMS_LAST_KEY){
		return _create_param_by_key_user(cls, sptrND, lKey);
	}else{
//		_ASSERTE(!"Parameter is not statistical. Verify lKey");
		return NULL;
	}
}

long _user_param_count( INamedDataPtr sptrData )
{
	sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );

	long lCount = 0;
	sptrData->GetEntriesCount( &lCount );

	if( lCount == 0 )
		return 0;

	long lParamsCount = lCount;

	long lSkip = 0;

	for( long i = 0; i < lParamsCount; i++ )
	{
		sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );
		_bstr_t bstrName;
		sptrData->GetEntryName( i, bstrName.GetAddress() );
		sptrData->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) + "\\" + bstrName );
		
		long lCount = 0;
		sptrData->GetEntriesCount( &lCount );

		if( lCount == 0 )
		{
			lSkip++;
			continue;
		}
		if( atol(bstrName) < STAT_USER_PARAMS_FIRST_KEY || atol(bstrName) > STAT_USER_PARAMS_LAST_KEY )
		{
			lSkip++;
			continue;
		}
	}

	lParamsCount -= lSkip;
	return lParamsCount;
}

long _standart_param_count( INamedDataPtr sptrData )
{
	INamedPropBagPtr sptrProp = sptrData;
	_variant_t var;
	if( sptrProp )
		sptrProp->GetProperty( _bstr_t( STAT_VERSION ), &var );

	if( var.vt == VT_EMPTY )
	{
		IApplicationPtr sptrApp = sptrData;
		if( sptrApp == 0 )
			return PARAM_SIZEOF_PARAM_VER5;
		else
			return PARAM_SIZEOF_PARAM;
	}

	if( (long)var == 1L )
		return PARAM_SIZEOF_PARAM_VER1;

	if( (long)var == 2L )
		return PARAM_SIZEOF_PARAM_VER2;

	if( (long)var == 3L )
		return PARAM_SIZEOF_PARAM_VER3;

	if( (long)var == 4L )
		return PARAM_SIZEOF_PARAM_VER4;

	if( (long)var == 5L )
		return PARAM_SIZEOF_PARAM_VER5;

	return PARAM_SIZEOF_PARAM;
}

long _standart_param_last( INamedDataPtr sptrData )
{
	INamedPropBagPtr sptrProp = sptrData;
	_variant_t var;
	if( sptrProp )
		sptrProp->GetProperty( _bstr_t( STAT_VERSION ), &var );

	if( var.vt == VT_EMPTY )
	{
		IApplicationPtr sptrApp = sptrData;
		if( sptrApp == 0 )
			return PARAM_LAST_PARAM_VER5;
		else
			return PARAM_LAST_PARAM;
	}

	if( (long)var == 1L )
		return PARAM_LAST_PARAM_VER1;

	if( (long)var == 2L )
		return PARAM_LAST_PARAM_VER2;

	if( (long)var == 3L )
		return PARAM_LAST_PARAM_VER3;

	if( (long)var == 4L )
		return PARAM_LAST_PARAM_VER4;

	if( (long)var == 5L )
		return PARAM_LAST_PARAM_VER5;

	return PARAM_LAST_PARAM;
}


void CStatisticObject::LoadDscrs(INamedDataPtr sptrND)
{
	sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );

	long lCount = 0;
	sptrND->GetEntriesCount( &lCount );
	long lSkip = 0;

	m_plUserKeys.clear();

	for( long i = 0; i < lCount; i++ )
	{
		sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) );
		_bstr_t bstrName;
		sptrND->GetEntryName( i, bstrName.GetAddress() );
		sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) + L"\\" + bstrName );
		
		long key = _wtoi(bstrName);

		long lCount = 0;
		sptrND->GetEntriesCount( &lCount );

		if( lCount == 0 )
		{
			lSkip++;
			continue;
		}
		if( key < STAT_USER_PARAMS_FIRST_KEY || key > STAT_USER_PARAMS_LAST_KEY )
		{
			lSkip++;
			continue;
		}
		m_plUserKeys.insert(key);
		CStatParamDscrBase*& parDscr=parDscrs[key];
		if(parDscr == NULL)
		{
			parDscr=new CStatUserParamDscr(key);
//			parDscr->LoadSettings(sptrND);
		}
	}
	for(ParDscr::const_iterator it=parDscrs.begin(); it!=parDscrs.end(); ++it)
	{
		CStatParamDscrBase *pDscr = it->second;
		pDscr->LoadSettings( sptrND );
	}
}

/*User 1*/
PARAMS_LIST& CStatisticObject::_create_param_list_user( CClass& cls, INamedDataPtr sptrND, PARAMS_LIST *params_list )
	{
	for( set<long>::const_iterator it=m_plUserKeys.begin(); it!=m_plUserKeys.end(); ++it){
		long key=*it;
		CStatParamDscrBase*& parDscr=parDscrs[key];
		if(parDscr == NULL)
		{
			parDscr=new CStatUserParamDscr(key);
			parDscr->LoadSettings(sptrND);
		}
		std::pair<PARAMS_LIST::iterator,bool>pair=cls.insert(PARAMS_LIST::value_type(key,(CStatParamBase*)NULL));
		if(pair.second){
			(*pair.first).second=new CStatParamUserDefined(cls, sptrND, key );
		}
		_ASSERTE((*pair.first).second);
	}
	return cls;
}

CStatParamBase* CStatisticObject::_create_param_by_key_user(CClass& cls, INamedDataPtr sptrND, long lKey )
{
	if(m_plUserKeys.find(lKey)!=m_plUserKeys.end())
	{
		sptrND->SetupSection( _bstr_t( SECT_STATUI_STAT_PARAM_ROOT ) + L"\\" + _bstr_t(_variant_t(lKey)) );
		std::pair<PARAMS_LIST::iterator,bool>pair=cls.insert(PARAMS_LIST::value_type(lKey,(CStatParamBase*)NULL));
		if(pair.second){
			(*pair.first).second=new CStatParamUserDefined(cls, sptrND, lKey );
		}
//		_ASSERTE((*pair.first).second);
		return (*pair.first).second;
	}
	return 0;
}
/*User 2*/

void CStatisticObject::_load_settings( INamedDataPtr sptrND )
{
	m_setForGroups.clear();
	m_setClassesForAll.clear();
	m_setClassesRelative.clear();

	CString str_images = (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CALC_FOR_GROUPS, "" );

	if( !str_images.IsEmpty() )
	{
		CString *pstrParts = 0;
		long lCount=0;
		split( str_images, ",", &pstrParts, &lCount );
		for( long i = 0; i < lCount; i++ ){
			m_setForGroups.insert(NumImg(atol(pstrParts[i])));
		}
		delete []pstrParts;
	}

	CString str_all_classes = (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CALC_FOR_ALL_CLASSES, "" );

	if( !str_all_classes.IsEmpty() )
	{
		CString *pstrParts = 0;
		long lCount=0;
		split( str_all_classes, ",", &pstrParts, &lCount );
		for( long i = 0; i < lCount; i++ ){
			m_setClassesForAll.insert(atol(pstrParts[i]));
		}
		delete []pstrParts;
	}

	CString strl_relative = (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CALC_FOR_RELATIVE_CLASSES, "" );
	if( !strl_relative.IsEmpty() )
	{
		CString *pstrParts = 0;
		long lCount=0;
		split( strl_relative, ",", &pstrParts, &lCount );
		for( long i = 0; i < lCount; i++ ){
			m_setClassesRelative.insert(atol(pstrParts[i]));
		}
		delete []pstrParts;
	}

	LoadDscrs(sptrND);

	IApplicationPtr sptrP = sptrND;
	if( sptrP == 0 )
		strcpy( m_strClassFile, (char*)::GetValueString( sptrND, SECT_STATUI_ROOT, CLASS_FILE, 0 ) );
}

void CStatisticObject::_store_settings( INamedDataPtr sptrND )
	{
	for(ParDscr::const_iterator it=parDscrs.begin(); it!=parDscrs.end(); ++it)
		{
			CStatParamDscrBase *pDscr = it->second;
			pDscr->StoreSettings( sptrND );
	}
	::SetValue( sptrND, SECT_STATUI_ROOT, CLASS_FILE, _bstr_t( m_strClassFile ) );
}

void CStatisticObject::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
 {
    // [vanek] SBT:1278 на UpdateSystenSettings обновляем только измерительные параметры - 22.12.2004
 	if( !strcmp( pszEvent, szEventNewSettings )  )
 	{
		LoadNewSettings();
 	}
   
 	__super::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
}

HRESULT CStatisticObject::SetValidateLock( BOOL bLOCK  )
{
	m_bLockValidate = bLOCK != 0;
	return S_OK;
}

HRESULT CStatisticObject::GetValidateLock( BOOL *pbLOCK  )
{
	*pbLOCK = m_bLockValidate;
	return S_OK;
}

HRESULT CStatisticObject::Calculate2( IUnknown *punkO, long lClassDivision, DWORD dwFlags )
{
	return _calculate_object( punkO, lClassDivision, dwFlags );
}

void CStatisticObject::LoadNewSettings(void)
{
	INamedDataPtr pND(this);
 	CreateAllParamDescrCache( m_map_meas_params, pND );
	{
		long lMeasKey = GetValueInt( Unknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, 0 ); 
		ParameterDescriptor_ex* pmp = m_map_meas_params(lMeasKey);
		if(!pmp){
			pmp=m_map_meas_params(0);
		}
		if(pmp)
		{
			m_XParamDescr.fCoef = pmp->fCoeffToUnits;
			m_XParamDescr.wsUnit =pmp->bstrUnit; 
			m_XParamDescr.m_lBaseKey = pmp->lKey;
		}else{
			_ASSERTE(!"Undefined X Param");
			m_XParamDescr.fCoef = 1.;
			m_XParamDescr.wsUnit = L""; 
			m_XParamDescr.m_lBaseKey = lMeasKey;
		}
	}
	//if(!LoadColorsNames())
	//{
	//	RestoreByClassifier(m_vecClasses);
	//}
}

HRESULT CStatisticObject::GetParamVal2(ClassID aClass , long lKey, long CalcFrom, double& fValue)
{
	static CClass cls(*this,ClassID(AllClasses,AllImages));
	cls.m_lClass=aClass;
	PARAMS_LIST* pParamList=NULL;
	ListClass& listClass = (AllClasses==aClass.cls)?
		(lKey < PARAM_USERDEFINE_MIN_KEY ? m_listFull : m_listFullUser) :
		(lKey < PARAM_USERDEFINE_MIN_KEY ?  m_listParams : m_listParamsUser);
	ListClass::const_iterator itClass=listClass.find(&cls);
	if(itClass==listClass.end())
		return E_FAIL;
	CClass& cClass=**itClass;
	CClass::const_iterator lpos2=cClass.find(lKey);
	if( lpos2 != cClass.end() )
	{
		CStatParamBase2* pObject = (CStatParamBase2*)lpos2->second;
		if( pObject->IsAvailable() && pObject->IsCalculated2(CalcFrom) )
		{
			try {
			fValue = pObject->GetValue2(CalcFrom);
			return S_OK;
			}catch(...){
				_ASSERTE(!"Not CStatParamBase2");
			}
		}
	}
	return E_FAIL;
}

void CStatisticObject::GetVirtualClasses(bool& bVirtualClasses, long& lVirtClasses)
{
	INamedPropBagPtr sptrBag = this;

	if( sptrBag )
	{
		_variant_t var, var2;
		sptrBag->GetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), &var );
		sptrBag->GetProperty( _bstr_t( VIRTUAL_CLASS_COUNT ), &var2 );

		if( var.vt == VT_I4 && var.lVal == 1L )
		{
			bVirtualClasses = 1;
			lVirtClasses = var2.lVal;
			return;
		}
	}
	bVirtualClasses = 0;
	lVirtClasses = -2;
}

COLORREF CStatisticObject::get_class_color( long lpos )
{
	bool bVirtualClasses;
	long lVirtClasses;
	GetVirtualClasses(bVirtualClasses, lVirtClasses);

	if( bVirtualClasses )
		return get_smooth_class_color( lpos, lVirtClasses, VIRTUAL_STAT_CLASSIFIER );
	return CColorsNames::get_class_color(lpos);
}

const char *CStatisticObject::get_class_name( long lpos )
{
	bool bVirtualClasses;
	long lVirtClasses;
	GetVirtualClasses(bVirtualClasses, lVirtClasses);

	if( bVirtualClasses )
	{
		char *str = _class_buffer();
		sprintf( str, "%ld", lpos + 1 );
		return str;
	}
	return CColorsNames::get_class_name(lpos);
}

namespace ObjectActions
{
/**********************************************************************************/
_ainfo_base::arg CActionCreateStatObjectInfo::s_pargs[] =
{
	{"StatDataName",szArgumentTypeString, "", true, false },
	{"OutputObjectName",szArgumentTypeString, "", true, false },
	{"ClassForDivision",szArgumentTypeInt, "-1", true, false },
	{"ActivateAfterCreate",szArgumentTypeInt, "1", true, false },
	{"ValidateAfterCreate",szArgumentTypeInt, "1", true, false },
	{"Flags",szArgumentTypeInt, "0", true, false },	// IFaces -> statistics.h -> enum CalcFlags 
	{0, 0, 0, false, false },
};

CActionCreateStatObject::CActionCreateStatObject()
{
	m_lNotify = 0;
}

//////////////////////////////////////////////////////////////////////
CActionCreateStatObject::~CActionCreateStatObject()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionCreateStatObject::DoInvoke()
{
	if( m_ptrTarget == 0 )
		return E_FAIL;				   
	
	IDocumentSitePtr sptrDoc = m_ptrTarget;

	if( sptrDoc == 0 )
		return E_FAIL;

	IUnknown *punkView = 0;
	sptrDoc->GetActiveView( &punkView );

	IUnknownPtr sptrContext = punkView;

	if( punkView )
		punkView->Release(); punkView = 0;

	CString strName = (char*)GetArgString( "StatDataName" );

	IUnknown *punkObject = CreateTypedObject( _bstr_t( szTypeStatObject ) );

	bool bActivate = GetArgLong( "ActivateAfterCreate" ) != 0;

	long nClassDiv = GetArgLong( "ClassForDivision" );

	if( !punkObject )
		return E_FAIL;

	IStatObjectDispPtr sptrStatObject = punkObject;
	punkObject->Release();

	if( sptrStatObject == 0 )
		return E_FAIL;

	// [vanek] : сначала получим таблицу данных, а потом уже будем курочить документ - 17.11.2004
	IUnknownPtr spunkStatData;
	if( !strName.IsEmpty() )
		spunkStatData = IUnknownPtr(::GetObjectByName( sptrDoc, _bstr_t( strName ), _bstr_t( szTypeStatTable ) ), false);
	else
		spunkStatData = IUnknownPtr(::GetActiveObjectFromContext( sptrContext, szTypeStatTable ), false);

	if( spunkStatData == 0 )
		return E_FAIL;

	CString strObjectName = (char*)GetArgString( "OutputObjectName" );
	if( !strObjectName.IsEmpty() )
	{
		IUnknown* punk_table_exist = ::GetObjectByName( sptrDoc, _bstr_t( strObjectName ), _bstr_t( szTypeStatObject ) );
		if( punk_table_exist )
		{
			punk_table_exist->Release();
			RemoveFromDocData( m_ptrTarget, punk_table_exist );
		}

		INamedObject2Ptr ptr_no2 = sptrStatObject;
		if( ptr_no2 )
			ptr_no2->SetName( _bstr_t( strObjectName ) );
	}

	IStatObjectPtr sptSt = sptrStatObject;
	// Long oretation

	sptSt->SetLongAction( Unknown() );

	StartNotification(5, 1, 1 );

	sptSt->SetValidateLock( GetArgLong( "ValidateAfterCreate" ) == 0 ); 

	HRESULT hrStatistics;

	// [vanek] BT2000:4112 - 02.12.2004
	IStatObject2Ptr sptrSt2 = sptSt;
	if( sptrSt2 != 0 )
    hrStatistics=sptrSt2->Calculate2( spunkStatData, nClassDiv, (DWORD)(GetArgLong("Flags")) );
	else
		hrStatistics=sptSt->Calculate( spunkStatData, nClassDiv );

	FinishNotification();

	if( !bActivate )
		sptrStatObject->LoadSettingsFromSD( _bstr_t( SECT_STATUI_ROOT ) );

	sptSt->SetLongAction( 0 );

	SetToDocData( m_ptrTarget, sptrStatObject );	


	
	IDataContext3Ptr ptr_dc;
	IDocumentSitePtr ptr_ds( m_ptrTarget );
	if( ptr_ds )
	{
		IUnknown* punk_view = 0;
		ptr_ds->GetActiveView( &punk_view );
		if( punk_view )
		{
			ptr_dc = punk_view;
			punk_view->Release();	punk_view = 0;
		}
	}
	else
		ptr_dc = m_ptrTarget;

	if( ptr_dc == 0 )
		return false;

	if( bActivate )
	{
		ptr_dc->SetActiveObject( _bstr_t( szTypeStatObject ), sptrStatObject, 1 );
		ExecuteAction( "ShowStatView", 0, 0 );
	}

	//sptSt->SetValidateLock( 0 ); 

	return hrStatistics;
}

HRESULT CActionCreateStatObject::Abort()
{
	return S_OK;
}

HRESULT CActionCreateStatObject::GetActionState( DWORD *pdwState )
{
	return __super::GetActionState( pdwState );
}
//////////////////////////////////////////////////////////////////////
IUnknown* CActionCreateStatObject::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else if( iid == IID_ILongOperation )
		return (ILongOperation*)this;
	else if( iid == IID_ILongController )
		return (ILongController*)this;
	
	return __super::DoGetInterface( iid );
}

HRESULT CActionCreateStatObject::SendNotify()
{
	Notify( m_lNotify++ );
	return S_OK;
}

/**********************************************************************************/
_ainfo_base::arg CActionAssignVirtualClassesInfo::s_pargs[] =
{
	{"ObjectListName",szArgumentTypeString, "", true, false },
	{"StatObjectName",szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

CActionAssignVirtualClasses::CActionAssignVirtualClasses()
{
}

//////////////////////////////////////////////////////////////////////
CActionAssignVirtualClasses::~CActionAssignVirtualClasses()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionAssignVirtualClasses::DoInvoke()
{
	if( m_ptrTarget == 0 )
		return E_FAIL;				   

	CString strListName = (char*)GetArgString( "ObjectListName" );
	CString strStatName = (char*)GetArgString( "StatObjectName" );

	IUnknown* punkObjectList = ::GetObjectByName( m_ptrTarget, _bstr_t( strListName ), _bstr_t( szTypeObjectList ) );
	IUnknown* punkStatObject = ::GetObjectByName( m_ptrTarget, _bstr_t( strStatName ), _bstr_t( szTypeStatObject ) );

	IStatObjectDispPtr sptrStat = punkStatObject;
	if( punkStatObject )
		punkStatObject->Release(); punkStatObject = 0;

	INamedDataObject2Ptr sptrList = punkObjectList;
	if( punkObjectList )
		punkObjectList->Release(); punkObjectList = 0;

	if( sptrList == 0 || sptrStat == 0 )
		return E_FAIL;

	INamedPropBagPtr sptrStatProp = sptrStat;
	if( sptrStatProp == 0 )
		return E_FAIL;

	INamedPropBagPtr sptrListProp = sptrList;
	if( sptrListProp == 0 )
		return E_FAIL;

	_variant_t var;
	sptrStatProp->GetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), &var );

	if( var.vt == VT_EMPTY || ( var.vt == VT_I4 && var.lVal == 0 ) )
		return E_FAIL;

	sptrListProp->SetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), var );

	var.Clear();
	sptrStatProp->GetProperty( _bstr_t( VIRTUAL_MAIN_CLASS ), &var );
	sptrListProp->SetProperty( _bstr_t( VIRTUAL_MAIN_CLASS ), var );

	if( var.vt != VT_I4 )
		return E_FAIL;

	long lSuperClass = var.lVal;

	var.Clear();
	sptrStatProp->GetProperty( _bstr_t( VIRTUAL_CLASS_COUNT ), &var );
	sptrListProp->SetProperty( _bstr_t( VIRTUAL_CLASS_COUNT ), var );


	_virtual_classify( sptrList, sptrStat, lSuperClass );

	long l = cncReset;
	FireEvent( m_ptrTarget, szEventChangeObjectList, sptrList, 0, &l, sizeof( l ) );

	return S_OK;
}
HRESULT CActionAssignVirtualClasses::GetActionState( DWORD *pdwState )
{
	return __super::GetActionState( pdwState );
}
/**********************************************************************************/
_ainfo_base::arg CActionShowVirtualClassesInfo::s_pargs[] =
{
	{"bShow",szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};

CActionShowVirtualClasses::CActionShowVirtualClasses()
{
}

//////////////////////////////////////////////////////////////////////
CActionShowVirtualClasses::~CActionShowVirtualClasses()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CActionShowVirtualClasses::DoInvoke()
{
	if( m_ptrTarget == 0 )
		return E_FAIL;				   

	IUnknown* punkObjectList = ::GetActiveObjectFromContext( m_ptrTarget, _bstr_t( szTypeObjectList ) );

	if( !punkObjectList )
		return E_FAIL;

	INamedPropBagPtr sptrListProp = punkObjectList;
	punkObjectList->Release();
	if( sptrListProp == 0 )
		return E_FAIL;

	_variant_t var;
	sptrListProp->GetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), &var );

	if( var.vt != VT_I4 )
		return E_FAIL;

	long nShow = GetArgLong( "bShow" );

	if( nShow == -1 )
		var.lVal = ( var.lVal != 0L ) ? 0L : 1L;
	else
		var.lVal = nShow;

	sptrListProp->SetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), var );

	long l = cncReset;
	FireEvent( m_ptrTarget, szEventChangeObjectList, punkObjectList, 0, &l, sizeof( l ) );

	return S_OK;
}
HRESULT CActionShowVirtualClasses::GetActionState( DWORD *pdwState )
{
	HRESULT hr = __super::GetActionState( pdwState );

	if( hr != S_OK )
		return hr;

	if( m_ptrTarget == 0 )
	{
		*pdwState &= ~afEnabled;
		return hr;
	}
	IImageViewPtr sptrV = m_ptrTarget;
	if( sptrV == 0 )
	{
		*pdwState &= ~afEnabled;
		return hr;
	}

	IUnknown* punkObjectList = ::GetActiveObjectFromContext( m_ptrTarget, _bstr_t( szTypeObjectList ) );

	if( !punkObjectList )
	{
		*pdwState &= ~afEnabled;
		return hr;
	}

	INamedPropBagPtr sptrListProp = punkObjectList;
	punkObjectList->Release();

	if( sptrListProp == 0 )
	{
		*pdwState &= ~afEnabled;
		return hr;
	}

	_variant_t var;
	sptrListProp->GetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), &var );

	if( var.vt != VT_I4 )
	{
		*pdwState &= ~afEnabled;
		return hr;
	}

	if( var.lVal == 1L )
		*pdwState |= afChecked;

	return hr;
}
/**********************************************************************************/

}

} // end of nspace
