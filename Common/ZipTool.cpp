
#include "stdafx.h"
#include "ZipTool.h"
 #include <atlstr.h>
#include <vector>
#include <algorithm>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
//#define USE_ZLIB  //ʹ��zlib��ѹ���ļ���Ŀ¼-->XZip��������ļ���ѹ�����������zlibʵ��
#ifdef USE_ZLIB
#define ZLIB_WINAPI
#include "zlib/contrib/minizip/zip.h"   //��zib��
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

//�ļ�·���Ƿ����
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
	else if (strlen((char *)strPath) == 2) //��β���ܲ���б��
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

	vPaths.push_back(strDir + '\\'); //��Ӹ�Ŀ¼

	//���������Ŀ¼���ļ���·�����ַ�����ʹ��ͨ�����*��
	lstrcpy(szFilePath, (LPCTSTR)strDir);
	lstrcat(szFilePath, "\\*");
	//���ҵ�һ���ļ�/Ŀ¼����ò��Ҿ��
	hFile = FindFirstFile(szFilePath, &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE) return;

	do
	{
		//    ���ˡ�.���͡�..��������Ҫ����
		if (lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 
			|| lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
		{
			continue;
		}
		//�����ȫ·��
		//wsprintf(szFullPath, "%s\\%s", szPath, FindFileData.cFileName);
		strPath.Format("%s\\%s", (LPCTSTR)strDir, FindFileData.cFileName);

		//�����Ŀ¼����ݹ���ã��о��¼�Ŀ¼
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ListAllFileInDrectoryZ(vPaths, (LPCTSTR)strPath);
		}
		else               //Ŀ¼Ҳ���
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

// ȡ�ļ�����
std::string ZipTool::GetContentFromZip( std::string strZip, std::string strFile )
{
	std::string strR;  //���ش�

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//��ʧ��
		return strR;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //�滻б��
	strFile = ConvertToZipPath(strFile);

	FindZipItem(hz, strFile.c_str(), true, &index, &ze);

	if(index == -1)
	{//����ʧ��
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

// �����ļ��Ƿ����
bool ZipTool::FindFileInZip(std::string strZip, std::string strFile)
{
	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//��ʧ��
		return false;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //�滻б��
	strFile = ConvertToZipPath(strFile);  //����·��-->���˵������б��
	FindZipItem(hz, strFile.c_str(), true, &index, &ze);
	if(index == -1)
	{//����ʧ��
		CloseZip(hz);
		return false;
	}
	else
	{
		CloseZip(hz);
		return true;
	}
}


// ��ѹһ���ļ���ĳ��
bool ZipTool::UnzipFile( std::string strZip, std::string strFileSrc, std::string strFileDst )
{
	std::replace(strFileDst.begin(), strFileDst.end(), '/', '\\'); //�滻б��

	size_t n = strFileDst.rfind('\\');
	if(n == std::string::npos)
	{//·���쳣
		return false;
	}
	std::string strDirName = strFileDst.substr(0, n + 1); //���·����
	std::string strFileName = strFileDst.substr(n + 1);  //����ļ���

	if(MakeSureDirectoryPathExists(strDirName.c_str()) == FALSE)
	{//Ŀ¼����ʧ��
		return false;
	}

	char currentDir[255];
	GetCurrentDirectory(255, currentDir); //���浱ǰĿ¼
	if(SetCurrentDirectory(strDirName.c_str()) == FALSE)
	{//�л�����Ŀ¼ʧ��
		return false;
	}

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//��ʧ��
		return false;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFileSrc.begin(), strFileSrc.end(), '\\', '/'); //�滻б��
	strFileSrc = ConvertToZipPath(strFileSrc);  //����·��-->���˵������б��
	FindZipItem(hz, strFileSrc.c_str(), true, &index, &ze);
	if(index == -1)
	{//����ʧ��
		CloseZip(hz);
		return false;
	}

	ZRESULT zr = UnzipItem(hz, index,  (void*)strFileName.c_str(), 0, ZIP_FILENAME);
	if(zr != ZR_OK)
	{//��ѹ�ļ�ʧ��
		SetCurrentDirectory(currentDir);
		CloseZip(hz);
		return false;
	}	

	CloseZip(hz);
	SetCurrentDirectory(currentDir);
	return true;
}


// ��ѹ��������ĳ�ļ���
bool ZipTool::Unzip(std::string strZip, std::string strDir)
{
	if(strDir.back() != '\\' || strDir.back() != '/')
	{
		strDir.push_back('\\');
	}

	if(MakeSureDirectoryPathExists(strDir.c_str()) == FALSE)
	{//Ŀ¼����ʧ��
		return false;
	}

	char currentDir[255];
	GetCurrentDirectory(255, currentDir); //���浱ǰĿ¼
	if(SetCurrentDirectory(strDir.c_str()) == FALSE)
	{//�л�����Ŀ¼ʧ��
		return false;
	}

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//��ʧ��
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //Ԫ����Ŀ

	for (int i=0; i<numitems; i++)
	{//������ѹ
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);
		UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);
	}

	CloseZip(hz);
	SetCurrentDirectory(currentDir);
	return true;
}


