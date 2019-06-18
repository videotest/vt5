#pragma once

#include <time.h>


#ifdef ABACUSCTRL_EXPORTS
#define ABACUSCTRL_API __declspec(dllexport)
#else
#define ABACUSCTRL_API __declspec(dllimport)
#endif

// Настройки для работы AbacusCtrl.dll
struct ABACUSCONTROLSETUP
{
	int nPackageBuffSize; // Размер буффера для пакета.
	int nDebugLog; // Выводить ли отладочную информацию
	bool bRequestRBC; // Запрашивать ли диаграмму RBC
	bool bRequestWBC; // Запрашивать ли диаграмму WBC
	bool bRequestPLT; // Запрашивать ли диаграмму PLT
	int nConnectTimeoutMs; // Сколько времени (в мс) ждать ответа абакуса
	                       // на запрос <ENQ> при соединении
};


// Настройки COMM-порта
struct COMMSETUP
{
	int nBaud; //CBR_2400, CBR_4800, CBR_9600 и т.д.
	int nStopBits; // ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
	int nParity; // NOPARITY, EVENPARITY, ODDPARITY, MARKPARITY или SPACEPARITY
	int nDataBits; // Число бит данных
	int nReadTotalTimeoutConstant; // Таймаут при чтении одного символа
	char szComm[50]; // Название, например "Com1"
};

// Состояние AbacusCtrl.dll
enum ABACUSSTATE
{
	AbacusState_NotConnected = 0,
	AbacusState_Connecting = 1,
	AbacusState_Connected = 5,
	AbacusState_Error = 6,
};

// Данные пакета INIT с абакуса
struct ABACUSINIT
{
	char *pszDevice;// Например "Abacus Junior Human"
	char *pszVer;// Например "2.66"
	tm   tmMsg;// Дата и время
};

// Информация о параметре
struct ABACUSPARAMETER
{
	const char *pszParID; // Название, например P01, P02 и т.д.
	double dParValue; // Значение
	int nParFlag; // Выдаваемые прибором предупреждения
};

// Данные пакета DATA с абакуса - общая информация об одном анализе.
// Для каждого поля в скобках дан комментарий из описания протокола
// (http://www.diatronltd.com/downloads/serpro17.pdf)
struct ABACUSDATA
{
	int nSNO; // Поле SNO (internal identifier), приведенное к числовому виду
	tm  tmMeasurement; // Данные полей DATE и TIME
	const char *pszSID; // Поле SID (sample identifier (alphanumeric))
	const char *pszPID; // Поле PID (patient identifier (alphanumeric))
	const char *pszName;// Поле NAME (patient name (alphanumeric))
	tm dtBirth;// Поле BIRTH. Структура содержит только дату, не время.
	int nSex; // Поле SEX(0 -male, 1- female)
	const char *pszDoc;// Поле DOC (doctor's name)
	const char *pszOPID;// Поле OPID (operator identifier (alphanumeric))
	int nMode;// Поле MODE (patient type)
	DWORD dwWarn;// Поле WARN (warning bits)
	int nPM1; // Поле PM1 (PLT lower marker)
	int nPM2; // Поле PM2 (PLT upper marker)
	int nRM1; // Поле RM1 (RBC lower marker)
	int nWM1; // Поле WM1 (WBC upper marker)
	int nWM2; // Поле WM2 (WBC LYM-MID marker)
	int nWM3; // Поле WM3 (WBC MID-GRA marker)
	int nParn; // Поле PARN (Number of parameters)
	ABACUSPARAMETER *padParams; // Массив параметров размером nParn
};

// Данные пакетов RBC, WBC, PLT с абакуса - гистограмма.
// Для каждого поля в скобках дан комментарий из описания протокола
// (http://www.diatronltd.com/downloads/serpro17.pdf)
struct ABACUSHISTOGRAM
{
	int nSNO;// Поле SNO (internal identifier), приведенное к числовому виду
	tm tmMeasurement;// Данные полей DATE и TIME
	const char *pszSID;// Поле SID (sample identifier (alphanumeric))
	const char *pszPID;// Поле PID (patient identifier (alphanumeric))
	int nCHN;// Поле CHN (number of histogram channels (256))
	int nData[256]; // Данные гистограммы
};

// Флаги, показывающие присутствует ли соответствующая гистограмма
enum AbacusHistoFlags
{
	ABACUS_RBC = 1, // Диаграмма RBC
	ABACUS_WBC = 2, // Диаграмма WBC
	ABACUS_PLT = 4, // Диаграмма PLT
};

