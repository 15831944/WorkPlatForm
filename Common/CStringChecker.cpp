#include "stdafx.h"
#include "CStringChecker.h"
#include "CStringCheckerConfig.h"
#include "atlrx.h"
#include <atldbcli.h>
#include <sstream>


LPCTSTR CStringChecker::m_strErrs[TYPE_COUNT] = {
	"��������� \\ / : * ? \" < > |",
	"��������� . ���ո�",
	"����ĸ����Ϊ����",
	"��󳤶Ȳ��ó���",
	"������Ϊ��",
	"�Ƿ��������������£�\n1. ���԰�����ĸ�����֡��»��߻�����\n2. ���ܰ����ո񡢱����š����������������\n3. ���������ֿ�ͷ\n4. ���Ȳ��ܳ���50",
	"�Ƿ��������������£�\n1. ���԰�����ĸ�����֡��»��߻�����\n2. ���ܰ����ո񡢱����š����������������\n3. ���������ֿ�ͷ\n4. ���Ȳ��ܳ���20",
	"����������֤ʧ��",
	"�������Ϲ���",
	"�������Ϲ���",
	"���������Ϲ���",
	"�������ݲ��Ϲ���",
	"���Ϲ���"
};

CString CStringChecker::m_strErr;

CStringChecker::CStringChecker(void)
{
}


CStringChecker::~CStringChecker(void)
{
}

//ת����double����ʽΪ.0
CString CStringChecker::FormatDouble( LPCTSTR cStr )
{
	CString str = cStr;
	str.Trim(' ');
	if(str.IsEmpty()) return "";
 	if(CheckDouble(str))
	{
		if(str[0] == '-')str.Delete(0);
		str.TrimLeft('0');
		if(str.IsEmpty() || str[0] == '.')
		{
			str = '0' + str;
		}
		if(CheckIntNumber(str))
		{
			str += ".0";
		}
		else if(str.FindOneOf("eE") == -1)
		{
			BOOL bDelete = TRUE;
			int nStart = str.Find('.') + 1;
			int nEnd = str.GetLength() - 1;
			for(int i = nEnd; i >= nStart; i--)
			{
				if(str[i] != '0')
				{
					bDelete = FALSE;
					break;
				}
			}
			if(bDelete)
			{
				str.Delete(nStart + 1, nEnd - nStart);
			}
		}
		if(*cStr == '-')str = '-' + str;
	}
	return str;
}

// 1.windows�ļ����������򣬲�������� \ / : * ? " < > |
BOOL CStringChecker::CheckWindowsRuler( LPCTSTR cStr )
{
	TCHAR cFilter[] = {'\\', '/', ':', '*', '?', '\"', '<', '>', '|', '\0'};
	
	CString str(cStr);
	if(str.FindOneOf(cFilter) != -1)
	{
		SetLastErr(TYPE_WR);
		return FALSE;
	}

	return TRUE;
}


// 2.��������� . ���ո�
BOOL CStringChecker::CheckSpacePoint( LPCTSTR cStr )
{
	TCHAR cFilter[] = {'.', ' ', '\0'};
	CString str(cStr);
	if(str.FindOneOf(cFilter) != -1)
	{
		SetLastErr(TYPE_SP);
		return FALSE;
	}

	return TRUE;
}


// 3.����ĸ����Ϊ����
BOOL CStringChecker::CheckFirstNumber( LPCTSTR cStr )
{
	if(!*cStr)
	{
		return TRUE;
	}
	char c = cStr[0];
	if(c >= '0' && c <= '9')
	{
		SetLastErr(TYPE_FN);
		return FALSE;
	}
	return TRUE;
}

// 4.��󳤶�Ϊ20
BOOL CStringChecker::CheckLength( LPCTSTR cStr, int nLength )
{
	if(CStringW(cStr).GetLength() > nLength)
	{
		CString cStrRet;
		cStrRet.Format("%s%d���ַ�", m_strErrs[TYPE_LEN], nLength);
		SetLastErr(cStrRet);
		return FALSE;
	}
	return TRUE;
}

// 5.������Ϊ��
BOOL CStringChecker::CheckEmpty( LPCTSTR cStr )
{
	if(!*cStr)
	{
		SetLastErr(TYPE_EMPTY);
		return FALSE;
	}
	return TRUE;
}

