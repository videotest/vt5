#include "stdafx.h"
//#include "DllInfo.h"
#include <conio.h>
//#include "fgdrv.h"
#include "resource.h"
#include "mmsystem.h"
#include "SetupDialog.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#pragma warning( disable:4018 )
extern int iFormatN;
//extern FGFormatDescr *pFD;
extern int iShowFrame;
extern int iL,iR,iT,iB, iChange;
extern CRectTracker *crtFrame;
extern void SetRect();

int iLEVEL0 = 25;
int iLimitProcMIN, iLimitProcMAX;

float	fProcMAX, fProcMIN;
int	imax=0, imin=255;
float A=0;

CCriticalSection	csMinMaxAccess;

/*
DAC
0 - zero
1 - ampl
2 -
3 -	\
4 -	 > shutter
5 -	/
6 -
7 -	super
*/



//const's for grapper proc's
char I2C=0;
int iDelay = 1, 
	iI2C0_zero = 0,
	iI2C1_ampl = 0, 
	iAuto=0, 
	iStep = 28,
	iSuper = 0,
	iGamma = 0,
	iShowGammaWindow=0,
	iStartAutoH=0;
int 
	iSkip = 0, 
	iSkip2 = 0, 
	iFirstFrame = 1, 
	iShutter = 0, 
	iSubAdr  = 4,
	iAddr=0x300,
	iAddr1=	iAddr+1,
	iAddr2=	iAddr+2,
	iAddr4=	iAddr+4,
	iAddr5=	iAddr+5,
	iAddr6=	iAddr+6,
	iAddr7=	iAddr+7;
int ulNewCol = 740, 
	ulNewColSrc = 740, 
	ulNewRow = 286, 
	ulFullRow = 0;
float fCoef=1;
//some utils

BOOL ExecProcess( LPTSTR lpszCommandLine );

inline void delay( DWORD pause )
{
	DWORD dw1 = ::timeGetTime(), dw2; 
	int	i = 0;
	while( true )
	{
		dw2 = ::timeGetTime(); 
		if( dw2-dw1 >= pause )
			break;
		i++;
	}
}


void Ram_Jump (WORD w )
{
	byte b;
	_outp(iAddr5,(BYTE)w);
	_outp(iAddr6,(BYTE)w>>4);
	_outp(iAddr5,(BYTE)w>>8);
	b = _inp(iAddr2);       	
}
  //___________________ Wait low I2C clock _____________________________
inline void W_Lck()
{
  I2C=(I2C & 3);      
  _outp(iAddr,I2C); 
  delay(1);
}
inline void W_Hck()
{           
  I2C=(I2C & 3) | 4;
  _outp(iAddr,I2C);
  delay(1);
}
inline void Set_I2C_data()
{
  I2C=(I2C & 4) | 2;
  _outp(iAddr,I2C);
  delay(1);
}
inline void CLR_I2C_data()
{
  I2C=(I2C & 4) ;
  _outp(iAddr,I2C);
  delay(1);
}

