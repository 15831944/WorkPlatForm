// dllmain.h : 模块类的声明。

class CStartToolComModule : public ATL::CAtlDllModuleT< CStartToolComModule >
{
public :
	DECLARE_LIBID(LIBID_PeraStartToolComLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_STARTTOOLCOM, "{5066A291-5D31-4A34-9753-101E8784FBF9}")
};

extern class CStartToolComModule _AtlModule;