std::string CStringChecker::ReplaceNodeName( LPCTSTR str, LPCTSTR strRe )
{
	std::string strTemp(str);
	std::string strReplace(strRe);
	std::string strReturn;

	if(strTemp.empty()) return strReturn;

	for(int i = 0; i < (int)strTemp.length(); i++)
	{
		UCHAR c = strTemp[i];
		BOOL b = TRUE;
		if(c < 0xB0 && CString(CHAR(c)).FindOneOf(CStringCheckerConfig::CheckFilterReplace) != -1)
		{//�ַ�����
			b = FALSE;
		}
		else if(CString(strTemp.substr(i, 2).c_str()).FindOneOf(CStringCheckerConfig::CheckFilterReplace) != -1)
		{//���ֲ���
			i++;
			b = FALSE;
		}
		else if(i == 0 && c >= '0' && c <= '9') b = 2;
		else if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' )
		{
			if(c >= 0xB0 && c<= 0xF7 && UCHAR(strTemp[i + 1]) >= 0xA1 && UCHAR(strTemp[i + 1]) <= 0xFE)
			{//���ں��ַ�Χ
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			if(c == 0xA6 && UCHAR(strTemp[i + 1]) >= 0xA1 && UCHAR(strTemp[i + 1]) <= 0xFE)
			{//����ϣ����ĸ��Χ
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			if(c >= 0x80)
			{//��ȫ�Ƿ���
				strReturn += strRe;
				i++;
				continue;
			}
			if (c == '-') // '-'�л��߲��滻
			{
				strReturn += c;
				continue;
			}
			//��Ƿ���
			b = FALSE;
		}

		if(b == 2)
		{//���ַ�Ϊ����������滻�ַ�������Ӹ�����
			strReturn += strRe;
			strReturn += c;
		}
		else if(b)
		{//����ԭ�ַ�
			strReturn += c;
		}
		else
		{//�����滻�ַ���
			strReturn += strRe;
		}

	}

	return strReturn;
}

std::string CStringChecker::ReplaceDirName( LPCTSTR str, LPCTSTR strRe )
{
	std::string strTemp(str);
	std::string strReplace(strRe);
	std::string strReturn;

	if(strTemp.empty()) return strReturn;

	for(int i = 0; i < (int)strTemp.length(); i++)
	{
		UCHAR c = strTemp[i];
		BOOL b = TRUE;
		if(c < 0xB0 && CString(CHAR(c)).FindOneOf(CStringCheckerConfig::CheckFilterReplace) != -1)
		{//�ַ�����
			b = FALSE;
		}
		else if(CString(strTemp.substr(i, 2).c_str()).FindOneOf(CStringCheckerConfig::CheckFilterReplace) != -1)
		{//���ֲ���
			i++;
			b = FALSE;
		}
		else if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' )
		{
			if(c >= 0xB0 && c<= 0xF7 && UCHAR(strTemp[i + 1]) >= 0xA1 && UCHAR(strTemp[i + 1]) <= 0xFE)
			{//���ں��ַ�Χ
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			if(c >= 0x80)
			{//��ȫ�Ƿ���
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			b = FALSE;
		}

		if(b)
		{//����ԭ�ַ�
			strReturn += c;
		}
		else
		{//�����滻�ַ���
			strReturn += strRe;
		}

	}

	return strReturn;
}

//���12345
BOOL CStringChecker::CheckNodeName( LPCTSTR cStr )
{
// 	BOOL b;
// 	b = CheckEmpty(cStr);
// 	if(!b) return b;
// 	b = CheckWindowsRuler(cStr);
// 	if(!b) return b;
// 	b = CheckSpacePoint(cStr);
// 	if(!b) return b;
// 	b = CheckFirstNumber(cStr);
// 	if(!b) return b;
// 	b = CheckLength(cStr, 50);
// 	return b;

	//char str[] = "^[a-zA-Z_\\xB0-\\xF7\\xA1-\\xFE][a-zA-Z0-9_\\xB0-\\xF7\\xA1-\\xFE]*$";
	//BOOL b = RegexMatch(cStr, str);

	BOOL b = TRUE;
	LPCTSTR p = cStr;
	unsigned char c = *p;
	//if(c >= '0' && c <= '9') b =FALSE;
	//if(CheckIntNumber(cStr))b = FALSE;
	{
		char str[] = "^\\d+$";
		CString strTemp(cStr);
		//strTemp.Replace('-', '1');
		b = !RegexMatch(strTemp, str);
	}
	if(!b )
	{

	}
	else while(c = *p++)
	{
		if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_'  && c != '-') //������������м���
		//if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' )
		{
			if(
				(c >= 0xB0 && c<= 0xF7 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)
				|| (c == 0xA6 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)
				)
			{//���ں��ַ�Χ(16-87��)��ϣ����ĸ(6��)
				p++;
				continue;
			}
			b = FALSE;
			break;
		}
	}

	if(CString(cStr).FindOneOf(CStringCheckerConfig::CheckFilter) != -1)
	{//�ַ�����
		b = FALSE;
	}

	if(!b)
	{
		SetLastErr(CStringCheckerConfig::CheckErrorNode);
		return b; 
	}
	b = CheckEmpty(cStr);
	if(!b) return b;
	b = CheckLength(cStr, 50);
	return b;
}
BOOL CStringChecker::CheckParamName( LPCTSTR cStr )
{
	{
		CString sCheckString(cStr);
		if (sCheckString.Find("˫����Ӳ���") != -1)
		{
			SetLastErr(CStringCheckerConfig::CheckErrorParamCustom);
			return FALSE;
		}
	}

	BOOL b = TRUE;
	LPCTSTR p = cStr;
	int nlenth =0;
	unsigned char c = *p;

	nlenth = strlen(cStr);
	if (cStr[0]==0x20||cStr[nlenth-1]==0x20)
	{
		SetLastErr(CStringCheckerConfig::CheckErrorParam);
		return FALSE;
	}
		
	{
		char str[] = "^\\d+$";
		CString strTemp(cStr);
		//�ո�,���б�ܲ��ܺʹ�����һ����
		strTemp.Replace(' ', '1');
		strTemp.Replace('.', '1');
		strTemp.Replace('\\', '1');
		//strTemp.Replace('-', '1');
		b = !RegexMatch(strTemp, str);
	}
	if(!b)
	{

	}
	else while(c = *p++)
	{
		 
		if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && \
			(c != '_')&& (c!='\\') && (c!='.') &&(c!=' ') &&(c!='-') )//������������м��š�.��\��
		{
			if(
				(c >= 0xB0 && c<= 0xF7 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)
				|| (c == 0xA6 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE))
			{//���ں��ַ�Χ(16-87��)��ϣ����ĸ(6��)
				p++;
				continue;
			}

			b = FALSE;
			break;
		}
	}

	if(!b)
	{
		SetLastErr(CStringCheckerConfig::CheckErrorParam);
		return b; 
	}
	b = CheckEmpty(cStr);
	if(!b) return b;
	b = CheckLength(cStr, 50);
	return b;
}

//���1245
BOOL CStringChecker::CheckDirName( LPCTSTR cStr )
{
// 	BOOL b;
// 	b = CheckEmpty(cStr);
// 	if(!b) return b;
// 	b = CheckWindowsRuler(cStr);
// 	if(!b) return b;
// 	b = CheckSpacePoint(cStr);
// 	if(!b) return b;
// 	b = CheckLength(cStr, 20);
// 	return b;

	//char str[] = "^[a-zA-Z_\\xB0-\\xF7\\xA1-\\xFE][a-zA-Z0-9_\\xB0-\\xF7\\xA1-\\xFE]*$";
	//BOOL b = RegexMatch(cStr, str);

	BOOL b = TRUE;
 	LPCTSTR p = cStr;
 	unsigned char c = *p;
 	//if(c >= '0' && c <= '9') b =FALSE;
 	//else 
 		while(c = *p++)
 	{
 		if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' )
 		{
 			if(
				(c >= 0xB0 && c<= 0xF7 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)
				|| (c == 0xA6 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)				
				)
			{//���ں��ַ�Χ(16-87��)��ϣ����ĸ(6��)
 				p++;
 				continue;
 			}
			if (c=='-') // �����ļ����������л��߸�'-'
				continue;
 			b = FALSE;
 			break;
 		}
 	}
 
 	if(CString(cStr).FindOneOf(CStringCheckerConfig::CheckFilter) != -1)
 	{//�ַ�����
 		b = FALSE;
 	}
 
 	if(!b)
 	{
 		SetLastErr(CStringCheckerConfig::CheckErrorDir);
 		return b; 
 	}
	b = CheckEmpty(cStr);
	if(!b) return b;
	b = CheckLength(cStr, 20);
	return b;
}

//���124
BOOL CStringChecker::CheckName( LPCTSTR cStr )
{
	BOOL b;
	b = CheckWindowsRuler(cStr);
	if(!b) return b;
	b = CheckSpacePoint(cStr);
	if(!b) return b;
	b = CheckLength(cStr, 20);
	return b;
}


// 6.�汾���������Ϲ���
BOOL CStringChecker::CheckVersion( LPCTSTR cStr )
{	
	{//����ƥ�䷨
		char str[] = "^([Vv]?\\d+(.\\d+)*)?$";
		BOOL b = RegexMatch(cStr, str);
		if(!b)
		{
			SetLastErr(TYPE_VERSION);
			return FALSE;
		}
		return TRUE;
	}

// 	CString cStrRet = CheckLength(cStr, 20);
// 	if(!cStrRet.IsEmpty()) return cStrRet;
// 
// 	bool bCheckSuccess = true;
// 
// 	int nLength = cStr.GetLength();
// 
// 
// 	for(int i = 0; i < nLength; i++)
// 	{
// 		if(cStr[i] != '.' && (cStr[i] < '0' || cStr[i] >'9'))
// 		{//�ַ�����
// 			bCheckSuccess = false;
// 			break;
// 		}
// 
// 		if(i > 0 && cStr[i - 1] == '.' && cStr[i] == '.')
// 		{//����dot����
// 			bCheckSuccess = false;
// 			break;
// 		}
// 	}
// 
// 	if(nLength != 0)
// 	{//��β��֤
// 		if(cStr[0] == '.' || cStr[nLength - 1] == '.')
// 		{
// 			bCheckSuccess = false;
// 		}
// 	}
// 
// 	if(!bCheckSuccess)
// 	{
// 		return "�������Ϲ���";
// 	}
// 
// 	return "";
}


// ���Int����
BOOL CStringChecker::CheckInt(CString cStr)
{
	if(!CheckIntNumber(cStr))
	{
		return FALSE;
	}
	std::stringstream ss;
	int n;
	ss << cStr;
	ss >> n;
	if(ss.fail())
	{
		SetLastErr("����Խ��");
		return FALSE;
	}
	return TRUE;//"";
}

// �������
BOOL CStringChecker::CheckIntNumber(CString cStr)
{
	char str[] = "^[\\-]?([1-9]\\d*)|0$";
	CString strTemp(cStr);
	strTemp.Trim(' ');
	BOOL b = RegexMatch(strTemp, str);
	//BOOL b = RegexMatch(cStr, str);
	if(!b)
	{
		SetLastErr(TYPE_INT);
		return FALSE;//"�������Ϲ���";
	}

	CString sInt(cStr);
	if (sInt == "-0")
	{
		SetLastErr(TYPE_INT);
		return FALSE;
	}

	return TRUE;//"";
}


// ��⸡����
// "+73.24745e-363"
BOOL CStringChecker::CheckDouble( LPCTSTR cStr )
{
	char str[] = "^([\\+\\-])?\\d+(\\.\\d+)?([Ee][\\+\\-]?\\d+)?$";
	CString strTemp(cStr);
	strTemp.Trim(' ');
	BOOL b = RegexMatch(strTemp, str);
	//BOOL b = RegexMatch(cStr, str);
	if(!b)
	{
		SetLastErr(TYPE_DOUBLE);
		return FALSE; //"���������Ϲ���";
	}
	std::stringstream ss;
	double n;
	ss << cStr;
	ss >> n;
	if(ss.fail())
	{
		SetLastErr("������Խ��");
		return FALSE;
	}
	else if(n == numeric_limits<double>::infinity() || n == -numeric_limits<double>::infinity())
	{
		SetLastErr("������Խ��");
		return FALSE;
	}

	return TRUE; //"";
}

// ��Ⲽ����
BOOL CStringChecker::CheckBool(CString cStr)
{
	BOOL b = TRUE;
	if(cStr != "false" && cStr != "true")
	{
		b = FALSE;
	}
	if(!b)
	{
		SetLastErr(TYPE_BOOL);
		return FALSE;//"�������Ϲ���";
	}
	return TRUE;//"";
}


// �������
BOOL CStringChecker::CheckTime( LPCTSTR cStr )
{//////////////////////////////////δ���////////////////////////////////////////
	return TRUE;
}

// ����ƥ��
//************************************
//ʹ�÷�����
//RegexMatch("98a76", "[0-9][0-9]");
//ǰ��Ϊ��Ҫƥ����ִ�������Ϊ������ʽ
//************************************
BOOL CStringChecker::RegexMatch(const char* pStr, const char* pStrRegex)
{
	CAtlRegExp<> re; 
	CAtlREMatchContext<> mc; 
	re.Parse(pStrRegex); 
	return re.Match(pStr, &mc);
}

// ��ȡ������Ϣ
CString CStringChecker::GetLastErr()
{
	return m_strErr;
}

//���ô�����Ϣ
void CStringChecker::SetLastErr(LPCTSTR strErr)
{
	m_strErr = strErr;
}

// ���ô�����Ϣ
void CStringChecker::SetLastErr(CheckType checkType)
{
	SetLastErr(m_strErrs[checkType]);
}
