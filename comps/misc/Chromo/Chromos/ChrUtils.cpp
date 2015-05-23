#include "StdAfx.h"
#include "ChrUtils.h"
#include "PUtils.h"
#include "Woolz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

double check_angle(double a)
{
	if (a > PI)	a -= 2*PI;
	if (a < -PI) a += 2*PI;
	return a;
}


BOOL GetPointDir( CImageWrp& ImgWrp,  double oX, double oY,	 double fAngle, CPoint &pt )
{	
	double	dx, dy;
	int	iRow, iCol;//, iCol4;
//	BYTE	*pImage;
	color	ucPixel;
	BYTE* colStr;
	double X,Y;
	X=oX;
	Y=oY;
	iRow = ImgWrp.GetHeight();
	iCol = ImgWrp.GetWidth();
	dx =::cos(fAngle);
	dy =::sin(fAngle);
	if( X < 0 || Y < 0 || X >= iCol || Y >= iRow )
		return FALSE;
	pt.x = (long)X;
	pt.y = (long)Y;
	while (1)
	{                  
		X+=dx;         
		Y+=dy;
		if(X<0||Y<0||X>=iCol||Y>=iRow)
			return TRUE;
		colStr = ImgWrp.GetRowMask((int)Y);
		ucPixel = colStr[(int)X];
		if(ucPixel==0)	
			return TRUE;
		pt.x = (long)X;
		pt.y = (long)Y;
	}
	AfxMessageBox("Error GetPointDir ");
	return FALSE;
}

void SliceAxis(IChromoAxis2 *pAxis, CImageWrp &img)
{
	ASSERT((int)(pAxis->GetOffset().x)==img.GetOffset().x&&(int)(pAxis->GetOffset().y)==img.GetOffset().y);
	int nHeight = img.GetHeight();
	int nWidth = img.GetWidth();
	int nSize = pAxis->GetSize();
	int nFirst, nLast;
	if (pAxis->GetType() == FloatAxis)
	{
		FPOINT *fpt = (FPOINT *)pAxis->GetData();
		for (nFirst = 0; nFirst < nSize; nFirst++)
		{
			double x = fpt[nFirst].x;
			double y = fpt[nFirst].y;
			if (y >= 0 && y < nHeight && x >= 0 && x < nWidth && img.GetRowMask((int)y)[(int)x] > 0)
				break;
		}
		for (nLast = nSize-1; nLast > nFirst; nLast--)
		{
			double x = fpt[nLast].x;
			double y = fpt[nLast].y;
			if (y >= 0 && y < nHeight && x >= 0 && x < nWidth && img.GetRowMask((int)y)[(int)x] > 0)
				break;
		}
//		if (nFirst > 0) nFirst--;
//		if (nLast < nSize-1) nLast++;
		if (nFirst > 0 || nLast < nSize-1)
			pAxis->Shorten(nFirst, nLast+1);
	}
}


Contour *max_contour(CImageWrp &pImgWrp)
{
	int nContours = pImgWrp.GetContoursCount();
	if (nContours == 0)
		return NULL;
	else if (nContours == 1)
		return pImgWrp.GetContour(0);
	else
	{
		int nAreaMax = 0;
		Contour *pCntrMax = NULL;
		for (int i = 0; i < nContours; i++)
		{
			Contour *pCntr = pImgWrp.GetContour(i);
			CRect rc = ContourCalcRect(pCntr);
			int nArea = rc.Width()*rc.Height();
			if (nArea > nAreaMax)
			{
				nAreaMax = nArea;
				pCntrMax = pCntr;
			}
		}
		return pCntrMax;
	}
}

VARIANT _GetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry)
{
	sptrINamedData sptrND(GetAppUnknown());
	VARIANT var;
	::VariantInit(&var);
	CString sSecName(lpSection);
	CString sKeyName(lpEntry);
	sptrND->SetupSection( _bstr_t( (const char*)sSecName ) );
	sptrND->GetValue( _bstr_t( (const char*)sKeyName ), &var);
	return var;
};

void _SetProfileValue(LPCTSTR lpSection, LPCTSTR lpEntry, VARIANT var)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(lpSection);
	CString sKeyName(lpEntry);
	sptrND->SetupSection( _bstr_t( (const char*)sSecName ) );
	sptrND->SetValue( _bstr_t( (const char*)sKeyName ), var);
};

