#include "stdafx.h"
#include <string>
#include <atlbase.h>
using namespace ATL;

CString OldCrBotFile()
{
	CString sRegResult;

	HKEY hKeyParent = HKEY_CLASSES_ROOT;
	CString sRegKeyPath = "BOT_FileProgID\\Shell\\Open\\Command";
	CString sRegKey = "";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024];
	memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	if (!sRegResult.IsEmpty())
	{
		CString sBinPath = sRegResult.Left(sRegResult.ReverseFind('\\'));
		sBinPath.Trim("\"");
		return sBinPath;
	}

	return sRegResult;
}

CString OldCuFlexware()
{
	CString sRegResult;

	HKEY hKeyParent = HKEY_CURRENT_USER;
	CString sRegKeyPath = "Software\\Peraglobal\\Flexware";
	CString sRegKey = "FlexwareBinDir";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024];
	memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	if (!sRegResult.IsEmpty())
	{
		return sRegResult;
	}
	return sRegResult;
}

CString OldEnvFree()
{
	CString sRegResult;

	TCHAR newEnv[MAX_PATH +1];
	DWORD res = ::GetEnvironmentVariable( "FREE", newEnv, MAX_PATH );
	if( res > 0 )
	{
		sRegResult.Empty();
		sRegResult = newEnv;
		return sRegResult;
	}
	return sRegResult;
}

CString CrBotFile()
{
	CString sRegResult;

	HKEY hKeyParent = HKEY_CLASSES_ROOT;
	CString sRegKeyPath = "BOT_FileProgID\\Shell\\Open\\Command";
	CString sRegKey = "";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024];
	memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath, KEY_READ)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	if (!sRegResult.IsEmpty())
	{
		CString sBinPath = sRegResult.Left(sRegResult.ReverseFind('\\'));
		sBinPath.Trim("\"");
		return sBinPath;
	}

	return sRegResult;
}

CString CuFlexware()
{
	CString sRegResult;

	HKEY hKeyParent = HKEY_CURRENT_USER;
	CString sRegKeyPath = "Software\\Peraglobal\\Flexware";
	CString sRegKey = "FlexwareBinDir";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024];
	memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath, KEY_READ)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	if (!sRegResult.IsEmpty())
	{
		return sRegResult;
	}
	return sRegResult;
}

CString LmFlexware()
{
	CString sRegResult;

	HKEY hKeyParent = HKEY_LOCAL_MACHINE;
	CString sRegKeyPath = "Software\\Peraglobal\\Flexware";
	CString sRegKey = "FlexwareBinDir";

	CRegKey rk;
	DWORD pCount = 1024;
	std::string strKeyValue;
	char szValue[1024];
	memset(szValue, 0, 1024);

	if(rk.Open(hKeyParent, sRegKeyPath, KEY_READ)== ERROR_SUCCESS)
	{
		if(rk.QueryStringValue(sRegKey, szValue, &pCount)== ERROR_SUCCESS)
		{
			sRegResult = szValue;
		}
	}
	rk.Close();

	if (!sRegResult.IsEmpty())
	{
		return sRegResult;
	}
	return sRegResult;
}

CString FromCom()
{
	CComPtr<IDispatch> pDisp;
	HRESULT hr;
	CComVariant outVal;
	HRESULT hr1 = ::CoInitialize(NULL);
	hr = pDisp.CoCreateInstance(L"GetPath.GetPathInterface", NULL, CLSCTX_INPROC_SERVER);    
	if(SUCCEEDED(hr))
	{
		pDisp.Invoke0(L"GetDllPath", &outVal);
	}
	if (SUCCEEDED(hr1))
	{
		::CoUninitialize();
	}

	if (outVal.vt == VT_BSTR)
	{
		//CString sRegResult(outVal.bstrVal);
		CString sRegResult;
		//lc,国际化乱码问题
		sRegResult = (LPCTSTR)_bstr_t(outVal.bstrVal);
		return sRegResult;
	}

	return "";
}

CString OldMethod()
{
	CString str;
	{
		CString sResult = OldCrBotFile();
		if (sResult) str = sResult;
	}

	{
		CString sResult = OldCuFlexware();
		if (sResult) str = sResult;
	}

	{
		CString sResult = OldEnvFree();
		if (sResult) str = sResult;
	}
	return str;
}

CString TestAll()
{
	CString str;
	{
		CString sResult = OldMethod();
		if (sResult) str = sResult;
	}

	{
		CString sResult = CrBotFile();
		if (sResult) str = sResult;
	}

	{
		CString sResult = CuFlexware();
		if (sResult) str = sResult;
	}

	{
		CString sResult = LmFlexware();
		if (sResult) str = sResult;
	}

	{
		CString sResult = FromCom();
		if (sResult) str = sResult;
	}
	return str;
}

CString GetFlexwareBinPathMethod()
{
	CString str;
	{
		CString sResult = LmFlexware();
		if (sResult) str = sResult;
	}
	
	if (str.IsEmpty())
	{
		CString sResult = CrBotFile();
		if (sResult) str = sResult;
	}

	if (str.IsEmpty())
	{
		CString sResult = CuFlexware();
		if (sResult) str = sResult;
	}

	if (str.IsEmpty())
	{
		CString sResult = OldMethod();
		if (sResult) str = sResult;
	}

	if (str.IsEmpty())
	{
		CString sResult = FromCom();
		if (sResult) str = sResult;
	}
	return str;
}

CString GetGRCRunnerPath()
{
	CString sPath = GetFlexwareBinPathMethod() + "\\PeraComponentRunner.exe";
	if (PathFileExists(sPath) != TRUE)
	{
		sPath.Empty();
	}
	return sPath;	
}

CString GetPeraComponentPath()
{
	CString sPath = GetFlexwareBinPathMethod() + "\\PeraComponent.exe";
	if (PathFileExists(sPath) != TRUE)
	{
		sPath.Empty();
	}
	return sPath;	
}

CString GetPeraComponentEditorPath()
{
	CString sPath = GetFlexwareBinPathMethod() + "\\PeraComponentEditor.exe";
	if (PathFileExists(sPath) != TRUE)
	{
		sPath.Empty();
	}
	return sPath;	
}