void DAC_Write(char  DAC_num, char  data )
{
	char bit,adr,subadr,instr,dat;
	
	subadr=iSubAdr;     // <------- TDA 8444 A[2..0] input state 
	dat=data;
	instr=(0xF0 ^ (DAC_num & 7));  // not incrementing mode 
	adr=((subadr & 7) << 1) ^ 0x40;      // <--- Full address set 
//_asm cli ;
	
//	for (int t=0;t<1;t++)
//	{
		Set_I2C_data(); W_Hck(); CLR_I2C_data(); // <--- Start  
		
		for (bit=0; bit<8;bit++)                // <--- Address & write comm& 
		{
			W_Lck();
			if (((adr << bit) & 0x80)==0x80)
				Set_I2C_data();
			else 
				CLR_I2C_data();
			W_Hck();
		}
		W_Lck(); Set_I2C_data();  W_Hck();       // <--- ACK phase   
		
		for (bit=0; bit<8;bit++)                // <--- Instruction 
		{
			W_Lck();
			if (((instr << bit) & 0x80)==0x80)
				Set_I2C_data(); 
			else 
				CLR_I2C_data();
		 	W_Hck();
		}
		W_Lck(); Set_I2C_data(); W_Hck();       // <--- ACK phase   
		
		for (bit=0; bit<8;bit++)                // <--- Data 
		{
			W_Lck();
			if (((dat << bit) & 0x80)==0x80)
				Set_I2C_data(); 
			else 
				CLR_I2C_data();
			W_Hck();
		}
		W_Lck(); Set_I2C_data(); W_Hck();       // <--- ACK phase   
		W_Lck(); CLR_I2C_data(); W_Hck(); Set_I2C_data();       // <--- STOP phase   
		//Sleep( 10 );
//	}
//_asm sti ;          		    
}
static void Write_Shutter()
{
	switch (iShutter)
	{
		case 0:
			DAC_Write(3, 63 );
			DAC_Write(4, 63 );
			DAC_Write(5, 63 );
			break;   			
		case 1:
			DAC_Write(3, 0 );
			DAC_Write(4, 63 );
			DAC_Write(5, 63 );
			break;   			
		case 2:
			DAC_Write(3, 63 );
			DAC_Write(4, 0 );
			DAC_Write(5, 63 );
			break;   			
		case 3:
			DAC_Write(3, 0 );
			DAC_Write(4, 0 );
			DAC_Write(5, 63);
			break;   			
		case 4:
			DAC_Write(3, 63 );
			DAC_Write(4, 63 );
			DAC_Write(5, 0 );
			break;   			
		case 5:
			DAC_Write(3, 0 );
			DAC_Write(4, 63 );
			DAC_Write(5, 0 );
			break;   			
		case 6:
			DAC_Write(3, 63);
			DAC_Write(4, 0);
			DAC_Write(5, 0);
			break;   			
		case 7:
			DAC_Write(3, 0 );
			DAC_Write(4, 0 );
			DAC_Write(5, 0 );
			break;  					
	}
}
bool inline TestStop( HANDLE h )/*{return false;}*/
{
	if( !h )
		return false;
	if( ::WaitForSingleObject( h, 0 ) == WAIT_TIMEOUT )
		return true;
	return false;
}

bool bInit;

CCriticalSection cs;

