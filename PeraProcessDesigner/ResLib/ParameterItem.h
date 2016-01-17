#pragma once
#include <string>
#include <map>

using namespace std;

class CParameterItem
{
public:
	CParameterItem(void);
	virtual ~CParameterItem(void);
	map<string,string>	m_IdValueMap;
// 	string stId;		//"d11af104-708f-452c-96ca-d476ce632637" 
// 	string stName;				//="a" 
// 	string stDirection;		//="Out" 
// 	string stType;		//="Double" 
// 	string stDesc;		//="" 
// 	string stFormat;		//="" 
// 	string stValue;		//="1" 
// 	string stListname;		//=""
// 	string stGroup;		//="" />
// 	string stisdynamicarray;		//="" />
// 	string stunits;		//="" />
};

