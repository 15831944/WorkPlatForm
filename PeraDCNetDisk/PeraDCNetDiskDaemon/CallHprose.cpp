#include "StdAfx.h"
#include "CallHprose.h"
#include "Tools.h"
#include <sstream>
#include "hprose/httpclient.hpp"
#include <afxinet.h>
#include "DownUploadCommonFunc.h"
#include "ConvertData.h"
#include "UtilityFunc.h"
#include "FileFunc.h"

CallHprose::CallHprose(std::string strHproseRpc,std::string strFilepath,std::string strIndex)
{
	m_bCreateIndexForFileInfo = FALSE;
	m_bUploadErr = false;
	m_TaskStatus = Upload_Waiting;

	dataFileObject.m_strFileId = "";
	dataFileObject.m_strHproseRpc = strHproseRpc;
	dataFileObject.m_strFilePath = strFilepath;
	dataFileObject.m_strIndex = strIndex;
	m_strNetDiskFileID = strIndex; // ���ֵ��������ʱ�򲻻��
	LONGLONG iFileSize =  FileSize(strFilepath.c_str());
	if (iFileSize<0) 
	{
		m_bUploadErr = true;
		dataFileObject.m_strMsg = "ȡ�ļ�������Ϣʧ��,�޷����ļ���" + dataFileObject.m_strFilePath;
		dataFileObject.m_nFileSize  = 0;
	}

	dataFileObject.m_nFileSize = iFileSize;
}
std::string CallHprose::UploadDefault()
{
	if (m_bUploadErr)
	{
		m_TaskStatus = Upload_Error;
		return "";
	}

	m_TaskStatus = Upload_Runing;

	UploadFileObject();

	if (m_bUploadErr)
		m_TaskStatus = Upload_Error;
	else if (Upload_Runing == m_TaskStatus )
		m_TaskStatus = Upload_Finish;

	return dataFileObject.m_strFileId;
}
CallHprose::~CallHprose(void)
{
}