int _GetProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault, bool bSave)
{
	VARIANT var = _GetProfileValue(lpSection, lpEntry);
	int nResult;
	if (var.vt == VT_I2)
		nResult = var.iVal;
	else if (var.vt == VT_I4)
		nResult = var.lVal;
	else
	{
		nResult = nDefault;
		::VariantClear(&var);
		if (bSave)
		{
			var.vt = VT_I4;
			var.lVal = nDefault;
			_SetProfileValue(lpSection, lpEntry, var);
			::VariantClear(&var);
		}
	}
	return nResult;
}

void _WriteProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nValue)
{
	VARIANT var;
	var.vt = VT_I4;
	var.lVal = nValue;
	_SetProfileValue(lpSection, lpEntry, var);
	::VariantClear(&var);
}


CIniValue *CIniValue::s_pFirst = NULL;
CIniValue::CIniValue()
{
	m_pNext = s_pFirst;
	s_pFirst = this;
	m_dwLastUpdated=GetTickCount()-60000; //якобы апдейтили минуту назад
	m_nInit = 1;
}

void CIniValue::Clear()
{
	for (CIniValue *p = s_pFirst; p; p = p->m_pNext)
	{
		if (p->m_nInit == 0)
			p->m_nInit = 1;
	}
}

CIntIniValue::CIntIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, int nDefault, bool bSave)
{
	m_lpEntry = lpEntry;
	m_lpSection = lpSection;
	m_nValue = nDefault;
	m_nInit = bSave?2:1;
}

CIntIniValue::operator int()
{
	if (m_nInit == 0) return m_nValue;
	m_nValue = _GetProfileInt(m_lpSection, m_lpEntry, m_nValue, m_nInit==2);
	m_nInit = 0;
	return m_nValue;
}

int CIntIniValue::operator =(int nValue)
{
	int nRes = _GetProfileInt(m_lpSection, m_lpEntry, -1, false);
	if (nRes != -1 && nRes != nValue)
		_WriteProfileInt(m_lpSection, m_lpEntry, nValue);
	m_nInit = 0;
	m_nValue = nValue;
	return nValue;
}

void CIntIniValue::Save(int nValue)
{
	m_nInit = 0;
	m_nValue = nValue;
	_WriteProfileInt(m_lpSection, m_lpEntry, nValue);
}


CColorIniValue::CColorIniValue(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clrDefault)
{
	m_lpEntry = lpEntry;
	m_lpSection = lpSection;
	m_clrValue = clrDefault;
	m_nInit = 1;
}

CColorIniValue::operator COLORREF()
{
	if (m_nInit == 0 && (GetTickCount()-m_dwLastUpdated<200)) return m_clrValue; //если прошло менее 200mS - можно не обновлять
	m_clrValue = GetValueColor(GetAppUnknown(), m_lpSection, m_lpEntry, m_clrValue);
	m_dwLastUpdated=GetTickCount(); //время последнего апдейта
	m_nInit = 0;
	return m_clrValue;
}

int CColorIniValue::operator =(COLORREF clrValue)
{
	SetValueColor(GetAppUnknown(), m_lpSection, m_lpEntry, clrValue);
	m_nInit = 0;
	m_clrValue = clrValue;
	return clrValue;
}

CIntervalImage::CIntervalImage()
{
	nrows = 0;
	prows = NULL;
	szInt.cx = 0;
	szInt.cy = 0;
	ptOffset.x = ptOffset.y = 0;
}

CIntervalImage::~CIntervalImage()
{
	Free();
}

void CIntervalImage::Free()
{
	if (prows)
	{
		for (int y = 0; y < nrows; y++)
			delete prows[y].pints;
		delete prows;
	}
	prows = NULL;
	nrows = 0;
}

