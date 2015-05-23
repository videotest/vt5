#include "stdafx.h"
#include "cutframe.h"
#include "measure5.h"
#include "docview5.h"
#include "\vt5\awin\misc_list.h"

_ainfo_base::arg CCutFrameInfo::s_pargs[] =
{
	{"Frame",       szArgumentTypeImage, 0, true, true },
	{"ObjectList",    szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};

CCutFrame::CCutFrame(void)
{
}

CCutFrame::~CCutFrame(void)
{
}

IUnknown* CCutFrame::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	if( iid == IID_ILongOperation )return (ILongOperation*)this;
	return CAction::DoGetInterface( iid );
}

//extract object name from Unknown
static _bstr_t GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0) return "";

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr)
		return "";

	_bstr_t	bstrRes(bstr);
	::SysFreeString(bstr);

	return bstrRes;
}

IUnknown *CCutFrame::GetContextObject(_bstr_t bstrName, _bstr_t bstrType)
{ //Возвращает с AddRef

	IDataContext3Ptr ptrC = m_ptrTarget;

	if( bstrName==_bstr_t("") )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	LONG_PTR lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( GetObjectName( punkImage ) == bstrName )
			{
				//if( punkImage )
					//punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}

HRESULT CCutFrame::DoInvoke()
{
	if (m_ptrTarget==0) return false;
	IDocumentPtr doc(m_ptrTarget);
	if(doc==0) return S_FALSE;

	_bstr_t bstrFrame = GetArgString("Frame");
	_bstr_t bstrObjectList=GetArgString("ObjectList");

	//IImage3Ptr      pArg( GetDataArgument() );
	IUnknown *punk = GetContextObject(bstrFrame, szTypeImage);
	IImage3Ptr ptrFrame(punk);
	if (punk!=0) punk->Release();

	IUnknown *punk2 = GetContextObject(bstrObjectList, szTypeObjectList);
	INamedDataObject2Ptr ptrObjectList(punk2);
	if (punk2!=0) punk2->Release();

	if( ptrFrame==NULL || ptrObjectList==NULL)
		return S_FALSE;

	_rect cr_frame;
	{
		_size size;
		ptrFrame->GetSize((int*)&size.cx,(int*)&size.cy);
		_point ptOffset;
		ptrFrame->GetOffset(&ptOffset);
		cr_frame=_rect(ptOffset,size);
	}
	long lBorderWidth = GetValueInt(GetAppUnknown(), "\\CutFrame", "BorderWidth", 3);
	cr_frame.inflate(-lBorderWidth,-lBorderWidth);

	long nCountIn; ptrObjectList->GetChildsCount(&nCountIn);
	if(!nCountIn) return S_FALSE;

	int nN=0;
	StartNotification(nCountIn);

	_list_t<IMeasureObjectPtr> objects;

	POSITION pos; ptrObjectList->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		IUnknownPtr sptr;
		ptrObjectList->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		objects.add_tail(object);
	}

	for (TPOS pos = objects.head(); pos != 0; pos = objects.next(pos))
	{
		IMeasureObjectPtr object=objects.get(pos);

		IUnknownPtr sptr2;
		object->GetImage(&sptr2);
		IImage3Ptr image(sptr2);
		
		_rect cr;
		{
			_size size;
			image->GetSize((int*)&size.cx,(int*)&size.cy);
			_point ptOffset;
			image->GetOffset(&ptOffset);
			cr=_rect(ptOffset,size);
		}

		if(cr.left<cr_frame.left || cr.top<=cr_frame.top ||
			cr.right>=cr_frame.right || cr.bottom>=cr_frame.bottom)
		{
			RemoveFromDocData( doc, object);
		}

		Notify(nN++);
	}
 	FinishNotification();

	return S_OK;
}