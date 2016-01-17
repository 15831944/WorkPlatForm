#pragma once
#include <string>
using namespace std;
class CEncodedTool
{
public:
	CEncodedTool(void);
	~CEncodedTool(void);

	//UTF8����תΪ���ֽڱ���
	static bool UTF8ToMB( char*& pmb, const char* pu8 );
	//���ֽڱ���תΪUTF8����
	static bool MBToUTF8( char*& pu8, const char* pmb );
	//UTF8ת���ֽ�
	static std::string& UTF8ToMB(std::string& str);
	//���ֽ�תUTF8
	static std::string& MBToUTF8(std::string& str);

	static void StringToLPCTSTR(string stData,WCHAR*  LpStr,int nStrLen);
	static int StrReplace(string& src_str, const string& old_str, const string& new_str);
};

