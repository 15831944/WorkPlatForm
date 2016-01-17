#include "stdafx.h"
#include "ProcessCore.h"
#include "JsonData.h"
//#import"PeraFileUpDownEx.tlb"
#include "perafileupdownex.tlh"
//#import"ProcessClientServices.tlb"
#include "processclientservices.tlh"
#include "Tools.h"

void AttachTicket(CString& strUrl, CString strTicket)
{
	if (strUrl.IsEmpty() || strTicket.IsEmpty()) return;
	if (strUrl.CompareNoCase("about:blank") == 0) return;

	if (strUrl.Find(_T("?")) == -1)
		strUrl = strUrl + _T("?") + strTicket;
	else
		strUrl = strUrl + _T("&") + strTicket;
}

BOOL ExcuteWs(CString strWsFile,BOOL bShowProgress)
{
	if (!PathFileExists(strWsFile))
	{
		printf("文件：%s不存在!\n",strWsFile);
		ZTools::WriteZToolsFormatLog("文件：%s不存在!\n",strWsFile);
		return FALSE;
	}
	CString strUsr,strPassWd;
	if (!GetLoginConfig(strUsr,strPassWd))
	{
		ZTools::WriteZToolsLog("从配置文件PeraRunWsDemo.ini 获取用户名user 和 passwd 失败");
		printf("从配置文件PeraRunWsDemo.ini 获取用户名user 和 passwd 失败\n");
		return FALSE;
	}

	CString strQueryResult = UploadAndQuery(strWsFile);
	ZTools::WriteZToolsFormatLog("UploadAndQuery 返回值为：%s",strQueryResult);
	if (strQueryResult.IsEmpty())
	{
		ZTools::WriteZToolsLog("UploadAndQuery返回空字符串,过程终止...\n");
		printf("UploadAndQuery返回空字符串,过程终止...\n");
		return FALSE;
	}
	/*else if ()
	{
	}*/

	CString strIP = ComGetLocalIp();
	CString strLoginResult =  Login(strUsr,strPassWd,strIP,"true");
	ZTools::WriteZToolsFormatLog("登录本机IP地址为%s,返回值为：%s\n",strIP,strLoginResult);

	int iPos = strLoginResult.Find("ticket-proxy");
	if (iPos!=0 || strLoginResult.GetLength() < 14)
	{
		ZTools::WriteZToolsLog("登录出错!\n");
		printf("登录出错!\n");
		return FALSE;
	}
	CString strTicket = strLoginResult.Mid(strLoginResult.Find('=')+1);
	CDataCallStartWs DataCall(strQueryResult,strIP,strUsr,strUsr/*"超级管理员"*/,strTicket);
	CString strCallJs = DataCall.GetJsonStr();

	ZTools::WriteZToolsFormatLog("InvokeWsMethod输入参数为startLocalModelByWsFile,%s\n",strCallJs);
	CString strCallStartResult  = InvokeWsMethod("startLocalModelByWsFile",strCallJs);
	ZTools::WriteZToolsFormatLog("InvokeWsMethod返回值为：%s\n",strCallStartResult);

	CDataCallStartWsResult CallResData;
	CallResData.FromJson(strCallStartResult);
	if (!CallResData.GetOptMsg().IsEmpty())
	{
		ZTools::WriteZToolsLog("调用Ws方法startLocalModelByWsFile失败!");
		printf("调用Ws方法startLocalModelByWsFile失败!\n");
		return FALSE;
	}
	if (bShowProgress)
	{
		CString strUrl = CallResData.GetOptUrl();
		AttachTicket(strUrl,strLoginResult);
		ZTools::WriteZToolsFormatLog("用IE浏览器打开运行页面：%s\n",strUrl);
		OpenUrl(strUrl);
	}

	return TRUE;
}
CString UploadWsFile(CString strWsFile)
{
	CDataUploadIn UpIn(strWsFile);
	CString strUpIn = UpIn.GetJsonStr();

	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		PeraFileUpDownExLib::IFileUpDownExPtr piFileUpDown(__uuidof(PeraFileUpDownExLib::FileUpDownEx));
		_bstr_t bstrIn((LPCTSTR)strUpIn);
		CString sUpResult = (LPCTSTR)_bstr_t(piFileUpDown->StartUpload(bstrIn));
		sOut = sUpResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}
