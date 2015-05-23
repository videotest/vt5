#ifndef _MATRIX_H_
#define _MATRIX_H_

#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)

#define	XPIX 768
#define YPIX 582
#define XPIXM XPIX/2
#define YPIXM YPIX/2

#define XYRATIO ((double)XPIX/(double)YPIX )

// hier sollten die Definitioneverschiedensten  unterstützten
// Kameratypen eingetragen werden

// clamp the input to the specified range 
#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)
 
// //////////////////////////////
// Allgemeine Angaben
// //////////////////////////////
//
#define	TAKT_085	(29500000.0/2)
#define TAKT_075    (29500000.0/2)
#define TAKT_075A   (29500000.0/2)
#define TAKT_204	(29500000.0/2)
#define TAKT_CM1	(29500000.0/2)
#define TAKT_CM2	(29500000.0/2)
#define TAKT_12FPS	(20250000.0  )

// //////////////////////////////
// Schwarzweiß Varianten
// //////////////////////////////
//
#define SW75_TXPIX  944		// Taktregime der Kamera
#define SW75_TYPIX  625
#define SW75_XPIX   768
#define SW75_YPIX   582
#define SW75_GXPIX  768		// Gesamtbild
#define SW75_GYPIX  582
#define SW75_OXPIX   0		// Offset im Gesantbild
#define SW75_OYPIX   0
#define SW75_PICOFF  0	
#define SW75_FREQ	TAKT_075
#define SW75_VOFFX   0		// Offsetwerte video 
#define SW75_VOFFY   0
#define SW75_MOFFX   0		// Offsetwerte memory 
#define SW75_MOFFY   0

#define SW75A_TXPIX  944	// Taktregime der Kamera
#define SW75A_TYPIX  625
#define SW75A_XPIX   780
#define SW75A_YPIX   582
#define SW75A_GXPIX  784	// Gesamtbild
#define SW75A_GYPIX  583
#define SW75A_OXPIX   0		// Offset im Gesantbild
#define SW75A_OYPIX   0
#define SW75A_PICOFF  0	
#define SW75A_FREQ	TAKT_075A
#define SW75A_VOFFX   0		// Offsetwerte video 
#define SW75A_VOFFY   0
#define SW75A_MOFFX   0		// Offsetwerte memory 
#define SW75A_MOFFY   0

#define SW85_TXPIX  1616	// Taktregime der Kamera
#define SW85_TYPIX  1043
#define SW85_XPIX   1280
#define SW85_YPIX   1024
#define SW85_GXPIX  1356	// Gesamtbild
#define SW85_GYPIX  1034
#define SW85_OXPIX   4		// Offset im Gesantbild
#define SW85_OYPIX   4		// am 9.6. 99 geändert // 6
#define SW85_PICOFF  0		// 2 24.M	
#define SW85_FREQ	TAKT_085
#define SW85_VOFFX   40		// Offsetwerte video 
#define SW85_VOFFY   2
#define SW85_MOFFX   40		// Offsetwerte memory 
#define SW85_MOFFY   0

#define SW85A_TXPIX  1616	// Taktregime der Kamera
#define SW85A_TYPIX  1043
#define SW85A_XPIX   1300
#define SW85A_YPIX   1030
#define SW85A_GXPIX  1300	// Gesamtbild
#define SW85A_GYPIX  1031
#define SW85A_OXPIX   0     // Offset im Gesantbild
#define SW85A_OYPIX   0		// 
#define SW85A_PICOFF  0		// 
#define SW85A_FREQ	TAKT_085
#define SW85A_VOFFX   40	// Offsetwerte video 
#define SW85A_VOFFY   2
#define SW85A_MOFFX   40	// Offsetwerte memory 
#define SW85A_MOFFY   0

// //////////////////////////////
// Farbvarianten Varianten
// //////////////////////////////
//
#define CO75_TXPIX  944		// Taktregime der Kamera
#define CO75_TYPIX  625
#define CO75_XPIX   768
#define CO75_YPIX   582
#define CO75_GXPIX  768		// Gesamtbild
#define CO75_GYPIX  582
#define CO75_OXPIX   0		// Offset im Gesantbild
#define CO75_OYPIX   0
#define CO75_AOFFX   0		// Algorithmusoffset für Farbberechnung
#define CO75_AOFFY   0
#define CO75_PICOFF  0		// Byteverschiebung	 
#define CO75_FREQ	TAKT_075
#define CO75_VOFFX   0		// Offsetwerte video 
#define CO75_VOFFY   0
#define CO75_MOFFX   8		// Offsetwerte memory 
#define CO75_MOFFY   0


