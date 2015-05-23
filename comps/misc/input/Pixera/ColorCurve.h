#if !defined(__ColorCurve_H__)
#define __ColorCurve_H__

class CColorCurve
{
	BYTE *m_pbRed;
	BYTE *m_pbGreen;
	BYTE *m_pbBlue;
public:
	CColorCurve()
	{
		m_pbRed = NULL;
		m_pbGreen = NULL;
		m_pbBlue = NULL;
		m_bCurve = false;
	}
	~CColorCurve()
	{
		delete m_pbRed;
		delete m_pbGreen;
		delete m_pbBlue;
	}
	bool m_bCurve;
	void Load();
	void Convert(LPBITMAPINFOHEADER lpbi);
};






#endif
