#include "stdafx.h"
#include "CStringChecker.h"
#include "CStringCheckerConfig.h"
#include "atlrx.h"
#include <atldbcli.h>
#include <sstream>


LPCTSTR CStringChecker::m_strErrs[TYPE_COUNT] = {
	"不允许出现 \\ / : * ? \" < > |",
	"不允许出现 . 及空格",
	"首字母不能为数字",
	"最大长度不得超过",
	"不允许为空",
	"非法，命名规则如下：\n1. 可以包括字母、数字、下划线或中文\n2. 不能包含空格、标点符号、运算符等其他符号\n3. 不能以数字开头\n4. 长度不能超过50",
	"非法，命名规则如下：\n1. 可以包括字母、数字、下划线或中文\n2. 不能包含空格、标点符号、运算符等其他符号\n3. 不能以数字开头\n4. 长度不能超过20",
	"命名规则验证失败",
	"命名不合规则",
	"整数不合规则",
	"浮点数不合规则",
	"布尔数据不合规则",
	"不合规则"
};

CString CStringChecker::m_strErr;

CStringChecker::CStringChecker(void)
{
}


CStringChecker::~CStringChecker(void)
{
}

//转换成double，格式为.0
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

// 1.windows文件夹命名规则，不允许出现 \ / : * ? " < > |
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


// 2.不允许出现 . 及空格
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


// 3.首字母不能为数字
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

// 4.最大长度为20
BOOL CStringChecker::CheckLength( LPCTSTR cStr, int nLength )
{
	if(CStringW(cStr).GetLength() > nLength)
	{
		CString cStrRet;
		cStrRet.Format("%s%d个字符", m_strErrs[TYPE_LEN], nLength);
		SetLastErr(cStrRet);
		return FALSE;
	}
	return TRUE;
}

