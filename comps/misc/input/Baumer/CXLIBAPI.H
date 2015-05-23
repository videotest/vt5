#ifndef _VIDEO_API_
#define _VIDEO_API_

// Microsoft specific defines
#ifndef _WINDEF_
//#include <windef.h>     // Basic Windows Type Definitions
    typedef unsigned long       DWORD;
    typedef int                 BOOL;
    typedef unsigned char       BYTE;
    typedef unsigned short      WORD;
    typedef void                *PVOID;
#endif

// Calling Conventions 
#if defined(__BORLANDC__) || (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
    #ifndef _WINDEF_
        #define WINAPI __stdcall
    #endif
#else
    #define WINAPI
#endif

#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */

#include "camera.h"
#include "7146api.h"


#define MAX_DYNBUF_ANZ	MAX_BUF_ANZ
#define MAX_STABUF_ANZ	128

typedef struct _DLL_SETS_ {

	BOOL VadSearchMode;

} CAMERADLLSETS, * PCAMERADLLSETS;

// Description structure of one virtual memory buffer

typedef struct 
{
    void      *vmem;        // virtuel memory pointer to buffer
    DWORD     vmemLen;      // buffer size in bytes
    DWORD     status;       // information about buffer status, only reading
                            // access
    DWORD     picNr;        // picture number if status is BUF_FILLED, only
                            // reading access 
}   DMEM_BUF, *PDMEM_BUF;

// Description structure of a field of virtual memory buffers
// filled by user
typedef struct _DVMEM_STRUCT
{	int  n;
    DMEM_BUF  buffer[MAX_DYNBUF_ANZ];  // field of buffer descriptors 

} DVMEM, *PDVMEM;

// Description structure of a command field  
// filled by user , only for special camera
/*
typedef struct _PARAM_CMD_ {
	int         imnumber;	// for preloads only
	int			mode;		// direct output = 0 / enable with preload 1
	int         canal;		// number of selected camera
	int			command;	// type of command
	int			param1;
	int			param2;
	int			param3;
	int			param4;
	int			param5;
	int			param6;
} tCMuxCmd, * tpCMuxCmd;
*/

typedef struct _PARAM_NUM_ {
	double		systime;	// systemtime of useage
	int         imnumber;	// wird nur bei preloads benutzt
	int			delay;		// delay time
	int         mode;		// io-mode
	int         canal;		// Kanal
	int			command;	// Kommandotype
	int			param1;
	int			param2;
	int			param3;
	int			param4;
	int			param5;
	int			param6;
} tParameterNum, * tpParameterNum;





/*****************************************************************************
*    Programm:		 fgvideo.dll                                             *   
*	 Aufgebaut auf:	 fgaa7146.dll                                            *
*    Version:        1.0                                                     *
*    Compiler:       Visual C++ 4.2                                          *
*    Betriebssystem: Windows NT 4.0 / Windows 95                             *
*    Zweck:          Deklaration der exportierten API-Funktionen             *
*                    zur Nutzung des Videointerfaces der F+O Framegrabber    *        
*                    Bereitstellung von DEFINES zur allgemeinen Verwendung   *
*    Bemerkung:      das Modul erfordert installierten mapmem.sys Treiber    *
*																			 *	
*					 weiterhin muß die DLL fgaa7146.dll zur Verfügung stehen *
*																			 *	
*    Erstellt am:    22.7.97                                                 *
*    Stand:          31.8.98                                                 *
*    Bearbeiter:     M. Pester                                               *
*    Copyright:      (C) Baumer Optronic GmbH		                         *
******************************************************************************
*                                                                            *   
*    Aenderungen:                                                            *   
*                   SetSaturation( int pm ) - mp 31.9.98                       *       
******************************************************************************/

// #define WINAPI __declspec(dllexport)
// #define WINAPI 	FAR PASCAL EXPORT

/*****************************************************************************
Funktionsname:	 InitCxLib     
Beschreibung:    Grundinitialisierung VideoLib   
Argumente:
	camType		=>	CX05G
					CX05C	 
					CXMTG	  
					CX13G	  
					CX13C

Rückgabewert:        
     > 0       => wenn erfolgreich
     <=0         => wenn Fehler

Bemerkung:          
                    
*****************************************************************************/
int WINAPI InitCxLib( int camType ); 
int WINAPI ReInitCxLib( int camType ); 

/*****************************************************************************
Funktionsname:	 CXL2Monitor    
Beschreibung:    Control Panel für Kamera
				 
Argumente: 
	keine
    
Rückgabewert:        
    int 		 => FALSE /  

Bemerkung:		
*****************************************************************************/
HANDLE WINAPI CXL2Monitor( );

