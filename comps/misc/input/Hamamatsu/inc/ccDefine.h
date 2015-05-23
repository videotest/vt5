// ccDefine.h
//

// 2.1.00	[Nov.21,2000]	public.
// 2.1.02	[Jan.30,2000]	modified ID prefix to DCAM_IDxxxx
// 2.1.1	[Feb.16,2001]	removed #pack pragma(1) because there are no structures here and it is irrelevant
//							also, removed ccCapture_Focus and changed ccCapture_Snap = TRUE and ccCapture_Sequence = FALSE
//							added cc_success because this is the default state of LastError when initialized
//							added ccErr_notstable which is needed for dcam_precapture()
// 2.1.1.02 [Mar.12,2001]	defined ccDatatype,ccBitstype,ccCaptureMode,ccTriggerMode as typedef enum's
// 2.1.1.03 [Mar.16,2001]	removed ccTriggerMode because we will use DCAM_TRIGMODE_xxxx in dcamapi.h instead
//							removed cc_success and renamed it to ccErr_none.
// 2.1.2.00 [Sep.25,2001]	removed unused error code
// 2.1.2.01 [Sep.26,2001]	added error codes used by DC1394OH module
// 2.1.3.0	[Feb.28,2002]	Removed braces around constants so that they can be used in switch...case() statements easily.

#ifndef	_INCLUDE_CCDEFINE_H_
#define	_INCLUDE_CCDEFINE_H_

// ----------------------------------------------------------------

	const long	ccErr_none				=	0			;	// no error
	const long	ccErr_busy				=	0x80000101	;	// busy, cannot process
	const long	ccErr_abort				=	0x80000102	;	// abort process
	const long	ccErr_notready			=	0x80000103	;	// not ready state
	const long	ccErr_notstable			=	0x80000104	;	// not stable state
	const long	ccErr_timeout			=	0x80000106	;	// timeout
	const long	ccErr_notbusy			=	0x80000107	;	// not busy state

	const long	ccErr_nomemory			=	0x80000203	;	// not enough memory

	const long	ccErr_unknownmsgid		=	0x80000801	;	// unknown message id
	const long	ccErr_unknownstrid		=	0x80000802	;	// unknown string id
	const long	ccErr_unknownparamid	=	0x80000803	;	// unkown parameter id
	const long	ccErr_unknownbitstype	=	0x80000804	;	// unknown transfer type for setbits()
	const long	ccErr_unknowndatatype	=	0x80000805	;	// unknown transfer type for setdata
	const long	ccErr_invalidhandle		=	0x80000807	;	// invalid camera handle
	const long	ccErr_invalidparam		=	0x80000808	;	// invalid parameter

	const long	ccErr_unreach			=	0x80000f01	;	// reach any point must not to run
	const long	ccErr_notimplement		=	0x80000f02	;	// not yet implementation
	const long	ccErr_notsupport		=	0x80000f03	;	// this driver or camera is not support

	const long	ccErr_unstable			=	0x80000105	;	// now unstable state
	const long	ccErr_noresource		=	0x80000201	;	// not enough resource without memory and freespace of disk
//	const long	ccErr_diskfull			=	0x80000202	;	// not enough freespace of disk
	const long	ccErr_nomodule			=	0x80000204	;	// no sub module 
	const long	ccErr_nodriver			=	0x80000205	;	// no driver
	const long	ccErr_nocamera			=	0x80000206	;	// no camera
	const long	ccErr_unknowncamera		=	0x80000806	;	// unknown camera
	const long	ccErr_failopen			=	0x80001001	;
	const long	ccErr_failopenbus		=	0x81001001	;
	const long	ccErr_failopencamera	=	0x82001001	;
	const long	ccErr_failreadcamera	=	0x83001002	;
	const long	ccErr_failwritecamera	=	0x83001003	;

// ----------------------------------------------------------------

typedef enum ccDatatype {
	ccDatatype_none		=	0,
	ccDatatype_uint8	=	0x00000001,
	ccDatatype_uint16	=	0x00000002,
	ccDatatype_uint32	=	0x00000008,
	ccDatatype_int8		=	0x00000010,
	ccDatatype_int16	=	0x00000020,
	ccDatatype_int32	=	0x00000080,
	ccDatatype_real32	=	0x00000100,
	ccDatatype_real64	=	0x00000200,
	ccDatatype_rgb8		=	0x00010000,
	ccDatatype_rgb16	=	0x00020000,
	ccDatatype_rgb24	=	0x00040000,	//? 8bit, [ r0, g0, b0], [r1, g1, b1]
	ccDatatype_rgb32	=	0x00080000,
	ccDatatype_rgb48	=	0x00100000,

	// just like 1394 format, Y is unsigned, U and V are signed.
	// about U and V, signal level is from -128 to 128, data value is from 0x00 to 0xFF
	ccDatatype_yuv411  =	0x01000000,	// 8bit, [ u0, y0, y1, v0, y2, y3 ], [u4, y4, y5, v4, v6, y7],
	ccDatatype_yuv422  =	0x02000000,	// 8bit, [ u0, y0, v0, y1 ], [u2, y2, v2, v3 ],
	ccDatatype_yuv444  =	0x04000000, // 8bit, [ u0, y0, v0 ], [ u1, y1, v1 ],

	ccDatatype_all		=	0x071F03bb,
} ccDatatype;

typedef enum ccBitstype {
	ccBitstype_none		=	0x00000000,
	ccBitstype_index8	=	0x00000001,
	ccBitstype_rgb16	=	0x00000002,
	ccBitstype_rgb24	=	0x00000004,	// 8bit, [ b0, g0, r0]
	ccBitstype_rgb32	=	0x00000008,

	ccBitstype_all		=	0x0000000F,
} ccBitstype;

// ================================================================

// precapture mode
typedef enum {
	ccCapture_Snap		= 0,
	ccCapture_Sequence,
} ccCaptureMode;

#endif // _INCLUDE_CCDEFINE_H_
