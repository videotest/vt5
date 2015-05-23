// ImageBySel.cpp: implementation of the CActionImageBySel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newdoc.h"
#include "ImageBySel.h"
#include "Input.h"
#include "NewDocVT5Profile.h"
#include "misc_utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CVT5ProfileManager g_ImageBySelProfile(_T("ImageBySel"), false);

static CString MakeMaskFileName()
{
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr = NULL;
		sptrDM->GetCurrentDeviceName(&bstr);
		CString sDevName(bstr);
		if (bstr) ::SysFreeString(bstr);
		if (sDevName.IsEmpty()) return CString(_T(""));
		TCHAR szPath[_MAX_PATH];
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szDir[_MAX_DIR];
		GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
		_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
		_tmakepath(szPath, szDrive, szDir, sDevName, _T(".mask"));
		return CString(szPath);
	}
	else return CString(_T(""));
}

static void SaveMask(IUnknown *punkImage)
{
	CString sFileName(MakeMaskFileName());
	if (sFileName.IsEmpty()) return;
	CImageWrp img(punkImage);
	int cx = img.GetWidth();
	int cy = img.GetHeight();
	int nRow = ((cx+3)>>2)<<2;
	int nSize = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+nRow*cy;
	BITMAPFILEHEADER bfh;
	ZeroMemory(&bfh, sizeof(bfh));
	bfh.bfType = ((int( 'M' )<<8)|int( 'B' ));
	bfh.bfSize = sizeof(bfh)+nSize;
	CFile file(sFileName, CFile::modeCreate|CFile::modeWrite);
	file.Write(&bfh, sizeof(bfh));
	BITMAPINFOHEADER bih;
	ZeroMemory(&bih, sizeof(bih));
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = cx;
	bih.biHeight = cy;
	bih.biBitCount = 8;
	bih.biPlanes = 1;
	bih.biCompression = BI_RGB;
	bih.biClrUsed = bih.biClrImportant = 256;
	file.Write(&bih, sizeof(bih));
	RGBQUAD rgb[256];
	for(int i = 0; i < 256; i++)
	{
		rgb[i].rgbBlue = rgb[i].rgbGreen = rgb[i].rgbRed = i;
		rgb[i].rgbReserved = 0;
	}
	file.Write(rgb, 256*sizeof(RGBQUAD));
	byte *pOffset = NULL;
	if (nRow>cx)
		pOffset = (byte *)calloc(nRow-cx, sizeof(byte));
	for (i = 0; i < cy; i++)
	{
		byte *pRow = img.GetRowMask(i);
		file.Write(pRow, cx);
		if (nRow>cx)
			file.Write(pOffset, nRow-cx);
	}
	if (nRow>cx)
		free(pOffset);
	CPoint pt(img.GetOffset());
	file.Write(&pt,sizeof(pt));
}

/*
static void RestoreMask(IUnknown *punkImage, IUnknown *punkSel)
{
	CString sFileName(MakeMaskFileName());
	if (sFileName.IsEmpty()) return;
	CImageWrp img(punkImage);
	CImageWrp imgSel(punkSel);
	int cxImg = img.GetWidth();
	int cyImg = img.GetHeight();
	try
	{
		BITMAPFILEHEADER bfh;
		CFile file(sFileName, CFile::modeRead);
		file.Read(&bfh, sizeof(bfh));
		if (bfh.bfType != ((int( 'M' )<<8)|int( 'B' )) || bfh.bfOffBits != 0)
			return;
		BITMAPINFOHEADER bih;
		file.Read(&bih, sizeof(bih));
		if (bih.biBitCount != 8 || bih.biCompression != BI_RGB) return;
		file.Seek(256*sizeof(RGBQUAD), CFile::current);
		int nRow = ((bih.biWidth+3)>>2)<<2;
		byte *pData = (byte *)calloc(nRow*bih.biHeight, sizeof(byte));
		if (!pData) return;
		file.Read(pData, nRow*bih.biHeight*sizeof(byte));
		POINT pt;
		file.Read(&pt, sizeof(pt));
		if (pt.x>=cxImg||pt.y>=cyImg)
		{
			free(pData);
			return;
		}
		int cxSel = min(cxImg-pt.x,bih.biWidth);
		int cySel = min(cyImg-pt.y,bih.biHeight);
		imgSel.CreateNew(cxSel, cySel, img.GetColorConvertor());
		byte *p = pData;
		imgSel.SetOffset(pt);
		imgSel.InitRowMasks();
		for (int i = 0; i < cySel; i++)
		{
			byte *pRowMask = imgSel.GetRowMask(i);
			memcpy(pRowMask, p, cxSel);
			p += nRow;
		}
		free(pData);
		int nPlanes = imgSel.GetColorsCount();
		for (int k = 0; k < nPlanes; k++)
		{
			for (int i = 0; i < cySel; i++)
			{
				color *pRowSrc = img.GetRow(pt.y+i, k);
				color *pRowDst = imgSel.GetRow(i, k);
				memcpy(pRowDst, pRowSrc+pt.x, cxSel*sizeof(color));
			}
		}
		imgSel.InitContours();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}
*/

