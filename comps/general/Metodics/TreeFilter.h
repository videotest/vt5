#pragma once
#include <string>
#include <map>
#include <list>
#include "MethodDataUtil.h"

class CTreeFilterLevel;

typedef map<wstring, CTreeFilterLevel> CTreeFilterLevelMap;

class CTreeFilterLevel
{
	CTreeFilterLevelMap* m_pKeys; // »нформаци€ дл€ нижележащих ключей
	CTreeFilterLevel* m_pParent; // Level уровнем выше

public:
	enum TL_USE
	{
		tlDefault,
		tlUseLeft,
		tlUseRight,
		tlUseNone,
	} m_Usage; // использовать дл€ данного ключа значени€ левой
	// сравниваемой ветви или правой

public:
	CTreeFilterLevel(CTreeFilterLevel* pParent=0, TL_USE usage=tlDefault);
	virtual ~CTreeFilterLevel();

	bool Set(list<wstring> &words, TL_USE value); // установить настройку дл€ конкретного ключа
	bool Set(wstring path, TL_USE value); // установить настройку дл€ конкретного ключа
	TL_USE Get(list<wstring> &words); // получить настройку; в случае tlDefault - спросить у парента

	bool Load(const char* szFileName); // загрузить из файла
	bool Save(const char* szFileName); // сохранить в файл
	bool Save(wofstream &out, list<wstring> &path);

	CTreeFilterLevel* Find(wstring s); // найти самый нижний уровень, соответствующий строке s
	CTreeFilterLevel* Find(list<wstring> &words, bool bInsert=false); // то же самое, но строка уже разобрана на уровни
	// если bInsert - то при отсутствии нужной ветви дерева создать ее

	bool Merge(INamedData* pLeft, INamedData* pRight, INamedData* pResult, TL_USE FilterBy, CSubstMap* pLeftMap=0, CSubstMap* pRightMap=0); // слить две Named Data
	bool RecursiveMerge(INamedData* pLeft, INamedData* pRight, INamedData* pResult, TL_USE FilterBy, _bstr_t bstrPath, CSubstMap* pLeftMap=0, CSubstMap* pRightMap=0); // внутренн€€ ф-€

public:
	CTreeFilterLevel(const CTreeFilterLevel &arg);
	const CTreeFilterLevel& operator=(const CTreeFilterLevel& arg);
};


class CTreeFilter : public CTreeFilterLevel
{
public:
	CTreeFilter();
	virtual ~CTreeFilter();
};