//��ѹ���е�ĳ���ļ��е�ָ��Ŀ¼-->ͬ��
bool ZipTool::UnzipDir(std::string strZip, std::string strDir, std::string strDirDst)
{
	if(strDir == "")
	{//����Ѱ����Ϊ�����ѹȫ��
		return Unzip(strZip, strDirDst);
	}

	std::replace(strDir.begin(), strDir.end(), '\\', '/'); //�滻б��
	strDir = ConvertToZipPath(strDir); //ȥ������б��

	if(strDirDst.back() != '\\' && strDirDst.back() != '/')
	{//Ŀ��Ŀ¼
		strDirDst.push_back('\\');
	}

	//return UnzipFile(strZip, strDir, strDirDst); //����������ѹ�ļ���Ŀ��һ�£�ֱ�ӵ���
	if(strDir.back() != '\\' || strDir.back() != '/')
	{//��ѰĿ¼
		strDir.push_back('/');
	}

	if(MakeSureDirectoryPathExists(strDirDst.c_str()) == FALSE)
	{//Ŀ¼����ʧ��
		return false;
	}

	char currentDir[255];
	GetCurrentDirectory(255, currentDir); //���浱ǰĿ¼
	if(SetCurrentDirectory(strDirDst.c_str()) == FALSE)
	{//�л�����Ŀ¼ʧ��
		return false;
	}

	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//��ʧ��
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //Ԫ����Ŀ

	bool bRet = false;
	for (int i=0; i<numitems; i++)
	{//������ѹ
		GetZipItem(hz, i, &ze);
		std::string strName(ze.name);
		if(strName.find(strDir) == 0)
		{//������Ѱ��Χ�������Ԫ��
			bRet = true;  //���óɹ���־
			strName = strName.substr(strDir.length()); //ȥ��ǰ����Ѱ��
			int pos = strName.rfind('/');
			if(pos != std::string::npos)
			{//������Ŀ¼�򴴽�
				std::string dir = (strDirDst + strName.substr(0, pos + 1));
				std::replace(dir.begin(), dir.end(), '/', '\\');  //�滻б�ܣ�����������Ŀ¼
				if(MakeSureDirectoryPathExists(dir.c_str()) == FALSE)
				{//Ŀ¼����ʧ��
					CloseZip(hz);
					SetCurrentDirectory(currentDir);
					return false;
				}
				if(pos != strName.length() - 1)
				{//���ļ����ѹ����������
					std::string file = strName.substr(pos + 1);  //����ļ���
					UnzipItem(hz, i, (void*)(dir + file).c_str(), 0, ZIP_FILENAME);
				}
			}
			else if(strName != "")
			{//���ļ�
				std::string dir = strDirDst;  //Ŀ¼��
				std::string file = strName;  //�ļ���
				UnzipItem(hz, i, (void*)(dir + file).c_str(), 0, ZIP_FILENAME);
			}
			
		}
	}

	CloseZip(hz);
	SetCurrentDirectory(currentDir);
	return bRet;

}


// ��������·��ת��ΪZIP��ʶ���·��
std::string ZipTool::ConvertToZipPath(std::string path)
{
	return TrimPath(path, '/');
}

