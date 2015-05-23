#include "StdAfx.h"
#include "treefilter.h"
#include <fstream>
#include <set>

CTreeFilterLevel::CTreeFilterLevel(CTreeFilterLevel* pParent, TL_USE usage)
: m_pParent(pParent), m_Usage(usage)
{
	m_pKeys = new CTreeFilterLevelMap;
}

CTreeFilterLevel::CTreeFilterLevel(const CTreeFilterLevel &arg)
{
	m_Usage = arg.m_Usage;
	m_pParent = arg.m_pParent;

	m_pKeys = new CTreeFilterLevelMap;
	CTreeFilterLevelMap::iterator	it_map;
	for(it_map = arg.m_pKeys->begin(); it_map!=arg.m_pKeys->end(); ++it_map)
	{
		(*m_pKeys) [ (*it_map).first ] = (*it_map).second;
	}
};

const CTreeFilterLevel& CTreeFilterLevel::operator=(const CTreeFilterLevel& arg)
{
	m_Usage = arg.m_Usage;
	m_pParent = arg.m_pParent;

	m_pKeys->clear();
	CTreeFilterLevelMap::iterator	it_map;
	for(it_map = arg.m_pKeys->begin(); it_map!=arg.m_pKeys->end(); ++it_map)
	{
		(*m_pKeys) [ (*it_map).first ] = (*it_map).second;
	}

	return *this;
};

CTreeFilterLevel::~CTreeFilterLevel()
{
	delete m_pKeys;
}

void split(wstring & text, wstring & separators, list<wstring> &words)
{
	int n = text.length();
	int start, stop;

	// найти первую букву, не разделитель
	start = text.find_first_not_of(separators);

	while ( (start >=0) && (start < n) )
	{
		// найти конец текущего слова
		stop = text.find_first_of(separators, start);
		if ( (stop <=0) || (stop > n) ) stop = n;

		// добавить слово к списку слов
		words.push_back(text.substr(start, stop-start));

		// найти начало следующего слова
		start = text.find_first_not_of(separators,stop+1);
	}
}

CTreeFilterLevel* CTreeFilterLevel::Find(wstring s)
{	// найти самый нижний уровень, соответствующий строке s
	list<wstring> words;
	split(s, wstring(L"\\"), words);
	return Find(words);
	return 0;
}

CTreeFilterLevel* CTreeFilterLevel::Find(list<wstring> &words, bool bInsert)
{	// найти самый нижний уровень, соответствующий разобранной строке
	CTreeFilterLevel* p = this;
	list<wstring>::iterator it;

	for(it = words.begin(); it!=words.end(); ++it)
	{
		CTreeFilterLevelMap::iterator	it_map =
			p->m_pKeys->find(*it);
		if( it_map == p->m_pKeys->end() )
		{ // не нашли
			if(bInsert)
			{ // вставим
				CTreeFilterLevel* p2 = & (*p->m_pKeys)[*it];
				//CTreeFilterLevel new_level(p, tlDefault);
				//(*p->m_pKeys)[*it] = new_level; // воткнем в мапу
				p2->m_Usage = tlDefault;
				p2->m_pParent = p;
				p = p2;
			}
			else
			{ //- берем default
				it_map =
					p->m_pKeys->find(wstring(L"*"));
				if( it_map == p->m_pKeys->end() ) // даже и default нет
					return p;
			}
		}
		else
			p = & it_map->second;
	}
	return p;
}

bool CTreeFilterLevel::Set(list<wstring> &words, TL_USE value)
{ // установить настройку для конкретного ключа
	CTreeFilterLevel* p = Find(words, true);
	p->m_Usage = value;
	return true;
}

bool CTreeFilterLevel::Set(wstring path, TL_USE value)
{ // установить настройку для конкретного ключа
	list<wstring> words;
	split(path, wstring(L"\\"), words);
	return Set(words, value);
}


CTreeFilterLevel::TL_USE CTreeFilterLevel::Get(list<wstring> &words)
{ // получить настройку; в случае tlDefault - спросить у парента
	CTreeFilterLevel* p = Find(words, false);
	while(p->m_Usage == tlDefault && p->m_pParent)
		p = p->m_pParent;
	return p->m_Usage;
}

bool CTreeFilterLevel::Load(const char* szFileName)
{
	wifstream in(szFileName);
	in.imbue(std::locale(""));

	if( !in.good() ) return false;

	wstring s;
	wstring path_splitters(L"\\");

	while(!in.eof())
	{
		getline(in, s);
		if( !in.good() ) return false;

		if(s.empty()) continue;
		if(s[0] == '#') continue; // комментарий

		TL_USE usage = tlDefault;

		char tags[] = "><-*";
		TL_USE usages[] = {tlUseRight, tlUseLeft, tlUseNone, tlDefault};
		char *p = strchr( tags, s[0] );
		if(p)
		{
			usage = usages[p-tags];
			s.erase(s.begin()); // удалим первый символ
		}

		list<wstring> path;
		split(s, path_splitters, path);
		Set(path, usage);
	}

	return true;
}

