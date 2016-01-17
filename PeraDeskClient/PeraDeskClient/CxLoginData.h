#pragma once
class CCxLoginData
{
public:
	CCxLoginData(void);
	~CCxLoginData(void);
	void Clear(void);
	BOOL Parse(LPCTSTR lpszData);
public:
	CString m_strUser;
	CString m_strPassword;
	CString m_strTicket;
	CString m_strRealName;
	CString m_strRoles;
	CString m_strErrInfo;
};
class CCxLoginInfo
{
public:
	CCxLoginInfo(void);
	void Clear();
	BOOL Parse(LPCTSTR lpszData);
public:
	CString m_strUser;
	CString m_strRealName;
};
