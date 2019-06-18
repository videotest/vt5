#include "stdafx.h"
#include "actionbase.h"
#include "actionobjectbase.h"
#include "Utils.h"
#include "resource.h"
#include "\vt5\common2\class_utils.h"

static void root_parent(IUnknown* prince, IUnknown* beggar)
{
	INamedDataObject2Ptr ndo1(prince);
	IUnknown* unk;
	ndo1->GetParent(&unk);

	INamedDataObject2Ptr ndo2(beggar);
	ndo2->SetParent(unk, 0);

	if(unk) unk->Release();

}

static bool check_group_for_useless(IMeasParamGroupPtr& group)
{

	long	lposP = 0;
 	group->GetFirstPos( &lposP );
	while( lposP )
	{
		ParameterDescriptor	*pdescr;
		group->GetNextParam( &lposP, &pdescr );
		if( pdescr->lEnabled ) return true;			
	}
	return false;
}


bool CActionObjectBase::SetObjectRemoveDelay(bool bDelay)
{
	IUnknown* o_list = GetActiveList();
	if(o_list == NULL) return false;
	ICompositeObjectPtr comp(o_list);
	if(o_list) o_list->Release();
	long bC;
	comp->IsComposite(&bC);
	if(bC) 
	{
			long d = bDelay ? 1:0;
			comp->DelayRemove(d, 0);
			return true;
	}
	return false;
}

BEGIN_INTERFACE_MAP(CActionObjectBase, CInteractiveActionBase)
	INTERFACE_PART(CActionObjectBase, IID_IObjectAction, DrAction)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CActionObjectBase, DrAction)

HRESULT CActionObjectBase::XDrAction::GetFirstObjectPosition( long *plpos )
{
	METHOD_PROLOGUE_EX(CActionObjectBase, DrAction);
	(*plpos) = (long)pThis->m_listToDocument.GetHeadPosition();
	return S_OK;
}
HRESULT CActionObjectBase::XDrAction::GetNextObject( IUnknown **ppunkObject, long *plpos )
{
	METHOD_PROLOGUE_EX(CActionObjectBase, DrAction);

	POSITION	pos = (POSITION)*plpos;
	*ppunkObject = (IUnknown*)pThis->m_listToDocument.GetNext( pos );
	*plpos = (long)pos;
	if( *ppunkObject )(*ppunkObject)->AddRef();

	return S_OK;
}

HRESULT CActionObjectBase::XDrAction::AddObject( IUnknown *punkObject )
{
	METHOD_PROLOGUE_EX(CActionObjectBase, DrAction);
	pThis->m_listToDocument.AddTail( punkObject );
	punkObject->AddRef();
	pThis->m_state = asReady;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CActionObjectBase
CActionObjectBase::CActionObjectBase()
{
	m_nOldSelectionType = 0;
	m_punkDocument = 0;
	m_hcurActive = 0;
	m_nDefineMode = dmUndef;
	m_bNoProcessOperations = false;
	m_bObjectActivated = false;
	m_punkDocument = 0;		
	m_rectInterest = NORECT;
	m_hcurAdd = 0;
	m_hcurSub = 0;

	m_bPhased = false;

	m_binvoke_return = false;
}

CActionObjectBase::~CActionObjectBase()
{
	{
		POSITION	pos = m_listToDocument.GetHeadPosition();
		while( pos )
			((IUnknown*)m_listToDocument.GetNext( pos ) )->Release();
		m_listToDocument.RemoveAll();
	}

	if (m_punkDocument)
		m_punkDocument->Release();

	if( m_hcurSub )DestroyCursor( m_hcurSub );
	if( m_hcurAdd )DestroyCursor( m_hcurAdd );
}

bool CActionObjectBase::DoSetCursor( CPoint point )
{
	int	mode = m_nDefineMode;
	if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
		mode = dmAdd;
	if( ::GetAsyncKeyState( VK_MENU ) & 0x8000 )
		mode = dmSub;

	if( mode == dmAdd && m_hcurAdd != 0 )
	{		::SetCursor( m_hcurAdd );		return true;	}

	if( mode == dmSub && m_hcurSub != 0 )
	{		::SetCursor( m_hcurSub );		return true;	}

	return CInteractiveActionBase::DoSetCursor( point );
}

bool CActionObjectBase::IsCompatibleTarget()
{
	DWORD dwFlags = 0;
	IImageViewPtr sptrIV(m_punkTarget);
	if(sptrIV != 0)sptrIV->GetImageShowFlags(&dwFlags);
	return ((dwFlags & isfFilledImageExceptObjects) == isfFilledImageExceptObjects) ||
         ((dwFlags & isfFilledOnlyObjects) == isfFilledOnlyObjects) ||
         ((dwFlags & isfContours) == isfContours) ||
         ((dwFlags & isfPseudoImage) == isfPseudoImage);
}


bool CActionObjectBase::DoRedo()
{
	bool b = SetObjectRemoveDelay(true);

	m_changes.DoRedo( m_punkTarget );

	if(b) SetObjectRemoveDelay(false);

	return true;
}

bool CActionObjectBase::DoUndo()
{	
	bool b = SetObjectRemoveDelay(true);

	m_changes.DoUndo( m_punkTarget );

	if(b) SetObjectRemoveDelay(false);

	return true;
}

//return the active image
IUnknown	*CActionObjectBase::GetActiveImage()
{
	sptrINamedDataObject2	sptrN;

	IUnknown	*punkImage = 0;


	if( CheckInterface( m_punkTarget, IID_IDocument ) )
		punkImage = ::GetActiveObjectFromDocument( m_punkTarget, szTypeImage );
	else
		punkImage = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );

	if( !punkImage )
		return 0;

	sptrN = punkImage;
	IUnknown	*punkParent = 0;
	sptrN->GetParent( &punkParent );

	if( punkParent )
	{
		punkImage->Release();
		return punkParent;
	}
	return punkImage;
}
//return the active object list
IUnknown	*CActionObjectBase::GetActiveList()
{
	if( CheckInterface( m_punkTarget, IID_IDocument ) )
		return ::GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );
	else
		return ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
}

