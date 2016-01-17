// MutexObject.cpp: implementation of the CMutexInteger class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MutexInteger.h"
#ifndef WIN32
#include <sys/time.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


	//////////////////////////////////////////////////////////////////////
	// CVorxEvent
	//////////////////////////////////////////////////////////////////////
	CVorxEvent::CVorxEvent()
	{
#ifndef USE_CHECK_EVENT
#ifdef WIN32 
		m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
#else
		pthread_cond_init(&m_hEvent,NULL);
		pthread_mutex_init(&m_mutex, NULL);
#endif
#else
		m_mutex.Lock();
		m_bSignal = FALSE;
		m_mutex.Unlock();
#endif
	}
	
	CVorxEvent::~CVorxEvent()
	{
#ifndef USE_CHECK_EVENT

#ifdef WIN32 
		CloseHandle(m_hEvent);
#else
		pthread_cond_destroy(&m_hEvent);
		pthread_mutex_destroy(&m_mutex);
#endif

#else
		m_mutex.Lock();
		m_bSignal = TRUE;
		m_mutex.Unlock();
#endif
	}
	
	BOOL CVorxEvent::WaitForSignal(int nTimeOut)
	{
#ifndef USE_CHECK_EVENT

#ifdef WIN32
		return (WaitForSingleObject(m_hEvent,nTimeOut) == WAIT_OBJECT_0);
#else
		struct timeval newtime;
		gettimeofday(&newtime,NULL);
		
		struct timespec timeout;
		timeout.tv_sec = newtime.tv_sec+(nTimeOut/1000);
		timeout.tv_nsec = (newtime.tv_usec+(nTimeOut%1000)*1000)*1000;
		if(timeout.tv_nsec > 1000000000)
		{
			timeout.tv_sec += 1;
			timeout.tv_nsec -= 1000000000;
		}
		
		pthread_mutex_lock(&m_mutex);
		int ret = pthread_cond_timedwait(&m_hEvent, &m_mutex, &timeout);
		pthread_mutex_unlock(&m_mutex);
		
		return (ret == 0);
#endif

#else
		BOOL ret = FALSE;
		DWORD nTime = GetTickCount()+nTimeOut;
		while(!ret && (nTimeOut==0 || GetTickCount()<nTime))
		{
			m_mutex.Lock();
			if(m_bSignal)
			{
				ret = TRUE;
				m_bSignal = FALSE;
			}
			m_mutex.Unlock();
			if(!ret)M_SLEEP(1);
		}
		return ret;
#endif
	}
	
	void CVorxEvent::Signal()
	{
#ifndef USE_CHECK_EVENT
#ifdef WIN32
		SetEvent(m_hEvent);
#else
		pthread_mutex_lock(&m_mutex);
		pthread_cond_signal(&m_hEvent);
		pthread_mutex_unlock(&m_mutex);
#endif
#else
		m_mutex.Lock();
		m_bSignal = TRUE;
		m_mutex.Unlock();
#endif
	}
	
	//////////////////////////////////////////////////////////////////////
	// CMutexInteger
	//////////////////////////////////////////////////////////////////////
	
	CMutexInteger::CMutexInteger()
	{
		m_nValue = 0;
	}
	
	CMutexInteger::CMutexInteger(const CMutexInteger &ref)
	{
		m_nValue = ref.m_nValue;
	}
	
	CMutexInteger::~CMutexInteger()
	{
	}

	int CMutexInteger::GetAndSetValue(int nVal)
	{
		CAutoMutex l(&m_mutex);
		int ret = m_nValue;
		m_nValue = nVal;
		return ret;
	}
	
	void CMutexInteger::operator&=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		m_nValue &= nParam;
	}
	
	void CMutexInteger::operator|=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		m_nValue |= nParam;
	}
	
	int  CMutexInteger::operator&(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue & nParam);
	}
	
	int  CMutexInteger::operator|(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue | nParam);
	}
	
	void  CMutexInteger::operator=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		m_nValue = nParam;
	}
	
	bool CMutexInteger::operator==(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue == nParam);
	}
	
	void CMutexInteger::operator+=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		m_nValue += nParam;
	}
	
	int  CMutexInteger::operator+(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue + nParam);
	}
	
	void CMutexInteger::operator-=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		m_nValue -= nParam;
	}
	
	int  CMutexInteger::operator-(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue - nParam);
	}
	
	bool CMutexInteger::operator!()
	{
		CAutoMutex l(&m_mutex);
		return !m_nValue;
	}
	
	bool CMutexInteger::operator>(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue > nParam);
	}
	
	bool CMutexInteger::operator>=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue >= nParam);
	}
	
	bool CMutexInteger::operator<(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue < nParam);
	}
	
	bool CMutexInteger::operator<=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue <= nParam);
	}
	
	bool CMutexInteger::operator!=(int nParam)
	{
		CAutoMutex l(&m_mutex);
		return (m_nValue != nParam);
	}
	
	void CMutexInteger::operator++()
	{
		CAutoMutex l(&m_mutex);
		m_nValue++;
	}
	
	void CMutexInteger::operator--()
	{
		CAutoMutex l(&m_mutex);
		m_nValue--;
	}
	
	CMutexInteger::operator int()
	{
		CAutoMutex l(&m_mutex);
		return m_nValue;
	}
	
	void CMutexInteger::operator=(const CMutexInteger& nParam)
	{
		CAutoMutex l(&m_mutex);
		m_nValue = nParam.m_nValue;
	}

