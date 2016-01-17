#include "stdafx.h"
#include "FilePro.h"
#include "Tools.h"
#include <algorithm>
#include "ClassExtract.h"

extern BOOL SaveIcon(HICON Icon, LPCSTR FileName, int nBit);
extern BOOL SavePngFile(HICON Icon, LPCSTR strPngFile,int iOutWith = -1,int iOutHeight = -1);
/************************************************************************/
/* 根据扩展名，从注册表读取图标位置和索引值                                      */
/************************************************************************/
bool GetExeDefaultIconPath(const char *pszExt,string & strIconFile, int &iConIndex,bool &bIsIConFile)
{
	HKEY  keyExt,keyFinal;
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CLASSES_ROOT,pszExt,&keyExt))
		return false;

	DWORD dwType;
	DWORD dwSize = 100;
	char  szString[100] = {0};

	LONG lReturn = ::RegQueryValueEx (keyExt,"", NULL,&dwType, (BYTE *) szString, &dwSize);
	if (lReturn!=ERROR_SUCCESS)
	{
		RegCloseKey( keyExt);
		return false;
	}

	RegCloseKey( keyExt);
	string strSubKey = szString;
	strSubKey += "\\DefaultIcon";
	if(ERROR_SUCCESS!=RegOpenKey(HKEY_CLASSES_ROOT,strSubKey.c_str(),&keyFinal))
		return false;

	dwSize = 1000;
	char  szStringFinal[1000] = {0};
	lReturn = ::RegQueryValueEx (keyFinal,"", NULL,&dwType, (BYTE *) szStringFinal, &dwSize);
	RegCloseKey( keyFinal);
	if (lReturn!=ERROR_SUCCESS)
		return false;

	strIconFile = szStringFinal;
	int iPos = strIconFile.find_last_of(',');
	if (iPos==string::npos) // 未找到，返回的直接是icon文件
	{
		char szBuf[1000]={0};
		DWORD iR = ExpandEnvironmentStrings(strIconFile.c_str(),szBuf,1000);
		if (szBuf[0]=='"' || szBuf[0]== '“')
		{
			strIconFile = szBuf+1;
			strIconFile[strIconFile.length()-1] =0;
		}
		else
			strIconFile = szBuf;

		iConIndex = -99999;
		bIsIConFile = true;
		return true;
	}
	string strIndex = strIconFile.substr(iPos+1);
	iConIndex =  atoi(strIndex.c_str());

	strIconFile = strIconFile.substr(0,strIconFile.find_last_of(','));
	char szBuf[1000]={0};
	DWORD iR = ExpandEnvironmentStrings(strIconFile.c_str(),szBuf,1000);

	if (szBuf[0]=='"' || szBuf[0]== '“')
	{
		strIconFile = szBuf+1;
		strIconFile[strlen(szBuf)-2] =0;
	}
	else
		strIconFile = szBuf;

	bIsIConFile = false;
	return true;
}

