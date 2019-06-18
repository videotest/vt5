// CrossDiv1.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrossDiv1.h"
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

#include "docview5.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//              CrossDiv1
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CCrossDiv1Info::s_pargs[] =
{
	//      {"Img",         szArgumentTypeImage, 0, true, true },
	//      {"Result",      szArgumentTypeImage, 0, false, true },
	{"MeasObjects",    szArgumentTypeObjectList, 0, true, true },
	{"MeasObjectsRes",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

CCrossDiv1Filter::CCrossDiv1Filter()
{
}

CCrossDiv1Filter::~CCrossDiv1Filter()
{
}

IUnknown* CCrossDiv1Filter::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

bool CCrossDiv1Filter::Initialize()
{
	if(!CInteractiveAction::Initialize()) return false;

	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;

	m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_CURSORCROSSDIV1));
	
	m_SqrPointsNum=0;
	
	return true;
}

void CCrossDiv1Filter::DeInit()
{
}

bool CCrossDiv1Filter::Finalize()
{
	::InvalidateRect(m_hwnd, 0, false);
	
	if( !CInteractiveAction::Finalize() )
		return false;
	
	return true;
}

HRESULT CCrossDiv1Filter::GetActionState( DWORD *pdwState )
{
	return CCrossDivFilter::GetActionState(pdwState);
}

bool CCrossDiv1Filter::DoStartTracking( _point point )
{
	return true;
}

bool CCrossDiv1Filter::DoTrack( _point point )
{
	return true;
}


bool CCrossDiv1Filter::DoFinishTracking( _point point )
{
	return true;
}

bool CCrossDiv1Filter::DoLButtonDblClick( _point point )
{
	return true;
}

/*
bool CCrossDiv1Filter::DoRButtonDown( _point point )
{
	
	return false;
}

bool CCrossDiv1Filter::DoRButtonUp( _point point )
{
	//Finalize();
	return false;
}
*/

bool CCrossDiv1Filter::DoLButtonDown( _point point )
{
	if(!IsAvaible()) return(false);
	SeparateTouch(point);
	return true;
}

bool CCrossDiv1Filter::DoMouseMove( _point point )
{
	if(m_SqrPointsNum!=0 && m_SqrPointsNum!=4)
	{
		TrackPoint(point);
		m_SqrPoints[m_SqrPointsNum]=point;
		//::InvalidateRect(m_hwnd, 0, false);
	}
	//return true; //не давать гадам обрабатывать
	return CInteractiveAction::DoMouseMove(point);
}

bool CCrossDiv1Filter::DoChar( int nChar, bool bKeyDown )
{
	if(CInteractiveAction::DoChar(nChar, bKeyDown))
		return true;
	
	return false;
}

void CCrossDiv1Filter::TrackPoint(const _point pt)
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