void Jump(int N)
{
	int iFastJmp, iTuneJmp;
	iFastJmp = 	(int)(N/16);
	iTuneJmp = N - iFastJmp*16 ;
	Ram_Jump(iFastJmp*16);
	for (int i=0;i<iTuneJmp ; i++) _inpw(iAddr4);
}
void GrabImage( BITMAPINFOHEADER *pbi, HANDLE hStop = 0, int iFast=FALSE )
{ 
	CSingleLock lock( &cs , true);	
	int	imax1, imin1;
	unsigned long N=0,S=0, NMAX=0, NMIN=0;
	bInit = false;
	static int iCurrentI2C0_zero = -1, iCurrentShutter= -1, iCurrentSuper= -1, iCurrentGamma = -1,
	iCurrentI2C1_ampl = -1;

	BOOL bNeedDacWrite1 = iCurrentI2C0_zero !=iI2C0_zero;
	BOOL bNeedDacWrite2 = iCurrentI2C1_ampl !=iI2C1_ampl;

	if (iCurrentShutter != iShutter)
	{
		iCurrentShutter = iShutter;
		Write_Shutter();
	}
	iCurrentI2C0_zero = iI2C0_zero;
	iCurrentI2C1_ampl = iI2C1_ampl;

	if( bNeedDacWrite1 )
		DAC_Write(0, iCurrentI2C0_zero );

	if( bNeedDacWrite2 )
		DAC_Write(1, 63-iCurrentI2C1_ampl );

	if (iCurrentSuper != iSuper)
	{
		iCurrentSuper = iSuper;
		if (iSuper) DAC_Write(7, 0 );
		else		DAC_Write(7, 63 );
		iCurrentI2C1_ampl =-1;
	}				

	if (iCurrentGamma != iGamma )
	{
		iCurrentGamma = iGamma;
		if (iGamma) 
			DAC_Write(6, 63 );
		else		
			DAC_Write(6, 0 );
		delay(10);
		iCurrentI2C1_ampl =-1;
	}

	unsigned long i, j;
	WORD		*p;
	BYTE		*pDIB;
	BYTE		bb;
	WORD		ww,	wCol;               
	DWORD DIBsize;
	SIZE sizeNewDib;
	LPBITMAPINFOHEADER lp;

	
	ulNewCol = ulNewColSrc;
	wCol = (WORD)ulNewCol/2;
    sizeNewDib.cx = (int)ulNewCol;
    ulFullRow = sizeNewDib.cy = (int)ulNewRow*2;    

    DIBsize = (DWORD)sizeNewDib.cx*sizeNewDib.cy+sizeof(BITMAPINFOHEADER)+2048;
    lp = (LPBITMAPINFOHEADER)pbi;
    lp->biHeight = sizeNewDib.cy;
    lp->biWidth = sizeNewDib.cx;
    lp->biBitCount = 8;
    lp->biPlanes = 1;
    lp->biSize = sizeof(BITMAPINFOHEADER);
    lp->biCompression = BI_RGB;
    lp->biSizeImage = 0;
    lp->biClrUsed = 256;

	pDIB = (BYTE *)lp + sizeof(BITMAPINFOHEADER);
	for (i = 0; i < 256; i++)
		for (j = 0; j < 3; j++)
			pDIB[i*4+j] = (int)i;

	

						// ввод изображения
    pDIB = (BYTE *)lp + sizeof(BITMAPINFOHEADER) + 1024L;

	if( TestStop( hStop ) )return;

	//инициализация
	
	_outp(iAddr,2);
	_inp(iAddr1);       	
	_outp(iAddr,2);

	if (iDelay>1)
		for (i=0;i<(unsigned)iDelay;i++)
			_outp(iAddr,1); 
	  
	_outp(iAddr1, iDelay>1?1:0);

	while ( (_inp(iAddr)&2) != 2);
			
	_inp(iAddr1); 	
	
	//начало
	WORD	w;
	WORD	*p1;						
	imax1 = 0;
	imin1 = 255;
	int iXFame;

	if (!iShowFrame || iChange)
	{
		iL = iT = 0;
		iR = ulNewCol ;
		iB = ulFullRow;
	}
	//	1ый полукад
	iXFame = (iR - iL)/2;
	int iYFrame = iB-iT;

	for (i=0;i<iT;i++) Jump(wCol+71);//
	for (i=iFirstFrame; i<iYFrame; i+=2)
	{                                               
//		if (!iShowFrame)
//			for ( j=0; j<iSkipLeft/2; j++,_inpw(iAddr4));
		Jump(iL/2);
		p = (WORD*)(pDIB + (ulFullRow - iT - 1L-i) * (long)ulNewCol)+iL/2;
		for ( j=0; j<iXFame; j++ )
		{
			w = _inpw(iAddr4);
			*(p++)= w;
			bInit = true;
			
			bb= (BYTE)w;
			S += bb;
			N++;
			if (bb>=250) NMAX++;	//
			if (bb<=iLEVEL0) NMIN++;
			imin1 = min ( imin1, bb );
			imax1 = max ( imax1, bb );				
		}
		Jump(wCol-iR/2);
		if( TestStop( hStop ) )return;
	}		 
	
	if (iChange) 
		SetRect();
	// 2ой полукад
	switch (iFast)
	{
	case true:
		for (i=!iFirstFrame?1:0; i<ulFullRow; i+=2)		//			заполнение 2 полукадра 1ым
		{                                               
			p1 = (WORD*)(pDIB + (ulFullRow-1L-i-1) * (long)ulNewCol);
			p = (WORD*)(pDIB + (ulFullRow-1L-i) * (long)ulNewCol);
			for ( j=0; j<wCol; j++ )
				*(p++)= *(p1++);
		}									   
		break;
	case false:
		for (i=0; i<iSkip2; i++) ww = _inpw(iAddr4);			    
		for (i=!iFirstFrame?1:0; i<ulFullRow; i+=2)
		{                                               
			p = (WORD*)(pDIB + (ulFullRow-1L-i) * (long)ulNewCol);
			//for ( j=0; j<iSkipLeft/2; j++,_inpw(iAddr4));
			for ( j=0; j<wCol; j++ )
			{
				w = _inpw(iAddr4);
				*(p++)= w;
				bInit = true;
			}
			if( TestStop( hStop ) )return;
		}
	}

	//_outp(iAddr,2);
	    
    fProcMAX = (float)NMAX / N * 100;
    fProcMIN = (float)NMIN / N * 100;

	csMinMaxAccess.Lock();
	A = (float)((float)S / N);
	imin = imin1;
	imax = imax1;
	csMinMaxAccess.Unlock();
}

					 
static void DoConvert(BYTE *pDst, BYTE *pSrc, int cx, int cy, int cxNew, double d)
{
	int x,y,x1,x2;
	double xd,d1,d2;
	memset(pDst, 0, cxNew*cy);
	for (y = 0; y < cy; y++)
	{
		for (x = 0; x < cxNew; x++)
		{
			xd = x/d;
			x1 = (int)floor(xd);
			x2 = (int)ceil(xd);
			if (x1 >= cx || x2 >= cx)
				pDst[x] = pSrc[cx-1];
			else
			{
				d1 = 1.+x1-xd;
				d2 = 1.-d1;
				pDst[x] = (BYTE)(d1*pSrc[x1]+d2*pSrc[x2]);
			}
		}
		pDst += cxNew;
		pSrc += cx;
	}
}

