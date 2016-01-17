#pragma once
class CCxStructTreeNodeFlow;
class CProcessMgr
{
public:
	CProcessMgr(void);
	~CProcessMgr(void);

	//设计过程临时路径，建模过程中用到，传给静态页面
	CString m_sWorkPath;
	//当前设计过程打包后的文件全路径，客户端保存时使用，对静态页面透明
	CString m_sProcessFilePath;

	//从静态页面查询是否需要保存（该接口尚未与页面达成一致如何做，是否需要）
	BOOL GetProcessStateNeedSave();
	//从静态页面查询是否允计保存，返回json,有flag,msg
	BOOL GetProcessStateCanSave();
	//从静态页面查询当前子流程节点是否允计保存，返回json,有flag,msg
	BOOL GetSubProcessStateCanSave();
	//从静态页查询当前设计过程是否允许上传服务器执行
	BOOL CanRunSubProcess();
	//从静态页查询当前组件是否允许保存，用于检测表单与组件是否同步
	BOOL GetComponentStateCanSave();

	//基本操作
	BOOL New();
	BOOL Open(CString sProcessFilePath);
	BOOL Save();
	void Close();
	BOOL Publish();

	//触发事件的入口
	BOOL OnNew();
	BOOL OnOpen();
	BOOL OnSave();
	BOOL OnSaveAs();
	BOOL OnClose();
	BOOL OnRunModel();

	BOOL OnSaveSubProcess(void* p);
	BOOL OnPublishSubProcess(void* p);
	BOOL OnRunSubProcess();
	BOOL OnEditSubProcess(void* p);

	static DWORD WINAPI RunInServerThread(LPVOID lpParameter);
	static BOOL InvokeRunProcess(LPVOID lpParameter, std::string strJsonParams);
	static BOOL OpenRunInNewTab(LPVOID lpParameter);
	static BOOL OpenRunInModal(LPVOID lpParameter);

	BOOL RunInServerVersion(BOOL bIsModelRun);

	//当前网络版执行时的信息及状态变量
	//保存当前运行的网址及运行实例号
	CString m_sRunUrl;
	CString m_sFlowId;
	CString m_sXpdlStreamWithFileid;
	BOOL m_bIsModelRun;
	BOOL m_bProcessStarting;
	void ClearRunVar();
	void SetRunVar(CString sRunUrl, CString sFlowId);
	
	//清空成员变量
	void Clear();
	//询问用户是否需要保存
	BOOL AskIsSave();
	//增加到历史记录列表
	void AddToHistory();
	void AddToHistory(LPCTSTR strName, LPCTSTR strPath);
	void SetModified(BOOL bModified);
	CString GetFileMD5(CString sFilePath);
	//调页面方法，页面组织上传bot及文件类型参数并改写xpdl返回
	CString GetReplacedXpdlByUploadFile();
	void RefreshHistory(); //刷新历史记录（清空失效的）

	//取树上根节点
	CCxStructTreeNodeFlow* GetTreeRoot();
};

