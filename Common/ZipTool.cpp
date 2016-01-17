
#include "stdafx.h"
#include "ZipTool.h"
 #include <atlstr.h>
#include <vector>
#include <algorithm>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
//#define USE_ZLIB  //使用zlib来压缩文件或目录-->XZip不能添加文件到压缩包，因此用zlib实现
#ifdef USE_ZLIB
#define ZLIB_WINAPI
#include "zlib/contrib/minizip/zip.h"   //用zib库
#pragma comment(lib, "zlibstat.lib")
#endif // USE_ZLIB

#include "Tools.h"

#ifndef CPPTL_JSON_READER_H_INCLUDED
#include "reader.h"
#endif



ZipTool::ZipTool(void)
{
}


ZipTool::~ZipTool(void)
{
}

//文件路径是否存在
BOOL IsAccessibleDirZ(LPCTSTR strPath)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	BOOL bRet = FALSE;

	if (strlen((char *)strPath) == 3)
	{
		char *p = (char *)strPath;
		if (((*p > 'A' && *p < 'Z') || (*p > 'a' && *p < 'z'))
			&& *(p+1) == ':'
			&& *(p+2) == '\\')
			return TRUE;
		else
			return FALSE;
	}
	else if (strlen((char *)strPath) == 2) //结尾可能不含斜杠
	{
		char *p = (char *)strPath;
		if (((*p > 'A' && *p < 'Z') || (*p > 'a' && *p < 'z'))
			&& *(p+1) == ':')
			return TRUE;
		else
			return FALSE;
	}

	CString str = strPath;
	str.TrimRight('\\');
	hFind = FindFirstFile(str, &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (FILE_ATTRIBUTE_DIRECTORY == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
	{
		bRet = TRUE;
	}
	FindClose(hFind);
	return bRet;
}

void ListAllFileInDrectoryZ(std::vector<CString>& vPaths, LPCTSTR lpszFolderPath)
{
	CString strDir = lpszFolderPath == NULL ? "" : lpszFolderPath;
	CString strPath;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFile = NULL;
	TCHAR szFilePath[MAX_PATH] = {NULL};

	if (!IsAccessibleDirZ(strDir)) return;

	vPaths.push_back(strDir + '\\'); //添加该目录

	//构造代表子目录和文件夹路径的字符串，使用通配符“*”
	lstrcpy(szFilePath, (LPCTSTR)strDir);
	lstrcat(szFilePath, "\\*");
	//查找第一个文件/目录，获得查找句柄
	hFile = FindFirstFile(szFilePath, &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE) return;

	do
	{
		//    过滤“.”和“..”，不需要遍历
		if (lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 
			|| lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
		{
			continue;
		}
		//构造成全路径
		//wsprintf(szFullPath, "%s\\%s", szPath, FindFileData.cFileName);
		strPath.Format("%s\\%s", (LPCTSTR)strDir, FindFileData.cFileName);

		//如果是目录，则递归调用，列举下级目录
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ListAllFileInDrectoryZ(vPaths, (LPCTSTR)strPath);
		}
		else               //目录也添加
		{
			vPaths.push_back(strPath);
		}
	}
	while (FindNextFile(hFile, &FindFileData));

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		FindClose( hFile );
	}
	return;
}

// 取文件内容
std::string ZipTool::GetContentFromZip( std::string strZip, std::string strFile )
{
	std::string strR;  //返回串

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//打开失败
		return strR;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //替换斜杠
	strFile = ConvertToZipPath(strFile);

	FindZipItem(hz, strFile.c_str(), true, &index, &ze);

	if(index == -1)
	{//查找失败
		CloseZip(hz);
		return strR;
	}

	char *buf = new char[ze.unc_size + 1];
	buf[ze.unc_size] = 0;

	UnzipItem(hz, index, buf, ze.unc_size, ZIP_MEMORY);
	strR = buf;
	delete [] buf;
	CloseZip(hz);
 	return strR;
}

// 查找文件是否存在
bool ZipTool::FindFileInZip(std::string strZip, std::string strFile)
{
	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//打开失败
		return false;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //替换斜杠
	strFile = ConvertToZipPath(strFile);  //过滤路径-->过滤掉多余的斜杠
	FindZipItem(hz, strFile.c_str(), true, &index, &ze);
	if(index == -1)
	{//查找失败
		CloseZip(hz);
		return false;
	}
	else
	{
		CloseZip(hz);
		return true;
	}
}