void CCrossDiv1Filter::SeparateTouch(const _point pt_x)
{
	IDataContextPtr sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	
	//sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkObj);
	//m_Image = punkObj;
	//if(punkObj)     punkObj->Release();
	
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	m_List = punkObj;
	if(punkObj)     punkObj->Release();
	
	smart_alloc(pTrack,_point,10000);
	int TrackSize=1;
	pTrack[0]=pt_x;

	long pos = 0;
	m_List->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		m_List->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr ptrO(sptr);
		m_ObjectUnk=IUnknownPtr(sptr); //!!!вы€снить - сбрасываетс€ ли счетчик у старого значени€ при присваивании
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
				smart_alloc(pimg, byte, (cx1+1)*(cy1+1)); //??? многовато выдел€ем
				ZeroMemory(pimg,(cx1+1)*(cy1+1));
				
				BYTE *pimgy;
				BYTE *pbyte;
				for (int y=0;y<cy;y++)
				{
					pimgy=pimg+(y+1)*cx1+1;
					ptrI->GetRowMask(y,&pbyte);
					CopyMemory(pimgy,pbyte,cx);
				}
				//pimg - копи€ маски объекта, на пиксел шире
				
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
							
							_point offSet(cr.left-1, cr.top-1);
							_point pt_x1(pt_x.x-cr.left,pt_x.y-cr.top);
							SeparateImg(pimg,cx1,cy1,nContour,pContrs,pContrsSize,pContrsFlags,offSet,pt_x1);
							
							//SeparateImg(pimg,cx1,cy1,nContour,pContrs,pContrsSize,
							//offSet,p4Point);
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

void CCrossDiv1Filter::SeparateImg(BYTE *in,int cx,int cy,
								   int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
								   _point OffSet,_point pt_x)
{
	//int N4C[4];
	//ReCalcTrack(nContour,pContrs,pContrsSize,OffSet,p4Point,N4C);
	//CreateTwoObjContours(in,cx,cy,nContour,pContrs,pContrsSize,OffSet,p4Point,N4C);
	
	int N4C[4];
	_point p4Point[4];
	
    int res=Find4Point(in,cx,cy,nContour,pContrs,pContrsSize,OffSet,pt_x,p4Point);
	
	if (res!=-1)
	{
		res=ReCalcTrack(nContour,pContrs,pContrsSize,pContrsFlags,OffSet,p4Point,N4C);
		if (res!=-1)
		{
			if(1)
			{ //отсортировать точки, чтобы исключить самопересечени€ и т.п.
				Sort4Points(p4Point);
			}

			CreateTwoObjContours(in,cx,cy,nContour,pContrs,pContrsSize,pContrsFlags,OffSet,p4Point,N4C);
		}
	}
}

static double dist(_point pt1,_point pt2)
{
	return _hypot(pt1.x-pt2.x, pt1.y-pt2.y);
}

static int FindD(_point pt1,_point pt2,_point pt3,double *maxd,BYTE *in,int cx)
{
    double A=sqrt((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y));
	double B=sqrt((pt1.x-pt3.x)*(pt1.x-pt3.x)+(pt1.y-pt3.y)*(pt1.y-pt3.y));
	double C=sqrt((pt3.x-pt2.x)*(pt3.x-pt2.x)+(pt3.y-pt2.y)*(pt3.y-pt2.y));
	double cos;
	if (A>=1 && B>=1)
	{
		cos=(A*A+B*B-C*C)/(2*A*B);
		if (cos>0.9999)
			cos=0.9999;
	} //косинус угла при pt1
	else
		cos=0.9999;
	double h=B*sqrt(1-cos*cos); //высота треугольника (из pt2 на pt1-pt3)
	
	_point ptm = pt1+pt2;
	ptm.x=ptm.x/2;
	ptm.y=ptm.y/2;
	
	if (h>*maxd && in[ptm.y*cx+ptm.x]<100)
	{
		*maxd=h;
		return 1;
	}
	else
		return 0;
}

static int ScanLine(BYTE *in,int cx,int cy,_point pt1,_point pt2)
{
	int x,y,x1,x2,y1,y2;
	x1=pt1.x;y1=pt1.y;
	x2=pt2.x;y2=pt2.y;
	double i;
	int a=0;
	double rad = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	if (rad<1)
		return 0;
	
	double kx = (double)(x2-x1)/rad;
    double ky = (double)(y2-y1)/rad;
    for (i=1;i<rad-1;i+=1)
	{
		y=int(y1+ky*i);
		x=int(x1+kx*i);
		if (x<cx && x>=0 && y<cy && y>=0)
			if (*(in+y*cx+x)<100)
			{
				a++;
				if (a>1)
					return 1;
			}
	}
    return 0;
}

int CCrossDiv1Filter::Find4Point(BYTE *in,int cx,int cy,
								 int nContour,_point **pContrs,int *pContrsSize,
								 _point OffSet,_point pt_x,_point *p4Point)
{
	int i,j,k,N,flag,n,tc,tp;
	int dc=20;
	double rad;
	_point pt,pt1,pt2;
	N=0;
	
	double p4DPoint[4];
	
	smart_alloc(pMark, byte*, nContour);
	for (i=0;i<nContour;i++)
	{
		pMark[i]=new byte[pContrsSize[i]];
		for (j=0;j<pContrsSize[i];j++)
			pMark[i][j]=0;
	}
	
	for (k=0;k<nContour;k++)
        if (pContrsSize[k]>25)
        {
			dc=20; if (pContrsSize[k]<=dc*2) dc=pContrsSize[k]/2-1;
			for (i=0;i<pContrsSize[k];i++)
			{
				pt=pContrs[k][i];
				n=i-dc;if (n<0) n=pContrsSize[k]+n;
				pt1=pContrs[k][n];
				n=i+dc;if (n>=pContrsSize[k]) n=n-pContrsSize[k];
				pt2=pContrs[k][n];
				if(min(dist(pt1,pt_x),dist(pt2,pt_x))<dist(pt,pt_x)-3)
					pMark[k][i]=1; //если точка не "в глубине" - пометим
			}
        }
        else
        {
			for (i=0;i<pContrsSize[k];i++)
                pMark[k][i]=1;
        }
		
        do
        {
			flag=0;rad=1000000;
			for (k=0;k<nContour;k++)
			{
				for (i=0;i<pContrsSize[k];i++)
					if (pMark[k][i]==0)
					{
						if (dist(pt_x,pContrs[k][i])<rad)
						{
							rad=dist(pt_x,pContrs[k][i]);
							pt=pContrs[k][i];
							tc=k;
							tp=i;
							flag=1;
						}
					}
			} //нашли ближайшую неотмеченную точку контура
			if (flag==1)
			{
				p4Point[N++]=pt; //в список
				if (N>=4) flag=0;
				dc=20; if (pContrsSize[tc]<=dc*2) dc=pContrsSize[tc]/2-1;
				for(i=0;i<dc;i++)
				{
					n=tp-i;if (n<0) n=pContrsSize[tc]-1;
					pMark[tc][n]=1;
					n=tp+i;if (n>=pContrsSize[tc]) n=0;
					pMark[tc][n]=1;
				}//пометили на 20 точек влево/вправо от нее
				
                int dc1=7;if (pContrsSize[tc]<=dc1*2) dc1=pContrsSize[tc]/2-1;
				double maxD=0;
				n=tp-dc1;if (n<0) n=pContrsSize[tc]+n;
				pt1=pContrs[tc][n];
				n=tp+dc1;if (n>=pContrsSize[tc]) n=n-pContrsSize[tc];
				pt2=pContrs[tc][n];
				pt=pContrs[tc][tp];
				FindD(pt1,pt2,pt,&maxD,in,cx);
				p4DPoint[N-1]=maxD;
			}
        }while (flag==1);
        // возможное отсечение ненужных точек
        ////////////////////////////////////////////////////////
        rad=100000;
        for (i=0;i<N-1;i++)
			for (j=i+1;j<N;j++)
				if (dist(p4Point[i],p4Point[j])<rad)
					rad=dist(p4Point[i],p4Point[j]);
				
				for (i=0;i<N;i++)
					if(dist(p4Point[i],pt_x)>rad*2)
					{
                        for (j=i+1;j<N;j++)
							p4Point[j-1]=p4Point[j];
                        i--;N--;
					}
					
					for (i=0;i<N;i++)
						if(ScanLine(in,cx,cy,p4Point[i],pt_x)==1)
						{
							for (j=i+1;j<N;j++)
                                p4Point[j-1]=p4Point[j];
							i--;N--;
						}
						
						if (N==4)
						{
							for (i=0;i<N;i++)
								if (p4DPoint[i]<2)
								{
									ReSetPoint1(in,cx,cy,nContour,pContrs,pContrsSize,pt_x,
										&(p4Point[i]),(int)(rad),p4Point);
								}
						}
						else
							if (N==3)
							{
								if (p4DPoint[0]<p4DPoint[1] && p4DPoint[0]<p4DPoint[2])
									k=0;
								else
									if (p4DPoint[1]<p4DPoint[2])
										k=1;
									else k=2;
									ReSetPoint(in,cx,cy,nContour,pContrs,pContrsSize,pt_x,
										&(p4Point[k]),&(p4Point[3]),(int)(rad));
									N=4;
							}
							else
								if (N==2)
								{
									ReSetPoint(in,cx,cy,nContour,pContrs,pContrsSize,pt_x,
										&(p4Point[0]),&(p4Point[2]),(int)(rad));
									ReSetPoint(in,cx,cy,nContour,pContrs,pContrsSize,pt_x,
										&(p4Point[1]),&(p4Point[3]),(int)(rad));
									N=4;
								}
								////////////////////////////////////////////////////////////
								
								PointCh(p4Point,N);
								
								for (i=0;i<nContour;i++)
									delete pMark[i];
								
								if (N!=4) return -1;
								
								for (i=0;i<N-1;i++)
									for (j=i+1;j<N;j++)
										if (dist(p4Point[i],p4Point[j])<10)
											return -1;
										return 0;
}

void CCrossDiv1Filter::PointCh(_point* p_pt,int N)
{
	_point pt;
	if (CrossLine(p_pt[0],p_pt[1],p_pt[2],p_pt[3]) == 1 )
	{
		pt=p_pt[1];p_pt[1]=p_pt[2];p_pt[2]=pt;
	}
	else
        if (CrossLine(p_pt[1],p_pt[2],p_pt[3],p_pt[0]) == 1 )
        {
			pt=p_pt[2];p_pt[2]=p_pt[3];p_pt[3]=pt;
        }
}

int CCrossDiv1Filter::CrossLine(_point pt1,_point pt2,_point pt3,_point pt4)
{
	double a1,a2,b1,b2,x,y;
	if (pt1.x!=pt2.x)
		a1=(double)(pt1.y-pt2.y)/(double)(pt1.x-pt2.x);
	else
		a1=1000;
	b1=(double)pt1.y-a1*(double)pt1.x;
	
	if (pt3.x!=pt4.x)
		a2=(double)(pt3.y-pt4.y)/(double)(pt3.x-pt4.x);
	else
		a2=1000;
	b2=(double)pt3.y-a2*(double)pt3.x;
	
	if(a1-a2<0.1 && a1-a2>-0.1)
		return 0;
	x=(int)((b1-b2)/(a2-a1));
	y=a1*x+b1;
	_point pt((int)x,(int)y);
	if (dist(pt,pt1)<dist(pt1,pt2) && dist(pt,pt2)<dist(pt1,pt2) &&
		dist(pt,pt3)<dist(pt3,pt4) && dist(pt,pt4)<dist(pt3,pt4) )
		return 1;
	else
		return 0;
	
}

void CCrossDiv1Filter::ReSetPoint(BYTE *in,int cx,int cy,
								  int nContour,_point **pContrs,int *pContrsSize,
								  _point pt_x,_point* p_pc1,_point* p_pc2,int D)
{
	int i,tp,tc,v,k,n;
	_point pc1=*p_pc1;
	_point pc2=*p_pc2;
	_point pt,pt1,pt2;
	
	double rad=100000;
	for (k=nContour-1;k>=0;k--)
		for (i=0;i<pContrsSize[k];i++)
			if(dist(pContrs[k][i],pc1)<rad)
			{
				rad=dist(pContrs[k][i],pc1);
				tp=i;tc=k;
			}
			
			int dc=7; if (pContrsSize[tc]<=dc*2) dc=pContrsSize[tc]/2-1;
			pt=pContrs[tc][tp];
			n=tp-dc;if (n<0) n=pContrsSize[tc]+n;
			pt1=pContrs[tc][n];
			n=tp+dc;if (n>=pContrsSize[tc]) n=n-pContrsSize[tc];
			pt2=pContrs[tc][n];
			pt=pContrs[tc][tp];
			double maxd=0;
			FindD(pt1,pt2,pt,&maxd,in,cx);
			if (maxd>2)
			{
                // определение направлени€
                dc=15;if (pContrsSize[tc]<=dc*2) dc=pContrsSize[tc]/2-1;
				n=tp-dc;if (n<0) n=pContrsSize[tc]+n;
                pt1=pContrs[tc][n];
                n=tp+dc;if (n>=pContrsSize[tc]) n=n-pContrsSize[tc];
                pt2=pContrs[tc][n];
                if (dist(pt_x,pt1)<dist(pt_x,pt2))
					v=-1;
                else
					v=1;
                dc=D*2;
                pc2=FindMaxP(in,cx,cy,pContrs[tc],pContrsSize[tc],tp,v,dc,int(D*0.8),1);
			}
			else
			{
                dc=int(D*1.5);
                pc2=FindMaxP(in,cx,cy,pContrs[tc],pContrsSize[tc],tp,1,dc,int(D*0.4+3),0);
                pc1=FindMaxP(in,cx,cy,pContrs[tc],pContrsSize[tc],tp,-1,dc,int(D*0.4+3),1);
			}
			*p_pc1=pc1;
			*p_pc2=pc2;
			return;
			
}

void CCrossDiv1Filter::ReSetPoint1(BYTE *in,int cx,int cy,
								   int nContour,_point **pContrs,int *pContrsSize,
								   _point pt_x,_point* p_pc1,int D,_point *p4Point)
{
	int i,i1,i2,tp,tc,v,k,n;
	_point pc1=*p_pc1;
	_point pt,pt1,pt2;
	
	double rad=100000;
	for (k=nContour-1;k>=0;k--)
		for (i=0;i<pContrsSize[k];i++)
			if(dist(pContrs[k][i],pc1)<rad)
			{
				rad=dist(pContrs[k][i],pc1);
				tp=i;tc=k;
			}
			
			int dc;
			// определение направлени€
			n=tp+1;if (n>=pContrsSize[tc]) n=0;
			for (i1=0;i1<pContrsSize[tc]/3;i1++)
			{
                for (i=0;i<4;i++)
					if (p4Point[i]==pContrs[tc][n])
						i=100;
					if (i>99) break;
					n++;if (n>=pContrsSize[tc]) n=0;
			}
			n=tp-1;if (n<0) n=pContrsSize[tc]-1;
			for (i2=0;i2<pContrsSize[tc]/3;i2++)
			{
                for (i=0;i<4;i++)
					if (p4Point[i]==pContrs[tc][n])
						i=100;
					if (i>99) break;
					n--;if (n<0) n=pContrsSize[tc]-1;
			}
			if (i1<i2) v=-1;
			else if (i2<i1) v=1;
			else return;
			
			/*      dc=10;if (pContrsSize[tc]<=dc*2) dc=pContrsSize[tc]/2-1;
			n=tp-dc;if (n<0) n=pContrsSize[tc]+n;
			pt1=pContrs[tc][n];
			n=tp+dc;if (n>=pContrsSize[tc]) n=n-pContrsSize[tc];
			pt2=pContrs[tc][n];
			if (dist(pt_x,pt1)>dist(pt_x,pt2))
			v=-1;
			else
			v=1;*/
			dc=D;
			pc1=FindMaxP(in,cx,cy,pContrs[tc],pContrsSize[tc],tp,v,dc,0,0);
			
			*p_pc1=pc1;
			return;
			
}

_point CCrossDiv1Filter::FindMaxP(BYTE *in,int cx,int cy,
								  _point *pCPd,int Size,int tp,int v,int dc,int D,int flag)
{
	int j,n,k;
	_point pt,pt1,pt2,pto;
	int dc1=5;
	double maxd=0;
	
	n=tp+D*v;
	if (n<0) n=Size-1;
	if (n>=Size) n=0;
	pto=pCPd[n];
	
	if (flag==0)
		k=0;
	else
	{
		k=D/2;
		tp=tp+k*v;
		if (tp<0) tp=Size-1;
		if (tp>=Size) tp=0;
	}
	for (j=k;j<dc;j++)
	{
		if (Size<=dc1*2) dc1=Size/2-1;
		n=tp-dc1;if (n<0) n=Size+n;
		pt1=pCPd[n];
		n=tp+dc1;if (n>=Size) n=n-Size;
		pt2=pCPd[n];
		pt=pCPd[tp];
		if( FindD(pt1,pt2,pt,&maxd,in,cx) == 1 )
			pto=pt;
		tp+=v;
		if (tp<0) tp=Size-1;
		if (tp>=Size) tp=0;
	}
	return pto;
}

int CCrossDiv1Filter::ReCalcTrack( int nContour,_point **pContrs,int *pContrsSize,int *pContrsFlags,
								  _point OffSet,_point *p4Point,int *N4C)
								  //заменить 4 точки на ближайшие к ним точки контуров (исключа€ коротенькие контура <25).
{
	for (int k=0;k<4;k++)
	{
		_point pt(p4Point[k]);
		double r=100000;
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
