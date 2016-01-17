#include "StdAfx.h"
#include "ResLibBaseComponentData.h"
#include "writer.h"


CResLibBaseComponentData::CResLibBaseComponentData(void)
{
	m_nResType = RT_BASECOMPONET;
}


CResLibBaseComponentData::~CResLibBaseComponentData(void)
{
}

std::string CResLibBaseComponentData::GetSvgJsonStr()
{
	Json::Value jsonOut;
	std::string strJsonParams("{}");
	jsonOut["RobotType"] = Json::Value(m_RobotType);
	Json::FastWriter writer;
	strJsonParams = writer.write(jsonOut);
	return strJsonParams;
}
