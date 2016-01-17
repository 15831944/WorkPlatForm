// dllmain.h : 模块类的声明。

class CPeraComponentEditModule : public ATL::CAtlDllModuleT< CPeraComponentEditModule >
{
public :
	DECLARE_LIBID(LIBID_PeraComponentEditLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PERACOMPONENTEDIT, "{12E8D51A-7BED-46E4-9358-16B7230DD7DE}")
};

extern class CPeraComponentEditModule _AtlModule;
