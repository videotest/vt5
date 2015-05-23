// paramC4742_98BT.h
// [Oct.3,2001]

#ifndef	_INCLUDE_PARAMC4742_98BT_H_
#define	_INCLUDE_PARAMC4742_98BT_H_

struct DCAM_PARAM_C4742_98BT {
	DCAM_HDR_PARAM	hdr;		// id == DCAM_PARAMID_C4742_95BT

	long	ACT;	// (Image / Dark image / shutter Open) shutter action

	long	AMD;	// (Normal / External) trigger mode
	long	EMD;	// (Edge / Level) external trigger mode

	long	SMD;	// (Normal / Super Pixel / sub-Array) scan mode

	double	AET;	// exposure time
	long	ACN;	// number of acquire count
	long	ATP;	// (Posi / Nega) tigger polarity -> EMD E

	long	SSP;	// (High / Slow) scan speed -> read out bit ?CAI S or ?CAI I
	long	SFD;	// (On / oFf) front dummy

	long	SPX;	// (1/2/4/8) super pixel -> SMD S

	long	SVO;	// sub array v-offset -> SMD A
	long	SVW;	// sub array v-witdh  -> SMD A

	long	SHO;	// sub array h-offset -> SMD A
	long	SHW;	// sub array h-witdh  -> SMD A

	long	CEG;	// (0-255) at [SSP H] Contrast Enhance Gain
					// 0 Super High D-range mode   slow scan
					// 1       High D-range mode   slow scan
					// 2 High sensitivety mode     slow scan

	long	CEO;	// (0-255) at [SSP H] Contrast Enhance Offset

	long	CSW;	// (On / oFf) cooler control switch

	long	CAI_I;
	long	CAI_S;
};


enum {
	dcamparam_c4742_98BT_ACT	= 0x00000001,

	dcamparam_c4742_98BT_AMD	= 0x00000002,
	dcamparam_c4742_98BT_EMD	= 0x00000004,
	dcamparam_c4742_98BT_SMD	= 0x00000008,

	dcamparam_c4742_98BT_AET	= 0x00000010,
	dcamparam_c4742_98BT_ACN	= 0x00000020,
	dcamparam_c4742_98BT_ATP	= 0x00000040,

	dcamparam_c4742_98BT_SSP	= 0x00000080,
	dcamparam_c4742_98BT_SFD	= 0x00000100,
	dcamparam_c4742_98BT_SPX	= 0x00000200,

	dcamparam_c4742_98BT_SVO	= 0x00000400,
	dcamparam_c4742_98BT_SVW	= 0x00000800,
	dcamparam_c4742_98BT_SHO	= 0x00001000,
	dcamparam_c4742_98BT_SHW	= 0x00002000,

	dcamparam_c4742_98BT_CEG	= 0x00004000,
	dcamparam_c4742_98BT_CEO	= 0x00008000,

	dcamparam_c4742_98BT_CSW	= 0x00010000,

	dcamparam_c4742_98BT_CAI_I	= 0x00020000,
	dcamparam_c4742_98BT_CAI_S	= 0x00040000,

};

enum {
	// parameter
	c4742_98BT_kUNIT_Shutter	= 0,
	c4742_98BT_kUNIT_USec,
	c4742_98BT_kUNIT_MSec,
	c4742_98BT_kUNIT_Sec,
	c4742_98BT_kUNIT_Min,

	c4742_98BT_kSFD_On			= 0,
	c4742_98BT_kSFD_Off,

	c4742_98BT_kATP_Negative	= 0,
	c4742_98BT_kATP_Positive,

	c4742_98BT_kSPX_1			= 0,
	c4742_98BT_kSPX_2,
	c4742_98BT_kSPX_4,
	c4742_98BT_kSPX_8,

	c4742_98BT_K_kSVO_unit		= 8,
	c4742_98BT_K_kSVO_min		= 0,
	c4742_98BT_K_kSVO_max		= 1016,

	c4742_98BT_K_kSVW_unit		= 8,
	c4742_98BT_K_kSVW_min		= 8,
	c4742_98BT_K_kSVW_max		= 1024,

	c4742_98BT_K_kSHO_unit		= 8,
	c4742_98BT_K_kSHO_min		= 0,
	c4742_98BT_K_kSHO_max		= 1016,

	c4742_98BT_K_kSHW_unit		= 8,
	c4742_98BT_K_kSHW_min		= 8,
	c4742_98BT_K_kSHW_max		= 1024,

	c4742_98BT_L_kSVO_unit		= 8,
	c4742_98BT_L_kSVO_min		= 0,
	c4742_98BT_L_kSVO_max		= 504,

	c4742_98BT_L_kSVW_unit		= 8,
	c4742_98BT_L_kSVW_min		= 8,
	c4742_98BT_L_kSVW_max		= 512,

	c4742_98BT_L_kSHO_unit		= 8,
	c4742_98BT_L_kSHO_min		= 0,
	c4742_98BT_L_kSHO_max		= 504,

	c4742_98BT_L_kSHW_unit		= 8,
	c4742_98BT_L_kSHW_min		= 8,
	c4742_98BT_L_kSHW_max		= 512,

	c4742_98BT_kCSW_On			= 0,
	c4742_98BT_kCSW_Off,
};

#endif // _INCLUDE_PARAMC4742_98BT_H_