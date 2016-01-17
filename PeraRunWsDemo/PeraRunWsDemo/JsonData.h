#pragma once
#include "reader.h"
#include "value.h"

class CDataUploadIn
{
public: 
	CDataUploadIn(CString strInWsFile);
		CString GetJsonStr();
private:
		CString m_strWsFile;
};
class CDataUpLoadResult
{
public:
	void FromJson(CString strJsonStr);
	CString GetIndex();
private:
	CString m_strIndex;
};
class CDataQueryUpProgressIn
{
public:
	CDataQueryUpProgressIn(CString strIndex);
    CString GetJsonStr();
public:
	CString m_strIndex;
};

class CDataQueryUpProgressResult
{
public:
	void FromJson(CString strJsonStr);
	int GetUpRate();
	CString GetFlag();
	CString GetMsg();
private:
	int m_iRate;
	CString m_strFlag;
	CString m_strMsg;
};

class CDataCallStartWs
{
public:
	CDataCallStartWs(CString strUpResult,CString strLocalIp,CString strUserId,CString strUsrName,CString strTicket);
	CString GetJsonStr();
private:
	CString m_strJsPart;
	CString m_strLocalIp;
	CString m_strUsrId;
	CString m_strUsrName;
	CString m_strTicket;
};
class CDataCallStartWsResult
{
public:
	void FromJson(CString strJsonStr);
	CString GetOptMsg();
	CString GetOptUrl();
private:
	CString m_strOptMsg;
	CString m_strOptUrl;
};