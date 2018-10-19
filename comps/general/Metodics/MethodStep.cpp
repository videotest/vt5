#include "StdAfx.h"
#include "MethodStep.h"
#include "Utils.h"
#include "mtd_utils.h"
#include "MethodDataUtil.h"

// 01.04.2005 build 82
/*
Прикручиваем новую концепцию методик:
Методика содержит:
1. shell.data - единая для всей методики, без разбивки по шагам
   Отфильтрованы секции из disabled_method_keys, пути приведены
   к относительным через %имя%
2. Для каждого шага - только информация о вьюхах и выделении,
   сохраняемая StoreSplitter.

Требуемые функции:
1. Засасывание из shell.data в методику (c фильтрацией по disabled и заменой на %имя%)
2. Закидывание из методики в shell.data (с заменой %имя% на пути)
3. StoreSplitter в шаг (а не в shell.data)
4. LoadSplitter из шага

Поправка: в 1-2 можно вместо замены %имя% - сделать таблицу замен (сохранить секцию path)

So, запись методики:
записать shell.data как есть (лишь отфильтровав лишнее);
отдельно - секцию [Paths]

Чтение:
1. Прочитать секцию [Paths], по ней и текущей секции из
shell.data построить мапу: старый_путь->новый_путь
2. Пройтись по всей shell.data методики, заменяя старые пути на
новые
3. Внести все в shell.data

Нижний уровень для записи:
FilterNamedData(IUnknown* punkND, const char* szWild); - удалить из NamedData все подозрительные ключи/секции

Для чтения:
ReplacePaths(IUnknown* punkND, IUnknown* punkFindPathsND, IUnknown* punkReplacePathsND);
// по секциям [Paths] Find/Replace строим мапу и фильтруем все строки из punkND
RecursiveAddEntry(...)

Поправка 2: объединяем все в одну функцию RecursiveAddEntry(dst, src, szWild, SubstMap).
На ее основе делаем все остальное

Поставить StoreSplitter в текущий шаг на события смены текущего шага (во время записи)
и завершения записи (хранить указатель на шаг, который должен быть проапдейчен
в случае чего)

// 05.04.2005 build 82
Готова функция ImportShellData;
запись методики - просто запись shell.data + запись алгоритма.
чтение методики - прочитать ее в отдельный named.data, ImportShellData,
сглотнуть шаги (предложение: хранить шаги прямо в shell.data в секциях
типа [MethodSteps\Step1] - тогда то, что касается текущего шага, будет
легко доступно (возможно, нужен "псевдоним" для текущего - или просто
держать имя его секции в _bstr_t в MethodMan)
[MethodSteps\Step1]
ActionName:String=...
ForcedName:String=...
Flags:Long=4
[MethodSteps\Step1\StateBeforeAction]
...
[MethodSteps\Step1\StateAfterAction]
*/

static CString LoadString(LPCTSTR sz_id, LPCTSTR sz_sect="Strings") 
{
	CString strResult( _T("") );

	// [vanek] : как и в CShellDocManagerDisp::LoadStringEx - 26.10.2004
	CString str_id( sz_id );
	str_id.TrimLeft();
	str_id.TrimRight();
	if( !str_id.IsEmpty() )
	{
		char	sz_string_file[MAX_PATH];
		GetModuleFileName( 0, sz_string_file, sizeof( sz_string_file ) );
		strcpy( strrchr( sz_string_file, '\\' )+1, "resource." );

		CString	strLang = GetValueString( GetAppUnknown(), "\\General", "Language", "en" );
		strcat( sz_string_file, strLang );
		
		char	*pbuffer = strResult.GetBuffer( 4096 );
		char sz_default[] = "";
		::GetPrivateProfileString( sz_sect, sz_id, sz_default, pbuffer, 4096, sz_string_file );
		if(0==*pbuffer)
		{
			strcpy(pbuffer, sz_id);
			::WritePrivateProfileString( sz_sect, sz_id, pbuffer, sz_string_file );
		}

		char	*pout = pbuffer;

		while( *pbuffer )
		{
			if( *pbuffer == '\\' )
			{
				pbuffer++;
				if( *pbuffer == 'n' || *pbuffer == 'N' )
					*pbuffer = '\n';
				if( *pbuffer == 'r' || *pbuffer == 'R' )
					*pbuffer = '\r';
			}
			*pout = *pbuffer;
			pout++;pbuffer++;
		}
		*pout = 0;

		strResult.ReleaseBuffer();
	}

	return strResult;
}


