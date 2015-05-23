// Command Definitions to control CCx camera

// all commands are 16 bit
// the MSB decides the commands destination:
//	0 :	command is meant for an interface box  (type Fisba Dental basestation)
//	1:	ultimate destination is a camera, a possible interface box may also decode the information but must retransmit the
//		command further on to the camera


// MSB == 0
#define    B_CFGDATA	0x0000
//   =   B"00000000XXXXXXXX" ;
#define    B_UARTDATA	0x0100
//   =   B"00000001XXXXXXXX" ;

// immediate commands not entering the fifo ...
#define    B_SHORT        0x7fc0
//   =   B"0111111111XXXXXX" ;   // general placeholder
#define    B_NOP          0x7fff
//   =   B"0111111111111111" ;   // to just read the status back
#define    B_CONFIG       0x7ffe
//   =   B"0111111111111110" ;   // config pulse to camera


// MSB == 1
// general definitions for windows of intrest
#define    WOI_YSTART      0x8000
//   =   B"1000XXXXXXXXXXXX" ;
#define    WOI_YEND        0x9000
//   =   B"1001XXXXXXXXXXXX" ;
#define    WOI_XSTART      0xa000
//   =   B"1010XXXXXXXXXXXX" ;
#define    WOI_XEND        0xb000
//   =   B"1011XXXXXXXXXXXX" ;
#define    WOI_YINC        0xc000
//   =   B"11000000XXXXXXXX" ;
#define    WOI_XINC        0xd000
//   =   B"11010000XXXXXXXX" ;

// lag0..3 are used to set the  integration time when using integrating sensors
// this 32 bit value should reflect the units used for timing e.g. in case of the Ibis4b: lines, for PVS clock
// counts ?
#define    C_LAG0          0xe000
//   =   B"11100000XXXXXXXX" ;
#define    C_LAG1          0xe100
//   =   B"11100001XXXXXXXX" ;
#define    C_LAG2          0xe200
//   =   B"11100010XXXXXXXX" ;
#define    C_LAG3          0xe300
//   =   B"11100011XXXXXXXX" ;
// gain and offset are Ibis4b parameters but will probably also be used with other sensors
#define   C_GAIN          0xe400
//   =   B"11100100XXXXXXXX" ;
#define   C_OFFSET        0xe500
//   =   B"11100101XXXXXXXX" ;
// control bits always prove to be very useful
//#define  C_NYU           0xe600
//   =   B"11100110XXXXXXXX" ;
#define   C_MISC          0xe700
#define		CC_SINGLESHOT		0x80
#define		CC_DIAGNOSTIC		0x40
#define		CC_DIAG_Y			0x20
#define		CC_DIAG_XXORY		0x10
//select sync on ad15
#define		CC_SYNC1_EOF		0x00
#define		CC_SYNC1_BOF	  	0x01
#define		CC_SYNC1_EOL	  	0x02
#define		CC_SYNC1_BOL	  	0x03
//select sync on ad13
#define		CC_SYNC2_EOF	  	0x00
#define		CC_SYNC2_BOF	  	0x04
#define		CC_SYNC2_EOL	  	0x08
#define		CC_SYNC2_BOL	  	0x0c
//definitions for Fuga1000
#define		CC_F1000_SHY		0x80

#define   C_ANAVAL0      0xe800
//   =   B"11101000XXXXXXXX" ;
#define   C_ANAVAL1      0xe900
//   =   B"11101001XXXXXXXX" ;
#define   C_ANAVAL2      0xea00
//   =   B"11101010XXXXXXXX" ;
#define   C_ANAVAL3      0xeb00
//   =   B"11101011XXXXXXXX" ;
#define   CSO_YDELAY     0xec00
//   =   B"11101100XXXXXXXX" ;
#define   CSO_XDELAY     0xed00
//   =   B"11101101XXXXXXXX" ;
#define   CSO_XDELAY2    0xee00
//   =   B"11101110XXXXXXXX" ;
#define	CSO_ADCDELAY   0xef00
//   =   B"11101111XXXXXXXX" ;
#define  	C_ANASEL		0xf000
//   =   B"11110000XXXXXXXX" ;
#define  	CSO_TDELAY     0xf100
//   =   B"11110001XXXXXXXX" ;
#define  	CSO_PWM	     0xf200
//   =   B"11110010XXXXXXXX" ;
#define  	C_LINK	     0xf300
//   =   B"11110011XXXXXXXX" ;
#define  I2CADDR	     0xf400
//   =   B"11110100XXXXXXXX" ;
#define  I2CDATA	     0xf500
//   =   B"11110101XXXXXXXX" ;
#define  C_LRATE0	     0xf600
//   =   B"11110110XXXXXXXX" ;
#define  C_LRATE1	     0xf700
//   =   B"11110111XXXXXXXX" ;
#define  C_LRATE2	     0xf800
//   =   B"11111000XXXXXXXX" ;
#define  C_LRATE3	     0xf900
//   =   B"11111001XXXXXXXX" ;


// start commands are not immediate and can be placed as marker between sets of data
#define    C_START       0xff80
//   =   B"1111111110000000" ;   // immediate start
#define    C_SYNCSTART   0xff81
//   =   B"1111111110000001" ;   // waits for sync input either via ttl of direct command
#define    C_WAIT        0xff82
//   =   B"1111111110000010" ;   // waits for vertical frame ended ...
#define    C_START_WAIT  0xff83
//   =   B"1111111110000011" ;   // starts a frame and waits for vertical frame ended ...
#define    C_SINGLE		0xff84
//   =   B"1111111110000100" ;   // single frame start
#define    C_SYNCCONT	0xff85
//   =   B"1111111110000101" ;   // continuous triggerred mode

#define    C_CAL		0xff88
//   =   B"1111111110001000" ;   // continuous triggerred mode



// immediate commands not entering the fifo ...
#define    CC_SHORT        0xffc0
//   =   B"1111111111XXXXXX" ;   // general placeholder
#define    CC_NOP          0xffff
//   =   B"1111111111111111" ;   // to just read the status back
#define    CC_SYNC         0xfffe
//   =   B"1111111111111110" ;   // see above : software controlled trigger
#define    CC_RESET        0xfffd
//   =   B"1111111111111101" ;   // resets about everything ( incl fifos )
#define    CC_ABORT        0xfffc
//   =   B"1111111111111100" ;   // ends an acquisition in progress
#define    I2CRDREQ        0xfffb
//   =   B"1111111111111011" ;   // ends an acquisition in progress
#define    I2CWRREQ        0xfffa
//   =   B"1111111111111010" ;   // ends an acquisition in progress
#define    I2CSTATRESET    0xfff9
//   =   B"1111111111111001" ;   // ends an acquisition in progress



#define    CC_CTRL         0xffc0
//   =   B"111111111100XXXX" ;   // direct control bits
#define	 CC_CTRL_AUTORESTART		0x0E
#define	 CC_CTRL_DOUBLECLOCKING		0x06