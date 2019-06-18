#pragma once
#include "AbacusI.h"

// Статическая обвязка вокруг функций из AbacusCtrl.dll. AbacusCtrl.dll загружается динамически.

// Загружает AbacusCtrl.dll, вызывает в ней AbacusSetSetup с параметром pAbacusSetup
// и AbacusSetCommSetup с параметром pCommSetup, вызывает AbacusConnect для установки
// соединения.
bool _AbacusConnect(ABACUSCONTROLSETUP *pAbacusSetup, COMMSETUP *pCommSetup);
// Разрывает соединение и выгружает AbacusCtrl.dll.
void _AbacusDisconnect();
// Вызывает AbacusGetState (см. AbacusI.h)
int  _AbacusGetState();
// Вызывает AbacusGetInit (см. AbacusI.h)
bool _AbacusGetInit(ABACUSINIT **ppAbacusInit);
// Вызывает AbacusGetFirstRecordPosition (см. AbacusI.h)
bool _AbacusGetFirstRecordPosition(long *plPos);
// Вызывает AbacusGetNextRecord (см. AbacusI.h)
bool _AbacusGetNextRecord(long *plPos, ABACUSRECORD **ppRecords);
// Вызывает AbacusRemoveRecord (см. AbacusI.h)
void _AbacusRemoveRecord(long lPos);
// Вызывает AbacusGetSetup (см. AbacusI.h)
bool _AbacusGetSetup(ABACUSCONTROLSETUP *pSetup);
// Вызывает AbacusSetSetup (см. AbacusI.h)
void _AbacusSetSetup(ABACUSCONTROLSETUP *pSetup);
// Вызывает AbacusGetCommSetup (см. AbacusI.h)
bool _AbacusGetCommSetup(COMMSETUP *pCommSetup);
// Вызывает AbacusSetCommSetup (см. AbacusI.h)
void _AbacusSetCommSetup(COMMSETUP *pCommSetup);
// Вызывает AbacusGetLastRecordNum (см. AbacusI.h)
int _AbacusGetLastRecordNum();
