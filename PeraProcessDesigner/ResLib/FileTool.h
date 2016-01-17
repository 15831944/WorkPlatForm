#pragma once
class CFileTool
{
public:
	CFileTool(void);
	virtual ~CFileTool(void);

public:
	static bool DeleteFile(char * lpszPath) ;
	static bool CopyFile(char *pTo,char *pFrom);
	static bool MoveFile(char *pTo,char *pFrom);
	static bool ReNameFile(char *pTo,char *pFrom);
};