std::string CallHprose::UploadFileObject()
{
	if (dataFileObject.CheckInfo() == FALSE)
	{
		m_bUploadErr = true; 
		return "";
	}
	if (m_TaskStatus  == Upload_PAUSE  || m_TaskStatus == Upload_STOP || m_TaskStatus ==  Upload_DELET)
	{
		 ZTools::WriteZToolsFormatLog("CheckInfo���,�û���%s",GetStatusMsg(m_TaskStatus));
		 return "";
	}

	ZTools::WriteZToolsFormatLog("�����ļ��ϴ�ǰ");
	
	dataFileObject.m_strFileId = "";
	
	if(m_bCreateIndexForFileInfo)
	{
		CString strTmp = GetNewGUID();
		strTmp.TrimLeft("{");
		strTmp.TrimRight("}");
		dataFileObject.m_strIndex = strTmp;
		ZTools::WriteZToolsFormatLog("�����µ�GUID��%s",dataFileObject.m_strIndex.c_str());
	}

	DataFiles oFiles;
	DataFile oFile;
	oFile.name = dataFileObject.m_strFileName;
	oFile.size = ZTools::FormatString("%lld", dataFileObject.m_nFileSize); // ���ļ�����long long ��ƥ��ʱ��lld
	oFile.lastModified = dataFileObject.m_strLastModifyTime;
	oFile.md5 = dataFileObject.m_strMd5;
	oFile.index = m_strNetDiskFileID; // ���ֵ��������ʱ�򲻻��
	oFiles.files.push_back(oFile);
	ZTools::WriteZToolsFormatLog("�ϴ��ļ�[%s]\nMD5ֵ[%s]\n�ļ���С[%s]\nIndexֵ[%s]\n������ʱ��[%s]",
		oFile.name.c_str(),oFile.md5.c_str(),oFile.size.c_str(),oFile.index.c_str(),oFile.lastModified.c_str());

	std::vector<hprose::any> vecAny;
	vecAny.clear();
	vecAny.push_back(oFiles.ToJson());
	if (!init(dataFileObject.m_strHproseRpc, vecAny))
	{
		m_bUploadErr = true;
		ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
		return "";
	}
	if (m_TaskStatus  == Upload_PAUSE  || m_TaskStatus == Upload_STOP || m_TaskStatus ==  Upload_DELET)
	{
		ZTools::WriteZToolsFormatLog("init�ɹ�,�û�:%s",GetStatusMsg(m_TaskStatus));
		return "";
	}
	if (m_bCreateIndexForFileInfo == FALSE)
		oFile.index = dataFileObject.m_strIndex; // ʹ��init�󷵻ص��µ�index

	//�ļ���СΪ0�����ļ�MD5�����ݿ����ҵ��ظ��ģ�init������ֱ�ӷ���fileId,������cwrite�߼�
	if (!dataFileObject.m_strFileId.empty())
	{
		ZTools::WriteZToolsFormatLog("�����ļ��봫���,%s",dataFileObject.m_strFileId.c_str());
		dataFileObject.m_nFileSizeUploaded = dataFileObject.m_nFileSize;

		return dataFileObject.m_strFileId;
	}

	std::wstring wstrFilePath = s2ws_1(dataFileObject.m_strFilePath);
	std::ifstream fIn(wstrFilePath, std::ios::binary);
	if (fIn == NULL || fIn.is_open() == false)
	{
		dataFileObject.m_strMsg = ZTools::FormatString("read file error: %s", dataFileObject.m_strFilePath.c_str());
		ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
		m_bUploadErr = true; 
		return "";
	}
	if(dataFileObject.m_nFileSizeUploaded > 0)
		fIn.seekg(dataFileObject.m_nFileSizeUploaded,std::ios::beg); //�ϵ�����

	int nBlockSize = 256 * 1024;
	std::vector<char> vchar;
	vchar.resize(nBlockSize, '\0');
	std::streamsize nBlockRead = 0;

	//dataFileObject.m_nFileSizeUploaded = 0;
	ZTools::WriteZToolsLog("��ʼִ��cwriteѭ��...");
	ZTools::WriteZToolsFormatLog("cwriteʹ�õ�IndexֵΪ��%s",oFile.index.c_str());
	while (true)
	{
		nBlockRead = fIn.read(reinterpret_cast<char*>(&vchar[0]), 256*1024).gcount();
		if (nBlockRead == 0)
		{
			break;
		}

		if (m_TaskStatus  == Upload_PAUSE  || m_TaskStatus == Upload_STOP || m_TaskStatus ==  Upload_DELET)
		{
			dataFileObject.m_strMsg = (LPCTSTR)GetStatusMsg(m_TaskStatus);
			ZTools::WriteZToolsFormatLog("cwriteѭ����,�û���%s",dataFileObject.m_strMsg.c_str());
			break;
		}

		vecAny.clear();
		vecAny.push_back(oFile.index);
		vecAny.push_back(vchar);
		vecAny.push_back(nBlockRead);

		if (!cwrite(dataFileObject.m_strHproseRpc, vecAny))
		{
			if (dataFileObject.m_strMsg.empty())
				dataFileObject.m_strMsg = "�ϴ�����!";
			ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
			//MessageBox(NULL,dataFileObject.m_strMsg.c_str(),"����",MB_OK|MB_ICONERROR);
			m_bUploadErr = true; 
			return "";
		}

		dataFileObject.m_nFileSizeUploaded += nBlockRead;
	}	

	if (dataFileObject.m_strFileId.empty())
	{
		if (dataFileObject.m_strMsg.empty())
			dataFileObject.m_strMsg = "unkown error";

		if (m_TaskStatus == Upload_Runing)
			m_bUploadErr = true; 

		ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
		return "";
	}

	ZTools::WriteZToolsFormatLog("�����ļ��ϴ����,%s",dataFileObject.m_strFileId.c_str());

	dataFileObject.m_nFileSizeUploaded = dataFileObject.m_nFileSize;

	return dataFileObject.m_strFileId;	
}

