#include "StdAfx.h"
#include "EncodedTool.h"


CEncodedTool::CEncodedTool(void)
{
}


CEncodedTool::~CEncodedTool(void)
{
}




//UTF8编码转为多字节编码
bool CEncodedTool::UTF8ToMB( char*& pmb, const char* pu8 )
{
	// convert an UTF8 string to widechar   
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, -1, NULL, 0);  

	WCHAR* lpszW = NULL;  
	try  
	{  
		lpszW = new WCHAR[nLen];  
	}  
	catch(std::bad_alloc &memExp)  
	{  
		memExp;
		return false;  
	}  

	int nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, -1, lpszW, nLen);  

	if(nRtn != nLen)  
	{  
		delete[] lpszW;  
		return false;  
	}  

	// convert an widechar string to Multibyte   
	int MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);  
	if (MBLen <=0)  
	{  
		return false;  
	}	
	pmb = new char[MBLen+1];
	nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, pmb, MBLen, NULL, NULL);
	delete[] lpszW;  

	if(nRtn != MBLen)  
	{		
		delete pmb;
		return false;  
	}  
	return true;  
}

//多字节编码转为UTF8编码
bool CEncodedTool::MBToUTF8( char*& pu8, const char* pmb )
{
	// convert an MBCS string to widechar   
	int nLen = MultiByteToWideChar(CP_ACP, 0, pmb, -1, NULL, 0);  

	WCHAR* lpszW = NULL;  
	try  
	{  
		lpszW = new WCHAR[nLen];  
	}  
	catch(std::bad_alloc &memExp)  
	{  
		memExp;
		return false;  
	}  

	int nRtn = MultiByteToWideChar(CP_ACP, 0, pmb, -1, lpszW, nLen);  

	if(nRtn != nLen)  
	{  
		delete[] lpszW;  
		return false;  
	}  
	// convert an widechar string to utf8  
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, NULL, 0, NULL, NULL);  
	if (utf8Len <= 0)  
	{  
		return false;  
	}  

	pu8 = new char[utf8Len + 1];

	nRtn = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, pu8, utf8Len, NULL, NULL);
	pu8[utf8Len] = '\0';
	delete[] lpszW;  

	if (nRtn != utf8Len)  
	{  
		//pu8.clear();  
		delete pu8;
		return false;  
	}  
	return true;
}

//UTF8转多字节
std::string& CEncodedTool::UTF8ToMB(std::string& str)
{
	char* sTemp;
	UTF8ToMB(sTemp, str.c_str());
	str = sTemp;
	delete[] sTemp;
	sTemp = NULL;
	return str;
}
//多字节转UTF8
std::string& CEncodedTool::MBToUTF8(std::string& str)
{
	char* sTemp;
	MBToUTF8(sTemp, str.c_str());
	str = sTemp;
	delete[] sTemp;
	sTemp = NULL;
	return str;
}
void CEncodedTool::StringToLPCTSTR(string stData,WCHAR*  LpStr,int nStrLen)
{  
	MultiByteToWideChar(CP_THREAD_ACP,MB_USEGLYPHCHARS,stData.c_str(),stData.length(),LpStr,nStrLen);   

}

int CEncodedTool::StrReplace(string& src_str, const string& old_str, const string& new_str)
{
	int count = 0;
	int old_str_len = old_str.length(), new_str_len = new_str.length();
	int pos = 0;
	while( (pos = src_str.find( old_str, pos ) ) != string::npos )
	{
		src_str.replace( pos, old_str_len, new_str );
		pos += new_str_len;
		++count;
	}
	return count;
}