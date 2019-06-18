#include "stdafx.h"
#include "RectFilter.h"
#include "nameconsts.h"
#include "image5.h"
#include "impl_long.h"
#include "misc5.h"
#include "measure5.h"
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>

#if _DEBUG
#define ASSERT(x) if (!(x)) DebugBreak()
#define _DebugBreak() DebugBreak()
#else
#define ASSERT(x) 
#define _DebugBreak()
#endif

class File
{
public:
	int h;
	File(LPCTSTR lpName, int nMode, int nAttr = 0)
	{
		h = _topen(lpName, nMode, nAttr);
	}
	~File()
	{
		if (h != -1) _close(h);
	}
};

IUnknown* _CreateNamedObject(LPCTSTR szType, IUnknown *punkParent = NULL)
{
	IUnknown *punkObject = NULL;
#ifdef _DEBUG
	HINSTANCE hDll_Common = GetModuleHandle("common_d.dll");
#else
	HINSTANCE hDll_Common = GetModuleHandle("common.dll");
#endif
	PCREATE_TYPED_OBJECT pCreateTypedObject = (PCREATE_TYPED_OBJECT)GetProcAddress(hDll_Common, "CreateTypedObject"); 
	DWORD dw = GetLastError();
	punkObject = pCreateTypedObject(szType);
	if (punkObject && punkParent)
	{
		INamedDataObject2Ptr sptrND(punkObject);
		if (sptrND != 0)
			sptrND->SetParent(punkParent, 0);
	}
	return punkObject;
}


CRectFilter::CRectFilter()
{
	m_bstrDocumentTemplate = _bstr_t("Image");
	m_bstrFilterName = _bstr_t("Rect files");
	m_bstrFilterExt = _bstr_t(".rc\n.rect");
	AddDataObjectType(szArgumentTypeImage);
	AddDataObjectType(szArgumentTypeObjectList);
	m_nObjects = 0;
	m_pObjects = NULL;
}

CRectFilter::~CRectFilter()
{
	FreeRecords();
}

void CRectFilter::FreeRecords()
{
	for (int i = 0; i < m_nObjects; i++)
		delete m_pObjects[i].m_pData;
	delete m_pObjects;
	m_nObjects = 0;
	m_pObjects = NULL;
}

