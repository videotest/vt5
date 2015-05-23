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
#include <windows.h>

typedef unsigned long CrItem;	// элемент данных

typedef struct					// криптоблок
{
	CrItem	low, high;
} CrBlock;

#ifdef __cplusplus
	extern "C" {
#endif

// Расширение таблицы замен
void __cdecl ExpCht(CrItem * source,		//   область исходной ТЗ
					CrItem * target);		//   область расширенной ТЗ

// Любое расширение ключа
void __cdecl ExpandKey (CrItem * source,	//   область исходного ключа
						CrItem * target,	//   область расширен. ключа
						DWORD    KeyLength,	//   размер ключа
						DWORD    KeyRepeat,	//   число повторений ключа
						DWORD    RepeatMask,
						...);				//   маска повторений ключа

// Гаммирование данных
void __cdecl gamma (CrItem * key,			//   адрес ключа
					CrItem * synchro,		//   адрес синхропосылки
					void *	 xcht,			//   адрес таблицы замен
					void *   data,			//   адрес гаммируемых данных
					DWORD_PTR    blocks,		//   число шифруемых блоков
					DWORD    keylen);       //   длина ключа в элементах

// Выработка гаммы
void __cdecl gamme (CrItem * key,			//   адрес ключа
					CrItem * synchro,		//   адрес синхропосылки
					void *   xcht,			//   адрес таблицы замен
					void *   data,			//   адрес гаммируемых данных
					DWORD_PTR    blocks,		//   число шифруемых блоков
					DWORD    keylen);		//   длина ключа в элементах

// Зашифрование-гаммир. с ОС
void __cdecl gammaLE(CrItem * key,			//   адрес ключа
					 CrItem * synchro,		//   адрес синхропосылки
					 void *   xcht,			//   адрес таблицы замен
					 void *   data,			//   адрес гаммируемых данных
					 DWORD_PTR    blocks,		//   число шифруемых блоков
					 DWORD    keylen);		//   длина ключа в элементах

// Расшифрование-гаммир. с ОС
void __cdecl gammaLD(CrItem * key,			//   адрес ключа
					 CrItem * synchro,		//   адрес синхропосылки
					 void *   xcht,			//   адрес таблицы замен
					 void *   data,			//   адрес гаммируемых данных
					 DWORD_PTR    blocks,		//   число шифруемых блоков
					 DWORD    keylen);		//   длина ключа в элементах


// Шифрование простой заменой
void __cdecl simple(CrItem * key,			//   адрес ключа
					void *   data,			//   адрес шифруемых данных
					void *   xcht,			//   адрес таблицы замен
					DWORD_PTR    blocks,		//   число шифруемых блоков
					DWORD    keylen);		//   длина ключа в элементах

// Вычисление имитоприставки
CrItem __cdecl imito(CrItem * key,			//   адрес ключа
					 CrItem * combine,		//   адрес начальной комбинации
					 void *   xcht,			//   адрес таблицы замен
					 void *   data,			//   адрес гаммируемых данных
					 DWORD_PTR    blocks,		//   число блоков
					 DWORD    keylen);		//   длина ключа в элементах

#ifdef __cplusplus
}  
#endif

//#define _GOST_LIBRARY "xgost32s.lib"
//#pragma comment(lib, _GOST_LIBRARY)
//#pragma message("gost 32 library linking with " _GOST_LIBRARY)



#endif// __GOST_H
