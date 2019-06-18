/***************************************************************************
 *      (C) ACTIVE Joint Stock company.                                    *
 * Defines for use Guardant Stealth                                        *
 *                                      2000  ACTIVE                       *
 ***************************************************************************/

#ifndef __NSKEY32__H_
#define __NSKEY32__H_

/* Common types */
#ifndef BYTE
 typedef unsigned char  BYTE;
#endif

#ifndef WORD
 typedef unsigned short WORD;
#endif

#ifndef DWORD
 typedef unsigned long  DWORD;
#endif

#ifdef  WINAPI
 #define NVSK_API        _cdecl
#else
 #define NVSK_API        _cdecl
#endif


/*- Demo codes ----------------------------------------------------------*/
#define ns_DEMONVK 0x519175b7Lu  /* Demo public code                     */
#define ns_DEMORDO 0x51917645Lu  /* Demo private read code               */
#define ns_DEMOPRF 0x51917603Lu  /* Demo private write code              */
#define ns_DEMOMST 0x5191758cLu  /* Demo private master code             */

/*- Guardant Stealth dongle Models --------------------------------------*/
#define nskm_GS          0  /* Guardant Stealth                          */
#define nskm_GU          1  /* Guardant USB                              */
#define nskm_GF          2  /* Guardant Fidus                            */
#define nskm_G2          3  /* Guardant StealthII                        */

/*- Guardant Stealth Types ----------------------------------------------*/
#define nskt_DOS         0x0000  /* DOS Stealth Dongle                   */
#define nskt_Win         0x0000  /* Windows Stealth Dongle               */
#define nskt_LAN         0x0001  /* LAN Stealth Dongle                   */
#define nskt_Time        0x0002  /* Time Stealth Dongle                  */

/*- Guardant Stealth Flags for nskSetMode() -----------------------------*/
/* Input: search what ?                                                  */
#define nsf_NProg        0x0001  /* wKeyNProg == wNProg                  */
#define nsf_ID           0x0002  /* dwKeyID == dwKeyID                   */
#define nsf_SN           0x0004  /* Serial Number                        */
#define nsf_Ver          0x0008  /* bKeyVersion >= bVer                  */
#define nsf_Mask         0x0010  /* wKeyMask & wMask == wMask            */
#define nsf_Type         0x0020  /* wKeyType & wType == wType            */
/* Input: control working mode                                           */
#define nsf_SysAddrMode  0x0080  /* Read & Write in System Address Mode  */
#define nsf_CodeIsString 0x0100  /* Reserved                             */
#define nsf_NoRetry      0x0200  /* Disable auto adjust exchange         */
#define nsf_NoFullAccess 0x0400  /* Disable catch LPT                    */
#define nsf_OnlyStdLPT1  0x0800  /* Search only in 378H                  */
#define nsf_OnlyStdLPT2  0x1000  /* Search only in 278H                  */
#define nsf_OnlyStdLPT3  0x2000  /* Search only in 3BCH                  */
#define nsf_NoAutoMem32  0x4000  /* Explicit __FLAT__ data selector      */
#define nsf_UseOldCRC    0x8000  /* Reserved                             */

/*- Address of any field in key ( User Address Mode )--------------------*/
#define uam_bNProg           (30-30) /* 00h Programm number              */
#define uam_bVer             (31-30) /* 01h Version                      */
#define uam_wSN              (32-30) /* 02h Serial number                */
#define uam_wMask            (34-30) /* 04h Mask of bit                  */
#define uam_wGP              (36-30) /* 06h GP                           */
#define uam_wRealLANRes      (38-30) /* 08h Real Lan Resource            */
#define uam_dwIndex          (40-30) /* 0Ah Index                        */
#define uam_abAlgoAddr       (44-30) /* 0Eh Array of bytes. Each         */
                                     /* point to algoritm descriptor     */