void CIntervalImage::MakeByImage(CImageWrp& ImgWrp)
{
	CPoint ptOffs = ImgWrp.GetOffset();
	int nHeight = ImgWrp.GetHeight();
	int nWidth = ImgWrp.GetWidth();
	int x,y,ymin=nHeight-1,ymax=0;
	Free();
	// First, find number of rows and offset.
	for (y = 0; y < nHeight; y++)
	{
		byte *pRowMask = ImgWrp.GetRowMask(y);
		bool bFound = false;
		for (x = 0; x < nWidth; x++)
			if (pRowMask[x])
			{
				bFound = true;
				break;
			}
		if (bFound)
		{
			if (y < ymin)
				ymin = y;
			if (y > ymax)
				ymax = y;
		}
	}
	if (ymin > ymax)
		return;
	// Second, allocate intervals.
	ptOffset.x = ptOffs.x;
	ptOffset.y = ptOffs.y+ymin;
	szInt.cx = nWidth;
	nrows = ymax-ymin+1;
	szInt.cy = nrows;
	prows = new INTERVALROW[nrows];
	for (y = 0; y < nrows; y++)
	{
		CArray<INTERVAL,INTERVAL&> arr;
		INTERVAL intvl;
		bool bInterval = false;
		byte *pRowMask = ImgWrp.GetRowMask(y+ymin);
		for (x = 0; x < nWidth; x++)
		{
			if (bInterval)
			{
				if (!pRowMask[x])
				{
					intvl.right = x-1;
					arr.Add(intvl);
					bInterval = false;
				}
			}
			else
			{
				if (pRowMask[x])
				{
					intvl.left = x;
					bInterval = true;
				}
			}
		}
		if (bInterval)
		{
			intvl.right = nWidth-1;
			arr.Add(intvl);
		}
		prows[y].nints = arr.GetSize();
		if (prows[y].nints)
		{
			prows[y].pints = new INTERVAL[prows[y].nints];
			memcpy(prows[y].pints, arr.GetData(), prows[y].nints*sizeof(INTERVAL));
		}
		else
			prows[y].pints = NULL;
	}
}

CChromoAxis::CChromoAxis()
{
	m_Type = NoData;
	m_nSize = 0;
	m_nScale = 0;
	m_iptAxisAlloc = m_iptAxis = NULL;
	m_bDelete = FALSE;
	m_ptOffset.x = m_ptOffset.y = 0;
}

CChromoAxis::CChromoAxis(CChromoAxis &S)
{
	m_Type = S.m_Type;
	m_nSize = m_nAllocSize = S.m_nSize;
	m_nScale = S.m_nScale;
	switch(m_Type)
	{
	case IntegerAxis:
		m_iptAxisAlloc = new POINT[m_nSize];
		m_iptAxis = m_iptAxisAlloc;
		memcpy(m_iptAxis, S.m_iptAxis, m_nSize*sizeof(POINT));
		break;
	case FloatAxis:
		m_fptAxisAlloc = new FPOINT[m_nSize];
		m_fptAxis = m_fptAxisAlloc;
		memcpy(m_fptAxis, S.m_fptAxis, m_nSize*sizeof(FPOINT));
		break;
	case IntegerProfile:
		m_pnProfAlloc = new int[m_nSize];
		m_pnProf = m_pnProfAlloc;
		memcpy(m_pnProf, S.m_pnProf, m_nSize*sizeof(int));
		break;
	}

}


CChromoAxis::~CChromoAxis()
{
	switch(m_Type)
	{
	case IntegerAxis:
		delete m_iptAxisAlloc;
		break;
	case FloatAxis:
		delete m_fptAxisAlloc;
		break;
	case IntegerProfile:
		delete m_pnProfAlloc;
		break;
	}
}

CChromoAxis& CChromoAxis::operator=(CChromoAxis &S)
{
	Init((IChromoAxis2*)&S);
	return *this;
}

void CChromoAxis::Init(IChromoAxis2 *p)
{
	switch(m_Type)
	{
	case IntegerAxis:
		delete m_iptAxisAlloc;
		break;
	case FloatAxis:
		delete m_fptAxisAlloc;
		break;
	case IntegerProfile:
		delete m_pnProfAlloc;
		break;
	}
	m_Type = p->GetType();
	m_nSize = m_nAllocSize = p->GetSize();
	m_nScale = p->GetScale();
	m_ptOffset = CPoint((int)p->GetOffset().x,(int)p->GetOffset().y);
	switch(m_Type)
	{
	case IntegerAxis:
		m_iptAxisAlloc = new POINT[m_nSize];
		m_iptAxis = m_iptAxisAlloc;
		memcpy(m_iptAxis, p->GetData(), m_nSize*sizeof(POINT));
		break;
	case FloatAxis:
		m_fptAxisAlloc = new FPOINT[m_nSize];
		m_fptAxis = m_fptAxisAlloc;
		memcpy(m_fptAxis, p->GetData(), m_nSize*sizeof(FPOINT));
		break;
	case IntegerProfile:
		m_pnProfAlloc = new int[m_nSize];
		m_pnProf = m_pnProfAlloc;
		memcpy(m_pnProf, p->GetData(), m_nSize*sizeof(int));
		break;
	}
}


