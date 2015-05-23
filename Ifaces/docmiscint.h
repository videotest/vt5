#ifndef __docmiscint_h__
#define __docmiscint_h__


interface IDocForNamedData : public IUnknown
{
	com_call AttachExternalDocument( IUnknown *punkND ) = 0;
};

interface IUserNameProvider : public IUnknown
{
	com_call GetUserName( IUnknown* punkObject, BSTR* pbstrUserName ) = 0;
};

interface IClipboardProvider : public IUnknown
{
	com_call GetActiveObject( IUnknown** ppunkObject ) = 0;
};

interface IViewData : public IUnknown
{
	com_call GetObject( IUnknown** ppunkObject, BSTR bstr_type ) = 0;
	com_call SetObject( IUnknown* punkObject, BSTR bstr_type ) = 0;
};

interface IFormNamesData : public IUnknown
{
	com_call GetFormName( BSTR bstr_label, BSTR *bstr_name ) = 0;
};


declare_interface( IDocForNamedData, "75077529-CC25-43d4-8558-1359ACF92770" );
declare_interface( IUserNameProvider, "C4E792C7-7BBE-43d9-A8D8-C6DA4774655D" );
declare_interface( IClipboardProvider, "FDFE8899-8C65-4583-962A-340BCE1D9E15" );
declare_interface( IViewData, "F6D409D3-CE0C-4407-BA07-5CB3A99B24E7" );
declare_interface( IFormNamesData, "9CE1A1D7-D6EF-4ded-A9CF-CF407185D8FB" );



#endif//__docmiscint_h__