static void RestoreMask(CRect &rc, CWalkFillInfo *&pinfo)
{
	CString sFileName(MakeMaskFileName());
	if (sFileName.IsEmpty()) return;
	try
	{
		BITMAPFILEHEADER bfh;
		CFile file(sFileName, CFile::modeRead);
		file.Read(&bfh, sizeof(bfh));
		if (bfh.bfType != ((int( 'M' )<<8)|int( 'B' )) || bfh.bfOffBits != 0)
			return;
		BITMAPINFOHEADER bih;
		file.Read(&bih, sizeof(bih));
		if (bih.biBitCount != 8 || bih.biCompression != BI_RGB) return;
		file.Seek(256*sizeof(RGBQUAD), CFile::current);
		int nRow = ((bih.biWidth+3)>>2)<<2;
		byte *pData = (byte *)calloc(nRow*bih.biHeight, sizeof(byte));
		if (!pData) return;
		file.Read(pData, nRow*bih.biHeight*sizeof(byte));
		POINT pt;
		file.Read(&pt, sizeof(pt));
		rc = CRect(pt.x, pt.y, pt.x+bih.biWidth, pt.y+bih.biHeight);
		pinfo = new CWalkFillInfo(rc);
		byte *p = pData;
		for (int i = 0; i < bih.biHeight; i++)
		{
			byte *pRowMask = pinfo->GetRowMask(i);
			memcpy(pRowMask, p, rc.Width());
			p += nRow;
		}
		free(pData);
	}
	catch(CException *e)
	{
		e->Delete();
	}
	catch(...)
	{
	}
}

static bool GetActiveImage( IUnknown *punkC, IUnknown **ppunkI, IUnknown **ppunkS )
{
	(*ppunkI) = 0;
	(*ppunkS) = 0;

	IUnknown	*punkImage = 0;
	IUnknown	*punkOld = 0;

	if( CheckInterface( punkC, IID_IDocument ) )
		punkImage = GetActiveObjectFromDocument( punkC, szTypeImage );
	else
		punkImage = GetActiveObjectFromContext( punkC, szTypeImage );
	if( ::GetParentKey( punkImage ) != INVALID_KEY )
	{
		punkOld = punkImage;
		INamedDataObject2Ptr	ptrN( punkImage );
		ptrN->GetParent( &punkImage );
	}
	
	if( !punkImage )
	{
		if( punkOld )
			punkOld->Release();
		return false;
	}

	(*ppunkI) = punkImage;
	(*ppunkS) = punkOld;

	return true;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CActionImageBySel, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRestoreSel, CCmdTargetEx);


// {98653601-883D-4B03-A1BC-64970E64D7A8}
GUARD_IMPLEMENT_OLECREATE(CActionImageBySel, "input.ImageBySel", 
0x98653601, 0x883d, 0x4b03, 0xa1, 0xbc, 0x64, 0x97, 0xe, 0x64, 0xd7, 0xa8);
// {7CD6B502-89B2-4853-9874-C7E790460CDD}
GUARD_IMPLEMENT_OLECREATE(CActionRestoreSel, "input.RestoreSel", 
0x7cd6b502, 0x89b2, 0x4853, 0x98, 0x74, 0xc7, 0xe7, 0x90, 0x46, 0xc, 0xdd);

// {E5441FD4-CE45-431E-8293-57237C81687F}
static const GUID guidImageBySel = 
{ 0xe5441fd4, 0xce45, 0x431e, { 0x82, 0x93, 0x57, 0x23, 0x7c, 0x81, 0x68, 0x7f } };
// {A00B6A27-96D9-48CE-B87E-98B44EDF61CD}
static const GUID guidRestoreSel = 
{ 0xa00b6a27, 0x96d9, 0x48ce, { 0xb8, 0x7e, 0x98, 0xb4, 0x4e, 0xdf, 0x61, 0xcd } };

ACTION_INFO_FULL(CActionImageBySel, IDS_ACTION_IMAGE_BY_SEL, -1, -1, guidImageBySel);
ACTION_INFO_FULL(CActionRestoreSel, IDS_ACTION_RESTORE_SEL, -1, -1, guidRestoreSel);

