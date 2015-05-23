// CrossDiv.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrossDiv.h"
#include "misc_utils.h"
#include <math.h>
#include <limits.h>
#include "measure5.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "alloc.h"
#include "aam_utils.h"
#include "ContourApi.h"

#include "docview5.h"
#include "chrom_aam_utils.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//              CrossDiv
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CCrossDivInfo::s_pargs[] =
{
	{"Img",         szArgumentTypeImage, 0, true, true },
	//      {"Result",      szArgumentTypeImage, 0, false, true },
	{"MeasObjects",    szArgumentTypeObjectList, 0, true, true },
	{"MeasObjectsRes",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

CCrossDivFilter::CCrossDivFilter()
{
}

CCrossDivFilter::~CCrossDivFilter()
{
}

IUnknown* CCrossDivFilter::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

bool CCrossDivFilter::Initialize()
{
	if(!CInteractiveAction::Initialize()) return false;

	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;

	m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSORCROSSDIV));
	
	m_SqrPointsNum=0;
	
	return true;
}

void CCrossDivFilter::DeInit()
{
}

bool CCrossDivFilter::Finalize()
{
	::InvalidateRect(m_hwnd, 0, false);
	
	if( !CInteractiveAction::Finalize() )
		return false;
	
	return true;
}

HRESULT CCrossDivFilter::GetActionState( DWORD *pdwState )
{
	HRESULT r=CInteractiveAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

bool CCrossDivFilter::IsAvaible()
{
	if(!CheckInterface(m_ptrTarget,IID_IImageView)) return false;

	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	
	//sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	//if(!punkObj) return false;
	//if(punkObj) punkObj->Release();
	
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	if(!punkObj) return false;

	// [max] bt: 3561
	if( punkObj )
	{
		INamedDataObject2Ptr sptrL = punkObj;
		punkObj->Release();

		long lCount = 0;
        sptrL->GetChildsCount( &lCount );

		if( !lCount )
			return false;
	}

	
	return true;
}

bool CCrossDivFilter::DoLButtonDblClick( _point point )
{
	return false;
}

/*
bool CCrossDivFilter::DoRButtonDown( _point point )
{
	return false;
}

bool CCrossDivFilter::DoRButtonUp( _point point )
{
	return false;
}
*/

bool CCrossDivFilter::DoLButtonDown( _point point )
{
	if(!IsAvaible()) return(false);

	m_SqrPoints[m_SqrPointsNum]=point;
	m_SqrPointsNum++;
	m_SqrPoints[m_SqrPointsNum]=point;
	
	//TrackPoint(point);
	::InvalidateRect(m_hwnd, 0, false);
	
	if(m_SqrPointsNum==4)
	{
		int flag=0;
		for (int i=0;i<4;i++)
		{
			for (int j=i+1;j<4;j++)
			{
				double r=_hypot(
					m_SqrPoints[i].x-m_SqrPoints[j].x,
					m_SqrPoints[i].y-m_SqrPoints[j].y);
				if(r<4) flag=1;
				if(r>70) flag=1;
			}
		}
		if (flag==0) SeparateTouch();
		m_SqrPointsNum=0;
	}

	m_bTestTracking=false;
	return true;
}

LRESULT	CCrossDivFilter::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	dbg_assert( m_ptrSite != 0 );
	int bResult = FALSE;

	if( (nMsg == WM_SETCURSOR) )
	{
		::SetCursor(m_hCurActive);
		bResult=true;
	}
	else bResult=CInteractiveAction::DoMessage(nMsg, wParam, lParam);

	return bResult;
}

int SignedContourArea(_point *pCont, int size)
{ //возвращает площадь контура, знак - направление обхода
	int s=0;
	int x0=pCont[size-1].x, y0=pCont[size-1].y;
	for(int i=0; i<size; i++)
	{
		int x1=pCont[i].x, y1=pCont[i].y;
		s+=x0*y1-x1*y0;
		x0=x1; y0=y1;
	}
	return s/2;
}

void Sort4Points(_point *pt4)
{ //сортирует 4 точки, чтобы контур из них обходился по часовой стрелке
	int s_max=0;
	int i_max=0;
	for(int i=0; i<6; i++)
	{
		int i0=i%3;
		int i1=i/3; i1=(i0+i1+1)%3;
		int i2=3-i0-i1;
		_point pt4dst[4]={pt4[i0],pt4[i1],pt4[i2],pt4[3]};
		int s=SignedContourArea(pt4dst,4);
		if(s>s_max)
		{
			s_max=s;
			i_max=i;
		}
	}
	int i0=i_max%3;
	int i1=i_max/3; i1=(i0+i1+1)%3;
	int i2=3-i0-i1;
	_point pt4dst[4]={pt4[i0],pt4[i1],pt4[i2],pt4[3]};
	for(int j=0; j<4; j++) pt4[j]=pt4dst[j];
}


bool CCrossDivFilter::DoMouseMove( _point point )
{
	if(m_SqrPointsNum!=0 && m_SqrPointsNum!=4)
	{
		TrackPoint(point);
		m_SqrPoints[m_SqrPointsNum]=point;
		if(0)
		{ //отсортировать точки, чтобы исключить самопересечения и т.п.
			if(m_SqrPointsNum==3) Sort4Points(m_SqrPoints);
		}
		//::InvalidateRect(m_hwnd, 0, false);
	}
	return CInteractiveAction::DoMouseMove(point);
}

bool CCrossDivFilter::DoChar( int nChar, bool bKeyDown )
{
	if(CInteractiveAction::DoChar(nChar, bKeyDown))
		return true;
	
	return false;
}

void CCrossDivFilter::TrackPoint(const _point pt)
{
	int minx=pt.x, miny=pt.y;
	int maxx=pt.x, maxy=pt.y;
	for (int i=0;i<=m_SqrPointsNum;i++)
	{
		if (minx>m_SqrPoints[i].x) minx=m_SqrPoints[i].x;
		if (miny>m_SqrPoints[i].y) miny=m_SqrPoints[i].y;
		if (maxx<m_SqrPoints[i].x) maxx=m_SqrPoints[i].x;
		if (maxy<m_SqrPoints[i].y) maxy=m_SqrPoints[i].y;
	}
	
	_rect rect(minx,miny,maxx,maxy);
	rect.left--;rect.top--;rect.right++;rect.bottom++;
	IScrollZoomSitePtr      ptrScrollZoom(m_ptrTarget);
	rect = _window(rect, ptrScrollZoom);
	
	::InvalidateRect(m_hwnd, &rect, false);
}

