#pragma once
#include "AbacusI.h"


bool _AbacusConnect(ABACUSCONTROLSETUP *pAbacusSetup, COMMSETUP *pCommSetup);
void _AbacusDisconnect();
int  _AbacusGetState();
bool _AbacusGetInit(ABACUSINIT **ppAbacusInit);
bool _AbacusGetFirstRecordPosition(long *plPos);
bool _AbacusGetNextRecord(long *plPos, ABACUSRECORD **ppRecords);
void _AbacusRemoveRecord(long lPos);
bool _AbacusGetSetup(ABACUSCONTROLSETUP *pSetup);
void _AbacusSetSetup(ABACUSCONTROLSETUP *pSetup);
bool _AbacusGetCommSetup(COMMSETUP *pCommSetup);
void _AbacusSetCommSetup(COMMSETUP *pCommSetup);
int _AbacusGetLastRecordNum();
