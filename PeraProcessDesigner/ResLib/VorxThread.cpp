// VorxThread.cpp: implementation of the CVorxThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VorxThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVorxThread::CVorxThread()
{
	//�߳̿��Ʋ���
	m_hThread = 0;
	m_bThread = FALSE;
	//�̶߳������
	m_lParam = 0;
	m_fun = 0;
	m_nCycle = 0;
	m_exit = 0;
}

CVorxThread::~CVorxThread()
{
	StopThread();
}

BOOL CVorxThread::SetParam(LPFUN_THREAD fun,LPVOID lParam,int nCycle,LPTHREAD_EXIT fpExit)
{
	if(m_bThread) return FALSE;
	m_fun = fun;
	m_exit = fpExit;
	m_lParam = lParam;
	m_nCycle = nCycle;
	return TRUE;
}

BOOL CVorxThread::StartThread()
{
	if(m_bThread || !m_fun) return FALSE;
#ifdef WIN32
	m_hThread = (HANDLE)_beginthread(thread_func,0,this);
#else
	pthread_create(&m_hThread,NULL,thread_func,this);
#endif
	return TRUE;
}

void CVorxThread::StopThread()
{
	if(m_bThread)
	{
		m_bThread = FALSE;
		//�����¼�,֪ͨ�߳̽���
		m_oEvent.Signal();
		//�ȴ��̷߳���
		if(m_hThread)
		{
#ifdef WIN32
			if(WaitForSingleObject(m_hThread,1000) == WAIT_TIMEOUT)
			{
				DWORD dwExitCode;
				GetExitCodeThread(m_hThread,&dwExitCode);
				TerminateThread(m_hThread,dwExitCode);
			}
#else
			pthread_join(m_hThread,NULL);
#endif
		}
	}
}

BOOL CVorxThread::SetPriority(int val)
{
#ifdef WIN32
	int nVal = 0;
	switch(val)
	{
	case -3:
		nVal = THREAD_PRIORITY_IDLE;break;
	case -2:
		nVal = THREAD_PRIORITY_LOWEST;break;
	case -1:
		nVal = THREAD_PRIORITY_BELOW_NORMAL;break;
	case 0:
		nVal = THREAD_PRIORITY_NORMAL;break;
	case 1:
		nVal = THREAD_PRIORITY_ABOVE_NORMAL;break;
	case 2:
		nVal = THREAD_PRIORITY_HIGHEST;break;
	case 3:
		nVal = THREAD_PRIORITY_TIME_CRITICAL;break;
	}
	return SetThreadPriority(m_hThread,nVal);
#else
	struct sched_param  hThreadPrior;
	int policy;
	pthread_getschedparam(m_hThread, &policy, &hThreadPrior);
	policy = SCHED_RR;
	hThreadPrior.sched_priority = val;
	return (pthread_setschedparam(m_hThread,policy,&hThreadPrior) == 0);
#endif
}

THREAD_RETURN CVorxThread::thread_func(void* lParam)
{
	CVorxThread* pThis = (CVorxThread*)lParam;

	pThis->m_bThread = TRUE;
	unsigned long tTime;
	int nSleepTime;			//����Ҫ���ߵĺ�����
	int nRes = 0;
	while(pThis->m_bThread)
	{
		//ִ�к���
		tTime = GetTickCount();
		if(pThis->m_fun)
		{
			nRes = pThis->m_fun(pThis->m_lParam);
		}
		if(!pThis->m_bThread)break;

		switch(nRes)
		{
		case 1://������һ��ѭ��
			break;

		case 0://ʵ������(�ȴ��¼���ʱ,�����¼�)
			nSleepTime = pThis->m_nCycle - (GetTickCount() - tTime);
			if(nSleepTime > 0)
				pThis->m_oEvent.WaitForSignal(nSleepTime);
			break;

		case -1: //�˳��߳�
			pThis->m_bThread = false;
			pThis->m_hThread = 0;
		}
	}
	if(pThis->m_exit)
		pThis->m_exit(pThis->m_lParam);
	return THREAD_VALUE;
}


