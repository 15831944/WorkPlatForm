// RunRobotClient.cpp : CRunRobotClient ��ʵ��

#include "stdafx.h"
#include "RunRobotClient.h"
#include "Tools.h"
#include <comdef.h>
#include "ThreadData.h"
#include "CallFileUpDown.h"
#include "fLogJob.h"
#include "fRateJob.h"
#include "csharedmemory.h"
#include "DownUploadFileCacheFinder.h"
#include "DownUploadCommonFunc.h"
#include "ConfigFunc.h"
#include <io.h>
#include "ConvertData.h"
#include "InvokeWsFunc.h"
#include "InvokeLocalWsFunc.h"
#include "GetGlobalDataFromReg.h"
#include "FileFunc.h"
//��ʱ�ϴ����غ�ʱ
//#define _TEST_PERFORMANCE_

static void CustomProcessEncode( std::vector<std::string>& vInfos )
{
	Json::FastWriter jWriter;
	Json::Reader jReader;
	std::string strInfo;
	for (std::vector<std::string>::iterator it = vInfos.begin(); it != vInfos.end(); ++it)
	{
		Json::Value jInfo;
		if (jReader.parse(*it, jInfo))
		{
			strInfo = jWriter.write(jInfo);
			ZTools::UTF8ToMB(strInfo);
			*it = strInfo;
		}
		else
		{
			ZTools::WriteZToolsFormatLog("����JSON��ʽ����\n%s", it->c_str());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//�ύ����
static std::queue<std::string> s_queueLog;
static BOOL s_bLogStop = FALSE;
CCriticalSection g_critical_section_log;
static std::queue<std::string> s_queueRate;
static BOOL s_bRateStop = FALSE;
CCriticalSection g_critical_section_rate;
// static void AddLog(std::string strId, std::string strLog, std::string strLevel = "2")
// {
// 	Json::Value root;
// 	root["id"] = Json::Value(strId);
// 	root["logContent"] = Json::Value(strLog);
// 	root["logLevel"] = Json::Value(strLevel);
// 	root["time"] = Json::Value(GetCurrentTimeString());
// 	Json::FastWriter writer;	
// 	s_queueLog.push(writer.write(root));
// }
// static void AddRate(std::string strId, std::string strRate)
// {
// 	Json::Value root;
// 	root["id"] = Json::Value(strId);
// 	root["rate"] = Json::Value(strRate);
// 	root["time"] = Json::Value(GetCurrentTimeString());
// 	Json::FastWriter writer;	
// 	s_queueRate.push(writer.write(root));
// }
static DWORD WINAPI ProcessLogThread(LPVOID lpParameter)
{
	while (!s_bLogStop)
	{
		if (s_queueLog.size() == 0)
		{
			Sleep(500);
			continue;
		}
		else
		{
			g_critical_section_log.Lock();
			std::vector<std::string> vInfos;
			while (s_queueLog.size() != 0)
			{
				std::string strLogInfo = s_queueLog.front();
				s_queueLog.pop();
				vInfos.push_back(strLogInfo);
			}
			g_critical_section_log.Unlock();
			CustomProcessEncode(vInfos);
			CallWsIceRunRobotLog(vInfos);
		}
	}
	return 0;
}
static DWORD WINAPI ProcessRateThread(LPVOID lpParameter)
{
	while (!s_bRateStop)
	{
		if (s_queueRate.size() == 0)
		{
			Sleep(500);
			continue;
		}
		else
		{
			std::vector<std::string> vInfos;
			g_critical_section_rate.Lock();
			while (s_queueRate.size() != 0)
			{
				std::string strRateInfo = s_queueRate.front();
				s_queueRate.pop();
				vInfos.push_back(strRateInfo);
			}
			g_critical_section_rate.Unlock();
			CustomProcessEncode(vInfos);
			CallWsIceRunRobotRate(vInfos);
		}
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//Local�ύ����
static std::queue<std::string> s_queueLogLocal;
static BOOL s_bLogStopLocal = FALSE;
CCriticalSection g_critical_section_logLocal;
static std::queue<std::string> s_queueRateLocal;
static BOOL s_bRateStopLocal = FALSE;
CCriticalSection g_critical_section_rateLocal;
// static void AddLog(std::string strId, std::string strLog, std::string strLevel = "2")
// {
// 	Json::Value root;
// 	root["id"] = Json::Value(strId);
// 	root["logContent"] = Json::Value(strLog);
// 	root["logLevel"] = Json::Value(strLevel);
// 	root["time"] = Json::Value(GetCurrentTimeString());
// 	Json::FastWriter writer;	
// 	s_queueLog.push(writer.write(root));
// }
// static void AddRate(std::string strId, std::string strRate)
// {
// 	Json::Value root;
// 	root["id"] = Json::Value(strId);
// 	root["rate"] = Json::Value(strRate);
// 	root["time"] = Json::Value(GetCurrentTimeString());
// 	Json::FastWriter writer;	
// 	s_queueRate.push(writer.write(root));
// }
static DWORD WINAPI ProcessLogThreadLocal(LPVOID lpParameter)
{
	while (!s_bLogStopLocal)
	{
		if (s_queueLogLocal.size() == 0)
		{
			Sleep(500);
			continue;
		}
		else
		{
			g_critical_section_logLocal.Lock();
			std::vector<std::string> vInfos;
			while (s_queueLogLocal.size() != 0)
			{
				std::string strLogInfo = s_queueLogLocal.front();
				s_queueLogLocal.pop();
				vInfos.push_back(strLogInfo);
			}
			g_critical_section_logLocal.Unlock();
			CustomProcessEncode(vInfos);
			CallWsIceRunLocalRobotLog(vInfos);
		}
	}
	return 0;
}
static DWORD WINAPI ProcessRateThreadLocal(LPVOID lpParameter)
{
	while (!s_bRateStopLocal)
	{
		if (s_queueRateLocal.size() == 0)
		{
			Sleep(500);
			continue;
		}
		else
		{
			std::vector<std::string> vInfos;
			g_critical_section_rateLocal.Lock();
			while (s_queueRateLocal.size() != 0)
			{
				std::string strRateInfo = s_queueRateLocal.front();
				s_queueRateLocal.pop();
				vInfos.push_back(strRateInfo);
			}
			g_critical_section_rateLocal.Unlock();
			CustomProcessEncode(vInfos);
			CallWsIceRunLocalRobotRate(vInfos);
		}
	}
	return 0;
}

// CRunRobotClient
//flag�ݶ�Ϊ
//90%������ִ��ģʽ��ΪWSServer
//10%��ģ��ִ��ʱΪLocalServer
//�������ִ��ʱΪUseEvent:EventName
//������ִ��ʱΪWSLocal
//


STDMETHODIMP CRunRobotClient::RunRobotFinished( BSTR bstrFlag, BSTR bstrTaskId, BSTR bstrOutxmlPath, BSTR bstrRunType, VARIANT* pvReturn )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//ZTools::WriteZToolsFormatLog("�����յ�finish");
	// TODO: �ڴ����ʵ�ִ���
	std::string strFlag, strTaskId, strOutxmlContent, strRunType;
	strFlag = (bstrFlag != NULL) ? _bstr_t(bstrFlag) : "";
	strTaskId = (bstrTaskId != NULL) ? _bstr_t(bstrTaskId) : "";
	strOutxmlContent = (bstrOutxmlPath != NULL) ? _bstr_t(bstrOutxmlPath) : "";
	strRunType = (bstrRunType != NULL) ? _bstr_t(bstrRunType) : "";

	//unicode�еĿո���ʱΪ0xC2A0,תΪ���ֽں��Ϊ�ʺţ�������Ϊ����wstring�в���0x00A0���滻Ϊ0x0020���������ֽڿ���ʶ��Ϊ�ո�
	std::wstring wstrOutXmlContent = (bstrOutxmlPath != NULL) ? _bstr_t(bstrOutxmlPath) : L"";
	wchar_t wcharOldBlank = 0x00A0;
	wchar_t wcharNewBlank = 0x0020;
	if (wstrOutXmlContent.find(wcharOldBlank) != std::wstring::npos)
	{
		std::replace(wstrOutXmlContent.begin(), wstrOutXmlContent.end(), wcharOldBlank, wcharNewBlank);
		strOutxmlContent = ws2s_1(wstrOutXmlContent);
	}

	ZTools::WriteZToolsFormatLog("flag=��%s��\n--taskId=��%s��\n--xml����=��%s��\n--runtype=��%s��", strFlag.c_str(), strTaskId.c_str(), strOutxmlContent.c_str(), strRunType.c_str());

	//���̣߳��߳��з���out�����ļ��ϴ����޸�out���ϴ�out��֪ͨICE
	if (strFlag.compare("WSServer") == 0)
	{
		PostComponentOutDataThreadData* postComponentOutDataThreadData = new PostComponentOutDataThreadData;
		postComponentOutDataThreadData->strTaskId = strTaskId;
		postComponentOutDataThreadData->strOutxmlContent = strOutxmlContent;
		postComponentOutDataThreadData->strRunType = strRunType;
		DWORD dwThreadId = 0;
		HANDLE hThread = ::CreateThread(
			NULL
			, 0
			, PostComponentOutDataThread
			, (LPVOID)postComponentOutDataThreadData
			, CREATE_SUSPENDED
			, &dwThreadId);

		if (hThread == NULL || hThread == 0)
		{
			delete postComponentOutDataThreadData;
			postComponentOutDataThreadData = NULL;
			std::string strError("PostComponentOutDataThread fail");
			ZTools::WriteZToolsLog(strError);
			CComVariant v(_bstr_t(strError.c_str()).GetBSTR());
			v.Detach(pvReturn);
			return S_OK;
		}
		ResumeThread(hThread);

		::CloseHandle(hThread);
	}
	else if (strFlag.compare("WSLocal") == 0)
	{
		PostComponentOutDataThreadData* postComponentOutDataThreadData = new PostComponentOutDataThreadData;
		postComponentOutDataThreadData->strTaskId = strTaskId;
		postComponentOutDataThreadData->strOutxmlContent = strOutxmlContent;
		postComponentOutDataThreadData->strRunType = strRunType;
		DWORD dwThreadId = 0;
		HANDLE hThread = ::CreateThread(
			NULL
			, 0
			, PostComponentOutDataThreadLocal
			, (LPVOID)postComponentOutDataThreadData
			, CREATE_SUSPENDED
			, &dwThreadId);

		if (hThread == NULL || hThread == 0)
		{
			delete postComponentOutDataThreadData;
			postComponentOutDataThreadData = NULL;
			std::string strError("PostComponentOutDataThreadLocal fail");
			ZTools::WriteZToolsLog(strError);
			CComVariant v(_bstr_t(strError.c_str()).GetBSTR());
			v.Detach(pvReturn);
			return S_OK;
		}
		ResumeThread(hThread);

		::CloseHandle(hThread);
	}
	else if (strFlag.compare("LocalServer") == 0)
	{
	}
	else if (strFlag.find("UseEvent") != std::string::npos)
	{
	}

	CComVariant v(_bstr_t("RunRobotFinished").GetBSTR());
	v.Detach(pvReturn);
	//ZTools::WriteZToolsFormatLog("��������finish");
	return S_OK;
}


STDMETHODIMP CRunRobotClient::RunRobotLog(BSTR bstrFlag, BSTR bstrLogInfo, VARIANT* pvReturn)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//return S_OK;

	//ZTools::WriteZToolsFormatLog("�����յ�log");
	// TODO: �ڴ����ʵ�ִ���
	std::string strFlag, strLogInfo;
	strFlag = (bstrFlag != NULL) ? _bstr_t(bstrFlag) : "";
	strLogInfo = (bstrLogInfo != NULL) ? _bstr_t(bstrLogInfo) : "";
	//ZTools::WriteZToolsFormatLog("log -- %s -- %s", strFlag.c_str(), strLogInfo.c_str());

	if (strFlag.compare("WSServer") == 0)
	{
		g_critical_section_log.Lock();
		s_queueLog.push(strLogInfo);
		g_critical_section_log.Unlock();
		//CallWsIceRunRobotLog(GetWsIceEndPoints(), strLogInfo);
		//AmiCallWsIceRunRobotLog(strLogInfo);

// 		LogJobParam* pLogJobParam = new LogJobParam();
// 		pLogJobParam->m_strEndPoints = GetWsIceEndPoints();
// 		pLogJobParam->m_strLogInfo = strLogInfo;
// 
// 		CfLogJob* pLogJob = new CfLogJob(pLogJobParam);
// 		long nCurJobIndex = 0;
// 		if (!m_pThreadPool->SubmitJob(pLogJob, &nCurJobIndex))
// 		{
// 			SAFE_DELETE(pLogJob);
// 		}
	}
	else if (strFlag.compare("WSLocal") == 0)
	{
		g_critical_section_logLocal.Lock();
		s_queueLogLocal.push(strLogInfo);
		g_critical_section_logLocal.Unlock();
	}
	else if (strFlag.compare("LocalServer") == 0)
	{
	}
	else if (strFlag.find("UseEvent") != std::string::npos)
	{
	}
		
	CComVariant v(_bstr_t("RunRobotLog").GetBSTR());
	v.Detach(pvReturn);
	//ZTools::WriteZToolsFormatLog("��������log");
	return S_OK;
}

STDMETHODIMP CRunRobotClient::RunRobotRate(BSTR bstrFlag, BSTR bstrRateInfo, VARIANT* pvReturn)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//ZTools::WriteZToolsFormatLog("�����յ�rate");
	// TODO: �ڴ����ʵ�ִ���
	std::string strFlag, strRateInfo;
	strFlag = (bstrFlag != NULL) ? _bstr_t(bstrFlag) : "";
	strRateInfo = (bstrRateInfo != NULL) ? _bstr_t(bstrRateInfo) : "";
	//ZTools::WriteZToolsFormatLog("rate -- %s -- %s", strFlag.c_str(), strRateInfo.c_str());

	if (strFlag.compare("WSServer") == 0)
	{
		g_critical_section_rate.Lock();
		s_queueRate.push(strRateInfo);
		g_critical_section_rate.Unlock();
		//CallWsIceRunRobotRate(GetWsIceEndPoints(), strRateInfo);
		//AmiCallWsIceRunRobotRate(strRateInfo);

// 		RateJobParam* pRateJobParam = new RateJobParam();
// 		pRateJobParam->m_strEndPoints = GetWsIceEndPoints();
// 		pRateJobParam->m_strRateInfo = strRateInfo;
// 
// 		CfRateJob* pRateJob = new CfRateJob(pRateJobParam);
// 		long nCurJobIndex = 0;
// 		if (!m_pThreadPool->SubmitJob(pRateJob, &nCurJobIndex))
// 		{
// 			SAFE_DELETE(pRateJob);
// 		}
	}
	else if (strFlag.compare("WSLocal") == 0)
	{
		g_critical_section_rateLocal.Lock();
		s_queueRateLocal.push(strRateInfo);
		g_critical_section_rateLocal.Unlock();
	}
	else if (strFlag.compare("LocalServer") == 0)
	{
	}
	else if (strFlag.find("UseEvent") != std::string::npos)
	{
	}

	CComVariant v(_bstr_t("RunRobotRate").GetBSTR());
	v.Detach(pvReturn);
	//ZTools::WriteZToolsFormatLog("��������rate");
	return S_OK;
}

STDMETHODIMP CRunRobotClient::GetIP(VARIANT* pvReturn)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CString sIP;
	sIP = GetIpFromShareMemory();
	if (sIP.IsEmpty())
	{
		sIP = GetLocalIP();
	}

	CComVariant v(_bstr_t(sIP).GetBSTR());
	v.Detach(pvReturn);
	return S_OK;
}

//�ӹ����ڴ��ж�
#define SM_LOCAL_IP_LENGTH 256
CString CRunRobotClient::GetIpFromShareMemory()
{
	CString sIp("");
	CSharedMemory* pSmIp = NULL;
	pSmIp = new CSharedMemory("SM_LOCAL_IP", SM_LOCAL_IP_LENGTH);
	pSmIp->Lock();
	sIp = (char*)pSmIp->GetData();
	pSmIp->Unlock();
	delete pSmIp;
	pSmIp = NULL;
	return sIp;
}

CString CRunRobotClient::GetLocalIP()
{
	//return "192.168.114.156";
	CString sHostName,sIPAddress;
	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err;
	char szHostName[256];   
	int  nRetCode;   

	wVersionRequested = MAKEWORD( 2, 0 );    
	err = WSAStartup( wVersionRequested, &wsaData );   
	if ( err != 0 ) 
	{    
		return "";   
	}    

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
	{   
		WSACleanup( );   
		return "";    
	}

	nRetCode = gethostname(szHostName, sizeof(szHostName));   

	if (nRetCode != 0) 
	{   
		sHostName = (""); 
		return "";   
	}   

	sHostName = szHostName;

	struct hostent FAR *lpHostEnt = gethostbyname (sHostName);   

	if (lpHostEnt == NULL) 
	{   
		sIPAddress = _T("");
		return sIPAddress;
	}   

	LPSTR lpAddr = lpHostEnt->h_addr_list[0];   
	if (lpAddr) 
	{   
		struct in_addr  inAddr;   
		memmove (&inAddr, lpAddr, 4);   
		sIPAddress = inet_ntoa (inAddr);   
		if (sIPAddress.IsEmpty())   
			sIPAddress = _T("");   
	}

	nRetCode = WSACleanup();  

	if (nRetCode != 0) 
	{ 
		return "";   
	}   

	return sIPAddress;   
}

DWORD WINAPI CRunRobotClient::PostComponentOutDataThread( LPVOID lpParameter )
{
	PostComponentOutDataThreadData* threadData = (PostComponentOutDataThreadData*)lpParameter;
	std::string strTaskId = threadData->strTaskId;
	std::string strRunType = threadData->strRunType;

	if (PostComponentOutDataThreadFunc(lpParameter) == FALSE)
	{
		//CallWsIceRunRobotFinished(GetWsIceEndPoints(), strTaskId, "");
		AmiCallWsIceRunRobotFinished(strTaskId, "", strRunType);
	}
	
	return 0;
}
static std::string FindTaskIdNodeName(const char *pszSearchDir,const char * pszTaskId)
{
	std::string strSearch = pszSearchDir;
	strSearch = strSearch + std::string ("\\*") + pszTaskId;
	_finddata_t filefind;          
	int   done=0,handle; 
	if((handle =_findfirst(strSearch.c_str() ,&filefind))==-1)
	{
		ZTools::WriteZToolsFormatLog("δ�ҵ���Ӧ�����Ŀ¼��taskid:%s", pszTaskId);
		return pszTaskId; 
	}

	std::string strOut ="";
	while( ! done ) 
	{ 
			if   ((_A_SUBDIR== (_A_SUBDIR & filefind.attrib))) 
			{
				CString strName = filefind.name;
				strOut = (LPCTSTR)strName;
				break;
			} 
			
		done=_findnext(handle,&filefind) ;    
	} 
	_findclose(handle); 

	return strOut;
}
BOOL CRunRobotClient::PostComponentOutDataThreadFunc( LPVOID lpParameter )
{
	PostComponentOutDataThreadData* threadData = (PostComponentOutDataThreadData*)lpParameter;
	std::string strTaskId = threadData->strTaskId;
	std::string strOutxmlContent = threadData->strOutxmlContent;
	std::string strRunType = threadData->strRunType;
	delete threadData;
	threadData = NULL;

// 	if (PathFileExists(strOutxmlPath.c_str()) == FALSE)
// 	{
// 		ZTools::WriteZToolsFormatLog("δ�ҵ�%s", strOutxmlPath.c_str());
// 		return FALSE;
// 	}

#ifdef _TEST_PERFORMANCE_
	clock_t clockStart, clockEnd;
	double dDuration;
	clockStart = clock();
#endif // _TEST_PERFORMANCE_

	ZTools::WriteZToolsFormatLog("�м�����������ʱ��ʼ��");
	strOutxmlContent = ProcessOutxml(strTaskId, strOutxmlContent);
	//strOutxmlContent = ProcessOutxml2(strTaskId, strOutxmlContent);
	ZTools::WriteZToolsFormatLog("�м�����������ʱ������");
	ZTools::WriteZToolsLog(strOutxmlContent);

#ifdef _TEST_PERFORMANCE_
	clockEnd = clock();
	dDuration = (double)(clockEnd - clockStart) / CLOCKS_PER_SEC;
	std::string strDownloadTimeMsg = ZTools::FormatString("�м���ϴ����������ʱ��%f��", dDuration);
//	CallWsIceRunRobotLog(GetWsIceEndPoints(), ZTools::FormatString("{\"id\":\"%s\",\"logContent\":\"%s\",\"logLevel\":\"2\"}"
//		, strTaskId.c_str(), strDownloadTimeMsg.c_str()));
	AmiCallWsIceRunRobotLog(ZTools::FormatString("{\"id\":\"%s\",\"logContent\":\"%s\",\"logLevel\":\"2\"}"
		, strTaskId.c_str(), strDownloadTimeMsg.c_str()));
	ZTools::WriteZToolsLog(strDownloadTimeMsg);
#endif // _TEST_PERFORMANCE_
	
	//CallWsIceRunRobotFinished(GetWsIceEndPoints(), strTaskId, strOutxmlContent);
	AmiCallWsIceRunRobotFinished(strTaskId, strOutxmlContent, strRunType);

	//////////////////////////////////////////////////////////////////////////
	//дout.xml������DRM���ִ����ɣ�0918�汾��ʹ��
	if (!strRunType.empty())
		return TRUE;

	ZTools::WriteZToolsFormatLog("runtypeΪ��,��ʼдout.xml");
	//���ػ���·��
	CString strOutXmlDirBase = GetPeraGlobalDataFromReg("lm","Depends");
	if (strOutXmlDirBase.IsEmpty())
		strOutXmlDirBase = "c:\\WorkSpace\\Depends";

	std::string strOutXmlPath = ZTools::FormatString("%s\\outForDrm\\%s\\out.xml",strOutXmlDirBase,strTaskId.c_str());
	if (MakeSureDirectoryPathExists(strOutXmlPath.c_str()))
	{
		std::wstring wstrOutXmlPath = s2ws_1(strOutXmlPath);
		std::ofstream fOut(wstrOutXmlPath);
		if (fOut != NULL && fOut.is_open() != false)
		{
			fOut << ZTools::MBToUTF8(strOutxmlContent);
			fOut.close();
		}
	}

	return TRUE;
}

BOOL CRunRobotClient::UploadTranslate(std::string strLocalFile,std::string & strNewValue)
{
	//�������ļ�����Ŀǰ��װ������֧�ֶ��ļ�
	Json::Value jValues(Json::arrayValue);
	Json::Value jValue;

	if (strLocalFile.find(':') == -1)//��������·��
		return FALSE;

	//����ļ�������
	if (PathFileExists(strLocalFile.c_str()) == TRUE)
	{
		if (IsAccessibleFile(strLocalFile.c_str()))
		{
			//�ϴ�
			std::string strFileId("");
			strFileId = UploadHprose(strLocalFile, FALSE);
			ZTools::WriteZToolsFormatLog("��%s���ϴ�����FileID =��%s��",strLocalFile.c_str(),strFileId.c_str());
			if (strFileId.length() != 0)
			{
				jValue["fileID"] = Json::Value(strFileId);
				std::string strFileName = strLocalFile;
				std::replace(strFileName.begin(), strFileName.end(), '/', '\\');
				strFileName = strFileName.substr(strFileName.find_last_of('\\') + 1);
				jValue["fileName"] = Json::Value(strFileName);
				jValues.append(jValue);
			}
		}
		else
		{
			ZTools::WriteZToolsFormatLog("��%s��Ϊ�ļ��У����ϴ�...",strLocalFile.c_str());
			jValue["fileID"] = Json::Value("");
			jValue["fileName"] = Json::Value(strLocalFile.c_str());
			jValues.append(jValue); // �ļ��У����ϴ���
		}
		
	}

	Json::FastWriter fastWriter;
	strNewValue = fastWriter.write(jValues);
	return TRUE;
}
std::string CRunRobotClient::ProcessOutxml( std::string strTaskId, std::string strOutxmlContent )
{
	if (strOutxmlContent.empty())
	{
		return "";
	}
	ZTools::WriteZToolsFormatLog("����outxml");
	ZTools::MBToUTF8(strOutxmlContent);
	//////////////////////////////////////////////////////////////////////////
	//�����ϴ�
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.Parse(strOutxmlContent.c_str(), 0, TIXML_ENCODING_UTF8);

		TiXmlElement* rootElement = doc.RootElement();  //RobotOutput
		if (!rootElement)
		{
			//����ʧ��
			return "";
		}
		else
		{
			//std::string strOutxmlLocalPathBak = strOutxmlContent + ".backup";
			//pXmlDocument->SaveFile(strOutxmlLocalPathBak);

			TiXmlElement* pElementRunInfo = rootElement->FirstChildElement("RobotWfRunInfo"); 
			if (pElementRunInfo==NULL)
			{
				ZTools::UTF8ToMB(strOutxmlContent);
				return strOutxmlContent;
			}

			std::string strRunState;
			pElementRunInfo->QueryStringAttribute("runstate", &strRunState);
			ZTools::WriteZToolsFormatLog("ProcessOutxml����,runstate=��%s��",strRunState.c_str());
			if (_stricmp(strRunState.c_str(),"Close")!=0)
			{
				ZTools::UTF8ToMB(strOutxmlContent);
				return strOutxmlContent;
			}

			ZTools::WriteZToolsFormatLog("runstate=��%s��,��ʼִ���ϴ�ת��xml�е��ļ�",strRunState.c_str());
			TiXmlElement* pParametersElement = rootElement->FirstChildElement("Parameters"); 
			if (pParametersElement)
			{
				TiXmlElement* pParameterElement = pParametersElement->FirstChildElement("ParameterItem");
				while(pParameterElement)
				{
					//��������
					std::string strDirection;
					pParameterElement->QueryStringAttribute("direction", &strDirection);
					if (strDirection.empty())
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}
					ZTools::UTF8ToMB(strDirection);
					/*if (strDirection.compare("Out") != 0 && strDirection.compare("InOut") != 0)
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}*/

					//��ȡ��������
					std::string strDataType;
					pParameterElement->QueryStringAttribute("type", &strDataType);
					if (strDataType.empty())
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}

					ZTools::UTF8ToMB(strDataType);
					if (strDataType.compare("File") != 0)
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}

					//��������ļ���������
					{ // �����ļ�InOut��Out���Ͳ����е�value
						std::string strValue;
						pParameterElement->QueryStringAttribute("value", &strValue);
						if (!strValue.empty())
						{
							ZTools::UTF8ToMB(strValue);
							std::string strNewValue;
							if (UploadTranslate(strValue,strNewValue))
								pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("value")), ZTools::MBToUTF8(strNewValue));
							else
							{
								ZTools::WriteZToolsFormatLog("ProcessOutxml value = ��%s���ϴ�ת��ʧ�ܣ�", strValue.c_str());
							}
						}
						
					}
					
					if(strDirection.compare("InOut")==0)  //InOut���͵Ĳ��� DefaultValue��ԭʼֵ��Ϊ�ļ���Ҳ��Ҫ�ϴ�
					{
						std::string strDefaultValue;
						pParameterElement->QueryStringAttribute("defaultvalue", &strDefaultValue);
						if (strDefaultValue.empty())
						{
							pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
							continue;
						}
						ZTools::UTF8ToMB(strDefaultValue);
						std::string strNewDefaultValue;
						if (UploadTranslate(strDefaultValue,strNewDefaultValue))
							pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("defaultvalue")), ZTools::MBToUTF8(strNewDefaultValue));
						else
						{
							ZTools::WriteZToolsFormatLog("ProcessOutxml DefaultValue=��%s���ϴ�ת��ʧ�ܣ�", strDefaultValue.c_str());
						}
					}

					pParameterElement = pParameterElement->NextSiblingElement("ParameterItem"); //����������һ���ڵ�
				}
			}
		}

		std::string strXml;
		strXml << doc;
		ZTools::UTF8ToMB(strXml);

		return strXml;
	}
}