// 5.不允许为空
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
		{//字符查找
			b = FALSE;
		}
		else if(CString(strTemp.substr(i, 2).c_str()).FindOneOf(CStringCheckerConfig::CheckFilterReplace) != -1)
		{//汉字查找
			i++;
			b = FALSE;
		}
		else if(i == 0 && c >= '0' && c <= '9') b = 2;
		else if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' )
		{
			if(c >= 0xB0 && c<= 0xF7 && UCHAR(strTemp[i + 1]) >= 0xA1 && UCHAR(strTemp[i + 1]) <= 0xFE)
			{//属于汉字范围
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			if(c == 0xA6 && UCHAR(strTemp[i + 1]) >= 0xA1 && UCHAR(strTemp[i + 1]) <= 0xFE)
			{//属于希腊字母范围
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			if(c >= 0x80)
			{//是全角符号
				strReturn += strRe;
				i++;
				continue;
			}
			if (c == '-') // '-'中画线不替换
			{
				strReturn += c;
				continue;
			}
			//半角符号
			b = FALSE;
		}

		if(b == 2)
		{//首字符为数字先添加替换字符，再添加该数字
			strReturn += strRe;
			strReturn += c;
		}
		else if(b)
		{//加上原字符
			strReturn += c;
		}
		else
		{//加上替换字符串
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
		{//字符查找
			b = FALSE;
		}
		else if(CString(strTemp.substr(i, 2).c_str()).FindOneOf(CStringCheckerConfig::CheckFilterReplace) != -1)
		{//汉字查找
			i++;
			b = FALSE;
		}
		else if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' )
		{
			if(c >= 0xB0 && c<= 0xF7 && UCHAR(strTemp[i + 1]) >= 0xA1 && UCHAR(strTemp[i + 1]) <= 0xFE)
			{//属于汉字范围
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			if(c >= 0x80)
			{//是全角符号
				strReturn += strTemp.substr(i, 2);
				i++;
				continue;
			}
			b = FALSE;
		}

		if(b)
		{//加上原字符
			strReturn += c;
		}
		else
		{//加上替换字符串
			strReturn += strRe;
		}

	}

	return strReturn;
}

//检查12345
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
		if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_'  && c != '-') //允许参数名含有减号
		//if((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' )
		{
			if(
				(c >= 0xB0 && c<= 0xF7 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)
				|| (c == 0xA6 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)
				)
			{//属于汉字范围(16-87区)或希腊字母(6区)
				p++;
				continue;
			}
			b = FALSE;
			break;
		}
	}

	if(CString(cStr).FindOneOf(CStringCheckerConfig::CheckFilter) != -1)
	{//字符查找
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
		if (sCheckString.Find("双击添加参数") != -1)
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
		//空格,点和斜杠不能和纯数字一起用
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
			(c != '_')&& (c!='\\') && (c!='.') &&(c!=' ') &&(c!='-') )//允许参数名含有减号、.和\。
		{
			if(
				(c >= 0xB0 && c<= 0xF7 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE)
				|| (c == 0xA6 && UCHAR(*p) >= 0xA1 && UCHAR(*p) <= 0xFE))
			{//属于汉字范围(16-87区)或希腊字母(6区)
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

//检查1245
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
			{//属于汉字范围(16-87区)或希腊字母(6区)
 				p++;
 				continue;
 			}
			if (c=='-') // 允许文件夹名称有中画线杠'-'
				continue;
 			b = FALSE;
 			break;
 		}
 	}
 
 	if(CString(cStr).FindOneOf(CStringCheckerConfig::CheckFilter) != -1)
 	{//字符查找
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

//检查124
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


// 6.版本号命名不合规则
BOOL CStringChecker::CheckVersion( LPCTSTR cStr )
{	
	{//正则匹配法
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
// 		{//字符过滤
// 			bCheckSuccess = false;
// 			break;
// 		}
// 
// 		if(i > 0 && cStr[i - 1] == '.' && cStr[i] == '.')
// 		{//连续dot过滤
// 			bCheckSuccess = false;
// 			break;
// 		}
// 	}
// 
// 	if(nLength != 0)
// 	{//首尾验证
// 		if(cStr[0] == '.' || cStr[nLength - 1] == '.')
// 		{
// 			bCheckSuccess = false;
// 		}
// 	}
// 
// 	if(!bCheckSuccess)
// 	{
// 		return "命名不合规则";
// 	}
// 
// 	return "";
}


// 检测Int类型
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
		SetLastErr("整数越界");
		return FALSE;
	}
	return TRUE;//"";
}

// 检测整数
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
		return FALSE;//"整数不合规则";
	}

	CString sInt(cStr);
	if (sInt == "-0")
	{
		SetLastErr(TYPE_INT);
		return FALSE;
	}

	return TRUE;//"";
}


// 检测浮点数
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
		return FALSE; //"浮点数不合规则";
	}
	std::stringstream ss;
	double n;
	ss << cStr;
	ss >> n;
	if(ss.fail())
	{
		SetLastErr("浮点数越界");
		return FALSE;
	}
	else if(n == numeric_limits<double>::infinity() || n == -numeric_limits<double>::infinity())
	{
		SetLastErr("浮点数越界");
		return FALSE;
	}

	return TRUE; //"";
}

// 检测布尔型
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
		return FALSE;//"整数不合规则";
	}
	return TRUE;//"";
}


// 检测日期
BOOL CStringChecker::CheckTime( LPCTSTR cStr )
{//////////////////////////////////未完成////////////////////////////////////////
	return TRUE;
}

// 正则匹配
//************************************
//使用方法：
//RegexMatch("98a76", "[0-9][0-9]");
//前者为需要匹配的字串，后者为正则表达式
//************************************
BOOL CStringChecker::RegexMatch(const char* pStr, const char* pStrRegex)
{
	CAtlRegExp<> re; 
	CAtlREMatchContext<> mc; 
	re.Parse(pStrRegex); 
	return re.Match(pStr, &mc);
}

// 获取错误信息
CString CStringChecker::GetLastErr()
{
	return m_strErr;
}

//设置错误信息
void CStringChecker::SetLastErr(LPCTSTR strErr)
{
	m_strErr = strErr;
}

// 设置错误信息
void CStringChecker::SetLastErr(CheckType checkType)
{
	SetLastErr(m_strErrs[checkType]);
}
