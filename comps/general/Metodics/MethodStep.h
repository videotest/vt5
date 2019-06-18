#pragma once
#include <comutil.h>
#include "method_int.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_map.h"
#include "PropBagImpl.h"
#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\misc_list.h"

//#include "MethodDoer.h"
class CMethodDoer; // предварительное объявление класса - понадобится в CMethod::m_xMethodChanges

bool TestMethod(); // тестирование объектов данных. вызвать и посмотреть, что будет

struct tagMethodStepNotification
{
	long lHint;
	long lPos;
};

enum MethodStepFlags
{
	msfHasPropPage	= 1<<0,	// с формой (ставится автоматически при записи)
	msfShowPropPage	= 1<<1, // показывать форму
	msfChecked		= 1<<2, // шаг разрешен?
	msfUndead		= 1<<3, // нельзя удалить этот шаг
	msfStateless	= 1<<4, // у шага нет своего состояния и кэша - тянуть все с предыдущего (для Message Step)
	msfInteractive	= 1<<5, // шаг интерактивный
	msfWriting		= 1<<6, // шаг находится в процессе записи (не для технологов). Такой шаг всегда можно отменить.
};

bool RecursiveAddEntry2( INamedData* ptrDst, INamedData* ptrSrc );
bool RecursiveUpdateEntry2( INamedData* ptrDst, INamedData* ptrSrc );
// обновляет значения всех ключей в ptrDst

class CMethodStep
{
public:
	_bstr_t				m_bstrActionName; // имя акции (по нему можно вызывать)
	_bstr_t				m_bstrUserName; // пользовательское имя акции (локализованное)
	_bstr_t				m_bstrForcedName; // имя, проставленное технологом из скрипта. если не "" - то прочитать по нему из ресурсов и воткнуть в m_bstrUserName (вместо юзернейма акции m_bstrActionName);

	INamedDataPtr		m_ptrNamedData; // Урезанная NamedData - только то, что использовалось на данном шаге
	DWORD				m_dwFlags;

	bool				m_bSkipData;   // если true - не копировать соотв. параметр
	// используются только при копировании

	CMethodStep(); // конструктор по умолчанию
	CMethodStep(CMethodStep& x); // конструктор копирования
	CMethodStep& operator= (CMethodStep& x); // копирующее присваивание
	void CopyFrom(CMethodStep& x); // скопировать - с учетом флагов bHasParams, bHasData
	void StoreToNamedData(IUnknown* punkND, const char *pszSection);
	void LoadFromNamedData(IUnknown* punkND, const char *pszSection);
	bool ApplyToShellData(); // закинуть все настройки шага в shell.data
	_bstr_t GetScript(bool bWithSetValue=false);
};
