#include "StdAfx.h"
#include "RobotRemoteServiceI.h"
#include "Tools.h"
#include <tlhelp32.h>
#include "ThreadData.h"
#include "GetFlexwareBin.h"
#include <afxinet.h>
#include <time.h>
#include "CallFileUpDown.h"
#include "JsonData.h"
#include "CallRunTimeIce.h"
#include <queue>
#include "DownUploadFileCacheFinder.h"
#include "ConfigFunc.h"
#include "InvokeWsFunc.h"
#include "InvokeLocalWsFunc.h"
#include "GetGlobalDataFromReg.h"
//计时上传下载耗时
//#define _TEST_PERFORMANCE_

//执行中心进程启动锁
CCriticalSection g_critical_section_ProcessService;

static std::string GetCurrentTimeString()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	std::string strTm = ZTools::FormatString(
		"%04d-%02d-%02d %02d:%02d:%02d"
		, systime.wYear
		, systime.wMonth
		, systime.wDay
		, systime.wHour
		, systime.wMinute
		, systime.wSecond);
	return strTm;
}
//////////////////////////////////////////////////////////////////////////
//提交队列
static std::queue<std::string> s_queueLog;
static BOOL s_bLogStop = FALSE;
CCriticalSection g_critical_section_log;
static std::queue<std::string> s_queueRate;
static BOOL s_bRateStop = FALSE;
CCriticalSection g_critical_section_rate;
static void AddLog(std::string strId, std::string strLog, std::string strLevel = "2")
{
	Json::Value root;
	root["id"] = Json::Value(strId);
	root["logContent"] = Json::Value(strLog);
	root["logLevel"] = Json::Value(strLevel);
	root["time"] = Json::Value(GetCurrentTimeString());
	Json::FastWriter writer;
	g_critical_section_log.Lock();
	s_queueLog.push(writer.write(root));
	g_critical_section_log.Unlock();
}
static void AddRate(std::string strId, std::string strRate)
{
	Json::Value root;
	root["id"] = Json::Value(strId);
	root["rate"] = Json::Value(strRate);
	root["time"] = Json::Value(GetCurrentTimeString());
	Json::FastWriter writer;	
	g_critical_section_rate.Lock();
	s_queueRate.push(writer.write(root));
	g_critical_section_rate.Unlock();
}
static DWORD WINAPI ProcessLogThread(LPVOID lpParameter)
{
	while (!s_bLogStop)
	{
		if (s_queueLog.size() == 0)
		{
			//ZTools::WriteZToolsFormatLog("提交日志线程空转");
			Sleep(500);
			continue;
		}
		else
		{
			std::vector<std::string> vInfos;
			g_critical_section_log.Lock();
			while (s_queueLog.size() != 0)
			{
				std::string strLogInfo = s_queueLog.front();
				s_queueLog.pop();
				vInfos.push_back(strLogInfo);
			}
			g_critical_section_log.Unlock();
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
			//ZTools::WriteZToolsFormatLog("提交进度线程空转");
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
			CallWsIceRunRobotRate(vInfos);
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//Local提交队列
static std::queue<std::string> s_queueLogLocal;
static BOOL s_bLogStopLocal = FALSE;
CCriticalSection g_critical_section_logLocal;
static std::queue<std::string> s_queueRateLocal;
static BOOL s_bRateStopLocal = FALSE;
CCriticalSection g_critical_section_rateLocal;
static void AddLogLocal(std::string strId, std::string strLog, std::string strLevel = "2")
{
	Json::Value root;
	root["id"] = Json::Value(strId);
	root["logContent"] = Json::Value(strLog);
	root["logLevel"] = Json::Value(strLevel);
	root["time"] = Json::Value(GetCurrentTimeString());
	Json::FastWriter writer;
	g_critical_section_logLocal.Lock();
	s_queueLogLocal.push(writer.write(root));
	g_critical_section_logLocal.Unlock();
}
static void AddRateLocal(std::string strId, std::string strRate)
{
	Json::Value root;
	root["id"] = Json::Value(strId);
	root["rate"] = Json::Value(strRate);
	root["time"] = Json::Value(GetCurrentTimeString());
	Json::FastWriter writer;	
	g_critical_section_rateLocal.Lock();
	s_queueRateLocal.push(writer.write(root));
	g_critical_section_rateLocal.Unlock();
}
static DWORD WINAPI ProcessLogThreadLocal(LPVOID lpParameter)
{
	while (!s_bLogStopLocal)
	{
		if (s_queueLogLocal.size() == 0)
		{
			//ZTools::WriteZToolsFormatLog("提交日志线程空转");
			Sleep(500);
			continue;
		}
		else
		{
			std::vector<std::string> vInfos;
			g_critical_section_logLocal.Lock();
			while (s_queueLogLocal.size() != 0)
			{
				std::string strLogInfo = s_queueLogLocal.front();
				s_queueLogLocal.pop();
				vInfos.push_back(strLogInfo);
			}
			g_critical_section_logLocal.Unlock();
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
			//ZTools::WriteZToolsFormatLog("提交进度线程空转");
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
			CallWsIceRunLocalRobotRate(vInfos);
		}
	}
	return 0;
}

BOOL FindProcessByName(LPCTSTR szFileName)
{
	// 采用进程快照枚举进程的方法查找指定名称进程
	HANDLE hProcesses;
	PROCESSENTRY32 lpe =
	{
		sizeof(PROCESSENTRY32)
	};
	CString strFileName(szFileName);
	strFileName.MakeLower();

	// 创建进程快照
	hProcesses = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcesses == INVALID_HANDLE_VALUE)
		return FALSE;
	// 获取第一个进程实例
	BOOL isExist = ::Process32First(hProcesses, &lpe);
	BOOL isRunning = FALSE;
	CString strName;
	while (isExist)
	{
		strName = lpe.szExeFile;
		strName.MakeLower();
		if (-1 != strFileName.Find(strName))
		{
			isRunning = TRUE;
			break;
		}
		// 遍历下一个进程实例
		isExist = ::Process32Next(hProcesses, &lpe);
	}

	CloseHandle(hProcesses);
	return isRunning;
}

RobotRemoteServiceI::RobotRemoteServiceI()
{
	m_setId.clear();
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
			ZTools::WriteZToolsFormatLog("创建提交日志线程失败，无法提交任何运行日志");
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
			ZTools::WriteZToolsFormatLog("创建提交进度线程失败，无法提交任何运行进度");
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
			ZTools::WriteZToolsFormatLog("创建提交日志线程失败，无法提交任何运行日志");
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
			ZTools::WriteZToolsFormatLog("创建提交进度线程失败，无法提交任何运行进度");
			return;
		}
		ResumeThread(hThread);
		::CloseHandle(hThread);
	}
}