BOOL CallHprose::deltask(std::string strIndex)
{
	 std::vector<hprose::any> vecAny;
	 vecAny.push_back(strIndex);

	 std::string strReturn;
	 hprose::httpclient hc(dataFileObject.m_strHproseRpc);
	 hc.invoke(strReturn, "deltask", vecAny);

	 ZTools::WriteZToolsFormatLog(" deltask �����ַ���Ϊ��%s ",strReturn.c_str());
	 if (strReturn.empty())
	 {
		 dataFileObject.m_strMsg = "invoke deltask ���ؿ��ַ���";
		 ZTools::WriteZToolsLog(dataFileObject.m_strMsg.c_str());
	 }

	 DeltaskReturn delReturn;
	 delReturn.FromJson(strReturn);

	 if (delReturn.error.compare("0") == 0)
		 return TRUE;

	else if (delReturn.error.compare("1") == 0)
	 {
		 dataFileObject.m_strMsg = delReturn.error;
		 ZTools::WriteZToolsFormatLog("invoke deltask ���� error:%s",dataFileObject.m_strMsg.c_str());
		 return FALSE;
	 }
	 else
		 return TRUE;
}
//�ýӿڳɹ��ؿ��ַ�����ʧ�ܷ��ش�����ϸ��Ϣ
BOOL CallHprose::init(std::string strUrl, std::vector<hprose::any> vecAny)
{
	std::string strReturn;
	//std::locale old_loc;
	try
	{
		//old_loc = std::locale::global(std::locale("C"));
		dataFileObject.m_nFileSizeUploaded  = 0;
		hprose::httpclient hc(strUrl);
		hc.invoke(strReturn, "init", vecAny);
		ZTools::WriteZToolsFormatLog("init �����ַ���Ϊ��%s��",strReturn.c_str());
		if (strReturn.empty())
		{
			dataFileObject.m_strMsg  = ZTools::FormatString("�ϴ��ļ���%s��ʧ�ܣ�invoke init ���ؿ��ַ���!",dataFileObject.m_strFilePath.c_str());
			ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
			return FALSE;
		}
		//std::locale::global(old_loc);

		DataInitReturn oInitReturn;
		oInitReturn.FromJson(strReturn);
		dataFileObject.m_strIndex = oInitReturn.index; // ʹ�÷��ص�index����cwrite

		if (oInitReturn.flag.compare("0") == 0)
		{
			return TRUE;
		}
		else if (oInitReturn.flag.compare("1") == 0)
		{
			dataFileObject.m_strFileId = oInitReturn.fileID;
			dataFileObject.m_nFileSizeUploaded = dataFileObject.m_nFileSize;
			char szSize[50] ={0};
			sprintf_s(szSize,"%lld",dataFileObject.m_nFileSize);
			m_strFileSize = szSize;

			if (dataFileObject.m_strFileId.empty())
			{
				dataFileObject.m_strMsg  = ZTools::FormatString("init error: flag is 1, but fileID is NULL");
				ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
		else if (oInitReturn.flag.compare("4") == 0) //�ϵ�����
		{
			dataFileObject.m_nFileSizeUploaded  = oInitReturn.curentSize; //��¼�ϵ��ϴ��ļ�λ��
			return TRUE;
		}
		else
		{
			dataFileObject.m_strMsg  = ZTools::FormatString("init error: flag is not 0 or 1");
			ZTools::WriteZToolsFormatLog("%s , falg = %s",dataFileObject.m_strMsg.c_str(),oInitReturn.flag.c_str());
			return FALSE;
		}
	}
	catch (hprose::exception e)
	{
		//std::locale::global(old_loc);
		dataFileObject.m_strMsg = ZTools::FormatString("init error: %s", e.what());
		ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
		return FALSE;
	}
	catch (...)
	{

	}
	return TRUE;
}

//�ýӿڳɹ��ؿ��ַ�����ʧ�ܷ��ش�����ϸ��Ϣ
BOOL CallHprose::cwrite(std::string strUrl, std::vector<hprose::any> vecAny)
{
	std::string strReturn;
	//std::locale old_loc;	
	try
	{
		//old_loc = std::locale::global(std::locale("C"));
		hprose::httpclient hc(strUrl);
		hc.invoke(strReturn, "cwrite", vecAny);
		if (strReturn.empty())
		{
			ZTools::WriteZToolsFormatLog("cwrite �����ַ���Ϊ��");
		}
		//ZTools::WriteZToolsFormatLog("cwrite �����ַ���Ϊ��%s��",strReturn.c_str());
		//std::locale::global(old_loc);

		DataCwriteReturn oCwriteReturn;
		oCwriteReturn.FromJson(strReturn);
		if (oCwriteReturn.error.compare("0") != 0)
		{
			dataFileObject.m_strMsg  = ZTools::FormatString("cwrite error: %s", oCwriteReturn.msg.c_str());
			ZTools::WriteZToolsLog(dataFileObject.m_strMsg);
			return FALSE;
		}

		if (!oCwriteReturn.fileID.empty())
		{
			dataFileObject.m_strFileId = oCwriteReturn.fileID;
			m_strFileSize = oCwriteReturn.end;
		}
	}
	catch (hprose::exception e)
	{
		//std::locale::global(old_loc);
		strReturn = ZTools::FormatString("cwrite error: %s", e.what());
		dataFileObject.m_strMsg = strReturn;
		ZTools::WriteZToolsLog(strReturn);
		return FALSE;
	}
	catch (...)
	{

	}
	return TRUE;
}
char dec2hexChar(short int n) {
	if ( 0 <= n && n <= 9 ) {
		return char( short('0') + n );
	} else if ( 10 <= n && n <= 15 ) {
		return char( short('A') + n - 10 );
	} else {
		return char(0);
	}
}

short int hexChar2dec(char c) {
	if ( '0'<=c && c<='9' ) {
		return short(c-'0');
	} else if ( 'a'<=c && c<='f' ) {
		return ( short(c-'a') + 10 );
	} else if ( 'A'<=c && c<='F' ) {
		return ( short(c-'A') + 10 );
	} else {
		return -1;
	}
}

std::string escapeURL(const std::string &URL)
{
	std::string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if (
			( '0'<=c && c<='9' ) ||
			( 'a'<=c && c<='z' ) ||
			( 'A'<=c && c<='Z' ) ||
			c=='/' || c=='.'
			) {
				result += c;
		} else {
			int j = (short int)c;
			if ( j < 0 ) {
				j += 256;
			}
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1*16;
			result += '%';
			result += dec2hexChar(i1);
			result += dec2hexChar(i0);
		}
	}
	return result;
}

std::string deescapeURL(const std::string &URL) {
	std::string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if ( c != '%' ) {
			result += c;
		} else {
			char c1 = URL[++i];
			char c0 = URL[++i];
			int num = 0;
			num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
			result += char(num);
		}
	}
	return result;
}
std::string CallHprose::DownloadHprose( std::string strHproseDownloadUrl, std::string strFileDir, std::string strFileName )
{
	CInternetSession sess;
	CHttpFile* pHttpFile;
	try
	{
		pHttpFile=(CHttpFile*)sess.OpenURL(strHproseDownloadUrl.c_str());
	}
	catch(CException* e)
	{
		pHttpFile = 0;
		TCHAR msg[1024];
		memset(msg, 0, 1024);
		e->GetErrorMessage(msg, 1023, NULL);
		ZTools::WriteZToolsFormatLog("��URLʧ�ܣ�%s", msg);
		return "";
	}

	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

	int nReadBlockSize = 256*1024;
	char* pBuffer = new char[nReadBlockSize];
	if( bSuccess && dwStatus>= 200&& dwStatus<300 )
	{
		if (strFileName.length() == 0)
		{
			CString sContentDisposition;
			DWORD dwIndex = 0;
			bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_DISPOSITION, sContentDisposition, &dwIndex);
			std::string strContentDisposition((LPCTSTR)sContentDisposition);
			std::string strKeyBegin("filename=\"");
			std::string strKeyEnd("\"");

			if (!strContentDisposition.empty())
			{
				size_t nBegin = strContentDisposition.find(strKeyBegin) + strKeyBegin.length();
				if (nBegin < strContentDisposition.length())
				{
					size_t nEnd = strContentDisposition.find(strKeyEnd, nBegin);
					strFileName = strContentDisposition.substr(nBegin, nEnd - nBegin);
				}
				strFileName = deescapeURL(strFileName);
				ZTools::UTF8ToMB(strFileName);
			}
		}

		if (strFileName.length() == 0)
		{
			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			//delete pHttpFile;
			sess.Close();
			ZTools::WriteZToolsFormatLog("û���ҵ��ļ���");
			return "";
		}

		std::replace(strFileDir.begin(), strFileDir.end(), '/', '\\');
		strFileDir.erase(strFileDir.find_last_not_of('\\') + 1);
		std::replace(strFileName.begin(), strFileName.end(), '/', '\\');
		strFileName.erase(0, strFileName.find_first_not_of('\\'));
		std::string strFilePath = ZTools::FormatString("%s\\%s", strFileDir.c_str(), strFileName.c_str());
		//�����ص���ʱ�ļ��У����سɹ���������
		std::string strFilePathTemp = ZTools::FormatString("%s\\%s", strFileDir.c_str(), ZTools::GetCurrentTimeString16().c_str());

		if (!MakeSureDirectoryPathExists(strFilePath.c_str()))
		{
			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			//delete pHttpFile;
			sess.Close();
			ZTools::WriteZToolsFormatLog("�����ļ���ʧ�ܣ�%s", strFilePath.c_str());
			return "";
		}

		CFile fileWrite;
		ULONGLONG dwFileLen = 0;
		DWORD dwWriteIndex = 0;
		dwFileLen = pHttpFile->SeekToEnd();
		pHttpFile->SeekToBegin();
		if(fileWrite.Open(strFilePathTemp.c_str(), CFile::modeWrite|CFile::modeCreate))
		{
			int nCount = 0;
			while(dwWriteIndex < dwFileLen)
			{
				nCount = pHttpFile->Read(pBuffer, nReadBlockSize);
				fileWrite.Write(pBuffer, nCount);
				dwWriteIndex += nCount;                
			}
			fileWrite.Close();

			rename(strFilePathTemp.c_str(), strFilePath.c_str());

			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			delete pHttpFile;
			sess.Close();

			if (PathFileExists(strFilePath.c_str()))
			{
				return strFilePath;
			}
			else
			{
				return "";
			}
		}
		else
		{
			delete[] pBuffer;
			pBuffer = NULL;
			pHttpFile->Close();
			//delete pHttpFile;
			sess.Close();
			ZTools::WriteZToolsFormatLog("�����ļ�%s�򿪳���", strFilePath.c_str());
			return "";
		}
	}
	else
	{
		delete[] pBuffer;
		pBuffer = NULL;
		pHttpFile->Close();
		//delete pHttpFile;
		sess.Close();
		ZTools::WriteZToolsFormatLog("����ҳ�ļ����������룺%d", dwStatus);
		return "";
	}

	delete[] pBuffer;
	pBuffer = NULL;
	pHttpFile->Close();
	//delete pHttpFile;
	sess.Close();	
	return "";
}

