#if !defined(__VideoHook_H__)
#define __VideoHook_H__

#include "input.h"

class CVideoHookImpl : public IVideoHook
{
	int m_idMsg,m_idProgress;
	CString m_sMsg;
	HWND m_hwndParent;
	int m_nStages,m_nFrames;
	HWND m_hwndMsg;
	CProgressCtrl m_Progress;
public:
	CVideoHookImpl()
	{
		m_hwndMsg = NULL;
	}
	~CVideoHookImpl()
	{
	}
	void Init(HWND hwndParent, int nMsg, int nProgress)
	{
		m_hwndParent = hwndParent;
		m_idMsg = nMsg;
		m_idProgress = nProgress;
		m_hwndMsg = ::GetDlgItem(hwndParent, m_idMsg);
		::GetWindowText(m_hwndMsg, m_sMsg.GetBuffer(256), 256);
		m_sMsg.ReleaseBuffer();
		HWND hwnd = ::GetDlgItem(hwndParent, m_idProgress);
		m_Progress.Attach(hwnd);
	};
	void Deinit()
	{
		m_Progress.Detach();
	}
	com_call OnStart(int nStages)
	{
		m_nStages = nStages;
		::ShowWindow(m_hwndMsg, SW_SHOW);
		m_Progress.ShowWindow(SW_SHOW);
		return S_OK;
	}
	com_call OnBeginStage(int nStage, const char *pszStageName, int nFrames)
	{
		CString s;
		s.Format(m_sMsg, pszStageName, nStage, m_nStages);
		::SetWindowText(::GetDlgItem(m_hwndParent, m_idMsg), s);
		m_nFrames = nFrames;
		m_Progress.SetRange32(0, 100);
		m_Progress.SetPos(0);
		return S_OK;
	}
	com_call OnEndStage(int nStage)
	{
		return S_OK;
	}
	com_call OnFrame(int nFrame)
	{
		int nPos = nFrame*100/m_nFrames;
		m_Progress.SetPos(nPos);
		return S_OK;
	}
	com_call OnEnd()
	{
		::ShowWindow(m_hwndMsg, SW_HIDE);
		m_Progress.ShowWindow(SW_HIDE);
		return S_OK;
	};
};





#endif
