#include "StdAfx.h"
#include "watershed.h"
#include "nameconsts.h"
#include "alloc.h"
#include "\vt5\awin\misc_ptr.h"
#include "misc_utils.h"

_ainfo_base::arg CWatershedInfo::s_pargs[] =
{
	{"Viscosity",	szArgumentTypeInt, "1", true, false },
	{"Source",	szArgumentTypeImage, 0, true, true },
	{"Labels",	szArgumentTypeBinaryImage, 0, true, true },
	//{"Watershed",	szArgumentTypeObjectList, 0, false, true },
	//{"DbgOut",	szArgumentTypeImage, 0, false, true },
	{"BinOut",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

CWatershed::CWatershed(void)
{
}

CWatershed::~CWatershed(void)
{
}

static void MarkSegments(byte **buf, color **cnt, int cx, int cy)
{
	if(cx<3 || cy<3) return; //на такой фигне не работаем

	_ptr_t<int> smart_ind(256); // _ptr_t - из awin, он умеет реаллокейтить
	int *ind = smart_ind.ptr();

	int cur_ind=0;
	for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
	//индексы дл€ объединени€ нескольких областей в одну
	
	cnt[0][0]=cur_ind=1;
	for(int x=1; x<cx; x++)
	{
		if(buf[0][x]!=buf[0][x-1])
		{
			cur_ind++;
			if(cur_ind>=smart_ind.size())
			{
				smart_ind.alloc(smart_ind.size()*2);
				ind = smart_ind.ptr();
				for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
			}
		}
		cnt[0][x]=cur_ind;
	} //проиндексировали первую строку
	
	for(int y=1; y<cy; y++)
	{
		for(int x=0; x<cx; )
		{ //идем слева направо по строке
			int i=-1;
			int xx=x;
			for(; x<cx && buf[y][x]==buf[y][xx]; x++)
			{	//если значени€ совпадают - продолжим текущую область
				//(если уже выбрана область правее - то ее)
				if(buf[y][x]==buf[y-1][x] && cnt[y-1][x]>i) i=cnt[y-1][x];
			}				
			if(i<0)
			{ //попали в новую область
				cur_ind++;
				if(cur_ind>=smart_ind.size())
				{
					smart_ind.alloc(smart_ind.size()*2);
					ind = smart_ind.ptr();
					for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
				}
				i=cur_ind;
			}
			for(; xx<x; xx++) cnt[y][xx]=i;
		}
	}
	
	for(y=1; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(cnt[y-1][x]<cnt[y][x] && buf[y-1][x]==buf[y][x])
				ind[cnt[y-1][x]]=ind[cnt[y][x]];
		}
	}
	
	for(i=cur_ind; i>=0; i--)
		ind[i]=ind[ind[i]];

	// debug - сформируем карту цветов

	smart_alloc(rev, int, smart_ind.size());
	int ind_cnt=0;
	for(i=0; i<cur_ind; i++)
	{
        if(ind[i]==i) ind_cnt++;
		rev[i]=ind_cnt; // номера от 1 и выше
	}

	for(i=0; i<cur_ind; i++)
	{
		color src = rev[i], dst = 0;
		for(int j=0; j<16; j++)
		{
			dst <<= 1;
			if(src & 1) dst++;
			src >>= 1;
		}
		rev[i] = dst;
		//rev[i] = rand()%65536;
	}

	for(y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			cnt[y][x] = rev[ind[cnt[y][x]]];
		}
	}

}

/*
#define CHECK(dx,dy) \
if(dstRows0[y+dy][x+dx]>dstRows0[y][x]) \
{ \
	dstRows0[y][x] = min(srcRows[y+dy][x+dx],dstRows0[y+dy][x+dx]); \
	dstRows0[y][x] = max(0+dstRows0[y][x], 0); \
	dstRows1[y][x] = dstRows1[y+dy][x+dx]; \
	dstRows2[y][x] = nPass*8192; \
}
*/
#define CHECK(dx,dy) \
{ \
	int c = min(srcRows[y][x],dstRows0[y+dy][x+dx]-nViscosity); \
	c=max(c,0); \
	if(c>dstRows0[y][x]) \
	{ \
		dstRows0[y][x] = c; \
		dstRows1[y][x] = dstRows1[y+dy][x+dx]; \
		dstRows2[y][x] = dstRows2[y+dy][x+dx]; \
	} \
	if(dstRows1[y][x] == dstRows1[y+dy][x+dx]) \
		dstRows2[y][x] = max(dstRows2[y][x], dstRows2[y+dy][x+dx]); \
}

#define CHECK2(dx,dy) \
{ \
	if((binRows[y][x]&128) == 0) { CHECK(dx,dy); } \
	else dstRows2[y][x] = nPass*8192; \
}


