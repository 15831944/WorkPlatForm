#pragma once
#include "CxWebPageMgr.h"

/*
ʹ��˵����
���һ��ģ��Ĳ��裺
1���� CPeraWebPageMgr �У���� FW_ ��ͷ��ģ���ʶ����� FE_ ��ͷ���¼���ʶ��
2���� PeraWebPageEventExData �ļ�����������ࡣ
3���� _OnNewEventExData �� new �¼���Ӧ�����������
4��CPeraWebPageMgrHost::Require ����ӡ��ͻ��ˡ�����WebPage���̡�������¼���
5��CPeraWebPageMgrHost::OnWebPageEvent ����ӡ�WebPage���̡������ͻ��ˡ��������Ӧ���¼���
6��CPeraWebPageMgrClient::Require ����ӡ�WebPage���̡������ͻ��ˡ�������¼���
7��CPeraWebPageMgrClient::OnWebPageEvent ����ӡ��ͻ��ˡ�����WebPage���̡��������Ӧ���¼���
*/
class CPeraWebPageMgr :
	public CCxWebPageMgr
{
public:
	//WebPage��־---------------------
	enum{
		FW_TABVIEW_TASK_MODEL = 1          //
	};

	//�¼�����---------------------
	//��һ���� FE_CUSTOM + 1 ��ʼ
	enum{
		//����������WebPage
//		FE_RUNFLOW_CANCEL = FE_CUSTOM + 1,    //�ͻ���֪ͨsvg��ȡ������������
//		FE_RUNFLOW_FINISHED, //svg֪ͨ�ͻ��ˣ���̨���н���


		FE_END
	};

public:
	CPeraWebPageMgr(void);
	virtual ~CPeraWebPageMgr(void);
	virtual CString GetEventDisplay( CCxWebPageEvent * pEvent );
//	BOOL NavigateInNewTab( LPCTSTR lpszUrl, int nWebPageFlag );
protected:
	virtual CCxWebPageEventExData* _OnNewEventExData( int nModuleFlag, int nEventFlag );
	
};
