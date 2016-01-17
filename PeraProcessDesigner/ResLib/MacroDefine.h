#ifndef _VORX_MACRO_DEFINE_HEAD_FILE
#define _VORX_MACRO_DEFINE_HEAD_FILE


#define UINT16 unsigned short
#define UINT8  unsigned char

#ifdef WIN32
	#define M_SLEEP(x) Sleep(x)							//毫秒级休眠
	#define TRHEAD_RETURN_VALUE							//线程函数返回值
	#define THREAD_NULL NULL		
	#define THREAD_VOID void							//线程函数返回值类型
	#define HTHREAD HANDLE								//线程句柄
	#define CREATE_THREAD(handle,addr) handle = (HANDLE)_beginthread(addr,0,this)	//启动线程
	#define WAIT_FOR_THREAD(handle) \
			if(WaitForSingleObject(handle,1000) == WAIT_TIMEOUT)\
			{\
				DWORD dwExitCode;\
				GetExitCodeThread(handle,&dwExitCode);\
				TerminateThread(handle,dwExitCode);\
			}
	typedef int socklen_t;
	#include <windows.h>
	#include <process.h>
	#define MIN min
	#define MAX max

#else
	#define UINT64 unsigned long long
	#define UINT32 unsigned long
	#include <unistd.h>
	#include <pthread.h>
	#include <sys/times.h>
	#include <sys/time.h>
	#define __stdcall
	#define M_SLEEP(x) usleep(1000 * x)
	#define THREAD_VOID void*
	#define TRHEAD_RETURN_VALUE NULL
	#define THREAD_NULL 0
	#define HTHREAD pthread_t
	#define CREATE_THREAD(handle,addr) pthread_create(&handle,NULL,addr,this)
	#define WAIT_FOR_THREAD(handle) pthread_join(handle,NULL)
	#define BYTE unsigned char
	#define ULONG unsigned long
	#define UINT unsigned int
	#define WORD unsigned short
	#define DWORD unsigned long
	#define LPCTSTR const char*
	#define BOOL int
	#define TRUE 1
	#define FALSE 0
	#define LPVOID void*
	#define HANDLE void*
	#define TRACE printf
	#define INVALID_SOCKET (-1)
	#define closesocket close
	#define CloseHandle(xx)
	#define SOCKET_ERROR (-1)
	#define HOSTENT hostent
	typedef int SOCKET;
	#define MIN(a,b) (a>b?b:a)
	#define MAX(a,b) (a>b?a:b)
	inline ULONG GetTickCount()
	{
#ifdef PRECISION_LOCATE
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return tv.tv_sec*1000+tv.tv_usec/1000;
#else
		return (1000*times(NULL))/sysconf(_SC_CLK_TCK);
#endif
	}

#endif//ifdef WIN32

#pragma warning( disable : 4786 )
#include <vector>
#include <list>
using namespace std;
#include <stdio.h>

#define HRES  long
#define HTASK long
typedef void (__stdcall *OUTPUT_LOG)(int nType,LPCTSTR sMsg,void* lParam);
typedef void (__stdcall *ASYN_RET)(int nFun,int nSub,int nRetCode,unsigned char* pBody,int nLen,void* lParam);

#endif//ifndef _VORX_MACRO_DEFINE_HEAD_FILE
