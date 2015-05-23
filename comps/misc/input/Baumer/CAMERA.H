#ifndef _CAMERA_H_
#define _CAMERA_H_

enum CXTYPES {
	
	MX05		= 0,	// MX05G	-	Schwarzweiß	Matrix 75 
	MX05C		= 1,	// MX05		-	Farbe		Matrix 75
	MX13		= 2,	// MX13G	-	Schwarzweiß	Matrix 85 
	MX13C		= 3,	// MX13		-	Farbe    	Matrix 85 
	CX20G		= 4,	// noch nicht realisiert
	CX20C		= 5,	// noch nicht realisiert
	CX05		= 6,	// CX05G	-	Schwarzweiß	Matrix 75     
	CX05C		= 7,	// CX05		-	Farbe		Matrix 75	  
	CXCM1G		= 8,	//			-	CMOS Kamera	sw
	CXCM1C		= 9,	//			-	CMOS Kamera	co
	CX13		= 10,	// CX13G	-	Schwarzweiß	Matrix 85     
	CX13C		= 11,	// CX13		-	Farbe    	Matrix 85     
	CX13TMG		= 99,
	CXGENERIC	= 12,	// generischer type kann alles und nichts sein
						// hängt in jedem Fall vom verwendeten pga file ab
						// nur für internen gebrauch   
	CX32C		= 13,	// CX32		-	Farbe    	Matrix 242 
	CX131		= 14,	// CX131	-	SW    		Matrix  85 
	CX131C		= 15,	// CX131C	-	Farbe    	Matrix  85 

	CX13F		= 16,	// CX13G	-	Schwarzweiß	Matrix 85     
	CX13CF      = 17,   // CX13		-	Farbe    	Matrix 85     

	ARC1000		= 20,	// CX05
 	ARC1000C	= 21,   // CX05C
	ARC1800		= 22,	// CX13
	ARC1800C	= 23,	// CX13C
	ARC4000		= 24,	// CX131
	ARC4000C	= 25,   // CX131C
	ARC6000C	= 26,	// CX32C

	DXM33		= 50,	// CX32C

	CX13QVC		= 80,	// CX131C	
	CX13QV	    = 81,	// CX131
	CX13QV1C	= 82,	// CX131C
	CX13QV1		= 83,	// CX131
	
	DC100		= 100,	// MX05C
	DC150		= 101,	// MX05
	DC200		= 102,	// MX13C
	DC250		= 103,	// MX13
	DC300		= 104,	// CX32C
	DC300F		= 105,	// CX131C
	DC350F		= 106,	// CX131

	CX1000      = 1000, // Modul mit Kameramultiplexer
	SIPLACEIX13 = 1000, // Modul mit Kameramultiplexer
};


typedef struct {
	_TCHAR* name;
	int	    type;
	int     hwid; 
} tDcName, ptDcName;




enum CXROM {			// Special for CX32C

	ROMSTD = 0,
	ROMDM  = 0,			// Draft
	ROMA0,				// AF1
	ROMA1,				// AF2
	ROMFM,				// FM
};


enum CXPIXWIDTH	{

	W8BIT,
	W10BIT,
	W12BIT,
	W16BIT

};

enum CXSHMODE {
	
	SHNO,
	SHLS,
	SHHS,
	SHAUTO,
	SHFM
};


