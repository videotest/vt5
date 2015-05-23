#if !defined(__MorphInfo_H__)
#define __MorphInfo_H__


class CMorphInfo
{
public:
	BITMAPINFOHEADER *m_lpbmih;
	BYTE	*m_pBuf1;
	BYTE	*m_pBuf2;
	BYTE	*m_pBuf3;
	BYTE	*m_pimage;

	int		Frame_x1;
	int		Frame_x2;
	int		Frame_y1;
	int		Frame_y2;
	int		iCol4;
	int		m_iCol;
	int		m_iRow;
	int		m_iFactor;

public:
	CMorphInfo(BITMAPINFOHEADER *pbi)
	{
		m_lpbmih = pbi;
		m_pimage = (BYTE*)(pbi+1)+1024;

		m_pBuf1 = new BYTE[m_lpbmih->biWidth+4];
		m_pBuf2 = new BYTE[m_lpbmih->biWidth+4];
		m_pBuf3 = new BYTE[m_lpbmih->biWidth+4];

		Frame_x1 = 0;
		Frame_x2 = m_lpbmih->biWidth;
		Frame_y1 = 0;
		Frame_y2 = m_lpbmih->biHeight;

		iCol4 = (m_lpbmih->biWidth+3)/4*4;
		m_iCol = m_lpbmih->biWidth;
		m_iRow = m_lpbmih->biHeight;

		m_iFactor = 0;
	}
	~CMorphInfo()
	{
		delete []m_pBuf1;
		delete []m_pBuf2;
		delete []m_pBuf3;
	}
};

#endif