#define CO75A_TXPIX  944	// Taktregime der Kamera
#define CO75A_TYPIX  625
#define CO75A_XPIX   780
#define CO75A_YPIX   582
#define CO75A_GXPIX  784	// Gesamtbild
#define CO75A_GYPIX  583
#define CO75A_OXPIX   0		// Offset im Gesantbild
#define CO75A_OYPIX   0
#define CO75A_AOFFX   0		// Algorithmusoffset für Farbberechnung
#define CO75A_AOFFY   1
#define CO75A_PICOFF  0	 
#define CO75A_FREQ	TAKT_075A
#define CO75A_VOFFX   0		// Offsetwerte video 
#define CO75A_VOFFY   0
#define CO75A_MOFFX   8		// Offsetwerte memory 
#define CO75A_MOFFY   0


#define CO85_TXPIX  1616	// Taktregime der Kamera
#define CO85_TYPIX  1043
#define CO85_XPIX   1280
#define CO85_YPIX   1024
#define CO85_GXPIX  1356	// Gesamtbild
#define CO85_GYPIX  1034
#define CO85_OXPIX   4      // 28.1.00	// Offset im Gesantbild
#define CO85_OYPIX   4		// am 9.6. 99 geändert // 6
#define CO85_AOFFX   0		// Algorithmusoffset für Farbberechnung
#define CO85_AOFFY   0
#define CO85_PICOFF  0		// 2 24.M.	
#define CO85_FREQ	TAKT_085
#define CO85_VOFFX   40		// Offsetwerte video 
#define CO85_VOFFY   2
#define CO85_MOFFX   40		// Offsetwerte memory 
#define CO85_MOFFY   0

#define CO85A_TXPIX  1616	// Taktregime der Kamera
#define CO85A_TYPIX  1043
#define CO85A_XPIX   1300
#define CO85A_YPIX   1030
#define CO85A_GXPIX  1300	// Gesamtbild
#define CO85A_GYPIX  1032
#define CO85A_OXPIX   0     // Offset im Gesantbild
#define CO85A_OYPIX   0		// 
#define CO85A_AOFFX   0		// Algorithmusoffset für Farbberechnung
#define CO85A_AOFFY   1
#define CO85A_PICOFF  0		// 
#define CO85A_FREQ	TAKT_085
#define CO85A_VOFFX   40		// Offsetwerte video 
#define CO85A_VOFFY   2
#define CO85A_MOFFX   40		// Offsetwerte memory 
#define CO85A_MOFFY   0

#define CO32_TXPIX  2288	// Taktregime der Kamera
#define CO32_TYPIX   918
#define CO32_XPIX   2088
#define CO32_YPIX    776
#define CO32_GXPIX  2088	// Gesamtbild
#define CO32_GYPIX   776
#define CO32_OXPIX   0     // Offset im Gesantbild
#define CO32_OYPIX   0		// 
#define CO32_AOFFX   0		// Algorithmusoffset für Farbberechnung
#define CO32_AOFFY   1
#define CO32_PICOFF  0		// 
#define CO32_FREQ	TAKT_085
#define CO32_VOFFX   40		// Offsetwerte video 
#define CO32_VOFFY   2
#define CO32_MOFFX   40		// Offsetwerte memory 
#define CO32_MOFFY   0

#define CX131_TXPIX  1616	// Taktregime der Kamera
#define CX131_TYPIX  1043
#define CX131_XPIX   1300
#define CX131_YPIX   1030
#define CX131_GXPIX  1300	// Gesamtbild
#define CX131_GYPIX  1031
#define CX131_OXPIX   0     // Offset im Gesamtbild
#define CX131_OYPIX   0		// 
#define CX131_AOFFX   0		// Algorithmusoffset für Farbberechnung
#define CX131_AOFFY   1
#define CX131_PICOFF  0		// 
#define CX131_FREQ	TAKT_085
#define CX131_VOFFX   40		// Offsetwerte video 
#define CX131_VOFFY   2
#define CX131_MOFFX   40		// Offsetwerte memory 
#define CX131_MOFFY   0

