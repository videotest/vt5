#ifndef __obj_man_h__
#define __obj_man_h__

#include "defs.h"

#define szObjManSection			"\\ObjectManager"
#define szObjManPathPrefix		"Path_"

interface IObjectManager : public IUnknown
{
	//enum types
	com_call GetFirstObjectTypePos( long* plPos ) = 0;
	com_call GetNextObjectType( long* plPos, BSTR* pbstrTypeName ) = 0;	

	//enum names
	com_call GetFirstObjectNamePos( BSTR bstrType, long* plPos ) = 0;
	com_call GetNextObjectByName( BSTR bstrType, long* plPos, BSTR* pbstrName ) = 0;

	//enum unknown
	com_call GetFirstObjectPos( BSTR bstrType, long* plPos ) = 0;
	com_call GetNextObject( BSTR bstrType, long* plPos, IUnknown** ppunkObj ) = 0;

	//by name activation
	com_call GetActiveObjectName( BSTR bstrType, BSTR* pbstrName ) = 0;
	com_call SetActiveObjectByName( BSTR bstrType, BSTR bstrName ) = 0;

	//by unknown activation
	com_call GetActiveObject( BSTR bstrType, IUnknown** ppunkObj ) = 0;
	com_call SetActiveObject( IUnknown* punkObj ) = 0;

	
	com_call FreeLoadedObject( BSTR bstrType, BSTR bstrName ) = 0;
	com_call GetObject( BSTR bstrType, BSTR bstrName, IUnknown** ppunkObj ) = 0;

	com_call GetObjectDocument( BSTR bstrType, BSTR bstrName, IUnknown** ppunkDoc ) = 0;
};

declare_interface( IObjectManager, "40BB8C73-44AE-4d2a-B536-60927D4CB26C" );


#endif //__obj_man_h__