/*****************************************************************************
Funktionsname:	 CxCameraType    
Beschreibung:    Control Panel für Kameratype
				 
Argumente: 
	camera		 char pointer auf namensstring der Kamera 	
    
Rückgabewert:        
    keine

Bemerkung:		
*****************************************************************************/
void WINAPI CxCameraType( char * camera );

/*****************************************************************************
Funktionsname:	 CxCameraTable     
Beschreibung:    Liefert eine Beschrebungstabelle der initialisierbaren Kameras,
				 d.h Kameraname, InitialisierunsId und KamerahardwareId 	
Argumente: 
	table		 pointer <TABLE> auf eineN Strukturpointer vom Typ <tDcNAme> 
Rückgabewert:        
				 Anzahl der unterschiedlichen Kameraeinträge
Bemerkung:		        
*****************************************************************************/
int WINAPI CxCameraTable(  tDcName ** table  );

/*****************************************************************************
Funktionsname:	 SetVideoDma     
Beschreibung:    Erlauben/Verbieten VideoDMA   
Argumente:
    bStart      => boolscher Wert für TRUE = Start / FALSE = Stop 
    
Rückgabewert:        
     >= 0       => wenn erfolgreich
     -1         => wenn Fehler

Bemerkung:          
                    
*****************************************************************************/
int WINAPI SetVideoDma( BOOL bStart );

/*****************************************************************************
Funktionsname:	 SetMemoryDma     
Beschreibung:    Erlauben/Verbieten MemoryDMA   
Argumente:
    bStart      => boolscher Wert für TRUE = Start / FALSE = Stop 
    
Rückgabewert:        
     >= 0       => wenn erfolgreich
     -1         => wenn Fehler

Bemerkung:          
                    
*****************************************************************************/
int WINAPI SetMemoryDma( BOOL bStart );

/*****************************************************************************
Funktionsname:	 SetVideoOutputModeSet     
Beschreibung:    Setzen der Controllerregister für den Transfer in ein Fenster   
Argumente:
    hwnd		=> Handle auf das Ausgabefenster
	pr			=> Pointer auf Fensterkoordinaten( Clientkoordinaten )  
    stretch		=> TRUE -> downscaling, ist nur sinnvoll bei Bildern
				   mit <= 768 Pixel Breite
				   FALSE -> linke obere Ecke des Bildes

Rückgabewert:        
     >= 0       => wenn erfolgreich
     NULL       => wenn Fehler

Bemerkung:      wenn pr NULL ist wird der gesamte Clientbereich des Fensters 
				genutzt    
                    
*****************************************************************************/
int WINAPI SetVideoOutputMode( HWND hwnd, PRECT pr , int stretch);

/*****************************************************************************
Funktionsname:	 SetBrightness     
Beschreibung:    Setzen des Luminance-Brightness-Wertes     
Argumente:
	pm		=> Wert zw. 0 <= 128 <=  255 | Faktor: 0 <= 1.0 <= 1.999

Rückgabewert:        
	TRUE/FALSE
	  
Bemerkung:      wirkt nur aud den Videokanal    
                    
*****************************************************************************/
int WINAPI SetBrightness( int pm );	


/*****************************************************************************
Funktionsname:	 SetContrast   
Beschreibung:    Setzen des Luminance-Contrast-Wertes   
Argumente:
   	pm		=> Wert zw. 0 <= 64 <=  127 | Faktor: 0 <= 1.0 <= 1.999


Rückgabewert:        
     TRUE/FALSE

Bemerkung:      wirkt nur aud den Videokanal 
                    
*****************************************************************************/
int WINAPI SetContrast( int pm );

/*****************************************************************************
Funktionsname:	 SetSaturation   
Beschreibung:    Setzen des Farb-Sättigungs-Wertes   
Argumente:
   	pm		=> Wert zw. 0 <= 64 <=  127 | Faktor: 0 <= 1.0 <= 1.999


Rückgabewert:        
     TRUE/FALSE

Bemerkung:      wirkt nur aud den Videokanal 
                    
*****************************************************************************/
int WINAPI SetSaturation( int pm );


/*****************************************************************************
Funktionsname:	 SetGain( int pm )   
Beschreibung:    Setzen des Gain( Verstärkungs )-Wertes   
Argumente:
   	pm		=> Wert zw. 31 <= pm <=  213 

Rückgabewert:        
     TRUE/FALSE

Bemerkung:      wirkt direkt  auf Kamera 
                 
*****************************************************************************/
int WINAPI  SetGain( int pm );

/*****************************************************************************
Funktionsname:	 SetBlackLevel( int pm )   
Beschreibung:    Setzen des BlackLevel( Offset )-Wertes   
Argumente:
   	pm		=> Wert zw. 31 <= pm <=  213 

Rückgabewert:        
     TRUE/FALSE

Bemerkung:      wirkt direkt auf Kamera 
                 
*****************************************************************************/
int WINAPI SetBlackLevel( int pm );

