#if !defined(__VTAviInfo_H__)
#define __VTAviInfo_H__

#define streamtypeVTVI         mmioFOURCC('V', 'T', 'V', 'I')

struct VTVideoInfo
{
	char szLabel[21]; // "Videotest video info"
	int nVer;
	DWORD dwOrigRate;
	DWORD dwOrigScale;
};

void ConvertAVIFile(const char *pszAVIFileName, DWORD dwNewRate, DWORD dwNewScale);

#endif