typedef struct _T_CT_ {
	
	int		type;		// SW75, CO75, SW85, CO85, SW20, CO20
	int     hwtype;		// HW-Identifier eingeführt ab CxSerie
	char *  typeByName; // namentliche Kennzeichnung der Kamera
	SIZE	tpix;		// Takt-Pixel
	SIZE	gpix;		// GesamtPixel
	SIZE	ppix;		// Bildpixel
	SIZE	opix;		// Bildoffset
	SIZE	apix;		// Alg.Offset für Farbberechnung
	double	freq;		// Kamera-Taktfrequenz 

	int     boff;

	int		bpp;		// BitProPixel
	BOOL	bColor;		// Farbekamera
	
	int		cOffset;	// SchwarzwertOffset-Control
	int		vOffsetMin;	// SchwarzwertOffset Minimalwert
	int     vOffsetMax;	// SchwarzwertOffset Maximalwert

	int		cAmpl;		// Verstärker-Control ( Gain )
	int		vAmplMin;	// Verstärker Minimalwert
	int     vAmplMax;	// Verstärker Maximalwert

	BOOL	cShutLS;	// Shutteroption
	BOOL	cShutHS;	// Shutteroption
	BOOL	cShutAS;	// AutomaticShutter
	BOOL	cShutFS;	// FastShutter
	BOOL	cTrigMode;	// Triggeroption
	BOOL	cIlacMode;	// Interlacedmode
	BOOL	cBinnMode;	// Binningmode

	int		cCamInterface; // Controlinterface
						   // Centronics, Debi ...
	int		vShutLsMin;	   // LsShutter Minimalwert
	int		vShutLsMax;	   // LsShutter Maximalwert
	int		vShutLsDis;
	
	int		vShutHsMin;	   // HsShutter Minimalwert
	int		vShutHsMax;	   // HsShutter Maximalwert
	int		vShutHsDis;

	int		vShutFsMin;	   // HsShutter Minimalwert
	int		vShutFsMax;	   // HsShutter Maximalwert
	int		vShutFsDis;

	int     vxoff;		// Offsetwerte video 
	int     vyoff;
	int     mxoff;		// Offsetwerte memory 
	int     myoff;

	DWORD	econfig1;	// enable config Bits --> status siehe cconfig
	DWORD	econfig2;
	DWORD	econfig3;
	DWORD	econfig4;

} tCameraType, * tPCameraType;

typedef	struct { double red, green, blue; }	FLUT, * PFLUT;
typedef	struct { double red, green, blue; }	FAWB, * PFAWB;
typedef	struct { double offset, gamma;    }	VYPS, * PVYPS;
typedef struct { double offset, gamma;    }	VCR,  * PVCR;

typedef struct  _T_CS_ {
	
	struct {
		unsigned okFg		: 1;	// Framegrabber initialisiert
		unsigned okIo		: 1;	// Controlinterface erkannt
		unsigned okMem		: 1;	// statischer Speicher 
		unsigned okVideo	: 1;	// videosupport
		unsigned okMacroL	: 1;	// StartMacro gefunden
		unsigned okMacroE	: 1;	// StartMacro ausgeführt
		unsigned okFgFound	: 1;	// Framegrabber gefunden
		unsigned okCam		: 1;	// Camera ok
		
	} init;
	
	int		cShutMode;		// Shuttermode
	int		vShutLS;		// io-Werte eintragen
	int		vShutHS;		// io-Werte eintragen
	int		vShutFS;		// io-Werte eintragen

	int		vShutAS;		// ysoll
	int		vShutAS_SM;		// aktueller Shuttermode im Autoshuttermode
	int		vShutAS_LS;
	int		vShutAS_HS;

	double  vShutLSTime;	// Lowspeedtime pro Frame in LowSpeed
	int     vPivNr;			// Bildnummer
	double  vPicNrSetTime;	// n.c.

	int		vOffset;		// aktueller SWOffsetwert
	int		vAmplif;		// aktueller Verstärkungswert
	double	vAmplifFactor;	// aktueller Verstärkungswert als absoluter Faktor

	int		vDelay;			// vorausssichtliche Zeit bis zum Eintreffen des Bildes

	RECT    rInspect;		// Fenster für Inspectionen

	int		bLutEnable;		// 
	FLUT	vLut;			//							
	
	int		bAwbEnable;
	FAWB	vAwb;

	int		bYpsEnable;
	VYPS	vYps;

	int		bCrEnable;
	VCR		vCr;

	int		vPicNr;

	int		bVideoDmaDisable;
	RECT	sVideoRect;
	BOOL	bVideoScale;
	BOOL	bVideoMirrorVer;
	BOOL	bVideoMirrorHor;

	int		bMemoryDmaDisable;
	BOOL	bMemoryDoubleBuffer;
	BOOL	bMemoryRingBuffer;
	BOOL	bMemoryRingBufferNum;

	// Formatfenster
#ifdef CXLIBINTERN
	CRect rcCamMax;			// maximaler Bilddatenframe -->Begrenzung
	CRect rcCamInp;			// aktueller Bilddatenframe -->
	CRect rcZoomIn;			// geforderter Bidauschnitt für Übertragung
	CRect rcCaZoomInCor;	// korrigiertes rcZoomIn für Kamera
	CRect rcFgZoomInCor;	// korrigiertes rcZoomIn für den Framegrabber

	CRect rcVideoOut;		// Videoausgabeframe
	CRect rcVideoOutCor;	// korrigierter Videoausgabeframe
#else
	RECT rcCamMax;			// maximaler Bilddatenframe -->Begrenzung
	RECT rcCamInp;			// aktueller Bilddatenframe -->
	RECT rcZoomIn;			// geforderter Bidauschnitt für Übertragung
	RECT rcCaZoomInCor;	// korrigiertes rcZoomIn für Kamera
	RECT rcFgZoomInCor;	// korrigiertes rcZoomIn für den Framegrabber
	RECT rcVideoOut;		// Videoausgabeframe
	RECT rcVideoOutCor;	// korrigierter Videoausgabeframe
#endif //CXLIBINTERN


	// DMA3-Parameter
	int nrSubZ;				// Anzahl berechneter Subzeilen
	int szSubZ;				// Länge  der berechneten Subzeile
	int nrLines;			// Anzahl berechneter Bildzeilen
	
	// Camera-Modis
	BOOL  bBinning;
	BOOL  bTriggerEn;
	BOOL  bTrigger;
	BOOL  bWindow;
	BOOL  bScaling;
	BOOL  bVideoDma;
	BOOL  bMemoryDma;
	BOOL  bStandBy;
	int   iBytePerPixel;
	BOOL  bEsync; 
	BOOL  bPnp1;
	BOOL  bPnp2;
	BOOL  bPnp1fl;

	// ScanModis
	int       scanmode;			// Scanmode
							// 0 - Auto( Standard )
							// 1 - SnapShotS
							// 2 - SnapShotD
							// 3 - MultiShotS
							// 4 - MultiShotD
							// 5 - LoopModeD
							// 6 - LoopModeS
	int       scanbuffs;
	PVOID     scandynmm;	// pointer auf speichertabelle

	DWORD cconfig1;
	DWORD cconfig2;
	DWORD cconfig3;
	DWORD cconfig4;

} tCameraStatus, * tPCameraStatus;