void CCrossDivFilter::SeparateTouch()
{
	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;

	if(1)
	{ //отсортировать точки, чтобы исключить самопересечения и т.п.
		Sort4Points(m_SqrPoints);
	}

	//sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	//m_Image = punkObj;
	//if(punkObj)     punkObj->Release();
	
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	m_List = punkObj;
	if(punkObj)     punkObj->Release();
	
	//if(m_image != 0) ...
	//return m_image != 0;
	
	smart_alloc(pTrack,_point,10000);
	int TrackSize=0;
	AddLineToContour(pTrack,&TrackSize,m_SqrPoints[0],m_SqrPoints[1]);
	AddLineToContour(pTrack,&TrackSize,m_SqrPoints[1],m_SqrPoints[2]);
	AddLineToContour(pTrack,&TrackSize,m_SqrPoints[2],m_SqrPoints[3]);
	AddLineToContour(pTrack,&TrackSize,m_SqrPoints[3],m_SqrPoints[0]);
	
	long cnt = 0; POSITION pos = 0;
	m_List->GetChildsCount(&cnt);
	m_List->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		m_List->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr ptrO(sptr);
		m_ObjectUnk=IUnknownPtr(sptr); //!!!выяснить - сбрасывается ли счетчик у старого значения при присваивании
		IUnknownPtr sptr2;
		ptrO->GetImage(&sptr2);
		IImage3Ptr ptrI(sptr2);
		
		_rect cr;
		{
			_size size;
			ptrI->GetSize((int*)&size.cx,(int*)&size.cy);
			_point ptOffset;
			ptrI->GetOffset(&ptOffset);
			cr=_rect(ptOffset,size);
		}
		
		for (int i=0;i<TrackSize;i++)  //по всем точкам четырехугольника
		{
			if (pTrack[i].x>cr.left && pTrack[i].x<cr.right &&
				pTrack[i].y>cr.top && pTrack[i].y<cr.bottom)
			{
				int cx = cr.right-cr.left;int cx1=cx+2;
				int cy = cr.bottom-cr.top;int cy1=cy+2;
				smart_alloc(pimg, byte, (cx1+1)*(cy1+1)); //??? многовато выделяем
				ZeroMemory(pimg,(cx1+1)*(cy1+1));
				
				BYTE *pimgy;
				BYTE *pbyte;
				for (int y=0;y<cy;y++)
				{
					pimgy=pimg+(y+1)*cx1+1;
					ptrI->GetRowMask(y,&pbyte);
					CopyMemory(pimgy,pbyte,cx);
				}
				//pimg - копия маски объекта, на пиксел шире
				
				for (int j=0;j<TrackSize;j++) //по всем точкам четырехугольника
				{
					int x1,y1;
					x1=pTrack[j].x-cr.left+1;
					y1=pTrack[j].y-cr.top+1;
					if(x1>=0 && x1<cx1 && y1>=0 && y1<cy1)
					{
						if (pimg[y1*cx1+x1]==255) //если четырехугольник пересек объект
						{
							for (int y=0;y<cy1;y++) //приведем pimg к 0/255
							{
								pimgy=pimg+y*cx1;
								for (int x=0;x<cx1;x++)
									if (*pimgy==255)
										*pimgy++=255;
									else
										*pimgy++=0;
							}
							int nContour;
							ptrI->GetContoursCount(&nContour);
							smart_alloc(pContrs, _point*, nContour);
							smart_alloc(pContrsSize, int, nContour);
							smart_alloc(pContrsFlags, int, nContour);
							for (int i1=0;i1<nContour;i1++)
							{
								Contour* pContour;
								ptrI->GetContour(i1, &pContour);
								int size = pContour->nContourSize;
								pContrsSize[i1]=size;
								pContrsFlags[i1]=pContour->lFlags;
								pContrs[i1]=new _point[size];
								for (int j1=0;j1<size;j1++)
								{
									pContrs[i1][j1].x=pContour->ppoints[j1].x-cr.left+1;
									pContrs[i1][j1].y=pContour->ppoints[j1].y-cr.top+1;
								}
							}       //теперь в pContrs[][] - все точки всех контуров объекта
							{ //тупо отсортируем, чтобы внешние циклы были снаружи
								for(int i=0; i<nContour; i++)
								{
									for(int j=i+1; j<nContour; j++)
									{
										if( (!(pContrsFlags[i]&2)) && (pContrsFlags[j]&2))
										{
											_point *ptmp=pContrs[i]; pContrs[i]=pContrs[j]; pContrs[j]=ptmp;
											int tmp1=pContrsSize[i]; pContrsSize[i]=pContrsSize[j]; pContrsSize[j]=tmp1;
											int tmp2=pContrsFlags[i]; pContrsFlags[i]=pContrsFlags[j]; pContrsFlags[j]=tmp2;
										}
									}
								}
							}

							_point p4Point[4];
							for(i1=0;i1<4;i1++)
							{
								p4Point[i1].x=m_SqrPoints[i1].x-cr.left+1;
								p4Point[i1].y=m_SqrPoints[i1].y-cr.top+1;
							}       //p4Point - координаты 4 точек, приведенные к img
							_point offSet(cr.left-1, cr.top-1);
							SeparateImg(pimg,cx1,cy1,nContour,pContrs,pContrsSize,pContrsFlags,
								offSet,p4Point);
							///delete
							for (i1=0;i1<nContour;i1++)
								if (pContrsSize[i1]!=0) delete pContrs[i1];
								i=100000;j=100000;//break;
						}
					}
				}
				
				if(i>=100000)
				{
					Finalize();
					return;
				}
			}
		}
	}
	Finalize();
}

void CCrossDivFilter::SeparateImg(BYTE *in,int cx,int cy,
								  int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
								  _point OffSet,_point *p4Point)
{
	int N4C[4]={-1,-1,-1,-1};
	ReCalcTrack(nContour,pContrs,pContrsSize,pContrsFlags,OffSet,p4Point,N4C);
	if(N4C[0]>=0 && N4C[1]>=0 && N4C[2]>=0 && N4C[3]>=0)
	{
		CreateTwoObjContours(in,cx,cy,nContour,pContrs,pContrsSize,pContrsFlags,OffSet,p4Point,N4C);
	}
}