bool CActionObjectBase::Invoke()
{	
	bool	bForceAdd = ( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )!=0;
	bool	bForceSub = ( ::GetAsyncKeyState( VK_MENU ) & 0x8000 )!=0;

	m_binvoke_return = false;
	if( bForceAdd )
		m_nDefineMode = dmAdd;
	if( bForceSub )
		m_nDefineMode = dmSub;

	if(m_bPhased) m_nDefineMode = dmNormal;

	ASSERT(m_nDefineMode!=dmUndef||m_listToDocument.GetHeadPosition() );
//re-assign the target key to the document
	sptrIView	sptrV( m_punkTarget );

	sptrV->GetDocument( &m_punkDocument );

	if( !m_punkDocument	)
	{
		SetError( IDS_ERR_NODOCUMENT );
		return false;
	}

	m_lTargetKey = ::GetObjectKey( m_punkDocument );
//init the m_objects variable

	IUnknown* o_list = GetActiveList();
	ICompositeObjectPtr comp(o_list);
	
	long bC = 0;
	if(comp) comp->IsComposite(&bC);
	if(bC)
	{
		long i = GetValueInt(GetAppUnknown(), "Composite", "EditorClassNum",0);
		IUnknown* unkLevel;
		comp->GetLevelbyClass(i,&unkLevel);
		m_objects.Attach( unkLevel, false );
		comp->DelayRemove(TRUE, unkLevel);
	}
	else m_objects.Attach( o_list, false );
	
	m_image.Attach( GetActiveImage(), false );

	BOOL bFlag = false;
	INamedDataObject2Ptr sptrImage = m_image;
	sptrImage->IsBaseObject(&bFlag);

	bool bSetListToDoc = false;

	if( m_objects == 0 )
	{
		//create an object list
		IUnknown	*punkObjects = ::CreateTypedObject( szTypeObjectList );

		if( !punkObjects )
		{
			SetError( IDS_ERR_CANTCREATEOBJECTLIST );
			return false;
		}

		if( bFlag )SetBaseObject( punkObjects, m_image, m_punkDocument );
		m_changes.SetToDocData( m_punkDocument, punkObjects );
		m_objects.Attach( punkObjects, false );

		//bSetListToDoc = true;
		
		_bstr_t	bstr( ::GetObjectKind( punkObjects ) );
		IDataContextPtr	ptrC( m_punkTarget );
		ptrC->SetActiveObject( bstr, punkObjects, aofActivateDepended );

		m_binvoke_return = true;
	}
	ASSERT( m_objects != 0 );
//init the m_image
	//IDataObjectListPtr	ptrOL = m_objects;
	//ptrOL->LockObjectUpdate( true );

	IDataObjectListPtr sptrList = m_objects;

	//de-activate all objects
	{
		INamedDataObject2Ptr	ptrN(m_objects);
		ptrN->SetActiveChild( 0 );
	}
	

	if( m_listToDocument.GetCount() == 0 )
	{
		if( !DoChangeObjects() )
		{
			//ptrOL->LockObjectUpdate( false );
			//ptrOL->UpdateObject( 0 );
			return false;
		}

		POSITION	posActive = m_objects.GetCurPosition();

		if( posActive )
		{
			POSITION	posTest = posActive;
			CObjectWrp	wrp( m_objects.GetNextObject( posTest ), false );
			CImageWrp	i( wrp.GetImage(), false );
			if( i.IsEmpty() )posActive = 0;
		}




		if( m_nDefineMode == dmAdd && /*posActive && */!m_bNoProcessOperations )
		{
			ProcessObjectList();
		}
		else
		if( m_nDefineMode == dmSub && /*posActive && */!m_bNoProcessOperations )
		{
			ProcessObjectListSub();
		}
		else
		if( m_nDefineMode == dmSep && /*posActive && */!m_bNoProcessOperations )
		{
			ProcessObjectListSub();		   // Maxim
			m_listToDocument.AddTail( &m_listAddedObjects );
			m_listAddedObjects.RemoveAll();
		}
		else
		if( m_nDefineMode == dmNormal /*|| !posActive */|| m_bNoProcessOperations )
		{
			//copy the one list to other
			m_listToDocument.AddTail( &m_listAddedObjects );
			m_listAddedObjects.RemoveAll();
		}
	}

	

	bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;
	
	if(bRecalc && m_bPhased) CalculatePhases();
	//Init the objects and place it to the document
	else
	{
		POSITION	pos = m_listToDocument.GetHeadPosition();

		bool bSetParams = true;

		while( pos )
		{
			IUnknown	*punk = (IUnknown*)m_listToDocument.GetNext( pos );

			if( CheckInterface( punk, IID_IMeasureObject ) )
			{
				CObjectWrp	object( punk );
				CImageWrp	image( object.GetImage(), false );

				if( image.GetWidth() < m_nMinSize &&
					image.GetHeight() < m_nMinSize )
					continue;

				CWalkFillInfo	info(image);
				CPoint pt(0,0);
				info.InitContours(false, CPoint( -1, -1 ), &pt, NORECT, image, 0, true);
				int nCounter = info.GetContoursCount();
				for(int i = 0; i < nCounter; i++)
				  image->AddContour(info.GetContour(i));

				info.DetachContours();
        
				//image.InitContours();
				//if(bRecalc)
				//{
				//	::CalcObject(object, image, true, 0 );
				//}

				sptrINamedDataObject2	sptrN( object );
				sptrN->SetParent( m_objects, 0 );
				//recalc
				if(bRecalc)
				{
					::CalcObject(object, image, true, 0 );					

				}

				
				bSetParams = false;
			}
			else
			{
				sptrINamedDataObject2	sptrN( punk );
				sptrN->SetParent( m_objects, 0 );
			}

			
			m_changes.SetToDocData( m_punkDocument, punk );
			m_binvoke_return = true;
			

		}
	}

	//empty the added object list 
	{
		POSITION	pos = m_listAddedObjects.GetHeadPosition();
		ASSERT(pos==0);
		while( pos )
			((IUnknown*)m_listAddedObjects.GetNext( pos ) )->Release();
		m_listAddedObjects.RemoveAll();
	}

	//empty the placed to document object list
	{
		POSITION	pos = m_listToDocument.GetHeadPosition();
		while( pos )
			((IUnknown*)m_listToDocument.GetNext( pos ) )->Release();
		m_listToDocument.RemoveAll();
	}


	/*if(bSetListToDoc)
		m_changes.SetToDocData( m_punkDocument, m_objects );*/

	SyncViewData sd;
	::ZeroMemory( &sd, sizeof(SyncViewData) );

	sd.dwStructSize				= sizeof(SyncViewData);
	sd.punkObject2Activate		= (IUnknown*)m_objects;
	sd.bCanChangeViewType		= false;
	sd.bSyncAllView				= false;
	sd.bOnlyViewInterfaceMatch	= true;
	memcpy( &(sd.iidViewMatch), &IID_IMeasureView, sizeof( IID ) );

	::SyncSplitterView( m_punkTarget, &sd );

	//ptrOL->LockObjectUpdate( false );
	//ptrOL->UpdateObject( 0 );

	if(comp) comp->DelayRemove(FALSE, m_objects);
	if(bC) o_list->Release();
	if (m_punkDocument)
		m_punkDocument->Release();
	m_punkDocument = 0;
	return m_binvoke_return;
}