/* Address of all field in key ( System Address Mode ) ------------------*/
/* Protect against command nsc_Init, nsc_Protect & nsc_Write             */
/*    * - Partial protect : nsc_Protect                                  */
/*        can do only after nsc_Init                                     */
/*    X - Full protect                                                   */
/* Protect against command:           Init Protect Write                 */
#define sam_bKeyModelAddr    0 /*  0h   X     X     X    0=GS,1=GU,2=GF  */
#define sam_bKeyMemSizeAddr  1 /*  1h   X     X     X    0=0, 8=256      */
#define sam_bKeyProgVerAddr  2 /*  2h   X     X     X                    */
#define sam_bKeyProtocolAddr 3 /*  3h   X     X     X                    */
#define sam_wClientVerAddr   4 /*  4h   X     X     X    0x104=1.4       */
#define sam_bKeyUserAddrAddr 6 /*  6h   X     X     X                    */
#define sam_bKeyAlgoAddrAddr 7 /*  7h   X     X     X                    */
#define sam_wPrnportAddr     8 /*  8h   X     X     X                    */

#define sam_dwPublicCode    14 /*  Eh   X     X     X                    */
#define sam_bVersion        18 /* 12h   X     X     X                    */
#define sam_bLANRes         19 /* 13h   X     X     X                    */
#define sam_wType           20 /* 14h   X     X     X                    */
#define sam_dwID            22 /* 16h   X     X     X                    */
#define sam_bWriteProtect   26 /* 1Ah         *     X                    */
#define sam_bReadProtect    27 /* 1Bh         *     X                    */
#define sam_bNumFunc        28 /* 1Ch         *     X                    */
#define sam_bReserved2      29 /* 1Dh         *     X                    */
#define sam_bNProg          30 /* 1Eh   X     X                          */
#define sam_bVer            31 /* 1Fh   X     X                          */
#define sam_wSN             32 /* 20h   X     X                          */
#define sam_wMask           34 /* 22h   X     X                          */
#define sam_wGP             36 /* 24h   X     X                          */
#define sam_wRealLANRes     38 /* 26h   X     X                          */
#define sam_dwIndex         40 /* 28h   X     X                          */
#define sam_abAlgoAddr      44 /* 2Ch                                    */


/*- Guardant Stealth Algorithm Numbers ----------------------------------*/
#define nsan_AutoProtect   0    /* For AutoProtect program               */
#define nsan_Fast          1    /* for EnCode/DeCode function            */
#define nsan_Random        2    /* Random values                         */
#define nsan_DEMO          3    /* for TRANSFORM function                */

/*- Guardant Stealth Algorithm Request Size -----------------------------*/
#define nsars_AutoProtect  4    /* For AutoProtect program               */
#define nsars_Fast        32    /* for EnCode/DeCode function            */
#define nsars_Random       4    /* Random values                         */
#define nsars_DEMO         4    /* for TRANSFORM function                */

/*- Guardant Stealth Fast EnCode/DeCode Algorythm Types -----------------*/
#define nsat_Algo0         0    /* Source type                           */
#define nsat_AlgoASCII     1    /* For ASCII                             */
#define nsat_AlgoFile      2    /* Source but not block                  */

/*- Guardant Stealth CRC Start value ------------------------------------*/
#define ns_StartCRC            -1l /* First value for 'continued' CRC    */

/*- Guardant Stealth Errors ---------------------------------------------*/
#define nse_Ok                  0 /* All OK                              */
#define nse_KeyNotFound         1 /* Dongle not found                    */
                            /*  2 (not present in Stealth)               */
#define nse_AddressTooBig       3 /* Specifed address too big            */
                            /*  4 (not present in Stealth)               */
#define nse_GPis0               5 /* GP equ 0                            */
#define nse_BadCommand          6 /* Bad command                         */
                            /*  7 (not present in Stealth)               */
