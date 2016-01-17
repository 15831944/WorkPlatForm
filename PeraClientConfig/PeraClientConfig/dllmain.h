// dllmain.h : 模块类的声明。

class CPeraClientConfigModule : public ATL::CAtlDllModuleT< CPeraClientConfigModule >
{
public :
	DECLARE_LIBID(LIBID_PeraClientConfigLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PERACLIENTCONFIG, "{00B93943-E986-4E90-B84D-98E7DC8DB094}")
};

extern class CPeraClientConfigModule _AtlModule;
