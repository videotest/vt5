#include "stdafx.h"
#include "ObjectPool.h"
#include "misc_utils.h"
#include "nameconsts.h"

CObjectPool::CObjectPool()
{
	m_List=0;
	m_Image=0;
	m_lPos=0;
	m_cx=m_cy=0;
}

CObjectPool::~CObjectPool()
{
}

void CObjectPool::StartProcessing(IUnknown *list, IUnknown *image, int cx, int cy)
{	// начать выбор объектов из листа	(сразу удалив неформатные)
	m_List=list;
	m_Image=image;
	if(m_List==0) return;
	m_cx=cx;
	m_cy=cy;
	TPOS lPos=0;
	m_List->GetFirstChildPosition(&lPos);
	m_Objects.deinit();
	while(lPos)
	{	// пройдем по всем объектам. нужные - в свой список, ненужные - грохнем
		int cx1=0,cy1=0;
		IUnknown *punk;
		m_List->GetNextChild(&lPos, &punk);
		IMeasureObjectPtr ptrO=punk;
		if(punk!=0) punk->Release();
		if(ptrO!=0)
		{
			IUnknown *punk2;
			ptrO->GetImage( &punk2 );
			IImage3Ptr ptrI;
			ptrI=punk2;
			if(punk2!=0) punk2->Release();
			if(ptrI!=0)
			{
				ptrI->GetSize(&cx1, &cy1);
			}
		}
		if(cx1==m_cx && cy1==m_cy)
		{ // хороший объект - запомним
			ptrO->AddRef(); // потом можно переделать на работу без AddRef, но так спокойнее.
			m_Objects.insert(ptrO);
		}
		else
		{ // плохой - удалим
			INamedDataObject2Ptr ptrObject(ptrO);
			//IUnknown *punkND;
			//ptrObject->GetData(&punkND);
			//RemoveFromDocData(punkND, ptrObject);

			if(1) if(ptrObject!=0) ptrObject->SetParent(0, apfNotifyNamedData);	//грохнуть его нафиг
			//::DeleteObject( punkDocData, punkObject );
		}
	}
	m_lPos=m_Objects.head(); // указатель на начало
}

INamedDataObject2Ptr CObjectPool::GetNextObject()
{
	if(m_lPos) return m_Objects.next(m_lPos);
	return 0;
}

bool CObjectPool::CorrectRect(_rect &rect)
{ // привести rect к размеру m_cx*m_cy, чтобы при этом не вылезал за пределы image
	int cx=rect.width(), cy=rect.height();
	if(cx>m_cx || cy>m_cy) return false;
	
	rect.left -= (m_cx-cx)/2;
	rect.top -= (m_cy-cy)/2;

	int cx0=0, cy0=0;
	m_Image->GetSize(&cx0, &cy0);

	rect.left=max(0,min(cx0-m_cx, rect.left));
	rect.top=max(0,min(cy0-m_cy, rect.top));
	rect.right=rect.left+m_cx;
	rect.bottom=rect.top+m_cy;

	return true;
}

INamedDataObject2Ptr CObjectPool::GetObject(_rect &rect)
{
	IMeasureObjectPtr ptrO=0;
	bool bRectOk=CorrectRect(rect);
	if(bRectOk && m_lPos)
	{
		ptrO=m_Objects.next(m_lPos);
		IUnknown *punk2=0;
		ptrO->GetImage( &punk2 );
		IImage3Ptr      ptrI(punk2);
		if(punk2!=0) punk2->Release();
		ptrI->SetOffset(rect.top_left(),false);
	}
	else
	{
		//-------------------- image
		IUnknown        *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
		IImage3Ptr      ptrI(punk2);
		if(punk2!=0) punk2->Release();
		INamedDataObject2Ptr ptrImg(ptrI);
		ptrI->CreateVirtual( rect );
		ptrI->InitRowMasks();
		//ptrI->InitContours();
		ptrImg->SetParent( m_Image,  0);
		
		//-------------------- object
		IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
		ptrO = punk;
		if(punk!=0) punk->Release();
		
		ptrO->SetImage( ptrI );
		INamedDataObject2Ptr ptrObject(ptrO);
		ptrObject->SetParent(m_List, 0);
	}
	return ptrO;
}

void CObjectPool::StopProcessing()
{	// закончить выбор объектов и удалить лишние
	while(m_lPos)
	{
		INamedDataObject2Ptr ptrObject(m_Objects.next(m_lPos));
		//if(ptrObject!=0) 
		//{
		//	IUnknown *punkND;
		//	ptrObject->GetData(&punkND);
		//	RemoveFromDocData(punkND, ptrObject);
		//}
		if(ptrObject!=0) ptrObject->SetParent(0, apfNotifyNamedData);	//грохнуть его нафиг из листа
	}
}

