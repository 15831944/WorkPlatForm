#include "StdAfx.h"
#include "FilePro.h"
#include "PeraNetDiskIceI.h"
#include "InterfaceData.h"
#include "CommonMethod.h"
#include "UploadManager.h"
#include "PeraUploadManager.h"
#include "PeraCopyManager.h"
#include "UserDefine.h"
#include "PeraNetDiskSetting.h"
#include "PeraHttpDownloadMgr.h"
#include "PeraOpenFile.h"
#include "Tools.h"
#include "FileFunc.h"

extern CPeraHttpDownloadMgr theDownloadMgr;

std::string PeraNetDiskIceI::GetDefaultErrMsgJson(const char * pszErrMsg,int iErrCode)
{
	Json::Value objOut;
	objOut["flag"]=  Json::Value("false");
	objOut["msg"] =  Json::Value(pszErrMsg);
	objOut["errorCode"] =  Json::Value(iErrCode);
	return  objOut.toStyledString().c_str();
}
void PeraNetDiskIceI::QueryLocalFileType(std::string & strJsonOut)
{
	CFilePro pro;
	CInterfaceData_QueryLocalFileType IDataOut;
	pro.GetAllDesktopRightMenuNewItems(IDataOut.m_vItemList);

	CString strDir = GetExecDir() + "\\Icon";
	pro.GetAllNewItemsIconFile(IDataOut.m_vItemList,strDir);

	strJsonOut = (LPCTSTR)IDataOut.GetJsonStrResult();
}
void PeraNetDiskIceI::QueryFilesInfo(CInterfaceData_QueryFilesInfo &IData)
{
	for (size_t iIndex =0;iIndex < IData.m_vFiles.size();iIndex++)
	{
		CString strInFile = IData.m_vFiles[iIndex]->m_sPath;
		if (!IsAbsolutePath((LPCTSTR)strInFile))
		{
			string strAbsPath =  ConvertToAbsolutePath((LPCTSTR)strInFile,(LPCTSTR)IData.m_vFiles[iIndex]->m_sUserName,
				(LPCTSTR)IData.m_vFiles[iIndex]->m_sFlowId);
			strInFile = strAbsPath.c_str();
		}

		CFilePro filePro;
		CFilePro::SFileInfo info;
		bool bOk = filePro.GetFileInfo(strInFile,info);
		IData.m_vFiles[iIndex]->m_Result.m_sFileSize.Format("%d",info.iFileSize);
		IData.m_vFiles[iIndex]->m_Result.m_sPath    = IData.m_vFiles[iIndex]->m_sPath;
		IData.m_vFiles[iIndex]->m_Result.m_sFullPath = strInFile;
		IData.m_vFiles[iIndex]->m_Result.m_sFlag     = bOk ? "true":"false"; 
		IData.m_vFiles[iIndex]->m_Result.m_sMsg      = filePro.GetErrMsg(); 
		IData.m_vFiles[iIndex]->m_Result.m_sFileName     = info.m_strFileName;
		IData.m_vFiles[iIndex]->m_Result.m_sFileSize.Format("%lld",info.iFileSize);
		IData.m_vFiles[iIndex]->m_Result.m_sCreateTime   = info.m_strCreateTime;
		IData.m_vFiles[iIndex]->m_Result.m_sModitifyTime = info.m_strLastModifyTime;
		IData.m_vFiles[iIndex]->m_Result.m_sFileType     = info.m_strFileType;
		IData.m_vFiles[iIndex]->m_Result.m_sOpenWithType = info.m_strOpenWithType;
		IData.m_vFiles[iIndex]->m_Result.m_sFileIcon = info.m_strFileIcon;
	}

}
void PeraNetDiskIceI::QueryFilesInfo(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_QueryFilesInfo IData;
	if (!IData.Parse(strIn.c_str()))
	{
		CString strErr = IData.m_strErrMsg;
		strOut = GetDefaultErrMsgJson(strErr.IsEmpty() ? "解析Json参数出错!":(LPCTSTR)strErr);
		return;
	}
	QueryFilesInfo(IData);
	strOut = (LPCTSTR)IData.GetJsonStrResult();
}
void PeraNetDiskIceI::CreateFile(const ::std::string& strIn,std::string& strOut)
{
	CInterfaceData_CreateFile IData;
	if (!IData.Parse(strIn.c_str()))
	{
		CString strErr = IData.m_strErrMsg;
		strOut = GetDefaultErrMsgJson(strErr.IsEmpty() ? "解析Json参数出错!":(LPCTSTR)strErr);
		ZTools::WriteZToolsFormatLog("%s",strErr);
		return;
	}

	CreateFile(IData);
	strOut = (LPCTSTR)IData.GetJsonStrResult();
}