/*****************************************************************************
Funktionsname:	 SetHighSpeedShutter( int pm )   
Beschreibung:    Setzen des HighSpeedshutters( Offset )-Wertes   
Argumente:
   	pm		=> Wert zw. kamera-abhängig

Rückgabewert:        
     TRUE/FALSE

Bemerkung:      wirkt direkt auf Kamera 
                 
*****************************************************************************/
int WINAPI SetHighSpeedShutter( int pm );

/*****************************************************************************
Funktionsname:	 SetLowSpeedShutter( int pm )   
Beschreibung:    Setzen des LowSpeedshutters( Offset )-Wertes   
Argumente:
   	pm		=> Wert zw. Wert zw. kamera-abhängig

Rückgabewert:        
     TRUE/FALSE

Bemerkung:      wirkt direkt auf Kamera 
                 
*****************************************************************************/
int WINAPI SetLowSpeedShutter( int pm );


/*****************************************************************************
Funktionsname:	 SetFastSpeedShutter( int pm )   
Beschreibung:    Setzen des FastSpeedshutters( Offset )-Wertes   
Argumente:
   	pm		=> Wert zw. Wert zw. kamera-abhängig

Rückgabewert:        
      TRUE/FALSE

Bemerkung:      wirkt direkt auf Kamera 
                 
*****************************************************************************/
int WINAPI SetFastSpeedShutter( int pm );


/*****************************************************************************
Funktionsname:	DWORD   GetBmp( PVOID bp, int modus );
				HGLOBAL GetBmpHandle( int modus );
				DWORD   GetBmpNr( PVOID bp, int modus, int nr );
				HGLOBAL GetBmpNrHandle( int modus, int nr );

Beschreibung:   Liefert die nächste gültige Bitmap    
Argumente:
   	pp		=> Pointer auf einen reservierten Bereich
	modus   => Art des zu liefernden Bildes: - 0 nativ 1 - convert
	nr      => Dmaindex des zu liefernden Bildes für Sequenz- und Loop-Betrieb

Rückgabewert:        
    DWORD		>0  - ok
				<=0 - Fehler

	HGLOBAL     Handle auf den Bereich

Bemerkung:      Die Information inclusive Bitmapfileheader & Bitmapinfoheader
				geliefert 
*****************************************************************************/
DWORD   WINAPI  GetBmp( PVOID bp, int modus	);
HGLOBAL WINAPI  GetBmpHandle( int modus		);
DWORD   WINAPI  GetBmpNr( PVOID bp, int modus, int nr );
HGLOBAL WINAPI  GetBmpNrHandle( int modus, int nr );
DWORD	WINAPI GetBmpDirect( PBITMAPFILEHEADER pBmf, PBITMAPINFOHEADER pBmi, PBYTE  pBmp, int modus	);


/*****************************************************************************
Funktionsname:	HGLOBAL  GetBmpPartial(   r , int modus);
				HGLOBAL  GetBmpNrPartial( CRect r , int modus, int nr);

Description:    Auschnitt des Scans als Bitmap liefern  	
Parameters:		modus - 0 nativ 1 - convert
							
Return Value:	handle != 0
				Fehler == 0
Notes:     		
*****************************************************************************/
//HGLOBAL  WINAPI GetBmpPartial( PRECT r , int modus);
//HGLOBAL  WINAPI GetBmpNrPartial( PRECT r , int modus, int nr);

/*****************************************************************************
Funktionsname:	PBYTE GetDataPtr( );
				PBYTE GetDataPtrByNr( int nr );

Beschreibung:   Liefert einen Pointer auf den Bereich der Scannerdaten     
Argumente:
	nr   => Nummer des Puffer bei Spezialanwendungen

Rückgabewert:        
    PBYTE		NULL  - ungültig
				!NULL - Pointer

Bemerkung:      Die Pointer werden für den statischen Ram geliefert
				
*****************************************************************************/
PBYTE   WINAPI  GetDataPtr( );
PBYTE   WINAPI  GetDataPtrByNr( int nr );


/*****************************************************************************
Functionname :      GetBmpAdvanced
Description:        
Parameters:			pBmf	- Fileheader
					pBmi    - Bitmapinfoheader
					type	- Bildtype	
					pixRes  - Pixelauflösung
					    
Return Value:		TRUE / FALSE
Notes:              
*****************************************************************************/     
HANDLE  WINAPI GetBmpAdvanced( /*PBITMAPFILEHEADER pBmf, PBITMAPINFOHEADER pBmi, */ int type, int pixRes );