bool CRectFilter::ReadFile( const char *pszFileName )
{
	int nObjs = 1000;
	BYTE byData[18];
	int nFile = 0;
	TCHAR strBuff[255];
	try
	{
		// Read all data in file
		File f(pszFileName, _O_RDONLY|_O_BINARY);
		if (f.h == -1) return false;
		FreeRecords();
		m_pObjects = new CChromoRecord[nObjs];
		memset(m_pObjects, 0, nObjs*sizeof(CChromoRecord));
		m_nObjects = 0;
		while (1)
		{
			int nRd = _read(f.h, byData, 18);
			if (!(nRd == 18 || nRd == 1 && byData[0] == 255))
			{
				wsprintf(strBuff, _T("unexpected end of file during read object %d"), m_nObjects);
				_DebugBreak();
				FreeRecords();
				MessageBox(0, strBuff, _T("ChromExtrn"), MB_OK|MB_ICONEXCLAMATION);
				return false;
			}
			if (byData[0] == 255) break;
			if (byData[0] == 1 || byData[0] == 2)
				m_pObjects[m_nObjects].m_bAutoSeg = byData[0]==1;
			else
			{
				wsprintf(strBuff, _T("Invalid field 0 for object %d % %d"), m_nObjects, (int)byData[0]);
				_DebugBreak();
				FreeRecords();
				MessageBox(0, strBuff, _T("ChromExtrn"), MB_OK|MB_ICONEXCLAMATION);
				return false;
			}
			if (byData[1] <= 25)
				m_pObjects[m_nObjects].m_nClass = byData[1];
			else
			{
				wsprintf(strBuff, _T("Invalid class for object %d : %d"), m_nObjects, (int)byData[1]);
				_DebugBreak();
				FreeRecords();
				MessageBox(0, strBuff, _T("ChromExtrn"), MB_OK|MB_ICONEXCLAMATION);
				return false;
			}
			m_pObjects[m_nObjects].m_rcChromo.top = (((DWORD)(byData[2]))<<24)|(((DWORD)(byData[3]))<<16)
				|(((DWORD)(byData[4]))<<8)|((DWORD)(byData[5]));
			ASSERT(m_pObjects[m_nObjects].m_rcChromo.top < 10000);
			m_pObjects[m_nObjects].m_rcChromo.bottom = (((DWORD)(byData[6]))<<24)|(((DWORD)(byData[7]))<<16)
				|(((DWORD)(byData[8]))<<8)|((DWORD)(byData[9]));
			ASSERT(m_pObjects[m_nObjects].m_rcChromo.bottom < 10000);
			m_pObjects[m_nObjects].m_rcChromo.left = (((DWORD)(byData[10]))<<24)|(((DWORD)(byData[11]))<<16)
				|(((DWORD)(byData[12]))<<8)|((DWORD)(byData[13]));
			ASSERT(m_pObjects[m_nObjects].m_rcChromo.left < 10000);
			m_pObjects[m_nObjects].m_rcChromo.right = (((DWORD)(byData[14]))<<24)|(((DWORD)(byData[15]))<<16)
				|(((DWORD)(byData[16]))<<8)|((DWORD)(byData[17]));
			ASSERT(m_pObjects[m_nObjects].m_rcChromo.right < 10000);
			if (m_pObjects[m_nObjects].m_rcChromo.top < 0)
			{
				m_pObjects[m_nObjects].m_rcChromo.bottom -= m_pObjects[m_nObjects].m_rcChromo.top;
				m_pObjects[m_nObjects].m_rcChromo.top = 0;
			}
			if (m_pObjects[m_nObjects].m_rcChromo.left)
			{
				m_pObjects[m_nObjects].m_rcChromo.right -= m_pObjects[m_nObjects].m_rcChromo.left;
				m_pObjects[m_nObjects].m_rcChromo.left = 0;
			}
			int nLines = m_pObjects[m_nObjects].m_rcChromo.bottom-m_pObjects[m_nObjects].m_rcChromo.top+1;
			int nColumns = m_pObjects[m_nObjects].m_rcChromo.right-m_pObjects[m_nObjects].m_rcChromo.left+1;
			int n = nLines*nColumns;
			ASSERT(n > 0);
			BYTE *pData = new BYTE[n];
			if (_read(f.h, pData, n) != n)
			{
				wsprintf(strBuff, _T("unexpected end of file during read object %d"), m_nObjects);
				_DebugBreak();
				FreeRecords();
				MessageBox(0, strBuff, _T("ChromExtrn"), MB_OK|MB_ICONEXCLAMATION);
				return false;
			}
			m_pObjects[m_nObjects].m_pData = pData;
			m_nObjects++;
		}
		// Find image width and length
		int nOverlap = 2;
		int nImX = nOverlap==0?1:nOverlap==2?m_nObjects:(int)sqrt(double(m_nObjects)/4)*4;
		int nImY = nOverlap==0||nOverlap==2?1:(m_nObjects+nImX-1)/nImX;
		int imageWidth = 0,imageLength = 0;
		for (int i = 0; i < m_nObjects; i++)
		{
			imageWidth = max(imageWidth, m_pObjects[i].m_rcChromo.left+1);
			imageWidth = max(imageWidth, m_pObjects[i].m_rcChromo.right+1);
			imageLength = max(imageLength, m_pObjects[i].m_rcChromo.top+1);
			imageLength = max(imageLength, m_pObjects[i].m_rcChromo.bottom+1);
		}
		imageWidth += 1;
		imageLength += 1;
		// Save all data to image
		IUnknown* punk = CreateNamedObject(GetDataObjectType(0));
		TCHAR szName[_MAX_PATH];
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_PATH];
		TCHAR ext[_MAX_PATH];
		_splitpath(pszFileName, drive, dir, szName, ext);
		_bstr_t bstrName(szName);
		long	NameExists = 0;
		if( m_sptrINamedData != 0 )
			m_sptrINamedData->NameExists(bstrName, &NameExists);
		// if new name not exists allready
		if (!NameExists && punk)
		{
			// set this name to object
			INamedObject2Ptr sptrObj(punk);
			sptrObj->SetName(bstrName);
		}
		IImagePtr sptrI(punk);
		if (punk) punk->Release();
		if(sptrI == 0) return false;
		sptrI->CreateImage(imageWidth*nImX, imageLength*nImY, _bstr_t("GRAY"),-1);
		for (i = 0; i < imageLength*nImY; i++)
		{
			color *pDst;
			sptrI->GetRow(i,0,&pDst);
			memset(pDst, 0, imageWidth*nImX*sizeof(color));
		}
		for (i = 0; i < m_nObjects; i++)
		{
			BYTE *pSrc = m_pObjects[i].m_pData;
			int y1 = nOverlap==0||nOverlap==2?0:(i/nImX)*imageLength;
			int x1 = nOverlap==0?0:nOverlap==2?i*imageWidth:(i%nImX)*imageWidth;
			m_pObjects[i].m_ptPos.x = x1;
			m_pObjects[i].m_ptPos.y = y1;
			int nColumns = m_pObjects[i].m_rcChromo.right-m_pObjects[i].m_rcChromo.left+1;
			for (int y = m_pObjects[i].m_rcChromo.top; y <= m_pObjects[i].m_rcChromo.bottom; y++)
			{
				color *pDst;
				sptrI->GetRow(y1+y,0,&pDst);
				pDst += x1+m_pObjects[i].m_rcChromo.left;
				for (int x = 0; x < nColumns; x++)
				{
					if (*pSrc)
						*pDst++ = ((color)(*pSrc++))<<8;
					else
						pSrc++,pDst++;
				}
			}
		}
#if 0
		// Initialize objects list
		punk = CreateNamedObject(GetDataObjectType(1));
		INamedDataObject2Ptr sptrOL(punk);
		if(sptrOL == 0) return true;
		for (i = 0; i < m_nObjects; i++)
		{
			// Create new object
			IUnknown *punk1 = _CreateNamedObject(szArgumentTypeObject, punk);
			IMeasureObjectPtr sptrMO(punk1);
			if (punk1) punk1->Release();
			if(sptrMO == 0) continue;
			// Create image for it
			IUnknown *punkImg = _CreateNamedObject(szArgumentTypeImage, NULL);
			IImage2Ptr sptrI(punkImg);
			int nLines = m_pObjects[m_nObjects].m_rcChromo.bottom-m_pObjects[m_nObjects].m_rcChromo.top+1;
			int nColumns = m_pObjects[m_nObjects].m_rcChromo.right-m_pObjects[m_nObjects].m_rcChromo.left+1;
			sptrI->CreateImage(nColumns, nLines, _bstr_t("GRAY"),-1);
			// Initialize this image
			BYTE *pSrc = m_pObjects[i].m_pData;
			for (int y = 0; y < nLines; y++)
			{
				BYTE *pDstMask;
				color *pDstData;
				sptrI->GetRowMask(y, &pDstMask);
				sptrI->GetRow(y, 0, &pDstData);
				for (int x = 0; x < nColumns; x++)
				{
//					*pDstMask++ = *pSrc==0?0:255;
					*pDstData++ = ((color)*pSrc++)<<8;
				}
			}
			sptrMO->SetImage(punkImg);
			if (punkImg) punkImg->Release();
		}
		if (punk) punk->Release();
#endif
	}
	catch(TerminateLongException* e)
	{
		FreeRecords();
		e->Delete();
		return false;
	}
	return true;
}

bool CRectFilter::WriteFile( const char *pszFileName )
{
	return false;
}

bool CRectFilter::_InitNew()
{
	return true;
}

