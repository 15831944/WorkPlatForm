#pragma once

class CInterfaceData_OpenFile;
class CInterfaceData_UploadFile;
class CInterfaceData_DownloadFiles;
class CInterfaceData_QueryFilesOperation;

class MonitorThreadParam
{
public:
	MonitorThreadParam()
	{
		m_strDir = "";
	}
	CString m_strDir;
};

class COpenData
{
public:
	CInterfaceData_OpenFile m_IData; 
	HANDLE      m_handleOpenProcess;
	CString     m_sFilePath;
	ULONGLONG   m_uLastModifyTime;
	BOOL        m_bExitOpenProcess; //进程是否已经退出
};
class COpenFilePro
{
public:
	static void OnOpenFile(CInterfaceData_OpenFile &IData);
	static DWORD WINAPI NotifyThreadPro(LPVOID param); 
	static DWORD WINAPI ProcessChangeThreadPro(LPVOID param); //用以决定连续修改时何时执行上传操作
	static DWORD WINAPI OpenFileThreadPro(LPVOID param);
private:
	static void OpenFileNetDisk(CInterfaceData_OpenFile &IData);
	static void OpenFileShared(CInterfaceData_OpenFile &IData);
	static void OpenFileLocal(CInterfaceData_OpenFile &IData);
	static CString DownloadToLocal(CInterfaceData_OpenFile &IData);
	static CString UploadModifiedFile(CString strLocal,CInterfaceData_OpenFile &IData);
	static map<CString,COpenData> mapOpenFile;
	static CRITICAL_SECTION       g_cs; //互斥对象
};