_bstr_t GetUserActionName(char *szName)
{ // получить лузерское имя акции
	IApplicationPtr sptrApp = ::GetAppUnknown();

	IUnknownPtr ptrActionMan;
	if(sptrApp) sptrApp->GetActionManager( &ptrActionMan );
	IActionManagerPtr sptrActionMan(ptrActionMan);

	IUnknownPtr ptrAI;
	if(sptrActionMan) sptrActionMan->GetActionInfo( _bstr_t(szName), &ptrAI );
	IActionInfoPtr sptrAI(ptrAI);

	if(sptrAI)
	{
		_bstr_t bstrActionName, bstrActionUserName, bstrHelpString;
		sptrAI->GetCommandInfo( bstrActionName.GetAddress(), bstrActionUserName.GetAddress(), bstrHelpString.GetAddress(), 0 );
		return bstrActionUserName;
	}
	else
	{ // ни хрена нет такой акции - ну, авось технологи прописали строку...
		return _bstr_t(LoadString(szName));
		// (а если не прописали - то так и оставим)
	}
}

CMethodStep& CMethodStep::operator= (CMethodStep& x) // копирующее присваивание
{
	if(this == &x) return *this;
	m_bSkipData = false;
	CopyFrom(x);
	m_bSkipData = x.m_bSkipData;
	return *this;
}

void CMethodStep::CopyFrom(CMethodStep& x)
{ // скопировать - с учетом флагов bSkipParams, bSkipData
	if(this == &x) return;
	m_dwFlags = x.m_dwFlags;

	m_bstrActionName = x.m_bstrActionName;
	m_bstrUserName = x.m_bstrUserName;
	m_bstrForcedName = x.m_bstrForcedName;
	if(m_bstrForcedName==_bstr_t("")) m_bstrForcedName=m_bstrActionName;
	m_bstrUserName = GetUserActionName(m_bstrForcedName); // получаем лузерское имя
	
	if( !(x.m_bSkipData || m_bSkipData) )
	{
		//m_NamedDataKeys.clear();
		//m_NamedDataValues.clear();
		//for(LPOS lpos = x.m_NamedDataKeys.head(); lpos; lpos = x.m_NamedDataKeys.next(lpos) )
		//	m_NamedDataKeys.add_tail(x.m_NamedDataKeys.get(lpos));
		//for(LPOS lpos = x.m_NamedDataValues.head(); lpos; lpos = x.m_NamedDataValues.next(lpos) )
		//	m_NamedDataValues.add_tail(x.m_NamedDataValues.get(lpos));
		CopyNamedData(m_ptrNamedData, x.m_ptrNamedData);
	}
}


//#define RRR(name) _dbg_tracer _rrr(name);
#define RRR(name) 

CMethodStep::CMethodStep() // конструктор по умолчанию
{
	RRR("CMethodStep::CMethodStep")
	m_bSkipData = false;
	m_dwFlags = msfChecked;
	m_ptrNamedData.CreateInstance( szNamedDataProgID );
}

CMethodStep::CMethodStep(CMethodStep& x) // конструктор копирования
{
	RRR("CMethodStep::CMethodStep(x)")
	m_bSkipData = false;
	m_dwFlags = msfChecked;
	m_ptrNamedData.CreateInstance( szNamedDataProgID );
	*this = x;
}

