//	paramC4742_98.h

struct	DCAM_PARAM_C4742_98 {
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_C4742_98

	long	AMD;		// acquire mode							(N/E)
	long	EMD;		// external exposure time setting mode	(E/L)
	double	AET;		// acquire exposure time				(sec)
	long	ATP;		// acquire trigger polarity				(P/N)

	long	SSP;		// scan speed		(H/S)
	long	SFD;		// scan front dummy	(O/F)
	char	SHA[3];		// scan h-area		(F/K/M/HC/HL/HR/QC/QL/QR/EC)
	long	SMD;		// scan mode		(N/S/A)
	long	SPX;		// super pixel		(1/2/4/8)
	long	SHO;		// scan h-offset	(0 - 1336 step8)
	long	SHW;		// scan h-width		(8 - 1344 step8)
	long	SVO;		// scan v-offset	(0 - 1016 step8)
	long	SVW;		// scan v-width		(8 - 1024 step8)

	long	CEG;		// contrast enhance gain	(SSP:S 0 - 2)	(SSP:H 0 - 255)
	long	CEO;		// contrast enhance offset	(SSP:S not use)	(SSP:H 0 - 255)

	long	CAI_I;		// high speed scan mode a/d bits
	long	CAI_S;		// slow speed scan mode a/d bits
	double	TMP;		// temperature
};


enum	{
	dcamparam_c4742_98_AMD		= 0x00000001,
	dcamparam_c4742_98_EMD		= 0x00000002,
	dcamparam_c4742_98_AET		= 0x00000004,
	dcamparam_c4742_98_ATP		= 0x00000008,

	dcamparam_c4742_98_SSP		= 0x00000010,
	dcamparam_c4742_98_SFD		= 0x00000020,
	dcamparam_c4742_98_SHA		= 0x00000040,
	dcamparam_c4742_98_SMD		= 0x00000080,
	dcamparam_c4742_98_SPX		= 0x00000100,
	dcamparam_c4742_98_SHO		= 0x00000200,
	dcamparam_c4742_98_SHW		= 0x00000400,
	dcamparam_c4742_98_SVO		= 0x00000800,
	dcamparam_c4742_98_SVW		= 0x00001000,

	dcamparam_c4742_98_CEG		= 0x00010000,
	dcamparam_c4742_98_CEO		= 0x00020000,

	dcamparam_c4742_98_CAI_I	= 0x00100000,
	dcamparam_c4742_98_CAI_S	= 0x00200000,
	dcamparam_c4742_98_TMP		= 0x00400000,
};

enum {
	c4742_98_kSHO_unit		= 8,
	c4742_98_kSHO_min		= 0,
	c4742_98_kSHO_max		= 1336,

	c4742_98_kSHW_unit		= 8,
	c4742_98_kSHW_min		= 8,
	c4742_98_kSHW_max		= 1344,

	c4742_98_kSVO_unit		= 8,
	c4742_98_kSVO_min		= 0,
	c4742_98_kSVO_max		= 1016,

	c4742_98_kSVW_unit		= 8,
	c4742_98_kSVW_min		= 8,
	c4742_98_kSVW_max		= 1024,
};
