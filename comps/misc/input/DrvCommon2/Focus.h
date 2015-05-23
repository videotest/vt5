#pragma once

#include "CamValues.h"

extern CCamIntValue g_nRowsPer1Calc;
extern CCamIntValue g_CalcMethod;
extern CCamIntValue g_RGBPlane;
extern CCamBoolValue g_Focus;
extern CCamIntValue g_MaxFocus;
extern CCamIntValue g_FocusValue;

class CFocusRect : public CCamValueRect
{
public:
	CFocusRect();
	virtual bool NeedDrawRect();
};

extern CFocusRect g_FocusRect;

int CalcFocus(LPBITMAPINFOHEADER lpbi);