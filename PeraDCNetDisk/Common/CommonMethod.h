#pragma once

extern bool IsAbsolutePath(const ::std::string & strPath);
extern std::string ConvertToAbsolutePath(const ::std::string & strRelativePath,
	const ::std::string & strUserName,const ::std::string & strFlowID);

extern std::string GetCacheDir();
extern BOOL SetCacheDir(const ::std::string & strCacheDir);

extern std::string GetFileName(const ::std::string &strIn);
extern bool RMakeDir(std::string strDir);
extern  void NormalPath(CString &strPath);
extern BOOL IsFileReadOnly(LPCTSTR);
extern BOOL SetFileReadOnly(LPCTSTR,BOOL bAdd);
extern CString ReplaceFlowSc(LPCTSTR strFlowId);