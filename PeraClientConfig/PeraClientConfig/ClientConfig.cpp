// ClientConfig.cpp : CClientConfig ��ʵ��

#include "stdafx.h"
#include "ClientConfig.h"
#include "Tools.h"
#include "IniClass.h"
#include <comdef.h>

// CClientConfig

CClientConfig::CClientConfig()
{
	ZTools::InitZToolsLog("PeraClientConfig", "PeraClientConfig.dll");

	m_strFilePath = ZTools::FormatString("%sconfig.ini", ZTools::GetCurrentModulePath("PeraClientConfig.dll").c_str());
}

STDMETHODIMP CClientConfig::Set(BSTR bstrSection, BSTR bstrKey, BSTR bstrValue, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComVariant vRet("");
	std::string strSection, strKey, strValue, strRet;
	strSection = (bstrSection != NULL) ? _bstr_t(bstrSection) : "";
	strKey = (bstrKey != NULL) ? _bstr_t(bstrKey) : "";
	strValue = (bstrValue != NULL) ? _bstr_t(bstrValue) : "";
	ZTools::WriteZToolsFormatLog("д�������%s -> %s -> %s", strSection.c_str(), strKey.c_str(), strValue.c_str());
	
	CIniClass IniObj(m_strFilePath.c_str());
	if (FALSE == (LPCTSTR)IniObj.SetKey(strValue.c_str(), strKey.c_str(), strSection.c_str()))
	{
		std::string strMessage = ZTools::FormatString("д��������ʧ�ܣ����ܵ�ԭ��Ϊ��%s", ZTools::GetLastErrorMessageString().c_str());
		ZTools::WriteZToolsLog(strMessage);
		strRet = strMessage;
	}

	vRet = strRet.c_str();
	vRet.Detach(pvRet);

	return S_OK;
}

STDMETHODIMP CClientConfig::Get(BSTR bstrSection, BSTR bstrKey, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CComVariant vRet("");
	std::string strSection, strKey, strRet;
	strSection = (bstrSection != NULL) ? _bstr_t(bstrSection) : "";
	strKey = (bstrKey != NULL) ? _bstr_t(bstrKey) : "";
	ZTools::WriteZToolsFormatLog("��ȡ�����%s -> %s", strSection.c_str(), strKey.c_str());

	if (!PathFileExists(m_strFilePath.c_str()))
	{
		ZTools::WriteZToolsFormatLog("δ�ҵ������ļ���%s", m_strFilePath.c_str());
		vRet.Detach(pvRet);
		return S_OK;
	}

	CIniClass IniObj(m_strFilePath.c_str());
	strRet = (LPCTSTR)IniObj.GetKeyValue(strKey.c_str(), strSection.c_str());
	if(strRet.empty())
	{
		ZTools::WriteZToolsFormatLog("�������ֵΪ�գ����ܵ�ԭ��Ϊ��%s", ZTools::GetLastErrorMessageString().c_str());
	}

	ZTools::WriteZToolsFormatLog("��ȡ�����%s", strRet.c_str());
	vRet = strRet.c_str();
	vRet.Detach(pvRet);

	return S_OK;
}
