#if 1 //#ifdef _WIN64
#include "nvskey32.h"

/*- Find next specified key ------------------------------------*/
int nskFindNext ( DWORD *pdwID )
{
	return 0;
}

/*- Find first specified key -----------------------------------*/
int nskFindFirst( DWORD dwPublic, DWORD *pdwID )
{
	return 12;
}

/*- Read data from Guardant dongle ----------------------------*/
int nskRead( DWORD dwPrivateRD, BYTE bAddr,\
        BYTE bLng, void *pData )
{
	return 0;
}

/*- Set Stealth search mode -----------------------------------*/
void nskSetMode( DWORD dwFlags, BYTE bProg, DWORD dwID,\
        WORD wSN, BYTE bVer, WORD wMask, WORD wType )
{
}

/*- Search key with specifed private read code ----------------*/
int nskCheck( DWORD dwPrivateRD )
{
	return 0;
}

/*- Write data to Guardant dongle -----------------------------*/
int nskWrite( DWORD dwPrivateWR, BYTE bAddr,\
        BYTE bLng, void *pData )
{
	return 0;
}
#endif