// 解压一个文件到某处
bool ZipTool::UnzipFile( std::string strZip, std::string strFileSrc, std::string strFileDst )
{
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\'); //替换斜杠

	size_t n = strFileDst.rfind('\\');
	if(n == std::string::npos)
	{//路径异常
		return false;
	}
	std::string strDirName = strFileDst.substr(0, n + 1); //拆分路径名
	std::string strFileName = strFileDst.substr(n + 1);  //拆分文件名

	if(MakeSureDirectoryPathExists(strDirName.c_str()) == FALSE)
	{//目录创建失败
		return false;
	}

	char currentDir[255];
	GetCurrentDirectory(255, currentDir); //保存当前目录
	if(SetCurrentDirectory(strDirName.c_str()) == FALSE)
	{//切换工作目录失败
		return false;
	}

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//打开失败
		return false;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFileSrc.begin(), strFileSrc.end(), '\\', '/'); //替换斜杠
	strFileSrc = ConvertToZipPath(strFileSrc);  //过滤路径-->过滤掉多余的斜杠
	FindZipItem(hz, strFileSrc.c_str(), true, &index, &ze);
	if(index == -1)
	{//查找失败
		CloseZip(hz);
		return false;
	}

	ZRESULT zr = UnzipItem(hz, index,  (void*)strFileName.c_str(), 0, ZIP_FILENAME);
	if(zr != ZR_OK)
	{//解压文件失败
		SetCurrentDirectory(currentDir);
		CloseZip(hz);
		return false;
	}	

	CloseZip(hz);
	SetCurrentDirectory(currentDir);
	return true;
}


// 解压整个包到某文件夹
bool ZipTool::Unzip(std::string strZip, std::string strDir)
{
	if(strDir.back() != '\\' || strDir.back() != '/')
	{
		strDir.push_back('\\');
	}

	if(MakeSureDirectoryPathExists(strDir.c_str()) == FALSE)
	{//目录创建失败
		return false;
	}

	char currentDir[255];
	GetCurrentDirectory(255, currentDir); //保存当前目录
	if(SetCurrentDirectory(strDir.c_str()) == FALSE)
	{//切换工作目录失败
		return false;
	}

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//打开失败
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //元素数目

	for (int i=0; i<numitems; i++)
	{//遍历解压
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);
		UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);
	}

	CloseZip(hz);
	SetCurrentDirectory(currentDir);
	return true;
}


//解压包中的某个文件夹到指定目录-->同级
bool ZipTool::UnzipDir(std::string strZip, std::string strDir, std::string strDirDst)
{
	if(strDir == "")
	{//若搜寻条件为空则解压全部
		return Unzip(strZip, strDirDst);
	}

	std::replace(strDir.begin(), strDir.end(), '\\', '/'); //替换斜杠
	strDir = ConvertToZipPath(strDir); //去掉多余斜杠

	if(strDirDst.back() != '\\' && strDirDst.back() != '/')
	{//目标目录
		strDirDst.push_back('\\');
	}

	//return UnzipFile(strZip, strDir, strDirDst); //处理过程与解压文件到目标一致，直接调用
	if(strDir.back() != '\\' || strDir.back() != '/')
	{//搜寻目录
		strDir.push_back('/');
	}

	if(MakeSureDirectoryPathExists(strDirDst.c_str()) == FALSE)
	{//目录创建失败
		return false;
	}

	char currentDir[255];
	GetCurrentDirectory(255, currentDir); //保存当前目录
	if(SetCurrentDirectory(strDirDst.c_str()) == FALSE)
	{//切换工作目录失败
		return false;
	}

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//打开失败
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //元素数目

	bool bRet = false;
	for (int i=0; i<numitems; i++)
	{//遍历解压
		GetZipItem(hz, i, &ze);
		std::string strName(ze.name);
		if(strName.find(strDir) == 0)
		{//属于搜寻范围内则处理该元素
			bRet = true;  //设置成功标志
			strName = strName.substr(strDir.length()); //去除前面搜寻段
			int pos = strName.rfind('/');
			if(pos != std::string::npos)
			{//具有子目录则创建
				std::string dir = (strDirDst + strName.substr(0, pos + 1));
				std::replace(dir.begin(), dir.end(), '/', '\\');  //替换斜杠，否则不能生成目录
				if(MakeSureDirectoryPathExists(dir.c_str()) == FALSE)
				{//目录创建失败
					CloseZip(hz);
					SetCurrentDirectory(currentDir);
					return false;
				}
				if(pos != strName.length() - 1)
				{//是文件则解压，否则跳过
					std::string file = strName.substr(pos + 1);  //拆出文件名
					UnzipItem(hz, i, (void*)(dir + file).c_str(), 0, ZIP_FILENAME);
				}
			}
			else if(strName != "")
			{//是文件
				std::string dir = strDirDst;  //目录名
				std::string file = strName;  //文件名
				UnzipItem(hz, i, (void*)(dir + file).c_str(), 0, ZIP_FILENAME);
			}
			
		}
	}

	CloseZip(hz);
	SetCurrentDirectory(currentDir);
	return bRet;

}


