// dllmain.h : 模块类的声明。

class CPeraNetDiskComModule : public ATL::CAtlDllModuleT< CPeraNetDiskComModule >
{
public :
	DECLARE_LIBID(LIBID_PeraNetDiskComLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PERANETDISKCOM, "{45DAAF32-9667-494F-A5E9-AAC01DD36DAB}")
};

extern class CPeraNetDiskComModule _AtlModule;