IUnknown *CActionObjectBase::CreateMeasureObject()
{
	IUnknown	*punk = NULL;

	punk = CreateTypedObject( szTypeObject );
	ASSERT( punk );

	m_listAddedObjects.AddTail( punk );

	return punk;
}

void CActionObjectBase::DeleteMeasureObject( IUnknown *punkObject )
{
	m_changes.RemoveFromDocData( m_punkDocument, punkObject );
	m_binvoke_return = true;
}

bool CActionObjectBase::DoUpdateSettings()
{
	m_nDefineMode = (DefineMode)::GetValueInt( GetAppUnknown(), "ObjectEditor", "DefineMode", dmNormal );
	m_nMinSize = ::GetValueInt( GetAppUnknown(), "ObjectEditor", "MinSize", 5 );
	return true;
}

void CActionObjectBase::ProcessObjectList()
{
	POSITION	pos = m_objects.GetCurPosition();
	//CObjectWrp	object( m_objects.GetNextObject( pos ), false );
	//CImageWrp	image( object.GetImage(), false );
//calc the rect of interest
	CRect		rect;// = image.GetRect();
	bool		bFirst = true;

	pos = m_listAddedObjects.GetHeadPosition();
	while( pos )
	{
		CObjectWrp	object = (IUnknown*)m_listAddedObjects.GetNext( pos );
		CImageWrp	image( object.GetImage(), false );

		CRect	rectCur = image.GetRect();

		if( bFirst )
			rect = rectCur;
		bFirst = false;

		rect.left = min( rect.left, rectCur.left );
		rect.right = max( rect.right, rectCur.right );
		rect.top = min( rect.top, rectCur.top );
		rect.bottom = max( rect.bottom, rectCur.bottom );
	}
//create intersect object list
	{
		CWalkFillInfo	wfi_test( rect );
		pos = m_listAddedObjects.GetHeadPosition();
		while( pos )
		{
			CObjectWrp	object = (IUnknown*)m_listAddedObjects.GetNext( pos );
			_MapObjectMaskToInfo( object, wfi_test, dmAdd );
		}

		_CreateIntersectList( &wfi_test );
	}

	//create a WalkFillInfo
	CWalkFillInfo	wfi( m_rectInterest );
//map objects here
	pos = m_listIntersect.GetHeadPosition();
	while( pos )
	{
		CObjectWrp	object = (IUnknown*)m_listIntersect.GetNext( pos );
		_MapObjectMaskToInfo( object, wfi, dmAdd );
	}

	pos = m_listAddedObjects.GetHeadPosition();
	while( pos )
	{
		CObjectWrp	object = (IUnknown*)m_listAddedObjects.GetNext( pos );
		_MapObjectMaskToInfo( object, wfi, (m_nDefineMode==dmSub||m_nDefineMode==dmSep)?dmSub:dmAdd );
	}

	pos = m_listIntersect.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punk = (IUnknown*)m_listIntersect.GetNext( pos );
		DeleteMeasureObject( punk );
	}