void DataInterfaceReturn::Clear()
{
	error.clear();
	msg.clear();
	file.clear();
}

void DataInterfaceReturn::FromJson( std::string strJson )
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strJson, root))
	{
		error = root["error"].asString();
		msg = root["msg"].asString();
		file = root["file"].asString();
	}
}

std::string DataInterfaceReturn::ToJson()
{
	Json::FastWriter writer;
	Json::Value root;
	root["error"] = Json::Value(error);
	root["msg"] = Json::Value(msg);
	root["file"] = Json::Value(file);
	return writer.write(root);
}

void DataFile::Clear()
{
	lastModified.clear();
	name.clear(); 
	size.clear();
	index.clear();
	md5.clear();
}

void DataFiles::Clear()
{
	files.clear();
}

void DataFiles::FromJson( std::string strJson )
{

}

std::string DataFiles::ToJson()
{
	Json::FastWriter writer;
	Json::Value root;
	Json::Value jFiles(Json::arrayValue);
	for (std::vector<DataFile>::iterator it = files.begin(); it != files.end(); ++it)
	{
		Json::Value jFile;
		jFile["lastModified"] = Json::Value(it->lastModified);
		jFile["name"] = Json::Value(it->name);
		jFile["size"] = Json::Value(it->size);
		jFile["index"] = Json::Value(it->index);
		jFile["md5"] = Json::Value(it->md5);
		jFiles.append(jFile);
	}
	root["files"] = jFiles;
	return writer.write(root);
}

