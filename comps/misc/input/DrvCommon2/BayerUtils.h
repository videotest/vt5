#pragma once

#include "ImageUtil.h"


class CDeadPxlMask
{
	CString MakeMaskName(LPCTSTR lpstrMask);
	int m_nThreshold;
	bool m_bVideoMode;
public:
	CDib m_Dib;
	CDeadPxlMask();
	void Empty() {m_Dib.Empty();}
	bool IsEmpty() { return m_Dib.IsEmpty();}
	int cx() {return m_Dib.cx();}
	int cy() {return m_Dib.cy();}
	void ReadPxlMask(LPCTSTR lpstrMask = NULL);
	void WritePxlMask(LPCTSTR lpstrMask = NULL);
	void InitMask(LPBITMAPINFOHEADER lpbi);
	void FreeMask();
	void CorrectBayerDib(LPBITMAPINFOHEADER lpbiImg, LPBYTE lpData, int nBitCount, int xOffset, int yOffset);
	void SetVideoMode(bool bVideoMode);
};