// 将不规则路径转换为ZIP库识别的路径
std::string ZipTool::ConvertToZipPath(std::string path)
{
	return TrimPath(path, '/');
}

// 修整路径
std::string ZipTool::TrimPath(std::string path, char c)
{
	std::string::size_type pos = path.find_first_not_of(c); 

	if(pos != std::string::npos) 
	{ 
		path.erase(0, pos);
		pos = path.find_last_not_of(c); 
		if(pos != std::string::npos) path.erase(pos+1);  //左右去斜杠
	} 
	else 		
	{
		path.erase(path.begin(), path.end()); 
	}

	for(int i = 0; i < (int)path.length();)
	{
		if(path[i] == c && path[i+1] == c)
		{
			path.erase(i, 1);
		}
		else
		{
			i++;
		}
	}
	return path;
}

// 从指定目录创建ZIP文件，第四个参数指定压缩文件是否包含该目录，默认包含
bool ZipTool::ZipFromDir( std::string strZip, std::string strDir, int method /*= DEFLATE*/, bool isContainDir /*= false*/ )
{
	return Zip(strZip, "", strDir,  method, isContainDir);  //调用重载
}

//string转json
bool  ZipTool::StringToJson(std::string str, Json::Value& jv)
{
	Json::Reader reader;

	if (!reader.parse(str, jv, false))
	{//转换失败
		return false;
	}
	return true;
}

//从Json字符串创建ZIP
bool ZipTool::ZipFromJsonString( std::string strZip, std::string jsonString, int method /*= DEFLATE*/ )
{
	Json::Value json_object;
	if(StringToJson(jsonString, json_object) == false || json_object.type() != Json::arrayValue)
	{//string转换json对象失败
		ZTools::WriteZToolsFormatLog("JsonString转换失败或者不是arrayValue类型 :%s", jsonString.c_str());
		return false;
	}
	return ZipFromJson(strZip, json_object, method);
}

#ifndef USE_ZLIB
//从Json对象创建ZIP
bool ZipTool::ZipFromJson( std::string strZip, Json::Value& root, int method /*= DEFLATE*/ )
{
	bool bRet = true;

	HZIP hz = CreateZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//创建失败
		ZTools::WriteZToolsFormatLog("创建ZIP文件失败 :%s", strZip.c_str());
		return false;
	}

	for(Json::Value::iterator i = root.begin(); i != root.end(); i++)
	{
		std::string strSrc = (*i)["src"].asString();
		std::string strDst = (*i)["dst"].asString();
		if(!AddPathToHZIP(hz, strDst, strSrc, method))
		{//添加出现异常
			ZTools::WriteZToolsFormatLog("添加文件或目录发生错误 :%s---->%s", strSrc.c_str(), strDst.c_str());
			bRet = false;
		}
	}

	CloseZip(hz);
	return bRet;
}

