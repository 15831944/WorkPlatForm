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
	std::string m_strFileName; // 要替换的zip中的文件名
	std::string m_strNewName; // 替换后的文件名称
	char * m_pszFileContent; // 替换后的新文件的字节流
	int  m_iFileLen;       // 替换后的新文件的字节流长度
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
	// 查找文件是否存在
	static bool FindFileInZip(std::string strZip, std::string strFile);
	// 取文件内容
	static std::string GetContentFromZip(std::string strZip, std::string strFile);
	// 解压一个文件到某处
	static bool UnzipFile(std::string strZip, std::string strFileSrc, std::string strFileDst);
	// 解压整个包到某文件夹
	static bool Unzip(std::string strZip, std::string strDir);
	// //解压包中的某个文件到指定目录
	static bool UnzipDir(std::string strZip, std::string strDir, std::string strDirDst);
	// 将不规则路径转换为ZIP库识别的路径
	static std::string ConvertToZipPath(std::string path);
	// 从指定目录创建ZIP文件，第三个参数指定压缩文件是否包含该目录，默认包含
	static bool ZipFromDir(std::string strZip, std::string strDir, int method = DEFLATE, bool isContainDir = false);

	static bool ZipFromJson(std::string strZip, Json::Value& root, int method = DEFLATE);  //从Json创建zip

	static bool AddPathToHZIP(HZIP hz, std::string strDst, std::string strSrc, int method = DEFLATE, bool isContainDir = false);
	//从指定路径添加文件或目录到目标zip的路径，存在则添加，若不存在则新建
	static bool Zip(std::string strZip, std::string strDst, std::string strSrc, int method = DEFLATE, bool isContainDir = false);
	//处理路径
	static std::string TrimPath(std::string path, char c);

	static bool StringToJson(std::string str, Json::Value& jv);

	static bool ZipFromJsonString(std::string strZip, std::string jsonString, int method = DEFLATE);
	// 替换zip中的单个文件
	static bool ReplaceZipFile(const std::string & strZip,const std::string & strOutZip,const std::string & strFileName,const std::string & strNewContent,int method = DEFLATE);
	// 替换zip中的多个文件
	static bool ReplaceZipFile(const std::string & strZip,const std::string & strOutZip,std::vector<UpdateFileObj> vModifyFile,int method = DEFLATE);

	// 从一个内存zip文件中读取某个文件的内容，返回读取内容的内存地址和长度，需要外部释放
	static char* GetContentFormMemZip(char * pszZipStreamMem,int iSizeMem,const char * pszFetchFileName,int &iOutLen);

	// 更新一个内存zip文件中的某个文件，得到一个新的内存zip文件，新内存文件的内存需要在外部释放
	static bool UpdateMemoryZipFile(char * pszZipStreamMem,int iSizeMem,char *& pszOutZipStreamMem,int &iOutSizeMem,
		const char * pszUpdtaeFileName,const char *pszNewContent,int iNewContenLen);
	// 从zip文件中读取某个文件的内容,返回读取内容的内存地址和长度，需要外部释放
	static char * GetContentFromZip( std::string strZip, std::string strFile,int &iOutLen);
};

