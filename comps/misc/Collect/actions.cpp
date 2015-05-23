#include "stdafx.h"
#include "actions.h"
#include "resource.h"
#include "\vt5\awin\misc_ptr.h"
#include "\vt5\awin\misc_dbg.h"
#include <math.h>
#ifdef _DEBUG
#define DebugDraw(x,y) DbgDraw(x,y);
#else
#define DebugDraw() __asm nop
#endif
#include "\vt5\awin\profiler.h"
#undef _assert
#include <dvec.h>


#pragma warning( disable : 4244)
#pragma warning( disable : 4018)
template <class T>
int MaskZone<T>::m_count =0;

template <class T>
CompareFn MaskZone<T>::m_pFnCompare =0;


void DbgDraw(CImageWrp& image, int step)
{
	int cx = image.GetWidth();
	int cy = image.GetHeight();
	int cc = image.GetColorsCount();
	int X = cx/step;
	int Y = cy/step;

	color* col ;
	for(int y=0;y<cy;y++)
	{
		for(int c=0;c<cc;c++)
		{
			col=image.GetRow(y,c);
			if(y%step ==0)
				memset(col, -1, sizeof(col)*cx);
			for(int x=0;x<cx;x++)
			{
				if(x%step==0)	
					col[x] = -1;
			}
		}
	}


};


const char* pszSection = "Collect";



//[ag]1. dyncreate
IMPLEMENT_DYNCREATE(CActionCollectByMasks, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCollectByPoints, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCollectByDisp, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionAlignImages, CCmdTargetEx);

// olecreate 
// {38835A1B-49F7-491d-B843-25EB1F7D2F28}
GUARD_IMPLEMENT_OLECREATE(CActionCollectByMasks, "Collect.CollectByMasks",
0x38835a1b, 0x49f7, 0x491d, 0xb8, 0x43, 0x25, 0xeb, 0x1f, 0x7d, 0x2f, 0x28);
// {03C7FA90-7DEA-4839-AB97-5B46AF300DDA}
GUARD_IMPLEMENT_OLECREATE(CActionCollectByPoints, "Collect.CollectByPoints",
0x3c7fa90, 0x7dea, 0x4839, 0xab, 0x97, 0x5b, 0x46, 0xaf, 0x30, 0xd, 0xda);
// {26902A49-A749-464d-B6A8-CAFF2894974D}
GUARD_IMPLEMENT_OLECREATE(CActionCollectByDisp, "Collect.CollectByDisp", 
0x26902a49, 0xa749, 0x464d, 0xb6, 0xa8, 0xca, 0xff, 0x28, 0x94, 0x97, 0x4d);
// {226D085C-75C3-4b09-A0EE-A085F1D65654}
GUARD_IMPLEMENT_OLECREATE(CActionAlignImages, "Collect.AlignImages", 
						  0x226d085c, 0x75c3, 0x4b09, 0xa0, 0xee, 0xa0, 0x85, 0xf1, 0xd6, 0x56, 0x54);

// guidinfo 
// {4496B86C-FC85-4518-9EE5-86505AFCF0FD}
static const GUID guidCollectByMasks =
{ 0x4496b86c, 0xfc85, 0x4518, { 0x9e, 0xe5, 0x86, 0x50, 0x5a, 0xfc, 0xf0, 0xfd } };
// {B8AB507E-D6D8-413b-BE7F-97FFA4091ADF}
static const GUID guidCollectByPoints =
{ 0xb8ab507e, 0xd6d8, 0x413b, { 0xbe, 0x7f, 0x97, 0xff, 0xa4, 0x9, 0x1a, 0xdf } };
// {F588AF32-DB88-4c05-B8D0-0781019917D4}
static const GUID guidCollectByDisp = 
{ 0xf588af32, 0xdb88, 0x4c05, { 0xb8, 0xd0, 0x7, 0x81, 0x1, 0x99, 0x17, 0xd4 } };
// {AE649F55-8609-4f16-931B-7238ECD6A523}
static const GUID guidAlignImages = 
{ 0xae649f55, 0x8609, 0x4f16, { 0x93, 0x1b, 0x72, 0x38, 0xec, 0xd6, 0xa5, 0x23 } };



//[ag]6. action info

ACTION_INFO_FULL(CActionCollectByMasks, IDS_ACTION_COLLECT_BY_MASKS, -1, -1, guidCollectByMasks);
ACTION_INFO_FULL(CActionCollectByPoints, IDS_ACTION_COLLECT_BY_POINTS, -1, -1, guidCollectByPoints);
ACTION_INFO_FULL(CActionCollectByDisp, IDS_ACTION_COLLECT_BY_DISP, -1, -1, guidCollectByDisp);
ACTION_INFO_FULL(CActionAlignImages, IDS_ACTION_ALIGN_IMAGES, -1, -1, guidAlignImages);
//[ag]7. targets

ACTION_TARGET(CActionCollectByMasks, szTargetAnydoc);
ACTION_TARGET(CActionCollectByPoints, szTargetAnydoc);
ACTION_TARGET(CActionCollectByDisp, szTargetAnydoc);
ACTION_TARGET(CActionAlignImages, szTargetAnydoc);
//[ag]9. implementation

ACTION_ARG_LIST(CActionCollectByPoints)
	RES_IMAGE( _T("CollectedByPoints") )
	ARG_IMAGE( _T("Image0") )
	ARG_IMAGE( _T("Image1") )
	ARG_IMAGE( _T("Image2") )
	ARG_IMAGE( _T("Image3") )
	ARG_IMAGE( _T("Image4") )
	ARG_IMAGE( _T("Image5") )
	ARG_IMAGE( _T("Image6") )
	ARG_IMAGE( _T("Image7") )
	ARG_IMAGE( _T("Image8") )
	ARG_IMAGE( _T("Image9") )
	ARG_IMAGE( _T("Image10") )
	ARG_IMAGE( _T("Image11") )
	ARG_IMAGE( _T("Image12") )
	ARG_IMAGE( _T("Image13") )
	ARG_IMAGE( _T("Image14") )
	ARG_IMAGE( _T("Image15") )
	ARG_IMAGE( _T("Image16") )
	ARG_IMAGE( _T("Image17") )
	ARG_IMAGE( _T("Image18") )
	ARG_IMAGE( _T("Image19") )
	ARG_IMAGE( _T("Image20") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionCollectByMasks)
	RES_IMAGE( _T("Focus Result") )
	ARG_IMAGE( _T("Image0") )
	ARG_IMAGE( _T("Image1") )
	ARG_IMAGE( _T("Image2") )
	ARG_IMAGE( _T("Image3") )
	ARG_IMAGE( _T("Image4") )
	ARG_IMAGE( _T("Image5") )
	ARG_IMAGE( _T("Image6") )
	ARG_IMAGE( _T("Image7") )
	ARG_IMAGE( _T("Image8") )
	ARG_IMAGE( _T("Image9") )
	ARG_IMAGE( _T("Image10") )
	ARG_IMAGE( _T("Image11") )
	ARG_IMAGE( _T("Image12") )
	ARG_IMAGE( _T("Image13") )
	ARG_IMAGE( _T("Image14") )
	ARG_IMAGE( _T("Image15") )
	ARG_IMAGE( _T("Image16") )
	ARG_IMAGE( _T("Image17") )
	ARG_IMAGE( _T("Image18") )
	ARG_IMAGE( _T("Image19") )
	ARG_IMAGE( _T("Image20") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionCollectByDisp)
	RES_IMAGE( _T("Focus Result") )
	ARG_IMAGE( _T("Image0") )
	ARG_IMAGE( _T("Image1") )
	ARG_IMAGE( _T("Image2") )
	ARG_IMAGE( _T("Image3") )
	ARG_IMAGE( _T("Image4") )
	ARG_IMAGE( _T("Image5") )
	ARG_IMAGE( _T("Image6") )
	ARG_IMAGE( _T("Image7") )
	ARG_IMAGE( _T("Image8") )
	ARG_IMAGE( _T("Image9") )
	ARG_IMAGE( _T("Image10") )
	ARG_IMAGE( _T("Image11") )
	ARG_IMAGE( _T("Image12") )
	ARG_IMAGE( _T("Image13") )
	ARG_IMAGE( _T("Image14") )
	ARG_IMAGE( _T("Image15") )
	ARG_IMAGE( _T("Image16") )
	ARG_IMAGE( _T("Image17") )
	ARG_IMAGE( _T("Image18") )
	ARG_IMAGE( _T("Image19") )
	ARG_IMAGE( _T("Image20") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionAlignImages)
ARG_IMAGE( _T("Image0") )
ARG_IMAGE( _T("Image1") )
ARG_IMAGE( _T("Image2") )
ARG_IMAGE( _T("Image3") )
ARG_IMAGE( _T("Image4") )
ARG_IMAGE( _T("Image5") )
ARG_IMAGE( _T("Image6") )
ARG_IMAGE( _T("Image7") )
ARG_IMAGE( _T("Image8") )
ARG_IMAGE( _T("Image9") )
ARG_IMAGE( _T("Image10") )
ARG_IMAGE( _T("Image11") )
ARG_IMAGE( _T("Image12") )
ARG_IMAGE( _T("Image13") )
ARG_IMAGE( _T("Image14") )
ARG_IMAGE( _T("Image15") )
ARG_IMAGE( _T("Image16") )
ARG_IMAGE( _T("Image17") )
ARG_IMAGE( _T("Image18") )
ARG_IMAGE( _T("Image19") )
ARG_IMAGE( _T("Image20") )
END_ACTION_ARG_LIST();
//////////////////////////////////////////////////////////////////////
//CActionCollectByMasks implementation
CActionCollectByMasks::CActionCollectByMasks()
{
}