/*****************************************************************************
Funktionsname:  SetWindowMode( BOOL enable, PRECT prect )
				
Beschreibung:   Erlaubt / Verbietet der Kamera den Fenstermodus     
Argumente:
   	enable		=> Erlaubnis des modes
	prect		=> Frame
	
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:       
*****************************************************************************/
BOOL WINAPI  SetWindowMode( BOOL enable, PRECT prect );

/*****************************************************************************
Funktionsname:	BOOL SetTriggerMode( BOOL set)
				
Beschreibung:   Erlaubt / Verbietet den Kameratriggermode     
Argumente:
   	set		=> set - Erlaubis
	
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:       
*****************************************************************************/
BOOL WINAPI  SetTriggerMode( BOOL set, BOOL dis );

/*****************************************************************************
Funktionsname:	BOOL DoTrigger()
				
Beschreibung:   Softwaretrigger     
Argumente:
   		
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:      Formt intern einen Triggerimpuls  
*****************************************************************************/
BOOL  WINAPI   DoTrigger( );

/*****************************************************************************
Funktionsname:	BOOL SetBinningMode( BOOL set)
				
Beschreibung:   Erlaubt / Verbietet den KameraBinningmode     
Argumente:
   	set		=> set - Erlaubis
	
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:      In Abhängigkeit von der eigentlichen Kamerafirmware
				werden die Daten mindestens doppelt so schnell übertragen 
*****************************************************************************/
BOOL WINAPI  SetBinningMode( BOOL set	);

/*****************************************************************************
Funktionsname:	 DLLSettings     
Beschreibung:    Setzen von globalen Werten ind der DLL die das Verhalten
				 der DLL steuern können   
Argumente: 
	settings	 pointer auf eine Struktur PCAMERADLLSETS 
    
Rückgabewert:        
    TRUE		 => wenn erfolgreich
    FALSE        => wenn Fehler

Bemerkung:		Werte werden in die Registry eingetragen          
*****************************************************************************/
int WINAPI DLLSettings(  PCAMERADLLSETS settings  ); 
PCAMERADLLSETS WINAPI DLLSettingsGet( ); 


/*****************************************************************************
Funktionsname:	BOOL GetCameraInfo( * tPCameraType T, int sizeT, * tPCameraStatus S, int sizeS ) 
				
Beschreibung:   Nach erfolgter Kamerainformation enthält der Status und der Typ 
				die wichtigsten Kameraparameter      
Argumente:
	* tPCameraType		T	- pointer aus struktur
	int				sizeT	- laenge der daten
	* tPCameraStatus    S	- pointer auf struktur
	int				sizeS 	- laenge der daten
	
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:      
*****************************************************************************/
BOOL  WINAPI GetCameraInfo( tPCameraType T, int sizeT, tPCameraStatus S, int sizeS ); 

/*****************************************************************************
Funktionsname:	BOOL SetCameraInfo( * tPCameraType T, * tPCameraStatus S ) 
				
Beschreibung:   Die Kamerainformation Status und Typ wird in den Kameramodul
				übertragen und dieser neu initialisiert 
				
Argumente:
	* tPCameraType		pct	- pointer auf struktur
	* tPCameraStatus    pcs	- pointer auf struktur
	
	
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:      
*****************************************************************************/
BOOL  WINAPI SetCameraInfo( tCameraType * pct, tCameraStatus *pcs );

/*****************************************************************************
Funktionsname:	 DLLErrorCode    
Beschreibung:    Gibt den letzten Fehler zurück
				 
Argumente: 
	keine
    
Rückgabewert:        
    int 		 => Fehlercode siehe cerror.h

Bemerkung:		
*****************************************************************************/
int WINAPI DLLErrorCode( );
/*****************************************************************************
Funktionsname:	BOOL SetScanMode( int sm ,PVOID pvmem ) 
				
Beschreibung:   Nach erfolgter Kamerainstallation kann der Scanmodus umgestellt werden 
				
Argumente:
	sm		-	mode
	SM_A     =  Automatisch( Standard ) --> statischer Speicher
	SM_SS1TS = Static Memory SnapShotSingle,
	SM_SS1TC = Static Memory SnapShotContinue,
	SM_SSMTS = Static Memory MultiShotSingle,
	SM_SSMTC = Static Memory MultiShotContinue,

  	pvmem	- zeigt auf Struktur vom Typ PVMEM  für dynam. Speicher 
			- zeigt auf int-Wert der die Anzahl der geforderten Puffer angibt 
	
Rückgabewert:        
    >0 	      Anzahl der verwalteten DMA-Puffer
	<0        Fehlerwert
	
Bemerkung:	dieser Mode muss von den Bildaufnahmefunktionen berücksichtigt !!!       
*****************************************************************************/
int  WINAPI SetScanMode( int sm , PVOID pvmem );

