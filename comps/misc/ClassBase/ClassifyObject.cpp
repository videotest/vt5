#include "stdafx.h"
#include "classifyobject.h"
#include "misc_str.h"
#include "statistics.h"
#include "ScriptNotifyInt.h"
#include "misc_ptr.h"
#include "core5.h"

/**************************************************************************************************/
_ainfo_base::arg CActionClassifyObjectAllInfo::s_pargs[] =
{
	{ "ClassFile", szArgumentTypeString, "", true, false },
	{ "ObjectName", szArgumentTypeString, "", true, false },
	{"PrevClassFile",szArgumentTypeString, "", true, false },
	{"PrevClasses",szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

#include "\vt5\awin\profiler.h"

static bool _isfileexist( const char *szFileName )
{
//
	HANDLE	h = ::CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,  
		FILE_ATTRIBUTE_NORMAL, 0 );

	if( h == INVALID_HANDLE_VALUE )
	{
		DWORD	dw = ::GetLastError();
		return false;
	}

	::CloseHandle( h );

	return true;

}

_variant_t _exec_script_function( LPCTSTR lpctstrFunction, long nArgsCount, _variant_t *pvarArgs )
{
	if( !lpctstrFunction ) return _variant_t();

	IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	if( sptrScriptSite == 0 ) return S_FALSE;

//	_variant_t args[] = {1,"zzz"};

	_variant_t varRes;
	HRESULT hRes = sptrScriptSite->Invoke( _bstr_t( lpctstrFunction ), pvarArgs, nArgsCount, &varRes, fwFormScript | fwAppScript );

	return varRes;
}

static int GetGroupNoByObjectPos(IUnknown *punkTD, long lpos)
{
	IStatTablePtr sptrTD(punkTD);
	if (sptrTD == 0) return -1;
	stat_row *prow;
	sptrTD->GetNextRow(&lpos, &prow);
	dbg_assert(prow != NULL);
	GuidKey guildGroup = prow->guid_group;
	int nGroup = 0;
	long lposGroup;
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
		for (const char *p = pszMask; p != 0 && *p != 0; p = strchr(p, ','))
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
			for (p = pszMask; p != 0 && *p != 0; p = strchr(p, ','))
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
	bool CheckObjectByPos(IUnknown *punkTD, long lPosPrev)
	{
		int nGroup = GetGroupNoByObjectPos(punkTD, lPosPrev);
		if (nGroup > -1 && !Check(nGroup))
			return false;
		else
			return true;
	}
};

HRESULT CActionClassifyObjectAll::DoInvoke()
{
	_bstr_t bstrClassFile = GetArgString( "ClassFile" );
	_bstr_t bstrObjectName = GetArgString( "ObjectName" );
	_bstr_t bstrPrevClassFile = (char*)GetArgString( "PrevClassFile" );
	_bstr_t bstrPrevClasses = GetArgString( "PrevClasses" );
	BOOL bOverwriteManual = GetValueInt(GetAppUnknown(), "\\Classes", "ResetManualClass", 1);

	{
		CString tmp = short_filename( (char *)bstrClassFile );
		CString strClassFile = full_classifiername( tmp.GetBuffer( tmp.GetLength() ) );
		tmp.ReleaseBuffer();

		tmp = short_filename( (char *)bstrPrevClassFile );
		CString strPrevClassFile = full_classifiername( tmp.GetBuffer( tmp.GetLength() ) );
		tmp.ReleaseBuffer();

		bstrClassFile = strClassFile;
		bstrPrevClassFile = strPrevClassFile;
	}

	if( !bstrClassFile.length() )
		bstrClassFile = ::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );

	// проверить, существует ли классификатор; если нет - уведомить технологов
	{
		if(!_isfileexist(full_classifiername(bstrClassFile)))
		{
			_variant_t varClassFile(bstrClassFile);
			_variant_t var = _exec_script_function("WarningNoClassifier", 1, &varClassFile);
			long res = -1;
			if(VT_EMPTY!=var.vt) res = long(var);
			if(-1==res)
			{ // если функция не вызвалась, вернула -1 или пустой вариант - обработка по умолчанию
				CString strErr;
				strErr.FormatMessage(IDS_NO_CLASSIFIER, (char*)(bstrClassFile));
				HWND	hwnd_main = 0;
				IApplicationPtr	ptrA(GetAppUnknown());
				if(ptrA!=0) ptrA->GetMainWindowHandle(&hwnd_main);
				MessageBox(hwnd_main, strErr.GetBuffer(), "Error", MB_ICONERROR | MB_OK);
				return S_FALSE;
			}
			else if(0==res) return S_FALSE; // если функция вернула 0 - оборвем акцию
			// а если не 0 и не -1 - то можно работать дальше (типа технологи создали классификатор)
		}
	}

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

	IUnknown *punkObjList = 0;
	if( bstrObjectName.length() )
		punkObjList = ::GetObjectByName( m_ptrTarget, bstrObjectName, 0 );

	if( !punkObjList )
		punkObjList = ::GetActiveObjectFromDocument( m_ptrTarget, szTypeObjectList );

	if( !punkObjList )
		punkObjList = ::GetActiveObjectFromDocument( m_ptrTarget, szTypeStatTable );

	if( !punkObjList )
		return S_FALSE;

	INamedDataObject2Ptr ptrList = punkObjList;

	if( punkObjList )
		punkObjList->Release(); punkObjList = 0;

	if( ptrList == 0 )
		return S_FALSE;

	// Проверить набор параметров, заодно сделаем себе массив с ними на будущее
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
					long param_pos=0;
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
		}
		while(c);

		if(strBadKeys!="")
		{
			strBadKeys.Delete(0,1);
			_variant_t varBadKeys(strBadKeys);
			_variant_t var = _exec_script_function("WarningNoParams", 1, &varBadKeys);
			long res = -1;
			if(VT_EMPTY!=var.vt) res = long(var);
			if(-1==res)
			{ // если функция не вызвалась, вернула -1 или пустой вариант - обработка по умолчанию
				CString strErr;
				strErr.FormatMessage(IDS_NO_PARAMS, strBadKeys.GetBuffer());
				HWND	hwnd_main = 0;
				IApplicationPtr	ptrA(GetAppUnknown());
				if(ptrA!=0) ptrA->GetMainWindowHandle(&hwnd_main);
				MessageBox(hwnd_main, strErr.GetBuffer(), "Error", MB_ICONERROR | MB_OK);
				return S_FALSE;
			}
			else if(0==res) return S_FALSE; // если функция вернула 0 - оборвем акцию
			// а если не 0 и не -1 - то можно работать дальше (типа технологи создали классификатор)
		}
	}

	bool bStatTable = CheckInterface(ptrList, IID_IStatTable);
	CGroupsMask GroupsMask(bStatTable);

	sptrProxy->Start();

	long lPos = 0;
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

	if( bstrPrevClassFile.length() &&  bstrPrevClasses.length() )
		split( (char *)bstrPrevClasses, ";", &Strs, &lSz );

	_ptr_t<long> params_bad_count(n_params); // для выяснения, какие параметры не посчитаны хоть в одном из объектов
	{ for(int i=0; i<n_params; i++) params_bad_count.ptr()[i] = 0; }

	m_nSkipped = 0;
	long lCount = 0;
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

		// проверка наличия нужных параметров
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
			strBadKeys.Delete(0,1);
			strBadCounts.Delete(0,1);
			_variant_t vars[] = { _variant_t(strBadCounts), _variant_t(strBadKeys) } ;
			_variant_t var = _exec_script_function("WarningNotCalculatedParams", 2, vars);
		}
	}

	sptrProxy->Finish();
	
	m_nCount = lCount;

	sprintf( sz_output, "Classifier objects count: %d\n", m_nCount );
	OutputDebugString( sz_output );

	if( !m_nCount && !m_nSkipped )
		return S_FALSE;

	m_bstrClassFile = bstrClassFile;

	m_ptrObject_ = new ICalcObjectPtr[ m_nCount + m_nSkipped ];
	m_lClass_ = new long[ m_nCount + m_nSkipped ];
	m_undo_lClass_ = new long[ m_nCount + m_nSkipped ];
	m_bManualFlags.alloc(m_nCount + m_nSkipped);
	m_bUndoManualFlags.alloc(m_nCount + m_nSkipped);

	ptrList->GetFirstChildPosition( &lPos );
	long lId = 0;
	while( lPos )
	{
		bool bSkip = bStatTable && !GroupsMask.CheckObjectByPos(ptrList,lPos);
		IUnknownPtr sptrObject;
		ptrList->GetNextChild( &lPos, &sptrObject );

		long lClass = -1;

		sptrProxy->GetObjectClass( sptrObject, &lClass );

		m_ptrObject_[lId] = sptrObject;
		if( m_ptrObject_[lId] == 0 )
			return S_FALSE;
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

	SetModified( true );

	long l = cncReset;
	
	FireEvent( m_ptrTarget, szEventChangeObjectList, ptrSaveList, 0, &l, sizeof( l ) );
	return S_OK;
}

