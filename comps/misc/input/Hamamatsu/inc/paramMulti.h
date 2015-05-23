// paramMulit.h
// [Dec.18.2001]	add software trigger

#ifndef	_INCLUDE_PARAMMULTI_H_
#define	_INCLUDE_PARAMMULTI_H_

struct DCAM_PARAM_MULTI {
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_MULTI
	
	long	bValid;
};

enum {
	dcamparam_multi_setvalid	= 0x00000001,
};


#endif // _INCLUDE_PARAMMULTI_H_