//从指定路径添加文件或目录到HZIP
bool ZipTool::AddPathToHZIP( HZIP hz, std::string strDst, std::string strSrc, int method /*= DEFLATE*/, bool isContainDir /*= false*/ )
{
	bool bRet = true;

	std::replace(strSrc.begin(), strSrc.end(), '/', '\\');
	strSrc = TrimPath(strSrc, '\\');

	std::replace(strDst.begin(), strDst.end(), '/', '\\');
	strDst = TrimPath(strDst, '\\');  //替换处理斜杠

	//源文件或目录是否存在
	if (!PathFileExists(strSrc.c_str()))
	{
		return false;
	}

	BOOL isDir = IsAccessibleDirZ(strSrc.c_str()); //验证是目录

	if(isDir == false)
	{//是文件直接添加
		ZRESULT zr = ZipAdd(hz, strDst.c_str(), (void*)strSrc.c_str(), 0, ZIP_FILENAME, method);
		if(zr != ZR_OK)
		{//添加失败
			bRet = false;
		}
	}
	else
	{//是目录则遍历所有元素添加到ZIP
		std::vector<CString> files;
		ListAllFileInDrectoryZ(files, strSrc.c_str()); //获取所有文件和文件夹-->包括子级
		strSrc.append(1, '\\');
		if(!strDst.empty()) strDst.append(1, '\\'); //添加斜杠
		std::string strRoot;   //根目录
		if(isContainDir == false)
		{//设置为空
			strRoot = "";
		}
		else if(strSrc.length() == 3)
		{//只有磁盘
			strRoot = strSrc.front() + '\\';
		}
		else
		{//包含该目录
			int pos = strSrc.rfind('\\');
			int pos1 = strSrc.rfind('\\', pos - 1);
			strRoot = strSrc.substr(pos1 + 1, pos - pos1 - 1) + '\\'; //获取最后目录
		}

		for(int i = 0; i < (int)files.size(); i++)
		{
			ZRESULT zr;
			std::string strFile(files[i]);
			std::string strPath = strDst + strRoot + strFile.substr(strSrc.length());
			if(strPath.empty()) continue;
			if(strPath.back() == '\\')
			{//添加目录
				zr = ZipAdd(hz, strPath.c_str(), 0, 0, ZIP_FOLDER, method);
			}
			else
			{//添加文件
				zr = ZipAdd(hz, strPath.c_str(), (void*)strFile.c_str(), 0, ZIP_FILENAME, method);
			}
			if(zr != ZR_OK)
			{//添加失败
				bRet = false;
			}
		}

	}
	return bRet;
}

//从指定路径添加文件或目录到目标zip的路径
bool ZipTool::Zip( std::string strZip, std::string strDst, std::string strSrc, int method /*= DEFLATE*/, bool isContainDir /*= false*/ )
{
	HZIP hz = CreateZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//创建失败
		return false;
	}

	bool bRet = AddPathToHZIP(hz, strDst, strSrc, method, isContainDir ); //调用add方法
	CloseZip(hz);
	return bRet;
}

#else

//增加文件到zip
void AddFileToZip(zipFile zf, const char* fileNameInZip, const char* srcFile) 
{ 
	FILE* srcfp = NULL; 

	//初始化写入zip的文件信息 
	zip_fileinfo zi; 
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0; 
	zi.dosDate = 0; 
	zi.internal_fa = 0; 
	zi.external_fa = 0; 
	//如果srcFile为空，加入空目录 
	char new_file_name[MAX_PATH]; 
	memset(new_file_name, 0, sizeof(new_file_name)); 
	strcat(new_file_name, fileNameInZip); 
	if (srcFile == NULL) 
	{ 
		strcat(new_file_name, "/"); 
	} 

	//在zip文件中创建新文件 
	zipOpenNewFileInZip(zf, new_file_name, &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION); 
	if (srcFile != NULL) 
	{ 
		//打开源文件 
		srcfp = fopen(srcFile, "rb"); 
		if (srcfp == NULL) 
		{ 
			ZTools::WriteZToolsFormatLog("无法添加文件 :%s", CString(srcFile));
			zipCloseFileInZip(zf); //关闭zip文件 
			return; 
		} 

		//读入源文件并写入zip文件 
		char buf[100*1024]; //buffer 
		int numBytes = 0; 
		while( !feof(srcfp) ) 
		{ 
			numBytes = fread(buf, 1, sizeof(buf), srcfp); 
			zipWriteInFileInZip(zf, buf, numBytes); 
			if( ferror(srcfp) ) 
				break; 
		}  
		//关闭源文件 
		fclose(srcfp); 
	} 

	//关闭zip文件 
	zipCloseFileInZip(zf); 
} 

//从指定路径添加文件或目录到目标zip的路径，存在则添加，若不存在则新建
bool ZipTool::Zip(std::string strZip, std::string strDst, std::string strSrc, bool isContainDir)
{
	{
		zipFile newZipFile = zipOpen(strZip.c_str(), APPEND_STATUS_ADDINZIP); //创建zip文件 

		AddFileToZip(newZipFile, strDst.c_str(), strSrc.c_str());

		zipClose(newZipFile, NULL); //关闭zip文件 

		return true;
	}
	////////////////////////////////未完成////////////////////////////////////

}
#endif // USE_ZLIB

