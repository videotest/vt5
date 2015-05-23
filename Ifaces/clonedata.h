#ifndef __clonedata_h__
#define __clonedata_h__


#define ICD_COPY_DEFAULT		0
#define ICD_COPY_MASK			1
#define ICD_COPY_CONTOURS		2
#define ICD_COPY_IMGDATA		4
#define ICD_COPY_PANE_SELECT	8	// [vanek] 12.12.2003: если данный флаг не установлен - копируются все паны,
									// в противном случае - только выбранные (enable)


//Image clone input parameter
struct ImageCloneData{
	DWORD	dwCopyFlags;
	RECT	rect;
	const char *szCCName;
};


#endif //__clonedata_h__