//���̰߳汾
std::string CRunRobotClient::ProcessOutxml2( std::string strTaskId, std::string strOutxmlContent )
{	
	if (strOutxmlContent.empty())
	{
		return "";
	}
	ZTools::WriteZToolsFormatLog("���̴߳���outxml");
	ZTools::MBToUTF8(strOutxmlContent);
	//////////////////////////////////////////////////////////////////////////
	//�����ϴ�
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.Parse(strOutxmlContent.c_str(), 0, TIXML_ENCODING_UTF8);

		TiXmlElement* rootElement = doc.RootElement();  //RobotOutput
		if (!rootElement)
		{
			//����ʧ��
			return "";
		}
		else
		{
			//std::string strOutxmlLocalPathBak = strOutxmlContent + ".backup";
			//pXmlDocument->SaveFile(strOutxmlLocalPathBak);

			TiXmlElement* pParametersElement = rootElement->FirstChildElement("Parameters"); 
			if (pParametersElement)
			{
				TiXmlElement* pParameterElement = pParametersElement->FirstChildElement("ParameterItem");

				Json::Value jValues(Json::arrayValue); //���߳��ϴ�Json����

				while(pParameterElement)
				{
					//��������
					std::string strDirection;
					pParameterElement->QueryStringAttribute("direction", &strDirection);
					if (strDirection.empty())
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}
					ZTools::UTF8ToMB(strDirection);
					if (strDirection.compare("Out") != 0 && strDirection.compare("InOut") != 0)
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}

					//��ȡ��������
					std::string strDataType;
					pParameterElement->QueryStringAttribute("type", &strDataType);
					if (strDataType.empty())
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}

					ZTools::UTF8ToMB(strDataType);
					if (strDataType.compare("File") != 0)
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}

					//��������ļ���������
					std::string strValue;
					pParameterElement->QueryStringAttribute("value", &strValue);
					if (strValue.empty())
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}
					ZTools::UTF8ToMB(strValue);

					//�������ļ�����Ŀǰ��װ������֧�ֶ��ļ�
					Json::Value jValue;
					std::string strNewValue;

					if (strValue.find(':') == -1)//��������·��
					{
						pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
						continue;
					}

					//����ļ�����
					if (PathFileExists(strValue.c_str()) == TRUE)
					{
						jValue["filePath"] = strValue;
						jValues.append(jValue);
					}
					pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
				}
				///////////////////////////////////////////////////////////////////////////////
				{//ʵ���ϴ�
					Json::FastWriter fastWriter;
					std::string strNewValue = fastWriter.write(jValues);
					strNewValue = UploadHprose2(strNewValue);
					/////////////////////////////////////////////////////////////////////////////////
					Json::Reader jReader;
					Json::Value Jv;
					if(!jReader.parse(strNewValue, Jv) || !Jv.isObject()) return "";
					Json::Value::Members jms = Jv.getMemberNames();
					for(int i = 0; i < (int)jms.size(); i++)
					{
						Json::Value& JItem = Jv[jms[i]]; 
						//�滻Value
						if (pParametersElement)
						{
							pParameterElement = pParametersElement->FirstChildElement("ParameterItem");

							while(pParameterElement)
							{
								//��������
								std::string strDirection;
								pParameterElement->QueryStringAttribute("direction", &strDirection);
								if (strDirection.empty())
								{
									pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
									continue;
								}
								ZTools::UTF8ToMB(strDirection);
								if (strDirection.compare("Out") != 0 && strDirection.compare("InOut") != 0)
								{
									pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
									continue;
								}

								//��ȡ��������
								std::string strDataType;
								pParameterElement->QueryStringAttribute("type", &strDataType);
								if (strDataType.empty())
								{
									pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
									continue;
								}

								ZTools::UTF8ToMB(strDataType);
								if (strDataType.compare("File") != 0)
								{
									pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
									continue;
								}

								//��������ļ���������
								std::string strValue;
								pParameterElement->QueryStringAttribute("value", &strValue);
								if (strValue.empty())
								{
									pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
									continue;
								}
								ZTools::UTF8ToMB(strValue);

								//�������ļ�����Ŀǰ��װ������֧�ֶ��ļ�
								Json::Value jValues(Json::arrayValue);
								Json::Value jValue;
								std::string strNewValue;

								if (strValue.find(':') == -1)//��������·��
								{
									pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
									continue;
								}

								//���·����ͬ
								if (jms[i] == strValue)
								{
									std::string strFileId("");
									strFileId = JItem["fileId"].asString();
									if (strFileId.length() != 0)
									{
										jValue["fileID"] = Json::Value(strFileId);
										std::string strFileName = strValue;
										std::replace(strFileName.begin(), strFileName.end(), '/', '\\');
										strFileName = strFileName.substr(strFileName.find_last_of('\\') + 1);
										jValue["fileName"] = Json::Value(strFileName);
										jValues.append(jValue);
									}
									Json::FastWriter fastWriter;
									strNewValue = fastWriter.write(jValues);
									pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("value")), ZTools::MBToUTF8(strNewValue));
									pParameterElement = pParameterElement->NextSiblingElement("ParameterItem");
								}
							}
						}
					}
				}
			}
		}
		std::string strXml;
		strXml << doc;
		ZTools::UTF8ToMB(strXml);
		return strXml;
	}
}

