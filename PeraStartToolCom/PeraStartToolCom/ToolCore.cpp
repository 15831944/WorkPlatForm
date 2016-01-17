// ToolCore.cpp : CToolCore 的实现

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
	ZTools::WriteZToolsFormatLog("输入参数：%s",strIn);
	// TODO: 在此添加实现代码
	CDataInvokeApp IData;
	CComVariant vRet("true");
	if (!IData.Parse(strIn))
	{
		vRet = "false";
		vRet.Detach(pVOut);
		ZTools::WriteZToolsFormatLog("解析Json失败!");
		return S_OK;
	}

	CToolsProUnity::StartTool(IData.m_pToolObj);
	vRet.Detach(pVOut);

	return S_OK;
}


STDMETHODIMP CToolCore::ManageApp(BSTR strInJson, VARIANT* pVOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	CToolsProUnity::StartManagerUI();
	CComVariant vRet("true");
	vRet.Detach(pVOut);
	return S_OK;
}


STDMETHODIMP CToolCore::ConfigApp(BSTR strInJson, VARIANT* pVOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	CString strIn = (LPCTSTR)_bstr_t(strInJson);
	ZTools::WriteZToolsFormatLog("输入参数：%s",strIn);
	CDataInvokeApp IData;
	CComVariant vRet("true");
	if (!IData.Parse(strIn))
	{
		vRet  = "false";
		vRet.Detach(pVOut);
		ZTools::WriteZToolsFormatLog("解析Json失败!");
		return S_OK;
	}
	CToolsProUnity::ConfigTool(IData.m_pToolObj);
	vRet.Detach(pVOut);
	return S_OK;
}


STDMETHODIMP CToolCore::InvokeAppById(BSTR strInParam, VARIANT* pVOut)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	CString strIn = (LPCTSTR)_bstr_t(strInParam); // 传进来的是软件ID，字符串类型的
	ZTools::WriteZToolsFormatLog("输入参数：%s",strIn);

	BOOL bOK = CToolsProUnity::CheckStartApp(strIn);
	CComVariant vRet("");
	vRet = bOK ? "true":"false";
	vRet.Detach(pVOut);
	return S_OK;
}


STDMETHODIMP CToolCore::OpenUrl(BSTR strUrl, VARIANT* pVout)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加实现代码
	CString strIn = (LPCTSTR)_bstr_t(strUrl);
	ZTools::WriteZToolsFormatLog("输入URL地址：%s",strIn);
	BOOL bOK = CToolsProUnity::OpenUrlWithIE(strIn);
	CComVariant vRet("");
	vRet = bOK ? "true":"false";
	vRet.Detach(pVout);
	return S_OK;
}