void CChromoAxis::Init(AxisType Type, unsigned long nSize, void *pData)
{
	if (m_Type != Type || m_nAllocSize != nSize)
	{
		switch(m_Type)
		{
		case IntegerAxis:
			delete m_iptAxisAlloc;
			break;
		case FloatAxis:
			delete m_fptAxisAlloc;
			break;
		case IntegerProfile:
			delete m_pnProfAlloc;
			break;
		}
		switch(Type)
		{
		case IntegerAxis:
			m_iptAxis = m_iptAxisAlloc = new POINT[nSize+1];
			break;
		case FloatAxis:
			m_fptAxis = m_fptAxisAlloc = new FPOINT[nSize+1];
			break;
		case IntegerProfile:
			m_pnProf = m_pnProfAlloc = new int[nSize+1];
			break;
		}
		m_Type = Type;
		m_nAllocSize = m_nSize = nSize;
	}
	if (m_nAllocSize != m_nSize)
	{
		m_nSize = m_nAllocSize;
		switch(m_Type)
		{
		case IntegerAxis:
			m_iptAxisAlloc = m_iptAxis;
			break;
		case FloatAxis:
			m_fptAxisAlloc = m_fptAxis;
			break;
		case IntegerProfile:
			m_pnProfAlloc = m_pnProf;
			break;
		}
	}
	if (pData)
	{
		switch(Type)
		{
		case IntegerAxis:
			memcpy(m_iptAxis,pData,nSize*sizeof(POINT));
			break;
		case FloatAxis:
			memcpy(m_fptAxis,pData,nSize*sizeof(FPOINT));
			break;
		case IntegerProfile:
			memcpy(m_pnProf,pData,nSize*sizeof(int));
			break;
		}
	}
	else
	{
		switch(Type)
		{
		case IntegerAxis:
			memset(m_iptAxis,0,(nSize+1)*sizeof(POINT));
			break;
		case FloatAxis:
			memset(m_fptAxis,0,(nSize+1)*sizeof(FPOINT));
			break;
		case IntegerProfile:
			memset(m_pnProf,0,(nSize+1)*sizeof(int));
			break;
		}
	}
	m_nScale = 1;
}

void *CChromoAxis::GetData()
{
	switch(m_Type)
	{
	case IntegerAxis:
		return m_iptAxis;
	case FloatAxis:
		return m_fptAxis;
	case IntegerProfile:
		return m_pnProf;
	}
	return NULL;
}

void CChromoAxis::Shorten(int nBeg, int nEnd)
{
	switch(m_Type)
	{
	case IntegerAxis:
		m_iptAxis += nBeg;
		break;
	case FloatAxis:
		m_fptAxis += nBeg;
		break;
	case IntegerProfile:
		m_pnProf += nBeg;
		break;
	}
	m_nSize = nEnd-nBeg;
}

void CChromoAxis::Scale(int nScale)
{
	unsigned i;
	int nMul = nScale > 0 ? nScale : 1;
	int nDiv = nScale < 0 ? -nScale : 1;
	switch(m_Type)
	{
	case IntegerAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_iptAxis[i].x = m_iptAxis[i].x*nMul/nDiv;
			m_iptAxis[i].y = m_iptAxis[i].y*nMul/nDiv;
		}
		break;
	case FloatAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_fptAxis[i].x = m_fptAxis[i].x*nMul/nDiv;
			m_fptAxis[i].y = m_fptAxis[i].y*nMul/nDiv;
		}
		break;
	}
	m_nScale = m_nScale*nMul/nDiv;
}

