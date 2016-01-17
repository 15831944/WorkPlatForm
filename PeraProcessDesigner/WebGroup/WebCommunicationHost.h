#pragma once
#include "WebDataDefine.h"


class CWebCommunicationHost
{
public:
	CWebCommunicationHost(void);
	~CWebCommunicationHost(void);
	
	/*
	json��ʽ��
	{ "webId":"xxx", "webEvent":"xxx", "data":"{ webEvent��Ӧ�����ݽṹ���л����jsonstr }" }
	*/
	BOOL Send( HWND hWnd, CWebEventData* pData );
	//����jsonStr  ������ڴ棬ͬ������ʱ����Handle֮�������첽����ʱ�����첽��Ӧ�����е���Handle֮������
	BOOL Receive( LPCTSTR lpData );
	//���ݽ���������ݣ�����
	BOOL Handle( CWebEventData* pData );
};

extern CWebCommunicationHost theCommHost;
