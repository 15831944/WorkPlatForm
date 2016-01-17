#pragma once

#include <windows.h>
#include <string>
#include "XZip.h"
#include "XUnzip.h"
#include "StlHeader.h"

#ifndef CPPTL_JSON_H_INCLUDED
#include "value.h"
#endif

class UpdateFileObj
{
public:
	UpdateFileObj()
	{
		m_strFileName.clear();
		m_strNewName.clear();
		m_pszFileContent = NULL;
		m_iFileLen = NULL;
	}
	UpdateFileObj(const UpdateFileObj & other)
	{
		m_strFileName = other.m_strFileName;
		m_strNewName = other.m_strNewName;
		m_pszFileContent = other.m_pszFileContent;
		m_iFileLen = other.m_iFileLen;
	}
	std::string m_strFileName; // Ҫ�滻��zip�е��ļ���
	std::string m_strNewName; // �滻����ļ�����
	char * m_pszFileContent; // �滻������ļ����ֽ���
	int  m_iFileLen;       // �滻������ļ����ֽ�������
};
class ZipTool
{
public:

	enum METHOD
	{
		BEST  = -1   ,          
		STORE  = 0   ,              
		DEFLATE  = 8             
	};

	ZipTool(void);
	virtual ~ZipTool(void);
	// �����ļ��Ƿ����
	static bool FindFileInZip(std::string strZip, std::string strFile);
	// ȡ�ļ�����
	static std::string GetContentFromZip(std::string strZip, std::string strFile);
	// ��ѹһ���ļ���ĳ��
	static bool UnzipFile(std::string strZip, std::string strFileSrc, std::string strFileDst);
	// ��ѹ��������ĳ�ļ���
	static bool Unzip(std::string strZip, std::string strDir);
	// //��ѹ���е�ĳ���ļ���ָ��Ŀ¼
	static bool UnzipDir(std::string strZip, std::string strDir, std::string strDirDst);
	// ��������·��ת��ΪZIP��ʶ���·��
	static std::string ConvertToZipPath(std::string path);
	// ��ָ��Ŀ¼����ZIP�ļ�������������ָ��ѹ���ļ��Ƿ������Ŀ¼��Ĭ�ϰ���
	static bool ZipFromDir(std::string strZip, std::string strDir, int method = DEFLATE, bool isContainDir = false);

	static bool ZipFromJson(std::string strZip, Json::Value& root, int method = DEFLATE);  //��Json����zip

	static bool AddPathToHZIP(HZIP hz, std::string strDst, std::string strSrc, int method = DEFLATE, bool isContainDir = false);
	//��ָ��·������ļ���Ŀ¼��Ŀ��zip��·������������ӣ������������½�
	static bool Zip(std::string strZip, std::string strDst, std::string strSrc, int method = DEFLATE, bool isContainDir = false);
	//����·��
	static std::string TrimPath(std::string path, char c);

	static bool StringToJson(std::string str, Json::Value& jv);

	static bool ZipFromJsonString(std::string strZip, std::string jsonString, int method = DEFLATE);
	// �滻zip�еĵ����ļ�
	static bool ReplaceZipFile(const std::string & strZip,const std::string & strOutZip,const std::string & strFileName,const std::string & strNewContent,int method = DEFLATE);
	// �滻zip�еĶ���ļ�
	static bool ReplaceZipFile(const std::string & strZip,const std::string & strOutZip,std::vector<UpdateFileObj> vModifyFile,int method = DEFLATE);

	// ��һ���ڴ�zip�ļ��ж�ȡĳ���ļ������ݣ����ض�ȡ���ݵ��ڴ��ַ�ͳ��ȣ���Ҫ�ⲿ�ͷ�
	static char* GetContentFormMemZip(char * pszZipStreamMem,int iSizeMem,const char * pszFetchFileName,int &iOutLen);

	// ����һ���ڴ�zip�ļ��е�ĳ���ļ����õ�һ���µ��ڴ�zip�ļ������ڴ��ļ����ڴ���Ҫ���ⲿ�ͷ�
	static bool UpdateMemoryZipFile(char * pszZipStreamMem,int iSizeMem,char *& pszOutZipStreamMem,int &iOutSizeMem,
		const char * pszUpdtaeFileName,const char *pszNewContent,int iNewContenLen);
	// ��zip�ļ��ж�ȡĳ���ļ�������,���ض�ȡ���ݵ��ڴ��ַ�ͳ��ȣ���Ҫ�ⲿ�ͷ�
	static char * GetContentFromZip( std::string strZip, std::string strFile,int &iOutLen);
};