// Информация о конкретном анализе
struct ABACUSRECORD
{
	ABACUSDATA Data; // Содержание пакета DATA
	DWORD nHistoFlags; // Флаги из AbacusHistoFlags
	ABACUSHISTOGRAM Histo[3];// Содержание пакетов RBC, WBC, PLT
};

// Установить соединение. Возвращает true если соединение установлено
extern "C" ABACUSCTRL_API bool AbacusConnect();
typedef ABACUSCTRL_API bool (*ABACUSCONNECT)();
// Разорвать соединение и освободить COM-порт
extern "C" ABACUSCTRL_API void AbacusDisconnect();
typedef ABACUSCTRL_API void (*ABACUSDISCONNECT)();
// Состояние связи. Возвращает одно из значений ABACUSSTATE.
extern "C" ABACUSCTRL_API int  AbacusGetState();
typedef ABACUSCTRL_API int  (*ABACUSGETSTATE)();
// Получить информацию из пакета INIT. Возвращает false если соединение не установлено
// или пакет INIT не приходил. 
// Параметр ppAbacusInit указывает на переменную типа ABACUSINIT *, куда
// заносится указатель на структуру с данными из пакета INIT. Эта структура
// только для чтения, ее нельзя модифицировать.
extern "C" ABACUSCTRL_API bool AbacusGetInit(ABACUSINIT **ppAbacusInit);
typedef ABACUSCTRL_API bool (*ABACUSGETINIT)(ABACUSINIT **ppAbacusInit);
// Получить позицию первой записи с информацией об анализе из внутреннего
// буфера. Возвращает false, если в буфере нет ни одной такой записи.
extern "C" ABACUSCTRL_API bool AbacusGetFirstRecordPosition(long *plPos);
typedef ABACUSCTRL_API bool (*ABACUSGETFIRSTRECORDPOSITION)(long *plPos);
// Получить позицию следующей записи с информацией об анализе из внутреннего
// буфера, а по текущей позиции - информацию об анализе.
// Параметр plPos содержит указатель на позицию записи, по которой надо получить
// информацию. На выходе в него заносится позиция следующей записи.
// Параметр ppRecords указывает на переменную типа ABACUSRECORD *, куда
// заносится указатель на структуру с данными об анализе. Эта структура
// только для чтения, ее нельзя модифицировать.
// Всегда возвращается true.
extern "C" ABACUSCTRL_API bool AbacusGetNextRecord(long *plPos, ABACUSRECORD **ppRecords);
typedef ABACUSCTRL_API bool (*ABACUSGETNEXTRECORD)(long *plPos, ABACUSRECORD **ppRecords);
// Удалить запись по позиции.
extern "C" ABACUSCTRL_API void AbacusRemoveRecord(long lPos);
typedef ABACUSCTRL_API void (*ABACUSREMOVERECORD)(long lPos);
// Получить текущие настройки для работы AbacusCtrl.dll
extern "C" ABACUSCTRL_API void AbacusGetSetup(ABACUSCONTROLSETUP *pSetup);
typedef ABACUSCTRL_API void (*ABACUSGETSETUP)(ABACUSCONTROLSETUP *pSetup);
// Установить настройки для работы AbacusCtrl.dll.
extern "C" ABACUSCTRL_API void AbacusSetSetup(ABACUSCONTROLSETUP *pSetup);
typedef ABACUSCTRL_API void (*ABACUSSETSETUP)(ABACUSCONTROLSETUP *pSetup);
// Получить текущие настройки COM-порта
extern "C" ABACUSCTRL_API void AbacusGetCommSetup(COMMSETUP *pCommSetup);
typedef ABACUSCTRL_API void (*ABACUSGETCOMMSETUP)(COMMSETUP *pCommSetup);
// Установить настройки COM-порта
extern "C" ABACUSCTRL_API void AbacusSetCommSetup(COMMSETUP *pCommSetup);
typedef ABACUSCTRL_API void (*ABACUSSETCOMMSETUP)(COMMSETUP *pCommSetup);
// Получить число пакетов, принятых с абакуса за время работы после установки соединения.
extern "C" ABACUSCTRL_API int AbacusGetLastRecordNum();
typedef ABACUSCTRL_API int (*ABACUSGETLASTRECORDNUM)();