void PeraNetDiskIceI::OpenFile(const ::std::string& strIn,std::string& strOut)
{
	CInterfaceData_OpenFile IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}
	COpenFilePro::OnOpenFile(IData);
	strOut = (LPCTSTR)IData.GetResultJson();
}
void PeraNetDiskIceI::CopyFiles(const ::std::string& strIn,std::string& strOut)
{
	CInterfaceData_CopyFiles *pIData = new CInterfaceData_CopyFiles;
	if (!pIData->Parse(strIn.c_str()))
	{
		delete pIData;
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}
	strOut = CPeraCopyManager::Instance()->AddCopyTask(pIData);
	delete pIData;
}
void PeraNetDiskIceI::QueryDownloadProgress(CInterfaceData_QueryFilesOperation & IData)
{
	theDownloadMgr.Query(IData);
}
void PeraNetDiskIceI::QueryFilesOperation(const ::std::string& strIn,std::string& strOut)
{
	CInterfaceData_QueryFilesOperation IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}
	CPeraCopyManager::Instance()->QueryCopyProgress(IData);
	CPeraUploadManager::Instance()->QueryUploadProgress(IData);
	QueryDownloadProgress(IData);

	strOut = (LPCTSTR)IData.GetJsonStrForResult();
}
void PeraNetDiskIceI::DoFilesOperation(const ::std::string& strIn,std::string& strOut)
{
	CInterfaceData_DoFilesOperation IData;

	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}

	if ( IData.m_soperationFlag   != "delete"
		&& IData.m_soperationFlag != "stop"
		&& IData.m_soperationFlag != "pause"
		&& IData.m_soperationFlag != "resume")
	{
		strOut = GetDefaultErrMsgJson("operationFlag参数值不正确");
		return;
	}
	
	theDownloadMgr.DoFileOperation(&IData);
	CPeraUploadManager::Instance()->DoFilesOperation(IData);
	CPeraCopyManager::Instance()->DoFilesOperation(IData);
	strOut = DefaultReturnJson;
}
void PeraNetDiskIceI::OnDownloadFile(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_DownloadFiles* pIData = new CInterfaceData_DownloadFiles;

	if ( !pIData->Parse( strIn.c_str() ) )
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		delete pIData;
		pIData = NULL;
		return;
	}

	theDownloadMgr.Add(pIData);

	delete pIData;

	strOut = DefaultReturnJson;
}
void PeraNetDiskIceI::OnUploadFile(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_UploadFile *pIData = new CInterfaceData_UploadFile;
	if (!pIData->Parse(strIn.c_str()))
	{
		CString strErr = pIData->m_strErrMsg;
		strOut = GetDefaultErrMsgJson(strErr.IsEmpty() ? "解析Json参数出错!":(LPCTSTR)strErr);
		delete pIData;
		return;
	}
	strOut = (LPCTSTR)CPeraUploadManager::Instance()->AddUploadTask(pIData);
	delete pIData;
}
void PeraNetDiskIceI::CreateFile(CInterfaceData_CreateFile &IData)
{
	string strPath = (LPCTSTR)IData.m_sPath;
	if (!IsAbsolutePath(strPath) || strPath.empty())
		strPath = ConvertToAbsolutePath(strPath,(LPCTSTR)IData.m_sUserName,(LPCTSTR)IData.m_sFlowId);

	CFilePro pro;
	string strOutFullPath = (LPCTSTR)IData.m_sFileName; // 输入为文件名，输出为新文件的全路径

	ZTools::WriteZToolsFormatLog("CreateNewItem(%s,%s,%s)",(LPCTSTR)IData.m_sType,strPath.c_str(),strOutFullPath.c_str());
	bool bOK = pro.CreateNewItem((LPCTSTR)IData.m_sType,strPath,strOutFullPath);
	if (!bOK)
	{
		IData.m_Result.m_sflag = "false";
		IData.m_Result.m_sMsg.Format("在目录【%s】下创建【%s】类型文件失败 : %s",strPath.c_str(),(LPCTSTR)IData.m_sType, pro.GetErrMsg());
		IData.m_Result.m_iErrCode = pro.GetErrCode();
		ZTools::WriteZToolsLog((LPCTSTR)IData.m_Result.m_sMsg);
		return;
	}

	CFilePro::SFileInfo info;
	bOK = pro.GetFileInfo(strOutFullPath.c_str(),info);
	if (bOK)
	{
		IData.m_Result.m_sPath     = IData.m_sPath;
		IData.m_Result.m_sFullPath = strOutFullPath.c_str();
		IData.m_Result.m_sFileName = info.m_strFileName;
		IData.m_Result.m_sFileSize.Format("%lld",info.iFileSize);
		IData.m_Result.m_sCreatetime = info.m_strCreateTime;
		IData.m_Result.m_sModifyTime = info.m_strLastModifyTime;
		IData.m_Result.m_sMsg = "";
		IData.m_Result.m_sflag = "true";
		IData.m_Result.m_iErrCode = 0;
	}
	else
	{
		IData.m_Result.m_sMsg.Format("获取新建文件【%s】信息失败!",strPath.c_str());
		IData.m_Result.m_sflag = "false";
		IData.m_Result.m_iErrCode = pro.GetErrCode();
		ZTools::WriteZToolsLog((LPCTSTR)IData.m_Result.m_sMsg);
	}
}

