// portx.h
// [Oct.3.2001]

#ifndef	_INCLUDE_PORTX_H_
#define	_INCLUDE_PORTX_H_

// ================================================================
// extended port

// port message IDs

enum {
	DCAMPORT_IDMSG_SEND			= 0x0300,	//	DCAMPORT_SEND*	param, bytesize = sizeof( DCAMPORT_SEND);
	DCAMPORT_IDMSG_STARTWATCH	= 0x0301,	//	DCAMPORT_PARAM_WATCH*	param, bytesize = sizeof( DCAMPORT_PARAM_WATCH);
	DCAMPORT_IDMSG_STOPWATCH	= 0x0302,	//	DCAMPORT_PARAM_WATCH*	param, bytesize = sizeof( DCAMPORT_PARAM_WATCH);
};


enum {
	DCAMPORT_IDREASON_DESTROY	= 0x00000001,
	DCAMPORT_IDREASON_CLOSE		= 0x00000002,
	DCAMPORT_IDREASON_RECEIVE	= 0x00000004,
	DCAMPORT_IDREASON_SEND		= 0x00000008,
};

typedef void WINAPI DCAMPORT_CALLBACK(
//	HDCAM		hCam,
	DWORD		iReason,
	const void*	buffer,
	DWORD		bufsize,
	LPVOID		param
);

typedef struct _DCAMPORT_PARAM_SEND
	{
		LPVOID	buffer;
		DWORD	bufsize;
		DWORD	reserved;
	} DCAMPORT_PARAM_SEND;

typedef struct _DCAMPORT_PARAM_WATCH
	{
		DWORD				dwFlag;
		HANDLE				hAvailable;
		DCAMPORT_CALLBACK*	proc;
		LPVOID				param;
	} DCAMPORT_PARAM_WATCH;

// ================================================================

#endif // _INCLUDE_PORTX_H_