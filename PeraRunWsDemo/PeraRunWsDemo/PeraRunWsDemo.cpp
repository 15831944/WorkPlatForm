// PeraRunWsDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "PeraRunWsDemo.h"
#include "ProcessCore.h"
#include "Tools.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

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
	printf("ʹ�ð���:\nPeraRunWsDemo.exe -ws wsfile -showProcess true[false]\
		\nʾ�� PeraRunWsDemo.exe -ws \"c:\\desktop\\123.ws\" -showProcess true\n");
}
bool GetCmdParam(int argc,TCHAR *argv[],CString &strWsFile,BOOL & bShowProgress)
{
	CString strInParam;
	for (int i=0;i<argc;i++)
	{
		strInParam += argv[i];
		strInParam += " ";
	}
	ZTools::WriteZToolsFormatLog("���������в���Ϊ��%s\n",strInParam);
	strWsFile = "";
	bShowProgress  = FALSE;
	int iIndex = 0;
	BOOL bFindShowProgress = FALSE;
	for (int i=1;i<argc;)
	{
		if (_stricmp(argv[i],"-ws")==0)
		{
			if (!strWsFile.IsEmpty())
				goto ERR_CMD_PARAM; // �ظ��Ĳ���

			iIndex = ++i;
			if (iIndex >= argc)
			{
				goto ERR_CMD_PARAM; // ��������
			}
			else if (!IsWsFilePath(argv[iIndex]))
			{
				goto ERR_CMD_PARAM; // ��Ч�Ĳ���
			}
			strWsFile = argv[iIndex];
		}
		else if (_stricmp(argv[i],"-showProcess")==0)
		{
			if (bFindShowProgress)
				goto ERR_CMD_PARAM; // �ظ��Ĳ���

			iIndex = ++i;
			if (iIndex >= argc)
			{
				goto ERR_CMD_PARAM;  // ��������
			}
			else if (_stricmp(argv[iIndex],"true")==0)
				bShowProgress = TRUE;
			else if (_stricmp(argv[iIndex],"false")==0)
				bShowProgress = FALSE;
			else
			{
				goto ERR_CMD_PARAM; // ��Ч�Ĳ���
			}
			bFindShowProgress = TRUE;
		}
		else
		{
			goto ERR_CMD_PARAM; // ��Ч�Ĳ���
		}
		i++;
	}
	if (strWsFile.IsEmpty())
	{
		goto ERR_CMD_PARAM;
	}

	return true;

ERR_CMD_PARAM:
	printf("�����в�����ʽ����...\n");
	return false;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
			CString strWsFile = "";
			BOOL bShowProgress = FALSE;
			if (GetCmdParam(argc,argv,strWsFile,bShowProgress))
			{
				if (ExcuteWs(strWsFile,bShowProgress))
				{
					ZTools::WriteZToolsFormatLog("%sִ�гɹ�!\n",strWsFile);
					printf("%sִ�гɹ�!\n",strWsFile);
				}
				else
				{
					ZTools::WriteZToolsFormatLog("%sִ��ʧ��!\n",strWsFile);
					printf("%sִ��ʧ��!\n",strWsFile);
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
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
