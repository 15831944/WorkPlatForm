#pragma once
#include "WebDataDefine.h"

//////////////////////////////////////////////////////////////////////////

//���ڸ��ӽ���ʱ���͵���չ����
//CAttachWindow::CreateClient ʹ��
class CAttachWindowExData
{
public:
	CAttachWindowExData();
	~CAttachWindowExData();

	void	Clear();
	BOOL	Parse( LPCTSTR lpData );
	CString GetJsonStr();
public:
	CString m_sUrl;
};


//���״̬
//WEBTYPE_RUNFLOW
//WE_RUNFLOW_COMPLETED
class CWERunFlowCompleted : public CWebEventData
{
public:
	CWERunFlowCompleted();
	virtual ~CWERunFlowCompleted();
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	virtual CString GetDataStr(); //data:"{}"
	virtual void ClearData();
public:
	CString m_sStatus;
};


//��ȡ������id
//WEBTYPE_RUNFLOW
//WE_RUNFLOW_SUBFLOWID
class CWERunFlowSubFlowId : public CWebEventData
{
public:
	CWERunFlowSubFlowId();
	virtual ~CWERunFlowSubFlowId();
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	virtual CString GetDataStr(); //data:"{}"
	virtual void ClearData();
public:
	CString m_sSubFlowId;
};

//ȡ����������
//WEBTYPE_RUNFLOW
//WE_RUNFLOW_CANCEL
class CWERunFlowCancel : public CWebEventData
{
public:
	CWERunFlowCancel();
	virtual ~CWERunFlowCancel();
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	virtual CString GetDataStr(); //data:"{}"
	virtual void ClearData();
};

//�����鿴���̶Ի���ʱʹ��
//���������鿴���̶Ի�����ʾ�� url�����ߡ�
class CJsData_ShowViewRunFlow
{
public:
	CJsData_ShowViewRunFlow();
	~CJsData_ShowViewRunFlow();
	void Clear();
	BOOL Parse( LPCTSTR lpszData );
public:
	CString m_strUrl;
	int m_nWidth;
	int m_nHeight;
};