#if !defined(__Equalize_H__)
#define __Equalize_H__


_bstr_t _GetCCName(IImage *pimg1);
int _GetPanesCount(IImage *pimg1);

class CEqualize
{
public:
	CArray<double,double&> m_BorderValues;
	int m_nPanes;
	_bstr_t m_bstrCC;
	int m_nHeight,m_nWidth;
	bool m_bContours;
	IImage3Ptr m_pImgOld;
	POINT m_ptOffs;
	CEqualize(CImageWrp &pImgOld, bool bNotEqualize = false)
	{
		CalcBorderInfoValues(pImgOld,bNotEqualize);
	}
	bool IsContours(IImage3 *pImgOld)
	{
		int nContours;
		pImgOld->GetContoursCount(&nContours);
		return nContours > 0;
	}
	void CalcBorderInfoValues(CImageWrp &pImgOld, bool bNotEqualize)
	{
		// Sizes of images, offset and row mask will be copied from old image
		pImgOld->GetOffset(&m_ptOffs);
		m_bstrCC = pImgOld.GetColorConvertor();
		m_nPanes = pImgOld.GetColorsCount();
		pImgOld->GetSize(&m_nWidth,&m_nHeight);
		m_pImgOld = pImgOld;
		m_bContours = bNotEqualize?false:IsContours(pImgOld);
		if (!m_bContours)
			return;
		m_BorderValues.SetSize(m_nPanes);
		for (int nPane = 0; nPane < m_nPanes; nPane++)
			m_BorderValues[nPane] = 0.;
		Contour *pCntr;
		pImgOld->GetContour(0, &pCntr);
		int nPoints = 0;
		for (int i = 0; i < pCntr->nContourSize; i++)
		{
			ContourPoint pt = pCntr->ppoints[i];
			pt.x = (short)(pt.x-m_ptOffs.x);
			pt.y = (short)(pt.y-m_ptOffs.y);
			for (int nPane = 0; nPane < m_nPanes; nPane++)
			{
				color *pdata;
				pImgOld->GetRow(pt.y, nPane, &pdata);
				if (pt.y >= 0 && pt.y < m_nHeight && pt.x >= 0 && pt.x < m_nWidth)
				{
					m_BorderValues[nPane] = m_BorderValues[nPane]+pdata[pt.x];
					nPoints++;
				}
			}
		}
		if (nPoints > 0)
			for (int nPane = 0; nPane < m_nPanes; nPane++)
			{
				m_BorderValues[nPane] /= nPoints;
				if (m_BorderValues[nPane] < 0.)
					m_BorderValues[nPane] = 0.;
				if (m_BorderValues[nPane] > 0xFFFF)
					m_BorderValues[nPane] = 0xFFFF;
			}
	}
	void Equalize(IImage3 *pImgNew, IImage3 *pImgOld, int nPane)
	{
		int y,x;
		long i;
		const long lHistSize = 0x10000;
		DWORD dwHist[lHistSize];
		DWORD dwVmin, dwHistSum;
		color bBufTabl[lHistSize];
		DWORD dwPoints = 1;
		memset(&dwHist[0], 0, lHistSize*sizeof(DWORD) );
		memset(&bBufTabl[0], 0, lHistSize*sizeof(color));
		for (y = 0; y < m_nHeight; y++)
		{
			color *psrc;
			pImgOld->GetRow(y, nPane, &psrc);
			BYTE *psrcmsk;
			pImgOld->GetRowMask(y, &psrcmsk);
			for (x = 0; x < m_nWidth; x++)
			{
				if (psrcmsk[x])
				{
					dwHist[psrc[x]]++;
					dwPoints++;
				}
			}
		}
		long nMax = 0xFFFF;
		long nMin = 0;
		DWORD b = 0;
		for (i = 0; i < lHistSize; i++)
		{
			nMin = i;
			b += dwHist[i];
			if (b > dwPoints/50)
				break;
		}
		b = 0;
		for (i = lHistSize-1; i >= 0; i++)
		{
			nMax = i;
			b += dwHist[i];
			if (b > dwPoints/50)
				break;
		}
		dwVmin = dwHist[0];
		for (i = 0; i < lHistSize; i++)
		{
			if (dwHist[i] > 0)
			{
				dwVmin = dwHist[i];
				break;
			}
		}
		bBufTabl[0] = 0;
		dwHistSum = dwHist[0];
		for (i = 1; i < lHistSize; i++)
		{
			dwHistSum += dwHist[i];
			bBufTabl[i] = (color)((dwHistSum - dwVmin)*(lHistSize - 1)/(dwPoints - dwVmin));
		}
		DWORD pix;
		if (nMax-m_BorderValues[nPane]<m_BorderValues[nPane]-nMin)
		{
			for (y = 0; y < m_nHeight; y++)
			{
				color *psrc,*pdst;
				pImgOld->GetRow(y, nPane, &psrc);
				pImgNew->GetRow(y, nPane, &pdst);
				BYTE *psrcmsk;
				pImgOld->GetRowMask(y, &psrcmsk);
				for (x = 0; x < m_nWidth; x++)
				{
//					if (psrcmsk[x])
					{
						pix = (DWORD)(sqrt((double)bBufTabl[psrc[x]])*12.6*16.);
						if (pix >= lHistSize)
							pix = lHistSize-1;
						pdst[x] = (color)pix;
					}
				}
			}
		}
		else
		{
			for (y = 0; y < m_nHeight; y++)
			{
				color *psrc,*pdst;
				pImgOld->GetRow(y, nPane, &psrc);
				pImgNew->GetRow(y, nPane, &pdst);
				BYTE *psrcmsk;
				pImgOld->GetRowMask(y, &psrcmsk);
				for (x = 0; x < m_nWidth; x++)
				{
//					if (psrcmsk[x])
					{
						pix = (DWORD)((lHistSize-1-sqrt((double)(lHistSize-1-bBufTabl[psrc[x]]))
							*15.85*16.)*0.8)+50*lHistSize/256;
						if (pix >= lHistSize)
							pix = lHistSize-1;
						pdst[x] = (color)pix;
					}
				}
			}
		}
	}
	IImage3Ptr ProcessImage()
	{
		// Create new image
		IUnknownPtr punkImgNew(::CreateTypedObject(_bstr_t("Image")), false);
		IImage3Ptr pImgNew(punkImgNew);
		pImgNew->CreateImage(m_nWidth, m_nHeight, m_bstrCC, m_nPanes );
		pImgNew->SetOffset(m_ptOffs, FALSE);
		// Copy rows and row masks
		pImgNew->InitRowMasks();
		if (m_bContours)
		{
			for (int nPane = 0; nPane < m_nPanes; nPane++)
				Equalize(pImgNew, m_pImgOld, nPane);
		}
		else
		{
			// Copy image data from old (every pane)
			for (int y = 0; y < m_nHeight; y++)
			{
				for (int nPane = 0; nPane < m_nPanes; nPane++)
				{
					color *psrc,*pdst;
					m_pImgOld->GetRow(y, nPane, &psrc);
					pImgNew->GetRow(y, nPane, &pdst);
					memcpy(pdst,psrc,m_nWidth*sizeof(color));
				}
			}
		}
		for (int y = 0; y < m_nHeight; y++)
		{
			// Copy row mask from old
			BYTE *psrcmsk,*pdstmsk;
			m_pImgOld->GetRowMask(y, &psrcmsk);
			pImgNew->GetRowMask(y, &pdstmsk);
			memcpy(pdstmsk,psrcmsk,m_nWidth*sizeof(BYTE));
		}
		// Contours will be created
		pImgNew->InitContours();
		return pImgNew;
	}
	IImage3Ptr Process(IUnknown *ptr)
	{
		IImage3Ptr pImgNew;
		IMeasureObjectPtr ptrMO(ptr);
		if (ptrMO != 0)
		{
			pImgNew = ProcessImage();
			ptrMO->SetImage(pImgNew);
		}
		return pImgNew;
	}
};

#endif

