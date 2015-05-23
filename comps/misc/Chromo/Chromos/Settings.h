#if !defined(__Settings_H__)
#define __Settings_H__

#include "ChrUtils.h"


// Appearance settings values
extern CIntIniValue g_ShowAxisCmere;
extern CIntIniValue g_ShowSkelAxis;
extern CIntIniValue g_ShowNodes;
extern CColorIniValue g_AxisColor;
extern CColorIniValue g_CentromereColor;
// Algorythm
extern CIntIniValue g_CalcMethod;
extern CIntIniValue g_ShortAxisAlg; // 0 - as VT4, 1 - as Piper
extern CIntIniValue g_RotateMode; // Only if g_ShortAxisAlg==1, if 1 - simulate error during rotate
extern CIntIniValue g_PtsDist; // Only if g_ShortAxisAlg==0, 0 -auto, 1- length/10
extern CIntIniValue g_EndsAlg; // Only if g_ShortAxisAlg==0
extern CIntIniValue g_AxisCorrect; // Only if g_ShortAxisAlg==0
extern CIntIniValue g_AxisIterations; // Only if g_ShortAxisAlg==0 and g_AxisCorrect==2
extern CIntIniValue g_AxisCorrectPrimEnds; // Only if g_ShortAxisAlg==0
extern CIntIniValue g_AxisCorrectSecEnds; // Only if g_ShortAxisAlg==0
extern CIntIniValue g_AxisCorrectSkelEnds; // Only if g_ShortAxisAlg==0
extern CIntIniValue g_CentromereAlg; // 0 - as VT4, 1 - modernized, 2 - as Piper
extern CIntIniValue g_AxisCurveRecognition;
extern CIntIniValue g_FloatArith;
extern CIntIniValue g_MaxChromoWidth;
// Debug
extern CIntIniValue g_Debug;
// Other settings
extern CIntIniValue g_BlackOnWhite;
// CGH
extern CIntIniValue g_CGHPane; // Pane in CGH image wich will be used in calculations


void InitSettings();
void DeinitSettings();

class CChromoSettings
{
public:
	CChromoSettings()
	{
		InitSettings();
	}
	~CChromoSettings()
	{
		DeinitSettings();
	}
};


#endif

