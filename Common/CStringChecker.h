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
	// 1.windows�ļ����������򣬲�������� \ / : * ? " < > |
	static BOOL CheckWindowsRuler(LPCTSTR cStr);
	// 2.��������� . ���ո�
	static BOOL CheckSpacePoint(LPCTSTR cStr);
	// 3.����ĸ����Ϊ����
	static BOOL CheckFirstNumber(LPCTSTR cStr);
	// 4.��󳤶�Ϊ20
	static BOOL CheckLength(LPCTSTR cStr, int nLength);
	//���12345
	static BOOL CheckNodeName(LPCTSTR cStr);
	static BOOL CheckParamName(LPCTSTR cStr);
	//���1245
	static BOOL CheckDirName(LPCTSTR cStr);
	//���124
	static BOOL CheckName(LPCTSTR cStr);
	// 5.������Ϊ��
	static BOOL CheckEmpty(LPCTSTR cStr);
	// 6.�汾���������Ϲ���
	static BOOL CheckVersion(LPCTSTR cStr);
	// �������
	static BOOL CheckInt(CString cStr);
	// //��⸡����
	static BOOL CheckDouble(LPCTSTR cStr);
	//��Ⲽ����
	static BOOL CheckBool(CString cStr);
	// �������
	static BOOL CheckTime(LPCTSTR cStr);
	// ����ƥ��
	static BOOL RegexMatch(const char* pStr, const char* pStrRegex);
	// ��ȡ������Ϣ
	static CString GetLastErr();
	//�滻�ַ����еķ��ţ�strΪԴ�ַ�����strReΪ�滻���ַ�����Ҫ�滻�ķ��Ű�������λ�������ַ�����Ӣ�ĺ��ֵ��ַ�����CStringCheckerConfig�����õ��ַ����������滻����ַ���
	static std::string ReplaceNodeName(LPCTSTR str, LPCTSTR strRe);
	//�滻�ַ����еķ��ţ�strΪԴ�ַ�����strReΪ�滻���ַ�����Ҫ�滻�ķ��Ű�������Ӣ�ĺ��ֵ��ַ�����CStringCheckerConfig�����õ��ַ����������滻����ַ���
	static std::string ReplaceDirName( LPCTSTR str, LPCTSTR strRe );
	//��ʽ��������ΪҪ��ĸ�ʽ
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
	// ���ô�����Ϣ
	static void SetLastErr(const TCHAR* strErr);
	static void SetLastErr(CheckType checkType);
	static BOOL CheckIntNumber(CString cStr);
	static CString m_strErr;  //��������
};

