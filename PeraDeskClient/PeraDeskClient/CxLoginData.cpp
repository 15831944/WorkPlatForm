#include "StdAfx.h"
#include "CxLoginData.h"
#include "reader.h"
#include "value.h"
//#include "KeepTicketFunc.h"

CCxLoginData::CCxLoginData(void)
{

}


CCxLoginData::~CCxLoginData(void)
{
	Clear();
}

void CCxLoginData::Clear( void )
{
	m_strUser = "";
	m_strPassword = "";
	m_strTicket = "";
	m_strRealName = "";
	m_strRoles = "";
	m_strErrInfo = "";
}

BOOL CCxLoginData::Parse( LPCTSTR lpszData )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;

	try
	{
		Json::Value jLoginData;
		Json::Reader reader;
		if (!reader.parse((LPCTSTR)strSrc, jLoginData)) return FALSE;

		CString sError = jLoginData["error"].asString().c_str();
		if ( sError.CompareNoCase("0") != 0 )
		{
			//
			m_strErrInfo = jLoginData["errInfo"].asString().c_str();
			return FALSE;
		}
		
		m_strUser = jLoginData["userName"].asString().c_str();
		m_strPassword = jLoginData["password"].asString().c_str();
		m_strTicket = jLoginData["ticket"].asString().c_str();
		m_strRealName = jLoginData["realName"].asString().c_str();
		m_strRoles = jLoginData["roles"].asString().c_str();

//		CallKeepTicket(m_strTicket);
		return TRUE;
	}
	catch (...)
	{
		Clear();
		m_strErrInfo;
	}
	return FALSE;
}
CCxLoginInfo::CCxLoginInfo(void)
{
	Clear();
}
void CCxLoginInfo::Clear()
{
	m_strUser.Empty();
	m_strRealName.Empty();
}
BOOL CCxLoginInfo::Parse(LPCTSTR lpszData)
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;

	try
	{
		Json::Value jLoginData;
		Json::Reader reader;
		if (!reader.parse((LPCTSTR)strSrc, jLoginData)) return FALSE;

		m_strUser = jLoginData["userName"].asString().c_str();
		m_strRealName = jLoginData["realName"].asString().c_str();
		return TRUE;
	}
	catch (...)
	{
		Clear();
	}
	return FALSE;
}