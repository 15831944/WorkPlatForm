#include "StdAfx.h"
#include "FileTool.h"


CFileTool::CFileTool(void)
{
}


CFileTool::~CFileTool(void)
{
}
//删除文件或者文件夹
bool CFileTool::DeleteFile(char * lpszPath) 
{ 
	SHFILEOPSTRUCT FileOp={0}; 
	FileOp.fFlags = FOF_NOCONFIRMATION; //不出现确认对话框
	FileOp.pFrom = lpszPath; 
	FileOp.pTo = NULL;      //一定要是NULL
	FileOp.wFunc = FO_DELETE;    //删除操作
	return SHFileOperation(&FileOp) == 0; 
}

//复制文件或文件夹
bool CFileTool::CopyFile(char *pTo,char *pFrom)
{
	SHFILEOPSTRUCT FileOp; 
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
	FileOp.fFlags = FOF_NOCONFIRMATION|   //不出现确认对话框
		FOF_NOCONFIRMMKDIR ; //需要时直接创建一个文件夹,不需用户确定
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

//移动文件或文件夹
bool CFileTool::MoveFile(char *pTo,char *pFrom)
{
	SHFILEOPSTRUCT FileOp={0}; 
	FileOp.fFlags = FOF_NOCONFIRMATION|   //不出现确认对话框 
		FOF_NOCONFIRMMKDIR ; //需要时直接创建一个文件夹,不需用户确定
	FileOp.pFrom = pFrom; 
	FileOp.pTo = pTo; 
	FileOp.wFunc = FO_MOVE; 
	return SHFileOperation(&FileOp) == 0;   
}


//从命名文件或文件夹
bool CFileTool::ReNameFile(char *pTo,char *pFrom)
{
	SHFILEOPSTRUCT FileOp={0}; 
	FileOp.fFlags = FOF_NOCONFIRMATION;   //不出现确认对话框 
	FileOp.pFrom = pFrom; 
	FileOp.pTo = pTo; 
	FileOp.wFunc = FO_RENAME; 
	return SHFileOperation(&FileOp) == 0;   
}