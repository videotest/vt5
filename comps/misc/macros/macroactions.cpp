#include "stdafx.h"
#include "macroactions.h"
#include "MacroRecorder.h"
#include "resource.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionMacroRecord, CCmdTargetEx);

// olecreate 

// {614578B3-6B71-11d3-A652-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionMacroRecord, "macro.MacroRecord",
0x614578b3, 0x6b71, 0x11d3, 0xa6, 0x52, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

// guidinfo 

// {614578B1-6B71-11d3-A652-0090275995FE}
static const GUID guidMacroRecord =
{ 0x614578b1, 0x6b71, 0x11d3, { 0xa6, 0x52, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };



//[ag]6. action info

ACTION_INFO_FULL(CActionMacroRecord, IDS_ACTION_RECORDMACRO, -1, -1, guidMacroRecord);

//[ag]7. targets

ACTION_TARGET(CActionMacroRecord, "anydoc");

//[ag]8. arguments


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionMacroRecord implementation
CActionMacroRecord::CActionMacroRecord()
{
}

CActionMacroRecord::~CActionMacroRecord()
{
}

bool CActionMacroRecord::Invoke()
{
	if( CMacroRecorder::s_pMacroRecorderInstance->GetRecording() )
		CMacroRecorder::s_pMacroRecorderInstance->Finalize( m_punkTarget );
	else
		CMacroRecorder::s_pMacroRecorderInstance->Record();
	return true;
}


bool CActionMacroRecord::IsAvaible()
{
	return true;
};

bool CActionMacroRecord::IsChecked()
{
	return CMacroRecorder::s_pMacroRecorderInstance->GetRecording();
};
