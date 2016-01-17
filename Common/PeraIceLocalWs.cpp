#include "StdAfx.h"
#include "PeraIceLocalWs.h"
#include "Tools.h"


CPeraIceLocalWs::CPeraIceLocalWs(void)
{
}

CPeraIceLocalWs::~CPeraIceLocalWs(void)
{
}

std::string CPeraIceLocalWs::GetIceEndPoints()
{
	std::string strEndPoints = ZTools::FormatString("WsICEService:tcp -h %s -p %s", "127.0.0.1", "20135");
	return strEndPoints;
}

std::string CPeraIceLocalWs::InvokeLocalWsMethodOrignal( std::string strMethod, std::string strJson )
{
	std::string strResult;

	if (!InitProxy())
	{
		return strResult;
	}

	try
	{
		strResult = m_pPrx->invokeLocalWsMethod(strMethod, strJson);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
	}

	return strResult;
}

BOOL CPeraIceLocalWs::InvokeLocalWsMethod_Async( std::string strMethod, std::string strJson, AMI_WsICEService_invokeLocalWsMethod_I* pCbObj )
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		ZTools::MBToUTF8(strMethod);
		ZTools::MBToUTF8(strJson);
		bResult = m_pPrx->invokeLocalWsMethod_async(pCbObj, strMethod, strJson);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
	}

	return bResult;
}

BOOL CPeraIceLocalWs::InvokeLocalWsMethodOrignal_Async( std::string strMethod, std::string strJson, AMI_WsICEService_invokeLocalWsMethodOrignal_I* pCbObj )
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		bResult = m_pPrx->invokeLocalWsMethod_async(pCbObj, strMethod, strJson);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
	}

	return bResult;
}

BOOL CPeraIceLocalWs::runLocalRobotLog(std::string strLog)
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		m_pPrx->runLocalRobotLog(strLog);	 
		bResult = TRUE;
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
		bResult = FALSE;
	}

	return bResult;
}

BOOL CPeraIceLocalWs::runLocalRobotRate(std::string strRate)
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		m_pPrx->runLocalRobotRate(strRate);	 
		bResult = TRUE;
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
		bResult = FALSE;
	}

	return bResult;
}

std::string CPeraIceLocalWs::runLocalRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType)
{
	std::string strResult;

	if (!InitProxy())
	{
		return strResult;
	}

	try
	{
		strResult = m_pPrx->runLocalRobotFinished(strTaskId, strOutxml, strRunType);	 	
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
	}

	return strResult;
	return "";
}

BOOL CPeraIceLocalWs::amiRunLocalRobotFinished(std::string strTaskId, std::string strOutxml, std::string strRunType)
{
	BOOL bResult = FALSE;

	if (!InitProxy())
	{
		return bResult;
	}

	try
	{
		bResult = m_pPrx->runLocalRobotFinished_async(new AMI_WsICEService_runLocalRobotFinished_I, strTaskId, strOutxml, strRunType);
	}
	catch(const Ice::Exception& ex)
	{
		ZTools::WriteZToolsFormatLog("ICE异常：%s", ex.ice_name().c_str());
		bResult = FALSE;
	}

	return bResult;
}

void AMI_WsICEService_runLocalRobotFinished_I::ice_response( const ::std::string& str)
{
	std::string strResponse = str;
	ZTools::UTF8ToMB(strResponse);
	ZTools::WriteZToolsFormatLog("Local调用运行组件完成return:%s", strResponse.c_str());
}

void AMI_WsICEService_runLocalRobotFinished_I::ice_exception( const Ice::Exception& ex)
{
	ZTools::WriteZToolsLog(
		ZTools::FormatString("Local调用finish接口出现异常\nice_name:%s", 
		ex.ice_name().c_str())
		);
}

void AMI_WsICEService_invokeLocalWsMethod_I::ice_response( const ::std::string& str)
{
	std::string strResult = str;
	ZTools::UTF8ToMB(strResult);
	CString sResult(strResult.c_str());
	m_cbFunc(sResult);
}

void AMI_WsICEService_invokeLocalWsMethod_I::ice_exception( const Ice::Exception& ex)
{
	ZTools::WriteZToolsFormatLog("ICE异常：%s\nAMI_WsICEService_invokeLocalWsMethod_I::ice_exception", ex.ice_name().c_str());
}

AMI_WsICEService_invokeLocalWsMethod_I::AMI_WsICEService_invokeLocalWsMethod_I( cbInvokeLocalWsMethod cbFunc )
	: m_cbFunc(cbFunc)
{
}

void AMI_WsICEService_invokeLocalWsMethodOrignal_I::ice_response( const ::std::string& str)
{
	std::string strResult = str;
	//ZTools::UTF8ToMB(strResult);
	CString sResult(strResult.c_str());
	m_cbFunc(sResult);
}

void AMI_WsICEService_invokeLocalWsMethodOrignal_I::ice_exception( const Ice::Exception& ex)
{
	ZTools::WriteZToolsFormatLog("ICE异常：%s\nAMI_WsICEService_invokeLocalWsMethodOrignal_I::ice_exception", ex.ice_name().c_str());
}

AMI_WsICEService_invokeLocalWsMethodOrignal_I::AMI_WsICEService_invokeLocalWsMethodOrignal_I( cbInvokeLocalWsMethodOrignal cbFunc )
	: m_cbFunc(cbFunc)
{
}