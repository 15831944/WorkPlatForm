#pragma once

//��ҳ���������˹���̨ʱ��ȡ�ö�Ӧ��JSON���ķ���

//PS:��������в���Ϊ-i�����Զ���Э��д��ע�����������в���Ϊ-u�����Զ���Э���ע���ɾ��

/*
��ҳ���������˹���̨�ӿ��ĵ�

��ҳ�е���ʾ����

<HTML><HEAD><META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=utf-8">		</HEAD>
<BODY>
<a href='PeraDC:{"userName":"admin","realName":"��������Ա","ticket-proxy":"aaabbbcccddd","mainUrl":"http://www.baidu.com/baidu?word=%25E6%25B1%2589%25E5%25AD%2597"}'>���˹���ƽ̨</>
</BODY></HTML>

��ʽ˵����
�����Զ���Э��ĳ����ӷ�ʽ�������˹���ƽ̨
��������PeraDC:��ͷ
�������ַ�����500
�����ӳ�ȥͷ�����Ǹ�json��ʽ���ַ���
{
"userName": "admin",
"realName": "��������Ա",
"ticket-proxy": "aaabbbcccddd",
"mainUrl": "http://www.baidu.com/baidu?word=%E6%B1%89%E5%AD%97"
}
����������userName��realName��ticket-proxy��mainUrl�ĸ�����
json��ʽ���ַ����������%����Ҫʹ��%25����ת��
PeraDC:{"userName":"admin","realName":"��������Ա","ticket-proxy":"aaabbbcccddd","mainUrl":"http://www.baidu.com/baidu?word=%25E6%25B1%2589%25E5%25AD%2597"}
*/

//����������
extern CString g_sCommandLine;

//ȡ����json��
extern CString g_sParamLine;

//����json����ֻ�������Զ���Э����߲��������в����ŷ���TRUE����������FALSE
//��������в���Ϊ-i�����Զ���Э��д��ע�����������в���Ϊ-u�����Զ���Э���ע���ɾ����ͬʱ����FALSE
BOOL ParseJsonCommandLine();

class CCxPeraDCData
{
public:
	CCxPeraDCData();
	virtual ~CCxPeraDCData();
	BOOL Parse( LPCTSTR lpszData );
public:
	void Clear(void);
	CString m_sUrl;
	CString m_sTicket;
	CString m_sUser;
	CString m_sRealName;
};