void CChromoAxis::Scale(double dScale)
{
	unsigned i;
	switch(m_Type)
	{
	case FloatAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_fptAxis[i].x = m_fptAxis[i].x*dScale;
			m_fptAxis[i].y = m_fptAxis[i].y*dScale;
		}
		break;
	case IntegerAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_iptAxis[i].x = int(m_iptAxis[i].x*dScale);
			m_iptAxis[i].y = int(m_iptAxis[i].y*dScale);
		}
		break;
	};
}

void CChromoAxis::Convert(AxisType NewType)
{
	if (m_Type == IntegerAxis && NewType == FloatAxis)
	{
		double d = m_nScale == 0 ? 1. : m_nScale > 0 ? 1./double(m_nScale) : double(-m_nScale);
		FPOINT *fpt = new FPOINT[m_nSize];
		for (unsigned i = 0; i < m_nSize; i++)
		{
			fpt[i].x = m_iptAxis[i].x*d;
			fpt[i].y = m_iptAxis[i].y*d;
		}
		delete m_iptAxisAlloc;
		m_fptAxis = m_fptAxisAlloc = fpt;
		m_nAllocSize = m_nSize;
		m_Type = NewType;
		m_nScale = 1;
	}
	else if (m_Type == FloatAxis && NewType == IntegerAxis)
	{
		POINT *pt = new POINT[m_nSize];
		for (unsigned i = 0; i < m_nSize; i++)
		{
			pt[i].x = int(m_fptAxis[i].x);
			pt[i].y = int(m_fptAxis[i].y);
		}
		delete m_fptAxisAlloc;
		m_iptAxis = m_iptAxisAlloc = pt;
		m_nAllocSize = m_nSize;
		m_Type = NewType;
	}
}

void CChromoAxis::Rotate(FPOINT fptOrg, double dAngle)
{
	unsigned i;
	double cosa = cos(dAngle);
	double sina = sin(dAngle);
	switch(m_Type)
	{
	case FloatAxis:
		for (i = 0; i < m_nSize; i++)
		{
			FPOINT fp,fp2;
			fp.x = m_fptAxis[i].x-fptOrg.x;
			fp.y = m_fptAxis[i].y-fptOrg.y;
			fp2.x = fp.x*cosa-fp.y*sina;
			fp2.y = fp.x*sina+fp.y*cosa;
			m_fptAxis[i].x = fp2.x+fptOrg.x;
			m_fptAxis[i].y = fp2.y+fptOrg.y;
		}
		break;
	case IntegerAxis:
		for (i = 0; i < m_nSize; i++)
		{
			FPOINT fp,fp2;
			fp.x = m_iptAxis[i].x-fptOrg.x;
			fp.y = m_iptAxis[i].y-fptOrg.y;
			fp2.x = fp.x*cos(dAngle)-fp.y*sin(dAngle);
			fp2.y = fp.x*sin(dAngle)+fp.y*cos(dAngle);
			m_iptAxis[i].x = int(fp2.x+fptOrg.x);
			m_iptAxis[i].y = int(fp2.y+fptOrg.y);
		}
		break;
	};
}

void CChromoAxis::MirrorX(double dxMirror)
{
	unsigned i;
	switch(m_Type)
	{
	case FloatAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_fptAxis[i].x = dxMirror-m_fptAxis[i].x;
		}
		break;
	case IntegerAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_iptAxis[i].x = int(dxMirror-m_iptAxis[i].x);
		}
		break;
	};
}


void CChromoAxis::MirrorY(double dyMirror)
{
	unsigned i;
	switch(m_Type)
	{
	case FloatAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_fptAxis[i].y = dyMirror-m_fptAxis[i].y;
		}
		break;
	case IntegerAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_iptAxis[i].y = int(dyMirror-m_iptAxis[i].y);
		}
		break;
	};
}

void CChromoAxis::Offset(FPOINT fptOffs)
{
	unsigned i;
	switch(m_Type)
	{
	case FloatAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_fptAxis[i].x += fptOffs.x;
			m_fptAxis[i].y += fptOffs.y;
		}
		break;
	case IntegerAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_iptAxis[i].x = int(m_iptAxis[i].x+fptOffs.x);
			m_iptAxis[i].y = int(m_iptAxis[i].y+fptOffs.y);
		}
		break;
	};
	m_ptOffset.x -= int(fptOffs.x);
	m_ptOffset.y -= int(fptOffs.y);
}

