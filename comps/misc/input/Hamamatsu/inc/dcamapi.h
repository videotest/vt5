/* **************************************************************** *
	dcamapi.h

2.1.2.00 [Sep. 25,2001]
2.1.2.01 [Oct. 29,2001]
2.1.2.02 [Oct. 30,2001]
2.1.2.03 [Oct. 31,2001]
2.1.2.04 [Jan. 10,2002] binning
2.1.3.00 [Jan. 26,2002]
		1. Add dcam_firetrigger() function
		2. Add DCAM_QUERYCAPABILITY_EVENTS
		3. Add DCAM_EVENT_VVALIDBEGIN
		4. Add DCAM_TRIGMODE_SOFTWARE
2.1.3.01 [Jan. 29,2002]
		1. Add DCAM_CAPABILITY_TRIGGER_SOFTWARE
2.1.3.02 [Feb. 19,2002]
		1. Add DCAM_CAPABILITY_TRIGGER_PIV
		2. Add DCAM_TRIGMODE_PIV
2.1.3.03 [March 7,2002]
		1. Fixed DCAM_CAPABILITY_ALL to correct value

 * **************************************************************** */

#ifndef _INCLUDE_DCAMAPI_H_

/* ================================================================ *
	defines
 * ---------------------------------------------------------------- */

#ifdef __cplusplus