#define CX1000_TXPIX  1616	// Taktregime der Kamera
#define CX1000_TYPIX  1043
#define CX1000_XPIX   1300
#define CX1000_YPIX   1031
#define CX1000_GXPIX  1300	// Gesamtbild
#define CX1000_GYPIX  1031
#define CX1000_OXPIX   0     // Offset im Gesamtbild
#define CX1000_OYPIX   0		// 
#define CX1000_AOFFX   0		// Algorithmusoffset für Farbberechnung
#define CX1000_AOFFY   1
#define CX1000_PICOFF  0		// 
#define CX1000_FREQ	TAKT_085
#define CX1000_VOFFX   40		// Offsetwerte video 
#define CX1000_VOFFY   2
#define CX1000_MOFFX   40		// Offsetwerte memory 
#define CX1000_MOFFY   0

#define COCM1_TXPIX  640	// Taktregime der Kamera
#define COCM1_TYPIX  480
#define COCM1_XPIX   640
#define COCM1_YPIX   480
#define COCM1_GXPIX  640	// Gesamtbild
#define COCM1_GYPIX  480
#define COCM1_OXPIX   0		// Offset im Gesantbild
#define COCM1_OYPIX   1
#define COCM1_PICOFF  0	 
#define COCM1_FREQ	TAKT_CM1
#define COCM1_VOFFX   0		// Offsetwerte video 
#define COCM1_VOFFY   0
#define COCM1_MOFFX   0		// Offsetwerte memory 
#define COCM1_MOFFY   0


// /////////////////////////
// Interface Definitionen
// /////////////////////////
//
// MATRIX 085
//
// ////////////////////////

// Dreh mich um
#define SWAP8( s )  (	(s&0x80)>>7 | (s&0x40)>>5 |	(s&0x20)>>3 | (s&0x10)>>1 | \
						(s&0x08)<<1 | (s&0x04)<<3 | (s&0x02)<<5 | (s&0x01)<<7   ) 


#define SWAP16( s ) ((	(s&0x8000)>>15 | (s&0x4000)>>13 | (s&0x2000)>>11 | (s&0x1000)>>9  | \
						(s&0x0800)>>7  | (s&0x0400)>>5  | (s&0x0200)>>3  | (s&0x0100)>>1  | \
						(s&0x0080)<<1  | (s&0x0040)<<3  | (s&0x0020)<<5  | (s&0x0010)<<7  | \
						(s&0x0008)<<9  | (s&0x0004)<<11 | (s&0x0002)<<13 | (s&0x0001)<<15  ))

// Geraete - Adressmodi
#define	C85_CXD	0x01	//Shutter
#define C85_MAX 0x06	//Offset/Verstärkung
#define C85_CST 0x02	//Kameraoptionen 
#define C85_CWN 0x03	//Fensteroption

#define C85_SHV( type, datum ) (  (type<<14) | (datum<<3) | C85_CXD )

// Shuttermode
#define C85_SLS( datum ) ( (0x1<<14) | (datum<<3) | (C85_CXD) )
#define C85_SHS( datum ) ( (0x2<<14) | (datum<<3) | (C85_CXD) )
#define C85_SNO( datum ) ( (0x2<<14) | (  0x1<<3) | (C85_CXD) )

// E2Potmode
#define C85_OFF( datum ) ( ((SWAP8(datum))<<8) | (1<<6)  | C85_MAX	)
#define C85_AMP( datum ) ( ((SWAP8(datum))<<8) | (1<<7)  | C85_MAX	)

// Statusmode 	
#define C85_STM( datum ) ( ((datum)<<3) | C85_CST )


// Fenstermode
#define C85_WNDX( datum) ( (SWAP16(datum/4 ) & 0xff80 ) | C85_CWN )
#define C85_WNDY( datum) ( (SWAP16(datum   ) & 0xffe0 ) | C85_CWN )


