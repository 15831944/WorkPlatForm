#pragma once
#include <string>
using namespace std;
class CEncodedTool
{
public:
	CEncodedTool(void);
	~CEncodedTool(void);

	//UTF8编码转为多字节编码
	static bool UTF8ToMB( char*& pmb, const char* pu8 );
	//多字节编码转为UTF8编码
	static bool MBToUTF8( char*& pu8, const char* pmb );
	//UTF8转多字节
	static std::string& UTF8ToMB(std::string& str);
	//多字节转UTF8
	static std::string& MBToUTF8(std::string& str);

	static void StringToLPCTSTR(string stData,WCHAR*  LpStr,int nStrLen);
	static int StrReplace(string& src_str, const string& old_str, const string& new_str);
};

