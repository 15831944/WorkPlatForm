#pragma once
#include "WebDataDefine.h"


class CWebCommnunicationClient
{
public:
	CWebCommnunicationClient(void);
	virtual ~CWebCommnunicationClient(void);
	
	/*
	json格式：
	{ "webId":"xxx", "webEvent":"xxx", "data":"{ webEvent对应的数据结构序列化后的jsonstr }" }
	*/
	BOOL Send( HWND hWnd, CWebEventData* pData );
	//解析jsonStr  分配的内存，同步处理时调用Handle之后清理，异步处理时，在异步响应函数中调用Handle之后清理。
	BOOL Receive( LPCTSTR lpData );
	//根据解析后的数据，处理
	BOOL Handle( CWebEventData* pData );
};

extern CWebCommnunicationClient theCommClient;