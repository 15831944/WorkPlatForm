#include "stdafx.h"
#include "CommonMethod.h"
#include "StlHeader.h"
#include <io.h>
#include <direct.h>
#include "ConfigFunc.h"
#include "FileFunc.h"

bool IsAbsolutePath(const ::std::string & strPath)
{
	if (strPath.size() >1 && strPath[0]== '\\' && strPath[1]== '\\')
		return true; // 开头以"\\"开始的当做绝对路径
	else if (strPath.size()>1 && strPath[1]==':')
		return true;

	return false;
}
std::string ConvertToAbsolutePath(const ::std::string & strRelativePath,
	const ::std::string & strUserName,const ::std::string & strFlowID)
{
	CString cstrTmp = strRelativePath.c_str();
	cstrTmp.Replace("/","\\");

	string strRelativePathTmp = (LPCTSTR)cstrTmp;

	if (IsAbsolutePath(strRelativePathTmp))
		return strRelativePathTmp; // 本来就是绝对路径的不转换

	string strCacheDir =  ::GetCacheDir();
	strCacheDir = strCacheDir + string("\\") + strUserName + string("\\") + strFlowID;
	if(!strRelativePathTmp.empty())
		strCacheDir = strCacheDir + string("\\") + strRelativePathTmp;

	cstrTmp = strCacheDir.c_str();
	cstrTmp.Replace("\\\\","\\");

	return (LPCTSTR)cstrTmp;
}

std::string GetFileName(const ::std::string &strIn)
{
	int iPos = strIn.find_last_of('\\');
	string str = strIn.substr(iPos+1);
	return str;
}
bool RMakeDir(std::string strDir)
{
	if (access(strDir.c_str(),0)==0)
		return true;

	char   DirName[500] ={0};
	strcpy(DirName,strDir.c_str());
	int  ilen = strDir.length(); 
	if(DirName[ilen-1]!='\\')
		strcat(DirName,   "\\");

	ilen  = strlen(DirName); 

	for(int i=1;   i<ilen;   i++) 
	{ 
		if(DirName[i]=='\\') 
		{
			DirName[i]   =   0; 
			if( access(DirName,  0)!=0) 
			{ 
				if(_mkdir(DirName)==-1) 
				{  
					return   false;  
				} 
			} 
			DirName[i]   =   '\\'; 
		} 
	} 
	return   true; 
}  
std::string GetCacheDir()
{
	//static CString  g_CacheDir = "";
	//if (!g_CacheDir.IsEmpty())
	//	return (LPCTSTR)g_CacheDir;

	CString g_CacheDir = GetConfig("PeraDCNetDisk","CacheDir");
	if (g_CacheDir.IsEmpty())
	{
		g_CacheDir = GetExecDir() + "\\CacheDir";
		return (LPCTSTR)g_CacheDir;
	}

	return (LPCTSTR)g_CacheDir;
}
BOOL SetCacheDir(const ::std::string & strCacheDir)
{
	return SetConfig("PeraDCNetDisk","CacheDir",strCacheDir.c_str());
}
void NormalPath(CString &strPath)
{
	strPath.Replace("/","\\");
	strPath.Replace("\\\\","\\");
}
BOOL IsFileReadOnly(LPCTSTR strFile)
{
	if (strFile==NULL || strFile == "")
		return FALSE;

	DWORD dwAttrs = GetFileAttributes(strFile);
	BOOL  bReadOnly = (dwAttrs & FILE_ATTRIBUTE_READONLY);
	return bReadOnly;
}
BOOL SetFileReadOnly(LPCTSTR strFile,BOOL bAdd)
{
	if (strFile==NULL || strFile == "")
		return FALSE;

	DWORD dwAttrs = GetFileAttributes(strFile);
	if (bAdd)
		dwAttrs |= FILE_ATTRIBUTE_READONLY;
	else
		dwAttrs &= ~FILE_ATTRIBUTE_READONLY;

	return SetFileAttributes(strFile,dwAttrs);
}
CString ReplaceFlowSc(LPCTSTR strFlowId)
{
	BOOL b = TRUE;
	LPCTSTR p = strFlowId ? strFlowId :"";
	unsigned char c;
	CString strOutPath;

	while(c = *p)
	{
		// 替换不允许出现在文件夹名称的一些字符
		if ( c== ':'|| c == '*' || c== '?'|| c == '\"' || c== '<'|| c == '>' || c== '|')
			strOutPath.AppendChar('_');

		else
			strOutPath.AppendChar(*p);
		p++;
	}

	strOutPath.Replace("/","\\");
	strOutPath.Replace("\\\\","\\");

	return strOutPath;
}