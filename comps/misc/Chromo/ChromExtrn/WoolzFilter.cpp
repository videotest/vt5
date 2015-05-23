#include "stdafx.h"
#include "WoolzFilter.h"
#include "nameconsts.h"
#include "image5.h"
#include "impl_long.h"
#include "misc_utils.h"
#include "misc5.h"
#include "measure5.h"
#include "WoolzObject.h"
#include "StdioFile.h"
#include <tchar.h>
#include <math.h>
#include "class_utils.h"

#define COORD ICOORD
#define __NOTEXT
extern "C"
{
struct _iobuf *fStdIn;
struct _iobuf *fStdOut;
struct _iobuf *fStdErr;


#include "wstruct.h"
#define class _class
#include "chromanal.h"
#undef class

struct object *vreadobj(FILE *);
void freeobj(struct object *);
int initrasterscan(struct object *obj, register struct iwspace *iwsp, int raster);
int nextinterval(register struct iwspace *iwsp);
int initgreyscan(struct object *obj, struct iwspace* iwsp, struct gwspace *gwsp);		/* raster and tranpl of 0 provided */
int nextgreyinterval(register struct iwspace *iwsp);
int getdim(int);
}

#define readobj vreadobj

static void _make_feat_pathname(char *feat_pathname, const char *woolz_pathname)
{
	char szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	_splitpath(woolz_pathname,szDrive,szDir,szFName,NULL);
	_makepath(feat_pathname,szDrive,szDir,szFName,".feat");
}

static bool _read_feat_file(const char *woolz_pathname, int nObjects, int *pnObjectClasses)
{
	char szFeatPath[_MAX_PATH];
	_make_feat_pathname(szFeatPath, woolz_pathname);
	int ndim = getdim(0);
	double t;
	try
	{
		_StdioFile FeatFile(szFeatPath,"rt");
		int _class;
		int iObj = 0;
		while (fscanf(FeatFile.f,"%d",&_class) > 0)
		{
			_class--;
			for (int i=0; i<30; i++)
				fscanf(FeatFile.f, "%E", &t);
			if ( _class >= 100)
				_class -= 100;
			pnObjectClasses[iObj++] = _class;
			if (iObj >= nObjects)
				break;
		}
	
	}
	catch(_StdioFileException* e)
	{
		e->Delete();
		return false;
	}
	catch(...)
	{
		return false;
	}
	return true;
};

IUnknown* _CreateNamedObject(LPCTSTR szType, IUnknown *punkParent = NULL);

CWoolzFilter::CWoolzFilter()
{
	m_bstrDocumentTemplate = _bstr_t("Image");
	m_bstrFilterName = _bstr_t("Woolz files");
	m_bstrFilterExt = _bstr_t(".wz\n.woolz");
	AddDataObjectType(szArgumentTypeImage);
	AddDataObjectType(szArgumentTypeObjectList);
	AddDataObjectType(szArgumentTypeClassList);
}

CWoolzFilter::~CWoolzFilter()
{
}

