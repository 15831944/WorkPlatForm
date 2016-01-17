#pragma once
#include <atlcomcli.h>
#include <atlstr.h>
#include "StlHeader.h"

#define WCB_FALSE						0
#define WCB_TRUE						1

/*
�첽����WebEvent��Ϣ��
ͨ����OnCopyData�У��Ƚ�����WE_XXX��Ȼ�������Ϣ���ţ�������ͬ���������첽����
������첽���������Ӧ��������Post����Ϣ��
������
	wp��CWebEventData*
	lp��0
*/
extern UINT WM_WEBEVENT_ASYNC;
/*
��Ƕҳ�������ɡ�
ͨ����web�ص�����CBaseHtmlDialog::OnWebCallback�У�Post����Ϣ�������ڣ�����������
������
wp��0��
lp��0��
*/
extern UINT WM_WEB_LOADED;

//�� js �����еĲ����ṹ���Ӧ
typedef map< CString, CComVariant > WEBCALLBACK_METHOD_PARAM_MAP;


class CWebMethodParamData
{
public:
	CWebMethodParamData();
	virtual ~CWebMethodParamData();
	void Clear(void);

public:
	CString m_sName;
	CString m_sValue;
	CString m_sDataType;
};

class CWebMethodData
{
public:
	CWebMethodData();
	virtual ~CWebMethodData();
	void Clear(void);

	CString GetJsonStr(void);
	BOOL Parse(LPCTSTR lpszJsonStr);
public:
	//CString m_strWebID;
	CString m_sMethodName;
	vector< CWebMethodParamData * > m_vParams;
	CString m_sResult;
	CString m_sInfo;
};

//���̼�ͨѶ�����������ݽ�����
class CWebEventData
{
public:
	CWebEventData();
	virtual ~CWebEventData();
	void Clear();
	//�������У�ɾ�� m_sData �������������ݶ����� url��
	virtual void ClearData();
	//{ webId:"xxx", webType:"xxx", webEvent:"xxx", data:"xxx" }
	BOOL Parse( LPCTSTR lpData );
	//����json��
	CString GetJsonStr(); // for copydata
	//����data����
	virtual BOOL ParseData( LPCTSTR lpData ); //data:"{}"
	//����data����
	virtual CString GetDataStr(); //data:"{}"
	//����id��webId��webType��webEvent
	BOOL CopyEventHeaderFrom( CWebEventData* pSrc );
	
public:
	CString m_sId; // ����� CWebEventData �����Ψһ��ʶ
	CString m_sWebId;	//���� CAttachWindow::m_sId;
	DWORD m_dwWebType;	// ���� CAttachWindow::m_nExeFlag
	DWORD m_dwWebEvent;
	CString m_sData;
};