CActionCollectByMasks::~CActionCollectByMasks()
{
}

bool CActionCollectByMasks::IsAvaible()
{
	long nArgs = 0;
	_Init();

	POSITION pos = m_arguments.GetHeadPosition();

	CFilterArgument	*parg = 0;
	
	while(pos)
	{
		parg = m_arguments.GetNext(pos);

		if(parg->m_bInArgument && parg->m_strArgType == "Image")
		{
			nArgs++;
			if(nArgs > 1)
				break;
		}
	}

	return nArgs > 1;
}

bool CActionCollectByMasks::InvokeFilter()
{
	int	nMaxSrcImages = 0;
	sptrIActionInfo2	sptrA(m_pActionInfo);
	sptrA->GetArgsCount( &nMaxSrcImages );
	nMaxSrcImages--;

	CImageWrp* imageSrc = new CImageWrp[nMaxSrcImages];

	CImageWrp imageDest(GetDataResult());

	WORD nCX = 65535;
	WORD nCY = 65535;

	int nSrcImages = 0;
	CString sCC;
	bool bDiffCC = false;

	for(long i = 0; i < nMaxSrcImages; i++)
	{
		CString str = "";
		str.Format("Image%d", i);
		IUnknown* punkImg = GetDataArgument(str);

		if(punkImg)
		{
			ASSERT(GetParentKey(punkImg) == INVALID_KEY);

			imageSrc[nSrcImages] = CImageWrp(punkImg);

			if (sCC.IsEmpty())
				sCC = imageSrc[nSrcImages].GetColorConvertor();
			else
			{
				if (sCC != imageSrc[nSrcImages].GetColorConvertor())
					bDiffCC = true;
			}

			long nCXi = imageSrc[nSrcImages].GetWidth();
			long nCYi = imageSrc[nSrcImages].GetHeight();

			nCX = min(nCXi, nCX);
			nCY = min(nCYi, nCY);

			nSrcImages++;
		}
		else 
			break;
	}

	if(nSrcImages < 2)
	{
		delete []imageSrc;
		return false;
	}

	if (bDiffCC)
	{
		AfxMessageBox(IDS_DIFFERENT_MODEL, MB_OK|MB_ICONEXCLAMATION);
		delete []imageSrc;
		return false;
	}

	long nArea = 2*GetValueInt(GetAppUnknown(), "Collect\\CollectByMasks", "AreaSize", 10);// размер маски

	RECT* rects = new RECT[nSrcImages];
	BOOL bAlignedSerie = SubstituteByAlignedImages(imageSrc, nSrcImages, rects);

	nCX = imageSrc[0].GetWidth();
	nCY = imageSrc[0].GetHeight();

	imageDest.CreateCompatibleImage(imageSrc[0]);

	long activcolors = imageSrc[0].GetColorsCount();




	long hcx = (long)(nCX/nArea)+1;
	long hcy = (long)(nCY/nArea)+1;

	color	*psrc;
	color	*pRowUp;
	color	*pRowDown;
	color	*pDest;


	StartNotification(long(nCX/nArea) + 1, 3);

	MaskZone<DWORD>* mz =0;
	int* ppix =0;
	if(bAlignedSerie)
	{
		mz = new MaskZone<DWORD>[(hcx+1)*(hcy+1)];
		mz[0].Init(compareDWORD, nSrcImages); 
	}
	else  
		{
		ppix= new int[(hcx+1)*(hcy+1)];
		memset(ppix,0,(hcx+1)*(hcy+1)*sizeof(long));
		}

	CRect R(0,0,(nCX+ nArea-1)/nArea*nArea,(nCY+ nArea-1)/nArea*nArea);
	for(long x = 0; x <= long(nCX/nArea); x++)
	{
		long x1= x*nArea;
		long x2= x*nArea + nArea;



		for(long y = 0; y <= long(nCY/nArea); y++)
		{
			long y1= y*nArea;
			long y2= y*nArea + nArea;
			DWORD rKof = 0;

			for(int i = 0; i < nSrcImages; i++)
			{

				if(bAlignedSerie)
				{
					CRect rectValid(x1-nArea, y1-nArea, x2+nArea, y2+nArea);
					rectValid.IntersectRect(rectValid,R);
					CRect inRect,imageRect;
					imageRect =rects[i];
					int w = imageRect.right- imageRect.left;
					int h = imageRect.bottom- imageRect.top;
					imageRect.right = (imageRect.right + nArea-1)/nArea*nArea;
					imageRect.bottom =(imageRect.bottom + nArea-1)/nArea*nArea;

					inRect.IntersectRect(rectValid, imageRect);
					if(inRect!=rectValid) continue;
				}

				long cx = imageSrc[i].GetWidth();
				long cy = imageSrc[i].GetHeight();
				long colors = imageSrc[i].GetColorsCount();
				if (colors == activcolors)
				{
					if (x <= long(cx/nArea) && y <= long(cy/nArea))
					{	    
						DWORD Kof=0;
						int n=0;

						if (x2 >= cx) 
							x2 = cx-1;

						if (y2>=cy) 
							y2 = cy-1;


						
						for (long y0 = y1+1; y0 < y2-1; y0++) 
						{
							for(long c = 0; c  < colors; c++ )
							{
								psrc = imageSrc[i].GetRow(y0, c);
								pRowUp = imageSrc[i].GetRow(y0+1, c);
								pRowDown = imageSrc[i].GetRow(y0-1, c);
								// вычисление суммы вторых производных в каждой маске
								for (long x0 = x1+1; x0 < x2-1; x0++) 
								{
									DWORD a = psrc[x0];
									Kof += abs(int(2*a - psrc[x0+1] - psrc[x0-1])); 
									a = 2*a - pRowUp[x0]; 
									Kof += abs(int(a - pRowDown[x0]));
									n++;
								}
							}
		
						}
						if(bAlignedSerie)
						{
      					
							if(n) mz[y*hcx+x].InitCell(rects[i],i , Kof/n);
						}
						else if (Kof>rKof) // выбор самой контрастной маски из входных изображ.
						{
							rKof = Kof;
							ppix[y*hcx+x] = i;
						}

					}
				}
			}
			if(bAlignedSerie) mz[y*hcx+x].sort();
		}

		Notify(x);
	}



	NextNotificationStage();

	// cглаживание между масками
	if(!bAlignedSerie)
	for(long y = 0; y <= long(nCY/nArea); y++)	
	{
		long y1= y*nArea;
		long y2= (y+1)*nArea;

	for(x = 0; x <= long(nCX/nArea); x++)
	{
			long j =ppix[y*hcx+x]; 

		long x1= x*nArea;
		long x2= (x+1)*nArea;

    
	  
			if (x2 > nCX) 
				x2 = nCX;
			if (y2 > nCY) 
				y2 = nCY;

			for (long y0 = y1; y0 < y2; y0++)
			{
				for (long c=0; c < activcolors; c++)	
				{
					pDest = imageDest.GetRow(y0, c);

					psrc = imageSrc[j].GetRow(y0, c);

					for (long x0 = x1; x0 < x2; x0++) 
						pDest[x0] = psrc[x0];
				}
			}
		}
		Notify(y*nCX/nCY);
	}

	NextNotificationStage();
	DWORD prevColor=0;
	Filler fil(imageDest);
	for(x = 0; x <= (long)(nCX/nArea) ; x++) 
	{
		long x1 = long(x*nArea - nArea/2);
		long x2 = long(x*nArea + nArea/2);


		for(long y = 0; y <= (long)(nCY/nArea); y++) 	
	{
			long j1,j2,j3,j4;

			if(!bAlignedSerie)
		{
				j1 = ppix[y*hcx+x];
				j2 = ppix[y*hcx+x+1];
				j3 = ppix[(y+1)*hcx+x];
				j4 = ppix[(y+1)*hcx+x+1];
			}


			long y1 = long(y*nArea - nArea/2);
			long y2 = long(y*nArea + nArea/2);
    	  

			x1=max(x1,0); 
			y1=max(y1,0);
			x2=min(x2,nCX);
			y2=min(y2,nCY);
			if(nCY-y2<=nArea/2) 
				y2=nCY;
			if(nCX-x2<=nArea/2) 
				x2 = nCX;
			if(nCY-y1<=nArea/2 || nCX-x1<=nArea/2) continue;
				 
			for(long y0 = y1; y0 < y2; y0++)
			{
				for (long c=0; c < activcolors; c++)	
				{
					color* psrc1, *psrc2, *psrc3, *psrc4; 
					if(!bAlignedSerie)
					{
						psrc1= imageSrc[j1].GetRow(y0, c);
						psrc2= imageSrc[j2].GetRow(y0, c);
						psrc3= imageSrc[j3].GetRow(y0, c);
						psrc4= imageSrc[j4].GetRow(y0, c);
					}
	
					pDest = imageDest.GetRow(y0, c);

					DWORD dwY1 = abs(y0-y1);
					DWORD dwY2 = abs(y0-y2);

					for(long x0 = x1; x0 < x2; x0++) 
					{
						if(bAlignedSerie)
						{

							j1 = mz[y*hcx+x].GetBest(x0,y0);
							j2 = mz[y*hcx+x+1].GetBest(x0,y0);
							j3 = mz[(y+1)*hcx+x].GetBest(x0,y0);
							j4 = mz[(y+1)*hcx+x+1].GetBest(x0,y0);

							psrc1 = (j1==-1 ?pDest:imageSrc[j1].GetRow(y0, c));
							psrc2 = (j2==-1 ?pDest:imageSrc[j2].GetRow(y0, c));
							psrc3 = (j3==-1 ?pDest:imageSrc[j3].GetRow(y0, c));
							psrc4 = (j4==-1 ?pDest:imageSrc[j4].GetRow(y0, c));

						}
						int k1 = (j1!=-1 ? min(abs(x0-x2),dwY2):0);
						int k2 = (j2!=-1 ? min(abs(x0-x1),dwY2):0);
						int k3 = (j3!=-1 ? min(abs(x0-x2),dwY1):0);
						int k4 = (j4!=-1 ? min(abs(x0-x1),dwY1):0);

						if(k1+k2+k3+k4 ==0) 
						{
							pDest[x0] = 1;
							continue;
						}
						DWORD a = (k1*psrc1[x0] + k2*psrc2[x0] + k3*psrc3[x0] + k4*psrc4[x0])/(k1+k2+k3+k4);
						pDest[x0] = (color)max(0, min(65535, a));

					}
				}
			}
		}

		Notify(x);
	}
	int cc = imageDest.GetColorsCount();
	for(int y = 0; y < nCY; y++)
		for(int c=0;c<cc;c++)
		{
			color* row = imageDest.GetRow(y,c);
			for(int x = 0; x < nCX ; x++) 
			{
				if(row[x]==1 && x-1>0 && x+1<nCX)
				{
					if(row[x-1]!=1 || row[x+1]!=1) fil.AddToQueue(x,y,c);
				}
			}
		}
	fil.Process();
	FinishNotification();
	/*DebugDraw(imageDest, nArea)*/
	delete []imageSrc;
	delete[] rects;
	if(bAlignedSerie) delete[] mz;
	else delete[] ppix;

	return true;
	


}
//////////////////////////////////////////////////////////////////////
//CActionCollectByPoints implementation
CActionCollectByPoints::CActionCollectByPoints()
{
}