RobotRemoteServiceI::~RobotRemoteServiceI()
{
	s_bLogStop = TRUE;
	s_bRateStop = TRUE;

	s_bLogStopLocal = TRUE;
	s_bRateStopLocal = TRUE;
}

::std::string RobotRemoteServiceI::runRobotStart( const ::std::string& strInfo, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
/*
Json对象的内容如下所示
{"id":"001",
"taskName":"任务1",
"robotFile":"\robot\002\组件a.bot",
"inputFile":"\files\input.xml",
" taskUser ":"admin",
"assignNode":"Node01",
"taskInfo":"表单名称",
"processName":"流程名称",
"isForm":"1"}

具体属性说明如下：
Id:唯一标识
taskName:任务名称
robotSvn：bot文件的svn相对路径
inputSvn：input文件的svn相对路径
taskUser：任务执行人
assignNode：远程计算节点的名称
taskInfo：表单信息
processName：流程名称
isForm：是否表单

客户端执行伪代码
解析JSON，取出ID，ID与成员set中比较，已存在，则返回，不存在，则加入，并
创建一线程
线程中解析JSON串，取出流程名称，在执行的工作目录下创建流程名称文件夹，取出ID，以ID为目录名创建子文件夹
取bot路径，下载到该文件夹
取input路径，下载到该文件夹
解析XML，取出输入文件参数的SVN路径，依次下载到该文件夹
运行robot，传入相关参数

*/
// 	ZTools::WriteZToolsLog(
// 		ZTools::FormatString("Functiong RobotRemoteServiceI::runRobotStart\nstrInfo:%s", 
// 		strInfo.c_str())
// 		);
	std::string strMyInfo(strInfo);
	ZTools::UTF8ToMB(strMyInfo);
	ZTools::WriteZToolsLog(
		ZTools::FormatString("Functiong RobotRemoteServiceI::runRobotStart\nstrInfo:%s", 
		strMyInfo.c_str())
		);

	Json::Value jInfo;
	Json::Reader reader;

	if (!reader.parse(strMyInfo, jInfo))
	{
		std::string strError("PeraTaskService: json format error!");
		ZTools::WriteZToolsLog(strError);
		return strError;
	}
	//得到任务id
	std::string strId = jInfo["id"].asString();
	/*if (strId.empty())
	{
	std::string strError("PeraTaskService: id empty!");
	ZTools::WriteZToolsLog(strError);
	return strError;		
	}*/

	/*std::set<std::string>::iterator itIdPos = m_setId.find(strId);
	if (itIdPos != m_setId.end())
	{
	std::string strError("PeraTaskService: id already exist!");
	ZTools::WriteZToolsLog(strError);
	return strError;	
	}

	m_setId.insert(strId);*/
	ZTools::WriteZToolsFormatLog("【】收到任务准备分发开始计时点：%s", strId.c_str());

	//增加runType区分执行逻辑
	//00:普通组件；01：带有表单的组件；02：带有表单的子流程   0开头为表单初始执行
	//12：带有表单的子流程接受输出值，更新页面状态，确定按钮可以用  1开头为更新表单状态
	//20：关闭表单(流程暂停,终止)  2开头为关闭表单 增加key名称为idList(json数组（[1,2,3,4]）)
	//引擎通知客户端暂停的多个节点ID，然后客户端再通知封装，关闭对应表单
	//该处strRunType为启动任务异常时，直接调后台所带的参数，所以区分组件和子流程单独处理
	std::string strRunType = jInfo["runType"].asString();
	if (strRunType == "00" || strRunType == "01")
	{
		strRunType = "";
	}
	else if (strRunType == "02"  || strRunType== "13" || strRunType== "14")
	{
		strRunType = "022";
	}
	else
	{
		strRunType = "";
	}

	PrepareComponentDataThreadData* prepareComponentDataThreadData = new PrepareComponentDataThreadData;
	prepareComponentDataThreadData->strInfo = strMyInfo;
	prepareComponentDataThreadData->strId = strId;
	prepareComponentDataThreadData->strRunType = strRunType;
	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, PrepareComponentDataThread
		, (LPVOID)prepareComponentDataThreadData
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		delete prepareComponentDataThreadData;
		prepareComponentDataThreadData = NULL;
		std::string strError("PrepareComponentDataThread fail");
		ZTools::WriteZToolsLog(strError);
		return strError;	
	}
	ResumeThread(hThread);

	::CloseHandle(hThread);
	

// 	ZTools::ExportFromSvn("svn://192.168.50.206:3690/MyProject/fileRoot/MyProject", "/svn/template/component/62880/我的6.bot",
// 	"", "d:\\aaa.bot", "fileRoot", "123456");

	std::string strReturn("");
	//strReturn = strMyInfo;
	ZTools::WriteZToolsLog(
		ZTools::FormatString("Functiong RobotRemoteServiceI::runRobotStart返回值\nstrReturn:%s", 
		strReturn.c_str())
		);
	ZTools::MBToUTF8(strReturn);
	return strReturn;
	//throw std::exception("The method or operation is not implemented.");
}

void RobotRemoteServiceI::runRobotStop( const ::std::string& strTaskId, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
	std::string strMyTaskId(strTaskId);
	ZTools::UTF8ToMB(strMyTaskId);
	ZTools::WriteZToolsLog(
		ZTools::FormatString("Functiong RobotRemoteServiceI::runRobotStop\nstrTaskId:%s", 
		strMyTaskId.c_str())
		);

	//throw std::exception("The method or operation is not implemented.");
}