bool RecursiveAddEntry2( INamedData* ptrDst, INamedData* ptrSrc )
{
	RRR("RecursiveAddEntry2")
	if( 0==ptrSrc || 0==ptrDst ) return false;
	_bstr_t bstrPathSrc;
	_bstr_t bstrPathDst;

	ptrSrc->GetCurrentSection(bstrPathSrc.GetAddress());
	ptrDst->GetCurrentSection(bstrPathDst.GetAddress());

	// get count of entries
	long EntryCount = 0;
	ptrSrc->GetEntriesCount( &EntryCount );

	// for all entries
	for( int i = 0; i<(int)EntryCount; i++ )
	{
		ptrSrc->SetupSection(bstrPathSrc);
		ptrDst->SetupSection(bstrPathDst);

		_bstr_t bstrEntryName;
		ptrSrc->GetEntryName( i, bstrEntryName.GetAddress() );
		_bstr_t bstrFullNameSrc = bstrPathSrc + "\\" + bstrEntryName;
		_bstr_t bstrFullNameDst = bstrPathDst + "\\" + bstrEntryName;

		if(bstrEntryName.length()==0) continue; // всякую хрень накидают, а я убирай? хуюшки!
		
		{
			// get value
			_variant_t var;
			ptrSrc->GetValue( bstrEntryName, &var );
			ptrDst->SetValue( bstrEntryName, var );
			
			// if this entry has children => we want to walk to them
			if( S_OK != ptrSrc->SetupSection( bstrPathSrc + "\\" + bstrEntryName ) )
				continue;
			if( S_OK != ptrDst->SetupSection( bstrPathDst + "\\" + bstrEntryName ) )
				continue;
			RecursiveAddEntry2( ptrDst, ptrSrc );
		}
	}
	
	// после себя оставляем все в исходном состоянии
	ptrSrc->SetupSection( bstrPathSrc );
	ptrDst->SetupSection( bstrPathDst );

	return true;
}

bool RecursiveUpdateEntry2( INamedData* ptrDst, INamedData* ptrSrc )
{ // обновляет значения всех ключей в ptrDst
	if( 0==ptrSrc || 0==ptrDst ) return false;
	_bstr_t bstrPathSrc;
	_bstr_t bstrPathDst;

	ptrSrc->GetCurrentSection(bstrPathSrc.GetAddress());
	ptrDst->GetCurrentSection(bstrPathDst.GetAddress());

	// get count of entries
	long EntryCount = 0;
	ptrDst->GetEntriesCount( &EntryCount );

	// for all entries
	for( int i = 0; i<(int)EntryCount; i++ )
	{
		ptrSrc->SetupSection(bstrPathSrc);
		ptrDst->SetupSection(bstrPathDst);

		_bstr_t bstrEntryName;
		ptrDst->GetEntryName( i, bstrEntryName.GetAddress() );

		if(bstrEntryName.length()==0) continue; // всякую хрень накидают, а я убирай? хуюшки!
		
		{
			// get value
			_variant_t var;
			ptrSrc->GetValue( bstrEntryName, &var );
			ptrDst->SetValue( bstrEntryName, var );
			
			// if this entry has children => we want to walk to them
			if( S_OK != ptrSrc->SetupSection( bstrPathSrc + "\\" + bstrEntryName ) )
				continue;
			if( S_OK != ptrDst->SetupSection( bstrPathDst + "\\" + bstrEntryName ) )
				continue;
			RecursiveUpdateEntry2( ptrDst, ptrSrc );
		}
	}
	
	// после себя оставляем все в исходном состоянии
	ptrSrc->SetupSection( bstrPathSrc );
	ptrDst->SetupSection( bstrPathDst );

	return true;
}

void CMethodStep::StoreToNamedData(IUnknown* punkND, const char *pszSection)
{
	INamedDataPtr sptrND(punkND);

	if(sptrND==0)
	{
		ASSERT(sptrND!=0);
		return; // ошибка, да еще какая
	}

	{
		m_ptrNamedData->SetupSection(_bstr_t("\\"));
		sptrND->SetupSection(_bstr_t(pszSection));
		RecursiveAddEntry2(sptrND, m_ptrNamedData);
	}

	CString strPathInternal = CString(pszSection) + "\\_step";

	::SetValue(punkND,strPathInternal,"ActionName", m_bstrActionName);
	::SetValue(punkND,strPathInternal,"ForcedName", m_bstrForcedName);
	//::SetValue(punkND,strPathInternal,"UserName", m_bstrUserName);

	::SetValue(punkND,strPathInternal,"Flags", long(m_dwFlags));
}