CActionCollectByPoints::~CActionCollectByPoints()
{
}

bool CActionCollectByPoints::IsAvaible()
{
	long nArgs = 0;
	_Init();

	POSITION pos = m_arguments.GetHeadPosition();

	CFilterArgument	*parg = 0;
	
	while(pos)
	{
		parg = m_arguments.GetNext(pos);

		if(parg->m_bInArgument && parg->m_strArgType == "Image")
		{
			nArgs++;
			if(nArgs > 1)
				break;
		}
	}

	return nArgs > 1;
}

bool CActionCollectByPoints::InvokeFilter()
{
	int	nMaxSrcImages = 0;
	sptrIActionInfo2	sptrA(m_pActionInfo);
	sptrA->GetArgsCount( &nMaxSrcImages );
	nMaxSrcImages--;

	CImageWrp* imageSrc = new CImageWrp[nMaxSrcImages];

	CImageWrp imageDest(GetDataResult());

	WORD nCX = 65535;
	WORD nCY = 65535;

	int nSrcImages = 0;
	CString sCC;
	bool bDiffCC = false;

	for(long i = 0; i < nMaxSrcImages; i++)
	{
		CString str = "";
		str.Format("Image%d", i);
		IUnknown* punkImg = GetDataArgument(str);

		if(punkImg)
		{
			nSrcImages++;

			imageSrc[i] = punkImg;

			if (sCC.IsEmpty())
				sCC = imageSrc[i].GetColorConvertor();
			else
			{
				if (sCC != imageSrc[i].GetColorConvertor())
					bDiffCC = true;
			}

			long nCXi = imageSrc[i].GetWidth();
			long nCYi = imageSrc[i].GetHeight();

			nCX = min(nCXi, nCX);
			nCY = min(nCYi, nCY);
		}
		else 
			break;
	}

	if(nSrcImages < 2)
	{
		delete []imageSrc;
		return false;
	}

	if (bDiffCC)
	{
		AfxMessageBox(IDS_DIFFERENT_MODEL, MB_OK|MB_ICONEXCLAMATION);
		delete []imageSrc;
		return false;
	}
	
	RECT* rects = new RECT[nSrcImages];
	BOOL bAlignedSerie = SubstituteByAlignedImages(imageSrc,nSrcImages,rects);
	imageDest.CreateCompatibleImage(imageSrc[0]);

	nCY = imageDest.GetHeight();
	nCX = imageDest.GetWidth();

	StartNotification(nCY);

	
	color	**psrc = new color*[nSrcImages];
	color	**pRowUp = new color*[nSrcImages];
	color	**pRowDown = new color*[nSrcImages];
	color	*pdst = 0;
	

	long activcolors = imageSrc[0].GetColorsCount();

	const int nArea = 3;


	for(long y = 0; y < nCY; y++)
	{
		for(long c = 0; c  < activcolors; c++ )
		{
			for(int i = 0; i < nSrcImages; i++)
			{
				psrc[i] = imageSrc[i].GetRow(y, c);

				if(y > nArea)
				{
					pRowUp[i] = imageSrc[i].GetRow(y-nArea, c);
				}

				if (y < nCY-nArea-1)
				{
					pRowDown[i] = imageSrc[i].GetRow(y+nArea, c);
				}
			}

			pdst = imageDest.GetRow(y, c);
			
			for(long x = 0; x < nCX; x++, pdst++)
			{
				// производна€ в каждой точке

				color maxColor = *psrc[0];
				BOOL bAssigned =0; 
				DWORD rKof = 0;

				for(int i = 0; i < nSrcImages; i++)
				{

					if(bAlignedSerie)
						if(!PtInRect(&rects[i], CPoint(x,y))) 
						{
							psrc[i]++;
							pRowDown[i]++;
							pRowUp[i]++;
							continue;
						}
					DWORD Kof = 0;

					if (x > nArea)
						Kof = max(abs(*psrc[i]-*(psrc[i]-nArea)), Kof);
					if (x < nCX-nArea-1)
						Kof = max(abs(*psrc[i]-*(psrc[i]+nArea)), Kof);

					if (y > nArea)
					{
						Kof = max(abs(*psrc[i]-*pRowUp[i]), Kof);
						if (x > nArea)
							Kof = max(abs(*psrc[i]-*(pRowUp[i]-nArea))/1.4142135, Kof);
						if (x < nCX-nArea-1)
							Kof = max(abs(*psrc[i]-*(pRowUp[i]+nArea))/1.4142135, Kof);
					}

					if (y < nCY-nArea-1)
					{
						Kof = max(abs(*psrc[i]-*pRowDown[i]), Kof);
						if (x > nArea)
							Kof = max(abs(*psrc[i]-*(pRowDown[i]-nArea))/1.4142135, Kof);
						if (x < nCX-nArea-1)
							Kof = max(abs(*psrc[i]-*(pRowDown[i]+nArea))/1.4142135, Kof);
					}
					
						if (Kof>=rKof) // выбор самой контрастной точки из входных изображ.
					{	
						rKof=Kof;
						maxColor = *psrc[i];
							bAssigned = TRUE; 
					}
					psrc[i]++;
					pRowDown[i]++;
					pRowUp[i]++;

				}

				//if (rKof != 0) 	     
				if(bAssigned) *pdst = maxColor;
				else *pdst = get_owner_color(x,y,c,imageSrc,rects,nSrcImages);
			}
		}

		Notify( y );
	}

	FinishNotification();

	delete []imageSrc;
	delete[] rects;

	delete psrc;
	delete pRowUp;
	delete pRowDown;
	
	
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionCollectByDisp implementation

#if 0
#define MEMUSE(type, count) \
	{ \
	FILE *f = fopen("c:\\temp\\memlog.txt", "a"); \
	fprintf(f, "%i\t%i\t// %u %s\n", __LINE__, count*sizeof(type), count, #type ); \
	fclose(f); \
	};		

#define TIME(name, time) \
	{ \
	FILE *f = fopen("c:\\temp\\timelog.txt", "a"); \
	fprintf(f, "%s\t %d\n", name, time); \
	fclose(f); \
	};
#else
#define MEMUSE(type, count)
#define TIME(name, time)
#endif

CActionCollectByDisp::CActionCollectByDisp()
{
}

CActionCollectByDisp::~CActionCollectByDisp()
{
}

bool CActionCollectByDisp::IsAvaible()
{
	long nArgs = 0;
	_Init();

	POSITION pos = m_arguments.GetHeadPosition();

	CFilterArgument	*parg = 0;
	
	while(pos)
	{
		parg = m_arguments.GetNext(pos);

		if(parg->m_bInArgument && parg->m_strArgType == "Image")
		{
			nArgs++;
			if(nArgs > 1)
				break;
		}
	}

	return nArgs > 1;
}

template <class Type> bool FastAvrg(unsigned long *ArrSrc, unsigned long *ArrRlt, int cnAreaLength, WORD nCX, WORD nCY)
 {
  if (ArrSrc == NULL || ArrRlt == NULL || cnAreaLength == 0 || cnAreaLength > min(nCX, nCY)/4) return (false);

  unsigned long *ArrTmp = new unsigned long [nCX*nCY]; 
  int nHalfAreaLength = (int)cnAreaLength/2;
  Type *FirstHSegmentSumm = new Type [nCY];	 
  Type *FirstVSegmentSumm = new Type [nCX];	

  try 
  { 	
	// по горизонтальным сегментам
	for (long y = 0; y < nCY; y++)
		{
			FirstHSegmentSumm[y] = 0;
			for (int x = 0; x < cnAreaLength; x++)
				FirstHSegmentSumm[y] += ArrSrc[nCX*y + x];
			ArrTmp[nCX*y + nHalfAreaLength] = FirstHSegmentSumm[y]/cnAreaLength;
		}
	  
	for (long y = 0; y < nCY; y++)
		{
			Type CurrentSumm = FirstHSegmentSumm[y]; 
			for (long x = nHalfAreaLength + 1; x < nCX - nHalfAreaLength; x++)
			{
				CurrentSumm += ArrSrc[nCX*y + x + nHalfAreaLength];
				CurrentSumm -= ArrSrc[nCX*y + x - (nHalfAreaLength + 1)];
				ArrTmp[nCX*y + x] =  CurrentSumm/cnAreaLength;
			}
		}

	
	// по вертикальным сегментам
	for (long x = 0; x < nCX; x++)
		{
			FirstVSegmentSumm[x] = 0;
			for (int y = 0; y < cnAreaLength; y++)
				FirstVSegmentSumm[x] += ArrTmp[nCX*y + x];
			ArrRlt[nCX*y + nHalfAreaLength] = FirstVSegmentSumm[x]/cnAreaLength;
		}
	  
	for (long x = 0; x < nCX; x++)
		{
	        Type CurrentSumm = FirstVSegmentSumm[x]; 
			for (long y = nHalfAreaLength + 1; y < nCY - nHalfAreaLength; y++)
			{
				CurrentSumm += ArrTmp[nCX*(y + nHalfAreaLength) + x];
				CurrentSumm -= ArrTmp[nCX*(y - (nHalfAreaLength + 1)) + x];
				ArrRlt[nCX*y + x] = CurrentSumm/cnAreaLength;
			}
		}
  }
  catch (...)
  {
	delete []ArrTmp; 
	delete []FirstHSegmentSumm;	
	delete []FirstVSegmentSumm; 
	return (false);
  }
 
  delete []ArrTmp; 
  delete []FirstHSegmentSumm; 
  delete []FirstVSegmentSumm; 
  return (true);
 }

template <class Type> bool FastAvrg1(unsigned long *ArrSrc, unsigned long *ArrRlt, int cnAreaLength, WORD nCX, WORD nCY)
 {
  if (ArrSrc == NULL || ArrRlt == NULL || cnAreaLength == 0 || cnAreaLength > min(nCX, nCY)/4) return (false);

  unsigned long *ArrTmp = new unsigned long [nCX*nCY]; 
  int nHalfAreaLength = (int)cnAreaLength/2;
  Type *FirstHSegmentSumm = new Type [nCY];	 
  Type *FirstVSegmentSumm = new Type [nCX];	

  try 
  { 	
	// по горизонтальным сегментам
	for (long y = 0; y < nCY; y++)
		{
			FirstHSegmentSumm[y] = 0;
			for (int x = 0; x < cnAreaLength; x++)
				FirstHSegmentSumm[y] += ArrSrc[nCX*y + x]>>2;
			ArrTmp[nCX*y + nHalfAreaLength] = FirstHSegmentSumm[y]/cnAreaLength;
		}
	  
	for (long y = 0; y < nCY; y++)
		{
			Type CurrentSumm = FirstHSegmentSumm[y]; 
			for (long x = nHalfAreaLength + 1; x < nCX - nHalfAreaLength; x++)
			{
				CurrentSumm += ArrSrc[nCX*y + x + nHalfAreaLength]>>2;
				CurrentSumm -= ArrSrc[nCX*y + x - (nHalfAreaLength + 1)]>>2;
				ArrTmp[nCX*y + x] =  (CurrentSumm/cnAreaLength)<<2;
			}
		}

	
	// по вертикальным сегментам
	for (long x = 0; x < nCX; x++)
		{
			FirstVSegmentSumm[x] = 0;
			for (int y = 0; y < cnAreaLength; y++)
				FirstVSegmentSumm[x] += ArrTmp[nCX*y + x]>>2;
			ArrRlt[nCX*y + nHalfAreaLength] = (FirstVSegmentSumm[x]/cnAreaLength)<<2;
		}
	  
	for (long x = 0; x < nCX; x++)
		{
	        Type CurrentSumm = FirstVSegmentSumm[x]; 
			for (long y = nHalfAreaLength + 1; y < nCY - nHalfAreaLength; y++)
			{
				CurrentSumm += ArrTmp[nCX*(y + nHalfAreaLength) + x]>>2;
				CurrentSumm -= ArrTmp[nCX*(y - (nHalfAreaLength + 1)) + x]>>2;
				ArrRlt[nCX*y + x] = (CurrentSumm/cnAreaLength)<<2;
			}
		}
  }
  catch (...)
  {
	delete []ArrTmp; 
	delete []FirstHSegmentSumm;	
	delete []FirstVSegmentSumm; 
	return (false);
  }
 
  delete []ArrTmp; 
  delete []FirstHSegmentSumm; 
  delete []FirstVSegmentSumm; 
  return (true);
 }

bool CActionCollectByDisp::InvokeFilter()
{										        
	int	nMaxSrcImages = 0;
	sptrIActionInfo2	sptrA(m_pActionInfo);
	sptrA->GetArgsCount( &nMaxSrcImages );
	nMaxSrcImages--;

	CImageWrp* imageSrc = new CImageWrp[nMaxSrcImages];

	CImageWrp imageDest(GetDataResult());

	WORD nCX = 65535;
	WORD nCY = 65535;

	int nSrcImages = 0;
	CString sCC;
	bool bDiffCC = false;

	for(long i = 0; i < nMaxSrcImages; i++)
	{
		CString str = "";
		str.Format("Image%d", i);
		IUnknown* punkImg = GetDataArgument(str);

		if(punkImg)
		{
			nSrcImages++;

			imageSrc[i] = CImageWrp(punkImg);

			if (sCC.IsEmpty())
				sCC = imageSrc[i].GetColorConvertor();
			else
			{
				if (sCC != imageSrc[i].GetColorConvertor())
					bDiffCC = true;
			}

			long nCXi = imageSrc[i].GetWidth();
			long nCYi = imageSrc[i].GetHeight();

			nCX = min(nCXi, nCX);
			nCY = min(nCYi, nCY);
		}
		else 
			break;
	}

	if(nSrcImages < 2)
	{
		delete []imageSrc;
		return false;
	}

	if (bDiffCC)
	{
		AfxMessageBox(IDS_DIFFERENT_MODEL, MB_OK|MB_ICONEXCLAMATION);
		delete []imageSrc;
		return false;
	}

	imageDest.CreateCompatibleImage(imageSrc[0]);


	StartNotification(400 + nCY + 50 + 1);
	int Status = 0;
            
	long activcolors = imageSrc[0].GetColorsCount();

	CString csGrayOrRGB = imageSrc[nSrcImages-1].GetColorConvertor();
	if ("GRAY" != csGrayOrRGB && "RGB" != csGrayOrRGB) return false;

	//--------------------------------------------
	int cnCalcAreaLength = GetValueInt(GetAppUnknown(), "Collect\\CollectByDisp", "CalcMask", 10);
	if (! cnCalcAreaLength) cnCalcAreaLength = 9;
	if (cnCalcAreaLength > min(nCX, nCY)/4) cnCalcAreaLength = min(nCX, nCY)/4;
	int cnAvgAreaLength = GetValueInt(GetAppUnknown(), "Collect\\CollectByDisp", "AvgMask", 9);
	if (! cnAvgAreaLength) cnAvgAreaLength = cnCalcAreaLength;

	int nCalcHalfAreaLength = (int)cnCalcAreaLength/2;
	
	CString csMethod = GetValueString(GetAppUnknown(), "Collect\\CollectByDisp", "Method", "Disp");
	CString csGVOrWeight = GetValueString(GetAppUnknown(), "Collect\\CollectByDisp", "GVOrWeight", "Weight");
	//---------------------------------------------

	unsigned long **DispArray = new unsigned long *[nSrcImages];
	unsigned long **AvgGVArray = new unsigned long *[nSrcImages];
    
	DWORD t1, t2; t1 = GetTickCount();
			// массивы указателей на строки	исходных изображений
	color **SrcImagesRedRowsArray = new color *[nSrcImages*nCY];
	color **SrcImagesGreenRowsArray = new color *[nSrcImages*nCY];
	color **SrcImagesBlueRowsArray = new color *[nSrcImages*nCY];
		   // заполним эти массивы
	for (int k = 0; k < nSrcImages; k++)
	{
		for (long y = 0; y < nCY; y++)
		{
			SrcImagesRedRowsArray[k*nCY + y] = imageSrc[k].GetRow(y, 0);
			SrcImagesGreenRowsArray[k*nCY + y] = imageSrc[k].GetRow(y, 1);
			SrcImagesBlueRowsArray[k*nCY + y] = imageSrc[k].GetRow(y, 2);
		}
	}
		   // массивы указателей на строки результирующего изображени€
	color **DstImageRedRowsArray = new color *[nCY];
	color **DstImageGreenRowsArray = new color *[nCY];
	color **DstImageBlueRowsArray = new color *[nCY];
		   // заполним эти массивы
	for (long y = 0; y < nCY; y++)
	{
		DstImageRedRowsArray[y] = imageDest.GetRow(y, 0);
		DstImageGreenRowsArray[y] = imageDest.GetRow(y, 1);
		DstImageBlueRowsArray[y] = imageDest.GetRow(y, 2);
	}
	t2 = GetTickCount(); TIME("«аполн", t2-t1);
	
	// «аполним массивы и посчитаем дисперсию
	for (int i = 0; i < nSrcImages; i++)
	{
		unsigned long *GVArray = new unsigned long [nCX*nCY]; 
		unsigned long *GVSQRArray = NULL; if (csMethod == "Disp") GVSQRArray = new unsigned long [nCX*nCY];
		unsigned long *Disp = new unsigned long [nCX*nCY];

			t1 = GetTickCount();
		if ("RGB" == csGrayOrRGB)
			for (long y = 0; y < nCY; y++)
			{
				color *psrcRrow = SrcImagesRedRowsArray[i*nCY + y];
				color *psrcGrow = SrcImagesGreenRowsArray[i*nCY + y];
				color *psrcBrow = SrcImagesBlueRowsArray[i*nCY + y];
				for (long x = 0; x < nCX; x++)
				{
					GVArray[nCX*y + x] =  ((long)psrcRrow[x] + psrcGrow[x] + psrcBrow[x])/3;
					if (GVSQRArray) GVSQRArray[nCX*y + x] = GVArray[nCX*y + x] * GVArray[nCX*y + x];
				}
			}
		else //("GRAY" == csGrayOrRGB) 
			for (long y = 0; y < nCY; y++)
			{
				color *psrcRrow = SrcImagesRedRowsArray[i*nCY + y];
				color *psrcGrow = SrcImagesGreenRowsArray[i*nCY + y];
				color *psrcBrow = SrcImagesBlueRowsArray[i*nCY + y];
				for (long x = 0; x < nCX; x++)
				{
					GVArray[nCX*y + x] =  (psrcRrow[x]);
					if (GVSQRArray) GVSQRArray[nCX*y + x] = GVArray[nCX*y + x] * GVArray[nCX*y + x];
				}
			}
			t2 = GetTickCount();	TIME ("яркость ", t2-t1);
			t1 = GetTickCount();
		// результаты усреднени€ будем писать туда же в GVArray и GVSQRArray
		if (!FastAvrg <unsigned long> (GVArray, GVArray, cnCalcAreaLength, nCX, nCY) || 
			(!FastAvrg1 <unsigned long> (GVSQRArray, GVSQRArray, cnCalcAreaLength, nCX, nCY) && GVSQRArray) ) return (false);
			t2 = GetTickCount(); TIME ("—реднееи среднее квадратов ", t2-t1);
			t1 = GetTickCount();
		// вычисл€ем либо дисперсию, либо градиент, либо лапласиан
		if (csMethod == "Disp")
			for (int j = 0; j < nCX*nCY; j++)
			{
				Disp[j] = max(0, GVSQRArray[j] - GVArray[j] * GVArray[j]);
			}
		else if (csMethod == "Grad")
			{
				for (long y = 1; y < nCY-1; y++)
				{
					for (long x = 1; x < nCX-1; x++)
					{
						Disp[nCX*y + x] = _hypot(double(GVArray[nCX*y + x+1])-GVArray[nCX*y + x-1], double(GVArray[nCX*y + x+nCX])-GVArray[nCX*y + x-nCX]);				
					}
				}
			}
		else //лапласиан
			{
				for (long y = 1; y < nCY-1; y++)
				{
					for (long x = 1; x < nCX-1; x++)
					{
						double a = abs(double(GVArray[nCX*y + x+1])-2*GVArray[nCX*y + x]+GVArray[nCX*y + x-1]);
						a += abs(double(GVArray[nCX*y + x+nCX])-2*GVArray[nCX*y + x]+GVArray[nCX*y + x-nCX]);
						Disp[nCX*y + x] = a + 100;				
					}
				}
			}
			 t2 = GetTickCount();  TIME ("¬ычисление дисперсии ", t2-t1);
			 t1 = GetTickCount();
		if (!FastAvrg <unsigned long> (Disp, Disp, cnAvgAreaLength, nCX, nCY)) return (false);
			 t2 = GetTickCount();   TIME ("—реднее дисперсии", t2-t1);

		try
		{
		    Status += 400/nSrcImages;
			Notify(Status);
		}
		catch (...)
			{
				DispArray[i] = Disp;
				AvgGVArray[i] = GVArray;

				if (csGVOrWeight == "Weight") delete []GVArray; 
				delete []GVSQRArray; 
				//delete []Disp;
			}
		
		DispArray[i] = Disp;
		AvgGVArray[i] = GVArray;

		if (csGVOrWeight == "Weight") delete []GVArray; 
		delete []GVSQRArray; 
		//delete []Disp;
	}
	
	t1 = GetTickCount(); 
	// √енерируем результирующее изображение
	byte *BestImageNumberArray = new byte [nCX*nCY];   
	try 
	{
		bool bGVOrWeight;
		if (csGVOrWeight == "GV") bGVOrWeight = true;
		else bGVOrWeight = false;

		bool bGrayOrRGB;
		if ("GRAY" == csGrayOrRGB) bGrayOrRGB = true;
		else bGrayOrRGB = false;

		for (long y = 0; y < nCY; y++)
		{
			for (long x = 0; x < nCX; x++)
			{
				unsigned long MaxDisp = DispArray[0][nCX*y + x]; int MaxDispIndex = 0;  
				unsigned __int64 AvgGVSumm = 0;
				for (int k = 0; k < nSrcImages; k++)
				{
					
					if (DispArray[k][nCX*y + x] > MaxDisp) {MaxDisp = DispArray[k][nCX*y + x]; MaxDispIndex = k;}
					if (bGVOrWeight) AvgGVSumm += AvgGVArray[k][nCX*y + x];
				}
				
				BestImageNumberArray[nCX*y + x] = MaxDispIndex;

				color *psrcRrow = SrcImagesRedRowsArray[MaxDispIndex*nCY + y];
				color *psrcGrow = SrcImagesGreenRowsArray[MaxDispIndex*nCY + y];
				color *psrcBrow = SrcImagesBlueRowsArray[MaxDispIndex*nCY + y];
				
				if (bGVOrWeight)
				{	 
					unsigned __int64 CurrentGVAvg = AvgGVArray[MaxDispIndex][nCX*y + x];
					unsigned long CurrentGVBest;
					int RedPart, GreenPart, BluePart;  // процентные вклады каждой составл€ющей в €ркость

					if (!bGrayOrRGB)
					{	
						CurrentGVBest = (long(psrcRrow[x]) + psrcGrow[x] + psrcBrow[x]) / 3;

						if (CurrentGVBest)
						{
							RedPart = ((psrcRrow[x] / 3) * 100) / CurrentGVBest;
							GreenPart = ((psrcGrow[x] / 3) * 100) / CurrentGVBest;
							BluePart = ((psrcBrow[x] / 3) * 100) / CurrentGVBest;
						}  
						else RedPart = GreenPart = BluePart = 0;
					}
					else										 
						CurrentGVBest = psrcRrow[x];			   
																									
					long CurrentGVNew = long(AvgGVSumm/nSrcImages) + (long)CurrentGVBest - (long)CurrentGVAvg;
					CurrentGVNew = max(min(CurrentGVNew,0xFFFF), 0);
																			//MaxDispIndex*20000;// 
					if (!bGrayOrRGB)
					{
						color *pdstRrow = DstImageRedRowsArray[y]; pdstRrow[x] = min((long)((RedPart * CurrentGVNew * 3) / 100), 0xFFFF);
						color *pdstGrow = DstImageGreenRowsArray[y]; pdstGrow[x] = min((long)((GreenPart * CurrentGVNew * 3) / 100), 0xFFFF);
						color *pdstBrow = DstImageBlueRowsArray[y]; pdstBrow[x] = min((long)((BluePart * CurrentGVNew * 3) / 100), 0xFFFF);
					}
					else
					{
						color *pdstRrow = imageDest.GetRow(y, 0); pdstRrow[x] = min((long)(CurrentGVNew) , 0xFFFF);
					}
				}
				else  // csGVOrWeight == "Weight" или bGVOrWeight = fasle
					{
						__int64 tmpR = 0, tmpG = 0, tmpB = 0;
						__int64 Wtmp = 0;
						for (int k = 0; k < nSrcImages; k++)
						{
							unsigned long W = max(0, (long)DispArray[k][nCX*y + x] - (long)MaxDisp/2);
							if (k == MaxDispIndex) W = max(W,1);
							
							color *ptmpRrow = SrcImagesRedRowsArray[k*nCY + y];
							color *ptmpGrow = SrcImagesGreenRowsArray[k*nCY + y];
							color *ptmpBrow = SrcImagesBlueRowsArray[k*nCY + y];
							
							tmpR += (__int64)ptmpRrow[x] * W;
							tmpG += (__int64)ptmpGrow[x] * W;
							tmpB += (__int64)ptmpBrow[x] * W;
							Wtmp += W;
						}
						
						if (!bGrayOrRGB)
						{
							color *pdstGrow = DstImageGreenRowsArray[y]; pdstGrow[x] = min (tmpG/Wtmp, 0xFFFF);
							color *pdstBrow = DstImageBlueRowsArray[y]; pdstBrow[x] = min (tmpB/Wtmp, 0xFFFF);
						}

						color *pdstRrow = DstImageRedRowsArray[y]; pdstRrow[x] = min (tmpR/Wtmp, 0xFFFF);
					}	
			}
			
			Notify(Status + y);		   
		}
		Status += nCY;
		// -------------------граничные точки     
		// сначала левый и правый  столбик
		for (long y = nCalcHalfAreaLength; y < nCY - nCalcHalfAreaLength; y++)
		{
			byte ImageIndex;
			for (long x = 0; x < nCalcHalfAreaLength; x++)
			{	
				ImageIndex = BestImageNumberArray[nCX*y + nCalcHalfAreaLength];
				color *psrcRrow = SrcImagesRedRowsArray[ImageIndex*nCY + y];
				color *psrcGrow = SrcImagesGreenRowsArray[ImageIndex*nCY + y];
				color *psrcBrow = SrcImagesBlueRowsArray[ImageIndex*nCY + y];
	            
				color *pdstRrow = DstImageRedRowsArray[y];	  pdstRrow[x] =	psrcRrow[x];
				color *pdstGrow = DstImageGreenRowsArray[y]; pdstGrow[x] =	psrcGrow[x];
				color *pdstBrow = DstImageBlueRowsArray[y];  pdstBrow[x] = psrcBrow[x];
			
			}
			for (long x = nCX - nCalcHalfAreaLength; x < nCX; x++)	
			{
				ImageIndex = BestImageNumberArray[nCX*y + nCX - nCalcHalfAreaLength - 1];
				color *psrcRrow = SrcImagesRedRowsArray[ImageIndex*nCY + y];
				color *psrcGrow = SrcImagesGreenRowsArray[ImageIndex*nCY + y];
				color *psrcBrow = SrcImagesBlueRowsArray[ImageIndex*nCY + y];
	            
				color *pdstRrow = DstImageRedRowsArray[y];   pdstRrow[x] =	psrcRrow[x];
				color *pdstGrow = DstImageGreenRowsArray[y]; pdstGrow[x] =	psrcGrow[x];
				color *pdstBrow = DstImageBlueRowsArray[y];  pdstBrow[x] = psrcBrow[x];
			}
		}
		Status += 25;
		Notify(Status);
		// теперь верхн€€ и нижн€€ области
		for (long x = 0; x < nCX; x++)
		{
			byte ImageIndex;
			for (long y = 0; y < nCalcHalfAreaLength; y++)
			{	
				ImageIndex = BestImageNumberArray[nCX*nCalcHalfAreaLength + x];
				color *psrcRrow = SrcImagesRedRowsArray[ImageIndex*nCY + y];
				color *psrcGrow = SrcImagesGreenRowsArray[ImageIndex*nCY + y];
				color *psrcBrow = SrcImagesBlueRowsArray[ImageIndex*nCY + y];
	            	            
				color *pdstRrow = DstImageRedRowsArray[y];	 pdstRrow[x] =	psrcRrow[x];
				color *pdstGrow = DstImageGreenRowsArray[y]; pdstGrow[x] =	psrcGrow[x];
				color *pdstBrow = DstImageBlueRowsArray[y];  pdstBrow[x] = psrcBrow[x];
			}
			for (long y = nCY - nCalcHalfAreaLength; x < nCY; x++)	
			{
				ImageIndex = BestImageNumberArray[nCX*(nCY - nCalcHalfAreaLength - 1) + x];
				color *psrcRrow = SrcImagesRedRowsArray[ImageIndex*nCY + y];
				color *psrcGrow = SrcImagesGreenRowsArray[ImageIndex*nCY + y];
				color *psrcBrow = SrcImagesBlueRowsArray[ImageIndex*nCY + y];
	            	            
				color *pdstRrow = DstImageRedRowsArray[y];   pdstRrow[x] =	psrcRrow[x];
				color *pdstGrow = DstImageGreenRowsArray[y]; pdstGrow[x] =	psrcGrow[x];
				color *pdstBrow = DstImageBlueRowsArray[y];  pdstBrow[x] = psrcBrow[x];
			}
		}
		Status += 25;
		Notify(Status);
		// -------------------

		FinishNotification();
	}
	catch (...)
		{
			delete []imageSrc;
			for (int k = 0; k < nSrcImages; k++)
			{
				delete []DispArray[k];	
				if (csGVOrWeight == "GV") delete []AvgGVArray[k]; 
			}
			delete []AvgGVArray;
			delete []BestImageNumberArray; 
		return false;
		}
	t2 = GetTickCount(); TIME ("генераци€", t2-t1);
	delete []imageSrc;
	for (int k = 0; k < nSrcImages; k++)
	{
		delete []DispArray[k];	
		if (csGVOrWeight == "GV") delete []AvgGVArray[k]; 
	}
	delete []AvgGVArray;
	delete []BestImageNumberArray;

	return true;
}

//////////////////
//CActionAlignImages

CActionAlignImages::CActionAlignImages()
{
}

CActionAlignImages::~CActionAlignImages()
{
}

bool CActionAlignImages::InvokeFilter()
{
	//StartNotification(1);
	int count;
	{
		IActionInfo2Ptr	ai(m_pActionInfo);
		ai->GetArgsCount(&count);
	}

	double sh = GetValueDouble(GetAppUnknown(), "Collect\\Align", "Shift", 0.2);
	BOOL bSimple = (GetValueInt(GetAppUnknown(), "Collect\\Align", "Simple", 0)==1);

	CImageWrp* imgSrc = new CImageWrp[count];
	/*ZeroMemory(imgSrc, sizeof(void*),count);*/
	CString sCC;
	for(long i = 0; i < count; i++)
	{
		CString str = "";
		str.Format("Image%d", i);
		IUnknown* punkImg = GetDataArgument(str);

		if(punkImg)
		{

			imgSrc[i] = CImageWrp(punkImg);

			if (sCC.IsEmpty())
				sCC = imgSrc[i].GetColorConvertor();
			else
			{
				if (sCC != imgSrc[i].GetColorConvertor())
				{
					AfxMessageBox(IDS_DIFFERENT_MODEL, MB_OK|MB_ICONEXCLAMATION);
					delete []imgSrc;
					return false;
				}

			}
		}
		else 
			count =i;
	}
	StartNotification(long(count), 2,1);

	CSignature s1,s2;
	int cx,cy;
	POINT p={0};
	GUID g = GUID_NULL;
	Memorizer memo(p,0,imgSrc[0],g);//base image
	memo.WriteNamedData(); 
	CSignature* sig = new CSignature[count];
	for(int i=0;i<count;i++)
	{
		if(bSimple) sig[i].Init(imgSrc[i]);
		else sig[i].Build(imgSrc[i],sh);
		Notify(i);
	}
	NextNotificationStage();
	for(i=0;i<count-1;i++)
	{
		cx = min(imgSrc[i].GetWidth(), imgSrc[i+1].GetWidth());
		cy = min(imgSrc[i].GetHeight(), imgSrc[i+1].GetHeight());
		int s = min(cx,cy)*sh;

		POINT p;
		if(bSimple) p = sig[i+1].Find(sig[i],s,s);
		else  p = sig[i].FindQuad(sig[i+1],s,s);

		Memorizer memo(p,i+1,imgSrc[i+1], g);
		memo.WriteNamedData();
		Notify(i+1);
	}



	delete []sig;
	delete []imgSrc;
	FinishNotification();
	return true;
}

bool CActionAlignImages::IsAvaible()
{
	return true;
}



CSignature::CSignature()
{
	m_hsums=0;
	m_vsums=0;
	m_Qs = 0;
}
CSignature::~CSignature()
{
	delete[] m_hsums;
	delete[] m_vsums;
	delete[] m_Qs;
}
void CSignature::Init(IUnknown* image)
{
	CImageWrp iw(image);
	m_cx = iw.GetWidth();
	m_cy = iw.GetHeight();
	delete []m_vsums;
	delete []m_hsums;
	m_vsums = new int[m_cy];
	ZeroMemory(m_vsums, sizeof(int)*m_cy);
	m_hsums = new int[m_cx];
	ZeroMemory(m_hsums, sizeof(int)*m_cx);
	int cc = iw.GetColorsCount();
	color** col= new color*[cc];
	for(int y=0;y<m_cy;y++)
	{
		for(int c=0;c<cc;c++)
		{
			col[c] = iw.GetRow(y,c);
		}
		for(int x=0;x<m_cx;x++)
		{
			int n = 0;
			for(int c=0;c<cc;c++)
			{
				n+=ColorAsByte( col[c][x]);
			}
			n/=3;
			m_hsums[x]+= n;
			m_vsums[y]+= n;
		}
	}
	delete[] col;
}
	
void  CSignature::Build(IUnknown* image, double shift)
{
	m_Image = image; 
	m_cx = m_Image.GetWidth();
	m_cy = m_Image.GetHeight();
	int ds = min(m_cx, m_cy)*shift;
	int n=1;
	m_pow = 1;
	int cc = m_Image.GetColorsCount();
	while(1)
	{
		if((n<<m_pow)>=ds) break;
		m_pow++;
	}
	m_pow = m_pow>6 ? 6 : m_pow;
	m_Qs = new Quadrant[m_pow];

	{
		time_test t("CSignature::Build graying:");
		m_Qs[0].set_size( m_cx, m_cy, 1);
		color** cols =new color*[cc];

		for(int y=0; y<m_cy; y++)
		{
			for(int pane=0; pane<cc; pane++)
				cols[pane] = m_Image.GetRow(y,pane);
			float *pDst = &m_Qs[0].get(y,0);

			for(int x=0; x<m_cx; x++)
			{
				double f=0;
				for(int pane=0; pane<cc; pane++)
					f += *(cols[pane]++);
				*(pDst++) = f;
			}
		}
		delete[] cols;
	}

	{
		time_test t("CSignature::Build building");
		for(int i=1;i<m_pow;i++)
		{
			m_Qs[i].create_by_parent( &m_Qs[i-1] );
		}
	}

}


POINT CSignature::Find(CSignature& s, int dx, int dy)
{
	int CX = min(this->m_cx, s.m_cx); 
	int CY = min(this->m_cy, s.m_cy);
	dx =abs(dx);dy =abs(dy);
	int C, X =0, Y=0;
	int nx=0,ny=0;
	double SUM =1.7E+308;
	CSignature *s1,*s2;
	for(int i=-dx;i<=dx;i++)
	{
		if(i<=0)
		{
			s1 = this;
			s2 = &s;
			C=-i;
		}
		else
		{
			s2 = this;
			s1 = &s;
			C=i;
		}
		int CX = min(s1->m_cx-C, s2->m_cx);
		double sum=0;
		for(int j=0;j<CX;j++)
		{
			sum += abs(s1->m_hsums[j+C]- s2->m_hsums[j]);
		}
		sum/=CX;
		if(SUM>sum) 
		{
			SUM=sum;
			X = i;
			nx=1;
		}
		else if(SUM==sum)
		{
			nx++;
		}
	}

	SUM = 0xffffffff;
	for(int i=-dy;i<=dy;i++)
	{
		if(i<=0)
		{
			s1 = this;
			s2 = &s;
			C=-i;
		}
		else
		{
			s2 = this;
			s1 = &s;
			C=i;
		}
		int CY = min(s1->m_cy-C, s2->m_cy);
		double sum=0;
		for(int j=0;j<CY;j++)
		{
			sum += abs(s1->m_vsums[j+C]-s2->m_vsums[j]);
		}
		sum/=CY;
		if(SUM>sum) 
		{
			SUM=sum;
			Y = i;
			ny=1;
		}	
		else if(SUM==sum)
		{
			ny++;
		}

	}

	POINT p ={X,Y};
	return p;
}
POINT CSignature::FindQuad(CSignature& s, int dx, int dy)
{
#ifdef _M_X86
	_mm_empty(); 
#endif
	time_test t("finding quadrants");
	int CX = m_cx; 
	int CY = m_cy;
	dx =abs(dx);dy =abs(dy);
	int  X =0, Y=0;
	POINT P ={0};

	int k=min( m_pow-1,s.m_pow-1);
	int dX = (dx+m_Qs[k].m_step-1)/m_Qs[k].m_step;
	int dY = (dy+m_Qs[k].m_step-1)/m_Qs[k].m_step;


	double Sum = 1.7E+308;

   //seek for best compliant quadrants position 
	{
	time_test t("finding quadrants: 1st");
	for(int j=-dY;j<=dY;j++)
	{
		for(int i=-dX;i<=dX;i++)
		{	
			POINT p ={i,j};
			float sum = m_Qs[k].GetAlignSum(s.m_Qs[k], p);			
			if(sum<Sum)
			{
				Sum = sum;
				P.x = i;
				P.y = j;
			}
		}
	}

	}
	//repeat for twice as smaller quadrants

				
	for(k=k-1;k>=0;k--)
	{
		dX = P.x*2-1;
		dY = P.y*2-1;
		Sum = 1.7E+308;
		Quadrant* qk = &m_Qs[k], *qk1 = &s.m_Qs[k];
		for(int j=dY;j<dY+4;j++)
		{
			for(int i=dX;i<dX+4;i++)
			{	
				POINT p ={i,j};
				float sum = m_Qs[k].GetAlignSum(s.m_Qs[k], p);			
				if(sum<Sum)
				{
					Sum = sum;
					P.x = i;
					P.y = j;
				}
			}
		}
	}	
	return P;
}

Memorizer::Memorizer(IUnknown* pUnkND)
{
	nd = pUnkND;
	guid = GUID_NULL;
	ReadNamedData();
}

Memorizer::Memorizer(POINT offset, int number, IUnknown* pUnkND, GUID& g)
{
	nd = pUnkND;
	if(nd==0)
	{
		INamedDataObject2Ptr ndo(pUnkND);
		if(ndo==0) return;

		ndo->InitAttachedData();
		nd = pUnkND;
		if(nd==0) return;		
	}
	if(g==GUID_NULL)	CoCreateGuid(&g);
	guid = g;

	offsetX = offset.x;
	offsetY = offset.y;
	this->number =number;
}
Memorizer::~Memorizer()
{}
bool Memorizer::ReadNamedData()
{
	if(nd==0) return false;
	BSTR bstrCurrentSection;
	nd->GetCurrentSection( &bstrCurrentSection );
	bstr_t bstrSection(pszSection);
	nd->SetupSection(bstrSection);

	bstr_t bstr_series("Series");
	bstr_t bstr_ox("OffsetX");
	bstr_t bstr_oy("OffsetY");	
	bstr_t bstr_n("NumberInSeries");

	variant_t vdata;
	vdata =0;
	nd->GetValue(bstr_series, &vdata);
	CString s(vdata.bstrVal);
	if(s.IsEmpty()) return false;
	CLSIDFromString(vdata.bstrVal,&guid);
	vdata.Clear();

	nd->GetValue(bstr_ox, &vdata);
	offsetX = vdata.intVal;
	vdata.Clear();

	nd->GetValue(bstr_oy, &vdata);
	offsetY = vdata.intVal;
	vdata.Clear();

	nd->GetValue(bstr_n, &vdata);
	number = vdata.intVal;


	nd->SetupSection(bstrCurrentSection);
	SysFreeString(bstrCurrentSection);

	return true;
}
void Memorizer::WriteNamedData()
{
	BSTR bstrCurrentSection;
	nd->GetCurrentSection( &bstrCurrentSection );

	bstr_t bstrSection(pszSection);
	nd->SetupSection(bstrSection);

	bstr_t bstr_series("Series");
	bstr_t bstr_ox("OffsetX");
	bstr_t bstr_oy("OffsetY");	
	bstr_t bstr_n("NumberInSeries");	


	OLECHAR bstrBuff[40];
	StringFromGUID2(guid,bstrBuff,39);
	nd->SetValue(bstr_series, variant_t(bstrBuff));
	nd->SetValue( bstr_ox, variant_t(offsetX) );
	nd->SetValue( bstr_oy, variant_t(offsetY) );
	nd->SetValue( bstr_n, variant_t(number) );

	nd->SetupSection(bstrCurrentSection);
	SysFreeString(bstrCurrentSection);

}
void	Memorizer::EmptyNamedData()
{
	BSTR bstrCurrentSection;
	nd->GetCurrentSection( &bstrCurrentSection );

	bstr_t bstrSection("");
	nd->SetupSection(bstrSection);
	bstr_t bstrDelete(pszSection);
	nd->DeleteEntry( bstrDelete);

	nd->SetupSection(bstrCurrentSection);
	SysFreeString(bstrCurrentSection);
}

void CopyImage(CImageWrp& dest, CImageWrp& src, POINT p)
{
	int cx =src.GetWidth();
	int cy =src.GetHeight();

	int cc = src.GetColorsCount();
	color** rowSrc = new color*[cc];
	color** rowDst = new color*[cc];

	for(int y=0;y<cy;y++)
	{
		for(int c=0;c<cc;c++)
		{
			rowSrc[c]= src.GetRow(y,c);
			rowDst[c]= dest.GetRow(y+p.y,c);
		}

		for(int x=0;x<cx;x++)
		{
			for(int c=0;c<cc;c++)
			{
				rowDst[c][x+p.x]=rowSrc[c][x];				
			}
		}
	}
	delete[] rowSrc;
	delete[] rowDst;
}

BOOL SubstituteByAlignedImages(CImageWrp* images, int& count, RECT* rects)
{
	Memorizer memo(images[0]);
	if(memo.guid == GUID_NULL) return 0;//
	Memorizer **memos = new Memorizer*[count];
	for(int i=0;i<count;i++)
	{
		memos[i] = new Memorizer(images[i]);
	}

	int mindx =0, mindy=0, maxdx=0, maxdy=0, dx, dy;
	int CX=0,CY=0,cx,cy, currX =0, currY=0;
	RECT commonRect ={0};
	for(int i=0;i<count;i++)
	{
		dx = memos[i]->offsetX;
		dy = memos[i]->offsetY;
		currX+=dx;
		currY+=dy;
		//dx>0 ? maxdx+=dx : mindx+=dx;
		//dy>0 ? maxdy+=dy : mindy+=dy;

		cx =images[i].GetWidth();
		cy =images[i].GetHeight();
		//if(cx>CX) CX=cx;
		//if(cy>CY) CY=cy;

		//memos[i]->offsetX =currX;
		//memos[i]->offsetY =currY ;

		rects[i].left = currX;
		rects[i].top = currY;
		rects[i].right = cx+rects[i].left;
		rects[i].bottom =cy+rects[i].top;

		UnionRect(&commonRect, &commonRect, &rects[i]);

	}
	POINT op = {-commonRect.left, -commonRect.top};
	OffsetRect( &commonRect, op.x, op.y);
	CX = commonRect.right-commonRect.left;
	CY = commonRect.bottom - commonRect.top;

	for(int i=0;i<count;i++)
	{
		CImageWrp iw;
		iw.CreateNew(CX,CY, images[i].GetColorConvertor(), images[i].GetColorsCount());

		OffsetRect(&rects[i], op.x, op.y);
		POINT p = {rects[i].left, rects[i].top};
		CopyImage(iw, images[i], p);
		images[i]=iw;
	}	

	//:clear
	for(int i=0;i<count;i++)
	{
		memos[i]->EmptyNamedData();
		delete memos[i];
	}	
	delete[] memos;
	return 1;
}

void CreateDutyImage(CImageWrp* sources, CImageWrp& duty, RECT* rects, int count)
{
	color** rows = new color*[count];
	color* dutyRow;
	int cc = sources[0].GetColorsCount();
	int cx = sources[0].GetWidth();
	int cy = sources[0].GetHeight();
	duty.CreateNew(cx, cy, sources[0].GetColorConvertor(), cc);

	for(int y=0;y<cy;y++)
	{		
		for(int c=0;c<cc;c++)
		{	
			for(int n =0;n<count;n++)
			{		
				rows[n] = sources[n].GetRow(y,c);
			}
			dutyRow = duty.GetRow(y,c);

			for(int x=0;x<cx;x++)
			{
				int col=0, N=0;
				for(int n =0;n<count;n++)
				{			
					if(PtInRect(&rects[n],CPoint(x,y)))
					{
						col+=rows[n][x];
						N++;
					}
				}
				if(N) dutyRow[x] = col/N;
			}
		}
	}

	delete[] rows;
}

static color get_owner_color(int x, int y, color c, CImageWrp* images , RECT*  rects, int count)
{
	DWORD t=0;
	int matches =0;
	for(int i=0;i<count;i++)
	{
		if(PtInRect(&rects[i], CPoint(x,y)))
		{
			matches++;
			t+=images[i].GetRow(y,c)[x];
		}			
	}
	if(matches)
	{
		return t/matches;
	}
	return 1;
}



template<class T>
void MaskZone<T>::sort()
{
	MZ temp;
	for(int i=0;i<m_size;i++)
	{
		for(int j=m_size-1;j>i;j--)
		{
			if(m_pFnCompare((void*)&m_pmzOwners[j].value, (void*)&m_pmzOwners[j-1].value)>0)
			{
				temp = m_pmzOwners[j];
				m_pmzOwners[j] = m_pmzOwners[j-1];
				m_pmzOwners[j-1] =temp;
			}
		}
	}
}

template<class T>
void MaskZone<T>::InitCell(RECT r, int i, T v)
{
	if(m_pmzOwners==0) m_pmzOwners = new MZ[m_count];
	ASSERT(currInd<m_count);
	m_pmzOwners[currInd].value=v;
	m_pmzOwners[currInd].m_rect =r;
	m_pmzOwners[currInd].m_Index =i;
	m_size++;
	currInd++;
}

template<class T>
void MaskZone<T>::Init(int(c)(void*,void*),int count)
{
	m_count =count;
	m_pFnCompare = c;
}

template<class T>
int MaskZone<T>::GetBest(int x, int y)
{
	for(int i=0;i<m_size && m_pmzOwners[i].m_Index>=0;i++)
	{
		if(PtInRect(&m_pmzOwners[i].m_rect, CPoint(x,y))) break;
	}
	if(i>=m_size) return -1;
	return m_pmzOwners[i].m_Index;
}

Filler::Filler(CImageWrp& img, color mark)
{
	m_pImage = img;
	m_cx = m_pImage.GetWidth();
	m_cy = m_pImage.GetHeight();
	m_mark = mark;
}

void Filler::AddToQueue(int x, int y, int c)
{
	PXL px ={x, y, c};
	pixels.push(px);
}
void Filler::Process()
{
	while(!pixels.empty())
	{
		PXL px = pixels.front();
		get_around(px.x, px.y, px.c);
		pixels.pop();
	}
}
void Filler::get_around(int x, int y, int c)
{
	color* row, *thepoint;
	thepoint = m_pImage.GetRow(y,c);
	if(thepoint[x]!=1) return;
	DWORD col =0;
	int n=0;
	if(y-1>=0)
	{
		row = m_pImage.GetRow(y-1,c);
		if(x-1>=0)
		{	
			if(row[x-1] == 1) AddToQueue(x-1,y-1,c);
			else col+= row[x-1],n++;
		}
		if(x+1<m_cx)
		{
			if(row[x+1] == 1) AddToQueue(x+1,y-1,c);
			else col+= row[x+1],n++;
		}
			if(row[x] == 1) AddToQueue(x,y-1,c);
			else col+= row[x],n++;
	}

		row = m_pImage.GetRow(y,c);
		if(x-1>=0)
		{	
			if(row[x-1] == 1) AddToQueue(x-1,y,c);
			else col+= row[x-1],n++;
		}
		if(x+1<m_cx)
		{
			if(row[x+1] == 1) AddToQueue(x+1,y,c);
			else col+= row[x+1],n++;
		}

	if(y+1<m_cy)
	{
		row = m_pImage.GetRow(y+1,c);
		if(x-1>=0)
		{	
			if(row[x-1] == 1) AddToQueue(x-1,y+1,c);
			else col+= row[x-1],n++;
		}
		if(x+1<m_cx)
		{
			if(row[x+1] == 1) AddToQueue(x+1,y+1,c);
			else col+= row[x+1],n++;
		}
		if(row[x] == 1) AddToQueue(x,y+1,c);
		else col+= row[x],n++;
	}
	if (n>0) thepoint[x] = col/n;
}

inline float& Quadrant::get(int y,int x)
{
	return m_quads[y*m_cx + x];
}
void Quadrant::set_size(int cx, int cy, int  s)
{
	m_cx = cx/s;
	m_cy = cy/s;

	m_size = m_cx*m_cy;
	m_quads = new float[m_size];
	//ZeroMemory(m_quads, sizeof(float)*m_size);
	m_step = s;
}

void Quadrant::create_by_parent(Quadrant* p)
{
	set_size( p->m_cx*p->m_step, p->m_cy*p->m_step, p->m_step*2 );
	for(int y=0; y<m_cy; y++)
	{
		float* pDst = m_quads + y*m_cx;
		float* pSrc0 = p->m_quads + y * 2 * p->m_cx;
		float* pSrc1 = pSrc0 + p->m_cx;
		for(int x=0; x<m_cx; x++)
		{
			*(pDst++) = *(pSrc0++) + *(pSrc0++) + *(pSrc1++) + *(pSrc1++);
		}
	}
}

float Quadrant::GetAlignSum(Quadrant& q, POINT offset)
{
	float S=0;

	int x0 = max(offset.x,0);
	int y0 = max(offset.y,0);

	int cx = min(q.m_cx+offset.x, m_cx);
	int cy = min(q.m_cy+offset.y, m_cy);

	float* line1, *line2;

	__m128 temp, sum128 = _mm_setzero_ps();
	int a =0x7fffffff;
	__m128 mask = _mm_set_ps1(*((float*)&a));

	for(int y=y0;y<cy;y++)
	{
		line1 = (m_quads+ y*m_cx + x0);
		line2 = (q.m_quads+(y - offset.y)*q.m_cx + x0 - offset.x);

		for(int x=x0;x<cx;x+=4)
		{
			temp =_mm_sub_ps(_mm_loadu_ps(line1),_mm_loadu_ps(line2));
			line1+=4, line2+=4;
			temp =_mm_and_ps(temp ,mask);
			sum128 = _mm_add_ps(sum128 ,temp);
		}
		x=x*4;
		line1-=4;
		line2-=4;
		while(x++ < cx)
		{
			S+=fabsf(*line1++ - *line2++);
		}
	}
	__declspec(align(16)) float sum[4];
	_mm_store_ps(sum, sum128);
	S += sum[0]+sum[1]+ sum[2]+sum[3];
	S/=((cx-x0)*(cy-y0));
	
	return S;
}

#pragma warning( default : 4244)
#pragma warning( default: 4018)