//clear the added object list
	rect = wfi.CalcNoEmptyRect();

	if( rect == NORECT )
		return;

	CImageWrp	imageNew( m_image.CreateVImage( rect ), false );
	CObjectWrp objectNew( CreateTypedObject( szTypeObject ), false );
	objectNew.SetImage( imageNew );

	_AttachMaskToObject( objectNew, wfi );
	m_listToDocument.AddTail( objectNew.Detach() );

	/*CPoint	point( -1, -1 );

	IUnknown* punkImage = 0;
	
	while( wfi.InitContours(true, point, &point, rect, m_image, &punkImage) )
	{
		CImageWrp	image(punkImage, false);
		CRect	rectObject = wfi.CalcContoursRect();
		//_InitObject( pwfi, rectObject );

		CObjectWrp objectNew( CreateTypedObject( szTypeObject ), false );
		CImageWrp	imageNew( m_image.CreateVImage( rectObject ), false );
		objectNew.SetImage( imageNew );
		_AttachMaskToObject( objectNew, wfi );


		m_listToDocument.AddTail( objectNew.Detach() );

		wfi.ClearContours();
	}*/

	_DestroyIntersectList();
}

void CActionObjectBase::ProcessObjectListSub()
{
	POSITION	pos = m_objects.GetCurPosition();
	//CObjectWrp	object( m_objects.GetNextObject( pos ), false );
	//CImageWrp	image( object.GetImage(), false );
//calc the rect of interest
	CRect		rect;// = image.GetRect();
	bool		bFirst = true;

	pos = m_listAddedObjects.GetHeadPosition();
	while( pos )
	{
		CObjectWrp	object = (IUnknown*)m_listAddedObjects.GetNext( pos );
		CImageWrp	image( object.GetImage(), false );

		CRect	rectCur = image.GetRect();

		if( bFirst )
			rect = rectCur;
		bFirst = false;

		rect.left = min( rect.left, rectCur.left );
		rect.right = max( rect.right, rectCur.right );
		rect.top = min( rect.top, rectCur.top );
		rect.bottom = max( rect.bottom, rectCur.bottom );
	}
//create intersect object list
	{
		CWalkFillInfo	wfi_test( rect );
		pos = m_listAddedObjects.GetHeadPosition();
		while( pos )
		{
			CObjectWrp	object = (IUnknown*)m_listAddedObjects.GetNext( pos );
			_MapObjectMaskToInfo( object, wfi_test, dmAdd );
		}

		_CreateIntersectList( &wfi_test );
	}

//map objects here
	pos = m_listIntersect.GetHeadPosition();
	while( pos )
	{
		CObjectWrp	object = (IUnknown*)m_listIntersect.GetNext( pos );
		CImageWrp	image( object.GetImage(), false );
		CRect rcObj(image.GetRect());
		CWalkFillInfo	wfi( rcObj );
		_MapObjectMaskToInfo( object, wfi, dmAdd );
		POSITION pos1 = m_listAddedObjects.GetHeadPosition();
		while( pos1 )
		{
			CObjectWrp	object = (IUnknown*)m_listAddedObjects.GetNext( pos1 );
			_MapObjectMaskToInfo( object, wfi, dmSub);
		}
		CRect rect = wfi.CalcNoEmptyRect();
		if( rect == NORECT ) continue;
		CImageWrp	imageNew( m_image.CreateVImage( rect ), false );
		CObjectWrp objectNew( CreateTypedObject( szTypeObject ), false );
		objectNew.SetImage( imageNew );
		_AttachMaskToObject( objectNew, wfi );
		m_listToDocument.AddTail( objectNew.Detach() );
	}

	pos = m_listIntersect.GetHeadPosition();
	while( pos )
	{
		IUnknown	*punk = (IUnknown*)m_listIntersect.GetNext( pos );
		DeleteMeasureObject( punk );
	}

	_DestroyIntersectList();
}

void CActionObjectBase::_MapObjectMaskToInfo( CObjectWrp &object, CWalkFillInfo &info, DefineMode mode )
{
	CImageWrp	image( object.GetImage(), false );

	CRect	rectImage = image.GetRect();
	CRect	rectInfo = info.GetRect();

	int	x, y;

	for( y = max( rectImage.top, rectInfo.top ); y < min( rectImage.bottom, rectInfo.bottom ); y++ )
	{
		byte	*pbyteImageMask = image.GetRowMask( y-rectImage.top )
			+max( rectImage.left, rectInfo.left )-rectImage.left;
		byte	*pbyteInfoMask = info.GetRowMask( y-rectInfo.top )
			+max( rectImage.left, rectInfo.left )-rectInfo.left;

		for( x = max( rectImage.left, rectInfo.left ); x < min( rectImage.right, rectInfo.right ); x++, pbyteImageMask++, pbyteInfoMask++ )
		{
			if( mode == dmNormal ||
				mode == dmAdd )
				if( *pbyteImageMask == 255 )
					*pbyteInfoMask = 0xff;
			if( mode == dmSub )
				if( *pbyteImageMask == 255 )
					*pbyteInfoMask = 0x0;
		}
	}
}