ACTION_TARGET(CActionImageBySel, szTargetAnydoc);
ACTION_TARGET(CActionRestoreSel, szTargetAnydoc);

ACTION_ARG_LIST(CActionImageBySel)
	ARG_BOOL(_T("RestoreSel"), 0)
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionRestoreSel)
END_ACTION_ARG_LIST();

CActionImageBySel::CActionImageBySel()
{
}

CActionImageBySel::~CActionImageBySel()
{
}

bool CActionImageBySel::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionImageBySel::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionImageBySel::IsAvaible()
{
	BOOL bRestoreSel = GetArgumentInt("RestoreSel");
	if (bRestoreSel)
		return true;

	sptrIDocumentSite sptrDS(m_punkTarget);
	if( sptrDS == 0 )
		return false;

	IUnknownPtr punkView = 0;
	sptrDS->GetActiveView( &punkView );
	if( punkView == 0 )
		return false;

	IViewPtr sptrView = punkView;
	
	if( sptrView == 0 )
		return 0;

	IUnknownPtr punkMult;
	sptrView->GetMultiFrame( &punkMult, 0 ); 

	if( punkMult == 0 )
		return false;

	IMultiSelectionPtr ptrFrame = punkMult;

	if( ptrFrame == 0 )
		return 0;
	
/*	DWORD dwSz = 0;
	ptrFrame->GetObjectsCount( &dwSz );

	if( !dwSz)
		return 0;*/
	

	sptrIImageView sptrIV(punkView);
//	punkView->Release();

	if( sptrIV == 0 )
		return false;

	IUnknown *pimg = NULL;
	IUnknown *pimgSel = NULL;
//	sptrIV->GetActiveImage(&pimg, &pimgSel);
	GetActiveImage(sptrIV, &pimg, &pimgSel);
	if( pimgSel == 0 )
	{
		if( pimg )
			pimg->Release();	pimg = 0;
	}

	if( pimg )
		pimg->Release();	pimg = 0;

	if( pimgSel )
		pimgSel->Release();	pimgSel= 0;

	return true;
}