time_t FileTimeToTimeT(FILETIME  ft)
{
	SYSTEMTIME st; FileTimeToSystemTime(&ft,&st);
	if (st.wYear<1970) {st.wYear=1970; st.wMonth=1; st.wDay=1;}
	if (st.wYear>=2038) {st.wYear=2037; st.wMonth=12; st.wDay=31;}
	struct tm tm;
	tm.tm_sec = st.wSecond;
	tm.tm_min = st.wMinute;
	tm.tm_hour = st.wHour;
	tm.tm_mday = st.wDay;
	tm.tm_mon = st.wMonth-1;
	tm.tm_year = st.wYear-1900;
	tm.tm_isdst = 0;
	time_t t = mktime(&tm);
	return t;
}

bool ZipTool::ReplaceZipFile(const std::string & strZip,const std::string & strOutZip,const std::string & strFileName,const std::string & strNewContent,int method)
{
	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{
		ZTools::WriteZToolsFormatLog("打开zip文件【%s】失败！",strZip.c_str());
		return false;
	}

	//创建输出zip文件
	HZIP hzOut = CreateZip((void*)strOutZip.c_str(), 0, ZIP_FILENAME);
	if(hzOut == NULL)
	{//创建失败
		CloseZip(hz);
		ZTools::WriteZToolsFormatLog("创建输出zip文件【%s】失败！",strOutZip.c_str());
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //元素数目

	UseOriginalFileTime = false;
	for (int i=0; i<numitems; i++)
	{//遍历解压
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);

		UseOriginalFileTime = true;
		//解压文件到内存块
		char *pFileContentbuf = new char[ze.unc_size + 1];
		pFileContentbuf[ze.unc_size] = 0;
		UnzipItem(hz, i, pFileContentbuf, ze.unc_size, ZIP_MEMORY);
		if (strFileName == ze.name)
		{
			//将更新的文件根据更新后的内存块添加到输出zip文件
			UseOriginalFileTime = false;
			ZRESULT zr = ZipAdd(hzOut,ze.name,(void*)strNewContent.c_str(),strNewContent.length(), ZIP_MEMORY, 8);
			delete [] pFileContentbuf;
			continue;
		}
		atime_memfile = FileTimeToTimeT(ze.atime);
		mtime_memfile = FileTimeToTimeT(ze.mtime);
		ctime_memfile = FileTimeToTimeT(ze.ctime);
		FileTimeToLocalFileTime(&(ze.mtime),&ftm);

		//将未更新的文件根据内存块添加到输出zip文件
		ZRESULT zr = ZipAdd(hzOut,ze.name,pFileContentbuf, ze.unc_size, ZIP_MEMORY, 8);
		delete [] pFileContentbuf;
	}

	CloseZip(hz);
	CloseZip(hzOut);
	return true;
}
bool ZipTool::ReplaceZipFile(const std::string & strZip,const std::string & strOutZip,std::vector<UpdateFileObj> vModifyFile,int method)
{
	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{
		ZTools::WriteZToolsFormatLog("打开zip文件【%s】失败！",strZip.c_str());
		return false;
	}

	
	//创建输出zip文件
	HZIP hzOut = CreateZip((void*)strOutZip.c_str(), 0, ZIP_FILENAME);
	if(hzOut == NULL)
	{//创建失败
		CloseZip(hz);
		ZTools::WriteZToolsFormatLog("创建输出zip文件【%s】失败！",strOutZip.c_str());
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //元素数目

	UseOriginalFileTime = false;
	for (int i=0; i<numitems; i++)
	{//遍历解压
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);

		UseOriginalFileTime = true;
		//解压文件到内存块
		char *pFileContentbuf = new char[ze.unc_size + 1];
		pFileContentbuf[ze.unc_size] = 0;
		UnzipItem(hz, i, pFileContentbuf, ze.unc_size, ZIP_MEMORY);

		bool bReplaced = false;
		vector<UpdateFileObj>::iterator iter = vModifyFile.begin();
		for (; iter !=vModifyFile.end(); iter++)
		{
			if (iter->m_strFileName == ze.name)
			{
				//将更新的文件根据更新后的内存块添加到zip文件
				UseOriginalFileTime = false;
				ZRESULT zr = ZipAdd(hzOut,iter->m_strNewName.c_str(),(void*)iter->m_pszFileContent,
					iter->m_iFileLen, ZIP_MEMORY, 8);

				delete [] pFileContentbuf;
				bReplaced = true;
				iter = vModifyFile.erase(iter);
				break;
			}
		}
		if (bReplaced)
			continue;
		
		//将未更新的文件根据内存块添加到临时zip文件
		atime_memfile = FileTimeToTimeT(ze.atime);
		mtime_memfile = FileTimeToTimeT(ze.mtime);
		ctime_memfile = FileTimeToTimeT(ze.ctime);
		FileTimeToLocalFileTime(&(ze.mtime),&ftm);

		ZRESULT zr = ZipAdd(hzOut,ze.name,pFileContentbuf, ze.unc_size, ZIP_MEMORY, method);
		delete [] pFileContentbuf;
	}

	CloseZip(hz);
	CloseZip(hzOut);
	return true;
}

