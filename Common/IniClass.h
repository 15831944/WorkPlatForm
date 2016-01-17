#pragma once

class CIniClass
{
public:  
     CIniClass();                 // 构造函数 
     CIniClass(CString strFile);  // 带文件名参数的构造函数
     ~CIniClass();                // 析构清空链表释放

public:
 bool DelSection(CString strSection);
 bool Delete(CString  strSection, CString strKey);
 CString GetEqualeRight(CString str);
     void SetINIFileName(CString strINIFile);            // 设定INI文件的名字，要完全路径名
     CStringList* GetSectionData(CString strSection);    // 获取段类所有数据 
     CStringList* GetSectionNames();                     // 取所有段名
     BOOL SectionExists(CString strSection);             // 判断段名是否已经存在
     long SetKey(CString strValue, CString strKey, CString strSection);    // 设置键值
     CString GetKeyValue(CString strKey,CString strSection);               // 获得键值           

private:
  CStringList *m_sectionDataList;       // 保存段类所有数据的列表
  CStringList *m_sectionList;           // 段名列表
  long m_lRetValue;                     
  CString m_strFileName;                // 路径名
};
