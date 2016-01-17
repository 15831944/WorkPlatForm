/*
FilePro.h ���ش����ļ�������װ��
*/
#pragma once
#include <vector>
#include <string>
using  namespace std;

typedef struct 
{
	string m_strExt; // ��׺��
	string m_strDesrciption; //������
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
		CString m_strFileType; // �ļ�����
		CString m_strOpenWithType; // �򿪷�ʽ����
		CString m_strFileIcon;  // �ļ�ͼ��
	}SFileInfo;

	CFilePro(void);
	~CFilePro(void);
	CString GetErrMsg(){return m_strErrMsg;}
	int     GetErrCode(){return m_iErrCode;}
	void ClearErrMsg(){m_strErrMsg = _T("");}

	/*
	ѡ���ļ��Ի���
	*/
	vector<CString> GetSelectedFile(BOOL bMutilSelect = true, const char * pszDefExt = NULL,const char *pszFilter = NULL);

	/*
	�๦���ļ����ļ���ѡ��
	*/
	vector<SelectItem> GetSelectItems(const char * pszSelMode = NULL,const char *pszFilter = NULL);
	/*
	ѡ��Ŀ¼�Ի���
	*/
	CString GetSelectedFolder();
	/*
	��ָ�����ļ�
	*/
	bool OpenFile(CString strFile,HANDLE * h = NULL);

	/*
	��ȡ�ļ���MD5ֵ
	*/
	bool GetFileMD5(CString strInFile,CString & strOutMd5);

	/*
	��ȡ�ļ�����Ϣ
	*/
	bool GetFileInfo(CString strInFile,SFileInfo &fileInfo,const char *pszTimeFormat = "%Y-%m-%d %H:%M:%S");

	/*
	ɾ�������ļ�
	*/
	bool DeleteLocalFile(const CString & strFile,BOOL bAllowUndo = TRUE);
	/*
	ɾ���ļ���
	*/
	bool RemoveDir(const CString &);
	/*
	�����������ļ�
	*/
	bool ReNameLocalFile(const CString & strFile,const CString & strNewName);
	
	/* 
	��ȡ�����Ҽ��˵��½��ļ�����
	*/
	void GetAllDesktopRightMenuNewItems(vector<NewType *> &vAllItems,BOOL bOnlyOfficeType = FALSE);
	/*
	/* 
	��ȡ�����Ҽ��˵��½��ļ����͵�ͼ��png��icon�ļ�
	*/
	void GetAllNewItemsIconFile(vector<NewType *> &vAllItems,CString strDestDir);
	/*

	��ָ��Ŀ¼�´���ָ�����͵����ļ�
	*/
	bool CreateNewItem(string strItemType,string strDir,string &strNewPath);

	bool GetFolderItems(CString strFolder,Folder_Item &Items,bool bRecursion = false,bool bInCludeFile = true,bool bIncludeDir =true,bool RelativePath = true);

	static void GetFolderItemCount(const struct _Folder_Item & fItem,int & iFolderCount,int &iFileCount,const bool & bRecursion);
private:
	CString m_strErrMsg;
	int     m_iErrCode;
};