DWORD WINAPI RobotRemoteServiceI::PrepareComponentDataThread( LPVOID lpParameter )
{
	PrepareComponentDataThreadData* threadData = (PrepareComponentDataThreadData*)lpParameter;
	std::string strId = threadData->strId;
	std::string strRunType = threadData->strRunType;

	if (PrepareComponentDataThreadFunc(lpParameter) == FALSE)
	{
		//CallWsIceRunRobotFinished(GetWsIceEndPoints(), strId, "");
		AmiCallWsIceRunRobotFinished(strId, "", strRunType);
	}

	return 0;
}
BOOL RobotRemoteServiceI::PrepareComponentDataThreadFunc( LPVOID lpParameter )
{
	PrepareComponentDataThreadData* threadData = (PrepareComponentDataThreadData*)lpParameter;
	std::string strInfo = threadData->strInfo;
	delete threadData;
	threadData = NULL;

	//检查执行环境
	CString sProcessServicePath = GetProcessServicePath();
	CString sComponentBinPath = GetFlexwareBinPathMethod();
	if (sProcessServicePath.IsEmpty())
	{
		ZTools::WriteZToolsLog("未找到PeraProcessService.exe");
		return FALSE;
	}

	Json::Value jInfo;
	Json::Reader reader;

	if (!reader.parse(strInfo, jInfo))
	{
		std::string strError("PeraTaskService: json format error!");
		ZTools::WriteZToolsLog(strError);
		return FALSE;
	}
	//{"id":"001",
	//	"taskName":"任务1",
	//	"robotFile":"\robot\002\组件a.bot",
	//	"inputFile":"\files\input.xml",
	//	" taskUser ":"admin",
	//	"assignNode":"Node01",
	//	"taskInfo":"表单名称",
	//	"processName":"流程名称",
	//	"isForm":"1"}//1前表单，2后表单
	//得到任务id
	std::string strId = jInfo["id"].asString();
	std::string strTaskName = jInfo["taskName"].asString();
	if (strTaskName.empty())
	{
		strTaskName = "temp";
	}
	Json::Value jRobot = jInfo["robotFile"];
	std::string strInxmlContext = jInfo["inputFile"].asString();
	std::string strTaskUser = jInfo["taskUser"].asString();
	std::string strAssignNode = jInfo["assignNode"].asString();
	std::string strTaskInfo = jInfo["taskInfo"].asString();
	std::string strProcessName = jInfo["processName"].asString();
	std::string strIsForm = jInfo["isForm"].asString();
	std::string strRunType = jInfo["runType"].asString(); //  //attachType 运行类型，对应引擎传来的runType属性
	Json::Value jFormFile = jInfo["formFile"]; // 表单BOT文件路径
	std::string strFormTitle = jInfo["formTitle"].asString(); // 暂定作业名称+节点名称
	Json::Value jidList = jInfo["idList"];
	std::string strIsProcessPreForm = jInfo["isProcessPreForm"].asString();
	if (strIsProcessPreForm.compare("true") == 0)
	{
		Sleep(2000);
	}

	//网络版中，执行设计过程时，中间件准备节点的输入数据及指定组件的执行输出路径
	CString strCacheRoot = GetConfig("Pera","RunCache"); 
	if(strCacheRoot.IsEmpty())
		strCacheRoot =  "C:\\WorkSpace\\Data";
	std::string strBasePath = ZTools::FormatString("%s\\ModelRunCache\\RunInFlow\\%s_%s\\"
		, strCacheRoot, strTaskName.c_str(),strId.c_str());

	if (!MakeSureDirectoryPathExists(strBasePath.c_str()))
	{
		DWORD dError = GetLastError();
		std::string s = ZTools::FormatString("创建文件夹失败,%d,%s", dError, strBasePath.c_str());
		ZTools::WriteZToolsLog(s);
		return FALSE;
	}
	
	AddLog(strId, "准备输入数据");

#ifdef _TEST_PERFORMANCE_
	clock_t clockStart, clockEnd;
	double dDuration;
	clockStart = clock();
#endif // _TEST_PERFORMANCE_
	ZTools::WriteZToolsFormatLog("【】中间件准备输入开始计时点");
	//处理inxml
	std::string strInDir = strBasePath + "in\\"; 
	if (!MakeSureDirectoryPathExists(strInDir.c_str()))
	{
		DWORD dError = GetLastError();
		std::string s = ZTools::FormatString("创建in文件夹失败,%d,%s", dError, strInDir.c_str());
		ZTools::WriteZToolsLog(s);
		return FALSE;
	}
	strInxmlContext = ProcessInxml(strInxmlContext, strInDir, strId);
	//strInxmlContext = ProcessInxml2(strInxmlContext, strBasePath);
	ZTools::WriteZToolsLog(strInxmlContext);

	AddLog(strId, "准备组件文件");


	//下载bot
	std::string strBotLocalPath("");
	if (strRunType == "00" || strRunType == "01") // 00:普通组件；01：带有表单的组件
	{
		std::string strRobotFid = jRobot["fileID"].asString();
		std::string strRobotFileName = jRobot["fileName"].asString();
		strBotLocalPath = DownloadHprose(strRobotFid, "", strRobotFileName);
		if (strBotLocalPath.length() == 0)
		{
			std::string strErrorMsg = ZTools::FormatString("下载组件失败,fileid:%s", strRobotFid.c_str());
			ZTools::WriteZToolsLog(strErrorMsg);
			AddLog(strId, strErrorMsg);
			return FALSE;
		}
	}
	
	std::string strFormFile("");
	if (strRunType == "01" || strRunType == "02" || strRunType == "13" || strRunType == "14") // 表单文件需要下载
	{
		if (!jFormFile.isObject())
		{
			std::string s = ZTools::FormatString("解析formFile文件的路径信息(formFile)失败，格式不正确");
			ZTools::WriteZToolsLog(s);
			return FALSE;
		}

		std::string strFormFid = jFormFile["fileID"].asString();
		std::string strFormFileName = jFormFile["fileName"].asString();
		strFormFile = DownloadHprose(strFormFid, "", strFormFileName);
		if (strFormFile.length() == 0)
		{
			std::string strErrorMsg = ZTools::FormatString("下载表单组件失败,fileid:%s", strFormFid.c_str());
			ZTools::WriteZToolsLog(strErrorMsg);
			AddLog(strId, strErrorMsg);
			return FALSE;
		}
	}

	ZTools::WriteZToolsFormatLog("【】中间件准备输入结束计时点");
		
#ifdef _TEST_PERFORMANCE_
	clockEnd = clock();
	dDuration = (double)(clockEnd - clockStart) / CLOCKS_PER_SEC;
	std::string strDownloadTimeMsg = ZTools::FormatString("中间件下载组件及所有输入文件用时：%f秒", dDuration);
	AddLog(strId, strDownloadTimeMsg);

	ZTools::WriteZToolsLog(strDownloadTimeMsg);
#endif // _TEST_PERFORMANCE_
	
	AddRate(strId, "30");

	std::string strOutDir = strBasePath + "out\\"; 
	if (!MakeSureDirectoryPathExists(strOutDir.c_str()))
	{
		DWORD dError = GetLastError();
		std::string s = ZTools::FormatString("创建out文件夹失败,%d,%s", dError, strInDir.c_str());
		ZTools::WriteZToolsLog(s);
		return FALSE;
	}

	//运行
	JosnDataRunRobot jsonDataRunRobot;
	jsonDataRunRobot.Clear();
	jsonDataRunRobot.taskId = strId;
	jsonDataRunRobot.flag = "WSServer";
	jsonDataRunRobot.inXml = strInxmlContext;
	jsonDataRunRobot.outPath = strOutDir;
	jsonDataRunRobot.attachType = strRunType;
	jsonDataRunRobot.taskName = strTaskName;
	jsonDataRunRobot.processName = strProcessName;

	if (strRunType == "20")
	{
		if (!jidList.isArray() )
			return FALSE;

		jsonDataRunRobot.idList = jidList.toStyledString(); //当attachType为20时有效，数组中为要关闭的表单的taskId
	}

	if (strRunType == "00")
		jsonDataRunRobot.botPath = strBotLocalPath; // 当attachType为00时，传组件路径，对应引擎传来的robotFile
	else if (strRunType == "01" || strRunType == "02" || strRunType== "13" ||  strRunType== "14")
	{
		jsonDataRunRobot.title = strFormTitle; //如果是表单，该属性用于表单的标题，对应于引擎传来对象中的formTitle
		jsonDataRunRobot.botPath = strFormFile;   //当attachType为01、02时，传表单路径，对应引擎传来的formFile
		if (strRunType == "01")
		{
			jsonDataRunRobot.attachData = strBotLocalPath; //当attacheType为01时，传组件路径，对应引擎传来的robotFile
			//attachType为其它值，赋空字符串
		}
	}

	ZTools::WriteZToolsFormatLog("【】中间件分发任务结束计时点：%s", strId.c_str());
	if (!FindProcessByName("PeraProcessService.exe"))
	{
		ZTools::WriteZToolsFormatLog("第一次查找PeraProcessService进程，未找到");
		g_critical_section_ProcessService.Lock();
		if (!FindProcessByName("PeraProcessService.exe"))
		{
			ZTools::WriteZToolsFormatLog("第二次查找PeraProcessService进程，未找到");
			SHELLEXECUTEINFO si;
			memset(&si, NULL, sizeof(si));
			si.cbSize = sizeof(si);
			si.nShow = SW_HIDE;
			si.fMask = SEE_MASK_NOCLOSEPROCESS;
			si.lpVerb = _T("open");
			si.lpDirectory = sComponentBinPath;
			si.lpFile = sProcessServicePath;
			si.lpParameters = NULL;
			if (!ShellExecuteEx(&si))
			{
				ZTools::WriteZToolsFormatLog("启动PeraProcessService进程出现异常");
				g_critical_section_ProcessService.Unlock();
				return FALSE;
			}
		}
		else
		{
			ZTools::WriteZToolsFormatLog("第二次查找PeraProcessService进程存在");
		}
		g_critical_section_ProcessService.Unlock();
	}
	else
	{
		ZTools::WriteZToolsFormatLog("第一次查找PeraProcessService进程存在");
	}
	if (!CallRunRobot(jsonDataRunRobot.ToJson()))
	{
		for (int i = 0; i < 20; i++)
		{
			Sleep(500);
			if (!CallRunRobot(jsonDataRunRobot.ToJson()))
			{
				if (i == 19)
				{
					if (!FindProcessByName("PeraProcessService.exe"))
					{
						ZTools::WriteZToolsFormatLog("调用runtime的启动接口失败，失败时PeraProcessService进程未找到");
					}
					else
					{
						ZTools::WriteZToolsFormatLog("调用runtime的启动接口失败，失败时PeraProcessService进程存在");
					}
					return FALSE;
				}
			}
			else
			{
				break;
			}
		}
	}

	return TRUE;
}

