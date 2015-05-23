// parmIQV50PCI.h

#if !defined(PARMIQV50PCI__INCLUDED_)
#define PARMIQV50PCI__INCLUDED_

enum
{
	//	video
	IQV50_kSync_Video			= 1,
	IQV50_kSync_Sync,

	IQV50_kVideo_NTSC_Inter		= 1,
	IQV50_kVideo_NTSC_NonInter,
	IQV50_kVideo_CCIR_Inter,
	IQV50_kVideo_CCIR_NonInter,

	IQV50_kTransfer_field		= 1,
	IQV50_kTransfer_frame,

	//	condition
	IQV50_kExtTrig_off			= 1,
	IQV50_kExtTrig_on,

	IQV50_kOperate_Integrate	= 1,
	IQV50_kOperate_Addition,
	IQV50_kOperate_SubInteg,
	IQV50_kOperate_Transfer,

	IQV50_kBit_Auto				= 1,
	IQV50_kBit_0_7,
	IQV50_kBit_1_8,
	IQV50_kBit_2_9,
	IQV50_kBit_Non,

	IQV50_kVideoChannel_Min		= 0,
	IQV50_kVideoChannel_Max		= 3,
	IQV50_kVideoChannel_Def		= IQV50_kVideoChannel_Min,

	IQV50_kIntegNum_Min			= 1,
	IQV50_kIntegNum_Max			= 4096,
	IQV50_kIntegNum_Def			= IQV50_kIntegNum_Min,

	IQV50_kLUTChannel_Min		= 0,
	IQV50_kLUTChannel_Max		= 7,
	IQV50_kLUTChannel_Def		= IQV50_kLUTChannel_Min,

	//	image info
	IQV50_kType_NTSC			= 1,
	IQV50_kType_CCIR,

	IQV50_kStatus_OK			= 0,
	IQV50_kStatus_Over,
	IQV50_kStatus_Under,

	IQV50_kImage_Odd			= 0,
	IQV50_kImage_Even,

	// action
	IQV50_kAct_Initialize		= 0,
	IQV50_kAct_VideoCheck,

};


enum {
	dcamparam_iqv50_sync			= 0x00000001,
	dcamparam_iqv50_video			= 0x00000002,
	dcamparam_iqv50_trig			= 0x00000004,
	dcamparam_iqv50_operate			= 0x00000008,
	dcamparam_iqv50_bit				= 0x00000010,
	dcamparam_iqv50_transfer		= 0x00000020,
	dcamparam_iqv50_videoChannel	= 0x00000040,
	dcamparam_iqv50_integNumber		= 0x00000080,
	dcamparam_iqv50_lutChannel		= 0x00000100,

	dcamparam_iqv50_pllData			= 0x00000200,

};

enum {
	dcamparam_iqv50lut_lutChannel		= 0x00000001,
	dcamparam_iqv50lut_lutData			= 0x00000002,
};

enum {
	dcamparam_iqv50stat_imageStatus		= 0x00000001,
	dcamparam_iqv50stat_imageBit		= 0x00000002,
	dcamparam_iqv50stat_imageStart		= 0x00000004,
	dcamparam_iqv50stat_imageNum		= 0x00000008,
	dcamparam_iqv50stat_boardType		= 0x00000010,
};

struct DCAM_PARAM_IQV50 {
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_IQV50

	long	sync,
			video,
			trig,
			operate,
			bit,
			transfer,
			videoChannel,
			integNumber,
			lutChannel,
			pllData [10];
};

struct DCAM_PARAM_IQV50_LUT {	// DCAM_LUT_IQV50 
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_IQV50_LUT

	long	lutChannel,
			lutData [1024];
};

struct DCAM_PARAM_IQV50_STAT {	// DCAM_STAT_IQV50 
	DCAM_HDR_PARAM	hdr;		// id == DCAM_IDPARAM_IQV50_STAT

	long	imageStatus,
			imageBit,
			imageStart,
			imageNum,
			boardType;
};

#endif // PARMIQV50PCI__INCLUDED_
