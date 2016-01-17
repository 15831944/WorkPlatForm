#pragma once
#include "StlHeader.h"
#include "Tools.h"

class CInterfaceData_CopyFiles;           // 文件拷贝输入接口数据类
class CInterfaceData_QueryFilesOperation; //查询操作输入接口类
class CInterfaceData_DoFilesOperation; // 取消操作输入接口类
/*
业务层拷贝管理器
*/
class CPeraCopyManager
{
public:
	static CPeraCopyManager * Instance(); //获取管理器单例
	CString AddCopyTask(CInterfaceData_CopyFiles * pIData,bool IsLoadFinished = false); // 向业务层拷贝管理器中增加拷贝任务，返回Json字符串
	CString AddCopyTask(CInterfaceData_PasteFiles * pIData); // 向业务层拷贝管理器中增加拷贝任务，返回Json字符串
	void QueryCopyProgress(CInterfaceData_QueryFilesOperation & IData);  // 业务层查询进度
	void DoFilesOperation(const CInterfaceData_DoFilesOperation &IData);   // 业务层取消
	void LoadHistoryCopyTask(); // 程序启动后加载历史拷贝任务
private:
	~CPeraCopyManager();
	 CPeraCopyManager(); // 管理器构造函数，设为私有型，只允许用户通过Instance()构建实例
private:
	class GabageCollector // 定义私有内嵌类
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("垃圾回收器CPeraCopyManager::GabageCollector构建...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("垃圾回收器CPeraCopyManager::GabageCollector执行单例对象清理工作结束...");
			}
		}

	};
	static CPeraCopyManager * m_PManagetr;
	static GabageCollector m_gc; // 定义垃圾回收器的静态对象，程序结束后，系统会调用该对象的析构函数，以执行对单例对象的析构
};