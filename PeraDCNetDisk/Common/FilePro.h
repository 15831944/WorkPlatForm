/*
FilePro.h 本地磁盘文件操作封装类
*/
#pragma once
#include <vector>
#include <string>
using  namespace std;

typedef struct 
{
	string m_strExt; // 后缀名
	string m_strDesrciption; //描述符
}NewType;

typedef struct _Folder_Item
{
	CString     m_strPath;
	bool        m_bIsFile;
	vector<_Folder_Item> m_SubItem;
	_Folder_Item()
	{
		m_strPath = "";
		m_bIsFile = false;
	}
}Folder_Item;

typedef struct _SelectItem
{
	CString     m_strPath;
	bool        m_bIsFile;
	_SelectItem()
	{
		m_strPath = "";
		m_bIsFile = true;
	}
}SelectItem;

class CFilePro
{
public:
	typedef struct 
	{
		long long iFileSize;
		CString m_strFileName;
		CString m_strFullPath;
		CString m_strCreateTime;
		CString m_strLastModifyTime;
		CString m_strLastAccessTime;
		CString m_strFileType; // 文件类型
		CString m_strOpenWithType; // 打开方式名称
		CString m_strFileIcon;  // 文件图标
	}SFileInfo;

	CFilePro(void);
	~CFilePro(void);
	CString GetErrMsg(){return m_strErrMsg;}
	int     GetErrCode(){return m_iErrCode;}
	void ClearErrMsg(){m_strErrMsg = _T("");}

	/*
	选择文件对话框
	*/
	vector<CString> GetSelectedFile(BOOL bMutilSelect = true, const char * pszDefExt = NULL,const char *pszFilter = NULL);

	/*
	多功能文件或文件夹选择
	*/
	vector<SelectItem> GetSelectItems(const char * pszSelMode = NULL,const char *pszFilter = NULL);
	/*
	选择目录对话框
	*/
	CString GetSelectedFolder();
	/*
	打开指定的文件
	*/
	bool OpenFile(CString strFile,HANDLE * h = NULL);

	/*
	获取文件的MD5值
	*/
	bool GetFileMD5(CString strInFile,CString & strOutMd5);

	/*
	获取文件的信息
	*/
	bool GetFileInfo(CString strInFile,SFileInfo &fileInfo,const char *pszTimeFormat = "%Y-%m-%d %H:%M:%S");

	/*
	删除本地文件
	*/
	bool DeleteLocalFile(const CString & strFile,BOOL bAllowUndo = TRUE);
	/*
	删除文件夹
	*/
	bool RemoveDir(const CString &);
	/*
	重命名本地文件
	*/
	bool ReNameLocalFile(const CString & strFile,const CString & strNewName);
	
	/* 
	获取所有右键菜单新建文件类型
	*/
	void GetAllDesktopRightMenuNewItems(vector<NewType *> &vAllItems,BOOL bOnlyOfficeType = FALSE);
	/*
	/* 
	获取所有右键菜单新建文件类型的图标png和icon文件
	*/
	void GetAllNewItemsIconFile(vector<NewType *> &vAllItems,CString strDestDir);
	/*

	在指定目录下创建指定类型的新文件
	*/
	bool CreateNewItem(string strItemType,string strDir,string &strNewPath);

	bool GetFolderItems(CString strFolder,Folder_Item &Items,bool bRecursion = false,bool bInCludeFile = true,bool bIncludeDir =true,bool RelativePath = true);

	static void GetFolderItemCount(const struct _Folder_Item & fItem,int & iFolderCount,int &iFileCount,const bool & bRecursion);
private:
	CString m_strErrMsg;
	int     m_iErrCode;
};
