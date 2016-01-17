// dllmain.h : 模块类的声明。

class CPeraFileManageModule : public ATL::CAtlDllModuleT< CPeraFileManageModule >
{
public :
	DECLARE_LIBID(LIBID_PeraFileManageLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PERAFILEMANAGE, "{078F0096-69E7-45FC-8912-18E6CBD387A5}")
};

extern class CPeraFileManageModule _AtlModule;
