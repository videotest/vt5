#include "StdAfx.h"
#include "DigSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int iIntefaceSimple = false;
extern int iDelay, iI2C0_zero,iI2C1_ampl, iAuto, iStep, iStartAutoH, iSuper,iShowGammaWindow;
extern int iSkip, iSkip2, iFirstFrame, iShutter, iAddr, iSubAdr ;
extern int RPA,WPA,WPB,RMA;
extern float fCoef;
extern int ulNewCol, ulNewColSrc, ulNewRow, ulFullRow;
static char FAR *szSect = NULL;
static char /*FAR *szSect = "DigitFG",*/ FAR *szMode = "Mode", FAR *szZero = "Zero", FAR *szAmpl = "Ampl", 
			FAR *szCol = "Col", FAR *szRow = "Row", FAR *szAddres = "Addres", 
			FAR *szSkip = "Skip1", FAR *szDelay = "Delay" ,FAR *szLimitMin = "LimitMin" ,FAR *szLimitMax = "LimitMax" ,
			FAR *szSkip2 = "Skip", 
			FAR *szSkipLeft = "SkipLeft", FAR *szInterface = "Interface", 
			FAR *szFirstFrame = "FirstFrame", FAR *szShutter = "Shutter", FAR *szSuper = "Super", 
			FAR *szSubAdr = "SubAdr",FAR *szCoef = "Coef",FAR *szShowGammaWindow = "ShowGammaWindow",
			FAR *szSkipRight = "SkipRight" ;
static char FAR szProfile[] = "DigitalCam.ini";

int iBri = 0, iCnt = 0;
BOOL bAuto = 0;
int iExpoTime = 10000, iFormatN = 0;
int iNumForm = 1;
extern int iLimitProcMIN, iLimitProcMAX;

char *szINI = "FgTemp.ini", *szS = "Settings";

int Even( int in )
{
 	in = in/2*2 < in ? in+=1 : in; 		
 	return(in);
}

CVT5ProfileManager g_DigProfile(_T("Digital"), false);

/*
void ReadVideoSettings()
{
	ulNewColSrc = Even(GetPrivateProfileInt( szSect, szCol, 740, szProfile ));
	ulNewRow = GetPrivateProfileInt( szSect, szRow, 286, szProfile );
	iSkip2 = GetPrivateProfileInt( szSect, szSkip2, 143, szProfile ); 
	iSkip = GetPrivateProfileInt( szSect, szSkip, 0, szProfile ); 
	iDelay = GetPrivateProfileInt( szSect, szDelay, 1, szProfile );
	iFirstFrame = GetPrivateProfileInt( szSect, szFirstFrame, 1, szProfile );
	iI2C0_zero = GetPrivateProfileInt( szSect, szZero, 0, szProfile );
	iI2C1_ampl = GetPrivateProfileInt( szSect, szAmpl, 33, szProfile );
	iAddr = GetPrivateProfileInt( szSect, szAddres, 0x300, szProfile );

	iShutter = GetPrivateProfileInt( szSect, szShutter, 1, szProfile );
	iSuper = GetPrivateProfileInt( szSect, szSuper, 0, szProfile );
	iIntefaceSimple = GetPrivateProfileInt( szSect, szInterface, 0, szProfile );  

	iLimitProcMIN = GetPrivateProfileInt( szSect, szLimitMin, 0, szProfile );
	iLimitProcMAX = GetPrivateProfileInt( szSect, szLimitMax, 0, szProfile );	

	iSubAdr = GetPrivateProfileInt( szSect, szSubAdr, 4, szProfile );	
	iShowGammaWindow = GetPrivateProfileInt( szSect, szShowGammaWindow, 0, szProfile );	

	char szBuf[25];
	GetPrivateProfileString( szSect, szCoef, "1.0", szBuf,sizeof(szBuf), szProfile );
	sscanf( szBuf, "%f", &fCoef );
}

void SaveVideoSettings()
{
	char szBuffer[20];

	wsprintf( szBuffer, "%d", iI2C0_zero );
	WritePrivateProfileString( szSect, szZero, szBuffer, szProfile );
	
	wsprintf( szBuffer, "%d", iI2C1_ampl );
	WritePrivateProfileString( szSect, szAmpl, szBuffer, szProfile );

	wsprintf( szBuffer, "%d", iDelay );
	WritePrivateProfileString( szSect, szDelay, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", ulNewColSrc );
	WritePrivateProfileString( szSect, szCol, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", ulNewRow );
	WritePrivateProfileString( szSect, szRow, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iSkip2 );
	WritePrivateProfileString( szSect, szSkip2, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iSkip );
	WritePrivateProfileString( szSect, szSkip, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iFirstFrame );
	WritePrivateProfileString( szSect, szFirstFrame, szBuffer, szProfile );	

	wsprintf( szBuffer, "0x%X", iAddr );
	WritePrivateProfileString( szSect, szAddres, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iLimitProcMIN );
	WritePrivateProfileString( szSect, szLimitMin, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iLimitProcMAX );
	WritePrivateProfileString( szSect, szLimitMax, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iShutter );
	WritePrivateProfileString( szSect, szShutter, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iSuper );
	WritePrivateProfileString( szSect, szSuper, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iIntefaceSimple );
	WritePrivateProfileString( szSect, szInterface, szBuffer, szProfile );	

	sprintf(szBuffer, "%1.2f", fCoef );
	WritePrivateProfileString( szSect, szCoef, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iShowGammaWindow );
	WritePrivateProfileString( szSect, szShowGammaWindow, szBuffer, szProfile );	

	wsprintf( szBuffer, "%d", iSubAdr );
	WritePrivateProfileString( szSect, szSubAdr, szBuffer, szProfile );	
}
*/