// Zeit in Sekunden		--> für 1/x s ( 1 / C85_SHS_TIME( pos) ) 
// C85_SHS_TIME liefert die echte zeit in s !
//              um den wert 1 / x ( wie 1 / 50 ) zu bekommen Kehrwert !!!
#define	C85_SHS_TIME( d )   ( (double)(CO85_TXPIX*d + 510)/CO85_FREQ )
#define	C85_SLS_TIME( d )   ( (double)(CO85_TXPIX*CO85_TYPIX*d)/CO85_FREQ )

// Wert als x von 1/x [s]
#define	C85_SHS_VALUE( d )	( (double)(CO85_FREQ/d  - 510.0 ) / CO85_TXPIX )
// Wert Sekunden [s]
#define	C85_SLS_VALUE( d )	( (double)d*CO85_FREQ/(CO85_TXPIX*CO85_TYPIX))  
					

#define C85_SHS_MIN	  2	
#define C85_SHS_MAX   1042

#define C85_SLS_MIN	  2	
#define C85_SLS_MAX   120

#define C85_AMP_MIN    31
#define C85_AMP_MAX   213

#define C85_OFF_MIN    31
#define C85_OFF_MAX   213

// /////////////////////////
//
// MATRIX 075
//
// ////////////////////////

// Shuttermode
#define C75_SLS( datum ) ( (1<<11) | (0<<10) | (1<<9) | (datum & 0x1ff)  )
#define C75_SHS( datum ) ( (1<<11) | (1<<10) | (0<<9) | (datum & 0x1ff)  )
#define C75_SNO( datum ) ( (1<<11) | (1<<10) | (1<<9)  )
#define C75_FLL( datum ) ( (1<<11) | (0<<10) | (0<<9)  )
										 // (1<<11)  damit e2pot sich nicht verstellt
// Zeit in Sekunden		--> für 1/x s ( 1 / C85_SHS_VALUE ) 
#define	C75_SHS_TIME( d )   (		(   ((double)(d)-199)   *64.0 + 35.6    )/1000000.0 		)
#define	C75_SLS_TIME( d )   ( (double)(CO85_TXPIX*CO85_TYPIX*d)/CO85_FREQ )

// Wert als x von 1/x [s]
#define	C75_SHS_VALUE( d )	( (double)( (1000000.0/(double)d -35.6)/64.0 + 199 ))
// Wert Frames []
#define	C75_SLS_VALUE( d )	( (double)0x1ff - (double)d/2.0 )  

#define C75_SHS_MIN	  0xc8
#define C75_SHS_MAX   0x1ff

#define C75_SLS_MIN	  2	
#define C75_SLS_MAX   510					


// /////////////////////////
//
// MATRIX 075A
//
// ////////////////////////

// Shuttermode
#define C75A_SLS( datum ) ( (1<<11) | (0<<10) | (1<<9) | (datum & 0x1ff)  )
#define C75A_SHS( datum ) ( (1<<11) | (1<<10) | (0<<9) | (datum & 0x1ff)  )
#define C75A_SNO( datum ) ( (1<<11) | (1<<10) | (1<<9)  )
#define C75A_FLL( datum ) ( (1<<11) | (0<<10) | (0<<9)  )
										 // (1<<11)  damit e2pot sich nicht verstellt
// Zeit in Sekunden		--> für 1/x s ( 1 / C85_SHS_VALUE ) 
#define	C75A_SHS_TIME( d )   (		(   ((double)(d)-199)   *64.0 + 35.6    )/1000000.0 		)
#define	C75A_SLS_TIME( d )   ( (double)(CO75A_TXPIX*CO75A_TYPIX*d)/CO75A_FREQ )

// Wert als x von 1/x [s]
#define	C75A_SHS_VALUE( d )	( (double)( (1000000.0/(double)d -35.6)/64.0 + 199 ))
// Wert Frames []
#define	C75A_SLS_VALUE( d )	( (double)0x1ff - (double)d/2.0 )  

#define C75A_SHS_MIN   0xc8
#define C75A_SHS_MAX   0x1ff

#define C75A_SLS_MIN   2	
#define C75A_SLS_MAX   510					

#define C75A_AMP_STD    0x120
#define C75A_AMP_MIN    0x120
#define C75A_AMP_MAX    0x320

