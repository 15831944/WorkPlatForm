// ComponentEdit.cpp : CComponentEdit ��ʵ��

#include "stdafx.h"
#include "ComponentEdit.h"
#include "Tools.h"
#include <comdef.h>
#include "DlgOpentools.h"
#include "GetFlexwareBin.h"
#include "DlgRunbot.h"
#include "ParameterJson.h"
#include "ConfigFunc.h"
// CComponentEdit

std::string& replace_all(std::string& str,const std::string& old_value,const std::string& new_value) 
{ 
	while(true) { 
		std::string::size_type pos(0); 
		if( (pos=str.find(old_value))!=std::string::npos ) 
			str.replace(pos,old_value.length(),new_value); 
		else break; 
	} 
	return str; 
} 
std::string& replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value) 
{ 
	for(std::string::size_type pos(0); pos!=std::string::npos; pos+=new_value.length()) { 
		if( (pos=str.find(old_value,pos))!=std::string::npos ) 
			str.replace(pos,old_value.length(),new_value); 
		else break; 
	} 
	return str; 
} 
//unicode�еĿո���ʱΪ0xC2A0,תΪ���ֽں��Ϊ�ʺţ�������Ϊ����0xC2A0���滻Ϊ0x0020���������ֽڿ���ʶ��Ϊ�ո�
std::string& UTF8ReplaceBlank(std::string& str)
{
	unsigned char char1 = 0xC2;//0xC2
	unsigned char char2 = 0xA0;//0xA0
	unsigned char char3 = 0x20;//0x20
	std::string strOld;
	strOld.append(1,char1);
	strOld.append(1,char2);
	std::string strNew;
	strNew.append(1,char3);
	replace_all(str, strOld, strNew);
	return str;
}

