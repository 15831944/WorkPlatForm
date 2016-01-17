// ComponentEdit.h : CComponentEdit 的声明

#pragma once
#include "resource.h"       // 主符号

#include "PeraComponentEdit_i.h"
#include "_IComponentEditEvents_CP.h"
#include "SharedMemoryOnlineEdit.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CComponentEdit
typedef map<string, string> MAPSTR;
typedef map<string, string>::iterator ITERSTR_MAP;

class ATL_NO_VTABLE CComponentEdit :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComponentEdit, &CLSID_ComponentEdit>,
	public IConnectionPointContainerImpl<CComponentEdit>,
	public CProxy_IComponentEditEvents<CComponentEdit>,
	public IDispatchImpl<IComponentEdit, &IID_IComponentEdit, &LIBID_PeraComponentEditLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectSafetyImpl<CComponentEdit, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:
	CComponentEdit();

DECLARE_REGISTRY_RESOURCEID(IDR_COMPONENTEDIT)


BEGIN_COM_MAP(CComponentEdit)
	COM_INTERFACE_ENTRY(IComponentEdit)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CComponentEdit)
	CONNECTION_POINT_ENTRY(__uuidof(_IComponentEditEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_pSharedMemoryOnlineEdit)
		{
			delete m_pSharedMemoryOnlineEdit;
			m_pSharedMemoryOnlineEdit = NULL;
		}
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}

public:

	STDMETHOD(CreateComponent)(BSTR bstrJsonIn, VARIANT* pvRet);
	STDMETHOD(EditComponent)(BSTR bstrJsonIn, VARIANT* pvRet);
	STDMETHOD(CreateForm)(BSTR bstrJsonIn, VARIANT* pvRet);
	STDMETHOD(EditForm)(BSTR bstrJsonIn, VARIANT* pvRet);
	
	STDMETHOD(RunComponentWithForm)(BSTR bstrRunInfo, VARIANT* pvRet);
	STDMETHOD(SaveAsComponent)(BSTR bstrComponentPath, BSTR bstrComponentName, VARIANT* pvRet);
	STDMETHOD(CheckFormParamsChanged)(BSTR bstrParam1, BSTR bstrParam2, VARIANT* pvRet);
	STDMETHOD(CheckParamsChanged)(BSTR bstrParam1, BSTR bstrParam2, VARIANT* pvRet);

	

	int InitParameterMap( string strParamFront,string Params) ;
	BOOL ProcessJsonIn(string strJsonIn, string& strWorkPath, string& strNodeName, string& strRelativeDir, string& strBaseType, string& strFileName, string& strComponentName, string& strParamFilePath, string& strIsProcessForm);
	string ProcessJsonOut(string strWorkPath, string strRelativeDir, string strFileName, string strComponentName, BOOL bParseParams);
	string GetNewGuid(string strOldId);
	string ConvertJsonParamsToXmlParams( string strJsonParams, string strRootName, string strItemName, string strIsProcessForm );
	string ConvertXmlParamsToJsonParams( string strXmlParams, string strRootName, string strItemName );
	string ProcessJsonInOut(string strJsonIn/*网页调用时传入串 */);

	BOOL RunBot( string &strDstPath, string strTitle,string strProcessName,string strTaskName);
	// 以新的命令行运行组件
	BOOL RuntBotNew(string strFrontBot,string &strCommentBot, string strTitle,string strProcessName,string strTaskName);
	string RunBotWithParametersNew(string strFrontBot,string strCompentBotPath, string strInstanceID,string strTitle, string strWorkDir, string strParameters,string strProcessName,string strTaskName);

	string ProcessJsonOut2(string strXmlFile);
	string ConvertJsonParamsToXmlParams2( string strJsonParams, string strRootName, string strItemName, string strInstanceID );
	// 从Parameters运行BOT文件
	string RunBotWithParameters(string strBotPath, string strInstanceID, string strBotType, string strTitle, string strWorkDir, string strParameters,string strProcessName,string strTaskName);
	string ConvertXmlParamsToJsonParams2( string strXmlParams, string strRootName, string strItemName );
	void UpdateJson( Json::Value& jRet, string& strParameters ); //更新Json信息
public:
	SharedMemoryOnlineEdit* m_pSharedMemoryOnlineEdit;
	ULONG_PTR m_gdiplusToken;
	vector<string> m_vFrontFormParamsIn; //前表单进入参数集合
	vector<string> m_vFrontFormParamsOut; //前表单输出参数集合
	
	vector<string> m_vBackFormParamsIn; //后表单进入参数集合
	vector<string> m_vBackFormParamsOut; //后表单输出参数集合

	BOOL IsFrontFormParamsChanged(); //前表单参数是否改变
	BOOL IsBackFormParamsChanged(); //后表单参数是否改变

	map<string ,Json::Value> m_map_Json;
	
	map<string, string> m_mapEditParamsFront;//<strId ,strJson>在修改之前
	map<string, string> m_mapEditParamsBack;//<strId ,strJson>在修改之后
	map<string, int> m_mapModifyBack;//<strId ,strJson>修改后参数列表的顺序
	string SortAllParam(string strParam, int ndelet);
	string GetJsonPara(string strJsonIn, string strPara);
	string EditBackParams( string strXmlParams, string strRootName, string strItemName);
	string LoadParams( string strJsonParams, string strRootName, string strItemName, string strIsProcessForm , string strvalue);
	string ComponentParametersChanged();	//编辑时，对应的id是否改变
	string ComponentParametersChanged(MAPSTR mapEditParamsFront,MAPSTR mapEditParamsBack) /*编辑时，对应的id是 窀谋?*/;
	//string ConstructXml(string strAttribute, string strvalue);
	string LinkJson(string strJson1, string strJson2 );
	TiXmlElement* CComponentEdit::ConstructXml(string strID, string strFlag);
	map<string, Json::Value> m_mapParamIdValue; //参数IDValue映射
	
};

OBJECT_ENTRY_AUTO(__uuidof(ComponentEdit), CComponentEdit)
