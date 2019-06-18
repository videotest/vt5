#if !defined _types_h
#define _types_h

enum TemplateSource
{
	tsUseExist = 0,
	tsAutoTemplate = 1,
};

enum AutoTemplateAlghoritm
{
	ataActiveView = 0,
	ataOneObjectPerPage = 1,
	ataVerticalAsIs = 2,
	ataGallery = 3,
	ataFromSplitter = 4,
};

enum TemplateGenerateTarget
{
	tgtNoGenerate = 0,
	tgtSaveInDocument = 1,
	tgtSaveToFile = 2,
};

/*
interface IViewCtrl : public Iunknown
{
	//Auto build mode
	com_call GetAutoBuild( BOOL* pbAutoBuild ) = 0;
	com_call SetAutoBuild( BOOL bAutoBuild ) = 0;

	com_call Build( BOOL* pbSucceded ) = 0;

	//size
	com_call GetDPI( int* pnDPI ) = 0;
	com_call SetDPI( int nDPI ) = 0;

	com_call GetZoom( float* pfZoom ) = 0;
	com_call SetZoom( float fZoom ) = 0;

	com_call GetObjectDefinition( int* pnObjectDefinition ) = 0;
	com_call SetObjectDefinition( int nObjectDefinition ) = 0;

	//active view
	com_call GetUseActiveView( BOOL* pbUseActiveView ) = 0;
	com_call SetUseActiveView( BOOL bUseActiveView ) = 0;

	com_call GetViewAutoAssigned( BOOL* pbViewAutoAssigned ) = 0;
	com_call SetViewAutoAssigned( BOOL bViewAutoAssigned ) = 0;

	com_call GetViewProgID( BSTR* pbstrProgID ) = 0;
	com_call SetViewProgID( BSTR bstrProgID ) = 0;

	//object list
	com_call GetFirstObjectPosition( long* plPos ) = 0;
	com_call GetNextObject( BSTR* pbstrObjectName, BOOL* pbActiveObject, 
							BSTR* bstrObjectType, long *plPos ) = 0;
	com_call InsertAfter( long lPos,  
							BSTR bstrObjectName, BOOL bActiveObject, 
							BSTR bstrObjectType ) = 0;
	com_call EditAt( long lPos,  
							BSTR bstrObjectName, BOOL bActiveObject, 
							BSTR bstrObjectType ) = 0;
	com_call RemoveAt( long lPos ) = 0;
	
};
*/


#endif //_types_h 
