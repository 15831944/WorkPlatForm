#ifndef THREAD_ENV_H

#define THREAD_ENV_H

//#define PTW32_STATIC_LIB

#include <pthread.h>

#define PTHREAD_ENABLE_CANCEL {pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);}
#ifdef WIN32
#define PTHREAD_INIT {pthread_win32_process_attach_np();pthread_win32_thread_attach_np();atexit(detach_ptw32);}
static void detach_ptw32(void)
{
	pthread_win32_thread_detach_np();
	pthread_win32_process_detach_np();
}
#else
#define PTHREAD_INIT

#endif
#endif
