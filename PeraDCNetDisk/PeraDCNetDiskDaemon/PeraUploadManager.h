#pragma once
#include "StlHeader.h"
#include "Tools.h"

class CInterfaceData_UploadFile; // 上传输入接口数据类
class CInterfaceData_QueryFilesOperation; //查询操作输入接口类
class CInterfaceData_DoFilesOperation; // 取消操作输入接口类
/*
 业务层上传管理器
*/
class CPeraUploadManager
{
public:
	static CPeraUploadManager * Instance(); //获取管理器单例
	CString AddUploadTask(CInterfaceData_UploadFile * pIData,bool bLoadFinished = false); // 向业务层上传管理器中增加上传任务，返回Json字符串
	void QueryUploadProgress(CInterfaceData_QueryFilesOperation & IData); // 业务层查询进度
	void DoFilesOperation(const CInterfaceData_DoFilesOperation &IData); // 执行delete、stop、pause、resume的接口
	void LoadHistoryUploadTask(); // 程序启动后加载历史上传任务
	CString AddOutPutTask(CInterfaceData_UploadFile * pIData); // 提交到输出调用该函数，上传地址不同
private:
	CPeraUploadManager(); // 管理器构造函数，设为私有型，只允许用户通过Instance()构建实例
	~CPeraUploadManager();
private:
	class GabageCollector // 定义私有内嵌类
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("垃圾回收器CPeraUploadManager::GabageCollector构建...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("垃圾回收器CPeraUploadManager::GabageCollector执行单例对象清理工作结束...");
			}
		}

	};
	static CPeraUploadManager * m_PManagetr;
	static GabageCollector m_gc; // 定义垃圾回收器的静态对象，程序结束后，系统会调用该对象的析构函数，以执行对单例对象的析构
};