/*****************************************************************************
Funktionsname:	 GetCxLibVersion
Beschreibung:    
				 
Argumente: 
	keine
    
Rückgabewert:        
    int 		 siehe nachfolgende Tabelle

Bemerkung:		
*****************************************************************************/
//
//	xxxx xxxx xxxx xxxx	xxxx mmmm mmmm nnnn nnnnn
//						|||| \_______/ \________/ 
//						|||| 		 \			\ MinorNumber
//						|||+--- CX05G \ MajorNumber 
//						||+---- CX05C
//						|+-----	CX13G
//						+------ CX13C

int WINAPI  GetCxLibVersion();


/*****************************************************************************
Funktionsname:	BOOL StopScanMode( ) 
				
Beschreibung:   vor jeder Speicherfreigabe von virtuellem Speicher, der für den MemoryDMA
				( Bildaufnahme ) genutzt wurde ist dieser Befehl auszuführen .  
				
Argumente:
				keine
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:	Diese Funktion beendet den RPS-Zyklus
*****************************************************************************/
BOOL WINAPI StopScanMode( );

/*****************************************************************************
Functionname:	LoadMacroControl	
Description:    Laden eines Macros     	
Parameters:
	
			
Return Value:
	FALSE		 - Bibliothek nicht inialisiert
	TRUE 		 - ok
	CE_EXECMACRO - Macroladefehler

Notes:     
*****************************************************************************/
int   WINAPI LoadMacroControl( PCHAR mname, int nr_rps  ); 

/*****************************************************************************
Functionname:	StartMacro	
Description:    startet ein spezielles zuvor geladenes Macro    	
Parameters:
	timeout - wert in sekunden
			
Return Value:
	TRUE 
	CE_EXECMACRO

Notes:     
*****************************************************************************/
int   WINAPI StartMacroControl( int timeout, int nr_rps );

/*****************************************************************************
Funktionsname:	SetRgbLut
				
Beschreibung:   Übergibt Faktoren für die manuelle RGB-LUT-Manipulation.
Argumente:
   	enable		=> erlaubt die Anwendung in den LUT's
	set			=> übernimmt die Werte in die Bibliothek
	*fr,*fg,*fb	=> hier werden die Kennlinienfaktoren übergeben 

Rückgabewert:        
	TRUE/FALSE

Bemerkung:      Aus diesen Faktoren werden die RGB LUT's berechnet
				In diese LUT's gehen ebenfalls die manuell gesetzten Faktoren ein     
				Diese LUT's wirken nur auf Daten, die mit GetBmp/GetBmpHandle angefordert werden 
*****************************************************************************/
BOOL WINAPI SetRgbLut( int enable, int set, double * fr, double * fg, double * fb );

/*****************************************************************************
Functionname :      SetRgbLutAdd
Description:        additional universal characteristic for r,g,b
Parameters:			bEnable		- enable/disable the use of lut-arrays
					awRed		- array of max. 4096 entrys for red  -lut
					awGreen		- array of max. 4096 entrys for green-lut
					awBlue		- array of max. 4096 entrys for blue -lut

Return Value:		TRUE / FALSE
Notes:              
*****************************************************************************/   
BOOL WINAPI SetRgbLutAdd( BOOL bEnable, WORD awRed[4096], WORD awGreen[4096], WORD awBlue[4096] );



/*****************************************************************************
Funktionsname:	SetWhiteBalance
				
Beschreibung:   Übergibt Faktoren für die Whitebalance.
Argumente:
   	enable		=> erlaubt die Anwendung in den LUT's
	set			=> übernimmt die Werte in die Bibliothek
	*fr,*fg,*fb	=> hier werden die Kennlinienfaktoren übergeben 

Rückgabewert:        
	TRUE/FALSE

Bemerkung:      Aus diesen Faktoren werden die RGB LUT's berechnet
				In diese LUT's gehen ebenfalls die manuell gesetzten Faktoren ein     
				Diese LUT's wirken nur auf Daten, die mit GetBmp/GetBmpHandle angefordert werden 
*****************************************************************************/
BOOL WINAPI SetWhiteBalance( int enable, int set, double * fr, double * fg, double * fb );


/*****************************************************************************
Funktionsname:	DoWhiteBalance
				
Beschreibung:   Berechnet die Faktoren für die Whitebalance.
Argumente:
   	enable		=> erlaubt die Anwendung der errechneten Faktoren in den LUT's
	*fr,*fg,*fb	=> hier werden die Kennlinienfaktoren zurückgegeben 

Rückgabewert:        
	TRUE/FALSE

Bemerkung:      Aus diesen Faktoren werden die RGB LUT's berechnet
				In diese LUT's gehen ebenfalls die manuell gesetzten Faktoren ein     
				Diese LUT's wirken nur auf Daten, die mit GetBmp/GetBmpHandle angefordert werden 
*****************************************************************************/
DWORD WINAPI DoWhiteBalance( int enable, double * fr, double * fg, double * fb  );

