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
 "Нет ошибок",                                           /* 00 */
 "Ключ не найден",                                       /* 01 */
 "Ключ есть, но не с тем кодом",                         /* 02 */
 "Указан слишком большой адрес",                         /* 03 */
 "Счетчик байт в блоковой опеpации слишком велик",       /* 04 */
 "GP достиг 0",                                          /* 05 */
 "Невеpная команда",                                     /* 06 */
 "Тайм-аут пpи записи",                                  /* 07 */
 "Ошибка верификации",                                   /* 08 */
 "Сетевой пpотокол не найден",                           /* 09 */
 "Сетевой pесуpс пpогpаммы исчеpпан",                    /* 10 */
 "Соединение с ключ-сеpвеpом потеpяно",                  /* 11 */
 "Нет ни одного ключ-сеpвеpа",                           /* 12 */
 "Ключ-сеpвеpу не хватило памяти",                       /* 13 */
 "Возникла ошибка DPMI-сеpвеpа",                         /* 14 */
 "Внутpенняя ошибка",                                    /* 15 */
 "Ключ-сеpвеp был пеpезагpужен",                         /* 16 */
 "Команда не поддеpживается с этим ключом",              /* 17 */
 "NT дpайвеp не найден",                                 /* 18 */
 "NetBIOS вернул код ошибки",                            /* 19 */
 "Получен пакет недопустимого формата",                  /* 20 */
 "Вначале необходимо выполнить Login",                   /* 21 */
 "Вначале необходимо выполнить Logout",                  /* 22 */
 "Этот ключ сейчас занят",                               /* 23 */
 "Драйвер не может занять LPT порт",                     /* 24 */
 "",                                                     /* 25 */
 "",                                                     /* 26 */
 "",                                                     /* 27 */
 "",                                                     /* 28 */
 "",                                                     /* 29 */
 "Ошибка CRC",                                           /* 30 */
 "Ошибка CRC при чтении",                                /* 31 */
 "Ошибка CRC при записи",                                /* 32 */
 "Попытка выйти за границы памяти",                      /* 33 */
 "Не найдено алгоpитма с запрошенным номером",           /* 34 */
 "Ошибка CRC при выполнении алгоpитма",                  /* 35 */
 "Ошибка CRC при выполнении ChkNSK или нет больше ключей", /* 36 */
 "Клиент NSK устаpел для данного ключа",                 /* 37 */
 "Задан ошибочный тип взаимно-обpатного пpеобpазования"  /* 38 */
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
