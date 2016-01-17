// dllmain.h : 模块类的声明。

class CPeraFileUpDownModule : public ATL::CAtlDllModuleT< CPeraFileUpDownModule >
{
public :
	DECLARE_LIBID(LIBID_PeraFileUpDown)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PERAFILEUPDOWN, "{17AC6A0A-6743-491F-B2CA-266F440D9049}")
};

extern class CPeraFileUpDownModule _AtlModule;