char * ZipTool::GetContentFormMemZip(char * pszZipStreamMem,int iSizeMem,const char * pszFetchFileName,int &iOutLen)
{
	iOutLen = 0;
	HZIP hz = OpenZip((void*)pszZipStreamMem, iSizeMem, ZIP_MEMORY);

	if(hz == NULL)
	{//打开失败
		return NULL;
	}

	string strFile = pszFetchFileName;
	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //替换斜杠
	strFile = ConvertToZipPath(strFile);

	FindZipItem(hz, strFile.c_str(), true, &index, &ze);
	if(index == -1)
	{//查找失败
		CloseZip(hz);
		return NULL;
	}

	char *buf = new char[ze.unc_size + 1];
	buf[ze.unc_size] = 0;
	iOutLen = ze.unc_size + 1;
	UnzipItem(hz, index, buf, ze.unc_size, ZIP_MEMORY);
	CloseZip(hz);
	return buf;
}
// 取文件内容，该函数申请的内存需要在外部释放
char * ZipTool::GetContentFromZip( std::string strZip, std::string strFile,int &iOutLen)
{
	iOutLen = 0;
	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//打开失败
		return NULL;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //替换斜杠
	strFile = ConvertToZipPath(strFile);

	FindZipItem(hz, strFile.c_str(), true, &index, &ze);

	if(index == -1)
	{//查找失败
		CloseZip(hz);
		return NULL;
	}

	char *buf = new char[ze.unc_size + 1];
	buf[ze.unc_size] = 0;
	iOutLen = ze.unc_size + 1;
	UnzipItem(hz, index, buf, ze.unc_size, ZIP_MEMORY);
	CloseZip(hz);
	return buf;
}
bool ZipTool::UpdateMemoryZipFile(char * pszZipStreamMem,int iSizeMem,char *& pszOutZipStreamMem,int &iOutSizeMem,
	const char * pszUpdtaeFileName,const char *pszNewContent,int iNewContenLen)
{
	HZIP hz = OpenZip((void*)pszZipStreamMem, iSizeMem, ZIP_MEMORY);
	if(hz == NULL)
	{//打开失败
		return false;
	}

	//创建输出zip文件
	pszOutZipStreamMem = new char[iSizeMem*5];
	iOutSizeMem = iSizeMem*5;
	HZIP hzOut = CreateZip(pszOutZipStreamMem,iOutSizeMem, ZIP_MEMORY);
	if(hzOut == NULL)
	{//创建失败
		CloseZip(hz);
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //元素数目

	UseOriginalFileTime = false;
	for (int i=0; i<numitems; i++)
	{//遍历解压
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);

		UseOriginalFileTime = true;
		//解压文件到内存块
		char *pFileContentbuf = new char[ze.unc_size + 1];
		pFileContentbuf[ze.unc_size] = 0;
		UnzipItem(hz, i, pFileContentbuf, ze.unc_size, ZIP_MEMORY);
		if (stricmp(pszUpdtaeFileName,ze.name)==0)
		{
			//将更新的文件根据更新后的内存块添加到内存zip文件
			UseOriginalFileTime = false;
			ZRESULT zr = ZipAdd(hzOut,ze.name,(void*)pszNewContent,iNewContenLen, ZIP_MEMORY, 8);
			delete [] pFileContentbuf;
			continue;
		}
		atime_memfile = FileTimeToTimeT(ze.atime);
		mtime_memfile = FileTimeToTimeT(ze.mtime);
		ctime_memfile = FileTimeToTimeT(ze.ctime);
		FileTimeToLocalFileTime(&(ze.mtime),&ftm);

		//将未更新的文件根据内存块添加到输出zip文件
		ZRESULT zr = ZipAdd(hzOut,ze.name,pFileContentbuf, ze.unc_size, ZIP_MEMORY, 8);
		delete [] pFileContentbuf;
	}

	CloseZip(hz);
	char *pData = NULL;
	unsigned long iDataLen = 0;
	ZipGetMemory(hzOut,(void**)(&pData),&iDataLen);
	pszOutZipStreamMem = pData;
	iOutSizeMem = iDataLen;
	return true;
}