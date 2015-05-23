#include "StdAfx.h"
#include "resource.h"
//#include "stat_consts.h"
#include "statui_consts.h"
#include <statistics.h>
#include "statui_int.h"
#include "CmpStatObject.h"
#include "classify_int.h"

namespace{

	bool _write_entry(	INamedData* ptrSrc, INamedData* ptrTarget, 
		LPCSTR pszSrcEntry, LPCSTR pszTargetEntry, bool bOvewrite )
	{

		if( !ptrSrc || !ptrTarget || !pszSrcEntry || !pszTargetEntry )
			return false;


		_bstr_t bstrPathSrc( pszSrcEntry );
		_bstr_t bstrPathTarget( pszTargetEntry );

		// set section
		if( S_OK != ptrSrc->SetupSection( bstrPathSrc ) )
			return false;

		// get count of entries
		long EntryCount = 0;
		ptrSrc->GetEntriesCount( &EntryCount );

		// for all entries
		for( int i = 0;i<(int)EntryCount;i++ )
		{
			try
			{
				BSTR bstr = NULL;
				// get entry name

				ptrSrc->GetEntryName( i, &bstr );
				_bstr_t bstrEntryName = bstr;
				::SysFreeString( bstr );	bstr = 0;
				if(0==bstrEntryName.length())
					continue;

				// format a full path for entry
				_bstr_t bstrPathNewSrc;			
				int nLenSrc = bstrPathSrc.length();
				if( nLenSrc > 0 && ((char*)bstrPathSrc)[nLenSrc-1] == '\\' )
					bstrPathNewSrc = bstrPathSrc + bstrEntryName;
				else
					bstrPathNewSrc = bstrPathSrc + "\\" + bstrEntryName;

				_bstr_t bstrPathNewTarget;			
				int nLenTarget = bstrPathTarget.length();
				if( nLenTarget > 0 && ((char*)bstrPathTarget)[nLenTarget-1] == '\\' )
					bstrPathNewTarget = bstrPathTarget + bstrEntryName;
				else
					bstrPathNewTarget = bstrPathTarget + "\\" + bstrEntryName;

				// get value
				_variant_t var;
				ptrSrc->GetValue( bstrPathNewSrc, &var );

				bool bCanSetValue = true;
				if( !bOvewrite )
				{
					_variant_t _var;
					ptrTarget->GetValue( bstrPathNewTarget, &_var );

					bCanSetValue = ( _var.vt == VT_EMPTY );				
				}

				if( bCanSetValue )
				{
					if( var.vt == VT_BITS )
					{								
						long lsz = 0; 
						byte *pByte = GetValuePtr( ptrSrc, bstrPathTarget, bstrEntryName, &lsz );
						*(long*)pByte = lsz;
						var.lVal = (long)pByte;
					}

					ptrTarget->SetupSection( bstrPathNewTarget );
					ptrTarget->SetValue( bstrPathNewTarget, var );
				}

				// if this entry has children => we want to walk to them
				if( EntryCount > 0 )
					_write_entry( ptrSrc, ptrTarget, bstrPathNewSrc, bstrPathNewTarget, bOvewrite );

				// for next entry on this level restore Section
				ptrSrc->SetupSection( bstrPathSrc );

			}
			catch(...)
			{
				continue;
			}

		}
		return true;
	}
}

namespace ObjectSpace
{
	
	CCmpStatObjectInfo::CCmpStatObjectInfo():CDataInfoBase( clsidCmpStatObject, szTypeCmpStatObject, 0, IDI_STAT_CMP )
	{
	}

	HRESULT CCmpStatObjectInfo::GetUserName( BSTR *pbstr )
	{
		if( !pbstr )
			return E_POINTER;

		if( !m_bstrUserName.length() )
		{

			CStringW sz_buf;
			sz_buf.LoadString( App::handle(), IDS_CMP_STAT_OBJECT_TYPE_NAME);
			m_bstrUserName	= sz_buf;
		}

		*pbstr = m_bstrUserName.copy();

		return S_OK;
	}

	CCmpStatObject::CCmpStatObject(void)
		: ::IDispatchImpl<ICmpStatObjectDisp>("statobject.tlb")
		, _bMinMaxTrue(false)
	{
		_standard._bShow=FALSE;
		_standard._mx=0.;
		_standard._div=1.;
		INamedDataPtr sptrDN = Unknown();
		if( sptrDN == 0 )
		{
			INamedDataObject2Ptr sptrND = Unknown();
			sptrND->InitAttachedData();
		}
	}

	CCmpStatObject::~CCmpStatObject(void)
	{
	}

	void CCmpStatObject::deinit()
	{
		_statObjects.clear();
	}

	GuidKey CCmpStatObject::GetInternalClassID()
	{
		return clsidCmpStatObject;
	}

