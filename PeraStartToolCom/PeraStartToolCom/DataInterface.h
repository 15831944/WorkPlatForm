#pragma once
#include "ToolObjectDefine.h"

class CDataInvokeApp
{
public:
	CDataInvokeApp(void);
	void Clear();
	BOOL Parse(LPCTSTR strIn);
	~CDataInvokeApp(void);
public:
	ToolObject *m_pToolObj;
};

