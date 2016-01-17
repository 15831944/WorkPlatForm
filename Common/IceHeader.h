
#define ICE_STATIC_LIBS
#ifdef _DEBUG
#pragma comment(lib, "iced.lib")
#pragma comment(lib, "iceutild.lib")
#pragma comment(lib, "libbz2d.lib")
#else
#pragma comment(lib, "ice.lib")
#pragma comment(lib, "iceutil.lib")
#pragma comment(lib, "libbz2.lib")
#endif
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Iphlpapi.lib")

