#include "stdafx.h"
#include "resource.h"

#include "objectexchange.h"
#include "utils.h"



IMPLEMENT_DYNCREATE(CActionMapObjects2Image, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMakeObjectsVirtual, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMakeObjectsNonVirtual, CCmdTargetEx);

// {F9D27605-7467-498c-9B6F-1C65BA02A8A0}
GUARD_IMPLEMENT_OLECREATE(CActionMapObjects2Image, "Objects.MapObjects2Image", 
0xf9d27605, 0x7467, 0x498c, 0x9b, 0x6f, 0x1c, 0x65, 0xba, 0x2, 0xa8, 0xa0);

// {E6D8DDC2-D51C-4681-9762-185CB0E880BD}
GUARD_IMPLEMENT_OLECREATE(CActionMakeObjectsVirtual, "Objects.MakeObjectsVirtual", 
0xe6d8ddc2, 0xd51c, 0x4681, 0x97, 0x62, 0x18, 0x5c, 0xb0, 0xe8, 0x80, 0xbd);


// {F6019F6D-EC0B-48a9-A805-4ED252885AA9}
GUARD_IMPLEMENT_OLECREATE(CActionMakeObjectsNonVirtual, "Objects.MakeObjectsNonVirtual", 
0xf6019f6d, 0xec0b, 0x48a9, 0xa8, 0x5, 0x4e, 0xd2, 0x52, 0x88, 0x5a, 0xa9);


// {F2EF910F-8553-436b-9EB5-C6F7245413E4}
static const GUID guidMapObjects2Image = 
{ 0xf2ef910f, 0x8553, 0x436b, { 0x9e, 0xb5, 0xc6, 0xf7, 0x24, 0x54, 0x13, 0xe4 } };

// {163A91A4-4A21-40e1-95B4-2080E308DD7A}
static const GUID guidMakeObjectsVirtual = 
{ 0x163a91a4, 0x4a21, 0x40e1, { 0x95, 0xb4, 0x20, 0x80, 0xe3, 0x8, 0xdd, 0x7a } };

// {92B8A3EA-6589-41b9-8235-B78109C60B69}
static const GUID guidMakeObjectsNonVirtual = 
{ 0x92b8a3ea, 0x6589, 0x41b9, { 0x82, 0x35, 0xb7, 0x81, 0x9, 0xc6, 0xb, 0x69 } };


ACTION_INFO_FULL(CActionMapObjects2Image, IDS_ACTION_MAP_OBJECTS_TO_IMAGE, -1, -1, guidMapObjects2Image);
ACTION_INFO_FULL(CActionMakeObjectsVirtual, IDS_ACTION_MAKE_OBJECT_VIRTUAL, -1, -1, guidMakeObjectsVirtual);
ACTION_INFO_FULL(CActionMakeObjectsNonVirtual, IDS_ACTION_MAKE_OBJECT_NON_VIRTUAL, -1, -1, guidMakeObjectsNonVirtual);

ACTION_TARGET(CActionMapObjects2Image, "anydoc");
ACTION_TARGET(CActionMakeObjectsVirtual, "anydoc");
ACTION_TARGET(CActionMakeObjectsNonVirtual, "anydoc");


//////////////////////////////////////////////////////////////////////
CActionMapObjects2Image::CActionMapObjects2Image()
{

}

//////////////////////////////////////////////////////////////////////
CActionMapObjects2Image::~CActionMapObjects2Image()
{

}

