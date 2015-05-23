#pragma once

class CTimer
{
	int m_nID;
	static CTimer *m_pTimer;
	friend void CALLBACK EXPORT TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
public:
	CTimer();
	~CTimer();
	bool _SetTimer(DWORD dwElapse);
	void _KillTimer();
	virtual void OnTimer() = 0;
};