std::string RobotRemoteServiceI::ProcessInxml( std::string strInxmlContext, std::string strBasePath, std::string strTaskId )
{	
	ZTools::WriteZToolsFormatLog("处理inxml");
	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument xmlDocument;
	ZTools::MBToUTF8(strInxmlContext);
	xmlDocument.Parse(strInxmlContext.c_str(), 0, TIXML_ENCODING_UTF8);
	std::string strLogMsg;

	TiXmlElement* rootElement = xmlDocument.RootElement();  //Parameters
	if (rootElement)
	{
		TiXmlElement* pPackageElement = rootElement->FirstChildElement("Package"); 
		if (pPackageElement)
		{
			pPackageElement->SetAttribute(ZTools::MBToUTF8(std::string("IsPopupForm")), ZTools::MBToUTF8(std::string("True")));
			//IsPopupForm
			TiXmlElement* pParameterElement = pPackageElement->FirstChildElement("Parameter");
			while(pParameterElement)
			{
				//读取参数名
				std::string strParamName;
				pParameterElement->QueryStringAttribute("name", &strParamName);

				//读取参数类型
				std::string strDataType;
				pParameterElement->QueryStringAttribute("type", &strDataType);
				if (strDataType.empty())
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				ZTools::UTF8ToMB(strDataType);
				if (strDataType.compare("File") != 0)
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				//对文件型参数处理
				std::string strDefaultValue;
				pParameterElement->QueryStringAttribute("value", &strDefaultValue);
				if (strDefaultValue.empty())
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}
				ZTools::UTF8ToMB(strDefaultValue);//得到文件fileId

				std::string strNewValue("");
				//////
				Json::Value jFilesValue;
				Json::Reader reader;
				if (!reader.parse(strDefaultValue, jFilesValue))
				{
					strLogMsg = ZTools::FormatString("解析参数 %s 的值 %s 失败", strParamName.c_str(), strDefaultValue.c_str());
					ZTools::WriteZToolsLog(strLogMsg);
					AddLog(strTaskId, strLogMsg);
					//pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("value")), ZTools::MBToUTF8(std::string("")));	
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				if (jFilesValue.type() != Json::arrayValue)
				{
					strLogMsg = ZTools::FormatString("解析参数 %s 的值 %s 失败", strParamName.c_str(), strDefaultValue.c_str());
					ZTools::WriteZToolsLog(strLogMsg);
					AddLog(strTaskId, strLogMsg);
					//pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("value")), ZTools::MBToUTF8(std::string("")));	
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				int nArrayLength = jFilesValue.size();
				//只处理第一个文件
				if (nArrayLength > 0)
				{
					Json::Value jFileValue = jFilesValue[Json::Value::ArrayIndex(0)];
					std::string strFileID = jFileValue["fileID"].asString();
					std::string strFileName = jFileValue["fileName"].asString();

					if (strFileID.empty())
					{
						strLogMsg = ZTools::FormatString("fileId为空，下载失败，参数名 %s，值 %s", strParamName.c_str(), strDefaultValue.c_str());
						ZTools::WriteZToolsLog(strLogMsg);
						AddLog(strTaskId, strLogMsg);
						pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("value")), ZTools::MBToUTF8(std::string("")));	
						pParameterElement = pParameterElement->NextSiblingElement("Parameter");
						continue;
					}

					std::string strLocalPath("");
					AddLog(strTaskId, ZTools::FormatString("开始下载输入文件:%s", strFileName.c_str()));
					strLocalPath = DownloadHprose(strFileID, strBasePath, strFileName);
					if (strLocalPath.length() != 0)
					{
						strLogMsg = ZTools::FormatString("下载输入文件完成:%s", strFileName.c_str());
						ZTools::WriteZToolsLog(strLogMsg);
						AddLog(strTaskId, strLogMsg);
						strNewValue = strLocalPath;
					}
					else
					{
						strLogMsg = ZTools::FormatString("下载输入文件失败:参数名： %s，文件名： %s，fileid %s", strParamName.c_str(), strFileName.c_str(), strFileID.c_str());
						ZTools::WriteZToolsLog(strLogMsg);
						AddLog(strTaskId, strLogMsg);
						strNewValue = "";
					}
				}
				pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("value")), ZTools::MBToUTF8(strNewValue));				

				pParameterElement = pParameterElement->NextSiblingElement("Parameter");
			}
		}
	}

	std::string strXml;
	strXml << xmlDocument;
	ZTools::UTF8ToMB(strXml);

	return strXml;
}

