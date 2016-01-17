#pragma once
#include <atlstr.h>
#include "StlHeader.h"

class CStringChecker
{
public:

	enum CheckType
	{
		TYPE_WR = 0,
		TYPE_SP,
		TYPE_FN,
		TYPE_LEN,
		TYPE_EMPTY,
		TYPE_NODE,
		TYPE_DIR,
		TYPE_NAME,		
		TYPE_VERSION,
		TYPE_INT,
		TYPE_DOUBLE,
		TYPE_BOOL,
		TYPE_TIME,
		TYPE_COUNT
	};

	static LPCTSTR m_strErrs[TYPE_COUNT];

	CStringChecker(void);
	virtual ~CStringChecker(void);
	// 1.windows文件夹命名规则，不允许出现 \ / : * ? " < > |
	static BOOL CheckWindowsRuler(LPCTSTR cStr);
	// 2.不允许出现 . 及空格
	static BOOL CheckSpacePoint(LPCTSTR cStr);
	// 3.首字母不能为数字
	static BOOL CheckFirstNumber(LPCTSTR cStr);
	// 4.最大长度为20
	static BOOL CheckLength(LPCTSTR cStr, int nLength);
	//检查12345
	static BOOL CheckNodeName(LPCTSTR cStr);
	static BOOL CheckParamName(LPCTSTR cStr);
	//检查1245
	static BOOL CheckDirName(LPCTSTR cStr);
	//检查124
	static BOOL CheckName(LPCTSTR cStr);
	// 5.不允许为空
	static BOOL CheckEmpty(LPCTSTR cStr);
	// 6.版本号命名不合规则
	static BOOL CheckVersion(LPCTSTR cStr);
	// 检测整数
	static BOOL CheckInt(CString cStr);
	// //检测浮点数
	static BOOL CheckDouble(LPCTSTR cStr);
	//检测布尔型
	static BOOL CheckBool(CString cStr);
	// 检测日期
	static BOOL CheckTime(LPCTSTR cStr);
	// 正则匹配
	static BOOL RegexMatch(const char* pStr, const char* pStrRegex);
	// 获取错误信息
	static CString GetLastErr();
	//替换字符串中的符号，str为源字符串，strRe为替换的字符，需要替换的符号包括（首位是数字字符，非英文汉字的字符，在CStringCheckerConfig中配置的字符），返回替换后的字符串
	static std::string ReplaceNodeName(LPCTSTR str, LPCTSTR strRe);
	//替换字符串中的符号，str为源字符串，strRe为替换的字符，需要替换的符号包括（非英文汉字的字符，在CStringCheckerConfig中配置的字符），返回替换后的字符串
	static std::string ReplaceDirName( LPCTSTR str, LPCTSTR strRe );
	//格式浮点数据为要求的格式
	/************************************************************************/
	/*
	1             ->        1.0
	1.000      ->        1.0
	1.0001    ->        1.0001
	1.00100  ->        1.00100
	1.100      ->        1.100
	001         ->        1.0
	001.000  ->        1.0
	001.001  ->        1.001
	*/
	/************************************************************************/
	static CString FormatDouble( LPCTSTR cStr );

private:
	// 设置错误信息
	static void SetLastErr(const TCHAR* strErr);
	static void SetLastErr(CheckType checkType);
	static BOOL CheckIntNumber(CString cStr);
	static CString m_strErr;  //错误描述
};

