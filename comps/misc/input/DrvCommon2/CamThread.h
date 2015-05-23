#pragma once


class CCamThread
{
protected:
	HANDLE m_hEventStop;
	HANDLE m_hThread;
public:
	CCamThread();
	~CCamThread();

	void Start();
	void Stop();
	bool IsStarted();
	virtual void ThreadRoutine() {}

	bool WaitForEvent(HANDLE hEvent); // if true then need to stop thread
};

class CCamThread2
{
protected:
	HANDLE m_hThread;
	LONG m_lStopFlag;
public:
	CCamThread2();
	~CCamThread2();

	void Start();
	void Stop();
	bool IsStarted();
	virtual void ThreadRoutine() {}
};