	IUnknown* CCmpStatObject::DoGetInterface( const IID &iid )
	{
		if( iid == IID_IDispatch ) 
			return (IDispatch *)this;	
		else if( iid == __uuidof( ICmpStatObjectDisp ) ) 
			return (ICmpStatObjectDisp*)this;	
		else if( iid == IID_ICompatibleObject ) 
			return (ICompatibleObject*)this;	
		else if( iid == IID_IEventListener )
			return (IEventListener*)this;
		else if( iid == IID_INamedPropBag ) 
			return (INamedPropBag*)this;	
		else if( iid == IID_INamedPropBagSer ) 
			return (INamedPropBag*)this;	
		else 
			return CObjectBase::DoGetInterface( iid );
	}

	HRESULT CCmpStatObject::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
	{
		return S_OK;
	}

	bool LoadColorsNames(std::map<int,CColorsNames::ColorName>& mapDscrClass,
		INamedData* pNamedData)
	{
		mapDscrClass.clear();
		pNamedData->SetupSection(CLASSIFIER_CLASSES);
		long cnt=0; pNamedData->GetEntriesCount(&cnt);
		for (long iEntry=0; iEntry<cnt; ++iEntry)
		{
			CComBSTR entry;

			pNamedData->GetEntryName(iEntry, &entry);
			CComVariant var=entry;
			if(SUCCEEDED(var.ChangeType(VT_I4)))
			{
				int iClassNo=var.intVal;
				WCHAR wsz_ClsName[96];
				HRESULT hr=pNamedData->GetValue(entry, &var);
				if(SUCCEEDED(hr) && VT_BSTR==var.vt)
				{
					int	r = 255, g = 0, b = 0;
					int nFields=::swscanf( var.bstrVal, L"(%d,%d,%d),%[^\0]S", &r, &g, &b, wsz_ClsName );
					if(nFields>=0){
						if(nFields<=3){
							swprintf(wsz_ClsName,L"Class%d",iClassNo);
						}
						CColorsNames::ColorName colorName(RGB(r,g,b), CString(wsz_ClsName));
						mapDscrClass[iClassNo]=colorName;
					}
				}
			}
		}
		return mapDscrClass.size()>0;
	}

	// find keys using in current classifier
	void KeysInfo(vector<long>& keysClassifier) 
	{
		char sz_KeysEnum[256];
		::GetPrivateProfileString("KeysInfo", "KeysEnum", 0
			, sz_KeysEnum, 256, full_classifiername(0));
		{
			static char seps[]   = " ,\t";
			for(char* token=strtok(sz_KeysEnum,seps); token!=NULL; token = strtok(NULL,seps) )
			{
				long iKey;
				int nFields=::sscanf( token, "%d", &iKey);
				if(nFields)
					keysClassifier.push_back(iKey);
			}
		}
	}