#define nse_VerifyError         8 /* Verifycation error                  */
#define nse_LANProtNotFound     9 /* LAN protocol not found              */
#define nse_LANResourceExhaust 10 /* Session limit                       */
#define nse_ConnectionLoosed   11 /* Dongle server connection loosed     */
#define nse_LANplugsNotFound   12 /* Dongle server not found             */
#define nse_LANserverMemory    13 /* Dongle server memory allocation error  */
#define nse_LANDPMIError       14 /* DPMI error                          */
#define nse_Internal           15 /* Internal LAN error                  */
#define nse_ServerReloaded     16 /* Server reloaded                     */
#define nse_VersionTooOld      17 /* Command not support by this key     */
#define nse_BadDriver          18 /* Windows NT driver must by loaded    */
#define nse_LANNetBIOS         19 /* NetBIOS return error code           */
#define nse_LANpacket          20 /* LAN packet format not support       */
#define nse_LANneedLogin       21 /* at first need login                 */
#define nse_LANneedLogout      22 /* at first need logout                */
#define nse_LANKeyBusy         23 /* This Dongle is Busy                 */
#define nse_DriverBusy         24 /* Driver can't allocate LPT port      */
                            /* 25 - 29 (not present in Stealth)          */
#define nse_CRCError           30 /* CRC erorr                           */
#define nse_CRCErrorRead       31 /* Read data CRC error                 */
#define nse_CRCErrorWrite      32 /* Write data CRC error                */
#define nse_Overbound          33 /* Across memory boundary              */
#define nse_AlgoNotFound       34 /* Bad Algoritm number                 */
#define nse_CRCErrorFunc       35 /* Algoritm CRC error                  */
#define nse_CRCChkNSK          36 /* CRC error in ChkNSK                 */
#define nse_ProtocolNotSup     37 /* Client NSK too old                  */
#define nse_CnvTypeError       38 /* Wrong type of conversion for EnCode/DeCode */
#define nse_LastError          39

/*- Function prototypes ---------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*- Set Stealth search mode -----------------------------------*/
void NVSK_API nskSetMode( DWORD dwFlags, BYTE bProg, DWORD dwID,\
        WORD wSN, BYTE bVer, WORD wMask, WORD wType );

/*- Find first specifed key -----------------------------------*/
int NVSK_API nskFindFirst( DWORD dwPublic, DWORD *pdwID );

/*- Find next specifed key ------------------------------------*/
int NVSK_API nskFindNext ( DWORD *pdwID );

/*- Search key with specifed private read code ----------------*/
int NVSK_API nskCheck( DWORD dwPrivateRD );

/*- Search key with specifed private read code & dec it GP ----*/
int NVSK_API nskDecGP( DWORD dwPrivateWR );

/*- Conformation data -----------------------------------------*/
int NVSK_API nskTransform( DWORD dwPrivateRD, BYTE bAlgoNum,\
        BYTE bLng, void *pData );

/*- Read data from Guardant dongle ----------------------------*/
int NVSK_API nskRead( DWORD dwPrivateRD, BYTE bAddr,\
        BYTE bLng, void *pData );

/*- Write data to Guardant dongle -----------------------------*/
int NVSK_API nskWrite( DWORD dwPrivateWR, BYTE bAddr,\
        BYTE bLng, void *pData );

/*- Reset to 0 protected fields & algoritm number -------------*/
int NVSK_API nskInit( DWORD dwPrivateMST );

/*- Initialise Stealth with specifed private master -----------*/
int NVSK_API nskProtect( DWORD dwPrivateMST, BYTE bWrProt,\
        BYTE bRdProt, BYTE bNumFunc );

/*- Calculate CRC ---------------------------------------------*/
DWORD NVSK_API nskCRC( void *pData, DWORD dwLng, DWORD dwPrevCRC );

/*- Init Encrypt ----------------------------------------------*/
int NVSK_API nskCodeInit( DWORD dwPrivateRD, WORD wCnvType,\
        BYTE bAddr, void *pKey );

/*- EnCode ----------------------------------------------------*/
int NVSK_API nskEnCode( WORD wCnvType, void  *pKeyBuf,\
        void *pData, DWORD dwLng );

/*- DeCode ----------------------------------------------------*/
int NVSK_API nskDeCode( WORD wCnvType, void  *pKeyBuf,\
        void *pData, DWORD dwLng );

/*-------------------------------------------------------------*/
/*- LAN functions ---------------------------------------------*/
/*-------------------------------------------------------------*/

