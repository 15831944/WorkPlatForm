#include "stdafx.h"
#include "CallRunTimeIce.h"
#include "Tools.h"

#include "PeraIceRunRobot.h"

static CPeraIceRunRobot s_peraIceRunRobot;

BOOL CallRunRobot( std::string strInfo, int nProcessId/* = 0*/ )
{
	ZTools::WriteZToolsFormatLog("CallRunRobot\n%s", strInfo.c_str());

	ZTools::MBToUTF8(strInfo);
	BOOL bResult = s_peraIceRunRobot.RunRobot(strInfo, nProcessId);

	return bResult;
}