CString QueryUpload(CString strIndex)
{
	CDataQueryUpProgressIn Query(strIndex);
	CString strJs = Query.GetJsonStr();

	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		PeraFileUpDownExLib::IFileUpDownExPtr piFileUpDown(__uuidof(PeraFileUpDownExLib::FileUpDownEx));
		_bstr_t bstrIn((LPCTSTR)strJs);
		CString sResult = (LPCTSTR)_bstr_t(piFileUpDown->QueryUpload(bstrIn));
		sOut = sResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;

}
CString UploadAndQuery(CString strWsFile)
{
	CDataUploadIn UpIn(strWsFile);
	CString strUpIn = UpIn.GetJsonStr();

	bool bErr = false;
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		PeraFileUpDownExLib::IFileUpDownExPtr piFileUpDown(__uuidof(PeraFileUpDownExLib::FileUpDownEx));
		_bstr_t bstrIn((LPCTSTR)strUpIn);
		ZTools::WriteZToolsFormatLog("StartUpload输入参数为%s\n",strUpIn);
		CString sResult = (LPCTSTR)_bstr_t(piFileUpDown->StartUpload(bstrIn));
		ZTools::WriteZToolsFormatLog("StartUpload返回值为:%s\n",sResult);

		CDataUpLoadResult ResultUp;
		ResultUp.FromJson(sResult);
		CString strIndex = ResultUp.GetIndex();
		CDataQueryUpProgressResult QueryRes;
		CString strQueryResult = "";
		while(1)
		{
			strQueryResult = QueryUpload(strIndex);
			QueryRes.FromJson(strQueryResult);
			if (QueryRes.GetUpRate()==100)
				break;

			if (QueryRes.GetFlag()=="error")
			{
				ZTools::WriteZToolsFormatLog("QueryUpload 出错，返回json为：%s",strQueryResult);
				printf("error:%s\n",QueryRes.GetMsg());
				bErr = true;
				break;
			}
			Sleep(200);
		}
		sOut = strQueryResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("文件上传组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}
	if (bErr)
		return "";

	return sOut;

}
CString Login(CString strUser,CString strPswod,CString strIp,CString strbForce)
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sResult(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrUser(strUser);
		_bstr_t bstrPassword(strPswod);
		_bstr_t bstrIp(strIp);
		_bstr_t bstrForce(strbForce);

		 sResult = (LPCTSTR)_bstr_t(piClientHelper->ERMDoLogin(bstrUser, bstrPassword,bstrIp,bstrForce));
		 if (sResult.Find("ticket-proxy=") == -1)
		 {
			 if (sResult.Compare("-2") == 0)
			 {
				 printf("系统错误，请确认服务器工作正常\n");
			 }
			 else if (sResult.Compare("-6") == 0)
			 {
				 printf("用户不存在\n");
			 }
			 else if (sResult.Compare("-7") == 0)
			 {
				 printf("密码错误\n");
			 }
			 else if (sResult.Compare("-8") == 0)
			 {
				 printf( "用户被锁定\n");
			 }
			 else if (sResult.Compare("-9") == 0)
			 {
				 printf("用户被删除\n");
			 }
			 else if (sResult.Compare("-10") == 0)
			 {
				 printf("用户被禁用\n");
			 }
			 else
			 {
				 printf("登录失败，请确认服务器工作正常\n");
			 }
		 }
		

	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("PRC组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("RPC组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}
	return sResult;
}
CString InvokeWsMethod(CString strMethodName,CString strJs)
{
	HRESULT hr = ::CoInitialize(NULL);
	CString sOut(""); 
	try
	{
		ProcessClientServicesLib::IRpcCallHelperPtr piClientHelper(__uuidof(ProcessClientServicesLib::RpcCallHelper));
		_bstr_t bstrMethod(strMethodName);
		_bstr_t bstrJson(strJs);
		CString sResult = (LPCTSTR)_bstr_t(piClientHelper->InvokeWsMethod(bstrMethod, bstrJson));
		sOut = sResult;
	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("ICE组件调用异常,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE组件调用异常,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE组件调用异常,%s", szCause);
		return "";
	}
	if (SUCCEEDED(hr))
	{
		::CoUninitialize();
	}

	return sOut;
}
void OpenUrl(CString strUrl)
{
	if (strUrl.IsEmpty())
		return;

	ShellExecute(0, "open", "iexplore.exe",strUrl, NULL, SW_SHOW);
}
BOOL GetLoginConfig(CString &strUsr,CString &strPassWd)
{
	char szExeDir[500] ={0};
	GetModuleFileName(NULL,szExeDir,500);
	CString str = szExeDir;
	CString strConfigFile = str.Left(str.GetLength()-3) + "ini";
	char ac_Result[255];
	GetPrivateProfileString("PeraRunWsDemo","user","",ac_Result,255,(LPCTSTR)strConfigFile);
	strUsr = ac_Result;
	GetPrivateProfileString("PeraRunWsDemo","passwd","",ac_Result,255,(LPCTSTR)strConfigFile);
	strPassWd = ac_Result;

	if (strUsr.IsEmpty() || strPassWd.IsEmpty())
		return FALSE;

	return TRUE;
}