#pragma once
#include "PeraNetDiskIce.h"
#include <vector>
using namespace std;

class CInterfaceData_QueryFilesOperation ;
class CInterfaceData_CreateFile;
class CInterfaceData_QueryFilesInfo;
class CInterfaceData_OpenFile;

class Json::Value;
class PeraNetDiskIceI : public Pera::NetDisk::Daemon::PeraNetDiskDaemonService
{
public:
	PeraNetDiskIceI();
	~PeraNetDiskIceI();

	virtual ::std::string InvokeMethod( const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );
private:
	std::string GetDefaultErrMsgJson(const char * pszErrMsg = "输入Json参数错误！",int iErrCode = 0);
	void QueryLocalFileType(std::string & strJsonOut);		 // （外部）查询本地可新建的文件类型
	void QueryFilesInfo(const ::std::string&, std::string&); //（外部）查询文件几本信息
	void CreateFile(const ::std::string&,std::string&);		 // （外部）创建本地文件
	void OpenFile(const ::std::string&,std::string&);
	void CopyFiles(const ::std::string&,std::string&);		 //（外部）拷贝本地文件或文件夹
	void GetCacheDir(const ::std::string&,std::string&);	 //（外部）获取缓存路径
	void SetCacheDir(const ::std::string&,std::string&);	 //（外部）设置缓存路径
	void QueryFilesOperation(const ::std::string&,std::string&); //（外部）查询文件操作状态
	void DoFilesOperation(const ::std::string&,std::string&); //（外部）取消文件操作任务
	void OnDownloadFile(const ::std::string&, std::string&); //（外部）下载文件接口
	void OnUploadFile(const ::std::string&, std::string&); //（外部）上传文件接口
	void OnGetSettings(const ::std::string&, std::string&); //（外部）获取设置接口
	void OnSetSettings(const ::std::string&, std::string&);  //（外部）设置接口
	void OnQueryDir(const ::std::string&, std::string&);  //（外部）查询文件夹内容接口
	void QueryDirSubCount(const ::std::string&, std::string&);  //（外部）查询文件夹子项数目接口
	void QueryLocalOfficeFileType(std::string & strJsonOut);  //（外部）查询本机可新建的Office文件类型接口
	void ReNameItem(const ::std::string&, std::string&); //（外部）重命名单个文件或者文件夹的接口
	void DeleteItem(const ::std::string&, std::string&); //（外部）删除多个文件、文件夹的接口
	void PasteFiles(const ::std::string&, std::string&); //（外部）复制粘贴时的文件复制接口
	void MoveFiles(const ::std::string&, std::string&); //（外部）剪切粘贴时的文件移动接口
	void SubmitToOutPut(const ::std::string&, std::string&); //（外部）提交本地文件到输出接口

	void QueryDownloadProgress(CInterfaceData_QueryFilesOperation & IData);
	void CreateFile(CInterfaceData_CreateFile &IData);
	void QueryFilesInfo(CInterfaceData_QueryFilesInfo &IData);
};