/*****************************************************************************
Funktionsname:	BOOL IsTriggerEvent( int ms)
				
Beschreibung:   Fragt ab, ob neues Bild über Trigger eingelaufen ist    
Argumente:
   	ms		=> Zeit in ms
	
Rückgabewert:        
    BOOL		TRUE / FALSE - Bild eingelaufen oder nicht
	
Bemerkung:       
*****************************************************************************/
BOOL WINAPI IsTriggerEvent( int ms );



/*****************************************************************************
Functionname:	SetPrivate	
Description:    für interne testzwecke vorgesehen    	
Parameters:
	s   - special value
			
Return Value:
	keiner
Notes:     
*****************************************************************************/
void   WINAPI SetPrivatControl( int s );

/*****************************************************************************
Functionname:	GetImSizeX	
Description:    Bildbreite in Pixeln  	
Parameters:
				keiner 
			
Return Value:	Wert >= 0
				Fehler -1
Notes:     
*****************************************************************************/
int    WINAPI GetImSizeX(  );

/*****************************************************************************
Functionname:	GetImSizeY	
Description:    Bildhöhe in Pixeln  	
Parameters:
				keiner 
			
Return Value:	Wert >= 0
				Fehler -1
Notes:			MSB is an flag for frametransfer-image
				Here you will got the number of lines for an interlaced frame
				If you want to know the full number of a complete line you 
				must multiply the lines with 2
*****************************************************************************/
int    WINAPI GetImSizeY(  );

/*****************************************************************************
Functionname:	GetImOffX	
Description:    horiz. Bildoffset in Pixeln  	
Parameters:
				keiner 
			
Return Value:	Wert >= 0
				Fehler -1
Notes:     		sinnvoll wenn auf Rohdaten zugegriffen wird
*****************************************************************************/
int    WINAPI GetImOffX(  );

/*****************************************************************************
Functionname:	GetImOffY	
Description:    vertik. Bildoffset in Pixeln  	
Parameters:
				keiner 
			
Return Value:	Wert >= 0
				Fehler -1
Notes:     		sinnvoll wenn auf Rohdaten zugegriffen wird
*****************************************************************************/
int    WINAPI GetImOffY(  );

/*****************************************************************************
Functionname:	GetImFrameX	
Description:    GesamtBildbreite in Pixeln  	
Parameters:
				keiner 
			
Return Value:	Wert >= 0
				Fehler -1
Notes:     		sinnvoll wenn auf Rohdaten zugegriffen wird
*****************************************************************************/
int    WINAPI GetImFrameX(  );

/*****************************************************************************
Functionname:	GetImOffY	
Description:    GesamtBildhöhe in Pixeln  	
Parameters:
				keiner 
			
Return Value:	Wert >= 0
				Fehler -1
Notes:     		sinnvoll wenn auf Rohdaten zugegriffen wird
*****************************************************************************/
int    WINAPI GetImFrameY(  );

/*****************************************************************************
Functionname:	GetImBpp	
Description:    Pixelbreite in Byte  	
Parameters:
				keiner 
			
Return Value:	Wert >= 0
				Fehler -1
Notes:     		sinnvoll wenn auf Rohdaten zugegriffen wird
*****************************************************************************/
int    WINAPI GetImBpp(  );

/*****************************************************************************
Funktionsname:	DWORD GetImNumber
				
Beschreibung:   Gibt aktuelle Bildnummer und den aktuellen DMAIndex wieder
Argumente:
	info		Pointer auf den wenn ungleich null, ein imstamp hinterlegt wird
   	
Rückgabewert:        
    DWORD		0xFFFFFFFF - Fehler
				High Word  - DmaIndex für loopmacros
				Low  Word  - ImageNumber

				
Bemerkung:      ImageNuber mit 12 Bit 
*****************************************************************************/
DWORD WINAPI GetImNumber( int * info );


/*****************************************************************************
Funktionsname:	DWORD IsNewImage( )
				
Beschreibung:   Fragt ab, ob neues Bild über Trigger eingelaufen ist    
Argumente:
   		
Rückgabewert:        
    DWORD		High Word  - TRUE / FALSE
				Low  Word  - ImageNumber

Bemerkung:       
*****************************************************************************/
DWORD WINAPI IsNewImage(  );


/*****************************************************************************
Funktionsname:	 SetVideoOffset     
Beschreibung:    Modifiziert den Offset im Livebild   
Argumente:
    int x,y      Verschiebung 
    int *xx, *yy korrigirte Verschiebung
Rückgabewert:        
     > 0       => wenn erfolgreich
     0         => wenn Fehler

Bemerkung:          
                    
*****************************************************************************/
//int WINAPI SetVideoOffset(CSize off, CSize *cor );
int WINAPI SetVideoOffset(SIZE  off, SIZE  *cor );

