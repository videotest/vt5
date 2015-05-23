#ifndef __notifyspec_h__
#define __notifyspec_h__

#include "defs.h"


interface INotifyPlace : public IUnknown
{
	com_call NotifyPutToData( IUnknown* punkObj ) = 0;
	com_call NotifyPutToDataEx( IUnknown* punkObj, IUnknown** punkUndoObj ) = 0;
	com_call Undo( IUnknown* punkUndoObj ) = 0;
	com_call Redo( IUnknown* punkUndoObj ) = 0;
};

declare_interface( INotifyPlace, "5D2ECB17-C33D-43ed-8F1C-5864B6C130A7" );

#endif//__notifyspec_h__