void CChromoAxis::Offset(CPoint ptOffs)
{
	unsigned i;
	switch(m_Type)
	{
	case FloatAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_fptAxis[i].x += ptOffs.x;
			m_fptAxis[i].y += ptOffs.y;
		}
		break;
	case IntegerAxis:
		for (i = 0; i < m_nSize; i++)
		{
			m_iptAxis[i].x += ptOffs.x;
			m_iptAxis[i].y += ptOffs.y;
		}
		break;
	};
	m_ptOffset.x -= ptOffs.x;
	m_ptOffset.y -= ptOffs.y;
}

FPOINT CChromoAxis::GetFPoint(int i)
{
	FPOINT fpt;
	switch(m_Type)
	{
	case IntegerAxis:
		fpt.x = double(m_iptAxis[i].x)/double(m_nScale);
		fpt.y = double(m_iptAxis[i].y)/double(m_nScale);
		return fpt;
	case FloatAxis:
		return m_fptAxis[i];
	default:
		fpt.x = fpt.y = 0.;
	};
	return fpt;
}

void CChromoAxis::Serialize(CStreamEx& ar)
{
	int i;
	if (ar.IsStoring())
	{
		long lVersion = 2;
		ar << lVersion;
		ar << (long)m_Type;
		ar << (long)m_nSize;
		ar << (long)m_nAllocSize;
		long lAllocOffs;
		switch(m_Type)
		{
		case FloatAxis:
			lAllocOffs = m_fptAxis-m_fptAxisAlloc;
			break;
		case IntegerAxis:
			lAllocOffs = m_iptAxis-m_iptAxisAlloc;
			break;
		case IntegerProfile:
			lAllocOffs = m_pnProf-m_pnProfAlloc;
			break;
		};
		ar << lAllocOffs;
		ar << (long)m_nScale;
		switch(m_Type)
		{
		case FloatAxis:
			for (i = 0; i < (int)m_nAllocSize; i++)
			{
				ar << m_fptAxisAlloc[i].x;
				ar << m_fptAxisAlloc[i].y;
			}
			break;
		case IntegerAxis:
			for (i = 0; i < (int)m_nAllocSize; i++)
				ar << m_iptAxisAlloc[i];
			break;
		case IntegerProfile:
			for (i = 0; i < (int)m_nAllocSize; i++)
				ar << m_pnProfAlloc[i];
			break;
		};
		ar << m_ptOffset;
	}
	else
	{
		long lVer,lType,lSize,lAllocSize,lAllocOffs,lScale;
		ar >> lVer; // version
		ar >> lType;
		ar >> lSize;
		ar >> lAllocSize;
		ar >> lAllocOffs;
		ar >> lScale;
		if (lType == IntegerAxis || lType == FloatAxis || lType == IntegerProfile)
		{
			Init((AxisType)lType, lAllocSize);
			m_nSize = lSize;
			m_nScale = lScale;
			switch(m_Type)
			{
			case FloatAxis:
				m_fptAxis += lAllocOffs;
				for (i = 0; i < (int)m_nAllocSize; i++)
				{
					ar >> m_fptAxisAlloc[i].x;
					ar >> m_fptAxisAlloc[i].y;
				}
				break;
			case IntegerAxis:
				m_iptAxis += lAllocOffs;
				for (i = 0; i < (int)m_nAllocSize; i++)
					ar >> m_iptAxisAlloc[i];
				break;
			case IntegerProfile:
				m_pnProf += lAllocOffs;
				for (i = 0; i < (int)m_nAllocSize; i++)
					ar >> m_pnProfAlloc[i];
				break;
			};
		}
		if (lVer >= 2) ar >> m_ptOffset;
	}
}