enum SCANMODE {

	SM_A     = 0,			// Automatisch( Standard ) --> statischer Speicher

	SM_SS1TS = 1,
	SM_SS1TC = 2,
	SM_SSMTS = 3,
	SM_SSMTC = 4,

	SM_DS1TS = 5,
	SM_DS1TC = 6,
	SM_DSMTS = 7,
	SM_DSMTC = 8,

	SM_SPECIAL = 99,
	SM_REPAIR  = 1000,		// wurde SetScanmode einmal erfolgreich gesetzt dann
							// kann bei Unterbrechung dieser Mode genutzt werden um den alten
							// Zustand wieder herszustellen
};

// types for GetBmpAdvanced
enum BMPADVTYPE 
{
	NATIV		= 0x00000000,// rgb wird dann im Raster abgebildet
	CONVERT		= 0x00000001,// rgb-Bilder werden aus Raster berechnet
	G256		= 0x00000002,// Grauwertbilder die als 256-Farben kommen
	G1024		= 0x0a000002,// Grauwertbilder die als 1024-Farben kommen
	G4096		= 0x0c000002,// Grauwertbilder die als 4096-Farben kommen
	C256		= 0x08000001,// Farbwertbilder die als 256-Farben kommen
	C1024		= 0x0a000001,// Farbwertbilder die als 1024-Farben kommen
	C4096		= 0x0c000001,// Farbwertbilder die als 4096-Farben kommen
	PREVDC300   = 0x00001001,// Preview 1 ( DC300 Draftmode );
	FULLDC300   = 0x00000001,// Fullframe for DC300
};

// structures for auxilary function 
//
typedef struct _AUXSETID  {
	int		id;				// must be a known id of function
	int     set;			// switch on/off
} tAUXSETID, * tpAUXSETID;

typedef struct _AUXSETIDHD  {
	int			   itemNr;	// number of Items
	tAUXSETID	   item[1]; // item itself
} tAUXSETIDHD, * tpAUXSETIDHD;  

enum AUXITEMS {
	CXAUX_CANALATOB,		// switch to fgcanal b
	CXAUX_INVFLUSH,			// invert flushsignal 
	CXAUX_INVTRIGGER,		// invert triggersignal 
	CXAUX_EXTTRIGGER,		// switch to extern trigger
	CXAUX_PATTERN,			// select test pattern
	CXAUX_FGOUT1,			// res/set io1 fg
	CXAUX_FGOUT2,			// res/set io2 fg
	CXAUX_CAMOUT1,			// res/set io1 cam
	CXAUX_CAMOUT2,			// res/set io2 cam
	CXAUX_CAMIN1,			// in      io3 cam
};

#include "cameranewv.h"

#endif //_CAMERA_H_