#ifndef __clone_h__
#define __clone_h__


interface ICompatibleObject : public IUnknown
{
	com_call CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize ) = 0;	
};

interface ICompatibleObject2 : public ICompatibleObject
{
	com_call Synchronize( ) = 0;	
};

interface ICompatibleObject3 : public ICompatibleObject2
{
	com_call IsEqual( IUnknown *punkObjSource ) = 0;	
};

declare_interface( ICompatibleObject, "63F734EE-D851-436c-9FED-A841D3D65C41" )
declare_interface( ICompatibleObject2, "E15986C9-8CB5-40e4-B671-62DF19D6E2EF" )
declare_interface( ICompatibleObject3, "39A694F4-67F0-4ed2-8BAB-CC20803F5391" )


#endif //__clone_h__