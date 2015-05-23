// calibr.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "calibr_guids.h"
#include "com_dispatch.h"
#include "impl_misc.h"

//1. Create .idl file and include it to the project
//2. 

#pragma comment (lib, "common2.lib")



// {E0FA6E51-4FBB-4543-AB6F-42C682BC7DEC}
GUID dispCalibration = 
{ 0xe0fa6e51, 0x4fbb, 0x4543, { 0xab, 0x6f, 0x42, 0xc6, 0x82, 0xbc, 0x7d, 0xec } };


const char *pszCalibrationProgID = "calibr.Calibration";

class Calibration : public ComObjectBase,
					public CNamedObjectImpl,
					public CDispatchImpl<CalibrationTypeLib::ICalibration, &__uuidof(CalibrationTypeLib::ICalibration)>,
					public CDispatchImpl<CalibrationTypeLib::INamedObject, &__uuidof(CalibrationTypeLib::INamedObject)>,
					public _dyncreate_t<Calibration>
{
	route_unknown();
public:
	Calibration();
	virtual ~Calibration();

	com_call  raw_GetCalibration ( double * pfCalibr ) {*pfCalibr = 1;return S_OK;}
	com_call  raw_GetName( BSTR * pbstrName )			{* pbstrName = m_bstrName.copy();return S_OK;}
    com_call  raw_SetName( BSTR bstrName )				{m_bstrName = bstrName;return S_OK;}
	virtual IUnknown *DoGetInterface( const IID &iid );

	double	fCalibrValue;
};

Calibration::Calibration()
{
	m_bstrName = "Calibration";
}
Calibration::~Calibration()
{
}

IUnknown *Calibration::DoGetInterface( const IID &iid )
{
	if( iid == IID_INamedObject2 )
		return (INamedObject2*)this;
	else if( iid == IID_IDispatch )
		return (CalibrationTypeLib::INamedObject*)this;	
	else if( iid == __uuidof( CalibrationTypeLib::ICalibration ) )
		return (CalibrationTypeLib::ICalibration*)this;	
	else if( iid == __uuidof( CalibrationTypeLib::INamedObject ) )
		return (CalibrationTypeLib::INamedObject*)this;
	else
		return ComObjectBase::DoGetInterface( iid );
}

/////////////////////////////////////////////////////////////////////////////////////////
//main
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		app	*papp = new app;
		papp->Init( (HINSTANCE)hModule );

		papp->ObjectInfo( _dyncreate_t<Calibration>::CreateObject, __uuidof(CalibrationTypeLib::Calibration), pszCalibrationProgID, "script namespace" );

		ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
		pfactory->LoadObjectInfo();
		pfactory->Release();
	}
	else if( ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		app::instance()->Deinit();
		app::instance()->Release();
	}

    return TRUE;
}

/*
Calibration.SetName "qq"
name= "name"
Calibration.GetName name
MsgBox name
*/

implement_com_exports();