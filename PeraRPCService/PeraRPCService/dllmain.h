// dllmain.h : 模块类的声明。

class CProcessClientServicesModule : public ATL::CAtlDllModuleT< CProcessClientServicesModule >
{
public :
	DECLARE_LIBID(LIBID_ProcessClientServicesLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PROCESSCLIENTSERVICES, "{4A52A163-3054-43F3-A32C-533D89EE4081}")
};

extern class CProcessClientServicesModule _AtlModule;
