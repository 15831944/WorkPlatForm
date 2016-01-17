//////////////////////////////////////////////////////////////////////
//
// VorxThread.h: interface for the CVorxThread class.
// 功能:线程启动停止控制类,可跨Windows/Linux双平台
// 说明:线程启动后,周期性地循环调用指定的功能函数
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VORXTHREAD_H__4CD68B43_1A04_42E5_8EA0_978D749196E5__INCLUDED_)
#define AFX_VORXTHREAD_H__4CD68B43_1A04_42E5_8EA0_978D749196E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MacroDefine.h"
#include "MutexInteger.h"

#ifdef WIN32
	#include <windows.h>
	#include <process.h>

	#define THREAD_RETURN void
	#define THREAD_VALUE
	#define pthread_t HANDLE
#else
	#include <unistd.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <sys/times.h>

	#define THREAD_RETURN void*
	#define THREAD_VALUE 0

#endif


//////////////////////////////////////////////////////////////
typedef int (*LPFUN_THREAD)(LPVOID lParam);
// 函数说明 : 线程内周期性循环执行的回调函数格式定义
// 返 回 值 : 0:阻塞 1:不阻塞 -1:退出线程
typedef void (*LPTHREAD_EXIT)(LPVOID lParam);
// 函数说明 : 线程退出时执行的回调函数
//////////////////////////////////////////////////////////////

class CVorxThread
{
private:
	void*	m_lParam;		//外部传入的线程参数
	LPFUN_THREAD m_fun;		//外部传入的函数指针
	LPTHREAD_EXIT m_exit;	//线程退出指针

	pthread_t m_hThread;	//线程控制句柄
	BOOL      m_bThread;	//控制线程启动及停止
	int		  m_nCycle;		//函数执行周期(ms)
	CVorxEvent m_oEvent;	//线程等待事件
	static THREAD_RETURN thread_func(void* lParam);

public:
	//设置参数
	BOOL SetParam(LPFUN_THREAD fpFunc,LPVOID lParam,int msCycle,LPTHREAD_EXIT fpExit = NULL);
	//启动线程
	BOOL StartThread();
	//停止线程
	void StopThread();
	//设置优先级,值范围 -3 ~ 3
	BOOL SetPriority(int val);
	//获取运行状态
	BOOL IsRuning(){return m_bThread;}
	//唤醒休眠状态
	void Waken(){m_oEvent.Signal();}

	CVorxThread();
	virtual ~CVorxThread();

};
#endif // !defined(AFX_VORXTHREAD_H__4CD68B43_1A04_42E5_8EA0_978D749196E5__INCLUDED_)