/*- Set Stealth search mode -----------------------------------*/
void NVSK_API nnkSetMode( DWORD dwFlags, BYTE bProg, DWORD dwID,\
        WORD wSN, BYTE bVer, WORD wMask, WORD wType );

/*- Search key with specifed private read code ----------------*/
int NVSK_API nnkCheck( DWORD dwPrivateRD );

/*- Search key with specifed private read code & dec it GP ----*/
int NVSK_API nnkDecGP( DWORD dwPrivateWR );

/*- Conformation data -----------------------------------------*/
int NVSK_API nnkTransform( DWORD dwPrivateRD, BYTE bAlgoNum,\
        BYTE bLng, void *pData );

/*- Read data from Guardant dongle ----------------------------*/
int NVSK_API nnkRead( DWORD dwPrivateRD, BYTE bAddr,\
        BYTE bLng, void *pData );

/*- Write data to Guardant dongle -----------------------------*/
int NVSK_API nnkWrite( DWORD dwPrivateWR, BYTE bAddr,\
        BYTE bLng, void *pData );

/*- Reset to 0 protected fields & algoritm number -------------*/
int NVSK_API nnkInit( DWORD dwPrivateMST );

/*- Initialise Stealth with specifed private master -----------*/
int NVSK_API nnkProtect( DWORD dwPrivateMST, BYTE bWrProt,\
        BYTE bRdProt, BYTE bNumFunc );

/*- Calculate CRC ---------------------------------------------*/
DWORD NVSK_API nnkCRC( void *pData, DWORD dwLng, DWORD dwPrevCRC );

/*- Init Encrypt ----------------------------------------------*/
int NVSK_API nnkCodeInit( DWORD dwPrivateRD, WORD wCnvType,\
        BYTE bAddr, void *pKey );

/*- EnCode ----------------------------------------------------*/
int NVSK_API nnkEnCode( WORD wCnvType, void  *pKeyBuf,\
        void *pData, DWORD dwLng );

/*- DeCode ----------------------------------------------------*/
int NVSK_API nnkDeCode( WORD wCnvType, void  *pKeyBuf,\
        void *pData, DWORD dwLng );

/*- Login -----------------------------------------------------*/
int NVSK_API nnkLogin( DWORD dwPrivateRD );

/*- Logout ----------------------------------------------------*/
int NVSK_API nnkLogout( DWORD dwPrivateRD );

/*- Lock Beg --------------------------------------------------*/
int NVSK_API nnkLockBeg( DWORD dwPrivateRD );

/*- Lock End --------------------------------------------------*/
int NVSK_API nnkLockEnd( DWORD dwPrivateRD );

/*-------------------------------------------------------------*/

#ifdef  _NOVEX_USE_DLL_

#define NOVEXLANKEY32(p0,p1,p2,p3,p4,p5) __NOVEXLANKEY32(p0,p1,p2,p3,p4,p5)
#define NOVEXKEY32(p0,p1,p2,p3,p4,p5)    __NOVEXKEY32(p0,p1,p2,p3,p4,p5)
#define NOVEXNET32(p0,p1,p2,p3,p4,p5) __NOVEXNET32(p0,p1,p2,p3,p4,p5)
#define NOVEXSTL32(p0,p1,p2,p3,p4,p5)    __NOVEXSTL32(p0,p1,p2,p3,p4,p5)


/*      Declare access function */
extern short pascal __NOVEXKEY32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );
extern short pascal __NOVEXLANKEY32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );

extern short pascal __NOVEXSTL32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );
extern short pascal __NOVEXNET32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );

#else   /* _NOVEX_USE_DLL_ */

/*      Declare access function */
extern short cdecl NOVEXKEY32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );
extern short cdecl NOVEXLANKEY32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );

extern short cdecl NOVEXSTL32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );
extern short cdecl NOVEXNET32( char *PCODE, short *PCOMMAND,\
        short *PRESULT, short *PRM1, short *PRM2, short *PRM3 );
#endif  /* _NOVEX_USE_DLL_ */

/*-------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __NSKEY32__H_ */
