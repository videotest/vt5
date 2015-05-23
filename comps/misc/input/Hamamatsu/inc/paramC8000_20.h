// paramC8000_20.h
// [Oct.16,2001]

#ifndef	_INCLUDE_PARAMC8000_20_H_
#define	_INCLUDE_PARAMC8000_20_H_

struct DCAM_PARAM_C8000_20 {
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_C8000

	long	CEG;	// Contrast Enhance Gain
	long	CEO;	// Contrast Enhance Offset
	long	REC;	// recur control
	long	RCN;	// recur N
	long	BGC;	// back grand control
	long	BGO;	// back grand offset
	long	CAI_H;	// horizontal
	long	CAI_V;	// vertical
	long	CAI_A;	// bits per pixel
};

enum {
	dcamparam_c8000_20_CEG		= 0x00000001,
	dcamparam_c8000_20_CEO		= 0x00000002,
	dcamparam_c8000_20_REC		= 0x00000010,
	dcamparam_c8000_20_RCN		= 0x00000020,
	dcamparam_c8000_20_BGC		= 0x00000040,
	dcamparam_c8000_20_BGO		= 0x00000080,
	dcamparam_c8000_20_BGS		= 0x00000100,
	dcamparam_c8000_20_CAI_H	= 0x00000200,
	dcamparam_c8000_20_CAI_V	= 0x00000400,
	dcamparam_c8000_20_CAI_A	= 0x00000800,
};

#endif // _INCLUDE_PARAMC8000_20_H_