std::string CRunRobotClient::UploadHprose( std::string strFilePath, BOOL bMove/* = TRUE*/ )
{
	CString sHproseRpc = GetConfig("hprose", "hproseRpc");
	if(sHproseRpc.IsEmpty())
	{
		ZTools::WriteZToolsLog("δ�ҵ�hproseRpc");
		return "";
	}

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strHproseRpc = (LPCTSTR)sHproseRpc;
	dataFileObject.m_strFilePath = strFilePath;
	CDownUploadFileCacheFinder::ReadUploadCache(&dataFileObject);

	ZTools::WriteZToolsLog(dataFileObject.m_strFileId);
	
	if(!dataFileObject.m_strFileId.empty())
	{
		return dataFileObject.m_strFileId;
	}	

	return CallUploadHprose(dataFileObject.m_strHproseRpc, dataFileObject.m_strFilePath, dataFileObject.m_strMd5, bMove);
}

HRESULT CRunRobotClient::FinalConstruct()
{
	return S_OK;
}

void CRunRobotClient::FinalRelease()
{
// 	if (m_pThreadPool)
// 	{
// 		delete m_pThreadPool;
// 		m_pThreadPool = NULL;
// 	}
	s_bLogStop = TRUE;
	s_bRateStop = TRUE;
	s_bLogStopLocal = TRUE;
	s_bRateStopLocal = TRUE;
}

