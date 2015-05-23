#if !defined(__Settings_H__)
#define __Settings_H__

#include "PixSDK.h"

const int nMinRect = 15;

struct PixRectData
{
	bool bDraw;
	PxRectEx rect;
	UCHAR r, g, b;
};

bool check(int &nVal, int nMin, int nMax);
void SaveRect(LPCTSTR sSec, bool bDraw, PxRectEx &rect, UCHAR r, UCHAR g, UCHAR b);
void RestoreRect(LPCTSTR sSec, bool &bDraw, PxRectEx &rect, UCHAR &r, UCHAR &g, UCHAR &b);
PixRectData RestoreRect(LPCTSTR sSec);
void RestoreDefaultRect(LPCTSTR sSec, bool &bDraw, PxRectEx &rect, UCHAR &r, UCHAR &g, UCHAR &b);
PixRectData RestoreDefaultRect(LPCTSTR sSec);
void SaveSettings();
void RestoreSettings();
void RestoreSettingsLite();

extern int g_nWBMode;
extern BOOL g_bGetImageFromPreview;

#endif