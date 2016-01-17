// ComponentEdit.h : CComponentEdit ������

#pragma once
#include "resource.h"       // ������

#include "PeraComponentEdit_i.h"
#include "_IComponentEditEvents_CP.h"
#include "SharedMemoryOnlineEdit.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
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
	string ProcessJsonInOut(string strJsonIn/*��ҳ����ʱ���봮 */);

	BOOL RunBot( string &strDstPath, string strTitle,string strProcessName,string strTaskName);
	// ���µ��������������
	BOOL RuntBotNew(string strFrontBot,string &strCommentBot, string strTitle,string strProcessName,string strTaskName);
	string RunBotWithParametersNew(string strFrontBot,string strCompentBotPath, string strInstanceID,string strTitle, string strWorkDir, string strParameters,string strProcessName,string strTaskName);

	string ProcessJsonOut2(string strXmlFile);
	string ConvertJsonParamsToXmlParams2( string strJsonParams, string strRootName, string strItemName, string strInstanceID );
	// ��Parameters����BOT�ļ�
	string RunBotWithParameters(string strBotPath, string strInstanceID, string strBotType, string strTitle, string strWorkDir, string strParameters,string strProcessName,string strTaskName);
	string ConvertXmlParamsToJsonParams2( string strXmlParams, string strRootName, string strItemName );
	void UpdateJson( Json::Value& jRet, string& strParameters ); //����Json��Ϣ
public:
	SharedMemoryOnlineEdit* m_pSharedMemoryOnlineEdit;
	ULONG_PTR m_gdiplusToken;
	vector<string> m_vFrontFormParamsIn; //ǰ�������������
	vector<string> m_vFrontFormParamsOut; //ǰ�������������
	
	vector<string> m_vBackFormParamsIn; //��������������
	vector<string> m_vBackFormParamsOut; //��������������

	BOOL IsFrontFormParamsChanged(); //ǰ�������Ƿ�ı�
	BOOL IsBackFormParamsChanged(); //��������Ƿ�ı�

	map<string ,Json::Value> m_map_Json;
	
	map<string, string> m_mapEditParamsFront;//<strId ,strJson>���޸�֮ǰ
	map<string, string> m_mapEditParamsBack;//<strId ,strJson>���޸�֮��
	map<string, int> m_mapModifyBack;//<strId ,strJson>�޸ĺ�����б��˳��
	string SortAllParam(string strParam, int ndelet);
	string GetJsonPara(string strJsonIn, string strPara);
	string EditBackParams( string strXmlParams, string strRootName, string strItemName);
	string LoadParams( string strJsonParams, string strRootName, string strItemName, string strIsProcessForm , string strvalue);
	string ComponentParametersChanged();	//�༭ʱ����Ӧ��id�Ƿ�ı�
	string ComponentParametersChanged(MAPSTR mapEditParamsFront,MAPSTR mapEditParamsBack) /*�༭ʱ����Ӧ��id�� �ı?*/;
	//string ConstructXml(string strAttribute, string strvalue);
	string LinkJson(string strJson1, string strJson2 );
	TiXmlElement* CComponentEdit::ConstructXml(string strID, string strFlag);
	map<string, Json::Value> m_mapParamIdValue; //����IDValueӳ��
	
};

OBJECT_ENTRY_AUTO(__uuidof(ComponentEdit), CComponentEdit)
