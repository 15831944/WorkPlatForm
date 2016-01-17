#include "StdAfx.h"
#include "FileTool.h"


CFileTool::CFileTool(void)
{
}


CFileTool::~CFileTool(void)
{
}
//ɾ���ļ������ļ���
bool CFileTool::DeleteFile(char * lpszPath) 
{ 
	SHFILEOPSTRUCT FileOp={0}; 
	FileOp.fFlags = FOF_NOCONFIRMATION; //������ȷ�϶Ի���
	FileOp.pFrom = lpszPath; 
	FileOp.pTo = NULL;      //һ��Ҫ��NULL
	FileOp.wFunc = FO_DELETE;    //ɾ������
	return SHFileOperation(&FileOp) == 0; 
}

//�����ļ����ļ���
bool CFileTool::CopyFile(char *pTo,char *pFrom)
{
	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
	FileOp.fFlags = FOF_NOCONFIRMATION|   //������ȷ�϶Ի���
		FOF_NOCONFIRMMKDIR ; //��Ҫʱֱ�Ӵ���һ���ļ���,�����û�ȷ��
	FileOp.pFrom = pFrom; 
	FileOp.pTo = pTo; 
	FileOp.wFunc = FO_COPY; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 

	FileOp.wFunc = FO_COPY; 
	int nret = SHFileOperation(&FileOp);
	return  nret == 0;
}

//�ƶ��ļ����ļ���
bool CFileTool::MoveFile(char *pTo,char *pFrom)
{
	SHFILEOPSTRUCT FileOp={0}; 
	FileOp.fFlags = FOF_NOCONFIRMATION|   //������ȷ�϶Ի��� 
		FOF_NOCONFIRMMKDIR ; //��Ҫʱֱ�Ӵ���һ���ļ���,�����û�ȷ��
	FileOp.pFrom = pFrom; 
	FileOp.pTo = pTo; 
	FileOp.wFunc = FO_MOVE; 
	return SHFileOperation(&FileOp) == 0;   
}


//�������ļ����ļ���
bool CFileTool::ReNameFile(char *pTo,char *pFrom)
{
	SHFILEOPSTRUCT FileOp={0}; 
	FileOp.fFlags = FOF_NOCONFIRMATION;   //������ȷ�϶Ի��� 
	FileOp.pFrom = pFrom; 
	FileOp.pTo = pTo; 
	FileOp.wFunc = FO_RENAME; 
	return SHFileOperation(&FileOp) == 0;   
}