void CActionObjectBase::_AttachMaskToObject( CObjectWrp &object, CWalkFillInfo &info )
{
	CImageWrp	image( object.GetImage(), false );
	image.InitRowMasks();

	CRect	rectImage = image.GetRect();
	CRect	rectInfo = info.GetRect();

	int	x, y;

	for( y = max( rectImage.top, rectInfo.top ); y < min( rectImage.bottom, rectInfo.bottom ); y++ )
	{
		byte	*pbyteImageMask = image.GetRowMask( y-rectImage.top )
			+max( rectImage.left, rectInfo.left )-rectImage.left;
		byte	*pbyteInfoMask = info.GetRowMask( y-rectInfo.top )
			+max( rectImage.left, rectInfo.left )-rectInfo.left;

		for( x = max( rectImage.left, rectInfo.left ); x < min( rectImage.right, rectInfo.right ); x++, pbyteImageMask++, pbyteInfoMask++ )
		{
			if( *pbyteInfoMask )
				*pbyteImageMask = 0xff;
			else
				*pbyteImageMask = 0;
		}
	}
}
void CActionObjectBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	__super::OnNotify(pszEvent, punkHit, punkFrom, pdata, cbSize );

	if(!strcmp(pszEvent, szEventBeforeActionExecute))
	{
		Finalize();
		Terminate();
	}
}
void CActionObjectBase::ActivateObject( CPoint point )
{
	if( m_bObjectActivated )
		return;
	//m_bObjectActivated = true;

	m_objects.Attach( GetActiveList(), false );

	POSITION	posStart = m_objects.GetCurPosition();

	if( !posStart )
		posStart = m_objects.GetFirstObjectPosition();

	if( !posStart )
		return;

	//skip one object				  7
	m_objects.GetNextObject( posStart )->Release();

	POSITION	pos = posStart;

	while( pos )
	{
		POSITION	posSave = pos;
		CObjectWrp	object( m_objects.GetNextObject( pos ), false );
		if( object.PtInObject( point ) )
		{
			m_objects.SetCurPosition( posSave );
			return;
		}
	}

	pos = m_objects.GetFirstObjectPosition();

	while( pos != posStart )
	{
		POSITION	posSave = pos;
		CObjectWrp	object( m_objects.GetNextObject( pos ), false );
		if( object.PtInObject( point ) )
		{
			m_objects.SetCurPosition( posSave );
			return;
		}
	}
}

bool CActionObjectBase::DoLButtonDown( CPoint pt )
{
	if( CanActivateObjectOnLbutton() )
		ActivateObject( pt );

	return false;
}
void	CActionObjectBase::update_phases_info(IUnknown* pList)
{
	INamedDataObject2Ptr ndo = pList;
	if(ndo)
	{
		long pos;
		CString strClassifier, strData ="";
		CString str = ::GetValueString( ::GetAppUnknown(), "\\Classes", "ClassFile", "" );	
		int nidx = str.ReverseFind( '\\' );
		if( nidx == -1 )
		strClassifier = "";

		int nlen = str.GetLength();
		if( nlen <= nidx )
		strClassifier = "";
		else
		strClassifier = str.Right( nlen - nidx - 1 );
		int nClass =-1;

		ndo->GetFirstChildPosition( &pos);
		while(pos)
		{
			CString s;
			IUnknown* pPhase;
			ndo->GetNextChild(&pos, &pPhase );
			nClass = get_object_class(ICalcObjectPtr(pPhase), strClassifier);
			s.Format("%i,",nClass);
			strData+=s;
			pPhase->Release();
		}
		if(strData.IsEmpty()) 
		{
			::SetValue(GetAppUnknown(),"Phases", "ActivePhases", "");
			m_nPhaseEdit =-1;
			return;
		}
		strData =strData.Left(strData.GetLength()-1);
		::SetValue(GetAppUnknown(),"Phases", "ActivePhases", strData);
		m_nPhaseEdit = GetValueInt( GetAppUnknown(), "ObjectEditor", "EditorClassNum", -1 );	
	}
}
bool CActionObjectBase::is_phased(IUnknown* unk)
{
	if(unk==0) 
	{
		m_bPhased = false;
		return false;
	}
	INamedDataPtr nd = unk;
	if(nd)
	{
		variant_t value;
		nd->GetValue(bstr_t("Phases"), &value);
		m_bPhased = (VT_EMPTY!=value.vt && value.iVal!= 0);
	}
	
	return m_bPhased;
}
bool CActionObjectBase::Initialize()
{
	bool bSwitchView = ::GetValueInt( GetAppUnknown(), "\\ObjectEditor", "SwitchView", 1L ) != 0;

	//view manipulation
	if(bSwitchView)
	{
		if( !IsCompatibleTarget())
			if(!SetAppropriateView(m_punkTarget, avtObject) || m_state == asTerminate )
				return false;
	}
	
	IUnknown* unk = this->GetActiveList();
	/*if(unk==0) return false;*/

	if(is_phased(unk))
	{
		m_objects.Attach(unk);	
		if(!m_objects.GetCount())
		{
			unk->Release();
			return false;
		}
		update_phases_info(unk);
		AddPropertyPageDescription( c_szPhaseEditorPropPage);		
	}

	
	ICompositeObjectPtr co(unk);
	if(unk) unk->Release();

	if(!m_bPhased && co!=0)
	{
		long bC;
		co->IsComposite(&bC);
		if(bC!=0)
		{
			AddPropertyPageDescription( c_szCompositeEditorPropPage);
		}
	}

	sptrIImageView	sptrIV( m_punkTarget );
	sptrIV->EnableControlFrame( fcfNone );
	//sptrIV->EnableControlFrame( fcfRotate|fcfResize );
	//trIV->GetSelectionMode( &m_nOldSelectionType );
	//trIV->SetSelectionMode( 1 );

	m_image.Attach( GetActiveImage(), false );

	LockMovement( true, m_image.GetRect() );

	if( !CInteractiveActionBase::Initialize() )return false;
	return true;
}

