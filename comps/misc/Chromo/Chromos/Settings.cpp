#include "StdAfx.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CIntIniValue g_ShowAxisCmere(_T("Chromos\\Appearance"), _T("ShowAxisAndCentromere"), 1, true);;
CIntIniValue g_ShowSkelAxis(_T("Chromos\\Appearance"), _T("ShowSkelAxisByDifferentColor"), 0, true);;
CIntIniValue g_ShowNodes(_T("Chromos\\Appearance"), _T("ShowShortAxisNodes"), 0, true);;
CColorIniValue g_AxisColor(_T("Chromos\\Appearance"), _T("AxisColor"), RGB(255, 128, 0));
CColorIniValue g_CentromereColor(_T("Chromos\\Appearance"), _T("CentromereColor"), RGB(0, 128, 255));

CIntIniValue g_CalcMethod(_T("Chromos"), _T("CalcMethod"), 1, true);
CIntIniValue g_BlackOnWhite(_T("Chromos"), _T("BlackOnWhite"), 0, true);
CIntIniValue g_ShortAxisAlg(_T("Chromos"), _T("ShortAxisAlg"), 0, true);
CIntIniValue g_RotateMode(_T("Chromos"), _T("RotateMode"), 0, true);


CIntIniValue g_PtsDist(_T("Chromos"), _T("PtsDist"), 1, true);
CIntIniValue g_EndsAlg(_T("Chromos"), _T("EndsAlgorythm"), 4, true);
CIntIniValue g_Debug(_T("Chromos"), _T("Debug"), 0, false);
CIntIniValue g_CentromereAlg(_T("Chromos"), _T("CentromereAlgorythm"), 0, true);
CIntIniValue g_AxisCorrect(_T("Chromos"), _T("AxisCorrection"), 2, true);
CIntIniValue g_AxisIterations(_T("Chromos"), _T("AxisSmoothIterations"), 2, true);
CIntIniValue g_AxisCorrectPrimEnds(_T("Chromos"), _T("CorrectPrimaryAxisEnds"), 1, true);
CIntIniValue g_AxisCorrectSecEnds(_T("Chromos"), _T("CorrectSecondaryAxisEnds"), 0, true);
CIntIniValue g_AxisCorrectSkelEnds(_T("Chromos"), _T("CorrectSkelAxisEnds"), 1, true);
CIntIniValue g_AxisCurveRecognition(_T("Chromos"), _T("CurveRecognition"), 1, true);
CIntIniValue g_FloatArith(_T("Chromos"), _T("FloatArithmetic"), 0, true);

CIntIniValue g_MaxChromoWidth(_T("Chromos"), _T("MaxChromoWidth"), 8, true);

CIntIniValue g_CGHPane(_T("Chromos"), _T("CGHPane"), 0, true);


static bool s_bSettingsInited = false;

void InitSettings()
{
	if (!s_bSettingsInited)
	{
		s_bSettingsInited = true;
		// Methods:
		// 1 - VT 4,
		// 2,6 - Piper,
		// 3,4 - modernized VT4
		// 5 - modernized Piper
		// 7 - float point algorithm
		if (g_CalcMethod == 1)
		{
			// VT4
			g_ShortAxisAlg = 0;
			g_PtsDist = 1;
			g_EndsAlg = 4;
			g_CentromereAlg = 0;
			g_AxisCorrect = 2;
			g_AxisCorrectPrimEnds = 1;
			g_AxisCorrectSecEnds = 0;
			g_AxisCorrectSkelEnds = 0;
			g_AxisCurveRecognition = 0;
		}
		else if (g_CalcMethod == 2 || g_CalcMethod == 6 || g_CalcMethod == 7)
		{
			// Piper's method
			g_ShortAxisAlg = 1;
			g_AxisCurveRecognition = 1;
			g_AxisIterations = 2;
			g_CentromereAlg = 2;
			g_RotateMode = 0;
		}
		else if (g_CalcMethod == 3)
		{
			// modernized VT4 with axis smoothing
			g_ShortAxisAlg = 0;
			g_PtsDist = 1;
			g_EndsAlg = 4;
			g_CentromereAlg = 1;
			g_AxisCorrect = 1;
			g_AxisIterations = 2;
			g_AxisCorrectPrimEnds = 1;
			g_AxisCorrectSecEnds = 0;
			g_AxisCorrectSkelEnds = 1;
			g_AxisCurveRecognition = 1;
		}
		else if (g_CalcMethod == 4)
		{
			// modernized VT4
			g_ShortAxisAlg = 0;
			g_PtsDist = 1;
			g_EndsAlg = 4;
			g_CentromereAlg = 1;
			g_AxisCorrect = 2;
			g_AxisCorrectPrimEnds = 1;
			g_AxisCorrectSecEnds = 0;
			g_AxisCorrectSkelEnds = 1;
			g_AxisCurveRecognition = 1;
		}
		else if (g_CalcMethod == 5 || g_CalcMethod == 8)
		{
			// modernized Piper's method
			g_ShortAxisAlg = 1;
			g_AxisCurveRecognition = 0;
			g_AxisIterations = 2;
			g_CentromereAlg = 2;
			g_RotateMode = 1;
		}
		if (g_CalcMethod > 0) g_FloatArith = g_CalcMethod >= 7 ? 1 : 0;
	}
}

void DeinitSettings()
{
	CIniValue::Clear();
	s_bSettingsInited = false;
}

