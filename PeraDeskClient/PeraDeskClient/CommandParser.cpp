#include "stdafx.h"

#include "CommandParser.h"
#include "Tools.h"

#include "value.h"
#include "reader.h"

CString g_sCommandLine;
CString g_sParamLine;
const CString g_sRegName = _T("PeraDC");

BOOL ParseJsonCommandLine()
{
	g_sCommandLine = ::GetCommandLine();

	ZTools::WriteZToolsFormatLog("启动参数：\n%s", g_sCommandLine);

	int nBegin = -1;
	if ('\"' == g_sCommandLine.GetAt(0))
	{
		nBegin = g_sCommandLine.Find('\"', 1);
	}
	else
	{
		nBegin = g_sCommandLine.Find(' ', 0);
	}

	if (nBegin == -1)
	{
		return FALSE;
	}

	CString sAllParam = g_sCommandLine.Right(g_sCommandLine.GetLength() - nBegin - 1);
	sAllParam.Trim();

	if (sAllParam.IsEmpty())
	{
		//无参数启动
		return TRUE;
	}

	//-i,-u
	CString sParamHeader = sAllParam.Left(2);
	if (sParamHeader.CompareNoCase(_T("-i")) == 0)
	{
		//写注册表
		//需要写入注册表的权限
		//暂时只写入 HKEY_CLASSES_ROOT，不写HKEY_CURRENT_USER\Software\Classes
		ZTools::WriteZToolsFormatLog(_T("install write reg"));
		CString sRegPath;
		sRegPath.Format(_T("%s"), g_sRegName);
		CRegKey keyUrlProtocol;
		if (keyUrlProtocol.Create(HKEY_CLASSES_ROOT, sRegPath) == ERROR_SUCCESS)
		{
			keyUrlProtocol.SetStringValue(_T("URL Protocol"), _T("URL:PeraDC Protocol"));

			sRegPath.Format(_T("%s\\shell\\open\\command"), g_sRegName);
			CRegKey keyOpenCommand;
			if (keyOpenCommand.Create(HKEY_CLASSES_ROOT, sRegPath) == ERROR_SUCCESS)
			{
				TCHAR exeFullPath[MAX_PATH];
				::GetModuleFileName(NULL, exeFullPath, MAX_PATH);
				CString sOpenCommand;
				sOpenCommand.Format(_T("\"%s\" %%1"), exeFullPath);
				keyOpenCommand.SetStringValue(_T(""), sOpenCommand);
			}
			else
			{
				ZTools::WriteZToolsFormatLog(_T("create HKEY_CLASSES_ROOT\\%s failed"), sRegPath);
			}
		}
		else
		{
			ZTools::WriteZToolsFormatLog(_T("create HKEY_CLASSES_ROOT\\%s failed"), sRegPath);
		}

		exit(0);
		return FALSE;
	}
	else if (sParamHeader.CompareNoCase(_T("-u")) == 0)
	{
		//清注册表
		//需要写入注册表的权限
		//暂时只写入 HKEY_CLASSES_ROOT，不写HKEY_CURRENT_USER\Software\Classes
		ZTools::WriteZToolsFormatLog(_T("uninstall delete reg"));
		CString sRegPath;
		sRegPath.Format(_T("%s"), g_sRegName);
		CRegKey keyUrlProtocol;
		if (keyUrlProtocol.Create(HKEY_CLASSES_ROOT, sRegPath) == ERROR_SUCCESS)
		{
			keyUrlProtocol.RecurseDeleteKey(_T(""));
		}
		else
		{
			ZTools::WriteZToolsFormatLog(_T("create HKEY_CLASSES_ROOT\\%s failed"), sRegPath);
		}

		exit(0);
		return FALSE;
	}

	sParamHeader = sAllParam.Left(g_sRegName.GetLength());
	if (sParamHeader.CompareNoCase(g_sRegName) != 0)
	{
		//未知，错误格式
		return FALSE;
	}

	sAllParam = sAllParam.Right(sAllParam.GetLength() - g_sRegName.GetLength() - 1);
	sAllParam.Trim('/');

	g_sParamLine = sAllParam;

	return TRUE;
}

CCxPeraDCData::CCxPeraDCData()
{

}

CCxPeraDCData::~CCxPeraDCData()
{
	Clear();
}

void CCxPeraDCData::Clear()
{
	m_sUrl = "";
	m_sTicket = "";
	m_sUser = "";
	m_sRealName = "";
}

BOOL CCxPeraDCData::Parse( LPCTSTR lpszData )
{
	Clear();

	CString strSrc = lpszData == NULL ? "" : lpszData;
	if (strSrc.IsEmpty()) return FALSE;

	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if (!reader.parse((char*)(LPCTSTR)strSrc, obj, false)) return FALSE;
		if ( obj.isNull() || !obj.isObject() ) return FALSE;

		m_sUser = obj["userName"].isString() ? obj["userName"].asCString() : _T("");
		m_sRealName = obj["realName"].isString() ? obj["realName"].asCString() : _T("");
		m_sTicket = obj["ticket-proxy"].isString() ? obj["ticket-proxy"].asCString() : _T("");
		if ( !m_sTicket.IsEmpty() ) m_sTicket = "ticket-proxy=" + m_sTicket;
		m_sUrl = obj["mainUrl"].isString() ? obj["mainUrl"].asCString() : _T("");
	}
	catch (...)
	{
		Clear(); return FALSE;
	}

	if ( m_sUrl.IsEmpty() || m_sTicket.IsEmpty() || m_sUser.IsEmpty() )
	{
		Clear(); return FALSE;
	}

	return TRUE;
}