void CActionObjectBase::Finalize()
{
	_Draw();

	sptrIImageView	sptrIV( m_punkTarget );
	if (::GetValueInt(::GetAppUnknown(), "ImageView", "EnableObjectResize", 1))
		sptrIV->EnableControlFrame( fcfRotate|fcfResize );
	else 
		sptrIV->EnableControlFrame( fcfNone );
	///sptrIV->SetSelectionMode( m_nOldSelectionType );
	CInteractiveActionBase::Finalize();
}

bool CActionObjectBase::IsAvaible()
{
	if (CheckInterface( m_punkTarget, IID_IImageView ))
	{
		IUnknown* punk_img = ::GetActiveObjectFromContext( m_punkTarget, szTypeImage );
		if( punk_img )
		{
			punk_img->Release();
			return true;
		}
	}
	return false;
}


bool CActionObjectBase::CanActivateObjectOnLbutton()
{
	return ( ::GetValueInt( GetAppUnknown(), "\\ObjectEditor", "CanActivateObjectOnLbutton", 0L ) == 1L );
}


void CActionObjectBase::_CreateIntersectList( CWalkFillInfo *pwfi )
{
	int	x, y;

	CRect	rectArea = pwfi->CalcNoEmptyRect();
	CRect	rect = pwfi->GetRect();
	CPoint	ptAreaOffset( rectArea.left - rect.left, rectArea.top - rect.top );

	m_rectInterest =rectArea;

	POSITION	pos = m_objects.GetFirstObjectPosition();

	while( pos )
	{
		POSITION	posSave = pos;
		CObjectWrp	object(m_objects.GetNextObject( pos ), false );

		CImageWrp	image( object.GetImage(), false );

		CRect	rectTest = image.GetRect();

		if( rectTest.right < rectArea.left ||
			rectTest.left > rectArea.right ||
			rectTest.bottom < rectArea.top ||
			rectTest.top > rectArea.bottom )
			continue;

		bool	bIntersect = false;

		for( y = max( rectTest.top, rectArea.top ); y < min( rectTest.bottom, rectArea.bottom ); y++ )
		{
			byte	*pbyteTest = image.GetRowMask( y-rectTest.top )+max( rectTest.left, rectArea.left )-rectTest.left;
			byte	*pbyteArea = pwfi->GetRowMask( y-rectArea.top+ptAreaOffset.y )+max( rectTest.left, rectArea.left )-rectArea.left+ptAreaOffset.x;

			for( x = max( rectTest.left, rectArea.left ); x < min( rectTest.right, rectArea.right ); x++ )
			{
				if( *pbyteTest && *pbyteArea )
					bIntersect = true;
				pbyteTest++;
				pbyteArea++;
			}

			if( bIntersect )
				break;
		}

		if( !bIntersect )
			continue;
		m_listIntersect.AddTail( object.Detach() );
		m_rectInterest.left = min( rectTest.left, m_rectInterest.left );
		m_rectInterest.top = min( rectTest.top, m_rectInterest.top );
		m_rectInterest.right = max( rectTest.right, m_rectInterest.right );
		m_rectInterest.bottom = max( rectTest.bottom, m_rectInterest.bottom );


	}
}


void CActionObjectBase::_DestroyIntersectList()
{
	POSITION	pos = m_listIntersect.GetHeadPosition();

	while( pos )
	{
		IUnknown	*punk = (IUnknown*)m_listIntersect.GetNext( pos );
		punk->Release();
	}
	m_listIntersect.RemoveAll();
}