void DataInitReturn::FromJson( std::string strJson )
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strJson, root))
	{
		int nLength = root.size();
		//ZTools::WriteZToolsFormatLog("������JSON�ִ���%s", strJson.c_str());
		if (nLength > 0)
		{
			Json::Value file = root[(Json::Value::ArrayIndex)0];
			index = file["index"].asString();
			curentSize = file["curentSize"].asUInt();
			flag = file["flag"].asString();
			fileID = file["fileID"].asString();
		}
	}
}

void DataCwriteReturn::FromJson( std::string strJson )
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strJson, root))
	{
		error = root["error"].asString();
		fileID = root["fileID"].asString();
		fileName = root["fileName"].asString();
		end = root["end"].asString();
		msg = root["msg"].asString();
	}
	else
	{
		msg = strJson;
	}
}
void DeltaskReturn::FromJson(std::string strJson)
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strJson, root))
	{
		error = root["error"].asString();
		msg = root["msg"].asString();
	}
	else
	{
		msg = strJson;
	}
}
CString CallHprose::GetStatusMsg(UploadTaskStatus status)
{
	static CString StatusMsg[] = {"�ȴ���ʼ","�����ϴ�","�ϴ����",
		"ɾ���ϴ�","ֹͣ�ϴ�","��ͣ�ϴ�","�ٴ�ִ��","�ϴ�����",""};

	return StatusMsg[(int)status];
}
void CallHprose::ResetErrFlag()
{
	m_bUploadErr = !m_bUploadErr;
}