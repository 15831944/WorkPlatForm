#pragma once
class CCxStructTreeNodeFlow;
class CProcessMgr
{
public:
	CProcessMgr(void);
	~CProcessMgr(void);

	//��ƹ�����ʱ·������ģ�������õ���������̬ҳ��
	CString m_sWorkPath;
	//��ǰ��ƹ��̴������ļ�ȫ·�����ͻ��˱���ʱʹ�ã��Ծ�̬ҳ��͸��
	CString m_sProcessFilePath;

	//�Ӿ�̬ҳ���ѯ�Ƿ���Ҫ���棨�ýӿ���δ��ҳ����һ����������Ƿ���Ҫ��
	BOOL GetProcessStateNeedSave();
	//�Ӿ�̬ҳ���ѯ�Ƿ��ʼƱ��棬����json,��flag,msg
	BOOL GetProcessStateCanSave();
	//�Ӿ�̬ҳ���ѯ��ǰ�����̽ڵ��Ƿ��ʼƱ��棬����json,��flag,msg
	BOOL GetSubProcessStateCanSave();
	//�Ӿ�̬ҳ��ѯ��ǰ��ƹ����Ƿ������ϴ�������ִ��
	BOOL CanRunSubProcess();
	//�Ӿ�̬ҳ��ѯ��ǰ����Ƿ������棬���ڼ���������Ƿ�ͬ��
	BOOL GetComponentStateCanSave();

	//��������
	BOOL New();
	BOOL Open(CString sProcessFilePath);
	BOOL Save();
	void Close();
	BOOL Publish();

	//�����¼������
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

	//��ǰ�����ִ��ʱ����Ϣ��״̬����
	//���浱ǰ���е���ַ������ʵ����
	CString m_sRunUrl;
	CString m_sFlowId;
	CString m_sXpdlStreamWithFileid;
	BOOL m_bIsModelRun;
	BOOL m_bProcessStarting;
	void ClearRunVar();
	void SetRunVar(CString sRunUrl, CString sFlowId);
	
	//��ճ�Ա����
	void Clear();
	//ѯ���û��Ƿ���Ҫ����
	BOOL AskIsSave();
	//���ӵ���ʷ��¼�б�
	void AddToHistory();
	void AddToHistory(LPCTSTR strName, LPCTSTR strPath);
	void SetModified(BOOL bModified);
	CString GetFileMD5(CString sFilePath);
	//��ҳ�淽����ҳ����֯�ϴ�bot���ļ����Ͳ�������дxpdl����
	CString GetReplacedXpdlByUploadFile();
	void RefreshHistory(); //ˢ����ʷ��¼�����ʧЧ�ģ�

	//ȡ���ϸ��ڵ�
	CCxStructTreeNodeFlow* GetTreeRoot();
};