BOOL CActionObjectBase::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if(message==WM_SYSKEYDOWN && wParam==VK_MENU)
	{
		int	mode = m_nDefineMode;
		if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) mode = dmAdd;
		if( ::GetAsyncKeyState( VK_MENU ) & 0x8000 )  mode = dmSub;

		if( mode == dmAdd && m_hcurAdd != 0 ) ::SetCursor( m_hcurAdd );
		if( mode == dmSub && m_hcurSub != 0 ) ::SetCursor( m_hcurSub );
	}
	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CActionObjectBase::_activate_object( CPoint pt )
{
	if( !m_punkTarget )
		return;

	IUnknown *ptrDocList = ::GetActiveObjectFromContext( m_punkTarget, szTypeObjectList );
	bool bFounded = false;

	if( !ptrDocList )
		return;

	INamedDataObject2Ptr	ptrListObject = ptrDocList;
	ptrDocList->Release();


	long posStart = 0, lPrevPos = 0;
	
	ptrListObject->GetFirstChildPosition( &posStart );

	while( posStart )
	{
		IUnknown *punkObject = 0;
		lPrevPos = posStart;
		ptrListObject->GetNextChild( &posStart, &punkObject );

		if( punkObject )
		{
			IMeasureObjectPtr pObject = punkObject;
			punkObject->Release();

			if( pObject == 0 )
				continue;
		
			IUnknown *pImage = 0;
			pObject->GetImage( &pImage );

			if( !pImage )
				continue;

			IImage3Ptr ptrImage = pImage;
			pImage->Release();

			RECT rcImage;
			_get_img_rect( pImage , rcImage );

			if( ::PtInRect( &rcImage, pt ) )
			{
				BYTE *pByte = 0;
				ptrImage->GetRowMask( pt.y - rcImage.top, &pByte );

				if( pByte[pt.x - rcImage.left] )
				{
					ptrListObject->SetActiveChild( lPrevPos );
					return;
				}
			}
		}
	}
}

void CActionObjectBase::_get_img_rect(IUnknown *pImage,RECT &rc)
{
	IImage3Ptr ptrImage = pImage;

	POINT ptImgOffset;
	ptrImage->GetOffset( &ptImgOffset );

	int w = 0,h = 0;
	ptrImage->GetSize( &w,&h );

	rc.left = ptImgOffset.x;
	rc.top = ptImgOffset.y;

	rc.right = rc.left + w;
	rc.bottom = rc.top + h;
}

bool CActionObjectBase::CalculatePhases()
{
	CPtrList usefullGroups;
	IUnknown *punk = FindComponentByName(GetAppUnknown(false), IID_IMeasureManager, szMeasurement);
	ICompManagerPtr ptrManager = punk ;
	if( punk )punk->Release();
	if(ptrManager ==0) return false;
	int	nGroup, nGroupCount;
	ptrManager->GetCount( &nGroupCount );
	for( nGroup = 0; nGroup < nGroupCount; nGroup++ )
	{
		IUnknown*	unkGroup;
		ptrManager->GetComponentUnknownByIdx( nGroup, &unkGroup );
		IMeasParamGroupPtr	ptrGroup( unkGroup );
		if(unkGroup) unkGroup->Release();

		if( ptrGroup == 0 )continue;
		if(check_group_for_useless(ptrGroup))//? - содержится ли в группе хотя бы один "Enabled" parameter
		{
			usefullGroups.AddTail(ptrGroup.Detach());
		}
	}

	bool bContured =false;
	{
		POSITION	pos = m_listToDocument.GetHeadPosition();

		bool bSetParams = true;

		while( pos )
		{
			IUnknown	*punk = (IUnknown*)m_listToDocument.GetNext( pos );
			
			if( CheckInterface( punk, IID_IMeasureObject ) )
			{
				CObjectWrp	object( punk );
				CImageWrp	image( object.GetImage(), false );

				if( image.GetWidth() < m_nMinSize &&
					image.GetHeight() < m_nMinSize )
					continue;
				
				if(image.GetContoursCount())
				{
					bContured =true;
					image->FreeContours();
				}

				if(bContured)
				{
					CWalkFillInfo	info(image);
					CPoint pt(0,0);
					info.InitContours(false, CPoint( -1, -1 ), &pt, NORECT, image, 0, true);
					int nCounter = info.GetContoursCount();
					for(int i = 0; i < nCounter; i++)
						image->AddContour(info.GetContour(i));

					info.DetachContours();
				}



				//recalc
				IMeasParamGroup* mg =0;
				{

					POSITION pos;
					pos = usefullGroups.GetHeadPosition();
					while(pos)
					{
						IMeasParamGroupPtr	ptrG;
						mg = (IMeasParamGroup*)usefullGroups.GetNext(pos);
						ptrG = mg;
						if( ptrG == 0 )
							continue;

						ptrG->InitializeCalculation( m_objects );

						ptrG->CalcValues( punk, image );

						ptrG->FinalizeCalculation();
					}
				}

				sptrINamedDataObject2	sptrN( object );
				sptrN->SetParent( m_objects, 0 );
			}
			else
			{
				sptrINamedDataObject2	sptrN( punk );
				sptrN->SetParent( m_objects, 0 );
			}
			
			m_changes.SetToDocData( m_punkDocument, punk );
			m_binvoke_return = true;

		}

		pos = usefullGroups.GetHeadPosition();
		
		while(pos)
		{
			IUnknown *unk = (IUnknown*)usefullGroups.GetNext(pos);
			unk->Release();
		}
	}


	return true;
}