bool CActionImageBySel::Invoke()
{
	BOOL bRestoreSel = GetArgumentInt("RestoreSel");
	sptrIDocumentSite sptrDS(m_punkTarget);
	if (!bool(sptrDS)) return false;
	IUnknown *punkView = 0;
	sptrDS->GetActiveView(&punkView);

	DWORD dwColorBack = ::GetValueColor(GetAppUnknown(), "\\Editor", "Back", RGB(0,0,0));

	if (punkView)
	{
		sptrIImageView sptrIV(punkView);
		punkView->Release();

		if( sptrIV == 0 )
			return false;

		IUnknown *pimg = NULL;
		IUnknown *pimgSel = NULL;
//		sptrIV->GetActiveImage(&pimg, &pimgSel);
		GetActiveImage(sptrIV, &pimg, &pimgSel);

		if( pimgSel )
		{
			int nCount = GetImagePaneCount( pimgSel );
			if( nCount == 1 )
				dwColorBack = ::GetValueColor(GetAppUnknown(), "\\Editor", "Back gray", RGB(0,0,0));
		}

		if (pimgSel || bRestoreSel)
		{
			IUnknown *punkImgNew = ::CreateTypedObject("Image");
			INamedDataObject2Ptr sptrNDO2Old(pimg);
			INamedDataObject2Ptr sptrNDO2NewObj(punkImgNew);
			GUID guidBaseKey;
			sptrNDO2Old->GetBaseKey(&guidBaseKey);
			sptrNDO2NewObj->SetBaseKey(&guidBaseKey);
			if (punkImgNew)
			{
				if (bRestoreSel)
				{
					CRect rc;
					CWalkFillInfo *pinfo = NULL;
					RestoreMask(rc, pinfo);
					CImageWrp imgNew(punkImgNew);
					CImageWrp imgOld(pimg);
					CopyPropertyBag(pimg, punkImgNew, true);
					int cx = rc.Width();
					int cy = rc.Height();
					CString sCC = imgOld.GetColorConvertor();
					imgNew.CreateNew(cx, cy, sCC, imgOld.GetColorsCount());
					int nPanes = imgOld.GetColorsCount();
					for (int k = 0; k < nPanes; k++)
					{
						CString sPane;
						sPane.Format(_T("%s-Pane%d"), sCC, k);
						//int nColorComp = g_ImageBySelProfile.GetProfileInt(NULL, sPane, 0, true);
						color nColorComp = ByteAsColor((byte)(dwColorBack>>(k*8)));
						for (int i = 0; i < cy; i++)
						{
							color *pDst = imgNew.GetRow(i, k);
							color *pSrc = imgOld.GetRow(i+rc.top, k)+rc.left;
							byte  *pMsk = pinfo->GetRowMask(i);
							for (int j = 0; j < cx; j++)
							{
								if (pMsk[j] == 255)
									pDst[j] = pSrc[j];
								else
									pDst[j] = nColorComp;
							}
						}
					}
					delete pinfo;
				}
				else
				{
					SaveMask(pimgSel);
					CImageWrp imgNew(punkImgNew);
					CImageWrp imgSel(pimgSel);
					CopyPropertyBag(pimg, punkImgNew, true);
					int cx = imgSel.GetWidth();
					int cy = imgSel.GetHeight();
					CString sCC = imgSel.GetColorConvertor();
					imgNew.CreateNew(cx, cy, sCC, imgSel.GetColorsCount());
					int nPanes = imgSel.GetColorsCount();
					for (int k = 0; k < nPanes; k++)
					{
						CString sPane;
						sPane.Format(_T("%s-Pane%d"), sCC, k);
						//int nColorComp = g_ImageBySelProfile.GetProfileInt(NULL, sPane, 0, true);
						color nColorComp = ByteAsColor((byte)(dwColorBack>>(k*8)));
						for (int i = 0; i < cy; i++)
						{
							color *pDst = imgNew.GetRow(i, k);
							color *pSrc = imgSel.GetRow(i, k);
							byte  *pMsk = imgSel.GetRowMask(i);
							for (int j = 0; j < cx; j++)
							{
								if (pMsk[j] == 255)
									pDst[j] = pSrc[j];
								else
									pDst[j] = nColorComp;
							}
						}
					}
				}
				CopyObjectNamedData(pimg, punkImgNew, _T(""), true);

				CString str_name = ::GetObjectName( pimg );

				if (pimgSel)
				{
					m_changes.RemoveFromDocData( m_punkTarget, pimgSel );
					pimgSel->Release();
				}
				if (pimg)
				{
					m_changes.RemoveFromDocData( m_punkTarget, pimg );
					pimg->Release();
				}
				if( punkImgNew )
				{
					::SetObjectName( punkImgNew, str_name );
					m_changes.SetToDocData( m_punkTarget, punkImgNew, UF_NOT_GENERATE_UNIQ_NAME );
					punkImgNew->Release();
				}
			}
		}
	}
	return true;
}


CActionRestoreSel::CActionRestoreSel()
{
}

CActionRestoreSel::~CActionRestoreSel()
{
}

bool CActionRestoreSel::DoUndo()
{
	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionRestoreSel::DoRedo()
{
	m_changes.DoRedo(m_punkTarget);
	return true;
}


bool CActionRestoreSel::Invoke()
{
	sptrIDocumentSite sptrDS(m_punkTarget);
	if (!bool(sptrDS)) return false;
	IUnknown *punkView;
	sptrDS->GetActiveView(&punkView);
	if (punkView)
	{
		sptrIImageView sptrIV(punkView);
		IUnknown *pimg = NULL;
		IUnknown *pimgSel = NULL;
		sptrIV->GetActiveImage(&pimg, &pimgSel);
		if (pimg)
		{
			CImageWrp img(pimg);
			CRect rc;
			CWalkFillInfo *pinfo = NULL;;
			RestoreMask(rc, pinfo);
			if (pinfo)
			{
				IUnknown *punkSelNew = img.CreateVImage(rc);
				if (punkSelNew)
				{
					pinfo->AttachMasksToImage(punkSelNew);
					CImageWrp imgSel(punkSelNew);
					imgSel.InitContours();
					m_changes.RemoveFromDocData(m_punkTarget, pimgSel, true );
					m_changes.SetToDocData(m_punkTarget, punkSelNew);
					punkSelNew->Release();
				}
				delete pinfo;
			}
			/*
			IUnknown *punkSelNew = ::CreateTypedObject("Image");
			if (punkSelNew)
			{
				RestoreMask(pimg, punkSelNew);
				if (pimgSel)
				{
					::DeleteObject(m_punkTarget, pimgSel);
					m_punkSel = pimgSel;
				}
				::SetValue(m_punkTarget, 0, 0, _variant_t(punkSelNew));
				punkSelNew->Release();
			}
			*/
			pimg->Release();
		}
		punkView->Release();
	}
	return true;
}