/*****************************************************************************
Funktionsname:	BOOL SetPixelWidth( int type)
				
Beschreibung:    schaltet die Pixelbreite um     
Argumente:
   	type		 => Type der Pixelbreite --> enum-definition CXPIXWIDTH
	
Rückgabewert:        
    BOOL		TRUE / FALSE - Operation erfolgt
	
Bemerkung:      
*****************************************************************************/
BOOL WINAPI  SetPixelWidth( int type	);

/*****************************************************************************
Functionname :      ShowErrMessage
Description:        Enable or disable the output of error messages
Parameters:
					enable & ERRMSG_BOX  = TRUE    - enable error output to MessageBox
					enable & ERRMSG_FILE = TRUE    - enable error output to File

Return Value:		no
Notes:              The standard setting is to have output of error messages
                    switched off
*****************************************************************************/
#define ERRMSG_BOX  1
#define ERRMSG_FILE 2

void WINAPI SetMsgOutput( int flags);

/*****************************************************************************
Functionname :      SetBaumerLink
Description:        Selection of Canal and Baudrate of Baumerlink 
Parameters:
					HIGHWORD(set) = enablebits
					LOWWORD( set) = settingbits
					Bit 1:	Canal   A/B  - 0/1
					Bit 2:	FastCom FAST -   1
    
Return Value:		Current state of Baumerlink
Notes:              
*****************************************************************************/
int WINAPI SetBaumerLink( int flags);

/*****************************************************************************
Functionname :      SetShutterTime
Description:        Set the Shuttertime to the nearest possible value   
Parameters:			tms - Shuttertime
					sht - Pointer to structure of type tSHT
					    
Return Value:		TRUE / FALSE
Notes:              if sht == NULL the structure is not filled
					if tms == -1 only structure sht is filled, no io-operation 
					is done 
*****************************************************************************/
typedef struct {
	int time2set;	// time to set
	int timeNear;	// time that can be realized
	int timePrev;	// time that can be realized before
	int timeNext;	// time that can be realized next
	int timeMin;	// minimal possible time
	int timeMax;	// maximal possible time
} tSHT, * ptSHT; 

BOOL  WINAPI SetShutterTime( int tms, ptSHT sht );

/*****************************************************************************
Functionname :      SetGainFactor
Description:        Set the Gainfactor to the nearest possible value   
Parameters:			factor - GainFactor
					gain   - Pointer to structure of type tGAIN
					    
Return Value:		TRUE / FALSE
Notes:              if gain   == NULL the structure is not filled
					if factor == 0.0  the structure is filled with values,
					so the range can be detected
*****************************************************************************/     
typedef struct {
	double gain2set;	// gain to set
	double gainNear;	// gain that can be realized
	double gainPrev;	// gain that can be realized before
	double gainNext;	// gain that can be realized next
	double gainMin;	// minimal possible gain
	double gainMax;	// maximal possible gain
} tGAIN, * ptGAIN; 

BOOL  WINAPI SetGainFactor( double factor, ptGAIN sht );

/*****************************************************************************
Functionname:	FindShutterFind
Description:    Finden einer akzeptablen Einstellung für das Kamerabild
Parameters:		Brightness	- Level aus das eingestellt werden soll
				Prozent		- prozentuale Angabe für die Pixel die den Wert 
							  Brightness 	
				Area		- Range of data aquisition 

Return Value:	TRUE / FALSE
				
Notes:     		
*****************************************************************************/
BOOL	WINAPI FindShutter( int brightness, int prozent, RECT area  );

/*****************************************************************************
Functionname :      AutomaticShutter
Description:        Start an automatic  mechanism to control the shutter time
					for a given brightness 
Parameters:			start  - enable(TRUE)/disable(FAlSE); 
					bright - brightness to control
					    
Return Value:		TRUE / FALSE
Notes:              
*****************************************************************************/     
BOOL  WINAPI AutomaticShutter( int start, int bright );

/*****************************************************************************
Functionname :      Focus
Description:        
Parameters:			raster	- pixel distance between mesurement rows or colums 
					rect	- area of measurement	
					    
Return Value:		measurement value mv
Notes:              mv ist the average value of all
*****************************************************************************/     
double  WINAPI GetFocusIndicator( int raster, RECT r );

/*****************************************************************************
Functionname :      ReadoutMode
Description:        Set for a special readoutmode of camera 
					
Parameters:			value		- mode
					       - filename
					    
Return Value:		FALSE / TRUE
Notes:              only for special cameras
*****************************************************************************/     
BOOL  WINAPI ReadoutMode( int value );

