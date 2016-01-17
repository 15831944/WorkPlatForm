// ToolCore.cpp : CToolCore ��ʵ��

#include "stdafx.h"
#include "ToolCore.h"
#include "DataInterface.h"
#include "ToolsProUnity.h"
#include "Tools.h"
// CToolCore

STDMETHODIMP CToolCore::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_IToolCore
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CToolCore::InvokeApp(BSTR strInJson, VARIANT* pVOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strIn = (LPCTSTR)_bstr_t(strInJson);
	ZTools::WriteZToolsFormatLog("���������%s",strIn);
	// TODO: �ڴ����ʵ�ִ���
	CDataInvokeApp IData;
	CComVariant vRet("true");
	if (!IData.Parse(strIn))
	{
		vRet = "false";
		vRet.Detach(pVOut);
		ZTools::WriteZToolsFormatLog("����Jsonʧ��!");
		return S_OK;
	}

	CToolsProUnity::StartTool(IData.m_pToolObj);
	vRet.Detach(pVOut);

	return S_OK;
}


STDMETHODIMP CToolCore::ManageApp(BSTR strInJson, VARIANT* pVOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	CToolsProUnity::StartManagerUI();
	CComVariant vRet("true");
	vRet.Detach(pVOut);
	return S_OK;
}


STDMETHODIMP CToolCore::ConfigApp(BSTR strInJson, VARIANT* pVOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	CString strIn = (LPCTSTR)_bstr_t(strInJson);
	ZTools::WriteZToolsFormatLog("���������%s",strIn);
	CDataInvokeApp IData;
	CComVariant vRet("true");
	if (!IData.Parse(strIn))
	{
		vRet  = "false";
		vRet.Detach(pVOut);
		ZTools::WriteZToolsFormatLog("����Jsonʧ��!");
		return S_OK;
	}
	CToolsProUnity::ConfigTool(IData.m_pToolObj);
	vRet.Detach(pVOut);
	return S_OK;
}


STDMETHODIMP CToolCore::InvokeAppById(BSTR strInParam, VARIANT* pVOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	CString strIn = (LPCTSTR)_bstr_t(strInParam); // �������������ID���ַ������͵�
	ZTools::WriteZToolsFormatLog("���������%s",strIn);

	BOOL bOK = CToolsProUnity::CheckStartApp(strIn);
	CComVariant vRet("");
	vRet = bOK ? "true":"false";
	vRet.Detach(pVOut);
	return S_OK;
}


STDMETHODIMP CToolCore::OpenUrl(BSTR strUrl, VARIANT* pVout)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	CString strIn = (LPCTSTR)_bstr_t(strUrl);
	ZTools::WriteZToolsFormatLog("����URL��ַ��%s",strIn);
	BOOL bOK = CToolsProUnity::OpenUrlWithIE(strIn);
	CComVariant vRet("");
	vRet = bOK ? "true":"false";
	vRet.Detach(pVout);
	return S_OK;
}