/*
std_dll void GrabImageDCFG( CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo )
{

	int nNewX,nNewX4,nX4;
	BYTE *pbSrc, *pbDst;
	int i;
	sscanf (pInvokeStruct->m_pszTextParams,"%d",&i);
	if (i==0)
	{	
		CSetupDialog Dlg;	
		i = Dlg.DoModal();
		if (i==IDCANCEL) return;
	}
	
	//	first element of pArguments is CDIBArgument containing mask.
	CDIBArgument *pOutImage = (CDIBArgument *)pResult;
	pOutImage->m_fsOpts = CDIBArgument::Temp;
	LPBITMAPINFOHEADER	lpbi = 0;
	LPBITMAPINFOHEADER	lpbi1 = 0;

	int   sizex = pFD[iFormatN].cx, sizey = pFD[iFormatN].cy, 
		sizeheader = 40;

	lpbi = (LPBITMAPINFOHEADER)malloc( sizex*sizey+sizeheader+2000 );
	if( !lpbi )
	{
		::MessageBox( NULL, "Memory Low!", NULL, MB_SYSTEMMODAL|MB_OK|MB_ICONSTOP ); 
		return;
	}
	pOutImage->m_lpbi = lpbi;

	GrabImage( lpbi );

	if (fCoef != 1)
	{
		nX4 = (((lpbi->biWidth+3)>>2)<<2);
		nNewX = (int)ceil(lpbi->biWidth*fCoef);
		nNewX4 = (((nNewX+3)>>2)<<2);
		DWORD dwSize = nNewX4*lpbi->biHeight+2048+sizeof(BITMAPINFOHEADER);
		lpbi1 = (LPBITMAPINFOHEADER)malloc( dwSize);
		if( !lpbi1 )
		{
			::MessageBox( NULL, "Memory Low!", NULL, MB_SYSTEMMODAL|MB_OK|MB_ICONSTOP ); 
			return;
		}
		memcpy(lpbi1, lpbi, 1024+sizeof(BITMAPINFOHEADER));
		lpbi1->biWidth = nNewX;
		pbSrc = ((BYTE *)(lpbi+1))+1024;
		pbDst = ((BYTE *)(lpbi1+1))+1024;
		DoConvert(pbDst, pbSrc,nX4,lpbi->biHeight,nNewX4,fCoef);
		GlobalUnlock(lpbi);
		GlobalFree(lpbi);
		pOutImage->m_lpbi = lpbi1;
	}

	
	// call shell function
	pInvokeStruct->NotifyFunc( 0x80000000, pInvokeStruct->NotifyLParam );

}
*/

