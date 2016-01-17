// dllmain.h : 模块类的声明。

class CPeraPcIceClientModule : public ATL::CAtlDllModuleT< CPeraPcIceClientModule >
{
public :
	DECLARE_LIBID(LIBID_PeraPcIceClientLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PERAPCICECLIENT, "{4A52A163-3054-43F3-A32C-533D89EE4081}")
};

extern class CPeraPcIceClientModule _AtlModule;