void CChromoAxis::Smooth(int nIterations, bool bScaled)
{
	if (m_Type == IntegerAxis)
	{
		int i,j;
		int nPoints = m_nSize;
		for (i = 0; i < nIterations; i++)
		{
			POINT ptPrev = m_iptAxis[0];
			m_iptAxis[0].x <<= 2;
			m_iptAxis[0].y <<= 2;
			for (j = 1; j < nPoints-1; j++)
			{
				POINT ptCur = m_iptAxis[j];
				m_iptAxis[j].x = ptPrev.x+2*ptCur.x+m_iptAxis[j+1].x;
				m_iptAxis[j].y = ptPrev.y+2*ptCur.y+m_iptAxis[j+1].y;
				ptPrev = ptCur;
			}
			m_iptAxis[nPoints-1].x <<= 2;
			m_iptAxis[nPoints-1].y <<= 2;
		}
		int n = bScaled?2*nIterations-3:2*nIterations;
		for (j = 0; j < nPoints; j++)
		{
			m_iptAxis[j].x >>= n;
			m_iptAxis[j].y >>= n;
		}
		if (bScaled) m_nScale = 8;
	}
}


/*HRESULT CChromoAxis::Init(int nSize)
{
	Init(IntegerAxis, nSize);
	return S_OK;
}

HRESULT CChromoAxis::GetData(POINT *pAxisPoints)
{
	memcpy(pAxisPoints, m_iptAxis, m_nSize*sizeof(POINT));
	return S_OK;
}

HRESULT CChromoAxis::GetDataPtr(POINT **ppAxisPoints)
{
	*ppAxisPoints = m_iptAxis;
	return S_OK;
}*/

POINT rotatepoint(CIntervalImage *pIntImg, POINT pt, double angle)
{
	CPoint ptOrg(pIntImg->ptOffset.x+(pIntImg->szInt.cx-1)/2,pIntImg->ptOffset.y+(pIntImg->szInt.cy-1)/2);
	CPoint pt1(pt.x-ptOrg.x,pt.y-ptOrg.y);
	double x = pt1.x*cos(angle)-pt1.y*sin(angle);
	double y = pt1.x*sin(angle)+pt1.y*cos(angle);
	CPoint ptRet(int(x)+ptOrg.x,int(y)+ptOrg.y);
	return ptRet;
}

POINT rotatepoint(POINT ptOrg, POINT pt, double angle)
{
	CPoint pt1(pt.x-ptOrg.x,pt.y-ptOrg.y);
	double x = pt1.x*cos(angle)-pt1.y*sin(angle);
	double y = pt1.x*sin(angle)+pt1.y*cos(angle);
	CPoint ptRet(int(x)+ptOrg.x,int(y)+ptOrg.y);
	return ptRet;
}

FPOINT rotatepoint(FPOINT ptOrg, FPOINT pt, double angle)
{
	CFPoint pt1(pt.x-ptOrg.x,pt.y-ptOrg.y);
	double x = pt1.x*cos(angle)-pt1.y*sin(angle);
	double y = pt1.x*sin(angle)+pt1.y*cos(angle);
	CFPoint ptRet(x+ptOrg.x,y+ptOrg.y);
	return ptRet;
}

void CCentromere::Offset(POINT ptOffset)
{
	ptCen.x += ptOffset.x;
	ptCen.y += ptOffset.y;
	aptCenDraw[0].x += ptOffset.x;
	aptCenDraw[1].x += ptOffset.x;
	aptCenDraw[0].y += ptOffset.y;
	aptCenDraw[1].y += ptOffset.y;
	this->ptOffset.x -= ptOffset.x;
	this->ptOffset.y -= ptOffset.y;
}

void CCentromere::MirrorX(int x)
{
	ptCen.x = x-ptCen.x;
	if (!IsInvPoint(aptCenDraw[0]))	aptCenDraw[0].x = x-aptCenDraw[0].x;
	if (!IsInvPoint(aptCenDraw[1]))	aptCenDraw[1].x = x-aptCenDraw[1].x;
}

CChromoPerimeter::CChromoPerimeter()
{
	memset(this, 0, sizeof(*this));
}

CChromoPerimeter::~CChromoPerimeter()
{
	delete m_ppt;
	delete m_pChords;
}

