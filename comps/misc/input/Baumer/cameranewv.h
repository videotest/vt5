#ifndef _CAMERANEWVERSION_H_
#define _CAMERANEWVERSION_H_

// Kamergeräteadressen /////////////////////////////
#define CREG75A_VERS    0x00
#define CREG75A_AOFS1   0x01
#define CREG75A_AGAIN   0x02
#define CREG75A_TSHUT   0x03
#define CREG75A_CONFIG	0x04	 // Config 

// Shuttermodis
#define CSMD75A_HS		0x0000
#define CSMD75A_LS		0x1000
#define CSMD75A_AS		0x2000

// Moderegister der Kamera
#define CRMD75A_TRGM	  0x0001	// Triggermode
#define CRMD75A_08BIT	  0x0000	// Datenbreiten
#define CRMD75A_10BIT	  0x0002	
#define CRMD75A_12BIT	  0x0004	
#define CRMD75A_16BIT	  0x0006
#define CRMD75A_BINHV	  0x0008	// Binning
#define CRMD75A_SYNS1	  0x0010	// Syncsource
#define CRMD75A_SYNS2	  0x0020	
#define CRMD75A_INVS1	  0x0040	// Invertieren Sync
#define CRMD75A_INVS2	  0x0080	
#define CRMD75A_SWTS1	  0x0100	// Softtrigger
#define CRMD75A_SWTS2	  0x0200	
#define CRMD75A_TSS1	  0x0000	// Triggersource für Kamera
#define CRMD75A_TSS2	  0x0400	
#define CRMD75A_TSTP	  0x0800	
#define CRMD75A_TSCP2	  0x0C00	
#define CRMD75A_INVFLUSH  0x1000	// invert flushsignal 
#define CRMD75A_TPATTERN  0x2000	// Testmuster
#define CRMD75A_FASTCOM   0x4000	// schnelle Baudrate 	 

// Grenzwerte für Fast-Shutter
#define CFAST75A_MIN	  0x0001
#define CFAST75A_MAX	  0x00FF

#define CRMD75A_FASTV( v ) ( (v) <<8 )

// Kameraversionen
#define	CHWTYP_UNDEFINED  0xFFFF

#define	CTYP75A_COMPAR	  0x0101    // altes define
#define	CHWTYP_CX05C	  0x0101
#define	CHWTYP_CX05	      0x2101
#define	CHWTYP_CX13C	  0x0201
#define	CHWTYP_CX13	      0x2201
#define	CHWTYP_CX13CF	  0x0202
#define	CHWTYP_CX13F	  0x2202

#define CHWTYP_DC100	  CHWTYP_UNDEFINED
#define CHWTYP_DC150	  CHWTYP_UNDEFINED
#define CHWTYP_DC200	  CHWTYP_UNDEFINED
#define CHWTYP_DC250	  CHWTYP_UNDEFINED
#define CHWTYP_DC300	  0x0401	
#define CHWTYP_DC300F     0x0501
#define CHWTYP_DC350F     0x2501

#define	CHWTYP_ARC1000	  0x2141
#define	CHWTYP_ARC1000C	  0x0141
#define	CHWTYP_ARC1800	  0x2241
#define	CHWTYP_ARC1800C   0x0241
#define	CHWTYP_ARC4000	  0x2541
#define	CHWTYP_ARC4000C	  0x0541
#define	CHWTYP_ARC6000C	  0x0441

#define	CHWTYP_CX32C	  0x0401
#define	CHWTYP_DXM33	  0x0402
#define	CHWTYP_CX131C	  0x0501

#define	CHWTYP_CX131	  0x2501
#define	CHWTYP_CX1000	  0x8221
#define	CHWTYP_CX13QVC	  0x0601
#define	CHWTYP_CX13QV	  0x2601
#define	CHWTYP_CX13QV1C	  0x0701
#define	CHWTYP_CX13QV1	  0x2701

#define	CHWTYP_SIPLACEIX13 0x8201

#define	CHWTYP_CDR90      0x8001

#endif _CAMERANEWVERSION_H_