	STDMETHODIMP CCmpStatObject::get_Statuses(VARIANT* pVal)
	{
		// find keys using in current classifier
		vector<long> keysClassifier;
		KeysInfo(keysClassifier);

		// find key using in statistic
		long keyStatistic = GetValueInt( GetAppUnknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, 0 ); 

		// fill available classes
		ParseClassifier(full_classifiername(0));

		_statTables.clear();
		_bMinMaxTrue=false;
		CComBSTR bstrActiveDocFile;			
		IApplicationPtr sptrA(GetAppUnknown());
		IUnknownPtr punkActiveDoc;
		sptrA->GetActiveDocument(&punkActiveDoc);
		if(IDocumentSitePtr	ptrDocSite=punkActiveDoc){
			ptrDocSite->GetPathName( &bstrActiveDocFile );
		}
		double x=0., x2=0.; int N=0;
		LONG_PTR lPosTemplate = 0;
		sptrA->GetFirstDocTemplPosition( &lPosTemplate );
		for(; lPosTemplate;	sptrA->GetNextDocTempl(&lPosTemplate,0,0))
		{
			LONG_PTR lPosDoc=0;
			HRESULT hr = sptrA->GetFirstDocPosition(lPosTemplate,&lPosDoc);
			IUnknownPtr punkDoc; 
			for (;lPosDoc && SUCCEEDED(hr=sptrA->GetNextDoc(lPosTemplate,&lPosDoc,&punkDoc));)
			{
				if(IDocumentSitePtr	ptrDocSite=punkDoc){
					CComBSTR bstrDocType;
					ptrDocSite->GetDocType(&bstrDocType);
					if(bstrDocType==CComBSTR("Image")){
					CComBSTR bstrFileName;
					ptrDocSite->GetPathName( &bstrFileName );
					if(IDataContext2Ptr ptrDC2=ptrDocSite){						
						GuidKey guidDocKey = ::GetKey(ptrDC2);
						if(IDataTypeManagerPtr pDataType=ptrDocSite){
							long nCount=0;
							pDataType->GetTypesCount(&nCount);
							long lType=0;
							for( ;lType<nCount; ++lType){
								CComBSTR bsType;
								pDataType->GetType(lType,&bsType);
								if(bsType==szTypeStatTable){
									LONG_PTR lPos=0;
									pDataType->GetObjectFirstPosition(lType,&lPos);

									// take any first table in document - 
									if(lPos)
									{
										IUnknownPtr punkObj;
										pDataType->GetNextObject(lType,&lPos,&punkObj);
										TableStatus status=noTable|noStatParam|noClassMatch|noClassifyParam;
										if(IStatTablePtr pTable=punkObj)
										{
											status &= ~noTable;
											TPOS lposKey=0;

											// test: There is statistic parameter in table
												if(S_OK==pTable->GetParamPosByKey(keyStatistic,&lposKey))
													if(lposKey) 
												status &= ~noStatParam;

											// test: There are all classifier's parameters in table
											for(unsigned i=keysClassifier.size();i>0;)
												{
													if((S_OK==pTable->GetParamPosByKey(keysClassifier[--i],&lposKey)) && lposKey){
														;
													}else{
													goto NoClassifierParam;
												}
												}
											status &= ~noClassifyParam;
NoClassifierParam: ;

											// test classes match. Classes match if all classes in table also available in classifier,
											// else no match;
											if(INamedDataObject2Ptr sptrDNO=pTable)
											{
												TPOS posRow = 0;
												stat_row* pRow=0;
												set<long> Classes;
														double xx=0., xx2=0.; int NN=0;

												for(HRESULT hr=pTable->GetFirstRowPos(&posRow);
													posRow;
													pTable->GetNextRow(&posRow, &pRow),posRow)
												{
													IUnknownPtr punkO;
													{
														TPOS lP = posRow;
														sptrDNO->GetNextChild( &lP, &punkO );
													}
													if(ICalcObjectPtr sptrCalc = punkO)
													{
														Classes.insert(get_object_class(sptrCalc));
														double value;
																if(S_OK==sptrCalc->GetValue(keyStatistic,&value))
														{
															if(!_bMinMaxTrue){
																_valMin=value;
																_valMax=value;
																_bMinMaxTrue=true;
															}else{
																_valMin=__min(_valMin,value);
																_valMax=__max(_valMax,value);
															}

																	xx += value;
																	xx2 += value*value;
																	++NN;
																}else{
																	status |= noStatParam;
																}
																// test: There are all classifier's parameters in object
																for(unsigned i=keysClassifier.size();i>0;)
																{
																	double value;
																	if(S_OK==sptrCalc->GetValue(keysClassifier[--i],&value)){
																		;
																	}else{
																		status |= noClassifyParam;
																	}
														}
													}
												}
												if(INamedDataPtr pND=pTable)
												{
													std::map<int,CColorsNames::ColorName> mapDscrClass;
													ObjectSpace::LoadColorsNames(mapDscrClass,pND);

													//	for each(long cls in Classes)
													for(set<long>::const_iterator itCls=Classes.begin();
														itCls!=Classes.end(); ++itCls)
													{
														if(!testClass(*itCls)){
															goto NoClassMatch;
														}
													}
													status &= ~noClassMatch;
NoClassMatch: ;
												}
											StatTable  statTable(pTable,status,FALSE);
											_statTables[bstrFileName]=statTable;
														x += xx;
														x2 += xx2;
													}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		_standard._mx=x/N;
		_standard._div= (x2 - x*x/N) / (N - 1);
		::SetValue( Unknown(), SECT_STATUI_CHART_ROOT, USERCURVE_MX, _standard._mx); 
		::SetValue( Unknown(), SECT_STATUI_CHART_ROOT, USERCURVE_DX, _standard._div); 

		// When copying document names & statuses, place active doc to zero position
		CComSafeArray<VARIANT> sa((ULONG)_statTables.size());
		int i=0, iActiveDoc=-1;
		for(StatTables::const_iterator it=_statTables.begin(); 
			it!=_statTables.end(); ++it)
		{
			const StatTables::value_type& valType=*it;
			const TableStatus& status=valType.second._status;
			CComSafeArray<VARIANT> va(2);
			va[0]=valType.first.Copy();
			va[1]=short(status);
			int index= (bstrActiveDocFile==valType.first)? 0 : ++i;
			sa[index]=va.Detach();
		}

		pVal->vt = VT_ARRAY | VT_VARIANT;
		pVal->parray=sa.Detach();
		return S_OK;
	}

	STDMETHODIMP CCmpStatObject::put_Statuses(VARIANT newVal)
	{
		if((VT_ARRAY & newVal.vt) && (VT_VARIANT & newVal.vt))
		{
			const CComSafeArray<VARIANT>& sa=(VT_BYREF & newVal.vt)?*newVal.pparray:newVal.parray;
			for(int i=0; i<(int)sa.GetCount(); ++i)
			{
				const VARIANT& rfConstVar = sa[i];
				const CComSafeArray<VARIANT>& saElement=(VT_BYREF & rfConstVar.vt)?*rfConstVar.pparray:rfConstVar.parray;
				{
					const VARIANT& rfConstVar = sa[i];
					const CComSafeArray<VARIANT>& saElement=(VT_BYREF & rfConstVar.vt)?*rfConstVar.pparray:rfConstVar.parray;
					if(VT_BSTR==saElement[0].vt)
					{
						StatTables::iterator it=_statTables.find(saElement[0].bstrVal);
						if(it!=_statTables.end())
						{
							StatTable& rStatTable=it->second;
							rStatTable._clrCmpStat=saElement[1].ulVal;
							rStatTable._bCompare=(VT_BOOL==saElement[2].vt && -1==saElement[2].iVal);
							rStatTable._iHatch=saElement[3].iVal;
						}
					}
				}
			}
			return S_OK;
		}
		return E_INVALIDARG;
	}

	//STDMETHODIMP CCmpStatObject::get_Standard(VARIANT* pVal)
	//{
	//	CComSafeArray<VARIANT> va(3);
	//	va[0]=_standard._bShow;
	//	va[1]=_standard._mx;
	//	va[2]=_standard._div;
	//	pVal->vt = VT_ARRAY | VT_VARIANT;
	//	pVal->parray=va.Detach();
	//	return S_OK;
	//}

	//STDMETHODIMP CCmpStatObject::put_Standard(VARIANT newVal)
	//{
	//	if((VT_ARRAY & newVal.vt) && (VT_VARIANT & newVal.vt))
	//	{
	//		const CComSafeArray<VARIANT>& sa=(VT_BYREF & newVal.vt)?*newVal.pparray:newVal.parray;
	//		_standard._bShow=sa[0].boolVal;
	//		_standard._mx=sa[1].dblVal;
	//		_standard._div=sa[2].dblVal;
	//		return S_OK;
	//	}
	//	return E_INVALIDARG;
	//}

	IStatTable* CreateCompatibleTable( IUnknown* punk_table_src )
	{
		IStatTablePtr ptr_new_table;
		//		HRESULT hr=ptr_new_table.CreateInstance();
		ptr_new_table = CreateTypedObject( _bstr_t(szTypeStatTable) );

		ICompatibleObjectPtr ptr_clone( ptr_new_table );

		HRESULT hr=ptr_clone->CreateCompatibleObject(punk_table_src, 0, 0);
		if( FAILED(hr)){
			return 0;
		}else{
			return ptr_new_table;
		}
	}


	STDMETHODIMP CCmpStatObject::Compare(LONG* rc)
	{
		*rc=S_FALSE;
		int iCheckMem=_CrtCheckMemory();
		INamedDataPtr pND=Unknown();
		BOOL b=CopyObjectNamedData( ::GetAppUnknown(), Unknown(), _T(SECT_STATUI_ROOT), false );
		//_diagType=DiagType;
		//_colorHatch=colorHatch;
		UpdateColorsNames(0);
		try
		{
			{
			_statObjects.clear();
			long lStat=0;
				vector<IStatTable*> tablesNew;
				{
			for(StatTables::iterator it=_statTables.begin(); 
				it!=_statTables.end(); ++it,++lStat)
			{
				StatTables::value_type valType=*it;
				const TableStatus& status=valType.second._status;
						if(valType.second._bCompare)
						{
							if(comparable==status || noClassMatch==status)
				{
					IStatTablePtr pStatTable=valType.second._pTable;
							IStatTable* pStatTableNew=CreateCompatibleTable(pStatTable);
					tablesNew.push_back(pStatTableNew);
					valType.second._pTable=pStatTableNew;
					_statObjects.push_back(valType);
				}
							else
							{
								_statObjects.clear();
								return status;
							}
						}
			}

			// should be reclassify if classifier has classifying parameters
					HRESULT hrClassify=ClassifyTables(tablesNew);
					if(S_OK!=hrClassify){
						_statObjects.clear();
						return hrClassify;
				}
			}
			}

			for(StatObjects::iterator it=_statObjects.begin(); 
				it!=_statObjects.end(); ++it)
			{
				const StatObjects::value_type& valType=*it;
				StatTable& statTable=const_cast<StatTable&>(valType.second);
				IStatTablePtr pStatTableNew=statTable._pTable;		

				IStatObjectPtr pStatObj;
				HRESULT hr=pStatObj.CreateInstance(clsidStatObject);
				if(SUCCEEDED(hr))
				{
					hr=pStatObj->Calculate(pStatTableNew,-1);
					if(SUCCEEDED(hr))
					{
						statTable._pStat=pStatObj;
						statTable._pTable=0;
//					pNamedData->SetValue(_bstr_t(L"Stat")+_bstr_t(lStat),CComVariant((IUnknown*)pStatObj));
					}
				}
			}

			// get active document file name	
			CComBSTR bstrActiveDocFile;	
			{
				IApplicationPtr sptrA(GetAppUnknown());
				IUnknownPtr punkActiveDoc;
				sptrA->GetActiveDocument(&punkActiveDoc);
				if(IDocumentSitePtr	ptrDocSite=punkActiveDoc){
					ptrDocSite->GetPathName( &bstrActiveDocFile );
				}
			}

			// replace zero element by stat object for current active doc
			for(unsigned index=0; index<_statObjects.size(); ++index)
			{
				StatObjects::value_type& valType=_statObjects[index];
				if(bstrActiveDocFile==valType.first)
				{
					std::swap(valType,_statObjects[0]);
					break;
				}
			}
			{
				CString sClassifierFile=short_classifiername(0);
				long lBaseKey = ::GetValueInt( ::GetAppUnknown(), SECT_STATUI_CHART_AXIS_ROOT, X_PARAM_KEY, 0 );
				CString strPath;
				strPath.Format( "\\Classify\\%s\\%d", short_classifiername(0), lBaseKey) ;
				_write_entry(INamedDataPtr(::GetAppUnknown()),INamedDataPtr(Unknown())
					,strPath, SECT_STATUI_CHART_ROOT SECT_CUSTOM_INTERVALS, true);

				double XMinVal,XMaxVal,YMaxVal;
				long lYKey=::GetValueInt(Unknown(), SECT_STATUI_CHART_AXIS_ROOT, Y_PARAM_KEY,0);
				unsigned iStat=0;
				for(;iStat<_statObjects.size(); ++iStat)
				{
					IStatObjectDispPtr pStat = _statObjects[iStat].second._pStat;
					double YMax=0.;
					{
						long nClass=0;
						pStat->GetClassCount(&nClass);
						for(int i=0; i<nClass; ++i)
						{
							double fY=0.;
							pStat->GetValue2(i,lYKey,&fY);
							if(0==i)
								YMax=fY;
							else
								YMax=__max(YMax,fY);
						}
					}
					double XMin,XMax;
					pStat->GetValueGlobal2(PARAM_MIN,&XMin);
					pStat->GetValueGlobal2(PARAM_MAX,&XMax);

					if(0==iStat){
						XMinVal=XMin;
						XMaxVal=XMax;
						YMaxVal=YMax;
					}else{
						XMinVal=__min(XMinVal,XMin);
						XMaxVal=__max(XMaxVal,XMax);
						YMaxVal=__max(YMaxVal,YMax);
					}
				}
				if(iStat){
					::SetValue( Unknown(), SECT_STATCMP_CHART_ROOT, "XMinVal", XMinVal);
					::SetValue( Unknown(), SECT_STATCMP_CHART_ROOT, "XMaxVal", XMaxVal);
					::SetValue( Unknown(), SECT_STATCMP_CHART_ROOT, "YMaxVal", YMaxVal);
				}
			}

			*rc=S_OK;
			return S_OK;
		}catch(_com_error& err){
			*rc=err.Error();
			return S_FALSE;
		}
	}

	STDMETHODIMP CCmpStatObject::GetPrivateNamedData( /*[out,retval]*/ IDispatch **ppDisp )
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

	//////////////////////////////////////////////////////////////////////
	//interface ISerializableObject
	//////////////////////////////////////////////////////////////////////

	HRESULT CCmpStatObject::Load( IStream *pStream, SerializeParams *pparams )
	{
		TIME_TEST( "CCmpStatObject::Load()" );

		deinit();

		HRESULT hr = CObjectBase::Load( pStream, pparams );
		if( S_OK != hr )
			return hr;

		ULONG nRead = 0;
		//At first version control
		long nVersion  = 1;
		pStream->Read( &nVersion, sizeof(long), &nRead );

		INamedDataPtr pNamedData=Unknown();
		size_t nStats;
		pStream->Read(&nStats, sizeof(size_t), &nRead);
		for(size_t i=0; i<nStats; ++i)
		{
			StatObjects::value_type valType;

			valType.first.ReadFromStream(pStream);
			pStream->Read(&valType.second._clrCmpStat ,sizeof(COLORREF), &nRead );
			pStream->Read(&valType.second._iHatch, sizeof(int), &nRead );

			valType.second._pStat;

			IStatObjectPtr pStatObj;
			HRESULT hr=pStatObj.CreateInstance(clsidStatObject);
			ISerializableObjectPtr pSerObject=pStatObj;
			if(ISerializableObjectPtr pSerObject=pStatObj)
			{
				hr=pSerObject->Load(pStream, pparams);
			}
			valType.second._pStat=pStatObj;
			valType.second._pTable=0;
			_statObjects.push_back(valType);
			//		pNamedData->SetValue(_bstr_t(L"Stat")+_bstr_t((long)i),CComVariant((IUnknown*)pStatObj));
		}

		LoadColorsNames();
		return S_OK;
	}

	HRESULT CCmpStatObject::Store( IStream *pStream, SerializeParams *pparams )
	{
		TIME_TEST( "CCmpStatObject::Store()" );

		HRESULT hr = CObjectBase::Store( pStream, pparams );
		if( S_OK != hr )
			return hr;

		ULONG nWritten = 0;

		//At first version control
		long nVersion  = 1;
		pStream->Write( &nVersion, sizeof(long), &nWritten );

		//pStream->Write(&_diagType, sizeof(_diagType), &nWritten);
		//pStream->Write(&_colorHatch, sizeof(_colorHatch), &nWritten);
		//pStream->Write(&_standard, sizeof(_standard), &nWritten);

		size_t nStats=_statObjects.size();
		pStream->Write(&nStats, sizeof(size_t), &nWritten);

		for(StatObjects::iterator it=_statObjects.begin(); 
			it!=_statObjects.end(); ++it)
		{
			StatObjects::value_type& valType=*it;
			valType.first.WriteToStream(pStream);
			pStream->Write(&valType.second._clrCmpStat ,sizeof(COLORREF), &nWritten );
			pStream->Write(&valType.second._iHatch, sizeof(int), &nWritten );
			ISerializableObjectPtr pSerObject=valType.second._pStat;
//			_ASSERTE(pSerObject);
			pSerObject->Store(pStream,pparams);
		}

		return S_OK;
	}

}

namespace
{


	CString _get_value_string( CString strFileName, CString strSection, CString strKey )
	{
		TCHAR szT[4096];
		DWORD dw = ::GetPrivateProfileString( strSection, strKey,
			"", szT, sizeof( szT ) / sizeof( szT[0] ), strFileName );
		if( dw > 0 )
			return szT;
		return "";
	}

	static int GetGroupNoByObjectPos(IUnknown *punkTD, LONG_PTR lpos)
	{
		IStatTablePtr sptrTD(punkTD);
		if (sptrTD == 0) return -1;
		stat_row *prow;
		sptrTD->GetNextRow((TPOS*)&lpos, &prow);
		dbg_assert(prow != NULL);
		GuidKey guildGroup = prow->guid_group;
		int nGroup = 0;
		TPOS lposGroup;
		sptrTD->GetFirstGroupPos(&lposGroup);
		while (lposGroup)
		{
			stat_group *pGroup;
			sptrTD->GetNextGroup(&lposGroup, &pGroup);
			if (guildGroup == pGroup->guid_group)
				return nGroup;
			nGroup++;
		}
		dbg_assert(FALSE);
		return -1;
	}

	class CGroupsMask
	{
		_ptr_t2<bool> m_abMask;
		bool m_bClassifyAll;
	public:
		CGroupsMask(bool bInit)
		{
			if (bInit)
				Init();
		}

		void Init(const char *pszMask)
		{
			m_bClassifyAll = pszMask==NULL||*pszMask==0;
			int nMaskSize = 0;
			for (const char* p = pszMask; p != 0 && *p != 0; p = strchr(p, ','))
			{
				while(*p&&(*p ==','||*p==' ')) p++;
				int n = atoi(p)+1;
				if (n > nMaskSize)
					nMaskSize = n;
			}
			if (nMaskSize > 0)
			{
				m_abMask.alloc(nMaskSize);
				m_abMask.zero();
				for (const char* p = pszMask; p != 0 && *p != 0; p = strchr(p, ','))
				{
					while(*p&&(*p ==','||*p==' ')) p++;
					int n = atoi(p);
					m_abMask[n] = true;
				}
			}
		}
		void Init()
		{
			_bstr_t bstr = GetValueString(GetAppUnknown(), "StatUI", "CalcForGroups", NULL);
			Init(bstr);
		}
		bool Check(int n)
		{
			if (m_bClassifyAll)
				return true;
			if (n >= m_abMask.size())
				return false;
			return m_abMask[n];
		}
		bool CheckObjectByPos(IUnknown *punkTD, TPOS lPosPrev)
		{
			int nGroup = GetGroupNoByObjectPos(punkTD, (LONG_PTR)lPosPrev);
			if (nGroup > -1 && !Check(nGroup))
				return false;
			else
				return true;
		}
	};


	class CSupportComposite
	{
		int m_nPurpose;
		ICompositeObjectPtr m_ptrComp;
		bool m_bComp;
	public:
		CSupportComposite(IUnknown* unk)
		{
			m_bComp = false;
			m_ptrComp = unk;

			if(m_ptrComp)
			{		
				long bComp;
				m_ptrComp->IsComposite(&bComp);
				if(!bComp)
				{
					m_ptrComp->SetPurpose(0);
					return;
				}
				m_bComp = bComp!=0;
				m_ptrComp->BuildTree(-1);
				m_ptrComp->GetPurpose(&m_nPurpose);
				m_ptrComp->SetPurpose(1);
			}
		};
		~CSupportComposite()
		{
			if(m_ptrComp) m_ptrComp->SetPurpose(m_nPurpose);
		};
		ICompositeObject* operator->()
		{
			return m_ptrComp;
		}
		bool IsComposite()
		{
			return m_bComp;
		}
		operator INamedDataObject2*()
		{
			INamedDataObject2Ptr ndo(m_ptrComp);
			return ndo;
		}
	};
}

namespace ObjectSpace
{

	HRESULT CCmpStatObject::ClassifyTables(const vector<IStatTable*>& tablesNew)
	{
		_bstr_t bstrClassFile = ::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );
		_bstr_t bstrPrevClassFile;// = (char*)GetArgString( "PrevClassFile" );
		_bstr_t bstrPrevClasses;// = GetArgString( "PrevClasses" );
		BOOL bOverwriteManual = false;

		CString strProgID = _get_value_string( (char*)bstrClassFile, ID_SECT_GENERAL, ID_ENTR_CLASSIFY_PROGID );

		if( strProgID.IsEmpty() )
			return S_FALSE;

		char sz_output[255];
		sprintf( sz_output, "Classifier: %s, %s\n", (const char*)strProgID, (const char*)bstrClassFile );
		OutputDebugString( sz_output );
		TIME_TEST( "CActionClassifyObjectAll::DoInvoke" );

		PROFILE_TEST( "CActionClassifyObjectAll::DoInvoke" );


		IUnknownPtr sptrUnk( strProgID, 0, CLSCTX_INPROC_SERVER );
		if( sptrUnk == 0 )
			return S_FALSE;

		IClassifyProxyPtr sptrProxy = sptrUnk;

		if( sptrProxy == 0 )
			return S_FALSE;

		if( sptrProxy->Load( bstrClassFile ) != S_OK )
			return S_FALSE;

		sptrProxy->Start();

		int m_nSkipped = 0;
		long lCount = 0;
		for(vector<IStatTable*>::const_iterator it=tablesNew.begin(); it!=tablesNew.end(); ++it)
		{

			IUnknown *punkObjList = *it;

			if( !punkObjList )
				return noTable;

			INamedDataObject2Ptr ptrList = punkObjList;

			if( punkObjList )
				punkObjList->Release(); punkObjList = 0;

			if( ptrList == 0 )
				return noTable;

			// ѕроверить набор параметров, заодно сделаем себе массив с ними на будущее
			_ptr_t<long> params(100); // запомним себе массив параметров
			long n_params=0;
			{
				ICalcObjectContainerPtr ptrContainer = ptrList;
				IStatTablePtr ptrTable = ptrList;
				CString strBadKeys = "";
				CString strKeyEnum = _get_value_string( (char*)bstrClassFile, ID_SECT_KEY_INFO, ID_ENTR_KEYS_ENUM );
				char *s = strKeyEnum.GetBuffer();
				int n=0, len=0;
				char c;
				do
				{
					c = *(s++);
					if(c>='0' && c<='9')
					{
						n = n*10 + (c-'0');
						len ++ ;
					}
					else
					{
						if(len!=0)
						{ // если уже была хоть одна цифра - значит, получили число
							if(n_params >= params.size()) params.alloc(params.size()*2);
							params.ptr()[n_params] = n; n_params++;
							struct ParameterContainer *pContainer=0;
							TPOS param_pos=0;
							if(ptrContainer!=0) ptrContainer->ParamDefByKey(n,&pContainer);
							if(ptrTable!=0) ptrTable->GetParamPosByKey(n,&param_pos);
							if(0==pContainer && 0==param_pos)
							{
								CString str;
								str.Format(",%u",n);
								strBadKeys += str;
							}
						}
						n=0; len=0;
					}
				}while(c);

				if(strBadKeys!="")
				{
					return noClassifyParam;
				}
			}

			bool bStatTable = CheckInterface(ptrList, IID_IStatTable);
			CGroupsMask GroupsMask(bStatTable);

			TPOS lPos = 0;
			CSupportComposite sc(ptrList);
			INamedDataObject2Ptr ptrSaveList(ptrList);
			if(sc.IsComposite())
			{
				int c = ::GetValueInt(GetAppUnknown(),"Classes","ClassifySubObjects",0);
				IUnknown* unk;
				if(c==0) 
				{
					sc->GetMainLevelObjects(&unk);
					if(unk) ptrList = unk;
				}
			}
			ptrList->GetFirstChildPosition( &lPos );

			CString *Strs = 0;
			long lSz = 0;

			_ptr_t<long> params_bad_count(n_params); // дл€ вы€снени€, какие параметры не посчитаны хоть в одном из объектов
			{ for(int i=0; i<n_params; i++) params_bad_count.ptr()[i] = 0; }

			while( lPos )
			{
				bool bSkip = bStatTable && !GroupsMask.CheckObjectByPos(ptrList,lPos);
				IUnknownPtr sptrObject;
				ptrList->GetNextChild( &lPos, &sptrObject );
				if (bSkip)
				{
					m_nSkipped++;
					continue;
				}

				// проверка наличи€ нужных параметров
				ICalcObjectPtr sptrCalc(sptrObject);
				for(int i=0; i<n_params; i++)
				{
					double fVal = 0;
					if( sptrCalc->GetValue( params.ptr()[i], &fVal ) != S_OK )
						params_bad_count.ptr()[i] ++ ;
				}		

				if( bstrPrevClassFile.length() )
				{
					if( bstrPrevClasses.length() )
					{
						long lClass = get_object_class( ICalcObjectPtr( sptrObject ), bstrPrevClassFile );

						bool bOK = false;
						for( long i = 0; i < lSz; i++ )
						{
							long lVal = atol( Strs[i] );

							if( lVal == lClass )
							{
								bOK = true;
								break;
							}
						}
						if( !bOK )
						{
							m_nSkipped++;
							continue;
						}
					}
				}

				if( sptrProxy->Process( sptrObject ) != S_OK )
					return S_FALSE;

				lCount++;
			}

			delete []Strs;

			{ // преобразуем результаты проверки на наличие параметров в строки
				CString strBadKeys = "";
				CString strBadCounts = "";
				for(int i=0; i<n_params; i++)
				{
					if( params_bad_count.ptr()[i] )
					{
						CString str;
						str.Format(",%u",params.ptr()[i]);
						strBadKeys += str;
						str.Format(",%u",params_bad_count.ptr()[i]);
						strBadCounts += str;
					}
				}
				if(strBadKeys!="")
				{
					return noClassifyParam;
//					_ASSERTE(!"WarningNotCalculatedParams");
				}
			}
		}
		sptrProxy->Finish();

		int m_nCount;
		m_nCount = lCount;

		sprintf( sz_output, "Classifier objects count: %d\n", m_nCount );
		OutputDebugString( sz_output );

		if( !m_nCount && !m_nSkipped )
			return noTable;

		_bstr_t m_bstrClassFile;
		m_bstrClassFile = bstrClassFile;

		ICalcObjectPtr *m_ptrObject_;
		long *m_lClass_;
		long *m_undo_lClass_;
		_ptr_t<bool> m_bManualFlags;
		_ptr_t<bool> m_bUndoManualFlags;

		m_ptrObject_ = new ICalcObjectPtr[ m_nCount + m_nSkipped ];
		m_lClass_ = new long[ m_nCount + m_nSkipped ];
		m_undo_lClass_ = new long[ m_nCount + m_nSkipped ];
		m_bManualFlags.alloc(m_nCount + m_nSkipped);
		m_bUndoManualFlags.alloc(m_nCount + m_nSkipped);

		for(vector<IStatTable*>::const_iterator it=tablesNew.begin(); it!=tablesNew.end(); ++it)
		{
			INamedDataObject2Ptr ptrList = *it;
			TPOS lPos=0;
			ptrList->GetFirstChildPosition( &lPos );
			long lId = 0;
			while( lPos )
			{
				bool bStatTable = CheckInterface(ptrList, IID_IStatTable);
				CGroupsMask GroupsMask(bStatTable);
				bool bSkip = bStatTable && !GroupsMask.CheckObjectByPos(ptrList,lPos);
				IUnknownPtr sptrObject;
				ptrList->GetNextChild( &lPos, &sptrObject );

				long lClass = -1;

				sptrProxy->GetObjectClass( sptrObject, &lClass );

				m_ptrObject_[lId] = sptrObject;
				if( m_ptrObject_[lId] == 0 )
					return noClassifyParam;
				m_bUndoManualFlags.ptr()[lId] = is_object_class_manual(m_ptrObject_[lId], m_bstrClassFile);
				m_bManualFlags.ptr()[lId] = m_bUndoManualFlags.ptr()[lId];
				if (!bSkip)
				{
					if (m_bUndoManualFlags.ptr()[lId])
					{
						if (bOverwriteManual)
						{
							m_bManualFlags.ptr()[lId] = false;
							set_object_class_manual(m_ptrObject_[lId], false, m_bstrClassFile);
							m_lClass_[lId] = lClass;
						}
						else
							m_lClass_[lId] = get_object_class(m_ptrObject_[lId], m_bstrClassFile);
					}
					else
						m_lClass_[lId] = lClass;
				}
				else
					m_lClass_[lId] = -1;

				m_undo_lClass_[lId] = get_object_class( m_ptrObject_[lId], m_bstrClassFile );
				set_object_class( m_ptrObject_[lId], m_lClass_[lId], m_bstrClassFile );
				lId++;
			}
		}
		return S_OK;
	}
}
