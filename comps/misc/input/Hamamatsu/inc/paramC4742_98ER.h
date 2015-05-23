//	paramC4742_98ER.h

struct	DCAM_PARAM_C4742_98ER {
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_C4742_98ER

	long	LMD;		// light mode	(L/H)
	long	ESC;		// external trigger source connector	(B/D/I)
	double	RAT;		// read real acquire time
};

enum {
	dcamparam_c4742_98ER_LMD	= 0x00000001,
	dcamparam_c4742_98ER_ESC	= 0x00000002,
	dcamparam_c4742_98ER_RAT	= 0x00000010,
};
