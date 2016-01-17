#include "StdAfx.h"
#include "PeraNetDiskIceI.h"
#include "Tools.h"
#include "FilePro.h"

PeraNetDiskIceI::PeraNetDiskIceI(void)
{
}


PeraNetDiskIceI::~PeraNetDiskIceI(void)
{
}
::std::string PeraNetDiskIceI::InvokeMethod( const ::std::string& strMethodName, const ::std::string& strJsonIn, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
	ZTools::WriteZToolsFormatLog("------PeraNetDiskIceI::InvokeMethod------\nstrMethodName: %s\nstrJsonIn: %s", strMethodName.c_str(), strJsonIn.c_str());
	std::string strResult;

	/*CFilePro pro;
	vector<_SelectItem> vstr = pro.GetSelectItems("singleFile","�����ļ�(*.*)|*.*||");
	vstr = pro.GetSelectItems("singleFolder","�����ļ�(*.*)|*.*||");
	vstr = pro.GetSelectItems("multiFile","�����ļ�(*.*)|*.*||");
	vstr = pro.GetSelectItems("multiFolder","�����ļ�(*.*)|*.*||");
	vstr = pro.GetSelectItems("fileAndFolder","�����ļ�(*.*)|*.*||");
	vstr = pro.GetSelectItems("singleItem","�����ļ�(*.*)|*.*||");
	return "";*/

	//����strMethodName�����������ĸ�������
	if ("QueryLocalFileType" == strMethodName ) // ��ѯ���ؿ��½����ļ�����
	{
		QueryLocalFileType(strResult);
	}
	else if ("QueryFilesInfo" == strMethodName )
	{
		QueryFilesInfo(strJsonIn,strResult);
	}
	else if ("CreateFile" == strMethodName )
	{
		CreateFile(strJsonIn,strResult);
	}
	else if ("OpenFile" == strMethodName )
	{
		OpenFile(strJsonIn,strResult);
	}
	else if ("CopyFiles" == strMethodName )
	{
		CopyFiles(strJsonIn,strResult);
	}
	else if ("SetCacheDir" == strMethodName )
	{
		SetCacheDir(strJsonIn,strResult);
	}
	else if ("GetCacheDir" == strMethodName )
	{
		GetCacheDir(strJsonIn,strResult);
	}
	else if ("QueryFilesOperation" == strMethodName )
	{
		QueryFilesOperation(strJsonIn,strResult);
	}
	else if ("DoFilesOperation" == strMethodName )
	{
		DoFilesOperation(strJsonIn,strResult);
	}
	else if ("DownloadFiles" == strMethodName )
	{
		OnDownloadFile(strJsonIn, strResult);
	}
	else if ("UploadFile" == strMethodName )
	{
		OnUploadFile(strJsonIn, strResult);
	}
	else if ("GetSettings" == strMethodName )
	{
		OnGetSettings(strJsonIn, strResult);
	}
	else if ("SetSettings" == strMethodName )
	{
		OnSetSettings(strJsonIn, strResult);
	}
	else if ("QueryDir" == strMethodName )
	{
		OnQueryDir(strJsonIn, strResult);
	}
	else if ("QueryDirSubCount" == strMethodName )
	{
		QueryDirSubCount(strJsonIn, strResult);
	}
	else if ("QueryLocalOfficeFileType" == strMethodName )
	{
		QueryLocalOfficeFileType(strResult);
	}
	else if ("ReNameItem" == strMethodName )
	{
		ReNameItem(strJsonIn, strResult);
	}
	else if ("DeleteItem" == strMethodName )
	{
		DeleteItem(strJsonIn, strResult);
	}
	else if ("PasteFiles" == strMethodName )
	{
		PasteFiles(strJsonIn, strResult);
	}
	else if ("MoveFiles" == strMethodName )
	{
		MoveFiles(strJsonIn, strResult);
	}
	else if ("SubmitToOutPut" == strMethodName )
	{
		SubmitToOutPut(strJsonIn, strResult);
	}
	else
		strResult = "����δ֪�ӿڣ�";

	ZTools::WriteZToolsFormatLog("------PeraNetDiskIceI::InvokeMethod����------\nstrMethodName: %s\nstrJsonIn: %s\n���������%s", strMethodName.c_str(), strJsonIn.c_str(),strResult.c_str());
	return strResult;
}
