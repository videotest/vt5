#pragma once
#include "AbacusI.h"

// ����������� ������� ������ ������� �� AbacusCtrl.dll. AbacusCtrl.dll ����������� �����������.

// ��������� AbacusCtrl.dll, �������� � ��� AbacusSetSetup � ���������� pAbacusSetup
// � AbacusSetCommSetup � ���������� pCommSetup, �������� AbacusConnect ��� ���������
// ����������.
bool _AbacusConnect(ABACUSCONTROLSETUP *pAbacusSetup, COMMSETUP *pCommSetup);
// ��������� ���������� � ��������� AbacusCtrl.dll.
void _AbacusDisconnect();
// �������� AbacusGetState (��. AbacusI.h)
int  _AbacusGetState();
// �������� AbacusGetInit (��. AbacusI.h)
bool _AbacusGetInit(ABACUSINIT **ppAbacusInit);
// �������� AbacusGetFirstRecordPosition (��. AbacusI.h)
bool _AbacusGetFirstRecordPosition(long *plPos);
// �������� AbacusGetNextRecord (��. AbacusI.h)
bool _AbacusGetNextRecord(long *plPos, ABACUSRECORD **ppRecords);
// �������� AbacusRemoveRecord (��. AbacusI.h)
void _AbacusRemoveRecord(long lPos);
// �������� AbacusGetSetup (��. AbacusI.h)
bool _AbacusGetSetup(ABACUSCONTROLSETUP *pSetup);
// �������� AbacusSetSetup (��. AbacusI.h)
void _AbacusSetSetup(ABACUSCONTROLSETUP *pSetup);
// �������� AbacusGetCommSetup (��. AbacusI.h)
bool _AbacusGetCommSetup(COMMSETUP *pCommSetup);
// �������� AbacusSetCommSetup (��. AbacusI.h)
void _AbacusSetCommSetup(COMMSETUP *pCommSetup);
// �������� AbacusGetLastRecordNum (��. AbacusI.h)
int _AbacusGetLastRecordNum();
