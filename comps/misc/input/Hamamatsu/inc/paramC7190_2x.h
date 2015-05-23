// paramC7190_2x.h
// [Oct.16,2001]

#ifndef	_INCLUDE_PARAMC7190_2X_H_
#define	_INCLUDE_PARAMC7190_2X_H_

struct DCAM_PARAM_C7190_2X {
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_C7190

	long	CEG;	// Contrast Enhance Gain
	long	CEO;	// Contrast Enhance Offset
	long	HVC;	// HV control
	long	HVN;	// HV sencitivity 
	long	REC;	// recur control
	long	RCN;	// recur N
	long	BGC;	// back grand control
	long	BGO;	// back grand offset
	long	CAI_H;	// horizontal
	long	CAI_V;	// vertical
	long	CAI_A;	// bits per pixel
};

enum {
	dcamparam_c7190_2x_CEG		= 0x00000001,
	dcamparam_c7190_2x_CEO		= 0x00000002,
	dcamparam_c7190_2x_HVC		= 0x00000004,
	dcamparam_c7190_2x_HVN		= 0x00000008,
	dcamparam_c7190_2x_REC		= 0x00000010,
	dcamparam_c7190_2x_RCN		= 0x00000020,
	dcamparam_c7190_2x_BGC		= 0x00000040,
	dcamparam_c7190_2x_BGO		= 0x00000080,
	dcamparam_c7190_2x_BGS		= 0x00000100,
	dcamparam_c7190_2x_CAI_H	= 0x00000200,
	dcamparam_c7190_2x_CAI_V	= 0x00000400,
	dcamparam_c7190_2x_CAI_A	= 0x00000800,
};

#endif // _INCLUDE_PARAMC7300_2X_H_