bool CTreeFilterLevel::Save(wofstream &out, list<wstring> &path)
{
	if(m_Usage==tlUseLeft)
		out << L"<";
	else if(m_Usage==tlUseRight)
		out << L">";
	else if(m_Usage==tlUseNone)
		out << L"-";
	else
		out << L"*";

	list<wstring>::iterator it;
	for(it = path.begin(); it!=path.end(); ++it)
		out << L"\\" << *it;
	out << endl;

	CTreeFilterLevelMap::iterator	it_map;
	for(it_map = m_pKeys->begin(); it_map!=m_pKeys->end(); ++it_map)
	{
		path.push_back( (*it_map).first );
		(*it_map).second.Save(out, path);
		path.pop_back();
	}
	return true;
}

bool CTreeFilterLevel::Save(const char* szFileName)
{ // для проверки
	wofstream out(szFileName);
	out.imbue(std::locale(""));
	list<wstring> path;
	Save(out, path);

	return true;
}

inline void EnumEntries(INamedData* pND, set<_bstr_t> &entries, BSTR bstrPath)
{
	if( S_OK != pND->SetupSection( bstrPath ) )
		return; // нет секции - ну и фиг с ним

	long nEntryCount = 0;
	pND->GetEntriesCount( &nEntryCount );

	// for all entries
	for( int i = 0; i<nEntryCount; i++ )
	{
		_bstr_t bstrEntryName;
		pND->GetEntryName( i, bstrEntryName.GetAddress() );
		if(bstrEntryName.length()==0) continue; // всякую хрень накидают, а я убирай? хуюшки!

		entries.insert( bstrEntryName );
	}
}

//!!! debug
//wofstream *g_pOut=0;

inline void subst_from_map(_variant_t &var, CSubstMap* pSubstMap)
{
	if(!pSubstMap) return;
	// проверим на соответствие шаблонам из списка
	if( VT_BSTR == var.vt )
	{
		CString str(var);
		TPOS lPosMap = pSubstMap->head();
		while(lPosMap)
		{
			CString strFrom = pSubstMap->get_key(lPosMap);
			CString strTo = pSubstMap->get(lPosMap);
			lPosMap = pSubstMap->next(lPosMap);

			long n = strFrom.GetLength();
			if( 0 == strFrom.CompareNoCase(str.Left(n)) )
			{
				str.Delete(0, n);
				str.Insert(0, strTo);
				var = str;
				break;
			}
		}
	}
}

bool CTreeFilterLevel::RecursiveMerge(INamedData* pLeft, INamedData* pRight, INamedData* pResult, TL_USE FilterBy, _bstr_t bstrPath, CSubstMap* pLeftMap, CSubstMap* pRightMap)
{ // внутренняя ф-я
	// 1. Обойти все ветви Left и Right
	// 2. Установить текущее значение в соответствии с настройкой

	set<_bstr_t> entries;
	EnumEntries(pLeft, entries, bstrPath);
	EnumEntries(pRight, entries, bstrPath);

	if(tlDefault != m_Usage)
		FilterBy = m_Usage; // по возможности берем Usage из настроек

	//(*g_pOut) << bstrPath.GetBSTR() << L" :" << FilterBy << endl;

	set<_bstr_t>::iterator it;
	for( it = entries.begin(); it != entries.end(); it++ )
	{ // for all entries
		CTreeFilterLevelMap::iterator	it_map =
			m_pKeys->find(wstring(*it));

		if( it_map == m_pKeys->end() )
			it_map = m_pKeys->find(wstring(L"*")); // берем default

		if( it_map == m_pKeys->end() )
		{	// даже и default нет - процессим сами
			// можно ускорить - копируя одну из секций целиком
			RecursiveMerge( pLeft, pRight, pResult, FilterBy, bstrPath + "\\" + *it, pLeftMap, pRightMap );
		}
		else
		{
			it_map->second.RecursiveMerge( pLeft, pRight, pResult, FilterBy, bstrPath + "\\" + *it, pLeftMap, pRightMap );
		}
	}

	_variant_t var;
	if(tlUseLeft == FilterBy)
	{ // использовать левую ND
		pLeft->GetValue( bstrPath, &var );
		if(VT_EMPTY == var.vt) // если нет в левой - все-таки возьмем из правой
			pRight->GetValue( bstrPath, &var );
		subst_from_map( var, pLeftMap );
	}
	else if(tlUseRight == FilterBy)
	{ // использовать правую ND
		pRight->GetValue( bstrPath, &var );
		if(VT_EMPTY == var.vt) // если нет в правой - все-таки возьмем из левой
			pLeft->GetValue( bstrPath, &var );
		subst_from_map( var, pRightMap );
	}

	if(VT_EMPTY != var.vt)
		pResult->SetValue( bstrPath, var );

	return true;
}

bool CTreeFilterLevel::Merge(INamedData* pLeft, INamedData* pRight, INamedData* pResult, TL_USE FilterBy, CSubstMap* pLeftMap, CSubstMap* pRightMap)
{ // слить две Named Data
	//wofstream out("d:\\merge.log");
	//out.imbue(std::locale("rus"));
	//g_pOut = &out;
	time_test ttt("CTreeFilterLevel::Merge");
	if( 0==pLeft || 0==pRight || 0==pResult )
		return false;

	return RecursiveMerge(pLeft, pRight, pResult, FilterBy, _bstr_t(L""));
}

// -----------------------------------------------

CTreeFilter::CTreeFilter(void)
: CTreeFilterLevel(0, tlDefault)
{
}

CTreeFilter::~CTreeFilter(void)
{
}