std::string RobotRemoteServiceI::ProcessInxml2( std::string strInxmlContext, std::string strBasePath )
{	
	ZTools::WriteZToolsFormatLog("多线程处理inxml");
	TiXmlBase::SetCondenseWhiteSpace(false);
	TiXmlDocument xmlDocument;
	ZTools::MBToUTF8(strInxmlContext);
	xmlDocument.Parse(strInxmlContext.c_str(), 0, TIXML_ENCODING_UTF8);

	TiXmlElement* rootElement = xmlDocument.RootElement();  //Parameters
	if (rootElement)
	{
		TiXmlElement* pPackageElement = rootElement->FirstChildElement("Package"); 
		if (pPackageElement)
		{
			pPackageElement->SetAttribute(ZTools::MBToUTF8(std::string("IsPopupForm")), ZTools::MBToUTF8(std::string("True")));
			//IsPopupForm
			TiXmlElement* pParameterElement = pPackageElement->FirstChildElement("Parameter");
			Json::Value jArray(Json::arrayValue); //下载列表
			while(pParameterElement)
			{
				//读取参数类型
				std::string strDataType;
				pParameterElement->QueryStringAttribute("type", &strDataType);
				if (strDataType.empty())
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				ZTools::UTF8ToMB(strDataType);
				if (strDataType.compare("File") != 0)
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				//对文件型参数处理
				std::string strDefaultValue;
				pParameterElement->QueryStringAttribute("value", &strDefaultValue);
				if (strDefaultValue.empty())
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}
				ZTools::UTF8ToMB(strDefaultValue);//得到文件fileId

				std::string strNewValue("");
				//////
				Json::Value jFilesValue;
				Json::Reader reader;
				if (!reader.parse(strDefaultValue, jFilesValue))
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				if (jFilesValue.type() != Json::arrayValue)
				{
					pParameterElement = pParameterElement->NextSiblingElement("Parameter");
					continue;
				}

				int nArrayLength = jFilesValue.size();
				//只处理第一个文件
				if (nArrayLength > 0)
				{
					Json::Value jFileValue = jFilesValue[Json::Value::ArrayIndex(0)];
					std::string strFileID = jFileValue["fileID"].asString();
					std::string strFileName = jFileValue["fileName"].asString();

					if (strFileID.empty())
					{
						pParameterElement = pParameterElement->NextSiblingElement("Parameter");
						continue;
					}	
					jFileValue["fileDir"] = strBasePath;
					jArray.append(jFileValue);
				}		
				pParameterElement = pParameterElement->NextSiblingElement("Parameter");
			}
			///////////////////////////////////////////////////////////////////////////////
			{//实际下载
				Json::FastWriter fastWriter;
				std::string strNewValue = fastWriter.write(jArray);
				strNewValue = DownloadHprose2(strNewValue);
				/////////////////////////////////////////////////////////////////////////////////
				Json::Reader jReader;
				Json::Value Jv;
				if(!jReader.parse(strNewValue, Jv) || !Jv.isObject()) return "";
				Json::Value::Members jms = Jv.getMemberNames();
				for(int i = 0; i < (int)jms.size(); i++)
				{
					Json::Value& JItem = Jv[jms[i]]; 
					if (pPackageElement)
					{
						//pPackageElement->SetAttribute(ZTools::MBToUTF8(std::string("IsPopupForm")), ZTools::MBToUTF8(std::string("True")));
						//IsPopupForm
						TiXmlElement* pParameterElement = pPackageElement->FirstChildElement("Parameter");
						while(pParameterElement)
						{
							//读取参数类型
							std::string strDataType;
							pParameterElement->QueryStringAttribute("type", &strDataType);
							if (strDataType.empty())
							{
								pParameterElement = pParameterElement->NextSiblingElement("Parameter");
								continue;
							}

							ZTools::UTF8ToMB(strDataType);
							if (strDataType.compare("File") != 0)
							{
								pParameterElement = pParameterElement->NextSiblingElement("Parameter");
								continue;
							}

							//对文件型参数处理
							std::string strDefaultValue;
							pParameterElement->QueryStringAttribute("value", &strDefaultValue);
							if (strDefaultValue.empty())
							{
								pParameterElement = pParameterElement->NextSiblingElement("Parameter");
								continue;
							}
							ZTools::UTF8ToMB(strDefaultValue);//得到文件fileId

							std::string strNewValue("");
							//////
							Json::Value jFilesValue;
							Json::Reader reader;
							if (!reader.parse(strDefaultValue, jFilesValue))
							{
								pParameterElement = pParameterElement->NextSiblingElement("Parameter");
								continue;
							}

							if (jFilesValue.type() != Json::arrayValue)
							{
								pParameterElement = pParameterElement->NextSiblingElement("Parameter");
								continue;
							}

							int nArrayLength = jFilesValue.size();
							//只处理第一个文件
							if (nArrayLength > 0)
							{
								Json::Value jFileValue = jFilesValue[Json::Value::ArrayIndex(0)];
								std::string strFileID = jFileValue["fileID"].asString();
								std::string strFileName = jFileValue["fileName"].asString();

								if (jms[i] == strFileID)
								{//fileId相同							
									pParameterElement->SetAttribute(ZTools::MBToUTF8(std::string("value")), ZTools::MBToUTF8(JItem["filePath"].asString()));		
								}				
							}		
							pParameterElement = pParameterElement->NextSiblingElement("Parameter");
						}
					}
				}		
			}
		}
	}
	std::string strXml;
	strXml << xmlDocument;
	ZTools::UTF8ToMB(strXml);
	return strXml;
}