void ReadVideoSettings()
{
	ulNewColSrc = Even(g_DigProfile.GetProfileInt(szSect, szCol, 740));
	ulNewRow = g_DigProfile.GetProfileInt(szSect, szRow, 286);
	iSkip2 = g_DigProfile.GetProfileInt(szSect, szSkip2, 143); 
	iSkip = g_DigProfile.GetProfileInt(szSect, szSkip, 0); 
	iDelay = g_DigProfile.GetProfileInt(szSect, szDelay, 1);
	iFirstFrame = g_DigProfile.GetProfileInt(szSect, szFirstFrame, 1);
	iI2C0_zero = g_DigProfile.GetProfileInt(szSect, szZero, 0);
	iI2C1_ampl = g_DigProfile.GetProfileInt(szSect, szAmpl, 33);
	iAddr = g_DigProfile.GetProfileInt(szSect, szAddres, 0x300);

	iShutter = g_DigProfile.GetProfileInt(szSect, szShutter, 1);
	iSuper = g_DigProfile.GetProfileInt(szSect, szSuper, 0);
	iIntefaceSimple = g_DigProfile.GetProfileInt(szSect, szInterface, 0);

	iLimitProcMIN = g_DigProfile.GetProfileInt(szSect, szLimitMin, 0);
	iLimitProcMAX = g_DigProfile.GetProfileInt(szSect, szLimitMax, 0);

	iSubAdr = g_DigProfile.GetProfileInt(szSect, szSubAdr, 4);
	iShowGammaWindow = g_DigProfile.GetProfileInt(szSect, szShowGammaWindow, 0);

	CString s = g_DigProfile.GetProfileString(szSect, szCoef, "1.0");
	sscanf(s, "%f", &fCoef );
}

void SaveVideoSettings()
{
	CString s;
	g_DigProfile.WriteProfileInt(szSect, szZero, iI2C0_zero);
	g_DigProfile.WriteProfileInt(szSect, szAmpl, iI2C1_ampl);
	g_DigProfile.WriteProfileInt(szSect, szDelay, iDelay);
	g_DigProfile.WriteProfileInt(szSect, szCol, ulNewColSrc);
	g_DigProfile.WriteProfileInt(szSect, szRow, ulNewRow);
	g_DigProfile.WriteProfileInt(szSect, szSkip2, iSkip2);
	g_DigProfile.WriteProfileInt(szSect, szSkip, iSkip);
	g_DigProfile.WriteProfileInt(szSect, szFirstFrame, iFirstFrame);
	g_DigProfile.WriteProfileInt(szSect, szAddres, iAddr);
	g_DigProfile.WriteProfileInt(szSect, szLimitMin, iLimitProcMIN);
	g_DigProfile.WriteProfileInt(szSect, szLimitMax, iLimitProcMAX);
	g_DigProfile.WriteProfileInt(szSect, szShutter, iShutter);
	g_DigProfile.WriteProfileInt(szSect, szSuper, iSuper);
	g_DigProfile.WriteProfileInt(szSect, szInterface, iIntefaceSimple);
	s.Format("%1.2f", fCoef);
	g_DigProfile.WriteProfileString(szSect, szCoef, s);
	g_DigProfile.WriteProfileInt(szSect, szShowGammaWindow, iShowGammaWindow);
	g_DigProfile.WriteProfileInt(szSect, szSubAdr, iSubAdr);
}
