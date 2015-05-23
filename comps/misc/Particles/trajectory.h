#ifndef __trajectory_h__
#define __trajectory_h__

#include "com_main.h"
#include "data_main.h"
#include "misc_list.h"
#include "impl_draw.h"
#include "trajectory_int.h"
#include "Named2DArrays.h"
#include "\vt5\awin\misc_map.h"

static const GUID clsid_trajectory = 
{ 0x86c6c4bc, 0x1b6c, 0x49b0, { 0x9b, 0x5, 0xcd, 0x25, 0xfa, 0x1a, 0x8b, 0x56 } };

static const GUID clsid_trajectory_info = 
{ 0x12ca7b41, 0x95eb, 0x48a3, { 0xbe, 0xcf, 0x79, 0xf0, 0x3c, 0x1f, 0xde, 0xca } };

class trajectory_info : public CDataInfoBase,
	public _dyncreate_t<trajectory_info>
{
	route_unknown();
public:
	trajectory_info();
};

struct CTrajectoryData
{
	double r_max;
};

typedef CTrajectoryData *PCTrajectoryData;

interface ITrajectoryData : public IUnknown
{ // только для внутреннего употребления в dll
	com_call GetTrajectoryDataPtr(CTrajectoryData **ppData)=0;
};

declare_interface(ITrajectoryData, "1E615D73-BDF0-416f-98A9-4E8721D42666");

class trajectory : public CChildObjectBase,
	public _dyncreate_t<trajectory>,
	public CDrawObjectImpl,
	public ITrajectory,
	public ITrajectoryData,
	public IRectObject,
	public INotifyObject,
	public ISmartSelector
{
	route_unknown();
public:
	virtual GuidKey GetInternalClassID() {return clsid_trajectory;};
	virtual IUnknown *DoGetInterface( const IID &iid )	
	{
		if( iid == IID_IDrawObject ) return (IDrawObject*)this;
		if( iid == IID_ITrajectory ) return (ITrajectory*)this;
		if( iid == IID_ITrajectoryData ) return (ITrajectoryData*)this;
		if( iid == IID_IRectObject ) return (IRectObject*)this;
		if( iid == IID_IRectObject ) return (IRectObject*)this;
		if( iid == IID_INotifyObject) return (INotifyObject*)this;
		if( iid == IID_ISmartSelector) return (ISmartSelector*)this;
		return CObjectBase::DoGetInterface( iid );
	}

	trajectory();

public:
// IDrawObject == CDrawObjectImpl
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call SetTargetWindow( IUnknown *punkTarget );

// ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );

// IRectObject
	com_call SetRect(RECT rc);
	//com_call GetRect(RECT* prc);
	com_call HitTest( POINT	point, long *plHitTest );
	com_call Move( POINT point );

// INotifyObject
	com_call NotifyCreate();
	com_call NotifyDestroy();
	com_call NotifyActivate( bool bActivate );
	com_call NotifySelect( bool bSelect );

// ISmartSelector
	com_call SmartSelect( bool bSelect, IUnknown *punkSelection );
	// в переданном punkSelection (MultiFrame) селектит те объекты, которые нужны данному

// ITrajectory
	com_call GetObjNum(long * plNum);
	com_call SetObjNum(long lNum);

// ITrajectoryData
	com_call GetTrajectoryDataPtr(CTrajectoryData **ppData);

public:

private:
	long m_nObjNum;
	_map_t<COLORREF, long, cmp_long>	ms_class_colors;  
	COLORREF _get_class_color( long lClass );
	CTrajectoryData m_TrajectoryData;
};


//---------------------------------------------------------------------------
IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid);
ITrajectoryPtr CreateTrajectory(IUnknown *punkCalcObject);
//---------------------------------------------------------------------------

#endif //__trajectory_h__