void CChromoPerimeter::MakeByInterval(CIntervalImage *pIntvl)
{
	delete m_ppt;
	delete m_pChords;
	int nSizeAlloc = 3+2*(pIntvl->nrows-1);
	POINT *ppt = new POINT[nSizeAlloc];
	ppt[0].x = pIntvl->prows[0].pints[0].left;
	ppt[0].y = 0;
	ppt[1].x = pIntvl->prows[0].pints[pIntvl->prows[0].nints-1].right;
	ppt[1].y = 0;
	POINT *ppt2 = ppt;
	POINT *ppt1 = ppt+1;
	POINT *ppt0 = ppt+2;
	int nvertices = 2;
	for (int i = 1; i < pIntvl->nrows; i++)
	{
		if (pIntvl->prows[i].nints==0)
			continue;
		ppt0->x = pIntvl->prows[i].pints[pIntvl->prows[i].nints-1].right;
		ppt0->y = i;
		nvertices++;
		while (nvertices >= 3 && (ppt0->y-ppt2->y)*(ppt1->x-ppt2->x) <= (ppt1->y-ppt2->y)*(ppt0->x-ppt2->x))
		{
			*ppt1 = *ppt0;
			ppt0--;
			ppt1--;
			ppt2--;
			nvertices--;
		}
		ppt0++;
		ppt1++;
		ppt2++;
	}
	int nhalfway = nvertices + 2;
	for (i = pIntvl->nrows-1; i >= 0; i--)
	{
		if (pIntvl->prows[i].nints == 0)
			continue;
		ppt0->x = pIntvl->prows[i].pints[0].left;
		ppt0->y = i;
		nvertices++;
		/*
		 * Concavity condition (may propagate backwards).
		 * Also deals satisfactorily with the case that last
		 * line consists of a single interval, itself a single point.
		 */
		while (nvertices >= nhalfway && (ppt0->y-ppt2->y)*(ppt1->x-ppt2->x) <=
		    	(ppt1->y-ppt2->y)*(ppt0->x-ppt2->x))
		{
			ppt1->x = ppt0->x;
			ppt1->y = ppt0->y;
			ppt0--;
			ppt1--;
			ppt2--;
			nvertices--;
		}
		ppt0++;
		ppt1++;
		ppt2++;
	}
	m_ppt = ppt;
	m_nSize = nvertices;
	m_pChords = new CChord[nvertices-1];
	for (i = 0; i < nvertices-1; i++)
	{
		m_pChords[i].acon = m_ppt[i+1].y-m_ppt[i].y;
		m_pChords[i].bcon = m_ppt[i+1].x-m_ppt[i].x;
		m_pChords[i].cl = hypot8(m_pChords[i].acon, m_pChords[i].bcon);
	}
}

int CChromoPerimeter::CalcPerim()
{
	int nPerim = 0;
	for (int i = 0; i < (int)m_nSize-1; i++)
	{
		int b = m_ppt[i+1].x-m_ppt[i].x;
		int a = m_ppt[i+1].y-m_ppt[i].y;
		int cl = hypot8(a, b);
		nPerim += cl;
	}
	return nPerim;
}

static int gap(CChord *pChords, POINT *ppt, int nvertices, int &nummin, int &nummax)
{
	int minc, maxc;
	int acon = pChords->acon;
	int bcon = pChords->bcon;
	for (int i = 0; i < nvertices; i++)
	{
		int c = acon*ppt->x - bcon*ppt->y;
		if (i == 0)
		{
			minc = maxc = c;
			nummin = nummax = i;
		}
		else if (c > maxc)
		{
			maxc = c;
			nummax = i;
		}
		else if (c < minc)
		{
			minc = c;
			nummin = i;
		}
		ppt++;
	}
	/* return 32*gap (rounded to integer) */
	return ( pChords->cl > 0 ? ((maxc - minc) << 8)/pChords->cl : 1 );
}


CChord CChromoPerimeter::CalcAngle(int &nn1, int &nn2, int &s, int &c)
{
	CChord *pChords = m_pChords;
	int minwidth = gap(pChords,m_ppt,m_nSize,nn1,nn2);
	CChord *pRet = pChords++;
	for (int i = 1; i < (int)m_nSize-1; i++)
	{
		int n1,n2;
		int w = gap(pChords,m_ppt,m_nSize,n1,n2);
		if (w < minwidth)
		{
			minwidth = w;
			pRet = pChords;
			nn1 = n1;
			nn2 = n2;
		}
		pChords++;
	}
	c = pRet->acon;
	s = pRet->bcon;
	return *pRet;
}


