#pragma once

extern CString GetCurModuleName(void);
extern CString GetExecDir(void);
extern BOOL IsAccessibleDir(LPCTSTR strPath);
extern BOOL IsAccessibleFile(LPCTSTR lpszFile);
extern CString CreateSubDir(LPCTSTR szParentDir, LPCTSTR szChildFolder);
extern CString GetCurUserAppDataDir(void);
extern void DeleteFiles(LPCTSTR lpszPath);
extern LONGLONG FileSize(CString strFile);