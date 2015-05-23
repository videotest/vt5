#include "stdafx.h"
#include "AvgChrom.h"
#include "misc_utils.h"
#include <math.h>
#include "measure5.h"
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "Chromosome.h"
#include "dpoint.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//              AvgChrom
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CAvgChromInfo::s_pargs[] =
{
        {"Source",    szArgumentTypeObjectList, 0, true, true },
        {"Averaged",    szArgumentTypeObjectList, 0, false, true },
        {0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

static void CopyParams( ICalcObjectContainerPtr in, ICalcObjectContainerPtr out )
// Скопировать все вычисляемые параметры из старого списка в новый
{
        if( in == 0 || out == 0)
                return;
        
        long lParamPos = 0;
        in->GetFirstParameterPos( &lParamPos );
        while( lParamPos )
        {
                ParameterContainer      *pc;
                in->GetNextParameter( &lParamPos, &pc );
                out->DefineParameterFull( pc, 0 );
        }
}

static IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
        INamedDataObject2Ptr sptrNDOParent(sptrParent);
        long lPos = 0;
        sptrNDOParent->GetFirstChildPosition(&lPos);
        while (lPos)
        {
                IUnknownPtr sptrChild;
                sptrNDOParent->GetNextChild(&lPos, &sptrChild);
                if (sptrChild!=0)
                {
                        if (::CheckInterface(sptrChild, guid))
                                return sptrChild;
                }
        }
        return NULL;
}

//собственно, главная функция - выпрямляет один объект.
//если с ним что-то не так (например, нет оси) - возвращает 0
IMeasureObjectPtr ErectOneChrom(IMeasureObjectPtr chrom)
{
        //1. получить ось
        //2. определить длину оси
        //3. определить ширину объекта (пока можно ограничить)
        //4. по всей оси - создаем объект

        //получим ось
        IChromosomePtr ptrC = IChromosomePtr(find_child_by_interface(chrom, IID_IChromosome));
        if(ptrC==0) return 0;

        //длина оси
        long lAxisSize=0;
        BOOL bIsManual;
        ptrC->GetAxisSize(AT_LongAxis, &lAxisSize, &bIsManual);
        if(lAxisSize<2) return 0;

        //получим саму ось
        smart_alloc(axis, _point, lAxisSize);
        ptrC->GetAxis(AT_LongAxis, axis);

        //скопируем ее во вспомогательный массив, увеличим число точек и сгладим
        int len2=lAxisSize*2-1;
        smart_alloc(daxis0, _dpoint, len2);
        daxis0[0]=axis[0];
        {for(int i=1; i<lAxisSize; i++)
        {
                daxis0[i*2]=axis[i];
                daxis0[i*2-1].x=(daxis0[i*2].x+daxis0[i*2-2].x)/2;
                daxis0[i*2-1].y=(daxis0[i*2].y+daxis0[i*2-2].y)/2;
        }}
        { //сгладим ось
                for(int i=1; i<len2-1; i++) 
                {
                        daxis0[i].x=(daxis0[i-1].x+daxis0[i].x+daxis0[i+1].x)/3;
                        daxis0[i].y=(daxis0[i-1].y+daxis0[i].y+daxis0[i+1].y)/3;
                }
                for(i=len2-2; i>0; i--) 
                {
                        daxis0[i].x=(daxis0[i-1].x+daxis0[i].x+daxis0[i+1].x)/3;
                        daxis0[i].y=(daxis0[i-1].y+daxis0[i].y+daxis0[i+1].y)/3;
                }
        }


        //прикинем длину объекта
        double dlen=0;
        for(int i=1; i<len2-1; i++)
        {
                dlen+=hypot(daxis0[i].x-daxis0[i-1].x,daxis0[i].y-daxis0[i-1].y);
        }

        long len=int(dlen);
        smart_alloc(daxis, _dpoint, len);
        smart_alloc(daxisvect, _dpoint, len);

        //for(i=0; i<len; i++) daxis[i]=axis[i];
        dlen=0;
        int j=0;
        for(i=1; i<len2; i++)
        {
                while(dlen>=j && j<len)
                {
                        daxis[j]=daxis0[i];
                        j++;
                }
                dlen+=hypot(daxis0[i].x-daxis0[i-1].x,daxis0[i].y-daxis0[i-1].y);
        }
        while(dlen>=j && j<len) //вроде не надо, но на всякий случай
        {
                daxis[j]=daxis0[len2-1];
                j++;
        }

        { //сгладим ось
                for(int i=1; i<len-1; i++) 
                {
                        daxis[i].x=(daxis[i-1].x+daxis[i].x+daxis[i+1].x)/3;
                        daxis[i].y=(daxis[i-1].y+daxis[i].y+daxis[i+1].y)/3;
                }
                for(i=len-2; i>0; i--) 
                {
                        daxis[i].x=(daxis[i-1].x+daxis[i].x+daxis[i+1].x)/3;
                        daxis[i].y=(daxis[i-1].y+daxis[i].y+daxis[i+1].y)/3;
                }
        }
        
        { //посчитаем вектора направления
                for(int i=0; i<len; i++) 
                {
                        int i0=max(i-3,0);
                        int i1=min(i+3,len-1);
                        double dx=daxis[i1].x-daxis[i0].x;
                        double dy=daxis[i1].y-daxis[i0].y;
                        double d=hypot(dx,dy); d=max(d,1e-5);
                        daxisvect[i].x=dx/d; daxisvect[i].y=dy/d;
                }
        }

        //получим image объекта
        IUnknownPtr sptr2;
        chrom->GetImage(&sptr2);
        IImage3Ptr image(sptr2);
        if(image==0) return 0;

        _point offset;
        image->GetOffset(&offset);
        int cx,cy;
        image->GetSize(&cx,&cy);

        _bstr_t bstrCC;
        int nPanes;
        {
                IUnknown *punkCC = 0;
                image->GetColorConvertor( &punkCC );
                IColorConvertor2Ptr     ptrCC( punkCC );
                punkCC->Release();

                ptrCC->GetColorPanesCount(&nPanes);

                BSTR    bstr;
                ptrCC->GetCnvName( &bstr );
                bstrCC = bstr;
                ::SysFreeString( bstr ); 
        }

        smart_alloc(srcRows, color*, cy*nPanes);
        {for(int pane=0; pane<nPanes; pane++)
        {
                for(int y=0; y<cy; y++)
                {
                        image->GetRow(y, pane, srcRows+pane*cy+y);
                }
        }}

        smart_alloc(srcMask, byte*, cy);
        {for(int y=0; y<cy; y++)
        {
                image->GetRowMask(y, srcMask+y);
        }}

        //посчитаем ширину нового объекта
        int width2=50; //половина ширины;
        
        IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
        IImage3Ptr imgOut(punk2);
        punk2->Release();

        imgOut->CreateImage(width2*2+1,len,bstrCC);
        imgOut->InitRowMasks();

        offset.x+=cx/2; offset.y+=cy/2;
        offset.x-=width2; offset.y-=len/2;
        imgOut->SetOffset(offset,false);

        {//fill image
                smart_alloc(pc, color*, nPanes);
                for(int y=0; y<len; y++)
                {
                        byte *m;
                        imgOut->GetRowMask(y,&m);
                        m+=width2;
                        for(int pane=0; pane<nPanes; pane++)
                        {
                                imgOut->GetRow(y, pane, pc+pane);
                                pc[pane]+=width2;
                        }

                        int badCount=-500;
                        for(int x=0; x<=width2; x++)
                        {
                                double xd=daxis[y].x+daxisvect[y].y*x;
                                double yd=daxis[y].y-daxisvect[y].x*x;
                                if(xd>=0 && xd<=cx && yd>=0 && yd<=cy && badCount<5)
                                {
                                        int xx=int(xd); xx=max(min(xx,cx-2),0);
                                        int yy=int(yd); yy=max(min(yy,cy-2),0);
                                        double ax=xd-xx, ay=yd-yy;
                                        double mm=
                                                (srcMask[yy][xx]*(1-ax)+srcMask[yy][xx+1]*ax)*(1-ay)+
                                                (srcMask[yy+1][xx]*(1-ax)+srcMask[yy+1][xx+1]*ax)*ay;
                                        if(mm<128)
                                        {
                                                m[x]=0;
                                                badCount++;
                                        }
                                        else
                                        {
                                                m[x] = 255;
                                                if(badCount>0) badCount--;
                                                if(badCount<0) badCount=0;
                                        }
                                        for(int pane=0; pane<nPanes; pane++)
                                        {
                                                double cc=
                                                        (srcRows[pane*cy+yy][xx]*(1-ax)+srcRows[pane*cy+yy][xx+1]*ax)*(1-ay)+
                                                        (srcRows[pane*cy+yy+1][xx]*(1-ax)+srcRows[pane*cy+yy+1][xx+1]*ax)*ay;
                                                cc=max(min(cc,65535),0);
                                                pc[pane][x]=int(cc);
                                        }
                                }
                                else
                                {
                                        m[x]=0;
                                        badCount++;
                                }
                        }

                        //в другую сторону
                        badCount=-500;
                        for(x=0; x>=-width2; x--)
                        {
                                double xd=daxis[y].x+daxisvect[y].y*x;
                                double yd=daxis[y].y-daxisvect[y].x*x;
                                if(xd>=0 && xd<=cx && yd>=0 && yd<=cy && badCount<5)
                                {
                                        int xx=int(xd); xx=max(min(xx,cx-2),0);
                                        int yy=int(yd); yy=max(min(yy,cy-2),0);
                                        double ax=xd-xx, ay=yd-yy;
                                        double mm=
                                                (srcMask[yy][xx]*(1-ax)+srcMask[yy][xx+1]*ax)*(1-ay)+
                                                (srcMask[yy+1][xx]*(1-ax)+srcMask[yy+1][xx+1]*ax)*ay;
                                        if(mm<128)
                                        {
                                                m[x]=0;
                                                badCount++;
                                        }
                                        else
                                        {
                                                m[x] = 255;
                                                if(badCount>0) badCount--;
                                                if(badCount<0) badCount=0;
                                        }
                                        for(int pane=0; pane<nPanes; pane++)
                                        {
                                                double cc=
                                                        (srcRows[pane*cy+yy][xx]*(1-ax)+srcRows[pane*cy+yy][xx+1]*ax)*(1-ay)+
                                                        (srcRows[pane*cy+yy+1][xx]*(1-ax)+srcRows[pane*cy+yy+1][xx+1]*ax)*ay;
                                                cc=max(min(cc,65535),0);
                                                pc[pane][x]=int(cc);
                                        }
                                }
                                else
                                {
                                        m[x]=0;
                                        badCount++;
                                }
                        }

                }
        }

        imgOut->InitContours();

        //create object
        IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
        IMeasureObjectPtr objOut(punk );
        punk->Release();
        
        objOut->SetImage( imgOut );

        return objOut;
}

bool CAvgChromFilter::InvokeFilter()
{
        INamedDataObject2Ptr objectsIn(GetDataArgument());
        INamedDataObject2Ptr objectsOut(GetDataResult());
        
        if(objectsOut==0 || objectsIn==0) return false;
        
        long nCountIn; objectsIn->GetChildsCount(&nCountIn);
        if(!nCountIn) return false;
        
        int nN=0;
        StartNotification(nCountIn);
        
        //CopyParams(objectsIn, objectsOut);
        
        long pos; objectsIn->GetFirstChildPosition(&pos);
        while( pos ) //по всем объектам
        {
                IUnknownPtr sptr;
                objectsIn->GetNextChild(&pos, &sptr);
                IMeasureObjectPtr object(sptr);
                if(object==0) continue;

                IMeasureObjectPtr objOut(ErectOneChrom(object));
                {
                        DWORD dwFlags;
                        INamedDataObject2Ptr ptrObject(objOut);
                        if(ptrObject!=0)
                        {
                                ptrObject->GetObjectFlags(&dwFlags);
                                ptrObject->SetParent(objectsOut, dwFlags);
                        }
                }

                Notify(nN++);
        }
        
        //GUID guidNewBase;
        //objectsIn->GetBaseKey(&guidNewBase);
        //objectsOut->SetBaseKey(&guidNewBase);
        
        FinishNotification();
        
        return true;
}

CAvgChromFilter::CAvgChromFilter()
{
}

CAvgChromFilter::~CAvgChromFilter()
{
}