UINT GetExeOrDllIconCount(LPCTSTR strFile)
{
	return ExtractIconEx(strFile,-1,NULL,NULL,0);
}
string GetFileNameNoExt(LPCTSTR strFile)
{
	string strName = strFile;
	std::replace(strName.begin(),strName.end(),'/','\\');
	int iPos = strName.find(':'); // 看是不是绝对路径
	if (iPos == string::npos) // 只有文件名
	{
		int iDotPos = strName.find_last_of(".");
		if (iDotPos==string::npos || iDotPos<1)
			return strFile;

		strName = strName.substr(0,iDotPos);
	}
	else
	{
		strName = strName.substr(strName.find_last_of("\\") +1);// 是绝对路径
		strName = strName.substr(0,strName.find_last_of('.'));
	}
	return strName;
}
/************************************************************************/
/*从exe或者dll或者icon文件提取指定大小的图标，并保存成png格式                   */
/************************************************************************/
bool ExtractIndifySizeIcon(LPCTSTR strExeFile,LPCTSTR strNameNoExt,UINT iSizeBig,UINT iSizeSmall,int iIndexICon = 0)
{
	HICON hIconArry[2];
	UINT uSize = (iSizeSmall<<16) + iSizeBig;
	BOOL bok1 = false;
	BOOL bok2 = false;
	CString strNamePart;
	strNamePart.Format("_%d.png",iSizeBig);
	string strPngFileBig     = string(strNameNoExt)  + (LPCTSTR)strNamePart;
	strNamePart.Format("_%d.png",iSizeSmall);
	string strPngFileSmall   = string(strNameNoExt)  + (LPCTSTR)strNamePart;

	HRESULT hr = SHDefExtractIcon(strExeFile,iIndexICon,0,hIconArry,NULL,uSize); //提取大图标
	if (S_OK == hr)
	{
		bok1  = SavePngFile(hIconArry[0],strPngFileBig.c_str());
		if (!bok1)
			ZTools::WriteZToolsFormatLog("保存png文件【%s】失败！",strPngFileBig.c_str());
	}
	else if (hr == S_FALSE)
	{
		ZTools::WriteZToolsFormatLog("从文件【%s】中获取索引为【%d】,尺寸为【%d】的图标失败:The requested icon is not present.",
			strExeFile,iIndexICon,iSizeBig);
	}
	else if (hr == E_FAIL)
	{
		ZTools::WriteZToolsFormatLog("从文件【%s】中获取索引为【%d】,尺寸为【%d】的图标失败:The file cannot be accessed, or is being accessed through a slow link.",
			strExeFile,iIndexICon,iSizeBig);
	}

	hr = SHDefExtractIcon(strExeFile,iIndexICon,0,NULL,&hIconArry[1],uSize);
	if (S_OK == hr)
	{
		bok2  = SavePngFile(hIconArry[1],strPngFileSmall.c_str());
		if (!bok2)
			ZTools::WriteZToolsFormatLog("保存png文件【%s】失败！",strPngFileSmall.c_str());
	}
	else if (hr == S_FALSE)
	{
		ZTools::WriteZToolsFormatLog("从文件【%s】中获取索引为【%d】,尺寸为【%d】的图标失败:The requested icon is not present.",
			strExeFile,iIndexICon,iSizeSmall);
	}
	else if (hr == E_FAIL)
	{
		ZTools::WriteZToolsFormatLog("从文件【%s】中获取索引为【%d】,尺寸为【%d】的图标失败:The file cannot be accessed, or is being accessed through a slow link.",
			strExeFile,iIndexICon,iSizeSmall);
	}

	if (bok1 && !bok2) //保存小图标,用大图标拉伸
	{
		bok2 = SavePngFile(hIconArry[0],strPngFileSmall.c_str(),iSizeSmall,iSizeSmall);
		DestroyIcon(hIconArry[0]);
	}
	else if (!bok1 && bok2) //保存大图标,用小图标拉伸
	{
		bok1 = SavePngFile(hIconArry[1],strPngFileBig.c_str(),iSizeBig,iSizeBig);
		DestroyIcon(hIconArry[1]);
	}

	if (bok1 && bok2) // 大小图标都提取保存失败
	{
		DestroyIcon(hIconArry[0]);
		DestroyIcon(hIconArry[1]);
		return true;
	}

	if (!bok1 && !bok2) // 大小图标都提取保存失败
		return false;

	return false;
}
/************************************************************************/
/*  从exe或者dll或者icon文件提取系统默认大小的图标，并保存成icon和png格式                                                                    */
/************************************************************************/
bool ExtractExeDllIcon(LPCTSTR strExeFile,LPCTSTR strSaveDir,bool bExtractLarge = false ,UINT iIndexICon = 0)
{
	HICON hIconArry[1]={0};
	UINT iSuccessCount = 0;
	if (bExtractLarge)
		iSuccessCount = ExtractIconEx(strExeFile,iIndexICon,hIconArry,NULL,1);
	else
		iSuccessCount = ExtractIconEx(strExeFile,iIndexICon,NULL,hIconArry,1);

	if (iSuccessCount!=1)
	{
		ZTools::WriteZToolsFormatLog("从文件【%s】中获取索引为【%d】的图标失败！",strExeFile,iIndexICon);
		return false;
	}

	string strName  = GetFileNameNoExt(strExeFile);

	string strPngFile  = string(strSaveDir) + "\\" + strName + ".png";
	string strIconFile = string(strSaveDir) + "\\" + strName + ".ico";

	BOOL bok1  = SavePngFile(hIconArry[0],strPngFile.c_str());
	if (!bok1)
	{
		ZTools::WriteZToolsFormatLog("保存png文件【%s】失败！",strPngFile.c_str());
	}
	BOOL bok2  =  SaveIcon(hIconArry[0],strIconFile.c_str(),32);
	if (!bok2)
	{
		ZTools::WriteZToolsFormatLog("保存icon文件【%s】失败！",strIconFile.c_str());
	}

	DestroyIcon(hIconArry[0]);
	if (bok1 && bok2)
		return true;

	return false;
}
/************************************************************************/
/* 获取指定扩展名的系统图标，保存成png和icon格式                     */
/************************************************************************/
bool GetTypeIconToFile(const char * pszExt,const char * pszOutDir,bool bLargeIcon = true)
{
	static HRESULT hr = S_FALSE;
	if (hr!=S_OK)
	{
		hr =  CoInitialize(NULL); //如果不初始化COM,那么调用该函数就无法得到.htm/.mht/.xml文件的图标。
	}


	if (pszExt==NULL && pszOutDir==NULL)
		return false;

	if (strlen(pszExt) < 2  || pszExt[0] !='.')
		return false;

	char szExt[30] ={0};
	strcpy_s(szExt,pszExt+1);

	string strDir = string(pszOutDir) + "\\" + szExt  + "\\";
	if (!MakeSureDirectoryPathExists(strDir.c_str()))
		return false;

	string strIconFile = strDir + szExt + ".ico";
	string strPngFile = strDir + szExt  + ".png";

	if(PathFileExists(strIconFile.c_str()))
		remove(strIconFile.c_str());

	if(PathFileExists(strPngFile.c_str()))
		remove(strPngFile.c_str());

	string sDummyFileName = string("foo.") + szExt;

	SHFILEINFO shfi;
	memset(&shfi,0,sizeof(shfi));

	UINT uFlags = SHGFI_ICON|SHGFI_USEFILEATTRIBUTES;
	if (bLargeIcon)
		uFlags|=SHGFI_LARGEICON; 
	else
		uFlags|=SHGFI_SMALLICON; 

	SHGetFileInfo(sDummyFileName.c_str(),FILE_ATTRIBUTE_NORMAL,&shfi,sizeof(shfi),uFlags);

	BOOL bok1  = SavePngFile(shfi.hIcon,strPngFile.c_str());
	if (!bok1)
	{
		ZTools::WriteZToolsFormatLog("保存png文件【%s】失败！",strPngFile.c_str());
	}
	BOOL bok2  =  SaveIcon(shfi.hIcon,strIconFile.c_str(),32);
	if (!bok2)
	{
		ZTools::WriteZToolsFormatLog("保存icon文件【%s】失败！",strIconFile.c_str());
	}

	DestroyIcon(shfi.hIcon);
	if (bok1 && bok2)
		return true;

	return false;
}
void CFilePro::GetAllNewItemsIconFile(vector<NewType *> &vAllItems,CString strDestDir)
{
	string strIconFile;
	int iIndex = 0;
	bool bIsIconFile = false;
	for (size_t i = 0; i< vAllItems.size();i++)
	{
		bool bok1 = GetExeDefaultIconPath(vAllItems[i]->m_strExt.c_str(),strIconFile,iIndex,bIsIconFile);
		CString strDirOutIcoFile = strDestDir + "\\";
		CString strEx = vAllItems[i]->m_strExt.c_str() +1;
		CString strNameNoExt = strDirOutIcoFile + strEx + "\\" + strEx;
		strDirOutIcoFile = strNameNoExt + ".ico";
		MakeSureDirectoryPathExists(strDirOutIcoFile);
		if (!bIsIconFile && !strIconFile.empty()) // exe or dll
		{
			// 保存ICON文件
			CExtract Extract(strIconFile.c_str());
			BOOL BOK = Extract.ExtractIcon(iIndex,strDirOutIcoFile);
			if (Extract.m_bUseDefaultIcon)
			{
				char szPath[1024] ={0};
				ExpandEnvironmentStrings("%SystemRoot%\\system32\\shell32.dll",szPath,1024);
				bool bOK2 = ExtractIndifySizeIcon(szPath,strNameNoExt,32,16,0);
				bool bOK3 = ExtractIndifySizeIcon(szPath,strNameNoExt,64,32,0);
				continue;
			}
		}
		else
		{
			if (bIsIconFile && !strIconFile.empty())
			{
				::CopyFile(strIconFile.c_str(),strDirOutIcoFile,FALSE);
			}
			else
			{
				GetTypeIconToFile(vAllItems[i]->m_strExt.c_str(),strDestDir,true);
				strIconFile = strDirOutIcoFile;
			}
		}

		//保存PNG文件
		bool bOK2 = ExtractIndifySizeIcon(strIconFile.c_str(),strNameNoExt,64,32,iIndex);
		bool bOK3 = ExtractIndifySizeIcon(strIconFile.c_str(),strNameNoExt,128,88,iIndex);
	}
}