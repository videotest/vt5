/*****************************************************************************\
*    Program:             CxLib2M                                             *
*    Version:                                                                 *
*    Module:              camera.h                                            *
*    Compiler:            MS Visual C++ 6.0                                   *
*    Operating System:    Windows NT/2000/XP & Windows 98                     *
*    Purpose:                                                                 *
*                                                                             *
*    Notes:                                                                   *
*    Created:             23.01.2002                                          *
*    Last modified:       23.01.2002                                          *
*    Author:              M. Pester                                           *
*    Copyright:           © Baumer Optronic GmbH, Radeberg                    *
\*****************************************************************************/

#ifndef _CAMERA_H_
#define _CAMERA_H_

enum eCXTYPES {
	
	CAMTYPEUNDEFINED = -1,
	
	MX05		= 0,	// MX05G	-	Schwarzweiﬂ	Matrix 75 
	MX05C		= 1,	// MX05		-	Farbe		Matrix 75
	MX13		= 2,	// MX13G	-	Schwarzweiﬂ	Matrix 85 
	MX13C		= 3,	// MX13		-	Farbe    	Matrix 85 
	CX05		= 6,	// CX05G	-	Schwarzweiﬂ	Matrix 75     
	CX05C		= 7,	// CX05		-	Farbe		Matrix 75	  
	CX13		= 10,	// CX13G	-	Schwarzweiﬂ	Matrix 85     
	CX13C		= 11,	// CX13		-	Farbe    	Matrix 85     
	
	CX32C		= 13,	// CX32		-	Farbe    	Matrix 242 
	CX131		= 14,	// CX131	-	SW    		Matrix  85 
	CX131C		= 15,	// CX131C	-	Farbe    	Matrix  85 

	CX13F		= 16,	// CX13G	-	Schwarzweiﬂ	Matrix 85     
	CX13CF      = 17,   // CX13		-	Farbe    	Matrix 85     

	ARC1000		= 20,	// CX05
 	ARC1000C	= 21,   // CX05C
	ARC1800		= 22,	// CX13
	ARC1800C	= 23,	// CX13C
	ARC4000		= 24,	// CX131
	ARC4000C	= 25,   // CX131C
	ARC6000C	= 26,	// CX32C
	ARC4400     = 27, 
	ARC4400C    = 28,
	ARC8000C    = 29,


	DXM33		= 50,	
    DXM50       = 51,

	CX13QVC		= 80,	// CX131C	
	CX13QV	    = 81,	// CX131
	CX13QV1C	= 82,	// CX131C
	CX13QV1		= 83,	// CX131
	
	DC100		= 100,	
	DC150		= 101,	
	DC200		= 102,	
	DC250		= 103,	
	DC300		= 104,	
	DC300F		= 105,	
	DC350F		= 106,	
	DC180		= 107,	
	DC300FX		= 108,
	DC350FX	    = 109,
	DC480	    = 110,

	IXC031S     = 200,


	IX14K01	    = 1001,	// modul with imux + ffheader
	IX05K01		= 1002, // modul with imux + ffheader
	IX14		= 1003, // modul with ffheader
	IX14C		= 1004, // modul with ffheader
	IX05S		= 1005, // modul with ffheader
	IX05CS		= 1006, // modul with ffheader
	IX06S		= 1007, // modul with ffheader
	IX06CS		= 1008, // modul with ffheader
	IX06K01		= 1009, // modul with imux + ffheader
	IX08		= 1010, // modul with ffheader
	IX08C		= 1011, // modul with ffheader
    IX05SII		= 1012, // modul with ffheader
	IX05CSII	= 1013, // modul with ffheader
    IX20K01	    = 1014, // modul with ffheader
};


enum CXSHMODE {
	SHNO,
	SHLS,
	SHHS,
	SHAUTO,
	SHFM
};

enum eCXFPGAFORMAT {
	FPGADESIGN_UNDEFINED,
	FPGADESIGN_UNIFORM,		// uniform format	
	FPGADESIGN_FREEFORMAT,  // free format
	FPGADESIGN_ISOLATED		// special solution
};



enum SCANMODE {
	
	SM_SSMTS = 3,
	SM_SSMTC = 4,

	SM_DSMTS = 7,
	SM_DSMTC = 8,

	SM_DLPMAN		= 111,		// dyn. Speicher, loop management,
	SM_SSMTS_SIZE	= 112,
	SM_SLPMAN		= 113,		// sta. Speicher, loop management,

	SM_REPAIR  = 1000,		// wurde SetScanmode einmal erfolgreich gesetzt dann
							// kann bei Unterbrechung dieser Mode genutzt werden um den alten
							// Zustand wieder herszustellen
};


#endif //_CAMERA_H_