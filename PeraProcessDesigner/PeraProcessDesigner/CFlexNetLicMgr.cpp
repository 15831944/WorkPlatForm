#include "stdafx.h"
#include "CFlexNetLicMgr.h"

const LPCTSTR g_lpszMsgTitle = _T("PERA");

CFlexNetLicMgr::CFlexNetLicMgr()
{
	
}


CFlexNetLicMgr::~CFlexNetLicMgr()
{
	CheckInLicense(m_feature.c_str());
	freeJob();
}

BOOL CFlexNetLicMgr::multiToUnicode(const std::string& multiText, std::wstring& unicodeText)
{
	int size = ::MultiByteToWideChar(CP_ACP,0,multiText.c_str(),-1,NULL,0);
	if (0 == size)
	{
		return FALSE;
	}
	wchar_t* wszBuffer = new wchar_t[size + 1];
	::ZeroMemory(wszBuffer, (size + 1) * sizeof(wchar_t));
	if (0 == ::MultiByteToWideChar(CP_ACP,0,multiText.c_str(),-1,wszBuffer,size + 1))
	{
		delete[] wszBuffer;
		return FALSE;
	}
	unicodeText = wszBuffer;
	delete[] wszBuffer;
	return TRUE;
}

BOOL CFlexNetLicMgr::UnicodeTomulti(const std::wstring& unicodeText, std::string& multiText)
{
	int iTextLen = WideCharToMultiByte(CP_ACP, 0, unicodeText.c_str(), -1, NULL, 0, NULL, NULL);
	if (iTextLen <= 0)
	{
		return FALSE;
	}
	char* pElementText = new char[iTextLen + 1];
	memset(pElementText, 0, sizeof(char)*(iTextLen + 1));
	::WideCharToMultiByte(CP_ACP, 0, unicodeText.c_str(), -1, pElementText, iTextLen, NULL, NULL);
	multiText = pElementText;
	delete[] pElementText;
	pElementText = NULL;
	return TRUE;
}

BOOL CFlexNetLicMgr::multiToUTF8(const std::string& multiText, std::string& utf8Text)
{
	int size = ::MultiByteToWideChar(CP_ACP,0,multiText.c_str(),-1,NULL,0);
	if (0 == size)
	{
		return FALSE;
	}
	wchar_t* wszBuffer = new wchar_t[size + 1];
	::ZeroMemory(wszBuffer, (size + 1) * sizeof(wchar_t));
	if (0 == ::MultiByteToWideChar(CP_ACP,0, multiText.c_str(),-1,wszBuffer,size + 1))
	{
		delete[] wszBuffer;
		return FALSE;
	}
	size = ::WideCharToMultiByte(CP_UTF8,0, wszBuffer,-1,NULL,0, NULL,NULL);
	if (0 == size)
	{
		delete[] wszBuffer;
		return FALSE;
	}
	char* szBuffer = new char[size + 1];
	::ZeroMemory(szBuffer, (size + 1) * sizeof(char));
	if (0 == ::WideCharToMultiByte(CP_UTF8,0,wszBuffer,-1,szBuffer,size + 1,NULL,NULL))
	{
		delete[] wszBuffer;
		delete[] szBuffer;
		return FALSE;
	}
	utf8Text = szBuffer;
	delete[] wszBuffer;
	delete[] szBuffer;
	return TRUE;
}

BOOL CFlexNetLicMgr::UTF8Tomulti(const std::string& utf8Text, std::string& multiText)
{
	int size = ::MultiByteToWideChar(CP_UTF8,0,utf8Text.c_str(),-1,NULL,0);
	if (0 == size)
	{
		return FALSE;
	}
	wchar_t* wszBuffer = new wchar_t[size + 1];
	::ZeroMemory(wszBuffer, (size + 1) * sizeof(wchar_t));
	if (0 == ::MultiByteToWideChar(CP_UTF8,0, utf8Text.c_str(),-1,wszBuffer,size + 1))
	{
		delete[] wszBuffer;
		return FALSE;
	}
	size = ::WideCharToMultiByte(CP_ACP,0, wszBuffer,-1,NULL,0, NULL,NULL);
	if (0 == size)
	{
		delete[] wszBuffer;
		return FALSE;
	}
	char* szBuffer = new char[size + 1];
	::ZeroMemory(szBuffer, (size + 1) * sizeof(char));
	if (0 == ::WideCharToMultiByte(CP_ACP,0,wszBuffer,-1,szBuffer,size + 1,NULL,NULL))
	{
		delete[] wszBuffer;
		delete[] szBuffer;
		return FALSE;
	}
	multiText = szBuffer;
	delete[] wszBuffer;
	delete[] szBuffer;
	return TRUE;
}