// ����·��
std::string ZipTool::TrimPath(std::string path, char c)
{
	std::string::size_type pos = path.find_first_not_of(c); 

	if(pos != std::string::npos) 
	{ 
		path.erase(0, pos);
		pos = path.find_last_not_of(c); 
		if(pos != std::string::npos) path.erase(pos+1);  //����ȥб��
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

// ��ָ��Ŀ¼����ZIP�ļ������ĸ�����ָ��ѹ���ļ��Ƿ������Ŀ¼��Ĭ�ϰ���
bool ZipTool::ZipFromDir( std::string strZip, std::string strDir, int method /*= DEFLATE*/, bool isContainDir /*= false*/ )
{
	return Zip(strZip, "", strDir,  method, isContainDir);  //��������
}

//stringתjson
bool  ZipTool::StringToJson(std::string str, Json::Value& jv)
{
	Json::Reader reader;

	if (!reader.parse(str, jv, false))
	{//ת��ʧ��
		return false;
	}
	return true;
}

//��Json�ַ�������ZIP
bool ZipTool::ZipFromJsonString( std::string strZip, std::string jsonString, int method /*= DEFLATE*/ )
{
	Json::Value json_object;
	if(StringToJson(jsonString, json_object) == false || json_object.type() != Json::arrayValue)
	{//stringת��json����ʧ��
		ZTools::WriteZToolsFormatLog("JsonStringת��ʧ�ܻ��߲���arrayValue���� :%s", jsonString.c_str());
		return false;
	}
	return ZipFromJson(strZip, json_object, method);
}

#ifndef USE_ZLIB
//��Json���󴴽�ZIP
bool ZipTool::ZipFromJson( std::string strZip, Json::Value& root, int method /*= DEFLATE*/ )
{
	bool bRet = true;

	HZIP hz = CreateZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//����ʧ��
		ZTools::WriteZToolsFormatLog("����ZIP�ļ�ʧ�� :%s", strZip.c_str());
		return false;
	}

	for(Json::Value::iterator i = root.begin(); i != root.end(); i++)
	{
		std::string strSrc = (*i)["src"].asString();
		std::string strDst = (*i)["dst"].asString();
		if(!AddPathToHZIP(hz, strDst, strSrc, method))
		{//��ӳ����쳣
			ZTools::WriteZToolsFormatLog("����ļ���Ŀ¼�������� :%s---->%s", strSrc.c_str(), strDst.c_str());
			bRet = false;
		}
	}

	CloseZip(hz);
	return bRet;
}

//��ָ��·������ļ���Ŀ¼��HZIP
bool ZipTool::AddPathToHZIP( HZIP hz, std::string strDst, std::string strSrc, int method /*= DEFLATE*/, bool isContainDir /*= false*/ )
{
	bool bRet = true;

	std::replace(strSrc.begin(), strSrc.end(), '/', '\\');
	strSrc = TrimPath(strSrc, '\\');

	std::replace(strDst.begin(), strDst.end(), '/', '\\');
	strDst = TrimPath(strDst, '\\');  //�滻����б��

	//Դ�ļ���Ŀ¼�Ƿ����
	if (!PathFileExists(strSrc.c_str()))
	{
		return false;
	}

	BOOL isDir = IsAccessibleDirZ(strSrc.c_str()); //��֤��Ŀ¼

	if(isDir == false)
	{//���ļ�ֱ�����
		ZRESULT zr = ZipAdd(hz, strDst.c_str(), (void*)strSrc.c_str(), 0, ZIP_FILENAME, method);
		if(zr != ZR_OK)
		{//���ʧ��
			bRet = false;
		}
	}
	else
	{//��Ŀ¼���������Ԫ����ӵ�ZIP
		std::vector<CString> files;
		ListAllFileInDrectoryZ(files, strSrc.c_str()); //��ȡ�����ļ����ļ���-->�����Ӽ�
		strSrc.append(1, '\\');
		if(!strDst.empty()) strDst.append(1, '\\'); //���б��
		std::string strRoot;   //��Ŀ¼
		if(isContainDir == false)
		{//����Ϊ��
			strRoot = "";
		}
		else if(strSrc.length() == 3)
		{//ֻ�д���
			strRoot = strSrc.front() + '\\';
		}
		else
		{//������Ŀ¼
			int pos = strSrc.rfind('\\');
			int pos1 = strSrc.rfind('\\', pos - 1);
			strRoot = strSrc.substr(pos1 + 1, pos - pos1 - 1) + '\\'; //��ȡ���Ŀ¼
		}

		for(int i = 0; i < (int)files.size(); i++)
		{
			ZRESULT zr;
			std::string strFile(files[i]);
			std::string strPath = strDst + strRoot + strFile.substr(strSrc.length());
			if(strPath.empty()) continue;
			if(strPath.back() == '\\')
			{//���Ŀ¼
				zr = ZipAdd(hz, strPath.c_str(), 0, 0, ZIP_FOLDER, method);
			}
			else
			{//����ļ�
				zr = ZipAdd(hz, strPath.c_str(), (void*)strFile.c_str(), 0, ZIP_FILENAME, method);
			}
			if(zr != ZR_OK)
			{//���ʧ��
				bRet = false;
			}
		}

	}
	return bRet;
}

//��ָ��·������ļ���Ŀ¼��Ŀ��zip��·��
bool ZipTool::Zip( std::string strZip, std::string strDst, std::string strSrc, int method /*= DEFLATE*/, bool isContainDir /*= false*/ )
{
	HZIP hz = CreateZip((void*)strZip.c_str(), 0, ZIP_FILENAME);
	if(hz == NULL)
	{//����ʧ��
		return false;
	}

	bool bRet = AddPathToHZIP(hz, strDst, strSrc, method, isContainDir ); //����add����
	CloseZip(hz);
	return bRet;
}

#else

//�����ļ���zip
void AddFileToZip(zipFile zf, const char* fileNameInZip, const char* srcFile) 
{ 
	FILE* srcfp = NULL; 

	//��ʼ��д��zip���ļ���Ϣ 
	zip_fileinfo zi; 
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0; 
	zi.dosDate = 0; 
	zi.internal_fa = 0; 
	zi.external_fa = 0; 
	//���srcFileΪ�գ������Ŀ¼ 
	char new_file_name[MAX_PATH]; 
	memset(new_file_name, 0, sizeof(new_file_name)); 
	strcat(new_file_name, fileNameInZip); 
	if (srcFile == NULL) 
	{ 
		strcat(new_file_name, "/"); 
	} 

	//��zip�ļ��д������ļ� 
	zipOpenNewFileInZip(zf, new_file_name, &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION); 
	if (srcFile != NULL) 
	{ 
		//��Դ�ļ� 
		srcfp = fopen(srcFile, "rb"); 
		if (srcfp == NULL) 
		{ 
			ZTools::WriteZToolsFormatLog("�޷�����ļ� :%s", CString(srcFile));
			zipCloseFileInZip(zf); //�ر�zip�ļ� 
			return; 
		} 

		//����Դ�ļ���д��zip�ļ� 
		char buf[100*1024]; //buffer 
		int numBytes = 0; 
		while( !feof(srcfp) ) 
		{ 
			numBytes = fread(buf, 1, sizeof(buf), srcfp); 
			zipWriteInFileInZip(zf, buf, numBytes); 
			if( ferror(srcfp) ) 
				break; 
		}  
		//�ر�Դ�ļ� 
		fclose(srcfp); 
	} 

	//�ر�zip�ļ� 
	zipCloseFileInZip(zf); 
} 

//��ָ��·������ļ���Ŀ¼��Ŀ��zip��·������������ӣ������������½�
bool ZipTool::Zip(std::string strZip, std::string strDst, std::string strSrc, bool isContainDir)
{
	{
		zipFile newZipFile = zipOpen(strZip.c_str(), APPEND_STATUS_ADDINZIP); //����zip�ļ� 

		AddFileToZip(newZipFile, strDst.c_str(), strSrc.c_str());

		zipClose(newZipFile, NULL); //�ر�zip�ļ� 

		return true;
	}
	////////////////////////////////δ���////////////////////////////////////

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
		ZTools::WriteZToolsFormatLog("��zip�ļ���%s��ʧ�ܣ�",strZip.c_str());
		return false;
	}

	//�������zip�ļ�
	HZIP hzOut = CreateZip((void*)strOutZip.c_str(), 0, ZIP_FILENAME);
	if(hzOut == NULL)
	{//����ʧ��
		CloseZip(hz);
		ZTools::WriteZToolsFormatLog("�������zip�ļ���%s��ʧ�ܣ�",strOutZip.c_str());
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //Ԫ����Ŀ

	UseOriginalFileTime = false;
	for (int i=0; i<numitems; i++)
	{//������ѹ
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);

		UseOriginalFileTime = true;
		//��ѹ�ļ����ڴ��
		char *pFileContentbuf = new char[ze.unc_size + 1];
		pFileContentbuf[ze.unc_size] = 0;
		UnzipItem(hz, i, pFileContentbuf, ze.unc_size, ZIP_MEMORY);
		if (strFileName == ze.name)
		{
			//�����µ��ļ����ݸ��º���ڴ����ӵ����zip�ļ�
			UseOriginalFileTime = false;
			ZRESULT zr = ZipAdd(hzOut,ze.name,(void*)strNewContent.c_str(),strNewContent.length(), ZIP_MEMORY, 8);
			delete [] pFileContentbuf;
			continue;
		}
		atime_memfile = FileTimeToTimeT(ze.atime);
		mtime_memfile = FileTimeToTimeT(ze.mtime);
		ctime_memfile = FileTimeToTimeT(ze.ctime);
		FileTimeToLocalFileTime(&(ze.mtime),&ftm);

		//��δ���µ��ļ������ڴ����ӵ����zip�ļ�
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
		ZTools::WriteZToolsFormatLog("��zip�ļ���%s��ʧ�ܣ�",strZip.c_str());
		return false;
	}

	
	//�������zip�ļ�
	HZIP hzOut = CreateZip((void*)strOutZip.c_str(), 0, ZIP_FILENAME);
	if(hzOut == NULL)
	{//����ʧ��
		CloseZip(hz);
		ZTools::WriteZToolsFormatLog("�������zip�ļ���%s��ʧ�ܣ�",strOutZip.c_str());
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //Ԫ����Ŀ

	UseOriginalFileTime = false;
	for (int i=0; i<numitems; i++)
	{//������ѹ
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);

		UseOriginalFileTime = true;
		//��ѹ�ļ����ڴ��
		char *pFileContentbuf = new char[ze.unc_size + 1];
		pFileContentbuf[ze.unc_size] = 0;
		UnzipItem(hz, i, pFileContentbuf, ze.unc_size, ZIP_MEMORY);

		bool bReplaced = false;
		vector<UpdateFileObj>::iterator iter = vModifyFile.begin();
		for (; iter !=vModifyFile.end(); iter++)
		{
			if (iter->m_strFileName == ze.name)
			{
				//�����µ��ļ����ݸ��º���ڴ����ӵ�zip�ļ�
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
		
		//��δ���µ��ļ������ڴ����ӵ���ʱzip�ļ�
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
	{//��ʧ��
		return NULL;
	}

	string strFile = pszFetchFileName;
	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //�滻б��
	strFile = ConvertToZipPath(strFile);

	FindZipItem(hz, strFile.c_str(), true, &index, &ze);
	if(index == -1)
	{//����ʧ��
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
// ȡ�ļ����ݣ��ú���������ڴ���Ҫ���ⲿ�ͷ�
char * ZipTool::GetContentFromZip( std::string strZip, std::string strFile,int &iOutLen)
{
	iOutLen = 0;
	HZIP hz = OpenZip((void*)strZip.c_str(), 0, ZIP_FILENAME);

	if(hz == NULL)
	{//��ʧ��
		return NULL;
	}

	ZIPENTRY ze; 
	int index;
	std::replace(strFile.begin(), strFile.end(), '\\', '/'); //�滻б��
	strFile = ConvertToZipPath(strFile);

	FindZipItem(hz, strFile.c_str(), true, &index, &ze);

	if(index == -1)
	{//����ʧ��
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
	{//��ʧ��
		return false;
	}

	//�������zip�ļ�
	pszOutZipStreamMem = new char[iSizeMem*5];
	iOutSizeMem = iSizeMem*5;
	HZIP hzOut = CreateZip(pszOutZipStreamMem,iOutSizeMem, ZIP_MEMORY);
	if(hzOut == NULL)
	{//����ʧ��
		CloseZip(hz);
		return false;
	}

	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index; //Ԫ����Ŀ

	UseOriginalFileTime = false;
	for (int i=0; i<numitems; i++)
	{//������ѹ
		ZeroMemory(ze.name,ZIP_MAX_PATH);
		GetZipItem(hz, i, &ze);

		UseOriginalFileTime = true;
		//��ѹ�ļ����ڴ��
		char *pFileContentbuf = new char[ze.unc_size + 1];
		pFileContentbuf[ze.unc_size] = 0;
		UnzipItem(hz, i, pFileContentbuf, ze.unc_size, ZIP_MEMORY);
		if (stricmp(pszUpdtaeFileName,ze.name)==0)
		{
			//�����µ��ļ����ݸ��º���ڴ����ӵ��ڴ�zip�ļ�
			UseOriginalFileTime = false;
			ZRESULT zr = ZipAdd(hzOut,ze.name,(void*)pszNewContent,iNewContenLen, ZIP_MEMORY, 8);
			delete [] pFileContentbuf;
			continue;
		}
		atime_memfile = FileTimeToTimeT(ze.atime);
		mtime_memfile = FileTimeToTimeT(ze.mtime);
		ctime_memfile = FileTimeToTimeT(ze.ctime);
		FileTimeToLocalFileTime(&(ze.mtime),&ftm);

		//��δ���µ��ļ������ڴ����ӵ����zip�ļ�
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