extern "C" {
/* C */

#define	OPTION = 0

#else
/* C++ */

#define	OPTION

#endif

#ifdef WIN32
/* Win32 platform */

#ifndef DCAMAPI
/* API based on PASCAL calling */
#define	DCAMAPI	PASCAL

#endif

#else
/* Other platform */

#define	DCAMAPI

typedef	Boolean			BOOL;
typedef	unsigned char	BYTE;
typedef	unsigned short	WORD;
typedef	unsigned long	DWORD;
typedef	unsigned long	UINT;
typedef	const char*		LPCSTR;
typedef	void*			LPVOID;
typedef	Handle			HINSTANCE;
typedef	Size			SIZE;
typedef	Handle			HANDLE;
typedef	WindowPtr		HWND;

#endif /* WIN32 */

/* include standard constants, declare types, define global constant */

#include "ccDefine.h"

typedef struct tag_dcam* HDCAM;

#ifndef DCAMAPI_VER
#define	DCAMAPI_VER		2130
#endif

/* ================================================================ *
	functions
 * ---------------------------------------------------------------- */

/*** --- error function --- ***/

long DCAMAPI dcam_getlasterror			( HDCAM h, char* buf OPTION, DWORD bytesize OPTION);

/*** --- initialize and finalize --- ***/

BOOL DCAMAPI dcam_init					( HINSTANCE hInst, long* pCount OPTION, LPCSTR reserved OPTION );
BOOL DCAMAPI dcam_uninit				( HINSTANCE hInst, LPCSTR reserved OPTION );
BOOL DCAMAPI dcam_getmodelinfo			( long index, long dwStringID, char* buf, DWORD bytesize );

BOOL DCAMAPI dcam_open					( HDCAM* ph, long index, LPCSTR reserved OPTION );
BOOL DCAMAPI dcam_close					( HDCAM h);

/*** --- camera infomation --- ***/

BOOL DCAMAPI dcam_getstring				( HDCAM h, long dwStringID, char* buf, DWORD bytesize );
BOOL DCAMAPI dcam_getcapability			( HDCAM h, DWORD* pCapability, DWORD dwCapTypeID );

BOOL DCAMAPI dcam_getdatatype			( HDCAM h, ccDatatype* pType );
BOOL DCAMAPI dcam_getbitstype			( HDCAM h, ccBitstype* pType );
BOOL DCAMAPI dcam_setdatatype			( HDCAM h, ccDatatype type );
BOOL DCAMAPI dcam_setbitstype			( HDCAM h, ccBitstype type );

BOOL DCAMAPI dcam_getdatasize			( HDCAM h, SIZE* pSize );
BOOL DCAMAPI dcam_getbitssize			( HDCAM h, SIZE* pSize );

/*** --- parameters --- ***/

BOOL DCAMAPI dcam_queryupdate			( HDCAM h, DWORD* pFlag, DWORD dwReserved OPTION );

BOOL DCAMAPI dcam_getbinning			( HDCAM h, long* pBinning );
BOOL DCAMAPI dcam_getexposuretime		( HDCAM h, double* pSec );
BOOL DCAMAPI dcam_gettriggermode		( HDCAM h, long* pMode );
BOOL DCAMAPI dcam_gettriggerpolarity	( HDCAM h, long* pPolarity );

BOOL DCAMAPI dcam_setbinning			( HDCAM h, long binning );
BOOL DCAMAPI dcam_setexposuretime		( HDCAM h, double sec );
BOOL DCAMAPI dcam_settriggermode		( HDCAM h, long mode );
BOOL DCAMAPI dcam_settriggerpolarity	( HDCAM h, long polarity );

/*** --- capturing --- ***/

BOOL DCAMAPI dcam_precapture			( HDCAM h, ccCaptureMode mode );
BOOL DCAMAPI dcam_getdatarange			( HDCAM h, long* pMax, long* pMin);
BOOL DCAMAPI dcam_getdataframebytes		( HDCAM h, DWORD* pSize);

BOOL DCAMAPI dcam_allocframe			( HDCAM h, long frame );
BOOL DCAMAPI dcam_getframecount			( HDCAM h, long* pFrame );

BOOL DCAMAPI dcam_capture				( HDCAM h );
BOOL DCAMAPI dcam_idle					( HDCAM h );
BOOL DCAMAPI dcam_wait					( HDCAM h, DWORD* pCode, DWORD timeout OPTION, HANDLE event OPTION );
BOOL DCAMAPI dcam_getstatus				( HDCAM h, DWORD* pStatus );
BOOL DCAMAPI dcam_gettransferinfo		( HDCAM h, long* pNewestFrameIndex, long* pFrameCount );

BOOL DCAMAPI dcam_freeframe				( HDCAM h );

/*** --- user memory support --- ***/

BOOL DCAMAPI dcam_attachbuffer			( HDCAM h, void** pTop, DWORD size);
BOOL DCAMAPI dcam_releasebuffer			( HDCAM h);

/*** --- data transfer --- ***/

BOOL DCAMAPI dcam_lockdata				( HDCAM h, void** pTop, long* pRowbytes, long frame );
BOOL DCAMAPI dcam_lockbits				( HDCAM h, BYTE** pTop, long* pRowbytes, long frame );
BOOL DCAMAPI dcam_unlockdata			( HDCAM h );
BOOL DCAMAPI dcam_unlockbits			( HDCAM h );

/*** --- LUT --- ***/

BOOL DCAMAPI dcam_setbitsinputlutrange	( HDCAM h, long inMax, long inMin OPTION );
BOOL DCAMAPI dcam_setbitsoutputlutrange	( HDCAM h, BYTE outMax, BYTE outMin OPTION );

/*** --- Control Panel --- ***/

BOOL DCAMAPI dcam_showpanel				( HDCAM h, HWND hWnd, DWORD reserved OPTION );

/*** --- extended --- ***/

BOOL DCAMAPI dcam_extended				( HDCAM h, UINT iCmd, LPVOID param, DWORD size );


/*** --- software trigger --- ***/
BOOL DCAMAPI dcam_firetrigger			( HDCAM h );


/* ================================================================ *
	parameter constants
 * ---------------------------------------------------------------- */

/*** --- camera capability  --- ***/
enum {
	DCAM_QUERYCAPABILITY_FUNCTIONS		= 0,
	DCAM_QUERYCAPABILITY_DATATYPE		= 1,
	DCAM_QUERYCAPABILITY_BITSTYPE		= 2,
	DCAM_QUERYCAPABILITY_EVENTS			= 3,
};

enum {
	DCAM_CAPABILITY_BINNING2					= 0x00000002,
	DCAM_CAPABILITY_BINNING4					= 0x00000004,
	DCAM_CAPABILITY_BINNING8					= 0x00000008,
	DCAM_CAPABILITY_BINNING16					= 0x00000010,
	DCAM_CAPABILITY_BINNING32					= 0x00000020,
	DCAM_CAPABILITY_TRIGGER_EDGE				= 0x00000100,
	DCAM_CAPABILITY_TRIGGER_LEVEL				= 0x00000200,
	DCAM_CAPABILITY_TRIGGER_MULTISHOT_SENSITIVE = 0x00000400,
	DCAM_CAPABILITY_TRIGGER_CYCLE_DELAY			= 0x00000800,
	DCAM_CAPABILITY_TRIGGER_SOFTWARE			= 0x00001000,
	DCAM_CAPABILITY_TRIGGER_PIV					= 0x00002000,
	DCAM_CAPABILITY_TRIGGER_POSI				= 0x00010000,
	DCAM_CAPABILITY_TRIGGER_NEGA				= 0x00020000,
	
	/*** --- from 2.1.2 --- ***/
	DCAM_CAPABILITY_USERMEMORY					= 0x00100000,
	DCAM_CAPABILITY_RAWDATA						= 0x00200000,

	DCAM_CAPABILITY_ALL							= 0x00333F3E,
};

/*** --- status  --- ***/
enum {
	DCAM_STATUS_ERROR					= 0x0000,
	DCAM_STATUS_BUSY					= 0x0001,
	DCAM_STATUS_READY					= 0x0002,
	DCAM_STATUS_STABLE					= 0x0003,
	DCAM_STATUS_UNSTABLE				= 0x0004,
};

enum {
	DCAM_EVENT_FRAMESTART				= 0x0001,
	DCAM_EVENT_FRAMEEND					= 0x0002,	// all modules support
	DCAM_EVENT_CYCLEEND					= 0x0004,	// all modules support
	DCAM_EVENT_VVALIDBEGIN				= 0x0008,
};

/*** --- update  --- ***/
enum {
	DCAM_UPDATE_RESOLUTION				= 0x0001,
	DCAM_UPDATE_AREA					= 0x0002,
	DCAM_UPDATE_DATATYPE				= 0x0004,
	DCAM_UPDATE_BITSTYPE				= 0x0008,
	DCAM_UPDATE_EXPOSURE				= 0x0010,
	DCAM_UPDATE_TRIGGER					= 0x0020,
	DCAM_UPDATE_DATARANGE				= 0x0040,
	DCAM_UPDATE_DATAFRAMEBYTES			= 0x0080,

	DCAM_UPDATE_ALL						= 0x00ff,
};

/*** --- trigger mode --- ***/
enum {
	DCAM_TRIGMODE_INTERNAL				= 0x0001,
	DCAM_TRIGMODE_EDGE					= 0x0002,
	DCAM_TRIGMODE_LEVEL					= 0x0004,
	DCAM_TRIGMODE_MULTISHOT_SENSITIVE	= 0x0008,
	DCAM_TRIGMODE_CYCLE_DELAY			= 0x0010,
	DCAM_TRIGMODE_SOFTWARE				= 0x0020,
	DCAM_TRIGMODE_PIV					= 0x0040,
};

/*** --- trigger polarity --- ***/
enum {
	DCAM_TRIGPOL_NEGATIVE				= 0x0000,
	DCAM_TRIGPOL_POSITIVE				= 0x0001,
};

/*** --- string id --- ***/

#define	DCAM_IDSTR_BUS					0x04000101
#define	DCAM_IDSTR_CAMERAID				0x04000102
#define	DCAM_IDSTR_VENDOR				0x04000103
#define	DCAM_IDSTR_MODEL				0x04000104
#define	DCAM_IDSTR_CAMERAVERSION		0x04000105
#define	DCAM_IDSTR_DRIVERVERSION		0x04000106
#define	DCAM_IDSTR_MODULEVERSION		0x04000107
#define	DCAM_IDSTR_SPECIFICATIONVERSION	0x04000108
#define	DCAM_IDSTR_DCAMAPIVERSION		0x04000108

/* ================================================================ *
	for extended function
 * ---------------------------------------------------------------- */

/*** -- iCmd parameter of dcam_extended() -- ***/
enum {
    DCAM_IDMSG_QUERYPARAMCOUNT  = 0x0200,   /*       DWORD*          param,   bytesize = sizeof( DWORD );     */
    DCAM_IDMSG_SETPARAM         = 0x0201,   /* const DCAM_HDR_PARAM* param,   bytesize = sizeof( parameters); */
    DCAM_IDMSG_GETPARAM         = 0x0202,   /*       DCAM_HDR_PARAM* param,   bytesize = sizeof( parameters); */
    DCAM_IDMSG_QUERYPARAMID     = 0x0204,   /*       DWORD           param[], bytesize = sizeof( param);      */
};

/*** -- parameter header -- ***/
typedef struct _DCAM_HDR_PARAM {
	DWORD		cbSize;						/* size of whole structure */
	DWORD		id;							/* specify the kind of this structure */
	DWORD		iFlag;						/* specify the member to be set or requested by application */
	DWORD		oFlag;						/* specify the member to be set or gotten by module */
} DCAM_HDR_PARAM;

/* **************************************************************** */

#ifdef __cplusplus
/* end of extern "C" */
};
#endif

#define	_INCLUDE_DCAMAPI_H_
#endif