bool CWatershed::InvokeFilter()
{
	IImage3Ptr      pArg( GetDataArgument("Source") );
	IBinaryImagePtr	pLabels(GetDataArgument("Labels"));
	//INamedDataObject2Ptr pRes(GetDataResult("Watershed"));
	//IImage3Ptr      pOutImg( GetDataResult("DbgOut") );
	IBinaryImagePtr	pBinOut(GetDataResult("BinOut"));
	int nDefViscosity = GetArgLong("Viscosity");
	nDefViscosity = max(1,nDefViscosity);


	IUnknown *punk5=::CreateTypedObject(_bstr_t(szTypeImage)); // создаем, но не включаем в контекст
	IImage3Ptr pOutImg = punk5;
	if (punk5!=0) punk5->Release();

	if( pArg==0 || pLabels==0 || pBinOut==0 || pOutImg==0 ) return false;

	int cx=0,cy=0;
	pArg->GetSize(&cx,&cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);

	{ // сверить размеры изображений
		int cx1=0,cy1=0;
		pLabels->GetSizes(&cx1,&cy1);
		if(cx1!=cx || cy1!=cy) return false;
		POINT ptOffset1;
		pLabels->GetOffset(&ptOffset1);
		if(ptOffset1.x!=ptOffset.x && ptOffset1.y!=ptOffset.y) return false;
	}

	pOutImg->CreateImage(cx, cy, _bstr_t("RGB"),-1);
	pOutImg->SetOffset(ptOffset, false);

	pBinOut->CreateNew(cx, cy);
	pBinOut->SetOffset( ptOffset, TRUE);

	smart_alloc(srcRows, color *, cy);
	smart_alloc(dstRows0, color *, cy);
	smart_alloc(dstRows1, color *, cy);
	smart_alloc(dstRows2, color *, cy);
	smart_alloc(binRows, byte *, cy);
	smart_alloc(binOutRows, byte *, cy);
	int nPane=1;
 	for(int y=0; y<cy; y++)
	{
		pArg->GetRow(y, nPane, &srcRows[y]);
		pOutImg->GetRow(y, 0, &dstRows0[y]);
		pOutImg->GetRow(y, 1, &dstRows1[y]);
		pOutImg->GetRow(y, 2, &dstRows2[y]);
		pLabels->GetRow(&binRows[y], y, 0);
		pBinOut->GetRow(&binOutRows[y], y, 0);
	}

	StartNotification(1);
	Notify(0);
	FinishNotification();

	MarkSegments(binRows, dstRows1, cx,cy);

	smart_alloc(sum0,int,65536); smart_sum0.zero();
	smart_alloc(sum1,__int64,65536); smart_sum1.zero();
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int n = dstRows1[y][x];
			sum0[n]++;
			sum1[n]+=srcRows[y][x];
		}
	}
	for(int i=0; i<65536; i++) sum1[i]/=max(1,sum0[i]);

	// собственно Watershed
	// используем: binRows - чтобы работать только между объектами
	// dstRows1 - метки объектов
	// dstRows0 - "удаленность" от объектов (€ркость самого темного пиксела, через который проходили)
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(binRows[y][x]&&128)
                dstRows0[y][x] = 65535;
			else
			{
				dstRows1[y][x] = 0;
				dstRows0[y][x] = 0;
			}
			dstRows2[y][x] = 0;
		}
	}

	for(int nPass=0; nPass<8; nPass++)
	{
		int nViscosity = (8192 >> nPass) + nDefViscosity;

		for(int y=0; y<cy; y++)
			for(int x=1; x<cx; x++)
				CHECK2(-1,0);
		for(int y=cy-1; y>=0; y--)
			for(int x=cx-2; x>=0; x--)
				CHECK2(1,0);
		for(int y=1; y<cy; y++)
			for(int x=0; x<cx; x++)
				CHECK2(0,-1);
		for(int y=cy-2; y>=0; y--)
			for(int x=cx-1; x>=0; x--)
				CHECK2(0,1);
		if(nPass>=6)
		{
			for(int y=0; y<cy; y++)
				for(int x=0; x<cx; x++)
					if((binRows[y][x]&128)==0 && dstRows2[y][x]<(nPass-3)*8192)
					{
						dstRows0[y][x]=0;
						dstRows1[y][x]=0;
					}
		}
	}

	// коррекци€ - шаг всегда ровно на 1 пиксел
	for(int nPass=8; nPass<12; nPass++)
	{
		int nViscosity = (8192 >> nPass) + 1;
		for(int y=0; y<cy; y++)
			for(int x=0; x<cx-1; x++)
				CHECK2(1,0);
		for(int y=cy-1; y>=0; y--)
			for(int x=cx-1; x>0; x--)
				CHECK2(-1,0);
		for(int y=0; y<cy-1; y++)
			for(int x=0; x<cx; x++)
				CHECK2(0,1);
		for(int y=cy-1; y>0; y--)
			for(int x=cx-1; x>=0; x--)
				CHECK2(0,-1);
	}

	for(int y=1; y<cy-1; y++)
	{
		for(int x=1; x<cx-1; x++)
		{
			int n = dstRows1[y][x];
			if( dstRows1[y][x-1]==n && dstRows1[y][x+1]==n &&
				dstRows1[y-1][x]==n && dstRows1[y+1][x]==n
				//&&
				//dstRows1[y-1][x-1]==n && dstRows1[y-1][x+1]==n &&
				//dstRows1[y+1][x-1]==n && dstRows1[y+1][x+1]==n
				)
			{
				dstRows2[y][x]=0;
				binOutRows[y][x] = n>0 ? 255 : 0;
			}
			else
			{
				int c = srcRows[y][x];
				c = min(65535,max(0,(sum1[n]-c)*4));
				dstRows2[y][x]=c;
				binOutRows[y][x] = 0;
			}
		}
	}

	return true;
}

bool CWatershed::CanDeleteArgument( CFilterArgument *pa )
{
	if (pa->m_bstrArgName==_bstr_t("Source")) return(false);
	else return __super::CanDeleteArgument(pa);
};
