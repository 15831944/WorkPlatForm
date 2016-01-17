// IniClass.cpp

#include "stdafx.h"
#include "IniClass.h"

CIniClass::CIniClass()
{  
	m_sectionList = new CStringList();
    m_sectionDataList = new CStringList();
}

// ���ļ��������Ĺ��캯��
CIniClass::CIniClass(CString strFile)     
{ 
	m_strFileName = strFile;
    m_sectionList = new CStringList();
    m_sectionDataList = new CStringList();
}

// ������������ͷ�
CIniClass::~CIniClass()                    
{
    delete m_sectionList;
    delete m_sectionDataList;
}

// ��ü�ֵ
CString CIniClass::GetKeyValue(CString strKey,CString strSection)            
{
    char ac_Result[255];
    m_lRetValue =GetPrivateProfileString((LPCTSTR)strSection,(LPCTSTR)strKey,"",ac_Result,255,(LPCTSTR)m_strFileName); 
    CString strResult(ac_Result);
    return strResult;
}

// ���ü�ֵ
long CIniClass::SetKey(CString strValue, CString strKey, CString strSection)  
{
    m_lRetValue=WritePrivateProfileString(strSection,strKey,strValue,m_strFileName);
    return m_lRetValue;
}

// �ж϶����Ƿ��Ѿ�����
BOOL CIniClass::SectionExists(CString strSection)        
{
    char ac_Result[100]; 
    CString csAux;
 
    m_lRetValue=GetPrivateProfileString((LPCTSTR)strSection,NULL,"",ac_Result,90,(LPCTSTR)m_strFileName);
    return (m_lRetValue>0);
}

// ȡ���ж���
CStringList* CIniClass::GetSectionNames()                
{
    char ac_Result[2000];
    m_sectionList->RemoveAll();
 
    m_lRetValue=GetPrivateProfileSectionNames(ac_Result,2000,(LPCTSTR)m_strFileName);
    CString strSectionName;
    for(int i=0;i<m_lRetValue;i++)
 {
        if(ac_Result[i] !='\0') 
  {
   strSectionName=strSectionName+ac_Result[i];
  } 
  else 
  {
   if(strSectionName!="")
   {
    m_sectionList->InsertAfter(m_sectionList->GetTailPosition(),strSectionName);
   }
            strSectionName ="";
  }
 }
    return m_sectionList;
}

// ��ȡ������������
CStringList* CIniClass::GetSectionData(CString strSection)  
{
    char ac_Result[2000]; 
    m_sectionDataList->RemoveAll();
    m_lRetValue=GetPrivateProfileSection((LPCTSTR)strSection,ac_Result,2000,(LPCTSTR)m_strFileName);

    CString strSectionData;
    for(int i=0;i<m_lRetValue;i++)
 {  
  if(ac_Result[i]!='\0') 
  {       
   strSectionData=strSectionData+ac_Result[i];
  }
  else
  {
            if(strSectionData!="") 
   {
                m_sectionDataList->InsertAfter(m_sectionDataList->GetTailPosition(),GetEqualeRight(strSectionData));
   }
            strSectionData = "";
  }
 }
    return m_sectionDataList;
}

// �趨INI�ļ������֣�Ҫ��ȫ·����
void CIniClass::SetINIFileName(CString strINIFile)
{
    m_strFileName = strINIFile;
}

//ȡ�ַ����ұ߲���
CString CIniClass::GetEqualeRight(CString str)
{
 int pos=str.Find('=');
    return str.Right(str.GetLength()-pos-1);
}


//ɾ��ָ����ֵ
bool CIniClass::Delete(CString  strSection, CString strKey)
{
    if(WritePrivateProfileString((LPCTSTR)strSection,(LPCTSTR)strKey,NULL,(LPCTSTR)m_strFileName))
  return FALSE;
 else
        return true;
}

//ɾ���ֶ�
bool CIniClass::DelSection(CString strSection)
{
 if(WritePrivateProfileString((LPCTSTR)strSection,NULL,NULL,(LPCTSTR)m_strFileName))
  return FALSE;
 else
        return true;
}