char dec2hexChar(short int n) {
	if ( 0 <= n && n <= 9 ) {
		return char( short('0') + n );
	} else if ( 10 <= n && n <= 15 ) {
		return char( short('A') + n - 10 );
	} else {
		return char(0);
	}
}

short int hexChar2dec(char c) {
	if ( '0'<=c && c<='9' ) {
		return short(c-'0');
	} else if ( 'a'<=c && c<='f' ) {
		return ( short(c-'a') + 10 );
	} else if ( 'A'<=c && c<='F' ) {
		return ( short(c-'A') + 10 );
	} else {
		return -1;
	}
}

std::string escapeURL(const std::string &URL)
{
	std::string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if (
			( '0'<=c && c<='9' ) ||
			( 'a'<=c && c<='z' ) ||
			( 'A'<=c && c<='Z' ) ||
			c=='/' || c=='.'
			) {
				result += c;
		} else {
			int j = (short int)c;
			if ( j < 0 ) {
				j += 256;
			}
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1*16;
			result += '%';
			result += dec2hexChar(i1);
			result += dec2hexChar(i0);
		}
	}
	return result;
}

std::string deescapeURL(const std::string &URL) {
	std::string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if ( c != '%' ) {
			result += c;
		} else {
			char c1 = URL[++i];
			char c0 = URL[++i];
			int num = 0;
			num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
			result += char(num);
		}
	}
	return result;
}
std::string RobotRemoteServiceI::DownloadHprose( std::string strFileId, std::string strFileDir, std::string strFileName )
{
	CString sHproseDownloadUrl("");
	sHproseDownloadUrl = GetConfig("hprose", "hproseDownloadUrl");
	if(sHproseDownloadUrl.IsEmpty())
	{
		ZTools::WriteZToolsLog("未找到sHproseDownloadUrl");
		return "";
	}

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strHproseRpc = (LPCTSTR)sHproseDownloadUrl;
	dataFileObject.m_strFileId = strFileId;

	std::string strRefFilePath;
	if (CDownUploadFileCacheFinder::ReadDownloadCache(&dataFileObject))
	{
		strRefFilePath = dataFileObject.m_strFilePath;
	}
	else
	{
		//未找到缓存就调com组件下载（性能降低）
		strRefFilePath = CallDownloadHprose(std::string((LPCTSTR)sHproseDownloadUrl), strFileId, strFileName);
	}
	
	if (strRefFilePath.empty())
	{
		ZTools::WriteZToolsFormatLog("下载文件失败：%s, %s", strFileId.c_str(), strFileName.c_str());
		return "";
	}

	if (strFileDir.empty())
	{
		return strRefFilePath;
	}
	else
	{
		std::string strDstFilePath;
		if (strFileName.empty())
		{
			strFileName = strRefFilePath.substr(strRefFilePath.find_last_of('\\') + 1, strRefFilePath.length() - strRefFilePath.find_last_of('\\') - 1);
		}
			
		std::replace(strFileDir.begin(), strFileDir.end(), '/', '\\');
		strFileDir.erase(strFileDir.find_last_not_of('\\') + 1);
		strDstFilePath = ZTools::FormatString("%s\\%s", strFileDir.c_str(), strFileName.c_str());

		MakeSureDirectoryPathExists(strDstFilePath.c_str());
		CopyFile(strRefFilePath.c_str(), strDstFilePath.c_str(), FALSE);
		if (PathFileExists(strDstFilePath.c_str()))
		{
			return strDstFilePath;
		}
		else
		{
			return "";
		}
	}	
}


std::string RobotRemoteServiceI::DownloadHprose2( std::string strJson )
{
	CString sHproseDownloadUrl("");
	sHproseDownloadUrl = GetConfig("hprose", "hproseDownloadUrl");
	if(sHproseDownloadUrl.IsEmpty())
	{
		ZTools::WriteZToolsLog("未找到sHproseDownloadUrl");
		return "";
	}

	strJson = CallDownloadFilesHprose(std::string((LPCTSTR)sHproseDownloadUrl), strJson);

	return strJson;

}