/*****************************************************************************
Functionname :      SetMacbethColors
Description:        
Parameters:			acrMacbeth		- area of measured color patterns
					adblMatrix		- transformation matrix
Return Value:		
Notes:              
*****************************************************************************/ 
int WINAPI SetMacbethColors(COLORREF acrMacbeth[24], double adblMatrix[4][4]);


/*****************************************************************************
Functionname :      SetTransformationMatrix
Description:        
Parameters:			adblMatrix		- transformation matrix

Return Value:		TRUE / FALSE
Notes:              
*****************************************************************************/     
BOOL WINAPI SetTransformationMatrix( double adblMatrix[4][4] );

/*****************************************************************************
Functionname :      EnableTransformationMatrix
Description:        
Parameters:			enable		-enable/disable 

Return Value:		TRUE / FALSE
Notes:              
*****************************************************************************/     
BOOL WINAPI EnableTransformationMatrix( BOOL enable  );


/*****************************************************************************
Functionname :      GetHistogram
Description:        Histogrammcalculation 
Parameters:			aiHistogram - array of 4096 items, for up to 12 bit pixelresolution

Return Value:		FALSE
					<0   - see error.h
					>0   - Number of Histogrammitems 
Notes:              
*****************************************************************************/     
int WINAPI GetHistogram(int aiHistogram[4096] ); 


/*****************************************************************************
Functionname :      DefineImageEventHandle
Description:        A valid eventhandle  can signal a new image event  
Parameters:			<event> - Handle to an event

Return Value:		TRUE/FALSE
					<0 - see error.h	
Notes:              Only vor Windows 2000/98
*****************************************************************************/     
int WINAPI DefineImageEventHandle( HANDLE event );

/*****************************************************************************
Functionname :      SetAuxOptions
Description:        A valid eventhandle  can signal a new image event  
Parameters:			<pAux> - pointer to a structure with control statements 

Return Value:		TRUE/FALSE
					<0 - see error.h	
Notes:              see "camera.h"
*****************************************************************************/     
int WINAPI SetAuxOptions( tpAUXSETIDHD pAux );

/*****************************************************************************
Functionname :      GetHwConfig
Description:        Asking about installed and supprted hardwarecomponents
Parameters:			<pConf> - pointer to a structure with control statements 

Return Value:		TRUE/FALSE
					<0 - see error.h	
Notes:              see "camera.h"
*****************************************************************************/   
#define HWREQPARAMETER 8
#define HWANSPARAMETER 8

typedef struct {
	int  iIdRequest;
	WORD awParameter[HWREQPARAMETER];
} tHWCONFIGR, * tpHWCONFIGR;

typedef struct {
	WORD awParameter[HWANSPARAMETER];
} tHWCONFIGA, * tpHWCONFIGA;

enum {
	ASKIFC,
	ASKCAM,
};

int WINAPI GetHwConfig( tpHWCONFIGR pConf, tpHWCONFIGA pAnswer);

int WINAPI ControlMuxCamera( tpParameterNum commandDesc );

/*****************************************************************************
Functionname :      GetHotSpot
Description:        Test of matrix for local deviation in signal
					
Parameters:			imNr		- bildanzahl
					deviation	- deviation in counts
					name        - filename
					    
Return Value:		Pointer to HotSpot
Notes:              - to use with a dark ccd ( not illuminated ) and exposured in LowSpeedshutter
					- if filename is NULL no file is saved
					- the file has the structure of tHotSpotHd
					- returnvalue is a pointer to an internal structure, 
					  every new call of this function or a reinitialisation of 
					  the lib will destroy this pointer
*****************************************************************************/     
typedef struct {
	WORD  x;			// x koordinate
	WORD  y;			// y koordinate	
	FLOAT delta;		// delta 

}	tHotSpot, *ptHotSpot;

typedef struct {
	int			nb;		// Number of items tHotSpot
	tHotSpot	hs[1];	// Field  of items tHotSpot

} tHotSpotHd, *ptHotSpotHd;

ptHotSpotHd WINAPI GetHotSpot( int imNr,  int deviation, char * name );

char * WINAPI CommandStrToCam( char * pchr );

int WINAPI InitCxLibGeneric(  int camType, char * csubname, int breite, int hoehe, int bpp, int iface  );    
int WINAPI ReInitCxLibGeneric(  int camType, char * csubname, int breite, int hoehe, int bpp, int iface  );    

DWORD WINAPI DoWhiteBalanceRect( int enable, double * fr, double * fg, double * fb, RECT rect  );
int   WINAPI CameraSystemManager( );
int WINAPI InitCxLibSys(   );

#ifdef __cplusplus
}
#endif


#endif // _VIDEO_API_