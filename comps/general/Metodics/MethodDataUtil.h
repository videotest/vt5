#pragma once
// ”тилиты дл€ использовани€ в методиках:

enum LoadStateFlags
{
	lssSwitchViews = 1<<0, // переключать вьюхи?
	lssActivateObjects = 1<<1, // активизировать объекты в контексте?
	lssRestoreZoom = 1<<2, // восстанавливать zoom и scroll pos вьюх.
	lssActiveViewOnly = 1<<3, // грузить только состо€ние активного окна, не раздел€€ сплиттер заново.
};

// сохранение сплиттера (либо отдельно вьюх/селекшнов) в NamedData
void StoreSplitter(const char *szSection, IUnknown* punkNamedData, bool bSimplified = false);
void LoadSplitter(const char *szSection, IUnknown* punkNamedData, DWORD dwFlags);

// сравнить путь (в named data) с эталонами
// каждый эталон в отдельной строке, строки разделены CR и/или LF
// true, если szName совпадает с одним из эталонов или находитс€
// внутри секции-эталона
bool CompareWildcards(const char* szName, const char* szWild);

// вспомогательна€ ф-€
inline long cmp_len_CString( const CString psz1, const CString psz2 )
{ // дл€ сортировки строк из \\Paths: самые длинные в начало, одной длины - по алфавиту
	long n = psz1.GetLength() - psz2.GetLength();
	if(n) return n;
	return psz2.CompareNoCase( psz1 );
}

typedef _list_map_t<CString, CString, cmp_len_CString> CSubstMap;


// ‘ильтраци€: перекладываем все, кроме pszDisabledKeys, из одной NamedData
// в другую; при этом подмен€ем все пути в соответствии с pSubstMap (все
// строки, начинающиес€ с ключей pSubstMap замен€ем на значени€ pSubstMap).
bool RecursiveAddEntry( INamedData* ptrDst, INamedData* ptrSrc, LPCSTR pszEntry,
					   char* pszDisabledKeys, CSubstMap* pSubstMap );

// импорт shell.data, за исключением DisabledKeys и с подменой
// всех путей из [Paths] на текущие
bool ImportShellData( IUnknown* punkSrc, char* pszDisabledKeys );

// импорт shell.data с учетом MergeKeys и с подменой
// всех путей из [Paths] на текущие
bool ImportShellData2( IUnknown* punkSrc );