::std::string RobotRemoteServiceI::runLocalRobotStart(const ::std::string& strInfo, const ::Ice::Current& /*= ::Ice::Current() */)
{
	std::string strMyInfo(strInfo);
	ZTools::UTF8ToMB(strMyInfo);
	ZTools::WriteZToolsLog(
		ZTools::FormatString("Functiong RobotRemoteServiceI::runLocalRobotStart\nstrInfo:%s", 
		strMyInfo.c_str())
		);

	Json::Value jInfo;
	Json::Reader reader;

	if (!reader.parse(strMyInfo, jInfo))
	{
		std::string strError("PeraTaskService: json format error!");
		ZTools::WriteZToolsLog(strError);
		return strError;
	}
	//得到任务id
	std::string strId = jInfo["id"].asString();
	/*if (strId.empty())
	{
	std::string strError("PeraTaskService: id empty!");
	ZTools::WriteZToolsLog(strError);
	return strError;		
	}*/

	/*std::set<std::string>::iterator itIdPos = m_setId.find(strId);
	if (itIdPos != m_setId.end())
	{
	std::string strError("PeraTaskService: id already exist!");
	ZTools::WriteZToolsLog(strError);
	return strError;	
	}

	m_setId.insert(strId);*/
	ZTools::WriteZToolsFormatLog("【】收到任务准备分发开始计时点：%s", strId.c_str());

	//增加runType区分执行逻辑
	//00:普通组件；01：带有表单的组件；02：带有表单的子流程   0开头为表单初始执行
	//12：带有表单的子流程接受输出值，更新页面状态，确定按钮可以用  1开头为更新表单状态
	//20：关闭表单(流程暂停,终止)  2开头为关闭表单 增加key名称为idList(json数组（[1,2,3,4]）)
	//引擎通知客户端暂停的多个节点ID，然后客户端再通知封装，关闭对应表单
	//该处strRunType为启动任务异常时，直接调后台所带的参数，所以区分组件和子流程单独处理
	std::string strRunType = jInfo["runType"].asString();
	if (strRunType == "00" || strRunType == "01")
	{
		strRunType = "";
	}
	else if (strRunType == "02" || strRunType== "13" ||strRunType== "14")
	{
		strRunType = "022";
	}
	else
	{
		strRunType = "";
	}

	PrepareComponentDataThreadData* prepareComponentDataThreadData = new PrepareComponentDataThreadData;
	prepareComponentDataThreadData->strInfo = strMyInfo;
	prepareComponentDataThreadData->strId = strId;
	prepareComponentDataThreadData->strRunType = strRunType;
	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, PrepareComponentDataThreadLocal
		, (LPVOID)prepareComponentDataThreadData
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		delete prepareComponentDataThreadData;
		prepareComponentDataThreadData = NULL;
		std::string strError("PrepareComponentDataThread fail");
		ZTools::WriteZToolsLog(strError);
		return strError;	
	}
	ResumeThread(hThread);

	::CloseHandle(hThread);


	// 	ZTools::ExportFromSvn("svn://192.168.50.206:3690/MyProject/fileRoot/MyProject", "/svn/template/component/62880/我的6.bot",
	// 	"", "d:\\aaa.bot", "fileRoot", "123456");

	std::string strReturn("");
	//strReturn = strMyInfo;
	ZTools::WriteZToolsLog(
		ZTools::FormatString("Functiong RobotRemoteServiceI::runRobotStart返回值\nstrReturn:%s", 
		strReturn.c_str())
		);
	ZTools::MBToUTF8(strReturn);
	return strReturn;
	//throw std::exception("The method or operation is not implemented.");
}

DWORD WINAPI RobotRemoteServiceI::PrepareComponentDataThreadLocal( LPVOID lpParameter )
{
	PrepareComponentDataThreadData* threadData = (PrepareComponentDataThreadData*)lpParameter;
	std::string strId = threadData->strId;
	std::string strRunType = threadData->strRunType;

	if (PrepareComponentDataThreadFuncLocal(lpParameter) == FALSE)
	{
		//CallWsIceRunRobotFinished(GetWsIceEndPoints(), strId, "");
		AmiCallWsIceRunLocalRobotFinished(strId, "", strRunType);
	}

	return 0;
}