HRESULT CActionClassifyObjectAll::DoUndo()
{
	for( int i = 0; i < m_nCount + m_nSkipped; i++ )
	{
		set_object_class( m_ptrObject_[i], m_undo_lClass_[i], m_bstrClassFile );
		set_object_class_manual(m_ptrObject_[i], m_bUndoManualFlags.ptr()[i], m_bstrClassFile);
	}
	NotifyParent();	//просигналим, чтоб обновить вьюху

	long l = cncReset;
	FireEvent( m_ptrTarget, szEventChangeObjectList, 0, 0, &l, sizeof( l ) );

	return S_OK;
}

HRESULT CActionClassifyObjectAll::DoRedo()
{
	for( int i = 0; i < m_nCount + m_nSkipped; i++ )
	{
		set_object_class( m_ptrObject_[i], m_lClass_[i], m_bstrClassFile );
		set_object_class_manual(m_ptrObject_[i], m_bManualFlags.ptr()[i], m_bstrClassFile);
	}
	NotifyParent();	//просигналим, чтоб обновить вьюху
	long l = cncReset;
	FireEvent( m_ptrTarget, szEventChangeObjectList, 0, 0, &l, sizeof( l ) );
	return S_OK;
}

HRESULT CActionClassifyObjectAll::GetActionState( DWORD *pdwState )
{
	HRESULT r = __super::GetActionState(pdwState);

	if( !_is_avaible() ) 
		*pdwState &= ~afEnabled;

	return(r);
}
bool CActionClassifyObjectAll::_is_avaible()
{
	return true;
}