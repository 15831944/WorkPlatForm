#include "StdAfx.h"
#include "WsICEServiceI.h"
#include "Tools.h"


WsICEServiceI::WsICEServiceI(void)
{
}


WsICEServiceI::~WsICEServiceI(void)
{
}

::std::string WsICEServiceI::runRobotFinished( const ::std::string& strTaskId, const ::std::string& strOutxmlPath, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
	std::string strMyTaskId(strTaskId);
	std::string strMyOutxmlPath(strOutxmlPath);
	ZTools::WriteZToolsFormatLog("本地虚拟接收者：%s,%s", ZTools::UTF8ToMB(strMyTaskId).c_str(), ZTools::UTF8ToMB(strMyOutxmlPath).c_str());
	return "";
}

::std::string WsICEServiceI::runRobotFinished(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& /*= ::Ice::Current() */)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void WsICEServiceI::runRobotLog( const ::std::string& str, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
	std::string strMy(str);
	ZTools::WriteZToolsFormatLog("本地虚拟接收者：%s", ZTools::UTF8ToMB(strMy).c_str());
}

void WsICEServiceI::runRobotRate( const ::std::string& str, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
	std::string strMy(str);
	ZTools::WriteZToolsFormatLog("本地虚拟接收者：%s", ZTools::UTF8ToMB(strMy).c_str());
}

::std::string WsICEServiceI::invokeWsMethod( const ::std::string& strMethod, const ::std::string& strJson, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
	std::string strMethod1(strMethod);
	std::string strJson1(strJson);
	ZTools::WriteZToolsFormatLog("本地虚拟接收者：invokeWsMethod >>>>>> %s, %s", ZTools::UTF8ToMB(strMethod1).c_str(), ZTools::UTF8ToMB(strJson1).c_str());
	return "another json";
}

::std::string WsICEServiceI::invokeLocalWsMethod( const ::std::string& strMethod, const ::std::string& strJson, const ::Ice::Current& /*= ::Ice::Current( ) */ )
{
	std::string strMethod1(strMethod);
	std::string strJson1(strJson);
	ZTools::WriteZToolsFormatLog("本地虚拟接收者：invokeLocalWsMethod >>>>>> %s, %s", ZTools::UTF8ToMB(strMethod1).c_str(), ZTools::UTF8ToMB(strJson1).c_str());
	return "another json";
}

::std::string WsICEServiceI::runLocalRobotFinished(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& /*= ::Ice::Current() */)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void WsICEServiceI::runLocalRobotLog(const ::std::string&, const ::Ice::Current& /*= ::Ice::Current() */)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void WsICEServiceI::runLocalRobotRate(const ::std::string&, const ::Ice::Current& /*= ::Ice::Current() */)
{
	throw std::logic_error("The method or operation is not implemented.");
}

::std::string WsICEServiceI::savePlanByByteXpdl(const ::std::string& strJson, const ::com::pera::base::runtime::remote::ByteSeq& bs, const ::Ice::Current& /*= ::Ice::Current() */)
{
	std::string strMy(strJson);
	ZTools::WriteZToolsFormatLog("本地虚拟接收者：%s", ZTools::UTF8ToMB(strMy).c_str());
	return "ok";
}