int CCrossDivFilter::ReCalcTrack( int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
								 _point OffSet,_point *p4Point,int *N4C)
								 //заменить 4 точки на ближайшие к ним точки контуров (исключая коротенькие контура <25).
{
	for (int k=0;k<4;k++)
	{
		_point pt(p4Point[k]);
		double r=1e100;
		int n=0;
		for(int i=0;i<nContour;i++)
		{
			int Size=pContrsSize[i];
			if (Size<25) Size=0;
			_point *pCPd=pContrs[i];
			for (int j=0;j<Size;j++)
				if((p4Point[k].x-pCPd[j].x)*(p4Point[k].x-pCPd[j].x)+
					(p4Point[k].y-pCPd[j].y)*(p4Point[k].y-pCPd[j].y) < r)
				{
					r=(p4Point[k].x-pCPd[j].x)*(p4Point[k].x-pCPd[j].x)+
						(p4Point[k].y-pCPd[j].y)*(p4Point[k].y-pCPd[j].y);
					pt=pCPd[j];
					n=i;
				}
		}
		p4Point[k]=pt;
		N4C[k]=n;
	}
	return 0;
}

void CCrossDivFilter::CreateTwoObjContours(BYTE *in,int cx,int cy,
										   int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
										   _point OffSet,_point *p4Point,int *N4C)
{
	int i,j,k,NI;
	smart_alloc(out1, byte, cx*cy);
	ZeroMemory(out1,cx*cy);
	smart_alloc(out2, byte, cx*cy);
	ZeroMemory(out2,cx*cy);
	
	int TotalContrsSize=0;
	for (i=0;i<nContour;i++)
		TotalContrsSize+=pContrsSize[i];

	//если все точки на внешнем контуре - сделаем его первым
	if(N4C[1]==N4C[0] && N4C[2]==N4C[0] && N4C[3]==N4C[0]) //все точки на один контур
	{
		if(pContrsFlags[N4C[0]]&2) //он внешний
		{
			_point *ptmp=pContrs[N4C[0]]; pContrs[N4C[0]]=pContrs[0]; pContrs[0]=ptmp;
			int tmp1=pContrsSize[N4C[0]]; pContrsSize[N4C[0]]=pContrsSize[0]; pContrsSize[0]=tmp1;
			int tmp2=pContrsFlags[N4C[0]]; pContrsFlags[N4C[0]]=pContrsFlags[0]; pContrsFlags[0]=tmp2;
			N4C[0]=0; N4C[1]=0; N4C[2]=0; N4C[3]=0;
		}
	}
	
	int nContour1=1;
	smart_alloc(pContrs1, _point*, nContour+3);
	smart_alloc(pContrsSize1, int, nContour+3);
	smart_alloc(pContrsFlags1, int, nContour+3);
	for (i=0;i<nContour+3;i++)
	{
		pContrsSize1[i]=0;
		pContrs1[i]=new _point[50000];
	}
	
	int nContour2=1;
	smart_alloc(pContrs2, _point*, nContour+3);
	smart_alloc(pContrsSize2, int, nContour+3);
	smart_alloc(pContrsFlags2, int, nContour+3);
	
	smart_alloc(pMark, byte*, nContour);
	for (i=0;i<nContour;i++)
	{
		pMark[i]=new byte[pContrsSize[i]];
		for (j=0;j<pContrsSize[i];j++) pMark[i][j]=0;
	}
	
	_point pt;
	int NC1=0;
	int NSP,NSC;
	int tp,tc,v;
	int flag=0;
	int n;
	int nOut=0;
	_point *pCPd;
	int Size;
	int TwoContour=0;

	// debug - output contours to file
	/*
	{
		FILE *f=fopen("e:\\mylog.log","a");
		fputs("\n\r",f);
		for (int k=0;k<nContour;k++)
		{
			fprintf(f, "Contour %i (%i points)\n", k, pContrsSize[k]);
			for (int i=0;i<pContrsSize[k];i++)
			{
				fprintf(f, "%4i%4i\n",pContrs[k][i].x, pContrs[k][i].y);
			}
		}
		fclose(f);
	}
	/**/
	
	//  Заполнение контуров
	do
	{
		//поиск первой точки контура
        pContrsSize1[NC1]=0;
		flag=0;
		
		for (k=0;k<nContour;k++)
		{
			for (i=0;i<pContrsSize[k];i++)
			{
				if (pMark[k][i]==0)
				{
					NSC=k;
					NSP=i;
					tc=k;
					tp=i+1;if (tp>=pContrsSize[k]) tp=0;
					v=1;
					flag=1;
                    goto out;
					//нашли первую неотмеченную точку в контурах,
					//NSC=tc=контур, NSP - точка, tp - следующая за ней
				}
			}
		}
				
out:    if (flag==0) break;
		
		pContrsFlags1[NC1]=pContrsFlags[tc];
		NI=0;
		do
		{
			NI++;
			if (NI>TotalContrsSize) goto outfunk;
			pContrs1[NC1][pContrsSize1[NC1]++]=pContrs[tc][tp]; //включаем точку из исходного контура в результат
			/*{
				FILE *f=fopen("e:\\mylog.log","a");
				fprintf(f, ">> %4i%4i\n", pContrs[tc][tp].x, pContrs[tc][tp].y); // debug - output to file
				fclose(f);
			}*/
			pMark[tc][tp]=1; //и помечаем ее
			if (tp==NSP && tc==NSC)
			{
				NC1++;
				break;
			}
			if (pContrsSize[NSC]>25)
				for (i=0;i<4;i++)
					if(pContrs[tc][tp]==p4Point[i])
					{
						n=i;
						int v1=v;
						if(SignedContourArea(pContrs[tc],pContrsSize[tc])<0) v1=-v1;
						pt=GetNext4Point(n,p4Point,tc,v1);
						GetNextPosition(pt,nContour,pContrs,pContrsSize,&tc,&tp,&v,N4C);
						AddLineToContour(pContrs1[NC1],&(pContrsSize1[NC1]),
							p4Point[i],pt);
						/*{
							FILE *f=fopen("e:\\mylog.log","a");
							fprintf(f, ">> %4i%4i - %4i%4i\n", p4Point[i].x, p4Point[i].y, pt.x, pt.y); // debug - output to file
							fclose(f);
						}*/
						pContrs1[NC1][pContrsSize1[NC1]++]=pContrs[tc][tp];
						/*{
							FILE *f=fopen("e:\\mylog.log","a");
							fprintf(f, ">> %4i%4i\n", pContrs[tc][tp].x, pContrs[tc][tp].y); // debug - output to file
							fclose(f);
						}*/
						pMark[tc][tp]=1;
						nOut++;
						break;
					}
					tp+=v;
					if (tp<0) tp=pContrsSize[tc]-1;
					if (tp>pContrsSize[tc]-1) tp=0;
					if (nOut>4) goto outfunk;;
		}while(1==1);
	}while(1==1);
	nContour1=NC1;

	if (N4C[0]==0 && N4C[1]==0 && N4C[2]==0 && N4C[3]==0)// внешний контур
	{
		int nContourOut=0;
		for (j=0;j<nContour;j++)
		{
			if(pContrsFlags[j]&2) nContourOut++;
		}

		TwoContour=1;
		nContour2=1;
		for (i=0;i<nContour+3;i++)
		{
			pContrsSize2[i]=0;
			pContrs2[i]=new _point[50000];
		}
		pContrsSize2[0]=pContrsSize1[1];
		pContrsFlags2[0]=pContrsFlags1[1];
		CopyMemory(pContrs2[0],pContrs1[1],pContrsSize1[1]*sizeof(_point));

		for (i=2;i<nContour1;i++)
		{
			CopyMemory(pContrs1[i-1],pContrs1[i],pContrsSize1[i]*sizeof(_point));
			pContrsSize1[i-1]=pContrsSize1[i];
		}
		nContour1--;

		FillContour(out1,cx,cy,pContrs2[0],pContrsSize2[0],0,0,0);
		//for (j=1;j<nContourOut;j++)
		//{
			//FillContour(out1,cx,cy,pContrs[j],pContrsSize[j],0,0,0);
		//}
		
		for(i=1;i<nContour;i++)
		{
			Size=pContrsSize[i];
			pCPd=pContrs[i];
			int flag=0;
			for (j=0;j<Size;j++)
				if(out1[pCPd[j].y*cx+pCPd[j].x]==0)
					flag=1;
				if (flag==0)
				{
					for (j=0;j<pContrsSize[i];j++)
						pContrs2[nContour2][j]=pContrs[i][j];
                    pContrsSize2[nContour2]=pContrsSize[i];
                    pContrsFlags2[nContour2]=pContrsFlags[i];
					nContour2++;
				}
		}
		
		FillContour(out1,cx,cy,pContrs1[0],pContrsSize1[0],0,0,0);
		//for (j=1;j<nContourOut;j++)
		//{
			//FillContour(out1,cx,cy,pContrs[j],pContrsSize[j],0,0,0);
		//}
		for(i=1;i<nContour1;i++)
		{
			Size=pContrsSize1[i];
			pCPd=pContrs1[i];
			int flag=0;
			for (j=0;j<Size;j++)
				if(out1[pCPd[j].y*cx+pCPd[j].x]==0)
					flag=1;
				if (flag==1)
				{
					for (j=i+1;j<nContour1;j++)
					{
						CopyMemory(pContrs1[j-1],pContrs1[j],pContrsSize1[j]*sizeof(_point));
						pContrsSize1[j-1]=pContrsSize1[j];
					}
					i--;
					nContour1--;
				}
		}
	}
	
	//08.07.2002 - bug fixed на chrom2 - если тыкать в правый отросток креста,
	//одно из nContour == 0 - ужасно падаем
	if(nContour1<=0 || nContour2<=0) goto outfunk;;

	{
		IUnknownPtr ptrDoc = get_document();
		//sptrIView     sptrV( m_punkTarget );
		//sptrV->GetDocument( &m_punkDocument );
		if (TwoContour==1)
		{
			IMeasureObjectPtr object(m_ObjectUnk);
			IUnknown* punkObj = 0;
			object->GetImage(&punkObj);
			IImage3Ptr image_obj(punkObj);
			if(punkObj)     punkObj->Release();

			DWORD dwFlags;
			image_obj->GetImageFlags(&dwFlags);

			bool bIsVirtual=(dwFlags&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными

			bool bCrossNonVirtual = GetValueInt(GetAppUnknown(), "\\Chromos\\CrossDiv", "CrossNonVirtual", 0) != 0;
			//надо ли переводить виртуальные кресты в невиртуальные, затирая под ними

			// разделили объект на несколько - затрем под ним изображение
			// и будем делать его не виртуальным
			if(bIsVirtual && bCrossNonVirtual)
			{
				INamedDataObject2Ptr imageNDO(image_obj);
				imageNDO->StoreData(sdfCopyParentData | sdfClean);
				//image_obj->AddRef(); m_undo_images.insert(image);
				bIsVirtual=false;
			}

			AddObjects(nContour1,pContrs1,pContrsSize1,pContrsFlags1,OffSet);
			AddObjects(nContour2,pContrs2,pContrsSize2,pContrsFlags2,OffSet);
			RemoveFromDocData( ptrDoc, m_ObjectUnk);
		}
		else
		{
			AddObjects1(nContour1,pContrs1,pContrsSize1,pContrsFlags1,OffSet);
			RemoveFromDocData( ptrDoc, m_ObjectUnk);
		}
	}
	///////////////////////////////////////////////////
outfunk:
	for (i=0;i<nContour+3;i++)
		delete pContrs1[i];
	
	for (i=0;i<nContour;i++)
		delete pMark[i];
	
	if (TwoContour==1)
	{
		for (i=0;i<nContour+3;i++)
			delete pContrs2[i];
	}
}

void CCrossDivFilter::AddObjects (int nContour,_point **pContrs,int *pContrsSize, int *pContrsFlags, _point OffSet)
{
	int cx,cy,xmin,xmax,ymin,ymax,i,j,x,y,k;
	xmin=10000;ymin=10000;
	xmax=0;ymax=0;
	for (i=0;i<pContrsSize[0];i++)
	{
		if(xmin>pContrs[0][i].x) xmin=pContrs[0][i].x;
		if(ymin>pContrs[0][i].y) ymin=pContrs[0][i].y;
		if(xmax<pContrs[0][i].x) xmax=pContrs[0][i].x;
		if(ymax<pContrs[0][i].y) ymax=pContrs[0][i].y;
	}
	xmax++;ymax++;
	cx=xmax-xmin;
	cy=ymax-ymin;
	
	_bstr_t s2(szTypeImage);
	IUnknown *punk2 = ::CreateTypedObject(s2);
	IImage3Ptr imgOut(punk2);
	punk2->Release();

	IMeasureObjectPtr object(m_ObjectUnk);
	IUnknown* punkObj = 0;
	object->GetImage(&punkObj);
	IImage3Ptr image_obj(punkObj);
	if(punkObj)     punkObj->Release();

	DWORD dwFlags;
	image_obj->GetImageFlags(&dwFlags);
	bool bIsVirtual=(dwFlags&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными

	bool bCrossNonVirtual = GetValueInt(GetAppUnknown(), "\\Chromos\\CrossDiv", "CrossNonVirtual", 0) != 0;
	//надо ли переводить виртуальные кресты в невиртуальные, затирая под ними

	// разделили объект на несколько - затрем под ним изображение
	// и будем делать его не виртуальным
	if(bIsVirtual && bCrossNonVirtual)
	{
		INamedDataObject2Ptr imageNDO(image_obj);
		imageNDO->StoreData(sdfCopyParentData | sdfClean);
		//image_obj->AddRef(); m_undo_images.insert(image);
		bIsVirtual=false;
	}

	_bstr_t	bstrCC;
	int nPanes;
	{
		IUnknown *punkCC = 0;
		image_obj->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		nPanes = ::GetImagePaneCount( image_obj );

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC = bstr;
		::SysFreeString( bstr ); 
	}

	_point pt_offset;
	pt_offset.x=xmin+OffSet.x;
	pt_offset.y=ymin+OffSet.y;

	if(bIsVirtual)
	{
		_rect rect=_rect(pt_offset,_size(cx,cy));

		imgOut->CreateVirtual( rect );
		imgOut->InitRowMasks();

		INamedDataObject2Ptr imgOutNDO(imgOut);
		INamedDataObject2Ptr imageNDO(image_obj);

		IUnknown *punkParent=0;
		imageNDO->GetParent(&punkParent);
		IImage3Ptr imageParent(punkParent);
		if(punkParent!=0) punkParent->Release();
		punkParent=0;

		DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
		//imgOutNDO->GetObjectFlags( &dwFlags );
		imgOutNDO->SetParent( imageParent,  dwFlags);
	}
	else
	{
		imgOut->CreateImage(cx,cy,bstrCC, ::GetImagePaneCount( image_obj ) );
		imgOut->SetOffset(pt_offset,false);
	}
	
	imgOut->InitRowMasks();
	imgOut->InitContours();

	for (y=0;y<cy;y++)
	{
		byte *p;
		imgOut->GetRowMask(y,&p);
		ZeroMemory(p,cx);
	}
	x=0;y=0;k=0;
	for (i=0;i<nContour;i++)
	{
		byte *p;
		imgOut->GetRowMask(y,&p);
		p[x]=255;
		p[x+1]=255;
		k++;
		x+=3;
		if (x>cx-3) {x=0;y+=2;}
		if (y>=cy) break;
	}
	if (k<nContour) nContour=k;
	if (k==0) return;
	imgOut->InitContours();
	
	//установка контуров
	for (j=0;j<nContour;j++)
	{
		Contour *pCont;
		imgOut->GetContour(j,&pCont);
		for (i=0;i<pContrsSize[j];i++)
		{
			if (i<pCont->nContourSize)
				::ContourSetPoint(pCont,i, pContrs[j][i].x+OffSet.x, pContrs[j][i].y+OffSet.y);
			else
				::ContourAddPoint(pCont, pContrs[j][i].x+OffSet.x, pContrs[j][i].y+OffSet.y);
		}
	}
	//заполнение маски
	smart_alloc(img, byte, cx*cy);
	smart_alloc(img1, byte, cx*cy);
	byte *imgy;
	byte *img1y;
	FillContour(img,cx,cy,pContrs[0],pContrsSize[0],xmin,ymin,0);
	for (j=1;j<nContour;j++)
	{
		if(pContrsFlags[j]&2)
		{
			FillContour(img1,cx,cy,pContrs[j],pContrsSize[j],xmin,ymin,0);
			for(y=0;y<cy;y++)
			{
				imgy=img+y*cx;
				img1y=img1+y*cx;
				for(x=0;x<cx;x++)
				{
					if(*img1y==255)
						*imgy=255;
					imgy++;img1y++;
				}
			}
		}
	}
	for (j=1;j<nContour;j++)
	{
		if((pContrsFlags[j]&2) == 0)
		{
			FillContour(img1,cx,cy,pContrs[j],pContrsSize[j],xmin,ymin,1);
			for(y=0;y<cy;y++)
			{
				imgy=img+y*cx;
				img1y=img1+y*cx;
				for(x=0;x<cx;x++)
				{
					if(*img1y==255)
						*imgy=0;
					imgy++;img1y++;
				}
			}
		}
	}
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		byte *p; imgOut->GetRowMask(y,&p);
		CopyMemory(p,imgy,cx);
	}
	
	// заполнение изображения image_obj
	if(!bIsVirtual)
	{
		int xos=xmin-1;
		int yos=ymin-1;
		_rect rect;
		{
			_size size;
			image_obj->GetSize((int*)&size.cx,(int*)&size.cy);
			_point ptOffset;
			image_obj->GetOffset(&ptOffset);
			rect=_rect(ptOffset,size);
		}
		
		int cx1=rect.right-rect.left;
		int cy1=rect.bottom-rect.top;
		int C;
		int def_color=128*256;
		for(int pane=0; pane<nPanes; pane++)
		{
			for (y=max(0,yos);y<min(cy1,yos+cy);y++)
			{
				color *pcol_out0;
				imgOut->GetRow(y-yos,pane,&pcol_out0);
				pcol_out0-=min(0,xos);
				
				color *pcol_in;
				image_obj->GetRow(y,pane,&pcol_in);
				pcol_in+=max(0,xos);

				byte *pmask_in;
				image_obj->GetRowMask(y,&pmask_in);
				pmask_in+=max(0,xos);

				imgy=img+(y-yos)*cx-min(0,xos);
				C=min(cx1,xos+cx)-max(0,xos);
				for (x=0;x<C;x++)
				{
					imgy[x]=113;
					pcol_out0[x]=pcol_in[x];
					/*
					if(pmask_in[x]<128)
					{
						pcol_out0[x]=def_color;
						pcol_out1[x]=def_color;
						pcol_out2[x]=def_color;
					}
					else def_color=pcol_in[x];
					*/
				}
				//CopyMemory(pcol_out0,pcol_in,C*sizeof(color));
				//CopyMemory(pcol_out1,pcol_in,C*sizeof(color));
				//CopyMemory(pcol_out2,pcol_in,C*sizeof(color));
			}
		}
	}
	
	//-------------------- object
	_bstr_t s(szTypeObject);
	IUnknown *punk = ::CreateTypedObject(s);
	IMeasureObjectPtr objectOut(punk );
	punk->Release();
	
	objectOut->SetImage( imgOut );

	IUnknownPtr ptrDoc = get_document();
	SetToDocData(ptrDoc, objectOut);
	::ConditionalCalcObject(objectOut, 0, true, 0 );

}

void CCrossDivFilter::AddObjects1(int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,_point OffSet)
{
	int cx,cy,i,j,x,y,k;
	
	IMeasureObjectPtr object(m_ObjectUnk);
	IUnknown* punkObj = 0;
	object->GetImage(&punkObj);
	IImage3Ptr image_obj(punkObj);
	if(punkObj)     punkObj->Release();

	DWORD dwFlags;
	image_obj->GetImageFlags(&dwFlags);
	bool bIsVirtual=(dwFlags&ifVirtual)!=0; //исходное изображение было виртуальным - надо и новые делать виртуальными
	
	_rect rect;
	{
		_size size;
		image_obj->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		image_obj->GetOffset(&ptOffset);
		rect=_rect(ptOffset,size);
	}
	
	cx=rect.right-rect.left;
	cy=rect.bottom-rect.top;

	_bstr_t	bstrCC;
	int nPanes;
	{
		IUnknown *punkCC = 0;
		image_obj->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		nPanes = ::GetImagePaneCount( image_obj );

		BSTR	bstr;
		ptrCC->GetCnvName( &bstr );
		bstrCC = bstr;
		::SysFreeString( bstr ); 
	}
	
	_bstr_t s2(szTypeImage);
	IUnknown *punk2 = ::CreateTypedObject(s2);
	IImage3Ptr imgOut(punk2);
	punk2->Release();
	
	//imgOut->CreateNew(cx,cy,"");
	_point pt_offset;
	pt_offset.x=OffSet.x+1;
	pt_offset.y=OffSet.y+1;

	if(bIsVirtual)
	{
		_rect rect=_rect(pt_offset,_size(cx+2,cy+2));

		imgOut->CreateVirtual( rect );
		imgOut->InitRowMasks();

		INamedDataObject2Ptr imgOutNDO(imgOut);
		INamedDataObject2Ptr imageNDO(image_obj);

		IUnknown *punkParent=0;
		imageNDO->GetParent(&punkParent);
		IImage3Ptr imageParent(punkParent);
		if(punkParent!=0) punkParent->Release();
		punkParent=0;

		DWORD dwFlags = sdfAttachParentData | apfNotifyNamedData;
		//imgOutNDO->GetObjectFlags( &dwFlags );
		imgOutNDO->SetParent( imageParent,  dwFlags);
	}
	else
	{
		imgOut->CreateImage(cx+2,cy+2,bstrCC, ::GetImagePaneCount( image_obj ) );
		imgOut->SetOffset(pt_offset,false);
	}

	
	imgOut->InitRowMasks();
	imgOut->InitContours();
	
	for (y=0;y<cy+2;y++)
	{
		byte *p;
		imgOut->GetRowMask(y,&p);
		ZeroMemory(p,cx+2);
	}
	
	x=0;y=0;k=0;
	for (i=0;i<nContour;i++)
	{
		byte *p; imgOut->GetRowMask(y,&p);
		p[x]=255;
		p[x+1]=255;
		k++;
		x+=3;
		if (x>cx+2-3) {x=0;y+=2;}
		if (y>=cy+2) break;
	}
	imgOut->InitContours();
	
	//установка контуров
	for (j=0;j<nContour;j++)
	{
		for (i=0;i<pContrsSize[j];i++)
		{
			if (pContrs[j][i].x<1) pContrs[j][i].x=1;
			if (pContrs[j][i].y<1) pContrs[j][i].y=1;
			if (pContrs[j][i].x>cx) pContrs[j][i].x=cx;
			if (pContrs[j][i].y>cy) pContrs[j][i].y=cy;
		}
	}
	for (j=0;j<nContour;j++)
	{
		Contour *pCont;
		imgOut->GetContour(j,&pCont);
		for (i=0;i<pContrsSize[j];i++)
		{
			if (i<pCont->nContourSize)
				::ContourSetPoint(pCont,i, pContrs[j][i].x+OffSet.x, pContrs[j][i].y+OffSet.y);
			else
				::ContourAddPoint(pCont, pContrs[j][i].x+OffSet.x, pContrs[j][i].y+OffSet.y);
		}
	}
	//заполнение маски
	for (y=0;y<cy+2;y++)
	{
		byte *p;
		imgOut->GetRowMask(y,&p);
		ZeroMemory(p,cx+2);
	}
	
	for (y=0;y<cy;y++)
	{
		byte *p1,*p2;
		imgOut->GetRowMask(y,&p1);
		image_obj->GetRowMask(y,&p2);
		CopyMemory(p1,p2,cx);
	}
	
	if(!bIsVirtual)
	{
		// заполнение изображения image_obj
		for(int pane=0; pane<nPanes; pane++)
		{
			for (y=0;y<cy;y++)
			{
				color *pcol_out0;
				imgOut->GetRow(y,pane,&pcol_out0);
				
				color *pcol_in;
				image_obj->GetRow(y,pane,&pcol_in);
				
				CopyMemory(pcol_out0,pcol_in,cx*sizeof(color));
			}
		}
	}
	
	
	_bstr_t s(szTypeObject);
	IUnknown *punk = ::CreateTypedObject(s);
	IMeasureObjectPtr objectOut(punk );
	punk->Release();
	
	objectOut->SetImage( imgOut );
	
	IUnknownPtr ptrDoc = get_document();
	SetToDocData( ptrDoc, objectOut );
	::ConditionalCalcObject(objectOut, 0, true, 0 );
}

void CCrossDivFilter::GetNextPosition(_point pt,int nContour,_point **pContrs,
									  int *pContrsSize,int *ptc,int *ptp,int *pv,int *N4C)
{
	int tc,tp,v,i,j;
	int NC=0;
	//      if (N4C[0]==N4C[1] && N4C[1]==N4C[2])
	//              NC=N4C[0];
	//      if (N4C[0]==N4C[1] && N4C[1]==N4C[3])
	//              NC=N4C[0];
	//      if (N4C[0]==N4C[2] && N4C[2]==N4C[3])
	//              NC=N4C[0];
	//      if (N4C[1]==N4C[2] && N4C[2]==N4C[3])
	//              NC=N4C[1];
	double r=1e100;
	for(i=0;i<nContour;i++)
	{
		int Size=pContrsSize[i];
		_point *pCPd=pContrs[i];  //текущий контур
		for (j=0;j<Size;j++)
			if((pt.x-pCPd[j].x)*(pt.x-pCPd[j].x)+
				(pt.y-pCPd[j].y)*(pt.y-pCPd[j].y) < r)
			{
				r=(pt.x-pCPd[j].x)*(pt.x-pCPd[j].x)+
					(pt.y-pCPd[j].y)*(pt.y-pCPd[j].y);
				tc=i;
				tp=j;
				v=*pv;
				//                              if (tc==NC || *ptc==NC)
				//                                      if (tc != *ptc)
				//                                              v=-(*pv);
				//                              if (tc!=NC && *ptc!=NC)
				//                                      if (tc != *ptc)
				//                                              v=-(*pv);
			} //ищем ближайшую к pt точку контура
	}
	*ptc=tc; //запоминаем ее индексы
	*ptp=tp;
	*pv=v;
	return;
}

_point CCrossDivFilter::GetNext4Point(int n,_point *p4Point,int tc,int v)
{
	// если tc==0 поворот не совпадает с v
	_point pt,pt1,pt2,p1,p2;
	_point pt_ch,pt_unch;
	int i;
	pt=p4Point[n];
	i=n-1;if(i<0) i=3;
	pt1=p4Point[i]; //предыдущая
	i=n+1;if(i>3) i=0;
	pt2=p4Point[i]; //следующая
	double r=_hypot(pt.x-pt1.x, pt.y-pt1.y);
	if (r==0)
		return pt;
	double kx=(pt1.x-pt.x)/r;
	double ky=(pt1.y-pt.y)/r;
	p1.x=int(pt.x+r*ky); //по
	p1.y=int(pt.y-r*kx); //предыдущая, повернутая на -90г отн-но текущей
	p2.x=int(pt.x-r*ky); // против
	p2.y=int(pt.y+r*kx); //предыдущая, повернутая на +90г отн-но текущей
	
	double r1=_hypot(pt2.x-p1.x, pt2.y-p1.y);
	double r2=_hypot(pt2.x-p2.x, pt2.x-p2.x);
	
	if (r1>r2) //если следующая ближе к повернутой на +90 предыдущей
	{
		pt_ch=pt2;
		pt_unch=pt1;
	}
	else
	{
		pt_ch=pt1;
		pt_unch=pt2;
	}

	//14.08.2002
	int s=SignedContourArea(p4Point,4);
	if (s<0) //если следующая ближе к повернутой на +90 предыдущей
	{
		pt_ch=pt2;
		pt_unch=pt1;
	}
	else
	{
		pt_ch=pt1;
		pt_unch=pt2;
	}
	
	if (tc==0)
	{
		if (v>0)
			return pt_unch;
		else
			return pt_ch;
	}
	else
	{
		if (v>0)  //(v<0)
			return pt_unch;
		else
			return pt_ch;
	}
}

void CCrossDivFilter::FillContour(byte *img,int cx,int cy,_point *pCont,int Size,
								  int xos,int yos,int inside)
{
	BYTE *imgy;
	int ItNum=0;
	int pd[10];
    pd[0]=-cx; pd[1]=1; pd[2]=cx; pd[3]=-1; pd[4]=-cx+1; pd[5]=cx+1; pd[6]=cx-1; pd[7]=-cx-1;
	int pd1[10];
    pd1[0]=-cx; pd1[1]=-cx+1; pd1[2]=1; pd1[3]=cx+1; pd1[4]=cx; pd1[5]=cx-1; pd1[6]=-1; pd1[7]=-cx-1; pd1[8]=-cx;
	
	int x,y,i,j,flag;
	
	smart_alloc(pC1, DWORD, max(cx*cy/4,100));
	smart_alloc(pC2, DWORD, max(cx*cy/4,100));
	int Count1=0;
	int Count2=0;
	DWORD *pC1d;
	DWORD *pC2d;
	DWORD *d_q;
	/////////////////////////////////////////////////////
	ZeroMemory(img,cx*cy);
	{ //изобразим контур
		int x=pCont[Size-1].x-xos; x=min(max(x,0),cx-1);
		int y=pCont[Size-1].y-yos; y=min(max(y,0),cy-1);
		for (i=0;i<Size;i++)
		{
			int x1=pCont[i].x-xos; x1=min(max(x1,0),cx-1);
			int y1=pCont[i].y-yos; y1=min(max(y1,0),cy-1);
			while(x!=x1 || y!=y1)
			{
				if(x<x1) x++; else if (x>x1) x--;
				if(y<y1) y++; else if (y>y1) y--;
				img[y*cx+x]=100;
			}
		}
	}

	for (y=0;y<cy;y++)
	{
		if (img[y*cx]!=100) img[y*cx]=50;
		if (img[y*cx+cx-1]!=100) img[y*cx+cx-1]=50;
	}
	for (x=0;x<cx;x++)
	{
		if (img[x]!=100) img[x]=50;
		if (img[(cy-1)*cx+x]!=100) img[(cy-1)*cx+x]=50;
	}
	//////////////////////////////////////////////////

	// debug - output mask to file
	/*
	FILE *f=fopen("e:\\mylog.log","a");
	fputs("\n",f);
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			char c='.';
			if(*imgy==50) c='*';
			if(*imgy==100) c='#';
			fputc(c,f);
			imgy++;
		}
		fputs("\n\r",f);
	}
	fputs("\n",f);
	fclose(f);
	/**/

	pC1d=pC1;pC2d=pC2;
	Count1=0;Count2=0;
	for (y=0;y<cy;y++)
	{
		imgy=img+y*cx;
		for (x=0;x<cx;x++)
		{
			if(*imgy==0)
				for(i=0;i<4;i++)
					if(*(imgy+pd[i])==50)
					{
						pC1d[Count1++]=imgy-img;
						break;
					}
					imgy++;
		}
	}
	for (i=0;i<Count1;i++)
		*(img+pC1d[i])=50;
	do
	{
		flag=0;
		for(i=0;i<Count1;i++)
			for(j=0;j<4;j++)
			{
				imgy=img+pC1d[i]+pd[j];
				if(*imgy==0)
				{    // можно ли удалять эту точку
					
					pC2d[Count2++]=imgy-img;
					*imgy=50;
					flag=1;
				}
			}
			
			d_q=pC1d;pC1d=pC2d;pC2d=d_q;
			Count1=Count2;Count2=0;
	}while(flag==1);
	
	if (inside==0)
	{
		for (y=0;y<cy;y++)
		{
			imgy=img+y*cx;
			for (x=0;x<cx;x++)
				if (*imgy==50)
					*imgy++=0;
				else
					*imgy++=255;
		}
	}
	else
	{
		for (y=0;y<cy;y++)
		{
			imgy=img+y*cx;
			for (x=0;x<cx;x++)
				if (*imgy==50 || *imgy==100)
					*imgy++=0;
				else
					*imgy++=255;
		}
	}
	return;
}

void CCrossDivFilter::AddLineToContour(_point *pCont,int *Size,_point pt1,_point pt2)
{
	int x,x1,x2,y,y1,y2;
	y1=pt1.y;
	x1=pt1.x;
	y2=pt2.y;
	x2=pt2.x;
	double rad = _hypot(x2-x1, y2-y1);
    if (rad>0)
	{
        double kx = (double)(x2-x1)/rad;
        double ky = (double)(y2-y1)/rad;
        int i;
        for (i=0;i<=(int)rad;i++)
		{
			y=y1+(int)(ky*i);
			x=x1+(int)(kx*i);
			pCont[*Size].x=x;
			pCont[(*Size)++].y=y;
		}
	}
}


IUnknownPtr     CCrossDivFilter::get_document()
{
	IViewPtr ptrView( m_ptrTarget );
	if( ptrView == 0 )
		return 0;
	
	IUnknownPtr ptrDoc;
	{
		IUnknown* punk = 0;
		ptrView->GetDocument( &punk );
		if( !punk       )
			return 0;
		
		ptrDoc = punk;
		punk->Release();        punk = 0;
	}
	
	return ptrDoc;
}


HRESULT CCrossDivFilter::DoInvoke()
{
	IUnknownPtr ptrDoc = get_document();
	
	OnAttachTarget( ptrDoc );
	
	//IImage3Ptr    ptrSrcImage = m_image; //( GetDataArgument() );
	
	//if( ptrSrcImage == NULL) return false;
	
	return S_OK;
}

HRESULT CCrossDivFilter::Paint( HDC hDC, RECT rectDraw,
							   IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache,
							   BYTE *pdibCache )
{
	IScrollZoomSitePtr      ptrScrollZoom(punkTarget);
	
	COLORREF c_axis = GetValueColor(GetAppUnknown(), "\\Chromos\\CrossDiv", "LineColor", RGB( 255, 128, 128 ));
	HPEN hPen = ::CreatePen( PS_SOLID, 0, c_axis );
	//HPEN hPen = ::CreatePen( PS_SOLID, 0, RGB( 255, 128, 128 ) );
	//::GetStockObject(WHITE_PEN)
	
	HGDIOBJ old_brush=::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	HGDIOBJ old_pen=::SelectObject(hDC, hPen);
	int old_rop2=::GetROP2(hDC);

	if(m_SqrPointsNum)
	{
		::SetROP2(hDC,R2_COPYPEN);
		_point pt=_window(m_SqrPoints[0], ptrScrollZoom);
		::MoveToEx(hDC, pt.x,pt.y, 0);
		for(int i=1; i<m_SqrPointsNum; i++)
		{
			_point pt=_window(m_SqrPoints[i], ptrScrollZoom);
			::LineTo(hDC, pt.x,pt.y);
		}
		if(m_SqrPointsNum==4)
		{
			_point pt=_window(m_SqrPoints[0], ptrScrollZoom);
			::LineTo(hDC, pt.x,pt.y);
		}
		else
		{
			::SetROP2(hDC,R2_NOT);
			pt=_window(m_SqrPoints[m_SqrPointsNum], ptrScrollZoom);
			::LineTo(hDC, pt.x,pt.y);
			if(m_SqrPointsNum==3)
			{
				_point pt=_window(m_SqrPoints[0], ptrScrollZoom);
				::LineTo(hDC, pt.x,pt.y);
			}
		}
	}

	::SetROP2(hDC,old_rop2);
	::SelectObject(hDC, old_pen);
	::SelectObject(hDC, old_brush);
	::DeleteObject(hPen);
	
	return S_OK;
}

HRESULT CCrossDivFilter::GetRect( RECT *prect )
{
	if(prect)
	{
		int minx=INT_MAX, miny=INT_MAX;
		int maxx=INT_MIN, maxy=INT_MIN;
		for (int i=0;i<=max(m_SqrPointsNum,1);i++)
		{
			if (minx>m_SqrPoints[i].x) minx=m_SqrPoints[i].x;
			if (miny>m_SqrPoints[i].y) miny=m_SqrPoints[i].y;
			if (maxx<m_SqrPoints[i].x) maxx=m_SqrPoints[i].x;
			if (maxy<m_SqrPoints[i].y) maxy=m_SqrPoints[i].y;
		}

		_rect rect(minx,miny,maxx,maxy);
		rect.left--;rect.top--;rect.right++;rect.bottom++;
		IScrollZoomSitePtr      ptrScrollZoom(m_ptrTarget);
		rect = _window(rect, ptrScrollZoom);
		rect.right++;rect.bottom++;

		//if(m_SqrPointsNum==0) rect=NORECT;

		*prect = rect;
		return S_OK;
	}
	
	return E_INVALIDARG;
}