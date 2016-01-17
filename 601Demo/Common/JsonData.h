#pragma once
#include "reader.h"
#include "value.h"


class CDataRpcInvokeServer
{
public:
    CString GetJsonStr();
	BOOL Parse(LPCTSTR strIn);
	void Clear();
public:
	CString m_strParam;
	CString m_strIp;
	CString m_strPort;
};

class CDataAddFunParam
{
public:
	CString GetJsonStr();
	BOOL Parse(LPCTSTR strIn);
	void Clear();
public:
	double m_da;
	double m_db;
};