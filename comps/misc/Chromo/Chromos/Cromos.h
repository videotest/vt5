// Cromos.h : main header file for the CROMOS DLL
//

#if !defined(AFX_CROMOS_H__05EF5A8E_7DD7_11D4_8336_85DC8E570373__INCLUDED_)
#define AFX_CROMOS_H__05EF5A8E_7DD7_11D4_8336_85DC8E570373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "ChrUtils.h"

/////////////////////////////////////////////////////////////////////////////
// CCromosApp
// See Cromos.cpp for the implementation of this class
//

class CCromosApp : public CWinApp
{
public:
	CCromosApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCromosApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCromosApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// Appearance settings values
extern CIntIniValue g_ShowAxisCmere;
extern CIntIniValue g_ShowSkelAxis;
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
// Other settings
extern CIntIniValue g_BlackOnWhite;

void InitSettings();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CROMOS_H__05EF5A8E_7DD7_11D4_8336_85DC8E570373__INCLUDED_)
