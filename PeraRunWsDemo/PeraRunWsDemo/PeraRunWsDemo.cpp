// PeraRunWsDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "PeraRunWsDemo.h"
#include "ProcessCore.h"
#include "Tools.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

BOOL IsWsFilePath(CString str)
{
	if (str.GetLength() <= 3)
		return FALSE;

	CString strExt = str.Right(3);
	strExt.MakeLower();
	if (strExt == ".ws")
		return TRUE;

	return FALSE;
}
void PrintfHelp()
{
	printf("使用帮助:\nPeraRunWsDemo.exe -ws wsfile -showProcess true[false]\
		\n示例 PeraRunWsDemo.exe -ws \"c:\\desktop\\123.ws\" -showProcess true\n");
}
bool GetCmdParam(int argc,TCHAR *argv[],CString &strWsFile,BOOL & bShowProgress)
{
	CString strInParam;
	for (int i=0;i<argc;i++)
	{
		strInParam += argv[i];
		strInParam += " ";
	}
	ZTools::WriteZToolsFormatLog("输入命令行参数为：%s\n",strInParam);
	strWsFile = "";
	bShowProgress  = FALSE;
	int iIndex = 0;
	BOOL bFindShowProgress = FALSE;
	for (int i=1;i<argc;)
	{
		if (_stricmp(argv[i],"-ws")==0)
		{
			if (!strWsFile.IsEmpty())
				goto ERR_CMD_PARAM; // 重复的参数

			iIndex = ++i;
			if (iIndex >= argc)
			{
				goto ERR_CMD_PARAM; // 参数不足
			}
			else if (!IsWsFilePath(argv[iIndex]))
			{
				goto ERR_CMD_PARAM; // 无效的参数
			}
			strWsFile = argv[iIndex];
		}
		else if (_stricmp(argv[i],"-showProcess")==0)
		{
			if (bFindShowProgress)
				goto ERR_CMD_PARAM; // 重复的参数

			iIndex = ++i;
			if (iIndex >= argc)
			{
				goto ERR_CMD_PARAM;  // 参数不足
			}
			else if (_stricmp(argv[iIndex],"true")==0)
				bShowProgress = TRUE;
			else if (_stricmp(argv[iIndex],"false")==0)
				bShowProgress = FALSE;
			else
			{
				goto ERR_CMD_PARAM; // 无效的参数
			}
			bFindShowProgress = TRUE;
		}
		else
		{
			goto ERR_CMD_PARAM; // 无效的参数
		}
		i++;
	}
	if (strWsFile.IsEmpty())
	{
		goto ERR_CMD_PARAM;
	}

	return true;

ERR_CMD_PARAM:
	printf("命令行参数格式错误...\n");
	return false;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
			CString strWsFile = "";
			BOOL bShowProgress = FALSE;
			if (GetCmdParam(argc,argv,strWsFile,bShowProgress))
			{
				if (ExcuteWs(strWsFile,bShowProgress))
				{
					ZTools::WriteZToolsFormatLog("%s执行成功!\n",strWsFile);
					printf("%s执行成功!\n",strWsFile);
				}
				else
				{
					ZTools::WriteZToolsFormatLog("%s执行失败!\n",strWsFile);
					printf("%s执行失败!\n",strWsFile);
				}

			}
			else
			{
				PrintfHelp();
				return -1;
			}
		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
