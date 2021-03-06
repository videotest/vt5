/*
 *   (C) ACTIVE Joint Stock company.
 * Defines for use Guardant Stealth
 *         Full list of possible errors
 */

#ifndef __NVSKERR__H_
#define __NVSKERR__H_

const char *nv_pszErrors[nse_LastError+1] =

#ifdef  RUS
/*- Guardant Stealth Errors ( Russian ) --------------------------*/
{
 "��� �訡��",                                           /* 00 */
 "���� �� ������",                                       /* 01 */
 "���� ����, �� �� � ⥬ �����",                         /* 02 */
 "������ ᫨誮� ����让 ����",                         /* 03 */
 "���稪 ���� � �������� ���p�樨 ᫨誮� �����",       /* 04 */
 "GP ���⨣ 0",                                          /* 05 */
 "����p��� �������",                                     /* 06 */
 "����-��� �p� �����",                                  /* 07 */
 "�訡�� ���䨪�樨",                                   /* 08 */
 "��⥢�� �p�⮪�� �� ������",                           /* 09 */
 "��⥢�� p���p� �p��p���� ���p���",                    /* 10 */
 "���������� � ����-�p��p�� ���pﭮ",                  /* 11 */
 "��� �� ������ ����-�p��p�",                           /* 12 */
 "����-�p��p� �� 墠⨫� �����",                       /* 13 */
 "�������� �訡�� DPMI-�p��p�",                         /* 14 */
 "����p����� �訡��",                                    /* 15 */
 "����-�p��p �� ��p����p㦥�",                         /* 16 */
 "������� �� �����p�������� � �⨬ ���箬",              /* 17 */
 "NT �p����p �� ������",                                 /* 18 */
 "NetBIOS ���� ��� �訡��",                            /* 19 */
 "����祭 ����� �������⨬��� �ଠ�",                  /* 20 */
 "���砫� ����室��� �믮����� Login",                   /* 21 */
 "���砫� ����室��� �믮����� Logout",                  /* 22 */
 "��� ���� ᥩ�� �����",                               /* 23 */
 "�ࠩ��� �� ����� ������ LPT ����",                     /* 24 */
 "",                                                     /* 25 */
 "",                                                     /* 26 */
 "",                                                     /* 27 */
 "",                                                     /* 28 */
 "",                                                     /* 29 */
 "�訡�� CRC",                                           /* 30 */
 "�訡�� CRC �� �⥭��",                                /* 31 */
 "�訡�� CRC �� �����",                                /* 32 */
 "����⪠ ��� �� �࠭��� �����",                      /* 33 */
 "�� ������� ����p�⬠ � ����襭�� ����஬",           /* 34 */
 "�訡�� CRC �� �믮������ ����p�⬠",                  /* 35 */
 "�訡�� CRC �� �믮������ ChkNSK ��� ��� ����� ���祩", /* 36 */
 "������ NSK ���p�� ��� ������� ����",                 /* 37 */
 "����� �訡��� ⨯ �������-��p�⭮�� �p���p��������"  /* 38 */
};

#else           /* RUS  */

/*- Guardant Stealth Errors ( England ) --------------------------*/
{
 "All OK",                                               /* 00 */
 "Dongle not found",                                     /* 01 */
 "Code not found",                                       /* 02 */
 "Specifed address too big",                             /* 03 */
 "Byte counter too big",                                 /* 04 */
 "GP equ 0",                                             /* 05 */
 "Bad command",                                          /* 06 */
 "Write time out error",                                 /* 07 */
 "Verifycation error",                                   /* 08 */
 "LAN protocol not found",                               /* 09 */
 "Session limit",                                        /* 10 */
 "Dongle server connection loosed",                      /* 11 */
 "Dongle server not found",                              /* 12 */
 "Dongle server memory allocation error",                /* 13 */
 "DPMI error",                                           /* 14 */
 "Internal LAN error",                                   /* 15 */
 "Server reloaded",                                      /* 16 */
 "Command not support by this key",                      /* 17 */
 "Windows NT driver must be loaded",                     /* 18 */
 "NetBIOS return error code",                            /* 19 */
 "LAN packet format not support",                        /* 20 */
 "At first need execute Login",                          /* 21 */
 "At first need execute Logout",                         /* 22 */
 "This dongle is Busy",                                  /* 23 */
 "Driver can't allocate LPT port",                       /* 24 */
 "",                                                     /* 25 */
 "",                                                     /* 26 */
 "",                                                     /* 27 */
 "",                                                     /* 28 */
 "",                                                     /* 29 */
 "CRC erorr",                                            /* 30 */
 "Read data CRC error",                                  /* 31 */
 "Write data CRC error",                                 /* 32 */
 "Across memory boundary",                               /* 33 */
 "Bad Algoritm number",                                  /* 34 */
 "Algoritm CRC error",                                   /* 35 */
 "CRC error in ChkNSK or can't find more dongles",       /* 36 */
 "Client NSK too old",                                   /* 37 */
 "Wrong type of conversion for EnCode/DeCode"            /* 38 */
};

#endif  /* RUS  */

/*-------------------------------------------------------------*/

#endif  /* __NVSKERR__H_        */
