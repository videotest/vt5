#if 1 //_WIN64
#include "nvskey32.h"

/*- Find next specified key ------------------------------------*/
int NVSK_API nskFindNext ( DWORD *pdwID )
{
	return 0;
}

/*- Find first specified key -----------------------------------*/
int NVSK_API nskFindFirst( DWORD dwPublic, DWORD *pdwID )
{
	return 0;
}

/*- Read data from Guardant dongle ----------------------------*/
int NVSK_API nskRead( DWORD dwPrivateRD, BYTE bAddr,\
        BYTE bLng, void *pData )
{
	return 0;
}

/*- Set Stealth search mode -----------------------------------*/
void NVSK_API nskSetMode( DWORD dwFlags, BYTE bProg, DWORD dwID,\
        WORD wSN, BYTE bVer, WORD wMask, WORD wType )
{
}

/*- Search key with specifed private read code ----------------*/
int NVSK_API nskCheck( DWORD dwPrivateRD )
{
	return 0;
}

/*- Write data to Guardant dongle -----------------------------*/
int NVSK_API nskWrite( DWORD dwPrivateWR, BYTE bAddr,\
        BYTE bLng, void *pData )
{
	return 0;
}
#endif