BOOL CFlexNetLicMgr::UTF8ToUnicode(const std::string& utf8Text, std::wstring& unicodeText)
{
	int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, NULL, 0);
	if (unicodeLen <= 0)
	{
		return FALSE;
	}
	wchar_t* pUnicode = new  wchar_t[unicodeLen+1];
	memset(pUnicode, 0, (unicodeLen+1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
	unicodeText = pUnicode;
	delete[] pUnicode;
	pUnicode = NULL;
	return TRUE;
}

BOOL CFlexNetLicMgr::UnicodeToUTF8(const std::wstring& unicodeText, std::string& utf8Text)
{
	int iTextLen = WideCharToMultiByte(CP_UTF8, 0, unicodeText.c_str(), -1, NULL, 0, NULL, NULL);
	if (iTextLen <= 0)
	{
		return FALSE;
	}
	char* pElementText = new char[iTextLen + 1];
	memset(pElementText, 0, sizeof(char) * (iTextLen + 1));
	::WideCharToMultiByte(CP_UTF8, 0, unicodeText.c_str(), -1, pElementText, iTextLen, NULL, NULL);
	utf8Text = pElementText;
	delete[] pElementText;
	pElementText = NULL;
	return TRUE;
}

BOOL CFlexNetLicMgr::UnicodeToGB2312(const std::wstring& unicodeText, std::string& gb2312Text)
{
	int iTextLen = ::WideCharToMultiByte(CP_ACP, 0, unicodeText.c_str(), -1, NULL, 0, NULL, NULL);
	if (iTextLen <= 0)
	{
		return FALSE;
	}
	char* pElementText = new char[iTextLen + 1];
	memset(pElementText, 0, sizeof(char) * (iTextLen + 1));
	int ret = ::WideCharToMultiByte(CP_ACP, NULL, unicodeText.c_str(), -1, pElementText, iTextLen, NULL, NULL);
	gb2312Text = pElementText;
	delete[] pElementText;
	pElementText = NULL;
	return TRUE;
}

BOOL CFlexNetLicMgr::GB2312ToUnicode(std::string& gb2312Text, std::wstring& unicodeText)
{
	int iTextLen = gb2312Text.length();
	if (iTextLen <= 0)
	{
		return FALSE;
	}
	WCHAR *pUnicode = new WCHAR[iTextLen + 1];
	memset(pUnicode, 0, sizeof(WCHAR)*(iTextLen + 1));
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gb2312Text.c_str(), iTextLen, (LPWSTR)pUnicode, iTextLen);
	unicodeText = pUnicode;
	delete[] pUnicode;
	pUnicode = NULL;
	return TRUE;
}

BOOL CFlexNetLicMgr::CheckInLicense(const char *feature)
{
	int res = checkIn(feature);
	if(!res)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CFlexNetLicMgr::CheckOutLicense(const char *feature)
{
	m_feature = feature;
	std::string strErr;
	int status = 0;
	int res = 0;
	setPromptForFile(1);
	status = newJob();
	if(!status)
	{
		res = checkOut(feature);
		if(!res)
		{
			int days = getExpireDays(feature);
			if (days < 0)
			{
				::MessageBox(NULL,"License����ļ��ѹ���!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				return FALSE;
			}
			return TRUE;
		}
		else
		{
			switch(res)
			{
			case -1:
				::MessageBox(NULL,"�Ҳ�������ļ�!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -2:
				::MessageBox(NULL,"��Ч������ļ���ʽ!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -4:
				::MessageBox(NULL,"�Ѵﵽ��ɸ���ʹ������!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -8:
				::MessageBox(NULL,"�����ɺ�/ǩ���Լ����ݺ�ģ�鲻ƥ��!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -9:
				::MessageBox(NULL,"ϵͳ��������ʶ������ļ��е�������ʶ��ƥ��!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -15:
				::MessageBox(NULL,"���Ӳ�����ɷ�������\r\n��������ɷ���û��������������ʹ���˴����port@host ��������ļ���������ļ��е�TCP/IP�˿ںŻ��߻�����������!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -34:
				::MessageBox(NULL,"�ͻ��˺���ɷ�����ϵͳ֮���ʱ�Ӳ���̫��!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -88:
				::MessageBox(NULL,"ϵͳʱ���Ѿ����ز�!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			case -97:
				::MessageBox(NULL,"����Ĺ�Ӧ���ػ������ѹر�!",g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			default:
				UTF8Tomulti(getErrorString(res),strErr);
				CString strerrcode;
				strerrcode.Format("�����쳣����!�������:%d\n%s", res, strErr.c_str());
				::MessageBox(NULL,strerrcode,g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
				break;
			}
			
			return FALSE;
		}
	}
	else
	{
		UTF8Tomulti(getErrorString(status),strErr);
		::MessageBox(NULL,strErr.c_str(),g_lpszMsgTitle,MB_OK|MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}