void CMethodStep::LoadFromNamedData(IUnknown* punkND, const char *pszSection)
{
	CString strPathInternal = CString(pszSection) + "\\_step";
	m_bstrActionName = ::GetValueString(punkND,strPathInternal,"ActionName");
	//m_bstrUserName = ::GetValueString(punkND,strPathInternal,"UserName");
	m_bstrForcedName = ::GetValueString(punkND,strPathInternal,"ForcedName","");
	if(m_bstrForcedName==_bstr_t("")) m_bstrForcedName=m_bstrActionName;
	m_bstrUserName = GetUserActionName(m_bstrForcedName); // получаем лузерское имя

	m_dwFlags = ::GetValueInt(punkND,strPathInternal,"Flags", 0);

	{
		m_ptrNamedData = 0; // придушим
		m_ptrNamedData.CreateInstance( szNamedDataProgID ); // создадим заново - пустую
		INamedDataPtr sptrND(punkND);
		if(sptrND==0)
		{
			ASSERT(sptrND!=0);
			return; // ошибка, да еще какая
		}
		m_ptrNamedData->SetupSection(_bstr_t("\\"));
		sptrND->SetupSection(_bstr_t(pszSection));
		RecursiveAddEntry2(m_ptrNamedData, sptrND);
	}
}

_bstr_t CMethodStep::GetScript(bool bWithSetValue)
{
	_bstr_t bstr;
	if(bWithSetValue)
	{
		bstr = bstr + "\' SetValues in script not implemented.\r";

		/*
		LPOS lpos1, lpos2;
		for(lpos1=m_NamedDataKeys.head(), lpos2=m_NamedDataValues.head();
			lpos1 && lpos2;
			lpos1=m_NamedDataKeys.next(lpos1), lpos2=m_NamedDataValues.next(lpos2))
		{
			_variant_t varVal = m_NamedDataValues.get(lpos2);
			CString strVal(varVal);
			if(varVal.vt == VT_BSTR)
			{
				strVal.Replace("\"","\"\"");
				strVal = "\"" + strVal + "\"";
			}
			else if(varVal.vt == VT_I4)
			{
				strVal = CString("CLng(") + strVal + ")";
			}
			else if(varVal.vt == VT_R8)
			{
				strVal = CString("CDbl(") + strVal + ")";
			}
			bstr = bstr +
				"Data.SetValue " +
				"\"" + m_NamedDataKeys.get(lpos1) + "\", " + strVal + "\r";
		}
		*/
	}

	if(m_bstrActionName == _bstr_t(szBeginMethodLoop))
	{
		bstr += "Do";
	}
	else
	if(m_bstrActionName == _bstr_t(szEndMethodLoop))
	{
		bstr += "Loop While NeedRepeatMethodLoop";
	}
	else
	{
		bstr += m_bstrActionName;
	}

	bstr = bstr + "\r";

	return bstr;
}

bool CMethodStep::ApplyToShellData()
// TODO: переделать, чтобы учитывались disabled_method_keys и \Paths
{	// закинуть все настройки шага в shell.data
	INamedDataPtr sptrShellData(::GetAppUnknown());
	ASSERT(sptrShellData!=0);
	if(sptrShellData==0) return false;
	if(m_ptrNamedData==0) return false;

	_bstr_t bstrPathNamedData;
	_bstr_t bstrPathShellData;

	m_ptrNamedData->GetCurrentSection(bstrPathNamedData.GetAddress());
	sptrShellData->GetCurrentSection(bstrPathShellData.GetAddress());

	m_ptrNamedData->SetupSection(_bstr_t("\\"));
	sptrShellData->SetupSection(_bstr_t("\\"));
	RecursiveAddEntry2(sptrShellData, m_ptrNamedData);

	// после себя оставляем все в исходном состоянии
	m_ptrNamedData->SetupSection( bstrPathNamedData );
	sptrShellData->SetupSection( bstrPathShellData );

	return true;
}
