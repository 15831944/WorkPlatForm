// FileUpDown.cpp : CFileUpDown ��ʵ��

#include "stdafx.h"
#include "FileUpDown.h"
#include "CallHprose.h"
#include "Tools.h"
#include <comdef.h>
#include "GetFlexwareBin.h"
#include <sys/stat.h>
#include "IniClass.h"
#include "DownUploadFileCacheFinder.h"
#include "DownUploadCommonFunc.h"
#include "ConfigFunc.h"

// CFileUpDown

std::string CFileUpDown::GetHproseRpc()
{
	CString sUrl = GetConfig("hprose", "hproseRpc");
	return std::string((LPCTSTR)sUrl);
}

std::string CFileUpDown::GetHproseDownloadUrl()
{
	CString sUrl = GetConfig("hprose", "hproseDownloadUrl");
	return std::string((LPCTSTR)sUrl);
}

STDMETHODIMP CFileUpDown::Upload(BSTR bstrHprose, BSTR bstrFilePath, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Upload");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	if (strHprose.empty())
	{
		strHprose = GetHproseRpc();
	}
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strFilePath = (bstrFilePath != NULL) ? _bstr_t(bstrFilePath) : "";
	ZTools::WriteZToolsFormatLog("strFilePath:%s", strFilePath.c_str());
	
	CallHprose ch;
	std::string strReturn = ch.Upload(strHprose, strFilePath);
	
	vRet = _bstr_t(strReturn.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}


STDMETHODIMP CFileUpDown::Download(BSTR bstrHprose, BSTR bstrDir, BSTR bstrFileName, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Download");
	CComVariant vRet(_bstr_t("").GetBSTR());
	
	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strDir = (bstrDir != NULL) ? _bstr_t(bstrDir) : "";
	ZTools::WriteZToolsFormatLog("strDir:%s", strDir.c_str());
	std::string strFileName = (bstrFileName != NULL) ? _bstr_t(bstrFileName) : "";
	ZTools::WriteZToolsFormatLog("strFileName:%s", strFileName.c_str());

	CallHprose ch;
	std::string strReturn = ch.DownloadHprose(strHprose, strDir, strFileName);

	vRet = _bstr_t(strReturn.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

//��ȡ�ļ�����
long GetFileSize(const char *filename)
{
	struct stat     f_stat;
	if (stat(filename, &f_stat) == -1) {
		return -1;
	}
	return (long)f_stat.st_size;
}
//��ȡ�ϴ��ļ�info.xml������Ϣ��FileID
std::string CFileUpDown::LoadFileID(std::string strUrl, std::string strFilePath)
{
	std::string strRet;

	long nFileSize = GetFileSize(strFilePath.c_str());
	if(nFileSize == -1)
	{
		return strRet;
	}

	std::string strFileMD5 = GetFileMd5(strFilePath); //MD5
	ZTools::WriteZToolsFormatLog("file Md5: %s", strFileMD5.c_str());
	std::string strFileName = GetFileName(strFilePath);

// 	std::ostringstream oss;
// 	oss << nFileSize;
// 
// 	std::string strFileSize = oss.str(); //�ļ���С

	CString str;
	str.Format("%d", nFileSize);
	std::string strFileSize = str;

	//���ػ���·��
	CString strCacheRoot =  GetConfig("Pera","RunCache");
	if (strCacheRoot.IsEmpty())
		strCacheRoot = "C:\\WorkSpace\\Data";
	//���ػ���+fileid
	std::string strFileCacheBaseDir;

	{//�ļ�·������
		strFileCacheBaseDir = ZTools::FormatString("%s\\DmsFileCache\\Upload\\%s\\", strCacheRoot, strUrl.c_str());
	}

	std::string strInfoDir = ZTools::FormatString("%s%s_%s_%s\\", strFileCacheBaseDir.c_str(), strFileName.c_str(), strFileMD5.c_str(), strFileSize.c_str());

	std::string strFileInfoXmlPath;
	strFileInfoXmlPath = ZTools::FormatString("%sinfo.xml", strInfoDir.c_str());

	std::string strFileNameInfo; //����
	std::string strFileMD5Info; //MD5
	std::string strFileIdInfo; //ID
	int nFileSizeInfo = 0; //��С

	if (PathFileExists(strFileInfoXmlPath.c_str()))
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.LoadFile(strFileInfoXmlPath, TIXML_ENCODING_UTF8);

		TiXmlElement* rootElement = doc.RootElement();
		if (rootElement)
		{
// 			rootElement->QueryStringAttribute("fileName", &strFileNameInfo);
// 			ZTools::UTF8ToMB(strFileNameInfo);
			rootElement->QueryIntAttribute("fileSize", &nFileSizeInfo);
			rootElement->QueryStringAttribute("fileID", &strFileIdInfo);
			rootElement->QueryStringAttribute("fileMD5", &strFileMD5Info);

			if (nFileSizeInfo == nFileSize && strFileMD5Info == strFileMD5)
			{
				return strFileIdInfo;
			}
		}
	}

	return strRet;
}

//��ȡ�ϴ��ļ�info.xml������Ϣ��FileID
BOOL CFileUpDown::SaveFileID(std::string strUrl, std::string strFilePath, std::string strFileId)
{
	std::string strRet;

	long nFileSize = GetFileSize(strFilePath.c_str());
	if(nFileSize == -1)
	{
		return FALSE;
	}

	std::string strFileMD5 = GetFileMd5(strFilePath); //MD5
	std::string strFileName = GetFileName(strFilePath);

	// 	std::ostringstream oss;
	// 	oss << nFileSize;
	// 
	// 	std::string strFileSize = oss.str(); //�ļ���С

	CString str;
	str.Format("%d", nFileSize);
	std::string strFileSize = str;

	//���ػ���·��
	CString strCacheRoot =  GetConfig("Pera","RunCache");
	if (strCacheRoot.IsEmpty())
		strCacheRoot = "C:\\WorkSpace\\Data";

	//���ػ���+fileid
	std::string strFileCacheBaseDir;

	{//�ļ�·������
		strFileCacheBaseDir = ZTools::FormatString("%s\\DmsFileCache\\Upload\\%s\\", strCacheRoot, strUrl.c_str());
	}

	std::string strInfoDir = ZTools::FormatString("%s%s_%s_%s\\", strFileCacheBaseDir.c_str(), strFileName.c_str(), strFileMD5.c_str(), strFileSize.c_str());

	std::string strFileInfoXmlPath;
	strFileInfoXmlPath = ZTools::FormatString("%sinfo.xml", strInfoDir.c_str());

	if (MakeSureDirectoryPathExists(strFileInfoXmlPath.c_str()))
	{//����Ŀ¼
		//��¼��Ϣ
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
		doc.LinkEndChild( decl ); 
		TiXmlElement* root = new TiXmlElement("file");
		root->SetAttribute("fileMD5", ZTools::MBToUTF8(strFileMD5));
		root->SetAttribute("fileSize", ZTools::MBToUTF8(strFileSize));
		root->SetAttribute("fileID", ZTools::MBToUTF8(strFileId));
		doc.LinkEndChild(root);
		doc.SaveFile(strFileInfoXmlPath);
	}

	return TRUE;
}

//�ϴ��ļ��������ݵ������ļ�������
//bstrHprose,dms�ṩ�ķ���ĵ�ַ
//bstrFilePath,Ҫ�ϴ����ļ�·��
//bstrBoolMove,ָ������ʱ�ƶ����Ǹ��ƣ��ƶ��Ļ�Ч�ʻ��һЩ������ֵӦΪtrue��false��Ĭ��Ϊfalse��ִ�и���
//����ֵΪfileid����Ϊ�գ���ʾʧ��
STDMETHODIMP CFileUpDown::Upload2( BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrBoolMove, VARIANT* pvRet )
{
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Upload2");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strFilePath = (bstrFilePath != NULL) ? _bstr_t(bstrFilePath) : "";
	ZTools::WriteZToolsFormatLog("strFilePath:%s", strFilePath.c_str());
	std::string strBoolMove = (bstrBoolMove != NULL) ? _bstr_t(bstrBoolMove) : "";
	ZTools::WriteZToolsFormatLog("strBoolMove:%s", strBoolMove.c_str());

	//���浽�����ļ�����
	bool bMove = false;
	ZTools::LowerString(strBoolMove);
	if (strBoolMove.compare("true") == 0)
	{
		bMove = true;
	}

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strFilePath = strFilePath;
	dataFileObject.m_strHproseRpc = strHprose;

	CallHprose ch;
	ch.UploadUseCache(&dataFileObject, bMove);

	if (dataFileObject.m_strFileId.empty())
	{
		ZTools::WriteZToolsFormatLog("�ϴ�ʧ��: %s", dataFileObject.m_strMsg.c_str());
	}

	ZTools::WriteZToolsFormatLog("finish,fileid: %s", dataFileObject.m_strFileId.c_str());
	vRet = _bstr_t(dataFileObject.m_strFileId.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

//�ϴ��ļ��������ݵ������ļ�������
//bstrHprose,dms�ṩ�ķ���ĵ�ַ
//bstrFilePath,Ҫ�ϴ����ļ�·��
//bstrFileMd5,Ҫ�ϴ����ļ���md5ֵ
//bstrBoolMove,ָ������ʱ�ƶ����Ǹ��ƣ��ƶ��Ļ�Ч�ʻ��һЩ������ֵӦΪtrue��false��Ĭ��Ϊfalse��ִ�и���
//����ֵΪfileid����Ϊ�գ���ʾʧ��
STDMETHODIMP CFileUpDown::Upload3(BSTR bstrHprose, BSTR bstrFilePath, BSTR bstrFileMd5, BSTR bstrBoolMove, VARIANT* pvRet)
{
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Upload3");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
	ZTools::WriteZToolsFormatLog("strHprose:%s", strHprose.c_str());
	std::string strFilePath = (bstrFilePath != NULL) ? _bstr_t(bstrFilePath) : "";
	ZTools::WriteZToolsFormatLog("strFilePath:%s", strFilePath.c_str());
	std::string strFileMd5 = (bstrFileMd5 != NULL) ? _bstr_t(bstrFileMd5) : "";
	ZTools::WriteZToolsFormatLog("strFileMd5:%s", strFileMd5.c_str());
	std::string strBoolMove = (bstrBoolMove != NULL) ? _bstr_t(bstrBoolMove) : "";
	ZTools::WriteZToolsFormatLog("strBoolMove:%s", strBoolMove.c_str());

	//���浽�����ļ�����
	bool bMove = false;
	ZTools::LowerString(strBoolMove);
	if (strBoolMove.compare("true") == 0)
	{
		bMove = true;
	}

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strFilePath = strFilePath;
	dataFileObject.m_strHproseRpc = strHprose;
	dataFileObject.m_strMd5 = strFileMd5;

	CallHprose ch;
	ch.UploadUseCache(&dataFileObject, bMove);

	if (dataFileObject.m_strFileId.empty())
	{
		ZTools::WriteZToolsFormatLog("�ϴ�ʧ��: %s", dataFileObject.m_strMsg.c_str());
	}

	ZTools::WriteZToolsFormatLog("finish,fileid: %s", dataFileObject.m_strFileId.c_str());
	vRet = _bstr_t(dataFileObject.m_strFileId.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

//�����ļ����ȴӱ����ļ���������
//bstrHproseBaseUrl,Ҫ���ص��ļ��Ļ���URL������fileid
//bstrFileId,Ҫ���ص��ļ���fileid
//bstrFileName,ָ�����ļ���������ļ��������У���ò����������ã�ֻ����������ʱ�õ�����Ϊ�գ����URL��ͷ��Ϣ��ȡ�ļ������绹û�У�������ʧ��
//����ֵΪ�ַ������ձ�ʾʧ�ܣ��ɹ��Ļ�������ֵΪ���غ���ļ�����·������·�������ļ������е��ļ�·�����������������Ҫ��ȷ���Ƿ���
STDMETHODIMP CFileUpDown::Download2( BSTR bstrHproseBaseUrl, BSTR bstrFileId, BSTR bstrFileName, VARIANT* pvRet )
{
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::Download2");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strHproseBaseUrl = (bstrHproseBaseUrl != NULL) ? _bstr_t(bstrHproseBaseUrl) : "";
	if (strHproseBaseUrl.empty())
	{
		strHproseBaseUrl = GetHproseDownloadUrl();
	}
	ZTools::WriteZToolsFormatLog("strHproseBaseUrl:%s", strHproseBaseUrl.c_str());
	std::string strFileId = (bstrFileId != NULL) ? _bstr_t(bstrFileId) : "";
	ZTools::WriteZToolsFormatLog("strFileId:%s", strFileId.c_str());
	std::string strFileName = (bstrFileName != NULL) ? _bstr_t(bstrFileName) : "";
	ZTools::WriteZToolsFormatLog("strFileName:%s", strFileName.c_str());

	CDownUploadDataFileObject dataFileObject;
	dataFileObject.m_strHproseRpc = strHproseBaseUrl;
	dataFileObject.m_strFileId = strFileId;

	if (strFileId.empty())
	{
		vRet = "";
		vRet.Detach(pvRet);
		return S_OK;
	}

	if (CDownUploadFileCacheFinder::ReadDownloadCache(&dataFileObject))
	{
		vRet = _bstr_t(dataFileObject.m_strFilePath.c_str()).GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}

	//ͬһ��fileid��һ��ʱ��ֻ��һ����ʵ����
	CString sMutexName;
	sMutexName.Format("download_hprose_fileid_%s", strFileId.c_str());
	HANDLE hMutex = CreateMutex(NULL, TRUE, sMutexName);
	WaitForSingleObject(hMutex, INFINITE);
	//�õ����ػ�����ټ��һ�黺�����Ƿ���������
	if (CDownUploadFileCacheFinder::ReadDownloadCache(&dataFileObject))
	{
		ZTools::WriteZToolsFormatLog("�õ�������Դ���ٴμ�黺�棬�ļ������أ�%s", strFileId.c_str());
		CloseHandle(hMutex);
		vRet = _bstr_t(dataFileObject.m_strFilePath.c_str()).GetBSTR();
		vRet.Detach(pvRet);
		return S_OK;
	}
	else
	{
		ZTools::WriteZToolsFormatLog("�õ�������Դ���ٴμ�黺�棬�ļ���δ���أ���ʼ���أ�%s", strFileId.c_str());
	}
	
	std::string strCacheBasePath = GetDownloadCacheBasePath();
	if (strCacheBasePath.find("\\default\\") != std::string::npos && !dataFileObject.m_strHproseRpc.empty())
	{
		strCacheBasePath.resize(strCacheBasePath.length() - 8);
		strCacheBasePath += FormatUrl(dataFileObject.m_strHproseRpc);
		strCacheBasePath += "\\";
	}

	std::string strCurFileDir;
	strCurFileDir = strCacheBasePath + dataFileObject.m_strFileId + "\\";//��ȡ����·��

	std::string strReturnFilePath;
	//����
	std::string strHprose = ZTools::FormatString("%s%s", strHproseBaseUrl.c_str(), strFileId.c_str());
	CallHprose ch;
	strReturnFilePath = ch.DownloadHprose(strHprose, strCurFileDir, strFileName);

	if (!strReturnFilePath.empty() && PathFileExists(strReturnFilePath.c_str()))
	{
		//��¼��Ϣ
		dataFileObject.m_strFilePath = strReturnFilePath;
		dataFileObject.CheckInfo(FALSE);
		CDownUploadFileCacheFinder::WriteDownloadCache(&dataFileObject);
	}
	else
	{
		strReturnFilePath.clear();
	}

	CloseHandle(hMutex);

	vRet = _bstr_t(strReturnFilePath.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

struct StructUpDown
{
	enum Type
	{
		TYPE_UPLOAD, //�ϴ�
		TYPE_DOWNLOAD  //����
	};

	CFileUpDown* m_pCFUD; //������

	Type m_type; //����

	std::string m_strFilePath; //�ļ�·�����ϴ���
	std::string m_strHprose; //��ַ
	std::string m_strBoolMove; //�ƶ���־
	std::string m_strFileID; //�����ļ�ID 
	std::string m_strFileDir; //����Ŀ¼
	std::string m_strFileName; //��������
};

static UINT RunThread( LPVOID pParam )
{
	StructUpDown* pSUD = (StructUpDown*)pParam;
	CFileUpDown* pFUD = pSUD->m_pCFUD;

	CComBSTR bstrHprose(_bstr_t(pSUD->m_strHprose.c_str()).GetBSTR());

	switch (pSUD->m_type)
	{
	case pSUD->TYPE_UPLOAD:
		{//�ϴ�
			CComBSTR bstrFilePath(_bstr_t(pSUD->m_strFilePath.c_str()).GetBSTR());
			CComBSTR bstrBoolMove(_bstr_t(pSUD->m_strBoolMove.c_str()).GetBSTR());
			CComVariant var;
			pFUD->Upload2(bstrHprose, bstrFilePath, bstrBoolMove, &var);
			CString strFileID = (LPCTSTR)_bstr_t(var);

			g_cCriticalSection.Lock();//�߳�ͬ��
			//pFUD->m_Jv.append(Json::Value()[pSUD->m_strFilePath] = (Json::Value()["fileID"] = strFileID.GetBuffer()));
			Json::Value jv;
			jv["fileID"] = strFileID.GetBuffer();
			pFUD->m_Jv[pSUD->m_strFilePath] = jv;
			g_cCriticalSection.Unlock();

			break;
		}
	case pSUD->TYPE_DOWNLOAD:
		{//����
			CComBSTR bstrFileId(_bstr_t(pSUD->m_strFileID.c_str()).GetBSTR());
			CComBSTR bstrFileName(_bstr_t(pSUD->m_strFileName.c_str()).GetBSTR());
			CComVariant var;
			pFUD->Download2(bstrHprose, bstrFileId, bstrFileName, &var);
			CString strFilePath = (LPCTSTR)_bstr_t(var);

			std::string strDstFilePath = strFilePath;
			if(!pSUD->m_strFileDir.empty())
			{//���ƻ����ļ���Ŀ��Ŀ¼
				std::string strRefFilePath = strFilePath;
				if (pSUD->m_strFileName.empty())
				{
					pSUD->m_strFileName = strRefFilePath.substr(strRefFilePath.find_last_of('\\') + 1, strRefFilePath.length() - strRefFilePath.find_last_of('\\') - 1);
				}

				std::replace(pSUD->m_strFileDir.begin(), pSUD->m_strFileDir.end(), '/', '\\');
				pSUD->m_strFileDir.erase(pSUD->m_strFileDir.find_last_not_of('\\') + 1);
				strDstFilePath = ZTools::FormatString("%s\\%s", pSUD->m_strFileDir.c_str(), pSUD->m_strFileName.c_str());

				MakeSureDirectoryPathExists(strDstFilePath.c_str());
				CopyFile(strRefFilePath.c_str(), strDstFilePath.c_str(), FALSE);
				if (PathFileExists(strDstFilePath.c_str()))
				{
					//return strDstFilePath;
				}
				else
				{
					strDstFilePath = "";
				}
			}

			g_cCriticalSection.Lock();//�߳�ͬ��
			//pFUD->m_Jv.append(Json::Value()[pSUD->m_strFilePath] = (Json::Value()["fileID"] = strFileID.GetBuffer()));
			Json::Value jv;
			jv["filePath"] = strDstFilePath;
			pFUD->m_Jv[pSUD->m_strFileID] = (jv);
			g_cCriticalSection.Unlock();

			break;
		}
	}
	
	delete pSUD;
	return 0;
}

STDMETHODIMP CFileUpDown::UploadFiles( BSTR bstrHprose, BSTR bstrJson, BSTR bstrBoolMove, VARIANT* pvRet )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::UploadFiles");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strJson = (bstrJson != NULL) ? _bstr_t(bstrJson) : "";
	std::string strJsonRet;
	ZTools::WriteZToolsFormatLog("bstrJson:%s", strJson.c_str());

	Json::Reader jReader;
	Json::Value jValue;
	Json::FastWriter jWriter;

	if(!jReader.parse(strJson, jValue) && !jValue.isArray())
	{//����ʧ��
		vRet.Detach(pvRet);
		return S_OK;
	}

	int nCount = jValue.size(); //�ϴ�����

	HANDLE* handles = new HANDLE[nCount]; //�߳̾��
	CWinThread** threads = new CWinThread*[nCount]; //WinThread

	//m_Jv = Json::Value(Json::arrayValue);
	m_Jv = Json::Value();

	for(int i = 0; i < nCount; i++)
	{
		StructUpDown* pSUD = new StructUpDown;
		pSUD->m_type = StructUpDown::TYPE_UPLOAD;
		pSUD->m_pCFUD = this;
		pSUD->m_strFilePath = jValue[i]["filePath"].asString();
		pSUD->m_strHprose = (bstrHprose != NULL) ? _bstr_t(bstrHprose) : "";
		pSUD->m_strBoolMove = (bstrBoolMove != NULL) ? _bstr_t(bstrBoolMove) : "";

		threads[i] = AfxBeginThread(RunThread, pSUD, 0, 0, CREATE_SUSPENDED, 0);
		threads[i]->m_bAutoDelete = FALSE;
		handles[i] = threads[i]->m_hThread; //�����	
		threads[i]->ResumeThread();
	}

	WaitForMultipleObjects(nCount, handles, TRUE, INFINITE); //�ȴ�ȫ���߳̽���

	for(int i = 0; i < nCount; i++)
	{
		delete threads[i];
		//CloseHandle(handles[i]);
	}
	delete [] threads;
	delete [] handles;

	strJsonRet = jWriter.write(m_Jv);
	vRet = _bstr_t(strJsonRet.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}


STDMETHODIMP CFileUpDown::DownloadFiles(BSTR bstrHproseBaseUrl, BSTR bstrJson, VARIANT* pvRet)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: �ڴ����ʵ�ִ���
	ZTools::WriteZToolsLog(">>>>>>PeraFileUpDown::CFileUpDown::DownloadFiles");
	CComVariant vRet(_bstr_t("").GetBSTR());

	std::string strJson = (bstrJson != NULL) ? _bstr_t(bstrJson) : "";
	std::string strJsonRet;
	ZTools::WriteZToolsFormatLog("bstrJson:%s", strJson.c_str());

	Json::Reader jReader;
	Json::Value jValue;
	Json::FastWriter jWriter;

	if(!jReader.parse(strJson, jValue) && !jValue.isArray())
	{//����ʧ��
		vRet.Detach(pvRet);
		return S_OK;
	}

	int nCount = jValue.size(); //���ظ���

	HANDLE* handles = new HANDLE[nCount]; //�߳̾��
	CWinThread** threads = new CWinThread*[nCount]; //WinThread

	//m_Jv = Json::Value(Json::arrayValue);
	m_Jv = Json::Value();

	for(int i = 0; i < nCount; i++)
	{
		StructUpDown* pSUD = new StructUpDown;
		pSUD->m_type = StructUpDown::TYPE_DOWNLOAD;
		pSUD->m_pCFUD = this;
		pSUD->m_strFileID = jValue[i]["fileID"].asString();
		pSUD->m_strFileDir = jValue[i]["fileDir"].asString();
		pSUD->m_strFileName = jValue[i]["fileName"].asString();
		pSUD->m_strHprose = (bstrHproseBaseUrl != NULL) ? _bstr_t(bstrHproseBaseUrl) : "";

		threads[i] = AfxBeginThread(RunThread, pSUD, 0, 0, CREATE_SUSPENDED, 0);
		threads[i]->m_bAutoDelete = FALSE;
		handles[i] = threads[i]->m_hThread; //�����	
		threads[i]->ResumeThread();
	}

	WaitForMultipleObjects(nCount, handles, TRUE, INFINITE); //�ȴ�ȫ���߳̽���

	for(int i = 0; i < nCount; i++)
	{
		delete threads[i];
		//CloseHandle(handles[i]);
	}
	delete [] threads;
	delete [] handles;

	strJsonRet = jWriter.write(m_Jv);
	vRet = _bstr_t(strJsonRet.c_str()).GetBSTR();
	vRet.Detach(pvRet);

	return S_OK;
}

CFileUpDown::CFileUpDown()
{
	ZTools::InitZToolsLog("PeraFileUpDown", "PeraFileUpDown.dll");
}