#define C75A_OFF_L1     0		//16 lsb
#define C75A_OFF_L2     1		//32 lsb
#define C75A_OFF_L3     2		//48 lsb
#define C75A_OFF_L4     3		//64 lsb


#define C75A_OFF_MIN    C75A_OFF_L1
#define C75A_OFF_MAX    C75A_OFF_L4


// /////////////////////////
//
// MATRIX 085A
//
// ////////////////////////

#define C85A_OFF_L1     0		//16 lsb
#define C85A_OFF_L2     1		//32 lsb
#define C85A_OFF_L3     2		//48 lsb
#define C85A_OFF_L4     3		//64 lsb

#define C85A_OFF_MIN    C85A_OFF_L1
#define C85A_OFF_MAX    C85A_OFF_L4

#define	C85A_SHS_VALUE( d )	( (double)((double)d - 35.6)/ 110.0 )
#define	C85A_SLS_VALUE( d )	( (double)(double)d         / 115.0 )  

#define C85A_AMP_STD    0x180
#define C85A_AMP_MIN    0x180
#define C85A_AMP_MAX    0x320

#define C85A_SHS_MIN	  2	
#define C85A_SHS_MAX   1042

#define C85A_SLS_MIN	  2	
#define C85A_SLS_MAX   120



// /////////////////////////
//
// MATRIX 242 - C32C
//
// ////////////////////////

#define C32_OFF_MIN    0		// Lsb bezogen auf 12 Bit
#define C32_OFF_MAX    255		// Lsb bezogen auf 12 Bit 
#define C32_OFF_STD    0x80		// Lsb bezogen auf 12 Bit 

#define C32_AMP_STD    0x1A0
#define C32_AMP_MIN    0x1A0
#define C32_AMP_MAX    0x3FF

#define	C32_CGAIN_MIN   -32
#define	C32_CGAIN_MAX    31


// /////////////////////////
//
// MATRIX 085 - CX131(C)
//
// ////////////////////////

#define CX131_OFF_MIN    0		// Lsb bezogen auf 12 Bit
#define CX131_OFF_MAX    255	// Lsb bezogen auf 12 Bit 
#define CX131_OFF_STD    0x80	// Lsb bezogen auf 12 Bit 

#define CX131_AMP_STD    0x200
#define CX131_AMP_MIN    0x200
#define CX131_AMP_MAX    0x3FF

#define	CX131_CGAIN_MIN   -32
#define	CX131_CGAIN_MAX    31

#define CX131_SHS_MIN	   2	
#define CX131_SHS_MAX   1042

#define CX131_SLS_MIN	  2	
#define CX131_SLS_MAX   520

// /////////////////////////
//
// MATRIX 085 - CX1000(C)
//
// ////////////////////////

#define CX1000_OFF_MIN    0		// Lsb bezogen auf 12 Bit
#define CX1000_OFF_MAX    255	// Lsb bezogen auf 12 Bit 
#define CX1000_OFF_STD    0x80	// Lsb bezogen auf 12 Bit 

#define CX1000_AMP_STD    0x140
#define CX1000_AMP_MIN    0x140
#define CX1000_AMP_MAX    0x3FF

#define	CX1000_CGAIN_MIN   -32
#define	CX1000_CGAIN_MAX    31

#define CX1000_SHS_MIN	   2	
#define CX1000_SHS_MAX   1042

#define CX1000_SLS_MIN	  2	
#define CX1000_SLS_MAX   520


// /////////////////////////
// E2-Pot's
// /////////////////////////

#define BYNO		0
#define BYINCR		1
#define BYVALUE		2
#define BYLEVEL     3

#define	E2PMIN		0
#define E2PMAX		100	


// /////////////////////////
// Control-Interface
// /////////////////////////

#define	NOCANAL		  0
#define	DEBIPORT	  1
#define CENTRONIC	  2
#define SERIALCOM	  3
#define SERIALCOMBYDP 4

// /////////////////////////
// Hauptspeicherformat 16 bit
// /////////////////////////
typedef union
{	
	struct {
		unsigned char pix1;	// Drehung ist erfolgt um mit dem 085-Modul-kompatibel zu sein
		unsigned char pix0;
	} p8;
	struct {
		unsigned short pix0;
	} p10;
    unsigned short pix0;
}	MATPIXEL, *PMATPIXEL; 

#endif 