CRunRobotClient::CRunRobotClient()
{
	ZTools::InitZToolsLog("PeraPcIceClient", "PeraPcIceClient.dll");
	//return;
	//m_pThreadPool = new FTL::CFThreadPool<void*>();
	//m_pThreadPool->Start(8, 32);
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = ::CreateThread(
			NULL
			, 0
			, ProcessLogThread
			, NULL
			, CREATE_SUSPENDED
			, &dwThreadId);

		if (hThread == NULL || hThread == 0)
		{
			ZTools::WriteZToolsFormatLog("�����ύ��־�߳�ʧ�ܣ��޷��ύ�κ�������־");
			return;
		}
		ResumeThread(hThread);
		::CloseHandle(hThread);
	}
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = ::CreateThread(
			NULL
			, 0
			, ProcessRateThread
			, NULL
			, CREATE_SUSPENDED
			, &dwThreadId);

		if (hThread == NULL || hThread == 0)
		{
			ZTools::WriteZToolsFormatLog("�����ύ�����߳�ʧ�ܣ��޷��ύ�κ����н���");
			return;
		}
		ResumeThread(hThread);
		::CloseHandle(hThread);
	}

	{
		DWORD dwThreadId = 0;
		HANDLE hThread = ::CreateThread(
			NULL
			, 0
			, ProcessLogThreadLocal
			, NULL
			, CREATE_SUSPENDED
			, &dwThreadId);

		if (hThread == NULL || hThread == 0)
		{
			ZTools::WriteZToolsFormatLog("�����ύ��־�߳�ʧ�ܣ��޷��ύ�κ�������־");
			return;
		}
		ResumeThread(hThread);
		::CloseHandle(hThread);
	}
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = ::CreateThread(
			NULL
			, 0
			, ProcessRateThreadLocal
			, NULL
			, CREATE_SUSPENDED
			, &dwThreadId);

		if (hThread == NULL || hThread == 0)
		{
			ZTools::WriteZToolsFormatLog("�����ύ�����߳�ʧ�ܣ��޷��ύ�κ����н���");
			return;
		}
		ResumeThread(hThread);
		::CloseHandle(hThread);
	}
}