STDMETHODIMP CComponentEdit::CreateComponent(BSTR bstrJsonIn, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraComponentEdit::CComponentEdit::CreateComponent");
	CComVariant vRet(_bstr_t("").GetBSTR());

	string strJsonIn = _bstr_t(bstrJsonIn);
	ZTools::WriteZToolsFormatLog("strJsonIn:%s", strJsonIn.c_str());

	string strWorkPath;//���̹���Ŀ¼
	string strBaseType;
	string strNodeName;
	string strRelativeDir;//������Ŀ¼
	string strFileName;//�������.bot
	string strComponentName;//�������
	string strParamFilePath;//�������л�ΪXML����ļ�����·�������ڱ�����ʱ�����װ����
	string strIsProcessForm;//�Ƿ����̵�ǰ���


	//��������õ���Ҫ����Ϣ
	if (!ProcessJsonIn(strJsonIn, strWorkPath, strRelativeDir, strBaseType, strNodeName, strFileName, strComponentName, strParamFilePath, strIsProcessForm))
	{
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (strWorkPath.empty()
		|| strFileName.empty()
		|| strBaseType.empty()
		|| strComponentName.empty()
		/*|| strParamFilePath.empty()*/)
	{
		ZTools::WriteZToolsFormatLog("�������ʱ����������");
		vRet.Detach(pvRet);
		return S_OK;
	}

	//�����Ի��򣬰󶨷�װ����
	CDlgOpentools dlg;
	dlg.m_sOpenFlag = "CreateComponent";
	dlg.m_sBaseType = strBaseType.c_str();
	dlg.m_sNodeName = strNodeName.c_str();
	dlg.m_sComponentName = strComponentName.c_str();
	dlg.m_sLocalPath = (strWorkPath + strRelativeDir).c_str();
	dlg.m_sParamsFilePath = strParamFilePath.c_str();
	dlg.m_pSharedMemoryOnlineEdit = m_pSharedMemoryOnlineEdit;

	//test
	//dlg.m_sOpenFlag = "Create";
	//dlg.DoModal();

	if (dlg.DoModal() == IDCANCEL)
	{
		if (dlg.m_bApplied == FALSE)  // ȡ����û��Ӧ��
		{
			vRet.Detach(pvRet);
			return S_OK;
		}
	}

	//�����װ�رպ󣬶������Ϣ���ظ���ҳ
	string strJsonOut = ProcessJsonOut(strWorkPath, strRelativeDir, strFileName, strComponentName, TRUE);
	vRet = _bstr_t(strJsonOut.c_str()).GetBSTR();

	vRet.Detach(pvRet);
	return S_OK;
}

/*
���룺
{
"workPath": "c:\\data\\",
"botPath": "\\F150000\\���1.bot"
}
���ͬ�½����
*/
STDMETHODIMP CComponentEdit::EditComponent(BSTR bstrJsonIn, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraComponentEdit::CComponentEdit::EditComponent");
	CComVariant vRet(_bstr_t("").GetBSTR());

	string strJsonIn = _bstr_t(bstrJsonIn);
	ZTools::WriteZToolsFormatLog("strJsonIn:%s", strJsonIn.c_str());

	string strWorkPath;//���̹���Ŀ¼
	string strBaseType;
	string strNodeName; //�ڵ�����
	string strRelativeDir;//������Ŀ¼
	string strFileName;//�������.bot
	string strComponentName;//�������
	string strParamFilePath;//�������л�ΪXML����ļ�����·�������ڱ�����ʱ�����װ����
	string strIsProcessForm;//�Ƿ����̵�ǰ���
	string strFrontJsonParams;	//�༭ǰ��arams����ֵ
	string strBackJsonParams; //�༭��Ĳ���ֵ


	//��������õ���Ҫ����Ϣ
	if (!ProcessJsonIn(strJsonIn, strWorkPath, strRelativeDir, strBaseType, strNodeName, strFileName, strComponentName, strParamFilePath, strIsProcessForm))
	{
		vRet.Detach(pvRet);
		return S_OK;
	}
	//��ȡ����json�ַ����е�params�����ֵ
	strFrontJsonParams = GetJsonPara(strJsonIn, "Parameters");

	//�����Ѿ����ڲ�����ֵ
	strFrontJsonParams = LoadParams(strFrontJsonParams, "ParametersChanged", "ParameterItem", strIsProcessForm,"front");

	//��������õ���Ҫ����Ϣ
	if (strWorkPath.empty()
		|| strFileName.empty()
		/*|| strBaseType.empty()*/
		|| strComponentName.empty()
		/*|| strParamFilePath.empty()*/)
	{
		ZTools::WriteZToolsFormatLog("�������ʱ����������");
		vRet.Detach(pvRet);
		return S_OK;
	}

	//�����Ի��򣬰󶨷�װ����
	CDlgOpentools dlg;
	dlg.m_sOpenFlag = "EditComponent";
	dlg.m_sBaseType = strBaseType.c_str();
	dlg.m_sNodeName = strNodeName.c_str();
	dlg.m_sLocalRobotPath = (strWorkPath + strRelativeDir + strFileName).c_str();
	dlg.m_sParamsFilePath = strParamFilePath.c_str();
	dlg.m_pSharedMemoryOnlineEdit = m_pSharedMemoryOnlineEdit;

	if (dlg.DoModal() == IDCANCEL)
	{
		if (dlg.m_bApplied==FALSE) // ȡ����û��Ӧ�ù�
		{
			vRet.Detach(pvRet);
			return S_OK;
		}
	}

	string strSubOut;
	string strJsonOut;
	string strk_value;
	string strChanage;
	string strJsonOutChanage;
	int nlengthjson = 0 ;
	Json::Value jsonchange;

	//�����װ�رպ󣬶������Ϣ���ظ���ҳ
	strJsonOut = ProcessJsonOut(strWorkPath, strRelativeDir, strFileName, strComponentName, TRUE);


#ifdef _DEBUG
	///����Chec/kParamsChanged�ӿ�ʹ�õ�///////////////
	string strTempOut = strJsonOut;///////////
	_bstr_t bstr_t(strTempOut.c_str()); ///////////
	BSTR ret_val = bstr_t.GetBSTR();	  ///////////
	CheckParamsChanged(bstrJsonIn,ret_val,pvRet);//
	/////////////////////////////////////////////////
#endif

	strBackJsonParams = GetJsonPara(strJsonOut, "Parameters");
	strBackJsonParams = LoadParams(strBackJsonParams, "ParametersChanged", "ParameterItem", strIsProcessForm,"back");

	if (strBackJsonParams.compare(strFrontJsonParams)!=0)
	{
		//�޸���󣬵�����水ť
		strChanage = ComponentParametersChanged();

		jsonchange["ParametersChanged"] = strChanage;

		ZTools::UTF8ToMB(strChanage);

		//ƴ�ӱ༭���޸ĺ����Ϣ
		//nlengthjson = strJsonOut.length()-2;
		//strSubOut = strJsonOut.substr(0, nlengthjson);
		//strJsonOut = strSubOut + "," + strChanage + "}";
	}
	//ƴ�ӱ༭���޸ĺ�json��
	strJsonOutChanage = LinkJson(strJsonOut, strChanage);

	vRet = (strJsonOutChanage.c_str());	
	vRet.Detach(pvRet);
	ZTools::WriteZToolsFormatLog(">>>>>>CComponentEdit::EditComponent return value is:%s", strJsonOutChanage.c_str());

	return S_OK;
}

string CComponentEdit::LinkJson(string strJson1, string strJson2 )
{
	Json::Value jsonOut;
	Json::Value jsonTurn;

	Json::Reader reader;
	Json::Reader reader1;
	Json::FastWriter writer;

	string strJsonOut;		

	reader1.parse(strJson2,jsonTurn);
	reader.parse(strJson1, jsonOut);
	jsonOut["ParametersChanged"] = jsonTurn;
	strJsonOut = writer.write(jsonOut);              

	return strJsonOut;
}
/*
���룺
{
"baseType": "�����",
"botPath": "\\F150000\\FrontForm.bot",
"workPath": "c:\\data\\",
"Parameters": [
{
"id": "63b06461-816c-4e07-b4e4-86db181a8ac5",
"name": "A",
"value": "1",
"direction": "in",
"type": "Int",
"version": ""
},
{
"id": "63b06461-816c-4e07-b4e4-86db18111111",
"name": "B",
"value": "2",
"direction": "in",
"type": "Int",
"version": ""
}
]
}
�����
{
"botPath": "\\F150000\\���1.bot",
"saveFlag": "save",
"InstanceID": "0caf483e-fe33-4d8e-9cdc-8bcc9b047991"
}
*/
STDMETHODIMP CComponentEdit::CreateForm(BSTR bstrJsonIn, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraComponentEdit::CComponentEdit::CreateForm");
	CComVariant vRet(_bstr_t("").GetBSTR());

	string strJsonIn = _bstr_t(bstrJsonIn);
	ZTools::WriteZToolsFormatLog("strJsonIn:%s", strJsonIn.c_str());

	string strWorkPath;//���̹���Ŀ¼
	string strBaseType;
	string strNodeName;
	string strRelativeDir;//������Ŀ¼
	string strFileName;//�������.bot
	string strComponentName;//�������
	string strParamFilePath;//�������л�ΪXML����ļ�����·�������ڱ�����ʱ�����װ����
	string strIsProcessForm;//�Ƿ����̵�ǰ���

	//��������õ���Ҫ����Ϣ
	if (!ProcessJsonIn(strJsonIn, strWorkPath, strRelativeDir, strBaseType, strNodeName, strFileName, strComponentName, strParamFilePath, strIsProcessForm))
	{
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (strWorkPath.empty()
		|| strFileName.empty()
		|| strBaseType.empty()
		|| strComponentName.empty()
		|| strParamFilePath.empty())
	{
		ZTools::WriteZToolsFormatLog("�������ʱ����������");
		vRet.Detach(pvRet);
		return S_OK;
	}

	//�����Ի��򣬰󶨷�װ����
	CDlgOpentools dlg;
	dlg.m_sOpenFlag = "CreateForm";
	dlg.m_sBaseType = strBaseType.c_str();
	dlg.m_sNodeName = strNodeName.c_str();
	dlg.m_sComponentName = strComponentName.c_str();
	dlg.m_sLocalPath = (strWorkPath + strRelativeDir).c_str();
	dlg.m_sParamsFilePath = strParamFilePath.c_str();
	dlg.m_pSharedMemoryOnlineEdit = m_pSharedMemoryOnlineEdit;

	if (dlg.DoModal() == IDCANCEL)
	{
		if (dlg.m_bApplied == FALSE)  // ȡ����û��Ӧ��
		{
			vRet.Detach(pvRet);
			return S_OK;
		}
	}

	//�����װ�رպ󣬶������Ϣ���ظ���ҳ
	string strJsonOut = ProcessJsonOut(strWorkPath, strRelativeDir, strFileName, strComponentName, FALSE);
	vRet = _bstr_t(strJsonOut.c_str()).GetBSTR();

	vRet.Detach(pvRet);
	return S_OK;
}

/*
���룺
{
"botPath": "\\F150000\\FrontForm.bot",
"workPath": "c:\\data\\",
"Parameters": [
{
"id": "63b06461-816c-4e07-b4e4-86db181a8ac5",
"name": "A",
"value": "1",
"direction": "in",
"type": "Int",
"version": ""
},
{
"id": "63b06461-816c-4e07-b4e4-86db18111111",
"name": "B",
"value": "2",
"direction": "in",
"type": "Int",
"version": ""
}
]
}
�����
{
"botPath": "\\F150000\\���1.bot",
"saveFlag": "save",
"InstanceID": "0caf483e-fe33-4d8e-9cdc-8bcc9b047991"
}
*/
STDMETHODIMP CComponentEdit::EditForm(BSTR bstrJsonIn, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraComponentEdit::CComponentEdit::EditForm");
	CComVariant vRet(_bstr_t("").GetBSTR());

	string strJsonIn = _bstr_t(bstrJsonIn);
	ZTools::WriteZToolsFormatLog("strJsonIn:%s", strJsonIn.c_str());

	string strWorkPath;//���̹���Ŀ¼
	string strBaseType;
	string strNodeName;
	string strRelativeDir;//������Ŀ¼
	string strFileName;//�������.bot
	string strComponentName;//�������
	string strParamFilePath;//�������л�ΪXML����ļ�����·�������ڱ�����ʱ�����װ����
	string strIsProcessForm;//�Ƿ����̵�ǰ���

	//��������õ���Ҫ����Ϣ
	if (!ProcessJsonIn(strJsonIn, strWorkPath, strRelativeDir, strBaseType, strNodeName, strFileName, strComponentName, strParamFilePath, strIsProcessForm))
	{
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (strWorkPath.empty()
		|| strFileName.empty()
		/*|| strBaseType.empty()*/
		|| strComponentName.empty()
		|| strParamFilePath.empty())
	{
		ZTools::WriteZToolsFormatLog("�������ʱ����������");
		vRet.Detach(pvRet);
		return S_OK;
	}

	//�����Ի��򣬰󶨷�װ����
	CDlgOpentools dlg;
	dlg.m_sOpenFlag = "EditForm";
	dlg.m_sBaseType = strBaseType.c_str();
	dlg.m_sNodeName = strNodeName.c_str();
	dlg.m_sLocalRobotPath = (strWorkPath + strRelativeDir + strFileName).c_str();
	dlg.m_sParamsFilePath = strParamFilePath.c_str();
	dlg.m_pSharedMemoryOnlineEdit = m_pSharedMemoryOnlineEdit;

	if (dlg.DoModal() == IDCANCEL)
	{
		if (dlg.m_bApplied == FALSE)  // ȡ����û��Ӧ��
		{
			vRet.Detach(pvRet);
			return S_OK;
		}
	}

	//�����װ�رպ󣬶������Ϣ���ظ���ҳ
	string strJsonOut = ProcessJsonOut(strWorkPath, strRelativeDir, strFileName, strComponentName, FALSE);
	vRet = _bstr_t(strJsonOut.c_str()).GetBSTR();

	vRet.Detach(pvRet);
	return S_OK;
}

string CComponentEdit::GetNewGuid(string strOldId)
{
	// Crypto++ MD5 object
	CryptoPP::Weak::MD5 hash;

	// Use native byte instead of casting chars
	byte digest[CryptoPP::Weak::MD5::DIGESTSIZE];

	// Do the actual calculation, require a byte value so we need a cast
	hash.CalculateDigest(digest, (const byte*)strOldId.c_str(), strOldId.length());

	// Crypto++ HexEncoder object
	CryptoPP::HexEncoder encoder;

	// Our output
	string output;

	// Drop internal hex encoder and use this, returns uppercase by default
	encoder.Attach(new CryptoPP::StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	ZTools::LowerString(output);

	output.insert(20, 1, '-');
	output.insert(16, 1, '-');
	output.insert(12, 1, '-');
	output.insert(8, 1, '-');

	return output;
}
//����ֵΪutf8����
string CComponentEdit::ConvertJsonParamsToXmlParams( string strJsonParams, string strRootName, string strItemName, string strIsProcessForm )
{
	/*
	[
	{
	"id": "63b06461-816c-4e07-b4e4-86db181a8ac5",
	"name": "A",
	"value": "1",
	"direction": "in",
	"type": "Int",
	"version": ""
	}
	]
	<Parameters>
	<ParameterItem id="259e7cef-1dfe-421d-964a-b163feda0d00" name="b" direction="In" type="Double" desc="" format="" value="" listname="" group=""/>
	</Parameters>
	*/
	//strJsonParams = "[]";
	string strXmlParams("");
	Json::Reader reader;
	Json::Value jParams;
	if (!reader.parse(strJsonParams, jParams))
	{
		return strXmlParams;
	}

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement(strRootName);
	doc.LinkEndChild(root);
	TiXmlElement* item;

	int nLength = jParams.size();
	for (int i = 0; i < nLength; i++)
	{
		Json::Value jParam = jParams[Json::Value::ArrayIndex(i)];

		Json::Value::Members JMembers =  jParam.getMemberNames();

		item = new TiXmlElement(strItemName);

		string strID;
		string strName;
		string strDirection;
		string strType;
		string strIsDynamicArray;
		string strIDMap; 

		for(size_t j = 0; j < JMembers.size(); j++)
		{		
			string& str = JMembers[j];
			string strValue = jParam[str].isString() ? jParam[str].asString() : "";
			if (strIsProcessForm.compare("1") == 0 && str == "id")
			{
				item->SetAttribute(str, GetNewGuid(strValue));			
			}
			else
			{
				item->SetAttribute(str, strValue);
			}
			if(str == "id")
			{
				strID = strValue;
				strIDMap = strValue;
			}
			else if(str == "name")
			{
				strName = strValue;
			}
			else if(str == "direction")
			{
				strDirection = strValue;
			}
			else if(str == "type")
			{
				strType = strValue;
			}
			else if(str == "isdynamicarray")
			{
				strIsDynamicArray = strValue;
			}
		}	

		if(strDirection == "In" || strDirection == "InOut")
		{
			m_vFrontFormParamsIn.push_back(move(strID + strName + strType + strIsDynamicArray));

		}
		if(strDirection == "Out" || strDirection == "InOut")
		{
			m_vBackFormParamsIn.push_back(move(strID + strName + strType + strIsDynamicArray));
		}

		root->LinkEndChild(item);
		//strXmlParams << doc;
		//m_mapEditParamsBack.insert(pair<string, string>(strIDMap,strXmlParams));

	}

	strXmlParams << doc;
	ZTools::WriteZToolsLog(strXmlParams);
	ZTools::MBToUTF8(strXmlParams);
	return strXmlParams;
}
string CComponentEdit::GetJsonPara(string strJsonIn, string strPara)
{
	string strJsonParams;
	Json::FastWriter writer;
	Json::Value jIn;
	Json::Reader reader;
	if (!reader.parse(strJsonIn, jIn))
	{
		ZTools::WriteZToolsFormatLog("�����������ʧ��");
		return strJsonParams;
	}

	//	strPara  = "\"" + strPara + "\"";
	Json::Value jParams = jIn[strPara];
	if (jParams.type() == Json::arrayValue)
	{
		strJsonParams = writer.write(jParams);
	}
	return strJsonParams;
}



//����ֵΪutf8����
string CComponentEdit::ConvertJsonParamsToXmlParams2( string strJsonParams, string strRootName, string strItemName, string strInstanceID )
{
	/*
	[
	{
	"id": "63b06461-816c-4e07-b4e4-86db181a8ac5",
	"name": "A",
	"value": "1",
	"direction": "in",
	"type": "Int",
	"version": ""
	}
	]
	<Parameters>
	<Package InstanceID="3fee8b86-1522-419f-a6fd-9b39378dd2ed" IsPopupForm="True">
	<ParameterItem id="259e7cef-1dfe-421d-964a-b163feda0d00" name="b" type="Double" desc="" format="" value="" listname="" group=""/>
	</Package>
	</Parameters>
	*/
	//strJsonParams = "[]";
	string strXmlParams("");
	Json::Reader reader;
	Json::Value jParams;
	if (!reader.parse(strJsonParams, jParams))
	{
		return strXmlParams;
	}

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement(strRootName);
	doc.LinkEndChild(root);
	TiXmlElement* package = new TiXmlElement("Package");
	package->SetAttribute("InstanceID", strInstanceID);
	package->SetAttribute("IsPopupForm", "True");
	root->LinkEndChild(package);

	TiXmlElement* item;

	int nLength = jParams.size();
	for (int i = 0; i < nLength; i++)
	{
		Json::Value jParam = jParams[Json::Value::ArrayIndex(i)];
		Json::Value::Members JMembers =  jParam.getMemberNames();

		item = new TiXmlElement(strItemName);

		for(size_t j = 0; j < JMembers.size(); j++)
		{
			string& str = JMembers[j];

			item->SetAttribute(str, jParam[str].isString() ? jParam[str].asString() : "");
		}	
		package->LinkEndChild(item);
	}

	strXmlParams << doc;
	ZTools::WriteZToolsLog(strXmlParams);
	ZTools::MBToUTF8(strXmlParams);
	return strXmlParams;
}

//����Ϊutf8�����ض��ֽ�
string CComponentEdit::ConvertXmlParamsToJsonParams( string strXmlParams, string strRootName, string strItemName )
{
	string strJsonParams("[]");
	Json::Value jsonParams(Json::arrayValue);

	TiXmlBase::SetCondenseWhiteSpace(false);
	istringstream iss(strXmlParams);
	TiXmlElement rootElement("");
	iss >> rootElement;
	if (strRootName.compare(rootElement.ValueStr()) != 0)
	{
		return strJsonParams;
	}
	TiXmlElement* pItemElement = rootElement.FirstChildElement(strItemName);
	while (pItemElement)
	{
		Json::Value jsonParam;
		string strValue("");
		string strName;
		string strChangeFlag;
		string strID;
		string strNameA;
		string strDirection;
		string strType;
		string strIsDynamicArray;
		TiXmlAttribute* pAb;

		string strSubxml ;
		strSubxml<< *pItemElement;
		pAb = pItemElement->FirstAttribute();
		while (pAb)
		{

			strName = pAb->NameTStr();
			strValue = pAb->ValueStr();
			ZTools::UTF8ToMB(strName);
			//unicode�еĿո���ʱΪ0xC2A0,תΪ���ֽں��Ϊ�ʺţ�������Ϊ����0xC2A0���滻Ϊ0x0020���������ֽڿ���ʶ��Ϊ�ո�
			UTF8ReplaceBlank(strValue);
			ZTools::UTF8ToMB(strValue);
			jsonParam[strName] = strValue;
			pAb = pAb->Next();
			string& str = strName;
			if(str == "id")
			{
				strID = strValue;
			}
			else if(str == "name")
			{
				strNameA = strValue;
			}
			else if(str == "direction")
			{
				strDirection = strValue;
			}
			else if(str == "type")
			{
				strType = strValue;
			}
			else if(str == "isdynamicarray")
			{
				strIsDynamicArray = strValue;
			}

		}

		if(strDirection == "In" || strDirection == "InOut")
		{
			m_vFrontFormParamsOut.push_back(move(strID + strNameA + strType + strIsDynamicArray));
		}
		if(strDirection == "Out" || strDirection == "InOut")
		{
			m_vBackFormParamsOut.push_back(move(strID + strNameA + strType + strIsDynamicArray));
		}
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("id", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["id"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("name", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["name"] = Json::Value(strValue);
		// 		
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("value", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["value"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("direction", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["direction"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("type", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["type"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("version", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["version"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("isdynamicarray", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["isdynamicarray"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("group", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["group"] = Json::Value(strValue);

		jsonParams.append(jsonParam);

		pItemElement = pItemElement->NextSiblingElement(strItemName);
	}

	Json::FastWriter writer;
	strJsonParams = writer.write(jsonParams);
	//ZTools::WriteZToolsLog(strJsonParams);

	return strJsonParams;
}

//����Ϊutf8�����ض��ֽ�
string CComponentEdit::ConvertXmlParamsToJsonParams2( string strXmlParams, string strRootName, string strItemName )
{
	string strJsonParams("[]");
	Json::Value jsonParams(Json::arrayValue);

	TiXmlBase::SetCondenseWhiteSpace(false);
	istringstream iss(strXmlParams);
	TiXmlElement rootElement("");
	iss >> rootElement;
	if (strRootName.compare(rootElement.ValueStr()) != 0)
	{
		return strJsonParams;
	}
	TiXmlElement* pItemElement = rootElement.FirstChildElement(strItemName);
	while (pItemElement)
	{
		Json::Value jsonParam;
		string strValue("");
		string strName;

		TiXmlAttribute* pAb;
		pAb = pItemElement->FirstAttribute();
		while (pAb)
		{
			strName = pAb->NameTStr();
			strValue = pAb->ValueStr();
			ZTools::UTF8ToMB(strName);
			//unicode�еĿո���ʱΪ0xC2A0,תΪ���ֽں��Ϊ�ʺţ�������Ϊ����0xC2A0���滻Ϊ0x0020���������ֽڿ���ʶ��Ϊ�ո�
			UTF8ReplaceBlank(strValue);
			ZTools::UTF8ToMB(strValue);
			jsonParam[strName] = strValue;
			pAb = pAb->Next();
		}

		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("id", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["id"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("name", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["name"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("value", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["value"] = Json::Value(strValue);

		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("direction", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["direction"] = Json::Value(strValue);

		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("type", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["type"] = Json::Value(strValue);
		// 
		// 		strValue.clear();
		// 		pItemElement->QueryStringAttribute("version", &strValue);
		// 		ZTools::UTF8ToMB(strValue);
		// 		jsonParam["version"] = Json::Value(strValue);

		jsonParams.append(jsonParam);

		pItemElement = pItemElement->NextSiblingElement(strItemName);
	}

	Json::FastWriter writer;
	strJsonParams = writer.write(jsonParams);
	//ZTools::WriteZToolsLog(strJsonParams);

	return strJsonParams;
}


/*
���룺
{
"baseType": "�����",
"botPath": "\\F150000\\FrontForm.bot",
"workPath": "c:\\data\\",
"Parameters": [
{
"id": "63b06461-816c-4e07-b4e4-86db181a8ac5",
"name": "A",
"value": "1",
"direction": "in",
"type": "Int",
"version": ""
},
{
"id": "63b06461-816c-4e07-b4e4-86db18111111",
"name": "B",
"value": "2",
"direction": "in",
"type": "Int",
"version": ""
}
]
}
�м���ṩ�Ĵ������༭���ӿڵĲ���ΪJSON��ʽ������һ������isProcessForm����ֵΪ�ַ�����"1"ʱ����ʾ��ǰ�����̱�����������ʾ�������
*/
BOOL CComponentEdit::ProcessJsonIn( string strJsonIn, string& strWorkPath, string& strRelativeDir, string& strBaseType, string& strNodeName, string& strFileName, string& strComponentName, string& strParamFilePath, string& strIsProcessForm )
{
	Json::Value jIn;
	Json::Reader reader;
	if (!reader.parse(strJsonIn, jIn))
	{
		ZTools::WriteZToolsFormatLog("�����������ʧ��");
		return FALSE;
	}

	strIsProcessForm = jIn["isProcessForm"].asString();
	strBaseType = jIn["baseType"].asString();
	strNodeName = jIn["nodeName"].asString();
	strWorkPath = jIn["workPath"].asString();

	string strBotPath = jIn["botPath"].asString();

	replace(strWorkPath.begin(), strWorkPath.end(), '/', '\\');
	strWorkPath.erase(strWorkPath.find_last_not_of('\\') + 1);
	strWorkPath += "\\";
	replace(strBotPath.begin(), strBotPath.end(), '/', '\\');
	strBotPath.erase(0, strBotPath.find_first_not_of('\\'));

	size_t nPos = strBotPath.find_last_of('\\');
	if (nPos == string::npos)
	{
		strRelativeDir = "";
		strFileName = strBotPath;
	}
	else
	{
		strRelativeDir = strBotPath.substr(0, nPos + 1);
		strFileName = strBotPath.substr(nPos + 1);
	}
	strComponentName = strFileName.substr(0, strFileName.find_last_of('.'));

	Json::Value jParams = jIn["Parameters"];
	if (jParams.type() == Json::arrayValue)
	{
		//д����xml�ļ�
		Json::FastWriter writer;
		string strJsonParams = writer.write(jParams);

		//		//string strsubxmlValue = LoadFrontParams(strJsonParams,"ParametersChanged", "ParameterItem",strIsProcessForm);

		string strXmlParams = ConvertJsonParamsToXmlParams(strJsonParams, "Parameters", "ParameterItem", strIsProcessForm);
		string strDeclaration("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
		strParamFilePath = ZTools::FormatString("%s%stemp\\Params%s.xml", strWorkPath.c_str(), strRelativeDir.c_str(), ZTools::GetCurrentTimeString16().c_str());
		if (!MakeSureDirectoryPathExists(strParamFilePath.c_str()))
		{
			ZTools::WriteZToolsFormatLog("�������������ļ����ļ���ʧ��");
			strParamFilePath = "";
			return FALSE;
		}

		//locale::global(locale(""));
		ofstream fOut(strParamFilePath);
		if (fOut == NULL || fOut.is_open() == false)
		{
			ZTools::WriteZToolsFormatLog("д���ļ� %s ʧ��", strParamFilePath.c_str());
			strParamFilePath = "";
			return FALSE;
		}
		fOut << strDeclaration;
		fOut << strXmlParams;
		fOut.close();
	}

	return TRUE;
}
//lc,20140415,������ʱ�����������������ٰ���������////////////////////////////////////////////////////////////////////////
static std::string ConvertToDirectionName(const Json::Value& j)
{
	std::string strName = j["name"].asString();
	std::string strDirection = j["direction"].asString();
	ZTools::LowerString(strDirection);
	if (strDirection.compare("in") == 0)
	{
		strDirection = "1";
	}
	else if (strDirection.compare("out") == 0)
	{
		strDirection = "2";
	}
	else if (strDirection.compare("inout") == 0)
	{
		strDirection = "3";
	}
	else
	{
		strDirection = "0";
	}

	std::string strReturn = strDirection + strName;
	return strReturn;
}

//lc,20140326,������ʱ����������������////////////////////////////////////////////////////////////////////////
static bool localeLessThan (const Json::Value& j1, const Json::Value& j2) 
{ 
	std::string s1 = ConvertToDirectionName(j1);
	std::string s2 = ConvertToDirectionName(j2);
	if (strcoll(s1.c_str(), s2.c_str()) <0)
		return true;
	else
		return false;
} 

static void SortParamJson(Json::Value& jsonParams)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs"); 

	vector<Json::Value> vJson;
	int nCount = jsonParams.size();
	for (int i = 0; i < nCount; i++)
	{
		vJson.push_back(jsonParams[Json::Value::ArrayIndex(i)]);
	}

	// 	for (UINT i = 0; i < vJson.size(); ++i)
	// 	{
	// 		ZTools::WriteZToolsLog(vJson[i]["name"].asString());
	// 	}

	std::sort(vJson.begin(), vJson.end(), localeLessThan);

	jsonParams.clear();
	for (UINT i = 0; i < vJson.size(); ++i)
	{
		//ZTools::WriteZToolsLog(vJson[i]["name"].asString());
		jsonParams.append(vJson[i]);
	}

	setlocale(LC_ALL, curLocale.c_str());
}
//////////////////////////////////////////////////////////////////////////
/*
�����
{
"botPath": "\\F150000\\���1.bot",
"saveFlag": "save",
"InstanceID": "0caf483e-fe33-4d8e-9cdc-8bcc9b047991",
"Parameters": [
{
"id": "63b06461-816c-4e07-b4e4-86db181a8ac5",
"name": "A",
"value": "1",
"direction": "in",
"type": "Int",
"version": ""
},
{
"id": "63b06461-816c-4e07-b4e4-86db18111111",
"name": "B",
"value": "2",
"direction": "in",
"type": "Int",
"version": ""
},
{
"id": "63b06461-816c-4e07-b4e4-86db133232323",
"name": "C",
"value": "1",
"direction": "out",
"type": "Int",
"version": ""
}
]
}
*/
string CComponentEdit::ProcessJsonOut(string strWorkPath, //���̹���·��
	string strRelativeDir, //����ļ���
	string strFileName, //�ļ���
	string strComponentName, //�����
	BOOL bParseParams//�Ƿ����ɲ�����JSON����
	)
{
	string strJsonOut("");
	string strRobotRelativePath = "\\" + strRelativeDir + strFileName;
	string strRobotFullPath = strWorkPath + strRelativeDir + strFileName;

	if (!PathFileExists(strRobotFullPath.c_str()))
	{
		ZTools::WriteZToolsFormatLog("%s ������", strRobotFullPath.c_str());
		return strJsonOut;
	}

	string strManifestPath = ZTools::FormatString("%s%s%s\\%s\\Manifest.xml", strWorkPath.c_str(), strRelativeDir.c_str(), strComponentName.c_str(), strComponentName.c_str());
	//string strManifestPath = "E:\\pera131\\gs\\Manifest.xml";
	if (!PathFileExists(strManifestPath.c_str()))
	{
		ZTools::WriteZToolsFormatLog("%s ������", strManifestPath.c_str());
		return strJsonOut;
	}

	string strInstanceID("");
	string strJsonParams("");

	string strRobotName;
	string strAuthor;
	string strCreated;
	string strVersion;
	string strDescription;

	map<string, string>  mParameters; //��������
	Json::Reader reader;
	Json::Value jsonParams;
	Json::FastWriter writer;
	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;
	doc.LoadFile(strManifestPath, TIXML_ENCODING_UTF8);
	if (doc.Error())
	{
		ZTools::WriteZToolsFormatLog("����manifest����");
		return strJsonOut;
	}
	TiXmlElement* rootElement = doc.RootElement();//Manifest
	if (rootElement)
	{
		TiXmlElement* pComponentElement = rootElement->FirstChildElement("Component");
		if (pComponentElement)
		{
			pComponentElement->QueryStringAttribute("RobotName", &strRobotName);
			//��id��Ϊinstanceid
			TiXmlElement* pPropertiesElement = pComponentElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				TiXmlElement* pPropertyElement = pPropertiesElement->FirstChildElement("Property");
				while (pPropertyElement)
				{
					string strName;
					pPropertyElement->QueryStringAttribute("name", &strName);

					if (strName.compare("ID") == 0)
					{
						const char* sID = pPropertyElement->GetText();
						if (sID)
						{
							strInstanceID = sID;
						}
						//break;
					}
					else if (strName.compare("Author") == 0)
					{
						const char* s = pPropertyElement->GetText();
						if (s)
						{
							strAuthor = s;
						}
					}
					else if (strName.compare("CreateTime") == 0)
					{
						const char* s = pPropertyElement->GetText();
						if (s)
						{
							strCreated = s;
						}
					}
					else if (strName.compare("Version") == 0)
					{
						const char* s = pPropertyElement->GetText();
						if (s)
						{
							strVersion = s;
						}
					}
					else if (strName.compare("Description") == 0)
					{
						const char* s = pPropertyElement->GetText();
						if (s)
						{
							strDescription = s;
						}
					}
					else if(!strName.empty())
					{
						const char* s = pPropertyElement->GetText();
						if (s)
						{
							mParameters[strName] = s;
						}
					}
					pPropertyElement = pPropertyElement->NextSiblingElement("Property");
				}
			}

			if (bParseParams == TRUE)
			{
				//��������Ϊ�ַ������뺯��תΪJSON��
				TiXmlElement* pParametersElement = pComponentElement->FirstChildElement("Parameters");
				if (pParametersElement)
				{
					string strXmlParams;
					strXmlParams << *pParametersElement;
					strJsonParams = ConvertXmlParamsToJsonParams(strXmlParams, "Parameters", "ParameterItem");
					/////////////add by zhaoyw//////////////////////////
					//lc,20140326,��װ��MANIFEST���ѶԲ���������////////////////////////////////////////////////////////////////////////
					if (reader.parse(strJsonParams, jsonParams))
					{
						SortParamJson(jsonParams);
						// 						ZTools::WriteZToolsFormatLog("before sort strJsonParams is %s", strJsonParams.c_str());
						// 						//�Բ�����������
						// 						CParameterJson paraJson;
						// 						paraJson.JsonParamSort(jsonParams);
						strJsonParams = writer.write(jsonParams);
						// 						ZTools::WriteZToolsFormatLog("strJsonParams have sorted: %s", strJsonParams.c_str());
					}
					//////////////////////////////////////////////////////
					//string strk_value = EditBackParams(strXmlParams, "Parameters", "ParameterItem");
				}
			}
		}
	}

	Json::Value jsonOut;
	jsonOut["botPath"] = Json::Value(strRobotRelativePath);
	jsonOut["saveFlag"] = Json::Value("save");

	ZTools::UTF8ToMB(strInstanceID);
	ZTools::UTF8ToMB(strRobotName);
	ZTools::UTF8ToMB(strAuthor);
	ZTools::UTF8ToMB(strCreated);
	ZTools::UTF8ToMB(strVersion);
	ZTools::UTF8ToMB(strDescription);

	jsonOut["InstanceID"] = Json::Value(strInstanceID);
	jsonOut["Name"] = Json::Value(strRobotName);
	jsonOut["Author"] = Json::Value(strAuthor);
	jsonOut["Created"] = Json::Value(strCreated);
	jsonOut["Version"] = Json::Value(strVersion);
	jsonOut["Description"] = Json::Value(strDescription);

	//  �޸��жϱ��Ƿ�ʧЧ�߼�
	string strIsFormParamsChanged = "false";
	if (IsFrontFormParamsChanged() || IsBackFormParamsChanged() )
		strIsFormParamsChanged = "true";

	jsonOut["IsFormParamsChanged"] = strIsFormParamsChanged;
	m_vFrontFormParamsIn.clear();
	m_vFrontFormParamsOut.clear();
	m_vBackFormParamsIn.clear();
	m_vBackFormParamsOut.clear();

	map<string, string>::iterator  i;
	for(i = mParameters.begin(); i != mParameters.end(); i++)
	{
		string strName = i->first;
		string strValue = i->second;
		ZTools::UTF8ToMB(strName);
		ZTools::UTF8ToMB(strValue);
		//jsonOut[strName] = strValue;
	}

	if (bParseParams == TRUE)
	{

		if (reader.parse(strJsonParams, jsonParams))
		{
			////�ļ����Ͳ���Ҫ����Ϊ����·��
			//int nLength = jsonParams.size();
			//for (int i = 0; i < nLength; i++)
			//{
			//	Json::Value jsonParam = jsonParams[Json::Value::ArrayIndex(i)];
			//	string strType = jsonParam["type"].asString();
			//	string strValue = jsonParam["value"].asString();
			//	if (strType.compare("File") == 0)
			//	{
			//		if (!strValue.empty() && (strValue.find(':') == string::npos))
			//		{
			//			//�����Ϊ���Ҳ��Ǿ���·������ƴΪ����·��
			//			strValue = ZTools::FormatString("%s%s%s\\%s\\Reference\\%s"
			//				, strWorkPath.c_str(), strRelativeDir.c_str(), strComponentName.c_str(), strComponentName.c_str(), strValue.c_str());
			//			
			//			//�ļ����ڲŸ�дvalueֵ
			//			if (PathFileExists(strValue.c_str()))
			//			{
			//				jsonParam["value"] = Json::Value(strValue);
			//				jsonParams[Json::Value::ArrayIndex(i)] = jsonParam;
			//			}
			//		}
			//	}
			//}
			jsonOut["Parameters"] = jsonParams;
		}
	}


	strJsonOut = writer.write(jsonOut);
	ZTools::WriteZToolsLog(strJsonOut);

	return strJsonOut;
}



/*
���룺
{
"front": {
"InstanceID": "26832afe-0ff6-433f-80a4-08cd745c9568",
"Path": "E:\\runbot\\ǰ�����.bot"
"Title": "1ǰ��"
},
"component": {
"InstanceID": "3fee8b86-1522-419f-a6fd-9b39378dd2ed",
"Path": "E:\\runbot\\���F358332.bot"
},
"back": {
"InstanceID": "8926b406-6cdd-461a-a9bb-ece627cd05f0",
"Path": "E:\\runbot\\������.bot"
"Title": "1���"
},
"Parameters": [
{
"id": "dd020620-70d9-d745-a57e-d5944726f6e3",
"name": "dse_txt",
"value": "E:\\runbot\\dse.txt",
"type": "File",
"version": ""
}
]
}
*/
//����Json�ַ���������ִ��ǰ��bot-->���bot-->���bot-->out.xml�ļ�-->���Json�ַ���
string CComponentEdit::ProcessJsonInOut( string strJsonIn/*��ҳ����ʱ���봮 */ )
{
	string strRet;
	Json::Value jIn;
	Json::Reader reader;
	Json::FastWriter writer;
	if (!reader.parse(strJsonIn, jIn))
	{
		ZTools::WriteZToolsFormatLog("�����������ʧ��");
		return strRet;
	}
	/////////////////////////////////////����Json
	Json::Value& jfront = jIn["front"];
	Json::Value& jcomponent = jIn["component"];
	Json::Value& jback = jIn["back"];
	Json::Value& jParams = jIn["Parameters"];
	string strNodeName = jIn["nodeName"].isString() ? jIn["nodeName"].asCString(): "TempNode"; 
	string strIsPopForm = jIn["isPopForm"].isString()? jIn["isPopForm"].asString():"true";
	string strProcessName = jIn["processName"].isString()? jIn["processName"].asString():"";
	string strBotPath;   //bot·��
	string strInstanceID; //ID
	string strBotType;  //bot�������
	string strTitle;      //����Title
	string strWorkDir;  //��������Ŀ¼
	string strParameters; //Json�ַ���������ParameterItem��Ϣ
	Json::Value jRet;
	jRet["Parameters"]= jParams;
	{//��ʼ��map
		m_mapParamIdValue.clear();
		for(unsigned int i = 0; i < jParams.size(); i++)
		{
			m_mapParamIdValue[jParams[i]["id"].asString()] = jParams[i];
		}
	}
	/////////////////////////////////////����Json

	//////////////////////////////////////////////////////////////////////////
	//��ȡ������Ϣ
	// 	Json::Value jv;
	// 	jv["Parameters"] = jParams;
	strParameters = writer.write(jRet);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//��������Ŀ¼
	strWorkDir = GetConfig("Pera","RunCache");
	if (strWorkDir.empty())
		strWorkDir = "C:\\WorkSpace\\Data";

	strWorkDir +="\\ModelRunCache\\RunSingle\\"; // ��ģ�У���������Ҽ�ִ�еĻ���·��

	SYSTEMTIME systime;
	GetLocalTime(&systime);

	string  strTime;
	ZTools::FormatString(strTime,"_%04d%02d%02d_%02d.%02d.%02d.%02d"
		, systime.wYear
		, systime.wMonth
		, systime.wDay
		, systime.wHour
		, systime.wMinute
		, systime.wSecond
		, systime.wMilliseconds);

	strWorkDir = strWorkDir + strNodeName + strTime + '\\';

	if(MakeSureDirectoryPathExists(strWorkDir.c_str()) == FALSE)
	{
		ZTools::WriteZToolsFormatLog("����Ŀ¼����ʧ��");
		ZTools::WriteZToolsFormatLog(strWorkDir.c_str());
		return strRet;
	}
	BOOL bRunNewCmd = FALSE;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//front
	if(jfront.type() != Json::nullValue && strIsPopForm=="true")
	{//��Ч����
		bRunNewCmd = TRUE; // ��front��isPopForm����ֵΪtrueʱ�����µ�����ִ�У�����ִֻ��component 
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//component
	if(jcomponent.type() != Json::nullValue && !bRunNewCmd) // �Ծɵ������ʽִ��component
	{//��Ч����
		strBotPath = jcomponent["Path"].asString();
		strInstanceID = jcomponent["InstanceID"].asString(); //�����
		strBotType = "component";
		strTitle = jcomponent["Title"].asString();
		strParameters = RunBotWithParameters(strBotPath, strInstanceID, strBotType, strTitle, strWorkDir, strParameters,strProcessName,strNodeName); //ִ��
		UpdateJson(jRet, strParameters);
	}
	else if (jcomponent.type() != Json::nullValue && bRunNewCmd)
	{
		string strFrontBot    = jfront["Path"].asString(); // ǰ��bot�ļ�
		string strCommentBotPath = jcomponent["Path"].asString(); // ���bot�ļ�
		strInstanceID = jfront["InstanceID"].asString(); // ǰ����
		strTitle = jfront["Title"].asString(); // ǰ������
		strParameters = RunBotWithParametersNew(strFrontBot,strCommentBotPath, strInstanceID, strTitle, strWorkDir, strParameters,strProcessName,strNodeName); //ִ��
		UpdateJson(jRet, strParameters);
	}
	if(jRet.type() != Json::objectValue)
	{
		ZTools::WriteZToolsFormatLog("Json������Json::objectValue����");
		return strRet;
	}

	jRet["paramList"] = jRet.removeMember("Parameters");
	jRet["componentIdList"] = Json::Value(Json::arrayValue);
	Json::Value jv;
	jv["id"] = jcomponent["Id"].isString() ? jcomponent["Id"].asString() : "";
	jv["status"] = jRet.removeMember("runstate");
	jRet["componentIdList"].append(jv);

	//////////////////////////////////////////////////////////////////////////
	//���������ʧ��ʱ����������������״̬ΪFail��ͬʱ���в���ִ�н��������
	if (jv["status"].asString().compare("Fail") == 0)
		jRet.removeMember("paramList");

	strRet = writer.write(jRet);
	return strRet;
}


/*
�����
{
"Parameters": [
{
"id": "fd0bbfb0-f710-47c4-bc04-5073814dc91a",
"name": "a",
"value": "Navigate",
"type": "String",
"version": ""
}
]
}

*/
//��Xml�ļ�����������ݵ�Json�ַ���
string CComponentEdit::ProcessJsonOut2(string strXmlFile /*  "E:\\runbot\\temp\\out.xml"  */ )
{
	string strJsonOut("");

	if (!PathFileExists(strXmlFile.c_str()))
	{
		ZTools::WriteZToolsFormatLog("%s ������", strXmlFile.c_str());
		return strJsonOut;
	}

	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument doc;
	doc.LoadFile(strXmlFile, TIXML_ENCODING_UTF8);
	if (doc.Error())
	{
		ZTools::WriteZToolsFormatLog("����%s����", strXmlFile.c_str());
		return strJsonOut;
	}

	Json::FastWriter writer;
	TiXmlElement* rootElement = doc.RootElement();
	if (rootElement)
	{
		TiXmlElement* pRobotOutputElement = rootElement;
		if (pRobotOutputElement)
		{
			TiXmlElement* pRobotWfRunInfoElement = pRobotOutputElement->FirstChildElement("RobotWfRunInfo");
			if(pRobotWfRunInfoElement)
			{
				std::string strRunState;
				pRobotWfRunInfoElement->QueryStringAttribute("runstate", &strRunState);
				ZTools::WriteZToolsFormatLog("out.xml--<RobotOutput>--<pRobotWfRunInfo>--runstate:%s", strRunState.c_str());
				if(strRunState != "Close")
				{//����ʧ�ܷ���
					ZTools::WriteZToolsFormatLog("out.xml--<RobotOutput>--<pRobotWfRunInfo>--runstate������Close");
					Json::Value jv2;
					jv2["runstate"] = Json::Value(strRunState);
					strJsonOut = writer.write(jv2); 
					//�Ƚ�����״̬��Ϣ���棬��ֹû��Parameters�ڵ�ʱ���ؿ��ַ���������ʧ��Ҳû�д�����ʾPera13.1-805 PERAS-2338��
					//return strJsonOut;
				}
			}
			else
			{//�ڵ㲻����
				return strJsonOut;
			}
			//��������Ϊ�ַ������뺯��תΪJSON��
			TiXmlElement* pParametersElement = pRobotOutputElement->FirstChildElement("Parameters");
			if (pParametersElement)
			{
				string strXmlParams;
				strXmlParams << *pParametersElement;
				strJsonOut = ConvertXmlParamsToJsonParams2(strXmlParams, "Parameters", "ParameterItem");

				Json::Reader reader;
				Json::Value jv;
				Json::Value jv2;
				std::string strRunState;
				pRobotWfRunInfoElement->QueryStringAttribute("runstate", &strRunState);
				jv2["runstate"] = Json::Value(strRunState);
				if (!reader.parse(strJsonOut, jv, false))
				{//ת��ʧ��
					strJsonOut = writer.write(jv2);
					return strJsonOut;
				}

				jv2["Parameters"] = jv;
				strJsonOut = writer.write(jv2);
			}
		}
	}

	//ZTools::WriteZToolsLog(strJsonOut);
	return strJsonOut;
}

//�������
STDMETHODIMP CComponentEdit::RunComponentWithForm(BSTR bstrRunInfo, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComVariant vRet(_bstr_t("").GetBSTR());

	string strJsonIn = _bstr_t(bstrRunInfo);
	ZTools::WriteZToolsFormatLog("strRunInfo:%s", strJsonIn.c_str());

	DlgRunbot dlg(NULL, this, strJsonIn); //�����Ի���
	dlg.DoModal();
	ZTools::WriteZToolsFormatLog("RunComponentWithForm return:\n%s", dlg.m_strRet.c_str());
	vRet = _bstr_t(dlg.m_strRet.c_str()).GetBSTR(); //��ȡ������
	//vRet = ProcessJsonInOut(strJsonIn).c_str();

	vRet.Detach(pvRet);
	return S_OK;
}

//ִ��BOT���������ΪBOT·��
BOOL CComponentEdit::RunBot( string &strBotFile /*"E:\\runbot\\temp\\���.bot"*/, string strTitle /*"ǰ��"*/,string strProcessName,string strTaskName)
{
	{//ִ��BOT�ļ�
		CString sOpentoolsBinPath = "";
		CString sOpentoolsPath = GetGRCRunnerPath();
		CString sParam;
		if(strTitle.empty())
		{
			sParam.Format(('\"' + strBotFile + '\"').c_str());
		}
		else
		{//��Ϊ�յĻ�����Title
			sParam.Format(('\"' + strBotFile + '\"' + " -title " + '\"' + strTitle +'\"' ).c_str());
		}	

		sParam += " -showForm no";
		CString strOther;
		strOther.Format(" -processName \"%s\" -taskName \"%s\"",strProcessName.c_str(),strTaskName.c_str());
		sParam += strOther;

		//����
		SHELLEXECUTEINFO si;
		memset(&si, NULL, sizeof(si));
		si.cbSize = sizeof(si);
		//si.nShow = SW_HIDE;
		si.fMask = SEE_MASK_NOCLOSEPROCESS;
		si.lpVerb = _T("open");
		si.lpDirectory = sOpentoolsBinPath;
		si.lpFile = sOpentoolsPath;
		si.lpParameters = sParam;
		if (!ShellExecuteEx(&si))
		{
			ZTools::WriteZToolsLog(ZTools::FormatString("%sִ��%sʧ��", sOpentoolsPath, sParam));
			return FALSE;
		}

		if (si.hProcess != 0) 
		{ //������Ч
			DWORD dwPid = ::GetProcessId(si.hProcess);
			ZTools::WriteZToolsLog(ZTools::FormatString("ִ��BOT���������%d", dwPid));
			WaitForSingleObject(si.hProcess, INFINITE);              
			CloseHandle(si.hProcess);          
		} 
		else
		{
			ZTools::WriteZToolsLog(ZTools::FormatString("���̴���ʧ��"));
			return FALSE;
		}
		return TRUE;
	}
}
BOOL CComponentEdit::RuntBotNew(string strFrontBot,string &strCommentBot, string strTitle,string strProcessName,string strTaskName)
{
	//ִ��BOT�ļ�
	CString sOpentoolsBinPath = "";
	CString sOpentoolsPath = GetGRCRunnerPath();
	CString sParam;
	sParam.Format("\"%s\" -attachType 01 -attachData \"%s\" -title \"%s\" -showForm no -processName \"%s\" -taskName \"%s\""
		,strFrontBot.c_str(),strCommentBot.c_str(),strTitle.c_str(),strProcessName.c_str(),strTaskName.c_str());

	//����
	SHELLEXECUTEINFO si;
	memset(&si, NULL, sizeof(si));
	si.cbSize = sizeof(si);
	//si.nShow = SW_HIDE;
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.lpVerb = _T("open");
	si.lpDirectory = sOpentoolsBinPath;
	si.lpFile = sOpentoolsPath;
	si.lpParameters = sParam;
	if (!ShellExecuteEx(&si))
	{
		ZTools::WriteZToolsLog(ZTools::FormatString("%sִ��%sʧ��", sOpentoolsPath, sParam));
		return FALSE;
	}

	if (si.hProcess != 0) 
	{ //������Ч
		DWORD dwPid = ::GetProcessId(si.hProcess);
		ZTools::WriteZToolsLog(ZTools::FormatString("ִ��BOT���������%d", dwPid));
		WaitForSingleObject(si.hProcess, INFINITE);              
		CloseHandle(si.hProcess);          
	} 
	else
	{
		ZTools::WriteZToolsLog(ZTools::FormatString("���̴���ʧ��"));
		return FALSE;
	}
	return TRUE;
}
string CComponentEdit::ComponentParametersChanged()	//�༭ʱ����Ӧ��id�Ƿ�ı�
{
	string strOutJson;
	string strID;
	int ncount = 0;
	string strChangedParaXml;
	string strNodeXml;
	string strFlag;

	map<string, string>::iterator iterator_map;
	map<string ,string >::iterator iterTemp;

	string strFront;
	string strBack;

	TiXmlElement* item;
	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("ParametersChanged");
	doc.LinkEndChild(root);

	for ( iterator_map = m_mapEditParamsFront.begin( ); iterator_map != m_mapEditParamsFront.end( ); iterator_map++ )
	{

		strFlag.clear();
		strID = iterator_map->first;
		ncount = m_mapEditParamsBack.count(strID);

		if(ncount>0)
		{
			//���Ѿ����ڵĲ����н����޸�
			iterTemp = m_mapEditParamsBack.find(strID);
			if(iterTemp!=m_mapEditParamsBack.end())
			{
				strFront = iterator_map->second;
				strBack = iterTemp->second;
				if(strFront.compare(strBack)!=0)
				{
					//�����ڵ�<id=edddd flag=modify>
					strFlag = "modify";
				}
			}
			m_mapEditParamsBack.erase(strID);	//ɾ���������Ѿ����ڵ�strid
		}
		else
		{
			//�ڵ���ɾ��
			//<id =dd flag=delete>
			strFlag = "delete";
		}
		if (strFlag=="")					//Ϊ��˵�����ڵ�idû�н����޸�
			continue;
		item = ConstructXml(strID, strFlag);
		root->LinkEndChild(item);
		//m_mapEditParamsBack.erase(strID);
		//strFlag.clear();
	}
	ncount = m_mapEditParamsBack.size();
	if (ncount>0)
	{
		for ( iterator_map = m_mapEditParamsBack.begin( ); iterator_map != m_mapEditParamsBack.end( ); iterator_map++ )
		{
			//����ʣ��Ľڵ㣬����Щ�ڵ㶼�������ڵ�
			strID = iterator_map->first;
			item = ConstructXml(strID, "add");
			root->LinkEndChild(item);
		}
	}
	strNodeXml <<doc;

	//strChangedParaXml.append(strNodeXml);
	strOutJson = ConvertXmlParamsToJsonParams(strNodeXml, "ParametersChanged", "Parameter");
	//strOutJson ="\"ParametersChanged\":" + strOutJson;
#if 0
	Json::Value jsonOut;
	jsonOut["ParametersChanged"] = Json::Value(strOutJson);
	Json::FastWriter writer;
	strOutJson = writer.write(jsonOut);
#endif

	return strOutJson;
}

TiXmlElement* CComponentEdit::ConstructXml(string strID, string strFlag)
{
	string strXmlParams;
	string strItem;


	TiXmlElement* item;

	TiXmlBase::SetCondenseWhiteSpace(false);

	item = new TiXmlElement("Parameter");
	item->SetAttribute("id", strID);
	item->SetAttribute("flag", strFlag);


	strItem <<*item;
	//strXmlParams << doc;

	return item;

}

//����Ϊutf8�����ض��ֽ�
string CComponentEdit::EditBackParams( string strXmlParams, string strRootName, string strItemName )
{
	string strJsonParams("[]");
	Json::Value jsonParams(Json::arrayValue);

	TiXmlBase::SetCondenseWhiteSpace(false);
	istringstream iss(strXmlParams);
	TiXmlElement rootElement("");
	iss >> rootElement;
	if (strRootName.compare(rootElement.ValueStr()) != 0)
	{
		return strJsonParams;
	}
	TiXmlElement* pItemElement = rootElement.FirstChildElement(strItemName);
	while (pItemElement)
	{
		Json::Value jsonParam;
		string strValue("");
		string strName;

		string strID;
		string strNameA;
		string strDirection;
		string strType;
		string strIsDynamicArray;
		TiXmlAttribute* pAb;

		string strXmlParams ;

		pAb = pItemElement->FirstAttribute();
		while (pAb)
		{

			strXmlParams<< *pItemElement;
			strName = pAb->NameTStr();
			strValue = pAb->ValueStr();
			ZTools::UTF8ToMB(strName);
			ZTools::UTF8ToMB(strValue);
			jsonParam[strName] = strValue;
			pAb = pAb->Next();
			string& str = strName;
			if(str == "id")
			{
				strID = strValue;
			}
			else if(str == "name")
			{
				strNameA = strValue;
			}
			else if(str == "direction")
			{
				strDirection = strValue;
			}
			else if(str == "type")
			{
				strType = strValue;
			}
			else if(str == "isdynamicarray")
			{
				strIsDynamicArray = strValue;
			}
		}

		if(strDirection == "In" || strDirection == "InOut")
		{
			m_vFrontFormParamsOut.push_back(move(strID + strNameA + strType + strIsDynamicArray));
		}
		if(strDirection == "Out" || strDirection == "InOut")
		{
			m_vBackFormParamsOut.push_back(move(strID + strNameA + strType + strIsDynamicArray));
		}
		m_mapEditParamsBack.insert(pair<string, string>(strID,strXmlParams));
		strXmlParams.clear();
		jsonParams.append(jsonParam);

		pItemElement = pItemElement->NextSiblingElement(strItemName);
	}

	Json::FastWriter writer;
	strJsonParams = writer.write(jsonParams);

	return strJsonParams;
}
//����ֵΪutf8����
string CComponentEdit::LoadParams(string strJsonParams, string strRootName, 
	string strItemName, string strIsProcessForm ,string strModify)
{
	/*
	"ParametersChanged":[
	{
	"id": "9a4a2247-e9ce-4908-a48f-d6c29d7e8249",
	"flag":"add"}
	,{
	"id": "9a4a2247-e9ce-4908-a48f-d6c29d7e8249",
	"flag":"add"}

	]
	<ParametersChanged>
	<ParameterItem id="259e7cef-1dfe-421d-964a-b163feda0d00" name="b" direction="In" type="Double" desc="" format="" value="" listname="" group=""/>
	</ParametersChanged>
	*/

	const char*pitem[]=
	{
		"name", "value", "type", 
		"direction", "isdynamicarray",
		"units","desc","format","group","cnname", "issaveinmodel"
	};

	string strXmlParams;
	TiXmlElement* item;
	TiXmlDocument doc;
	Json::Reader reader;
	Json::Value jParams;
	string strElement;	//���رȽϵ�����

	if (!reader.parse(strJsonParams, jParams))
	{
		return strXmlParams;
	}

	TiXmlBase::SetCondenseWhiteSpace(false);

	TiXmlElement* root = new TiXmlElement(strRootName);
	doc.LinkEndChild(root);

	int nLength = jParams.size();
	for (int i = 0; i < nLength; i++)
	{
		Json::Value jParam = jParams[Json::Value::ArrayIndex(i)];

		Json::Value::Members JMembers =  jParam.getMemberNames();

		item = new TiXmlElement(strItemName);

		string strID;
		string strName;
		string strDirection;
		string strType;
		string strIsDynamicArray;
		string strIDMap; 

		for(size_t j = 0; j < JMembers.size(); j++)
		{		
			string& str = JMembers[j];
			string strValue = jParam[str].isString() ? jParam[str].asString() : "";
			if (strIsProcessForm.compare("1") == 0 && str == "id")
			{
				item->SetAttribute(str, GetNewGuid(strValue));			
			}
			else
			{
				item->SetAttribute(str, strValue);
			}
			if(str == "id")
			{
				strID = strValue;
				strIDMap = strValue;
			}
			else if(str == "name")
			{
				strName = strValue;
			}
			else if(str == "direction")
			{
				strDirection = strValue;
			}
			else if(str == "type")
			{
				strType = strValue;
			}
			else if(str == "isdynamicarray")
			{
				strIsDynamicArray = strValue;
			}
		}	

		if(strDirection == "In" || strDirection == "InOut")
		{
			m_vFrontFormParamsIn.push_back(move(strID + strName + strType + strIsDynamicArray));
			string str_xmlvalue = move(strID + strName + strType + strIsDynamicArray);
		}
		if(strDirection == "Out" || strDirection == "InOut")
		{
			m_vBackFormParamsIn.push_back(move(strID + strName + strType + strIsDynamicArray));
		}

		root->LinkEndChild(item);
		strXmlParams << *item;
		int nSize = sizeof(pitem)/sizeof(const char*);
		for (int nattribute=0;nattribute<nSize;nattribute++)//���Ƚϵ�����ƴ�ӳ��ַ���
		{
			std::string s;
			item->QueryStringAttribute(pitem[nattribute], &s);
			strElement.append(s);
		}

		if(strModify=="back")
		{	
			m_mapEditParamsBack.insert(pair<string, string>(strIDMap,strElement));
		}
		else
		{
			m_mapEditParamsFront.insert(pair<string, string>(strIDMap,strElement));

		}
		strElement.clear();
		strXmlParams.clear();
	}

	strXmlParams << doc;
	ZTools::WriteZToolsLog(strXmlParams);
	ZTools::MBToUTF8(strXmlParams);

	return strXmlParams;
}

// ��Parameters����BOT�ļ�
string CComponentEdit::RunBotWithParameters( 
	string strBotPath,  //"E:\\RunBot\\a.bot"
	string strInstanceID,  //"3fee8b86-1522-419f-a6fd-9b39378dd2ed"//
	string strBotType, //"component"
	string strTitle,       //"ǰ��"
	string strWorkDir,  
	string strParameters,
	string strProcessName,
	string strTaskName
	/************************************************************************/
	/* 
	{
	"Parameters": [
	{
	"id": "dd020620-70d9-d745-a57e-d5944726f6e3",
	"name": "dse_txt",
	"value": "E:\\runbot\\dse.txt",
	"type": "File",
	"version": ""
	}]
	}
	*/
	/************************************************************************/
	)
{
	string strRet; //����ֵ
	string strBotFile; //bot����
	string strBotDir; //botĿ¼
	Json::Reader jReader; 
	Json::Value jv;
	if(!jReader.parse(strParameters, jv))
	{//����ʧ��
		ZTools::WriteZToolsFormatLog("Json����ʧ��");
		return strRet;
	}

	replace(strBotPath.begin(), strBotPath.end(), '/', '\\');

	size_t nPos = strBotPath.find_last_of('\\');
	if (nPos == string::npos)
	{
		ZTools::WriteZToolsFormatLog("BOT·������");
		return strRet;
	}
	else
	{
		strBotFile = strBotPath.substr(nPos + 1);
		strBotDir = strBotPath.substr(0, nPos + 1);
	}

	string strDstDir = strWorkDir + strBotType + '\\'; //����·��
	string strDstPath = strDstDir + strBotFile;

	///////////Parameters
	Json::Value jParams = jv["Parameters"];
	if(jParams.type() != Json::arrayValue)
	{
		ZTools::WriteZToolsFormatLog("jParams��Json::arrayValue����");
		return strRet;
	}

	if(false/*strBotType == "front"*/)  //�رտ����ļ�����
	{//��������ΪFILE�����򿽱��ļ�������Ŀ¼
		int nLength = jParams.size();
		for (int i = 0; i < nLength; i++)
		{
			Json::Value jsonParam = jParams[Json::Value::ArrayIndex(i)];
			string strType = jsonParam["type"].asString();
			string strValue = jsonParam["value"].asString();
			if (strType.compare("File") == 0)
			{
				if (!strValue.empty() && (strValue.find(':') != string::npos))
				{
					//�����Ϊ�����Ǿ���·�����򿽱��ļ�
					//�ļ����ڲŸ���
					if (PathFileExists(strValue.c_str()))
					{
						replace(strValue.begin(), strValue.end(), '/', '\\');
						string strFileName = strValue.substr(strValue.find_last_of('\\') + 1);
						CopyFile(strValue.c_str(), (strWorkDir + strFileName).c_str(), FALSE);
						ZTools::WriteZToolsFormatLog("�����ļ� %s-->%s", strValue.c_str(), (strWorkDir + strFileName).c_str());
					}
				}
			}
		}
	}

	if (jParams.type() == Json::arrayValue)
	{
		//д����xml�ļ�
		Json::FastWriter writer;
		string strJsonParams = writer.write(jParams);
		string strXmlParams = ConvertJsonParamsToXmlParams2(strJsonParams, "Parameters", "ParameterItem", strInstanceID);
		string strDeclaration("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
		string strParamFilePath = strDstDir + "in.xml";
		if (!MakeSureDirectoryPathExists(strParamFilePath.c_str()))
		{
			ZTools::WriteZToolsFormatLog("�������������ļ����ļ���ʧ��");
			return strRet;
		}

		//locale::global(locale(""));
		ofstream fOut(strParamFilePath);
		if (fOut == NULL || fOut.is_open() == false)
		{
			ZTools::WriteZToolsFormatLog("д���ļ� %s ʧ��", strParamFilePath.c_str());
			return strRet;
		}
		fOut << strDeclaration;
		fOut << strXmlParams;
		fOut.close();
	}
	else
	{
		ZTools::WriteZToolsFormatLog("Parameters����Json::arrayValue����");
		return strRet;
	}

	if (!::CopyFile((strBotPath).c_str(), (strDstPath).c_str(), FALSE))
	{
		ZTools::WriteZToolsFormatLog("����BOT�ļ�ʧ�ܣ�%s -->%s", (strBotPath).c_str(), (strDstPath).c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		return strRet;
	}

	if(RunBot(strDstPath, strTitle,strProcessName,strTaskName)) //ִ��bot�ļ�
	{//ִ�гɹ�
		strRet = ProcessJsonOut2(strDstDir + "out.xml");
	}
	return strRet;
}
string CComponentEdit::RunBotWithParametersNew(
	string strFrontBot,
	string strCompentBotPath,
	string strInstanceID, 
	string strTitle, 
	string strWorkDir, 
	string strParameters,
	string strProcessName,
	string strTaskName)
{
	string strRet; //����ֵ
	string strCompentBotFileName;   //���bot����
	string strFrontBotFileName; //��bot����
	Json::Reader jReader; 
	Json::Value jv;
	if(!jReader.parse(strParameters, jv))
	{//����ʧ��
		ZTools::WriteZToolsFormatLog("Json����ʧ��");
		return strRet;
	}

	replace(strFrontBot.begin(), strFrontBot.end(), '/', '\\');
	replace(strCompentBotPath.begin(), strCompentBotPath.end(), '/', '\\');

	size_t nPos = strCompentBotPath.find_last_of('\\');
	if (nPos == string::npos)
	{
		ZTools::WriteZToolsFormatLog("���BOT·������");
		return strRet;
	}
	else
		strCompentBotFileName = strCompentBotPath.substr(nPos + 1);

	nPos = strFrontBot.find_last_of('\\');
	if (nPos == string::npos)
	{
		ZTools::WriteZToolsFormatLog("��BOT·������");
		return strRet;
	}
	else
		strFrontBotFileName = strFrontBot.substr(nPos + 1);

	string strDstCompentDir = strWorkDir + "component\\";
	string strDstFrontDir   = strWorkDir + "front\\";
	string strDstCompentPath = strDstCompentDir + strCompentBotFileName;
	string strDstFrontBotPath = strDstFrontDir + strFrontBotFileName;

	///////////Parameters
	Json::Value jParams = jv["Parameters"];
	if(jParams.type() != Json::arrayValue)
	{
		ZTools::WriteZToolsFormatLog("jParams��Json::arrayValue����");
		return strRet;
	}

	if(false/*strBotType == "front"*/)  //�رտ����ļ�����
	{//��������ΪFILE�����򿽱��ļ�������Ŀ¼
		int nLength = jParams.size();
		for (int i = 0; i < nLength; i++)
		{
			Json::Value jsonParam = jParams[Json::Value::ArrayIndex(i)];
			string strType = jsonParam["type"].asString();
			string strValue = jsonParam["value"].asString();
			if (strType.compare("File") == 0)
			{
				if (!strValue.empty() && (strValue.find(':') != string::npos))
				{
					//�����Ϊ�����Ǿ���·�����򿽱��ļ�
					//�ļ����ڲŸ���
					if (PathFileExists(strValue.c_str()))
					{
						replace(strValue.begin(), strValue.end(), '/', '\\');
						string strFileName = strValue.substr(strValue.find_last_of('\\') + 1);
						CopyFile(strValue.c_str(), (strWorkDir + strFileName).c_str(), FALSE);
						ZTools::WriteZToolsFormatLog("�����ļ� %s-->%s", strValue.c_str(), (strWorkDir + strFileName).c_str());
					}
				}
			}
		}
	}

	if (jParams.type() == Json::arrayValue)
	{
		//д����xml�ļ�
		Json::FastWriter writer;
		string strJsonParams = writer.write(jParams);
		string strXmlParams = ConvertJsonParamsToXmlParams2(strJsonParams, "Parameters", "ParameterItem", strInstanceID);
		string strDeclaration("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
		string strParamFilePath = strDstFrontDir + "in.xml"; // ǰ������in.xml ��ǰ����ͬһĿ¼
		if (!MakeSureDirectoryPathExists(strParamFilePath.c_str()))
		{
			ZTools::WriteZToolsFormatLog("�������������ļ����ļ���ʧ��");
			return strRet;
		}

		//locale::global(locale(""));
		ofstream fOut(strParamFilePath);
		if (fOut == NULL || fOut.is_open() == false)
		{
			ZTools::WriteZToolsFormatLog("д���ļ� %s ʧ��", strParamFilePath.c_str());
			return strRet;
		}
		fOut << strDeclaration;
		fOut << strXmlParams;
		fOut.close();
	}
	else
	{
		ZTools::WriteZToolsFormatLog("Parameters����Json::arrayValue����");
		return strRet;
	}

	if (!MakeSureDirectoryPathExists(strDstCompentPath.c_str()))
	{
		ZTools::WriteZToolsFormatLog("��������ļ���Ŀ���ļ���ʧ��!");
		return strRet;
	}
	if (!::CopyFile(strCompentBotPath.c_str(), strDstCompentPath.c_str(), FALSE))
	{
		ZTools::WriteZToolsFormatLog("����BOT�ļ�ʧ�ܣ�%s -->%s", strCompentBotPath.c_str(), strDstCompentPath.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		return strRet;
	}
	if (!::CopyFile(strFrontBot.c_str(), strDstFrontBotPath.c_str(), FALSE))
	{
		ZTools::WriteZToolsFormatLog("������BOT�ļ�ʧ�ܣ�%s -->%s", strFrontBot.c_str(), strDstFrontBotPath.c_str());
		//ZTools::WriteZToolsLog(GetLastErrorMsg());
		return strRet;
	}

	if(RuntBotNew(strDstFrontBotPath,strDstCompentPath, strTitle,strProcessName,strTaskName)) //ִ��bot�ļ�
	{//ִ�гɹ�
		strRet = ProcessJsonOut2(strDstFrontDir + "out.xml");
	}
	return strRet;
}
//��ӻ���´��е���Ϣ��Json
void CComponentEdit::UpdateJson( Json::Value& jRet, string& strParameters )
{
	Json::Value jv;
	Json::Reader reader;
	if(!reader.parse(strParameters, jv))
	{
		ZTools::WriteZToolsFormatLog("Json����ʧ��");
		return;
	}

	jRet["runstate"] =  jv["runstate"].isString() ? jv["runstate"].asString() : "";

	Json::FastWriter jWriter;
	strParameters = jWriter.write(jRet);

	Json::Value& jvSrc = jv["Parameters"];

	if(jvSrc.type() != Json::arrayValue)
	{
		ZTools::WriteZToolsFormatLog("jSrc����array����");
		return;
	}

	Json::Value& jvDst = jRet["Parameters"];
	if(jvDst.type() != Json::arrayValue)
	{//Ϊ��
		jvDst = Json::Value(Json::arrayValue);
	}

	for (unsigned int i = 0; i <  jvSrc.size(); i++)
	{
		Json::Value& jParamSrc = jvSrc[Json::Value::ArrayIndex(i)];
		string strIDSrc = jParamSrc["id"].asString();
		m_mapParamIdValue[strIDSrc] = jParamSrc;
	}

	jvDst.clear();
	for(map<string, Json::Value>::iterator iter = m_mapParamIdValue.begin(); iter != m_mapParamIdValue.end(); iter++)
	{
		jvDst.append(iter->second);
	}

	strParameters = jWriter.write(jRet);

}

STDMETHODIMP CComponentEdit::SaveAsComponent( BSTR bstrComponentPath, BSTR bstrComponentName, VARIANT* pvRet )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraComponentEdit::CComponentEdit::SaveAsComponent");
	CComVariant vRet(false);

	string strComponentPath = _bstr_t(bstrComponentPath);
	ZTools::WriteZToolsFormatLog("strComponentPath:%s", strComponentPath.c_str());
	string strComponentName = _bstr_t(bstrComponentName);
	ZTools::WriteZToolsFormatLog("strComponentName:%s", strComponentName.c_str());

	if (!PathFileExists(strComponentPath.c_str()))
	{
		ZTools::WriteZToolsFormatLog("%s ������", strComponentPath.c_str());
		vRet.Detach(pvRet);
		return S_OK;
	}

	CFileDialog dlgFile(FALSE
		, "bot"
		, strComponentName.c_str()
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "Component File (*.bot)|*.bot||"
		, AfxGetMainWnd()
		);

	int nRet = dlgFile.DoModal();

	if (nRet == IDOK || nRet == IDYES)
	{
		CString sFilePath = dlgFile.GetPathName();

		CString sExt = sFilePath.Right(4);
		if (sExt.CompareNoCase(".bot") != 0)
		{
			sFilePath += ".bot";
		}

		if (::CopyFile(strComponentPath.c_str(), sFilePath, FALSE))
		{
			int nRet1 = MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "������Ϊ�ɹ����Ƿ������Ŀ¼", "������Ϊ", MB_OKCANCEL|MB_ICONQUESTION);
			if (nRet1 == IDOK)
			{
				CString strParam;
				strParam.Format("/e,/select,\"%s\"", sFilePath);
				ShellExecute(NULL, "open", "explorer.exe", strParam, NULL, SW_SHOW);
			}
		}
		else
		{
			MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "������Ϊʧ��", "������Ϊ", MB_OK|MB_ICONSTOP);
		}
	}

	vRet = true;
	vRet.Detach(pvRet);
	return S_OK;
}

BOOL CComponentEdit::IsFrontFormParamsChanged()
{
	sort(m_vFrontFormParamsIn.begin(), m_vFrontFormParamsIn.end());
	sort(m_vFrontFormParamsOut.begin(), m_vFrontFormParamsOut.end());
	return (m_vFrontFormParamsIn != m_vFrontFormParamsOut);
}

BOOL CComponentEdit::IsBackFormParamsChanged()
{
	sort(m_vBackFormParamsIn.begin(), m_vBackFormParamsIn.end());
	sort(m_vBackFormParamsOut.begin(), m_vBackFormParamsOut.end());
	return (m_vBackFormParamsIn != m_vBackFormParamsOut);
}
int CComponentEdit::InitParameterMap(string strParamFront ,string strFlag) 
{
	const char*pitem[]=
	{
		"name", "value", "type", 
		"direction", "isdynamicarray",
		"units","desc","format","group","cnname", "issaveinmodel"
	};
	int nSize = 0;

	stringstream strStream;
	string strID;
	string strElement;	//���رȽϵ�����

	Json::Reader reader;
	Json::Value jParams;
	Json::Value jv;

	if (!reader.parse(strParamFront, jv))
	{
		ZTools::WriteZToolsLog(">>>>>>PeraComponentEdit::InitParameterMap error");
		return 0;
	}
	//���������С
	nSize = sizeof(pitem)/sizeof(const char*);
	jParams = jv["Parameters"];
	if (jParams.type()==Json::arrayValue)
	{
		for (unsigned i = 0; i<jParams.size(); i++)
		{
			Json::Value jParam = jParams[Json::Value::ArrayIndex(i)];
			strID = jParam["id"].asString();
			for (int j=0;j<nSize;j++)				//���Ƚϵ�����ƴ�ӳ��ַ���
			{
				strElement.append(jParam[pitem[j]].asString());
			}
			if (!strFlag.compare("back"))
			{				
				m_mapEditParamsBack.insert(pair<string, string>(strID,strElement));
				m_mapModifyBack.insert(pair<string, int>(strID,i));
			}
			else if (!strFlag.compare("front"))
			{
				m_mapEditParamsFront.insert(pair<string, string>(strID,strElement));
			}
			strElement.clear();
		}
	}
	return 1;
}

string CComponentEdit::ComponentParametersChanged(MAPSTR mapEditParamsFront,MAPSTR mapEditParamsBack)	//�༭ʱ����Ӧ��id�Ƿ�ı�
{
	int i=0;				//ParametersChanged���������ֵ
	int ncount = 0;			//��mapEditParamsBack���ҵ�id����
	int ndeleteCount = 0;	//ͳ��ɾ�������ĸ���
	int nmodifyCount = 0;	//ͳ���޸Ĳ����ĸ���
	int naddCount    = 0;	//ͳ�����Ӳ����ĸ���

	string strOutJson;		//����json�ַ�������

	string strID;			//Params�е�id�ֶ�����
	string strFlag;			//���������仯�ı�־

	string strFront;		//�������ֶ��޸�ǰ������
	string strBack;			//�������ֶ��޸ĺ������

	ITERSTR_MAP iterTemp;		
	ITERSTR_MAP iterator_map;

	Json::FastWriter writer;
	Json::Value  jv;
	Json::Value  jParamSrc;
	Json::Value& jvDst = jv["ParametersChanged"];

	if(jvDst.type() != Json::arrayValue)
	{//Ϊ��
		jvDst = Json::Value(Json::arrayValue);
	}

	//���޸�ǰ��map�б���б���
	for ( iterator_map = mapEditParamsFront.begin( ); iterator_map != mapEditParamsFront.end( ); iterator_map++ )
	{

		strFlag.clear();
		strID = iterator_map->first;
		ncount = mapEditParamsBack.count(strID);

		if(ncount>0)//����0˵�������޸��ˣ����������ɾ����
		{
			//���Ѿ����ڵĲ����н����޸�
			iterTemp = mapEditParamsBack.find(strID);
			if(iterTemp!=mapEditParamsBack.end())
			{
				strFront = iterator_map->second;
				strBack = iterTemp->second;
				if(strFront.compare(strBack)!=0)
				{
					//�����ڵ�<id=edddd flag=modify>
					strFlag = "modify";
					nmodifyCount++;
				}
			}
			mapEditParamsBack.erase(strID);	//ɾ���������Ѿ����ڵ�strid
		}
		else
		{
			strFlag = "delete";
			ndeleteCount++;
		}
		if (strFlag=="")					//Ϊ��˵�����ڵ�idû�н����޸�
			continue;

		jParamSrc["id"] = strID;			//��id���ַ����ŵ�json������
		jParamSrc["flag"] = strFlag;
		jvDst[Json::Value::ArrayIndex(i)] = jParamSrc;
		i++;
	}

	ncount = mapEditParamsBack.size();		//����mapEditParamsBack��ʣ����
	if (ncount>0)							//����0˵�������ӵĲ���
	{
		for ( iterator_map = mapEditParamsBack.begin(); iterator_map != mapEditParamsBack.end( ); iterator_map++ )
		{
			//����ʣ��Ľڵ㣬����Щ�ڵ㶼�������ڵ�
			strID = iterator_map->first;
			jParamSrc["id"] = strID;
			jParamSrc["flag"] = "add"; 

			jvDst[Json::Value::ArrayIndex(i)] = jParamSrc;
			i++;					//json��������ֵ������
			naddCount++;			//���ӵ�[����]��ͳ��
		}
	}

	strOutJson = writer.write(jvDst);
	int nbegin=0;							//ɾ��������ֵ
	Json::Value jsonOut(Json::arrayValue);
	int nedit =ndeleteCount;	//���[�޸ĺ����Ӳ���]������ֵ�Ŀ�ʼ

	for (unsigned k=0;k<jvDst.size();k++)
	{
		jParamSrc = jvDst[Json::Value::ArrayIndex(k)];
		strFlag = jParamSrc["flag"].asString();
		if (!strFlag.compare("delete"))
		{
			jsonOut[Json::Value::ArrayIndex(nbegin)]=jParamSrc;
			nbegin++;
		}
		else
		{
			jsonOut[Json::Value::ArrayIndex(nedit)]=jParamSrc;
			nedit++;
		}
	}
	//if (jsonOut.type()==Json::nullValue)
	//	return strOutJson;
	strOutJson = writer.write(jsonOut);

	ZTools::WriteZToolsFormatLog("CComponentEdit::ComponentParametersChanged before SortAllParam's value is%s", strOutJson.c_str());
	if( !((naddCount==0)&&(ndeleteCount==0)&&(nmodifyCount==0)))
	{
		strOutJson = SortAllParam(strOutJson,  ndeleteCount);
	}

	return strOutJson;
}

string CComponentEdit::SortAllParam(string strInJson, int ndeleteCount)
{
	string strOutJson;

	Json::Value jIn;
	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value jParamSrc;

	Json::Value jDst(Json::arrayValue);
	Json::Value jTmp(Json::arrayValue);
	Json::Value jParams(Json::arrayValue);
	map<string,int>::iterator iterTmp;

	if (!reader.parse(strInJson, jIn))
	{
		ZTools::WriteZToolsFormatLog("�����������ʧ��");
		return strOutJson;
	}

	jParams = jIn;//��������еı仯�˵Ĳ���

	//��[ɾ��]�Ĳ����ĺ������//////////////////////////////
	for (unsigned k=ndeleteCount;k<jParams.size();k++)
	{
		jParamSrc = jParams[Json::Value::ArrayIndex(k)];
		iterTmp   = m_mapModifyBack.find(jParamSrc["id"].asString());
		if (iterTmp->first!="")
		{
			//���[�޸�]��[����]�Ĳ���
			jTmp[Json::Value::ArrayIndex(iterTmp->second)] = jParamSrc;

		}
	}//ѭ��������jTmp��ֵ������޸ĺ����ӵĲ���
	////////////////////////////////////////////////////
	int i=0;

	//ɾ����ȫ��
	for ( i=0;i<ndeleteCount;i++)
	{
		jParamSrc =  jParams[Json::Value::ArrayIndex(i)];
		if (jParamSrc["id"].asString()!="")
		{
			jDst[Json::Value::ArrayIndex(i)] = jParamSrc;
		}
	}

	//���ӵĺ��޸ĵ�
	for(unsigned k=0;k<jTmp.size();k++)
	{
		jParamSrc =  jTmp[Json::Value::ArrayIndex(k)];
		if (jParamSrc["id"].asString()!="")
		{
			jDst[Json::Value::ArrayIndex(i)] = jParamSrc;
			i++;
		}

	}

	strOutJson = writer.write(jDst);
	return strOutJson;
}

STDMETHODIMP CComponentEdit::CheckParamsChanged(BSTR bstrParam1, BSTR bstrParam2, VARIANT* pvRet)
{
	string strResult;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CComVariant vRet(_bstr_t("").GetBSTR());

	string strParamFront = _bstr_t(bstrParam1);
	string strParamBack = _bstr_t(bstrParam2);

	ZTools::WriteZToolsLog(">>>>>>CComponentEdit::CheckParamsChanged");
	ZTools::WriteZToolsFormatLog("Input bstrParam1 is:%s", strParamFront.c_str());	
	ZTools::WriteZToolsFormatLog("Input bstrParam2 is:%s", strParamBack.c_str());
	//��ʼ���޸�ǰ�Ĳ���Map
	InitParameterMap(strParamFront, "front");

	//��ʼ���޸ĺ�Ĳ���Map
	InitParameterMap(strParamBack, "back");	

	strResult = ComponentParametersChanged(m_mapEditParamsFront, m_mapEditParamsBack);
	vRet = _bstr_t(strResult.c_str()).GetBSTR();
	ZTools::WriteZToolsFormatLog(">>>>>>CComponentEdit::CheckParamsChanged return value is:%s", strResult.c_str());

	vRet.Detach(pvRet);
	return S_OK;
}

STDMETHODIMP CComponentEdit::CheckFormParamsChanged(BSTR bstrParam1, BSTR bstrParam2, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ZTools::WriteZToolsLog(">>>>>>PeraComponentEdit::CComponentEdit::CheckFormParamsChanged");
	CComVariant vRet(_bstr_t("").GetBSTR());

	string strParam1 = _bstr_t(bstrParam1);
	ZTools::WriteZToolsFormatLog("strParam1:%s", strParam1.c_str());
	string strParam2 = _bstr_t(bstrParam2);
	ZTools::WriteZToolsFormatLog("strParam2:%s", strParam2.c_str());

	m_vFrontFormParamsIn.clear();
	m_vBackFormParamsIn.clear();
	m_vFrontFormParamsOut.clear();
	m_vBackFormParamsOut.clear();

	{
		Json::Reader reader;
		Json::Value jRoot;
		Json::Value jParams;
		if (!reader.parse(strParam1, jRoot))
		{
			return S_OK;
		}

		if (!jRoot["Parameters"].isArray())
		{
			return S_OK;
		}

		jParams = jRoot["Parameters"];
		int nLength = jParams.size();
		for (int i = 0; i < nLength; i++)
		{
			Json::Value jParam = jParams[Json::Value::ArrayIndex(i)];

			string strID = jParam["id"].asString();
			string strName = jParam["name"].asString();
			string strDirection = jParam["direction"].asString();
			string strType = jParam["type"].asString();
			string strIsDynamicArray = jParam["isdynamicarray"].asString();

			if(strDirection == "In" || strDirection == "InOut")
			{
				m_vFrontFormParamsIn.push_back(move(strID + strName + strType + strIsDynamicArray));
			}
			if(strDirection == "Out" || strDirection == "InOut")
			{
				m_vBackFormParamsIn.push_back(move(strID + strName + strType + strIsDynamicArray));
			}
		}
	}


	{
		Json::Reader reader;
		Json::Value jRoot;
		Json::Value jParams;
		if (!reader.parse(strParam2, jRoot))
		{
			return S_OK;
		}

		if (!jRoot["Parameters"].isArray())
		{
			return S_OK;
		}

		jParams = jRoot["Parameters"];
		int nLength = jParams.size();
		for (int i = 0; i < nLength; i++)
		{
			Json::Value jParam = jParams[Json::Value::ArrayIndex(i)];

			string strID = jParam["id"].asString();
			string strName = jParam["name"].asString();
			string strDirection = jParam["direction"].asString();
			string strType = jParam["type"].asString();
			string strIsDynamicArray = jParam["isdynamicarray"].asString();

			if(strDirection == "In" || strDirection == "InOut")
			{
				m_vFrontFormParamsOut.push_back(move(strID + strName + strType + strIsDynamicArray));
			}
			if(strDirection == "Out" || strDirection == "InOut")
			{
				m_vBackFormParamsOut.push_back(move(strID + strName + strType + strIsDynamicArray));
			}
		}
	}

	Json::Value jsonOut;

	//  �޸��жϱ��Ƿ�ʧЧ�߼�
	string strIsFormParamsChanged = "false";
	if (IsFrontFormParamsChanged() || IsBackFormParamsChanged() )
		strIsFormParamsChanged = "true";

	jsonOut["IsFormParamsChanged"] = strIsFormParamsChanged;
	m_vFrontFormParamsIn.clear();
	m_vFrontFormParamsOut.clear();
	m_vBackFormParamsIn.clear();
	m_vBackFormParamsOut.clear();

	Json::FastWriter fw;
	string strResult = fw.write(jsonOut);
	vRet = _bstr_t(strResult.c_str()).GetBSTR();
	vRet.Detach(pvRet);
	return S_OK;
}

CComponentEdit::CComponentEdit()
{
	ZTools::InitZToolsLog("PeraComponentEdit", "PeraComponentEdit.dll");
	m_pSharedMemoryOnlineEdit = new SharedMemoryOnlineEdit();
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}
