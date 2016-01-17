#pragma once

class CIniClass
{
public:  
     CIniClass();                 // ���캯�� 
     CIniClass(CString strFile);  // ���ļ��������Ĺ��캯��
     ~CIniClass();                // ������������ͷ�

public:
 bool DelSection(CString strSection);
 bool Delete(CString  strSection, CString strKey);
 CString GetEqualeRight(CString str);
     void SetINIFileName(CString strINIFile);            // �趨INI�ļ������֣�Ҫ��ȫ·����
     CStringList* GetSectionData(CString strSection);    // ��ȡ������������ 
     CStringList* GetSectionNames();                     // ȡ���ж���
     BOOL SectionExists(CString strSection);             // �ж϶����Ƿ��Ѿ�����
     long SetKey(CString strValue, CString strKey, CString strSection);    // ���ü�ֵ
     CString GetKeyValue(CString strKey,CString strSection);               // ��ü�ֵ           

private:
  CStringList *m_sectionDataList;       // ��������������ݵ��б�
  CStringList *m_sectionList;           // �����б�
  long m_lRetValue;                     
  CString m_strFileName;                // ·����
};
