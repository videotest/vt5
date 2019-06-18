#include "StdAfx.h"
#include "BinaryFromObjects.h"
#include "nameconsts.h"
#include "misc_classes.h"
#include "misc_utils.h"

_ainfo_base::arg CBinaryFromObjectsInfo::s_pargs[] =
{
	{"srcListObj",	szArgumentTypeObjectList, 0, true, true },
	{"imgBin",	szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

CBinaryFromObjects::CBinaryFromObjects(void)
{
}

CBinaryFromObjects::~CBinaryFromObjects(void)
{
}

bool CBinaryFromObjects::InvokeFilter()
{
	IBinaryImagePtr ptrImageBin( GetDataResult("imgBin") );
	//IBinaryImagePtr imageBin;
	IMeasureObjectPtr Src;
	INamedDataObject2Ptr ptrSrc(GetDataArgument("srcListObj"));
	IImage3Ptr SrcImage;

	//������� ��������� �� ������ ��������
	//IUnknownPtr punkObjectList(::GetActiveObjectFromContext(Unknown(), szTypeObjectList),false);
	//if(punkObjectList==0)
	//	return false;
	if(ptrSrc==0)
		return false;
	INamedDataObject2Ptr ptrObjects = ptrSrc;//punkObjectList;
	//������� ����� ��������
	long nObjects; 
	ptrObjects->GetChildsCount(&nObjects);
	
	//������� ������� ������� ��������
	IUnknownPtr punkImage(::GetActiveObjectFromContext(m_ptrTarget, szTypeImage),false);
	int nWidth,nHeight;
	IImage3Ptr sptrImage(punkImage);
	sptrImage->GetSize(&nWidth, &nHeight);

	//�������� ��������
	ptrImageBin->CreateNew(nWidth,nHeight);
	//������ ��� ������
	for(int y=0; y<nHeight; y++)
	{
		byte *pdst;
		ptrImageBin->GetRow(&pdst, y, false);
		FillMemory(pdst,nWidth*sizeof(byte),0);
	}

	//���������� �� ���� ��������
	int nObject=0;
	long posObject=0; 
	ptrObjects->GetFirstChildPosition(&posObject);

	StartNotification( nObjects, 1, 1 );

	while( posObject && nObject<nObjects ) 
	{
		//����� ������
		IUnknownPtr sptr;
		ptrObjects->GetNextChild(&posObject, &sptr);
		Src = sptr;
		if(Src==0)
			return false;
		//�������� ������ � ��� �����������
		IUnknownPtr sptr2;
		Src->GetImage(&sptr2);
		SrcImage = sptr2;
		if(SrcImage==0) 
			return false;
		//��������� ���� � ���� ������� � ��������
		int xSrc=0, ySrc=0;
		SrcImage->GetSize(&xSrc,&ySrc);
		_point ptOffset(0,0);
		SrcImage->GetOffset(&ptOffset);
		if(xSrc+ptOffset.x<=nWidth && ySrc+ptOffset.y<=nHeight)
		{
			for(int y=0; y<ySrc; y++)
			{
				byte *pSrc, *pBin;
				SrcImage->GetRowMask(y, &pSrc);
				ptrImageBin->GetRow(&pBin, y+ptOffset.y, false);
				pBin+=ptOffset.x;
				for(int x=0; x<xSrc; x++)
					if(pSrc[x])
						pBin[x]=pSrc[x];
			}
		}
		nObject++;
	}

	FinishNotification();

	return true;
}