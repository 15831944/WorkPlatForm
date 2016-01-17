#pragma once
///////////////////////////////////////////////////以下是LIBCURL包含的头文件和加载库
#include "curl.h"

typedef int int32;
typedef unsigned int uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#define SAFE_CLOSE_HANDLE(h) if(h){CloseHandle(h);h=NULL;}
#define SIZE_OF_ARRAY(x) sizeof(x)/sizeof(x[0])
#define SAFE_DELETE_ARRAY(x) if(x){delete []x;x=NULL;}
#define LOINT32(i)				((int)((__int64)(i) & 0xffffffff))
#define HIINT32(i)				((int)((__int64)(i) >> 32))

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "libcurl_ud.lib")
#		pragma comment(lib, "Wldap32.lib")
#   else
#       pragma comment(lib, "libcurl_md.lib")
#		pragma comment(lib, "Wldap32.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "libcurl_u.lib")
#		pragma comment(lib, "Wldap32.lib")
#   else
#       pragma comment(lib, "libcurl_m.lib")
#		pragma comment(lib, "Wldap32.lib")
#   endif
#endif
////////////////////////////////////////////////////