//////////////////////////////////////////////////////////////////////
bool CActionMapObjects2Image::Invoke()
{
	IUnknown* punk_image		= GetActiveObjectFromDocument( m_punkTarget, szTypeImage );
	IUnknown* punk_object_list	= GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );

	IImage3Ptr				ptr_image		= punk_image;
	INamedDataObject2Ptr	ptr_object_list	= punk_object_list;

	if( punk_image )
		punk_image->Release();			punk_image = 0;

	if( punk_object_list )
		punk_object_list->Release();	punk_object_list = 0;

	if( ptr_image == 0 || ptr_object_list == 0 )
		return false;

	long lpos = 0;
	ptr_object_list->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown* punk_child = 0;
		ptr_object_list->GetNextChild( &lpos, &punk_child );

		IMeasureObjectPtr ptr_measure( punk_child );
		if( punk_child )
			punk_child->Release();	punk_child = 0;

		if( ptr_measure == 0 )
			continue;

		IUnknown* punk_child_image = 0;
		ptr_measure->GetImage( &punk_child_image );

		IImage3Ptr ptr_child_image = punk_child_image;
		if( punk_child_image )
			punk_child_image->Release();	punk_child_image = 0;

		if( ptr_child_image == 0 )
			continue;

		INamedDataObject2Ptr ptr_chid_image_ndo( ptr_child_image );
		if( ptr_chid_image_ndo == 0 )
			continue;

		ptr_chid_image_ndo->SetParent( ptr_image, get_flags() );
	}

	after_data_change( ptr_image, ptr_object_list );

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionMapObjects2Image::IsAvaible()
{
	IUnknown* punk_image		= GetActiveObjectFromDocument( m_punkTarget, szTypeImage );
	IUnknown* punk_object_list	= GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );

	bool bavailable = ( punk_image && punk_object_list );

	if( punk_image )		punk_image->Release();
	if( punk_object_list )	punk_object_list->Release();

	return bavailable;
}


//////////////////////////////////////////////////////////////////////
void CActionMapObjects2Image::after_data_change( IUnknown* punk_image, IUnknown* punk_object_list )
{
	::UpdateDataObject( m_punkTarget, punk_image );
}

//////////////////////////////////////////////////////////////////////
void CActionMakeObjectsVirtual::after_data_change( IUnknown* punk_image, IUnknown* punk_object_list )
{
	long l = cncReset;	
	FireEvent( m_punkTarget, szEventChangeObjectList, 0, punk_object_list, &l, 0);
}


//////////////////////////////////////////////////////////////////////
CActionMakeObjectsNonVirtual::CActionMakeObjectsNonVirtual()
{

}

//////////////////////////////////////////////////////////////////////
CActionMakeObjectsNonVirtual::~CActionMakeObjectsNonVirtual()
{

}

//////////////////////////////////////////////////////////////////////
bool CActionMakeObjectsNonVirtual::Invoke()
{
	IUnknown* punk_image		= GetActiveObjectFromDocument( m_punkTarget, szTypeImage );
	IUnknown* punk_object_list	= GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );

	IImage3Ptr				ptr_image		= punk_image;
	INamedDataObject2Ptr	ptr_object_list	= punk_object_list;

	if( punk_image )
		punk_image->Release();			punk_image = 0;

	if( punk_object_list )
		punk_object_list->Release();	punk_object_list = 0;

	if( ptr_image == 0 || ptr_object_list == 0 )
		return false;

	long lpos = 0;
	ptr_object_list->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown* punk_child = 0;
		ptr_object_list->GetNextChild( &lpos, &punk_child );

		IMeasureObjectPtr ptr_measure( punk_child );
		if( punk_child )
			punk_child->Release();	punk_child = 0;

		if( ptr_measure == 0 )
			continue;

		IUnknown* punk_child_image = 0;
		ptr_measure->GetImage( &punk_child_image );

		IImage3Ptr ptr_child_image = punk_child_image;
		if( punk_child_image )
			punk_child_image->Release();	punk_child_image = 0;

		if( ptr_child_image == 0 )
			continue;

		INamedDataObject2Ptr ptr_chid_image_ndo( ptr_child_image );
		if( ptr_chid_image_ndo == 0 )
			continue;

		ptr_chid_image_ndo->SetParent( 0, sdfCopyParentData );
	}

	long l = cncReset;	
	FireEvent( m_punkTarget, szEventChangeObjectList, 0, punk_object_list, &l, 0);

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CActionMakeObjectsNonVirtual::IsAvaible()
{
	IUnknown* punk_image		= GetActiveObjectFromDocument( m_punkTarget, szTypeImage );
	IUnknown* punk_object_list	= GetActiveObjectFromDocument( m_punkTarget, szTypeObjectList );

	bool bavailable = ( punk_image && punk_object_list );

	if( punk_image )		punk_image->Release();
	if( punk_object_list )	punk_object_list->Release();

	return bavailable;
}
