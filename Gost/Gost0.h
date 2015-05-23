#ifndef __GOST_H
#define __GOST_H

#include <BaseTsd.h>

/*----------------------------------------------------
  gost.h -- описания данных и  прототипы функций шиф-
	    рования по ГОСТ 28147-89.
  ----------------------------------------------------
  Версия 2.01 от 05.07.98, (c)1994,1998 Винокуров А.Ю.
  ----------------------------------------------------
  Варианты компиляции:
  Символ VarKey:
  - если не определен, длина ключа в функциях шифро-
    вания фиксирована и не передается как параметр;
  - если определен, длина ключа передается как пара-
    метр в функции шифрования;
  Символ CryptoImport:
  - если определен, криптофункции описываются как
    __import - функции.
  ----------------------------------------------------
*/


typedef unsigned long CRItem;	// элемент данных

typedef struct					// криптоблок
{
	CRItem	low, high;
} CrBlock;

#ifdef _cplusplus
	extern "C" {
#endif

// Расширение таблицы замен
void __cdecl ExpCht(CRItem * source,		//   область исходной ТЗ
					CRItem * target);		//   область расширенной ТЗ

// Любое расширение ключа
void __cdecl ExpandKey (CRItem * source,	//   область исходного ключа
						CRItem * target,	//   область расширен. ключа
						DWORD_PTR    KeyLength,	//   размер ключа
						DWORD_PTR    KeyRepeat,	//   число повторений ключа
						DWORD_PTR    RepeatMask,
						...);				//   маска повторений ключа

// Гаммирование данных
void __cdecl gamma (CRItem * key,			//   адрес ключа
					CRItem * synchro,		//   адрес синхропосылки
					void *	 xcht,			//   адрес таблицы замен
					void *   data,			//   адрес гаммируемых данных
					DWORD_PTR    blocks,		//   число шифруемых блоков
					DWORD_PTR    keylen);       //   длина ключа в элементах

// Выработка гаммы
void __cdecl gamme (CRItem * key,			//   адрес ключа
					CRItem * synchro,		//   адрес синхропосылки
					void *   xcht,			//   адрес таблицы замен
					void *   data,			//   адрес гаммируемых данных
					DWORD_PTR    blocks,		//   число шифруемых блоков
					DWORD_PTR    keylen);		//   длина ключа в элементах

// Зашифрование-гаммир. с ОС
void __cdecl gammaLE(CRItem * key,			//   адрес ключа
					 CRItem * synchro,		//   адрес синхропосылки
					 void *   xcht,			//   адрес таблицы замен
					 void *   data,			//   адрес гаммируемых данных
					 DWORD_PTR    blocks,		//   число шифруемых блоков
					 DWORD_PTR    keylen);		//   длина ключа в элементах

// Расшифрование-гаммир. с ОС
void __cdecl gammaLD(CRItem * key,			//   адрес ключа
					 CRItem * synchro,		//   адрес синхропосылки
					 void *   xcht,			//   адрес таблицы замен
					 void *   data,			//   адрес гаммируемых данных
					 DWORD_PTR    blocks,		//   число шифруемых блоков
					 DWORD_PTR    keylen);		//   длина ключа в элементах


// Шифрование простой заменой
void __cdecl simple(CRItem * key,		//   адрес ключа
					void*   data,							//   адрес шифруемых данных
					void*   xcht,							//   адрес таблицы замен
					DWORD_PTR    blocks,			//   число шифруемых блоков
					DWORD_PTR    keylen);			//   длина ключа в элементах

// Вычисление имитоприставки
CRItem __cdecl imito(CRItem * key,			//   адрес ключа
					 CRItem * combine,		//   адрес начальной комбинации
					 void *   xcht,			//   адрес таблицы замен
					 void *   data,			//   адрес гаммируемых данных
					 DWORD_PTR    blocks,		//   число блоков
					 DWORD_PTR    keylen);		//   длина ключа в элементах

#ifdef _cplusplus
}  
#endif



#endif// __GOST_H