bool CWoolzFilter::ReadFile( const char *pszFileName )
{
	int nObjs = 1000;
	int nFile = 0;
	try
	{
		// Prepare file stdin, stdout, stderr
		_StdioFilePtr StdIn(&fStdIn, "stdin.data", "w+t");
		_StdioFilePtr StdOut(&fStdOut, "stdout.data", "wta");
		_StdioFilePtr StdErr(&fStdErr, "stderr.data", "wta");
		// Read all data in file
		struct object *objlist[200], *obj;
		int cellkol1,cellline1,celllastkl,celllastln;
		int number = 0;
		_StdioFile File(pszFileName, "rb");
		while((obj = readobj(File.f)) != NULL) {
			if (obj->type == 1) {
				objlist[number++] = obj;
				if (number == 1) {
					cellline1 = obj->idom->line1;
					cellkol1 = obj->idom->kol1;
					celllastln = obj->idom->lastln;
					celllastkl = obj->idom->lastkl;
				} else {
					cellline1 = min(cellline1,obj->idom->line1);
					cellkol1 = min(cellkol1,obj->idom->kol1);
					celllastln = max(celllastln,obj->idom->lastln);
					celllastkl = max(celllastkl,obj->idom->lastkl);
				}
			}
		}
		// Calc objects disposition.
		BOOL bDisposeImages = ::GetValueInt(::GetAppUnknown(), "Woolz", "DisposeImages", TRUE);
		if (!bDisposeImages)
			cellline1 = cellkol1 = 0;
		int nObjectWidth = celllastkl-cellkol1+1;
		// Save all data to image.
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
		int imageWidth = bDisposeImages?nObjectWidth*number:celllastkl-cellkol1+1;
		int imageLength = celllastln-cellline1+1;
		sptrI->CreateImage(imageWidth, imageLength, _bstr_t("GRAY"),-1);
		for (int i = 0; i < imageLength; i++)
		{
			color *pDst;
			sptrI->GetRow(i,0,&pDst);
			memset(pDst, 0, imageWidth*sizeof(color));
		}
		for (i = 0; i < number; i++)
		{
			obj = objlist[i];
			int nObjX = bDisposeImages?nObjectWidth*i:0;
			int nObjY = 0;
			struct iwspace iwsp;
			struct gwspace gwsp;
//			register GREY *g;
			if (obj->vdom == NULL) {
				/* scanning routines will protect against empty objects */
				initrasterscan(obj,&iwsp,0);
				while (nextinterval(&iwsp) == 0) {
					color *pDst;
					sptrI->GetRow(nObjY+iwsp.linpos-cellline1,0,&pDst);
					memset(pDst+nObjX+iwsp.lftpos-cellkol1, 0xFF, iwsp.colrmn);
				}
			} else {
				initgreyscan(obj,&iwsp,&gwsp);
				while (nextgreyinterval(&iwsp) == 0) {
					short *pSrc = gwsp.grintptr;
					int nObjY1 = nObjY+iwsp.linpos-cellline1;
					if (nObjY1 < 0) continue;
					color *pDst;
					sptrI->GetRow(nObjY1,0,&pDst);
					int nObjX1 = nObjX+iwsp.lftpos-cellkol1;
					int nColRmn = iwsp.colrmn;
					if (nObjX1 < 0)
					{
						nColRmn -= -nObjX1;
						pSrc += -nObjX1;
						nObjX1 = 0;
					}
					for (int j = 0; j < nColRmn; j++)
						pDst[nObjX1+j] = ((color)pSrc[j])<<8;
				}
			}
		}
		// Read classes number from .feat file (if any)
		int *pClasses = new int[number];
		for (i = 0; i < number; i++) pClasses[i] = -1;
		_read_feat_file(pszFileName,number,pClasses);
		// Initialize objects list
		punk = CreateNamedObject(GetDataObjectType(1));
		INamedDataObject2Ptr sptrOL(punk);
		if(sptrOL == 0) return true;
		for (i = 0; i < number; i++)
		{
			obj = objlist[i];
			int nObjX = bDisposeImages?nObjectWidth*i:0;
			int nObjY = 0;
			if (obj->type != 1) continue;
			// Create new object
			IUnknown *punk1 = _CreateNamedObject(szArgumentTypeObject, punk);
			ICalcObjectPtr sptrCO(punk1);
			int _class = ((struct chromosome *)obj)->plist->pgroup-1;
			if (_class < 0 || _class >= 24)
				_class = pClasses[i];
			if (_class >= -1 && _class < 24)
			{
				set_object_class( sptrCO, _class );
				((struct chromosome *)obj)->plist->pgroup = _class+1;
			}
			IMeasureObjectPtr sptrMO(punk1);
			if (punk1) punk1->Release();
			if(sptrMO == 0) continue;
			// Create image for it
			IUnknown *punkImg = _CreateNamedObject(szArgumentTypeImage, NULL);
			IImage3Ptr sptrI(punkImg);
			int nLines = obj->idom->lastln-obj->idom->line1+1;
			int nColumns = obj->idom->lastkl-obj->idom->kol1+1;
			sptrI->CreateImage(nColumns, nLines, _bstr_t("GRAY"),-1);
			sptrI->InitRowMasks();
			POINT pt;
			pt.x = nObjX+obj->idom->kol1-cellkol1;
			pt.y = nObjY+obj->idom->line1-cellline1;
			sptrI->SetOffset(pt, FALSE);
			// Initialize this image.
			// Fill background.
			for (int y = 0; y < nLines; y++)
			{
				color *pDst;
				sptrI->GetRow(y,0,&pDst);
				memset(pDst, 0, nColumns*sizeof(color));
			}
			// Set image data.
			struct iwspace iwsp;
			struct gwspace gwsp;
			if (obj->vdom == NULL) {
				/* scanning routines will protect against empty objects */
				initrasterscan(obj,&iwsp,0);
				while (nextinterval(&iwsp) == 0) {
					color *pDst;
					sptrI->GetRow(iwsp.linpos-obj->idom->line1,0,&pDst);
					memset(pDst+iwsp.lftpos-obj->idom->kol1, 0xFF, iwsp.colrmn*sizeof(color));
				}
			} else {
				initgreyscan(obj,&iwsp,&gwsp);
				while (nextgreyinterval(&iwsp) == 0) {
					short *pSrc = gwsp.grintptr;
					color *pDst;
					sptrI->GetRow(iwsp.linpos-obj->idom->line1,0,&pDst);
					for (int j = 0; j < iwsp.colrmn; j++)
						pDst[iwsp.lftpos-obj->idom->kol1+j] = ((color)pSrc[j])<<8;
				}
			}
			// Set binary data.
			struct intervaldomain *idom = obj->idom;
			for (y = 0; y < nLines; y++)
			{
				BYTE *pDstMask;
				sptrI->GetRowMask(y, &pDstMask);
				memset(pDstMask, 0, nColumns);
				int nInts = idom->intvlines[y].nintvs;
				struct interval *itvl = idom->intvlines[y].intvs;;
				for (int nInt = 0; nInt < nInts; nInt++)
				{
					int x = itvl[nInt].ileft;
					int cx = itvl[nInt].iright-itvl[nInt].ileft+1;
					if (x >= 0 && x <= nColumns && x+cx >= 0 && x+cx <= nColumns)
						memset(pDstMask+x, 0xFF, cx);
					else
					{
						static int out = 0;
						if (out++ == 0)
							MessageBox(NULL, "Not supported woolz data", "ChromExtrn", MB_OK|MB_ICONEXCLAMATION);
					}
				}
			}
			// Init contours
			sptrI->InitContours();
			sptrMO->SetImage(punkImg);
			if (punkImg) punkImg->Release();
			// Save object in woolz form
			_bstr_t bstrWO(szTypeWoolzObject);
			IUnknown *punkWoolz = CreateTypedObject(bstrWO);
			if (punkWoolz)
			{
				sptrINamedDataObject2 sptrNO2 = punkWoolz;
				sptrNO2->SetParent(punk1, 0);
				objlist[i] = NULL;
				sptrIWoolzObject WO(punkWoolz);
				WO->SetWoolzObject(obj);
				punkWoolz->Release();
			}
		}
		if (punk) punk->Release();
		delete pClasses;

		// free all objects
		for (i = 0; i < number; i++)
		{
			obj = objlist[i];
			if (obj) freeobj(obj);
		}
	}
	catch(TerminateLongException* e)
	{
		e->Delete();
		return false;
	}
	catch(_StdioFileException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	return true;
}

bool CWoolzFilter::WriteFile( const char *pszFileName )
{
	return false;
}

bool CWoolzFilter::_InitNew()
{
	return true;
}

