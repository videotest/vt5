#ifndef __fileopenint_h__
#define __fileopenint_h__

enum OpenFileFlags
{
	offOpen=1,
	offSave=2
};

interface IOpenFileDialog : public IUnknown
{
	com_call Execute( DWORD dwFlags ) = 0;
	com_call SetOFNStruct( OPENFILENAME *pofn ) = 0;
	
	com_call GetPathName( BSTR * pbstrFileName ) = 0;
	com_call GetFilesCount( int *pnCount ) = 0;
	com_call GetFileName( int nPos, BSTR *pbstrFileName ) = 0;

	com_call SaveState( IUnknown *punkDD ) = 0;
	com_call RestoreState( IUnknown *punkDD ) = 0;

	com_call GetDlgWindowHandle( HWND *phWnd ) = 0;
	com_call SetDefaultSection( BSTR bstr ) = 0;
};

declare_interface( IOpenFileDialog, "49F1A191-70E0-11d3-A4F5-0000B493A187" );

#endif //__fileopenint_h__