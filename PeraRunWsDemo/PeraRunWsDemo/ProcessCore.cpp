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
		printf("�ļ���%s������!\n",strWsFile);
		ZTools::WriteZToolsFormatLog("�ļ���%s������!\n",strWsFile);
		return FALSE;
	}
	CString strUsr,strPassWd;
	if (!GetLoginConfig(strUsr,strPassWd))
	{
		ZTools::WriteZToolsLog("�������ļ�PeraRunWsDemo.ini ��ȡ�û���user �� passwd ʧ��");
		printf("�������ļ�PeraRunWsDemo.ini ��ȡ�û���user �� passwd ʧ��\n");
		return FALSE;
	}

	CString strQueryResult = UploadAndQuery(strWsFile);
	ZTools::WriteZToolsFormatLog("UploadAndQuery ����ֵΪ��%s",strQueryResult);
	if (strQueryResult.IsEmpty())
	{
		ZTools::WriteZToolsLog("UploadAndQuery���ؿ��ַ���,������ֹ...\n");
		printf("UploadAndQuery���ؿ��ַ���,������ֹ...\n");
		return FALSE;
	}
	/*else if ()
	{
	}*/

	CString strIP = ComGetLocalIp();
	CString strLoginResult =  Login(strUsr,strPassWd,strIP,"true");
	ZTools::WriteZToolsFormatLog("��¼����IP��ַΪ%s,����ֵΪ��%s\n",strIP,strLoginResult);

	int iPos = strLoginResult.Find("ticket-proxy");
	if (iPos!=0 || strLoginResult.GetLength() < 14)
	{
		ZTools::WriteZToolsLog("��¼����!\n");
		printf("��¼����!\n");
		return FALSE;
	}
	CString strTicket = strLoginResult.Mid(strLoginResult.Find('=')+1);
	CDataCallStartWs DataCall(strQueryResult,strIP,strUsr,strUsr/*"��������Ա"*/,strTicket);
	CString strCallJs = DataCall.GetJsonStr();

	ZTools::WriteZToolsFormatLog("InvokeWsMethod�������ΪstartLocalModelByWsFile,%s\n",strCallJs);
	CString strCallStartResult  = InvokeWsMethod("startLocalModelByWsFile",strCallJs);
	ZTools::WriteZToolsFormatLog("InvokeWsMethod����ֵΪ��%s\n",strCallStartResult);

	CDataCallStartWsResult CallResData;
	CallResData.FromJson(strCallStartResult);
	if (!CallResData.GetOptMsg().IsEmpty())
	{
		ZTools::WriteZToolsLog("����Ws����startLocalModelByWsFileʧ��!");
		printf("����Ws����startLocalModelByWsFileʧ��!\n");
		return FALSE;
	}
	if (bShowProgress)
	{
		CString strUrl = CallResData.GetOptUrl();
		AttachTicket(strUrl,strLoginResult);
		ZTools::WriteZToolsFormatLog("��IE�����������ҳ�棺%s\n",strUrl);
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
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
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
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
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
		ZTools::WriteZToolsFormatLog("StartUpload�������Ϊ%s\n",strUpIn);
		CString sResult = (LPCTSTR)_bstr_t(piFileUpDown->StartUpload(bstrIn));
		ZTools::WriteZToolsFormatLog("StartUpload����ֵΪ:%s\n",sResult);

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
				ZTools::WriteZToolsFormatLog("QueryUpload ��������jsonΪ��%s",strQueryResult);
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
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("�ļ��ϴ���������쳣,%s", szCause);
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
				 printf("ϵͳ������ȷ�Ϸ�������������\n");
			 }
			 else if (sResult.Compare("-6") == 0)
			 {
				 printf("�û�������\n");
			 }
			 else if (sResult.Compare("-7") == 0)
			 {
				 printf("�������\n");
			 }
			 else if (sResult.Compare("-8") == 0)
			 {
				 printf( "�û�������\n");
			 }
			 else if (sResult.Compare("-9") == 0)
			 {
				 printf("�û���ɾ��\n");
			 }
			 else if (sResult.Compare("-10") == 0)
			 {
				 printf("�û�������\n");
			 }
			 else
			 {
				 printf("��¼ʧ�ܣ���ȷ�Ϸ�������������\n");
			 }
		 }
		

	}
	catch (_com_error e)
	{
		CString strErrorMsg = e.ErrorMessage();
		ZTools::WriteZToolsFormatLog("RPC��������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("PRC��������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("RPC��������쳣,%s", szCause);
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
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", strErrorMsg);
		return "";
	}
	catch (COleException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
		return "";
	}
	catch (CException* e)
	{
		TCHAR szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);
		ZTools::WriteZToolsFormatLog("ICE��������쳣,%s", szCause);
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