BOOL RobotRemoteServiceI::PrepareComponentDataThreadFuncLocal(LPVOID lpParameter)
{
	PrepareComponentDataThreadData* threadData = (PrepareComponentDataThreadData*)lpParameter;
	std::string strInfo = threadData->strInfo;
	delete threadData;
	threadData = NULL;

	//检查执行环境
	CString sProcessServicePath = GetProcessServicePath();
	CString sComponentBinPath = GetFlexwareBinPathMethod();
	if (sProcessServicePath.IsEmpty())
	{
		ZTools::WriteZToolsLog("未找到PeraProcessService.exe");
		return FALSE;
	}

	Json::Value jInfo;
	Json::Reader reader;

	if (!reader.parse(strInfo, jInfo))
	{
		std::string strError("PeraTaskService: json format error!");
		ZTools::WriteZToolsLog(strError);
		return FALSE;
	}
	//{"id":"001",
	//	"taskName":"任务1",
	//	"robotFile":"\robot\002\组件a.bot",
	//	"inputFile":"\files\input.xml",
	//	" taskUser ":"admin",
	//	"assignNode":"Node01",
	//	"taskInfo":"表单名称",
	//	"processName":"流程名称",
	//	"isForm":"1"}//1前表单，2后表单
	//得到任务id
	std::string strId = jInfo["id"].asString();
	std::string strTaskName = jInfo["taskName"].asString();
	if (strTaskName.empty())
	{
		strTaskName = "temp";
	}

	std::string strInxmlContext = jInfo["inputFile"].asString();
	std::string strTaskUser = jInfo["taskUser"].asString();
	std::string strAssignNode = jInfo["assignNode"].asString();
	std::string strTaskInfo = jInfo["taskInfo"].asString();
	std::string strProcessName = jInfo["processName"].asString();
	std::string strIsForm = jInfo["isForm"].asString();
	std::string strRunType = jInfo["runType"].asString();  //attachType 运行类型，对应引擎传来的runType属性
	std::string strFormFile = jInfo["formFile"].asString(); // 表单BOT文件路径
	std::string strFormTitle = jInfo["formTitle"].asString(); // 暂定作业名称+节点名称
	Json::Value jidList = jInfo["idList"];    //当attachType为20时有效，数组中为要关闭的表单的taskId
	std::string strRunID = jInfo["runId"].asString(); 

	std::string strIsProcessPreForm = jInfo["isProcessPreForm"].asString();
	if (strIsProcessPreForm.compare("true") == 0)
	{
		Sleep(2000);
	}

	//单机版中，执行设计过程时，中间件准备节点的输入数据及指定组件的执行输出路径
	CString strCacheRoot = GetPeraGlobalDataFromReg("lm","PeraPRCacheDir"); 
	if(strCacheRoot.IsEmpty())
		strCacheRoot =  "C:\\WorkSpace\\Data\\RunCache";

	std::string strBasePath = ZTools::FormatString("%s\\%s\\ParamData\\%s_%s\\"
		, strCacheRoot, strRunID.c_str(),strTaskName.c_str(),strId.c_str());


	if (!MakeSureDirectoryPathExists(strBasePath.c_str()))
	{
		DWORD dError = GetLastError();
		std::string s = ZTools::FormatString("创建文件夹失败,%d,%s", dError, strBasePath.c_str());
		ZTools::WriteZToolsLog(s);
		return FALSE;
	}
	std::string strInDir = strBasePath + "in\\";
	if (!MakeSureDirectoryPathExists(strInDir.c_str()))
	{
		DWORD dError = GetLastError();
		std::string s = ZTools::FormatString("创建int文件夹失败,%d,%s", dError, strInDir.c_str());
		ZTools::WriteZToolsLog(s);
		return FALSE;
	}
	std::string strOutDir = strBasePath + "out\\";
	if (!MakeSureDirectoryPathExists(strOutDir.c_str()))
	{
		DWORD dError = GetLastError();
		std::string s = ZTools::FormatString("创建out文件夹失败,%d,%s", dError, strOutDir.c_str());
		ZTools::WriteZToolsLog(s);
		return FALSE;
	}
	//AddLogLocal(strId, "准备输入数据");

#ifdef _TEST_PERFORMANCE_
	clock_t clockStart, clockEnd;
	double dDuration;
	clockStart = clock();
#endif // _TEST_PERFORMANCE_
	//ZTools::WriteZToolsFormatLog("【】中间件准备输入开始计时点");
	//处理inxml
	//strInxmlContext = ProcessInxml(strInxmlContext, strBasePath, strId);
	//strInxmlContext = ProcessInxml2(strInxmlContext, strBasePath);
	ZTools::WriteZToolsLog(strInxmlContext);

	//AddLogLocal(strId, "准备组件文件");

#ifdef _TEST_PERFORMANCE_
	clockEnd = clock();
	dDuration = (double)(clockEnd - clockStart) / CLOCKS_PER_SEC;
	std::string strDownloadTimeMsg = ZTools::FormatString("中间件下载组件及所有输入文件用时：%f秒", dDuration);
	AddLogLocal(strId, strDownloadTimeMsg);

	ZTools::WriteZToolsLog(strDownloadTimeMsg);
#endif // _TEST_PERFORMANCE_

	AddRateLocal(strId, "30");

	//运行
	JosnDataRunRobot jsonDataRunRobot;
	jsonDataRunRobot.Clear();
	jsonDataRunRobot.taskId = strId;
	jsonDataRunRobot.flag = "WSLocal";
	jsonDataRunRobot.inXml = strInxmlContext;
	jsonDataRunRobot.outPath = strOutDir;
	jsonDataRunRobot.attachType = strRunType;
	jsonDataRunRobot.taskName = strTaskName;
	jsonDataRunRobot.processName = strProcessName;

	if (strRunType == "20")
	{
		if (!jidList.isArray() )
			return FALSE;
		jsonDataRunRobot.idList = jidList.toStyledString(); //当attachType为20时有效，数组中为要关闭的表单的taskId
	}

	if (strRunType == "00")
	{
		jsonDataRunRobot.botPath = jInfo["robotFile"].asString(); // 当attachType为00时，传组件路径，对应引擎传来的robotFile
	}
	else if (strRunType == "01" || strRunType == "02" || strRunType== "13"|| strRunType== "14")
	{
		jsonDataRunRobot.botPath = strFormFile;   //当attachType为01、02时，传表单路径，对应引擎传来的formFile
		jsonDataRunRobot.title = strFormTitle;   // 如果是表单，该属性用于表单的标题，对应于引擎传来对象中的formTitle
		if (strRunType == "01")
		{
			jsonDataRunRobot.attachData = jInfo["robotFile"].asString(); //当attacheType为01时，传组件路径，对应引擎传来的robotFile
			//attachType为其它值，attachData赋空字符串
		}
	}


	ZTools::WriteZToolsFormatLog("【】中间件分发任务结束计时点：%s", strId.c_str());
	if (!FindProcessByName("PeraProcessService.exe"))
	{
		ZTools::WriteZToolsFormatLog("第一次查找PeraProcessService进程，未找到");
		g_critical_section_ProcessService.Lock();
		if (!FindProcessByName("PeraProcessService.exe"))
		{
			ZTools::WriteZToolsFormatLog("第二次查找PeraProcessService进程，未找到");
			SHELLEXECUTEINFO si;
			memset(&si, NULL, sizeof(si));
			si.cbSize = sizeof(si);
			si.nShow = SW_HIDE;
			si.fMask = SEE_MASK_NOCLOSEPROCESS;
			si.lpVerb = _T("open");
			si.lpDirectory = sComponentBinPath;
			si.lpFile = sProcessServicePath;
			si.lpParameters = NULL;
			if (!ShellExecuteEx(&si))
			{
				ZTools::WriteZToolsFormatLog("启动PeraProcessService进程出现异常");
				g_critical_section_ProcessService.Unlock();
				return FALSE;
			}
		}
		else
		{
			ZTools::WriteZToolsFormatLog("第二次查找PeraProcessService进程存在");
		}
		g_critical_section_ProcessService.Unlock();
	}
	else
	{
		ZTools::WriteZToolsFormatLog("第一次查找PeraProcessService进程存在");
	}
	if (!CallRunRobot(jsonDataRunRobot.ToJson()))
	{
		for (int i = 0; i < 20; i++)
		{
			Sleep(500);
			if (!CallRunRobot(jsonDataRunRobot.ToJson()))
			{
				if (i == 19)
				{
					if (!FindProcessByName("PeraProcessService.exe"))
					{
						ZTools::WriteZToolsFormatLog("调用runtime的启动接口失败，失败时PeraProcessService进程未找到");
					}
					else
					{
						ZTools::WriteZToolsFormatLog("调用runtime的启动接口失败，失败时PeraProcessService进程存在");
					}
					return FALSE;
				}
			}
			else
			{
				break;
			}
		}
	}

	return TRUE;
}