bool CActionObjectBase::DoChangePhases(CWalkFillInfo* pWFI, bool bClear/* = false*/)
{
	if( !pWFI )
		return false;

	//adjust wfi

	CRect	rect = pWFI->GetRect();
	CRect	rectCalc = pWFI->CalcNoEmptyRect();

	int	x, y;

	for( x = rect.left-1; x < rect.right+1; x++ )
	{
		pWFI->SetPixel( x, rect.top-1, 0 );
		pWFI->SetPixel( x, rect.bottom, 0 );
	}

	for( y = rect.top-1; y < rect.bottom+1; y++ )
	{
		pWFI->SetPixel( rect.left-1, y, 0 );
		pWFI->SetPixel( rect.right, y, 0 );
	}


	CString strClassifier = ::GetValueString(GetAppUnknown(), "Classes", "ClassFile", "");
	m_nPhaseEdit = ::GetValueInt( GetAppUnknown(), "ObjectEditor", "EditorClassNum", -2 );
	//COLORREF col;
	//col = get_class_color(m_nPhaseEdit, strClassifier.GetBuffer());

	int  nCount= m_objects.GetCount();
	if(!nCount) return false;
	OBJINF* pObjects = new OBJINF[nCount];
	int i=1;
	POSITION	pos = m_objects.GetFirstObjectPosition();
	while( pos )
	{

		IUnknown* pUnk = m_objects.GetNextObject( pos );
		if(pUnk==0) continue;
		ICalcObjectPtr calcPtr = pUnk;
		int nClass = get_object_class( calcPtr, strClassifier);
		
		if( nClass == m_nPhaseEdit) 
		{
			
			pObjects[0].objectIn.Attach(pUnk);
			pObjects[0].objectOut.Attach(CloneObject(pUnk));
			IUnknown* unkTemp1 =  pObjects[0].objectIn.GetImage();
			pObjects[0].imageIn.Attach(unkTemp1 , false);
			IUnknown* unkTemp2 =  pObjects[0].objectOut.GetImage();
			pObjects[0].imageOut.Attach(unkTemp2, false);
			root_parent(unkTemp1, unkTemp2);
			pObjects[0].bChanged = false;
		}
		else
		{
			pObjects[i].objectIn.Attach(pUnk);
			pObjects[i].objectOut.Attach(CloneObject(pUnk));
			IUnknown* unkTemp1 =  pObjects[i].objectIn.GetImage();
			pObjects[i].imageIn.Attach(unkTemp1 , false);
			IUnknown* unkTemp2 =  pObjects[i].objectOut.GetImage();
			pObjects[i].imageOut.Attach(unkTemp2, false);
			root_parent(unkTemp1, unkTemp2);
			pObjects[i++].bChanged = false;

		}
		pUnk->Release();
	}
	if(pObjects[0].objectIn == 0)
	{
		delete []pObjects;
		return false;
	}


	int cx = pObjects[0].imageIn.GetWidth();
	int cy = pObjects[0].imageIn.GetHeight();

	/*CRect	rectTest = image.GetRect();*/	

	byte* src;
	byte* dst;
	byte* out;
	

	if(bClear)
	{
		for(int y = rectCalc.top ; y<rectCalc.bottom; y++)
		{
			dst = pWFI->GetRowMask(y);
			src = pObjects[0].imageIn.GetRowMask(y);
			out = pObjects[0].imageOut.GetRowMask(y);
			for(int x = rectCalc.left;x<rectCalc.right;x++)
			{
				out[x] = (src[x]!=0 && dst[x]==0)? src[x]:0;
				pObjects[0].bChanged = pObjects[0].bChanged ? true:(out[x]!=src[x]);
			}

		}
	}
	else
	{
		byte mark =0;
		for(int y = 0 ; y<cy; y++)
		{
			src = pObjects[0].imageIn.GetRowMask(y);
			for(int x = 0;x<cx;x++)
			{
				if(src[x]>=128) mark=src[x];
				break;
			}
			if(mark) break;
		}
		if(mark==0) mark =255;
		int nChanges =0;
		for(int y = rectCalc.top ; y<rectCalc.bottom; y++)
		{
			dst = pWFI->GetRowMask(y);
			for(int i =1;i<nCount;i++)
			{
				src = pObjects[i].imageIn.GetRowMask(y);
				out = pObjects[i].imageOut.GetRowMask(y);

				for(int x = rectCalc.left;x<rectCalc.right;x++)
				{
					out[x] = (dst[x]!=255)? src[x]:0;
					pObjects[i].bChanged = pObjects[i].bChanged ? true:(out[x]!=src[x]);
				}				
			}
			src = pObjects[0].imageIn.GetRowMask(y);
			out = pObjects[0].imageOut.GetRowMask(y);
			for(int x = rectCalc.left;x<rectCalc.right;x++)
			{
				out[x] = (src[x] || dst[x])? mark:0;
				pObjects[0].bChanged = pObjects[0].bChanged ? true:(out[x]!=src[x]);
			}
			
		}
	}

	for(int i=0;i<nCount;i++)
	{
       if(pObjects[i].bChanged)
		{
			m_listAddedObjects.AddTail(pObjects[i].objectOut);
			DeleteMeasureObject(pObjects[i].objectIn);
		}
	   else pObjects[i].objectOut->Release();
	}

	delete []pObjects;


//invalidate the rect	
	/*CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
	rect = ConvertToWindow( m_punkTarget, rectCalc );
	pwnd->InvalidateRect( rect );*/



	return true;
}