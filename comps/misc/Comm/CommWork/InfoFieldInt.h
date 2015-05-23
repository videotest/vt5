#if !defined(__InfoFieldInt_H__)
#define __InfoFieldInt_H__

#define MAX_MICRO_NAME 50

struct InfoFieldData
{
	char szMicroName[MAX_MICRO_NAME];
	SYSTEMTIME localTime;
	unsigned nVoltageKV;
	unsigned nZoom;
	double dMarkerNm;
};

interface IInfoField : public IUnknown
{
	com_call SetData(InfoFieldData ifd) = 0;
	com_call GetData(InfoFieldData *p) = 0;
	com_call CalcSize(SIZE *pSize) = 0;
};

declare_interface(IInfoField, "5FC31E0B-BD66-4B3A-B151-93438009656F" );

#define szTypeInfoFieldObject       "InfoFieldObject"
#define szInfoFieldObjectProgId     "InfoFieldObject"
#define szInfoFieldObjectInfoProgId "InfoFieldObjectInfo"


#endif