void PeraNetDiskIceI::OnGetSettings(const ::std::string&, std::string& strOut)
{
	strOut  = (LPCTSTR)theSetting.GetSettingToJson();
}
void PeraNetDiskIceI::OnSetSettings(const ::std::string& strIn, std::string& strOut)
{
	strOut  = (LPCTSTR)theSetting.SetSettingFromJson(strIn.c_str());
}
void PeraNetDiskIceI::OnQueryDir(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_QueryDir IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}

	CFilePro pro;
	_Folder_Item ItemAll;
	for (size_t i=0; i< IData.m_Dirs.size();i++)
	{
		_Folder_Item OneItem;
		pro.GetFolderItems(IData.m_Dirs[i],OneItem,IData.m_bRecursion,true,true,true);
		ItemAll.m_bIsFile = false;
		ItemAll.m_strPath = "";
		ItemAll.m_SubItem.push_back(OneItem);
	}
	
	strOut = IData.GetQueryDirResultJson(ItemAll);
}
void PeraNetDiskIceI::QueryDirSubCount(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_QueryDir IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}
	CFilePro pro;
	_Folder_Item ItemAll;
	ItemAll.m_bIsFile = false;
	ItemAll.m_strPath = "";
	for (size_t i=0; i< IData.m_Dirs.size();i++)
	{
		_Folder_Item OneItem;
		pro.GetFolderItems(IData.m_Dirs[i],OneItem,IData.m_bRecursion,true,true,true);
		ItemAll.m_SubItem.push_back(OneItem);
	}
	strOut = IData.GetQueryDirSubCountResultJson(ItemAll);
}
void PeraNetDiskIceI::QueryLocalOfficeFileType(std::string & strJsonOut)
{
	CFilePro pro;
	CInterfaceData_QueryLocalFileType IDataOut;
	pro.GetAllDesktopRightMenuNewItems(IDataOut.m_vItemList,TRUE);
	strJsonOut = (LPCTSTR)IDataOut.GetJsonStrResult();
}
void PeraNetDiskIceI::ReNameItem(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_ReName IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}

	string strPath = (LPCTSTR)IData.m_sPath;
	replace(strPath.begin(),strPath.end(),'/','\\');
	if (!IsAbsolutePath(strPath) || strPath.empty())
		strPath = ConvertToAbsolutePath(strPath,(LPCTSTR)IData.m_sUserName,(LPCTSTR)IData.m_sFlowId);

	/*if (!PathFileExists(strPath.c_str()))
	{
		ZTools::WriteZToolsFormatLog("%s不在缓存中",strPath.c_str());
		strOut = DefaultReturnJson;
		return;
	}*/

	CString strNewName = strPath.substr(0,strPath.rfind('\\')+1).c_str() + IData.m_sNewName;
	
	CFilePro pro;
	if (pro.ReNameLocalFile(strPath.c_str(),strNewName))
	{
		IData.m_sFlag = "true";
		strOut = DefaultReturnJson;
	}
	else
	{

		IData.m_sFlag = "false";
		IData.m_sMsg = pro.GetErrMsg();
		IData.m_iErrCode = pro.GetErrCode();
		strOut = GetDefaultErrMsgJson(IData.m_sMsg,IData.m_iErrCode);
	}
}
void PeraNetDiskIceI::DeleteItem(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_DeleteItem IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}
	CFilePro pro;
	bool bSuccess = true;
	CString strErrMsg;
	for(size_t i=0;i<IData.m_vItems.size();i++)
	{
		string strPath = (LPCTSTR)IData.m_vItems[i].m_strPath;
		std::replace(strPath.begin(),strPath.end(),'/','\\');
		if (!IsAbsolutePath(strPath) || strPath.empty())
			strPath = ConvertToAbsolutePath(strPath,(LPCTSTR)IData.m_sUserName,(LPCTSTR)IData.m_sFlowId);

		if (!PathFileExists(strPath.c_str()))
		{
			ZTools::WriteZToolsFormatLog("DeleteItem %s不在缓存中",strPath.c_str());
			continue;
		}
		bool bOK = false;
		if (IData.m_vItems[i].m_bIsFile)
			bOK = pro.DeleteLocalFile(strPath.c_str());
		else 
			bOK = pro.RemoveDir(strPath.c_str());
		if (!bOK)
		{
			bSuccess = false;
			strErrMsg += pro.GetErrMsg();
		}
	}
	if (bSuccess)
		strOut = DefaultReturnJson;
	else
	{
		strOut = GetDefaultErrMsgJson((LPCTSTR)strErrMsg);
	}
}
void PeraNetDiskIceI::PasteFiles(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_PasteFiles IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}
	strOut = CPeraCopyManager::Instance()->AddCopyTask(&IData);
}
void PeraNetDiskIceI::MoveFiles(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_MoveFiles IData;
	if (!IData.Parse(strIn.c_str()))
	{
		strOut = GetDefaultErrMsgJson("解析Json参数出错!");
		return;
	}

	CFilePro pro;
	for(size_t i=0;i<IData.m_vFiles.size();i++)
	{
		string strSrcFile = IData.m_vFiles[i].m_sSrcNetDiskPath;
		std::replace(strSrcFile.begin(),strSrcFile.end(),'/','\\');
		if (!IsAbsolutePath(strSrcFile) || strSrcFile.empty())
			strSrcFile = ConvertToAbsolutePath(strSrcFile,(LPCTSTR)IData.m_sUserName,(LPCTSTR)IData.m_vFiles[i].m_sSrcFlowId);

		if (!PathFileExists(strSrcFile.c_str()))
		{
			ZTools::WriteZToolsFormatLog("MoveFiles %s不在缓存中",strSrcFile.c_str());
			continue;
		}

		string strDstFile = IData.m_vFiles[i].m_sDstNetDiskPath;
		std::replace(strDstFile.begin(),strDstFile.end(),'/','\\');
		if (!IsAbsolutePath(strDstFile) || strDstFile.empty())
			strDstFile = ConvertToAbsolutePath(strDstFile,(LPCTSTR)IData.m_sUserName,(LPCTSTR)IData.m_vFiles[i].m_sDstFlowId);

		MakeSureDirectoryPathExists(strDstFile.c_str());
		pro.ReNameLocalFile(strSrcFile.c_str(),strDstFile.c_str());
	}

	strOut = DefaultReturnJson;
}
void PeraNetDiskIceI::SubmitToOutPut(const ::std::string& strIn, std::string& strOut)
{
	CInterfaceData_UploadFile IData;
	if (!IData.Parse(strIn.c_str()))
	{
		CString strErr = IData.m_strErrMsg;
		strOut = GetDefaultErrMsgJson(strErr.IsEmpty() ? "解析Json参数出错!":(LPCTSTR)strErr);
		return;
	}
	strOut = (LPCTSTR)CPeraUploadManager::Instance()->AddOutPutTask(&IData);
}