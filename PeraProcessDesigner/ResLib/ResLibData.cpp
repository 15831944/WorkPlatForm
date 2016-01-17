#include "StdAfx.h"
#include "ResLibData.h"


CResLibData::CResLibData(void)
{
}
void CResLibData::Myreplace(string &str,string oldstr,string newstr) 
{  
	int pos=string::npos; 
	int start_index=0;  
	int old_str_len=oldstr.size(),new_str_len=new_str_len=newstr.size(); 
	while((pos=str.find(oldstr,start_index))!=string::npos) 
	{  
		str.replace(pos,old_str_len,newstr); 
		start_index=pos+new_str_len; 
	} 
}  

CResLibData::~CResLibData(void)
{
	for (list<CResLibData*>::iterator it = m_pBaseList.begin();it != m_pBaseList.end();it++)
	{
		delete (*it);
		(*it) = NULL;
	}
	m_pBaseList.clear();
}
int CResLibData::GetSqlLists(list<string>& SqlList)
{
	for (list<string>::iterator it = m_SqlList.begin();it != m_SqlList.end();it++)
	{
		SqlList.push_back(*it);
	}
	return SqlList.size();
}

bool CResLibData::RemoveBaseList(CResLibData* pResLibDataBase)
{
	m_pBaseList.remove(pResLibDataBase);
	if (pResLibDataBase != NULL)
	{
		delete pResLibDataBase;
		pResLibDataBase=NULL;
	}
	
	return true;
}