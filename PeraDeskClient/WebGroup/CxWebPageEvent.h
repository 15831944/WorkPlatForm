#pragma once
#include "StlHeader.h"
#include <atlstr.h>

/*
ʹ��˵����

*/
//////////////////////////////////////////////////////////////////////////

//ע�⣺��ֹ�����ֲ�����
class CCxWebPageEvent;
class CCxWebPageMgr;

extern BOOL CanParseJsonObjPtr( LPVOID lpData );

/************************************************************************
WebPage�¼��ĸ������ݵĻ��ࡣ
CCxWebPageEvent ���캯���ĵ�����������
*************************************************************************/
class CCxWebPageEventExData
{
	friend class CCxWebPageEvent;

public:
	CCxWebPageEventExData();
	virtual ~CCxWebPageEventExData();
public:
	virtual void Clear(void);
public:
	BOOL GetCopy( CCxWebPageEventExData *& pOther );
protected:
	virtual BOOL _OnGetCopy( CCxWebPageEventExData *& pOther ) = 0;

public:
	//
	virtual BOOL Parse( LPCTSTR lpszData );
protected:
	//{...,"data":{},...}
	//����data��Ӧ�Ķ���
	virtual BOOL OnParseJsonObj( LPVOID lpJsonObj );
	//���ɣ�"data":{}
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );
};

class CCxWPENavigate : public CCxWebPageEventExData
{
public:
	CCxWPENavigate();
	virtual ~CCxWPENavigate();
	virtual void Clear(void);
protected:
	virtual BOOL _OnGetCopy( CCxWebPageEventExData *& pOther );
	virtual BOOL OnParseJsonObj( LPVOID lpJsonObj );
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );
public:
	CString m_strUrl;
};

//////////////////////////////////////////////////////////////////////////

class CCxWebPageEvent
{
	friend class CCxWebPageMgr;
	friend class CPeraWebPageMgr;

public:
	CCxWebPageEvent( int nModuleFlag = 0, int nEventFlag = 0 );
	virtual ~CCxWebPageEvent(void);

	virtual void Clear(void);
	BOOL Parse( LPCTSTR lpszData );
	CString GetJsonStr( void );
	//������չ���� "data":{} 
	virtual BOOL ParseExData( void );

	//����ͬ���¼������÷�������
	BOOL CreateSyncEvent(void);
	//��Ӧͬ���¼��������÷�Ӧ��
	BOOL AnswerSyncEvent(void);

	//ȡ������
	int GetModuleFlag(void) const { return m_nModuleFlag; }
	int GetEventFlag(void) const { return m_nEventFlag; }
	CCxWebPageEventExData * GetExData(void) { return m_pData; }
	//
	BOOL SendCopyData( HWND hwndRecv );

protected:
	virtual BOOL OnParsing( LPVOID lpJsonObj );
	virtual BOOL OnGetJsonStr( LPVOID lpJsonObj );

public:
	//�¼���ʶ������ʱ��ֵͬ m_strId
	//���̼�ͨѶʱ��Ӧ�����ø�ֵ����Է������е� m_strId ƥ�䡣
	CString m_strEventId;
	BOOL m_bAnswer; //�Ƿ�ΪӦ��Ĭ��Ϊ��FALSE
	BOOL m_bSync; //�Ƿ���Ҫͬ���������¼���������Ӧ��

	DWORD m_dwMainProcessId; //�ͻ��˽���Id��
	HWND m_hWndMain; //�ͻ��˽��̵Ĵ��ھ����
	DWORD m_dwWebPageProcessId; //WebPage���̡�
	HWND m_hWndWebPage; //WebPage���ھ����

protected:
	//�ڱ������ڣ������Ψһ��ʶ��������Ϊ���л����ݴ��ݡ�
	CString m_strId;
	//WebPageģ���ʶ
	int m_nModuleFlag;
	//�¼���ʶ
	int m_nEventFlag;
	//ÿ���¼�����չ���ݣ�m_nEventFlag ��Ӧ�����ݣ�
	CCxWebPageEventExData * m_pData;
	//ָ��һ�� Json::Value * ��Json����ָ�룩����Ӧ�����е� "data":{}
	LPVOID m_lpJsonObj;
	//
	CCxWebPageMgr * m_pMgr;


};