std::string CRunRobotClient::UploadHprose2( std::string strJson, BOOL bMove/* = TRUE*/ )
{
	CString sHproseRpc("");
	sHproseRpc = GetConfig("hprose", "hproseRpc");
	if(sHproseRpc.IsEmpty())
	{
		ZTools::WriteZToolsLog("δ�ҵ�hproseRpc");
		return "";
	}

	return CallUploadFilesHprose(std::string((LPCTSTR)sHproseRpc), strJson, bMove);
}

DWORD WINAPI CRunRobotClient::PostComponentOutDataThreadLocal( LPVOID lpParameter )
{
	PostComponentOutDataThreadData* threadData = (PostComponentOutDataThreadData*)lpParameter;
	std::string strTaskId = threadData->strTaskId;
	std::string strRunType = threadData->strRunType;

	if (PostComponentOutDataThreadFuncLocal(lpParameter) == FALSE)
	{
		//CallWsIceRunRobotFinished(GetWsIceEndPoints(), strTaskId, "");
		AmiCallWsIceRunLocalRobotFinished(strTaskId, "", strRunType);
	}

	return 0;
}

BOOL CRunRobotClient::PostComponentOutDataThreadFuncLocal( LPVOID lpParameter )
{
	PostComponentOutDataThreadData* threadData = (PostComponentOutDataThreadData*)lpParameter;
	std::string strTaskId = threadData->strTaskId;
	std::string strOutxmlContent = threadData->strOutxmlContent;
	std::string strRunType = threadData->strRunType;
	delete threadData;
	threadData = NULL;

	// 	if (PathFileExists(strOutxmlPath.c_str()) == FALSE)
	// 	{
	// 		ZTools::WriteZToolsFormatLog("δ�ҵ�%s", strOutxmlPath.c_str());
	// 		return FALSE;
	// 	}

#ifdef _TEST_PERFORMANCE_
	clock_t clockStart, clockEnd;
	double dDuration;
	clockStart = clock();
#endif // _TEST_PERFORMANCE_

	//ZTools::WriteZToolsFormatLog("�м�����������ʱ��ʼ��");
	//strOutxmlContent = ProcessOutxml(strTaskId, strOutxmlContent);
	//strOutxmlContent = ProcessOutxml2(strTaskId, strOutxmlContent);
	//ZTools::WriteZToolsFormatLog("�м�����������ʱ������");
	ZTools::WriteZToolsLog(strOutxmlContent);

#ifdef _TEST_PERFORMANCE_
	clockEnd = clock();
	dDuration = (double)(clockEnd - clockStart) / CLOCKS_PER_SEC;
	std::string strDownloadTimeMsg = ZTools::FormatString("�м���ϴ����������ʱ��%f��", dDuration);
	//	CallWsIceRunRobotLog(GetWsIceEndPoints(), ZTools::FormatString("{\"id\":\"%s\",\"logContent\":\"%s\",\"logLevel\":\"2\"}"
	//		, strTaskId.c_str(), strDownloadTimeMsg.c_str()));
	AmiCallWsIceRunRobotLog(ZTools::FormatString("{\"id\":\"%s\",\"logContent\":\"%s\",\"logLevel\":\"2\"}"
		, strTaskId.c_str(), strDownloadTimeMsg.c_str()));
	ZTools::WriteZToolsLog(strDownloadTimeMsg);
#endif // _TEST_PERFORMANCE_

	//CallWsIceRunRobotFinished(GetWsIceEndPoints(), strTaskId, strOutxmlContent);
	AmiCallWsIceRunLocalRobotFinished(strTaskId, strOutxmlContent, strRunType);

	return TRUE;
}