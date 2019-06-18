#include "stdafx.h"
#include "ErectChrom.h"
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
#include "docview5.h"
#include "idio_int.h"

#include "\vt5\comps\misc\chromo\utils\chromo_misc.h"

#include "carioint.h"

#include "chrom_aam_utils.h"

#define MaxColor color(-1)


inline int GetUnkCounter( IUnknown *punk )
{	
	if( !punk )	return(0);
	punk->AddRef();	
	return punk->Release();
}

/////////////////////////////////////////////////////////////////////////////
//
//
//              ErectChrom
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg        CErectChromInfo::s_pargs[] =
{
	{"Source",    szArgumentTypeObjectList, 0, true, true },
	{"Erected",    szArgumentTypeObjectList, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg        CErectChromOnInfo::s_pargs[] =
{
	{"ImageList",    szArgumentTypeObjectList, 0, true, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg        CErectOneChromOnOffInfo::s_pargs[] =
{
	{"NewState",    szArgumentTypeInt, "-1", true, false },
	{"NonInteractive",    szArgumentTypeInt, "0", true, false },
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

///////////////////////////////////////////////////////////////
CErectChromOn::CErectChromOn()
{
	m_ptrObjectList=0;
}

CErectChromOn::~CErectChromOn()
{
}

IUnknown* CErectChromOn::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

static IUnknownPtr get_document(IUnknown *punkTarget)
{
	IViewPtr ptrView( punkTarget );
	if( ptrView == 0 )
		return 0;
	
	IUnknownPtr ptrDoc;
	{
		IUnknown* punk = 0;
		ptrView->GetDocument( &punk );
		if(!punk) return 0;
		
		ptrDoc = punk;
		punk->Release(); punk=0;
	}
	
	return ptrDoc;
}

HRESULT CErectChromOn::GetActionState( DWORD *pdwState )
{
	HRESULT r=CAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

bool CErectChromOn::IsAvaible()
{
	//if(!CheckInterface(m_ptrTarget,IID_IImageView)) return false;

	IUnknown* punkObj = 0;
	IDataContextPtr sptrContext( m_ptrTarget );
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	//punkObj = ::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList);
	INamedDataObject2Ptr ptrList(punkObj);
	if(punkObj) punkObj->Release();


	//IDocumentSitePtr ptr_ds( m_ptrTarget );
	//if( ptr_ds )
	{
		//IUnknown* punk_view = 0;
		//ptr_ds->GetActiveView( &punk_view );
		//if( punk_view )
		{
			ICarioViewPtr ptrView( m_ptrTarget );
			//ICarioViewPtr ptrView( punk_view );
			//punk_view->Release();	punk_view = 0;
			if( ptrView )
			{
                if( ptrView->IsEmpty() == S_OK ) return false; // ни одной хромосомы
				CarioViewMode cvm = cvmPreview;
				ptrView->GetMode( &cvm );

				if( !( cvm == cvmPreview || cvm == cvmCellar  ) )
					return false;
				{
					IDataContextPtr sptrContext2(ptrView);
					sptrContext2->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
					if(!punkObj) return false;
					if(punkObj) punkObj->Release();
				}
			}
			else return false;
		}
		//else return false;
	}
	return true;
}

HRESULT CErectChromOn::DoInvoke()
{
	ICarioView2Ptr ptrView( m_ptrTarget );
	if( ptrView == 0 )
		return false;

	IUnknownPtr ptrDoc = get_document(m_ptrTarget);
	OnAttachTarget( ptrDoc );

	IUnknown* punkObj = 0;
	IDataContextPtr sptrContext( m_ptrTarget );
	sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	//punkObj = ::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList);
	INamedDataObject2Ptr ptrList(punkObj);
	if(punkObj) punkObj->Release();
	if(ptrList==0) return E_FAIL;
	m_ptrObjectList=ptrList;

	long nCountIn; ptrList->GetChildsCount(&nCountIn);
	if(!nCountIn) return false;

	int nN=0;
	
	long pos; ptrList->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		nN++;
		IUnknownPtr sptr;
		ptrList->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		if(object==0) continue;

		//ErectOneChrom(object);
		{
			//получим подобъект-хромосому
			IChromosomePtr ptrC = IChromosomePtr(find_child_by_interface(object, IID_IChromosome));
			if(ptrC==0) continue;

			IUnknownPtr ptrUnkErector(LPOLESTR(_bstr_t(pszErectCCProgID)));
			IChromoConvertPtr ptrErector=IChromoConvertPtr(ptrUnkErector);

			//для undo
			ptrC->AddRef(); m_undoChrom.insert(ptrC);

			IChromoConvert *pChromConv2;
			ptrC->GetChromoConvert(&pChromConv2);
			m_undoChromConv.insert(pChromConv2);

			int cnt=GetUnkCounter(pChromConv2);

			ptrC->SetChromoConvert(IChromoConvertPtr(ptrErector));
			int cnt2=GetUnkCounter(ptrErector);
			ptrErector->Recalc(ptrC);
		}

	}

	ptrView->SetErectedStateAll(true);	// изменим состояние

	{ //просигналим, чтоб обновить вьюху
		INamedDataObject2Ptr NDO(ptrList);
		IUnknown *punk = NULL;
		HRESULT hr = NDO->GetData(&punk);
		if (SUCCEEDED(hr) && punk)
		{
			long l = cncReset;
			::FireEvent(punk, szEventChangeObjectList, ptrList, NULL, &l, 0);
			punk->Release();
		}
	}

	return S_OK;
}

HRESULT CErectChromOn::DoUndo()
{
	long	posChrom = m_undoChrom.head();
	long	posChromConv = m_undoChromConv.head();
	while( posChrom && posChromConv ) 
	{
		long prevPosChromConv=posChromConv;
		IUnknown *punkChrom=m_undoChrom.next(posChrom);
		IUnknown *punkChromConv=m_undoChromConv.next(posChromConv);
		IChromosomePtr ptrChrom(punkChrom);
		IChromoConvertPtr ptrChromConv(punkChromConv);

		if(ptrChrom==0) continue; //мало ли что...

		IChromoConvert *pChromConv2;
		ptrChrom->GetChromoConvert(&pChromConv2);
		ptrChrom->SetChromoConvert(ptrChromConv);

		//cnt2=GetUnkCounter(pChromConv2);
		//cnt=GetUnkCounter(punkChromConv);
		//cnt=cnt;

		//if(pChromConv2) pChromConv2->Release(); //или в список
		m_undoChromConv.remove(prevPosChromConv);
		m_undoChromConv.insert_before(pChromConv2,posChromConv);

		if(ptrChromConv) ptrChromConv->Recalc(ptrChrom);

		//???punkChromConv->Release();
	}

	{ //просигналим, чтоб обновить вьюху
		INamedDataObject2Ptr NDO(m_ptrObjectList);
		if(NDO!=0)
		{
			IUnknown *punk = NULL;
			HRESULT hr = NDO->GetData(&punk);
			if (SUCCEEDED(hr) && punk)
			{
				long l = cncReset;
				::FireEvent(punk, szEventChangeObjectList, m_ptrObjectList, NULL, &l, 0);
				punk->Release();
			}
		}
	}

	return S_OK;
}

HRESULT CErectChromOn::DoRedo()
{
	long	posChrom = m_undoChrom.head();
	long	posChromConv = m_undoChromConv.head();
	while( posChrom && posChromConv ) 
	{
		long prevPosChromConv=posChromConv;
		IUnknown *punkChrom=m_undoChrom.next(posChrom);
		IUnknown *punkChromConv=m_undoChromConv.next(posChromConv);
		IChromosomePtr ptrChrom(punkChrom);
		IChromoConvertPtr ptrChromConv(punkChromConv);

		if(ptrChrom==0) continue; //мало ли что...

		IChromoConvert *pChromConv2;
		ptrChrom->GetChromoConvert(&pChromConv2);
		ptrChrom->SetChromoConvert(ptrChromConv);

		//cnt2=GetUnkCounter(pChromConv2);
		//cnt=GetUnkCounter(punkChromConv);
		//cnt=cnt;

		//if(pChromConv2) pChromConv2->Release(); //или в список
		m_undoChromConv.remove(prevPosChromConv);
		m_undoChromConv.insert_before(pChromConv2,posChromConv);

		if(ptrChromConv) ptrChromConv->Recalc(ptrChrom);

		//???punkChromConv->Release();
	}

	{ //просигналим, чтоб обновить вьюху
		INamedDataObject2Ptr NDO(m_ptrObjectList);
		if(NDO!=0)
		{
			IUnknown *punk = NULL;
			HRESULT hr = NDO->GetData(&punk);
			if (SUCCEEDED(hr) && punk)
			{
				long l = cncReset;
				::FireEvent(punk, szEventChangeObjectList, m_ptrObjectList, NULL, &l, 0);
				punk->Release();
			}
		}
	}

	return S_OK;
}

///////////////////////////////////////

CErectOneChromOnOff::CErectOneChromOnOff()
{
}

CErectOneChromOnOff::~CErectOneChromOnOff()
{
}

IUnknown* CErectOneChromOnOff::DoGetInterface( const IID &iid )
{
	//if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CInteractiveAction::DoGetInterface( iid );
}

LRESULT	CErectOneChromOnOff::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
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

bool CErectOneChromOnOff::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;

	m_hCurActive=::LoadCursor(App::handle(), MAKEINTRESOURCE(IDC_ERECT));

	//m_action=0;

	bool bNonInteractive = GetArgLong("NonInteractive")!=0;

	if(bNonInteractive)
	{
		Finalize();
		//LeaveMode();
		//return false;
	}

	return true;
}

void CErectOneChromOnOff::DeInit()
{
	//::InvalidateRect(m_hwnd, 0, false);
}

bool CErectOneChromOnOff::Finalize()
{
	//::InvalidateRect(m_hwnd, 0, false);
	if( !CInteractiveAction::Finalize() )
		return false;

	return true;
}

HRESULT CErectOneChromOnOff::GetActionState( DWORD *pdwState )
{
	HRESULT r=CInteractiveAction::GetActionState(pdwState);
	if(!IsAvaible()) *pdwState &= ~afEnabled;
	return(r);
}

bool CErectOneChromOnOff::IsAvaible()
{
	//if( !( IsActiveObjectListWasBuild( m_ptrTarget ) && IsActiveObjectListHasChromoObject( m_ptrTarget ) ) )
	if( !IsActiveObjectListHasChromoObject( m_ptrTarget ) )
		return false;

	ICarioViewPtr ptrView( m_ptrTarget );
	if( ptrView == 0 )
		return false;

	if( ptrView->IsEmpty() == S_OK ) return false; // ни одной хромосомы

	CarioViewMode cvm = cvmPreview;
	ptrView->GetMode( &cvm );

	if( !( cvm == cvmPreview || cvm == cvmCellar ) )
		return false;

	BOOL	fShow = true;
	ICarioView2Ptr sptrV = ptrView;
	sptrV->GetShowAllChromoObject( &fShow );

	if( !fShow )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CErectOneChromOnOff::DoLButtonDown( _point pt )
{
	if( activate_chromosome( pt ) )
	{
		Finalize();
		//GetWindowFromUnknown( m_punkTarget )->UpdateWindow();
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CErectOneChromOnOff::activate_chromosome( _point  pt )
{
	ICarioViewPtr	ptrView( m_ptrTarget );
	if( ptrView == 0 )return false;

	if( ptrView->IsEmpty() == S_OK ) return false; // ни одной хромосомы

	int	line, cell, pos;
	ptrView->GetCellByPoint( pt, &line, &cell, &pos, FALSE );

	if( pos == -1 )
		return false;

	ptrView->SetActiveChromo( line, cell, pos );

	return true;
}

HRESULT CErectOneChromOnOff::DoInvoke()
{
	ICarioView2Ptr ptrView( m_ptrTarget );
	if( ptrView == 0 )
		return false;

	IUnknown* punkObj = 0;
	IDataContextPtr sptrContext( m_ptrTarget );
	if(sptrContext!=0)
	{
		sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	}
	INamedDataObject2Ptr ptrList(punkObj);
	if(punkObj) punkObj->Release();

	int nLine	= 0;
	int nCell	= 0;
	int nChromo	= 0;
	if( S_OK != ptrView->GetActiveChromo( &nLine, &nCell, &nChromo ) )
		return false;

	long lNewState = GetArgLong("NewState");

	bool bErected=false;
	ptrView->GetErectedState(nLine, nCell, nChromo, &bErected);	
	if(lNewState==0) bErected=false;
	else if(lNewState==1) bErected=true;
	else bErected=!bErected;

	ptrView->SetErectedState(nLine, nCell, nChromo, bErected);	// изменим состояние

	if( nChromo < 0 )
		return false;

	IUnknown* punk = 0;
	ptrView->GetChromoObject( nLine, nCell, nChromo, &punk );
	
	if( !punk )	return false;
	INamedDataObject2Ptr ptrNDOChromo(punk);
	punk->Release();
	punk = 0;
	if(ptrNDOChromo==0) return false;

	IUnknown* punk2 = 0;
	ptrNDOChromo->GetParent(&punk2);
	if( !punk2 )	return false;
	IMeasureObjectPtr object(punk2);
	punk2->Release();
	punk2 = 0;
	if(object==0) return false;

	INamedDataObject2Ptr	ptrObjectNDO(object);
	if(ptrObjectNDO!=0)
	{
		long	lpos = 0;
		ptrObjectNDO->GetObjectPosInParent( &lpos ); 
		ptrList->SetActiveChild( lpos );
	}

	//получим подобъект-хромосому
	IChromosomePtr ptrC = IChromosomePtr(find_child_by_interface(object, IID_IChromosome));
	if(ptrC==0) return false;

	IUnknownPtr ptrUnkErector(LPOLESTR(_bstr_t(pszErectCCProgID)));
	IChromoConvertPtr ptrErector=IChromoConvertPtr(ptrUnkErector);
	if(ptrErector==0) return false;

/*** Max for editable idio ********************************************************/
	{
		INamedDataObject2Ptr ptrNDO = object;
		
		if( ptrNDO != 0 )
		{
			long lPosCromo = 0;
			
			IIdiogramPtr sptrEditable;

			ptrNDO->GetFirstChildPosition( &lPosCromo );
			while( lPosCromo )
			{
				IUnknown* punkChromo = 0;
				ptrNDO->GetNextChild( &lPosCromo, &punkChromo );
				if( !punkChromo )
					continue;

				IIdiogramPtr ptr_idio = punkChromo;

				if( ptr_idio != 0 )
				{
					unsigned uFlag = -1;
					ptr_idio->get_flags( &uFlag );

					if( uFlag == IDIT_EDITABLE )
					{
						sptrEditable = ptr_idio;
						break;
					}
				}
			}

			if( sptrEditable != 0 )
			{
				IIdiogramPtr ptr_idio = sptrEditable;
				ptr_idio->set_params( -1, -1 );
			}
		}
	}
/*** Max for editable idio ********************************************************/

	//для undo
	//ptrC->AddRef(); m_undoChrom.insert(ptrC);

	//IChromoConvert *pChromConv2;
	//ptrC->GetChromoConvert(&pChromConv2);
	//m_undoChromConv.insert(pChromConv2);

	//int cnt=GetUnkCounter(pChromConv2);

	IChromoConvert *pCCOld=0;
	ptrC->GetChromoConvert(&pCCOld);
	if(0==pCCOld)
	{ // если не было - создадим
		ptrC->SetChromoConvert(ptrErector);
		ptrErector->Recalc(ptrC);
	}
	else
	{  // если было - прибьем
		//ptrC->SetChromoConvert(0);
	}
	if(0!=pCCOld) pCCOld->Release();
	
	{ //просигналим, чтоб обновить вьюху
		INamedDataObject2Ptr NDO(ptrList);
		if(NDO!=0)
		{
			IUnknown *punk = NULL;
			HRESULT hr = NDO->GetData(&punk);
			if (SUCCEEDED(hr) && punk)
			{
				long l = cncReset;
				::FireEvent(punk, szEventChangeObjectList, ptrList, NULL, &l, 0);
				punk->Release();
			}
			}
	}

	return S_OK;
}

/////////////////////////////////

//собственно, главная функция - выпрямляет один объект.
//если с ним что-то не так (например, нет оси) - возвращает 0
bool CErectChromFilter::ErectOneChrom(IMeasureObjectPtr chrom)
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
		dlen+=_hypot(daxis0[i].x-daxis0[i-1].x,daxis0[i].y-daxis0[i-1].y);
	}

	long len=int(dlen);
	smart_alloc(daxis, _dpoint, len);
	smart_alloc(daxisvect, _dpoint,	len);

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
		dlen+=_hypot(daxis0[i].x-daxis0[i-1].x,daxis0[i].y-daxis0[i-1].y);
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
			double d=_hypot(dx,dy); d=max(d,1e-5);
			daxisvect[i].x=dx/d; daxisvect[i].y=dy/d;
		}
	}

	//получим image объекта
	IUnknownPtr sptr2;
	chrom->GetImage(&sptr2);
	IImage3Ptr image(sptr2);
	if(image==0) return 0;

	chrom->AddRef(); m_undoO.insert(chrom);
	ptrC->AddRef(); m_undoC.insert(ptrC);
	image->AddRef(); m_undoI.insert(image);

	_point offset;
	image->GetOffset(&offset);
	int cx,cy;
	image->GetSize(&cx,&cy);

	_bstr_t	bstrCC;
	int nPanes;
	{
		IUnknown *punkCC = 0;
		image->GetColorConvertor( &punkCC );
		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		nPanes = ::GetImagePaneCount( image );

		BSTR	bstr;
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

	//установим центральную линию
	int lNewAxisSize=max((len-1)/5+1,2);
	smart_alloc(new_axis,_point,lNewAxisSize);
	{
		for(int i=0; i<lNewAxisSize; i++)
		{
			int y=i*(len-1)/(lNewAxisSize-1);
			new_axis[i]=_point(width2,y);
		}
	}
	ptrC->SetAxis(AT_ShortAxis, lNewAxisSize, new_axis, true);

	// Установим центромеру
	_point ptCen;
	_point ptDraw[2];
	ptrC->GetCentromereCoord(0, &ptCen, ptDraw, &bIsManual);
	{
		int i_best=0;
		double r_best=1e20;
		for(int i=0; i<len; i++)
		{
			double r=_hypot(ptCen.x-daxis[i].x,ptCen.y-daxis[i].y);
			if(r<r_best)
			{
				i_best=i;
				r_best=r;
			}
		}
		ptCen=_point(width2,i_best);
		double r0=_hypot(ptDraw[0].x-daxis[i_best].x, ptDraw[0].y-daxis[i_best].y);
		ptDraw[0]=_point(width2-int(r0),i_best);
		double r1=_hypot(ptDraw[1].x-daxis[i_best].x, ptDraw[1].y-daxis[i_best].y);
		ptDraw[1]=_point(width2+int(r1),i_best);

	}
	ptrC->SetCentromereCoord(0, ptCen, ptDraw, false);

	
	IUnknown *punk2 = ::CreateTypedObject(_bstr_t(szTypeImage));
	IImage3Ptr imgOut(punk2);
	punk2->Release();

	imgOut->CreateImage(width2*2+1,len,bstrCC, ::GetImagePaneCount( image ) );
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
					int xx=int(xd);	xx=max(min(xx,cx-2),0);
					int yy=int(yd);	yy=max(min(yy,cy-2),0);
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
					int xx=int(xd);	xx=max(min(xx,cx-2),0);
					int yy=int(yd);	yy=max(min(yy,cy-2),0);
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
	//IUnknown *punk = ::CreateTypedObject(_bstr_t(szTypeObject));
	//IMeasureObjectPtr objOut(punk );
	//punk->Release();
	
	chrom->SetImage( imgOut );

	return true;
}

IUnknownPtr CErectChromFilter::get_document()
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

HRESULT CErectChromFilter::DoInvoke()
{
	IUnknownPtr ptrDoc = get_document();
	OnAttachTarget( ptrDoc );

	IUnknown* punkObj = 0;
	//IDataContextPtr sptrContext( m_ptrTarget );
	//sptrContext->GetActiveObject(_bstr_t(szTypeObjectList), &punkObj);
	punkObj = ::GetActiveObjectFromDocument(m_ptrTarget, szTypeObjectList);
	INamedDataObject2Ptr ptrList(punkObj);
	if(punkObj) punkObj->Release();
	if(ptrList==0) return E_FAIL;

	long nCountIn; ptrList->GetChildsCount(&nCountIn);
	if(!nCountIn) return false;

	m_undoO.deinit(); //очистим списки undo
	m_undoI.deinit(); //очистим списки undo
	m_undoC.deinit(); //очистим списки undo

	int nN=0;
	StartNotification(nCountIn);
	
	//CopyParams(objectsIn, objectsOut);
	
	long pos; ptrList->GetFirstChildPosition(&pos);
	while( pos ) //по всем объектам
	{
		nN++;
		IUnknownPtr sptr;
		ptrList->GetNextChild(&pos, &sptr);
		IMeasureObjectPtr object(sptr);
		if(object==0) continue;

		ErectOneChrom(object);

		Notify(nN);
	}
	
	FinishNotification();

	{
		INamedDataObject2Ptr NDO(ptrList);
		IUnknown *punk = NULL;
		HRESULT hr = NDO->GetData(&punk);
		if (SUCCEEDED(hr) && punk)
		{
			long l = cncReset;
			::FireEvent(punk, szEventChangeObjectList, ptrList, NULL, &l, 0);
			punk->Release();
		}
	}
	
	return S_OK;
}

IUnknown* CErectChromFilter::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	if( iid == IID_ILongOperation )return (ILongOperation*)this;
	return CAction::DoGetInterface( iid );
}

HRESULT CErectChromFilter::DoUndo()
{
	return E_FAIL;
	long	posO = m_undoO.head();
	long	posI = m_undoI.head();
	long	posC = m_undoC.head();
	while( posO && posI && posC ) 
	{
		IUnknown *punkO=m_undoO.next(posO);
		IUnknown *punkI=m_undoI.next(posI);
		IUnknown *punkC=m_undoC.next(posC);
		IMeasureObjectPtr ptrO(punkO);
		IImage3Ptr ptrI(punkI);
		IChromosomePtr ptrC(punkC);

		IUnknown *punkI2;
		ptrO->GetImage(&punkI2);
		IUnknown *punkC2=find_child_by_interface(ptrO, IID_IChromosome);

		//заменим изображение
		ptrO->SetImage(punkI);
		punkI->Release();

		//заменим ось
		sptrINamedDataObject2 sptrNO2 = punkC2;
		sptrNO2->SetParent(0, 0); // теперь объект хромосомы ничейный
		sptrINamedDataObject2 sptrNO = punkC;
		sptrNO->SetParent(ptrO, 0); //а из undo buffer - в объект
		punkC->Release();
	}
	return S_OK;
}

HRESULT CErectChromFilter::DoRedo()
{
	return E_FAIL;
	long	posO = m_undoO.head();
	long	posI = m_undoI.head();
	long	posC = m_undoC.head();
	while( posO && posI && posC ) 
	{
		IUnknown *punkO=m_undoO.next(posO);
		IUnknown *punkI=m_undoI.next(posI);
		IUnknown *punkC=m_undoC.next(posC);
		IMeasureObjectPtr ptrO(punkO);
		IImage3Ptr ptrI(punkI);
		IChromosomePtr ptrC(punkC);

		IUnknown *punkI2;
		ptrO->GetImage(&punkI2);
		IUnknown *punkC2=find_child_by_interface(ptrO, IID_IChromosome);

		//заменим изображение
		ptrO->SetImage(punkI);
		punkI->Release();

		//заменим ось
		sptrINamedDataObject2 sptrNO2 = punkC2;
		sptrNO2->SetParent(0, 0); // теперь объект хромосомы ничейный
		sptrINamedDataObject2 sptrNO = punkC;
		sptrNO->SetParent(ptrO, 0); //а из undo buffer - в объект
		punkC->Release();
	}
	return S_OK;
}

CErectChromFilter::CErectChromFilter()
{
}

CErectChromFilter::~CErectChromFilter()
{
}
