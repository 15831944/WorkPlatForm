#pragma once
#include "StdAfx.h"
////////////////////////////////////////日志头文件及宏定义
#define LOG4CPLUS_DISABLE_DLL_RUNTIME_WARNING
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;

#define FILENAME __FILE__
#define LINENUM __LINE__
//#define STRINGIFY(x) #x
//#define TOSTRING(x) STRINGIFY(x)
//#define LINENUM TOSTRING(__LINE__)

#define LOG_TRACE(logger,logEvent) LOG4CPLUS_TRACE(logger, logEvent)
#define LOG_DEBUG(logger,logEvent) LOG4CPLUS_DEBUG(logger, logEvent)
#define LOG_INFO(logger,logEvent) LOG4CPLUS_INFO(logger, logEvent)
#define LOG_WARN(logger,logEvent) LOG4CPLUS_WARN(logger, logEvent)
#define LOG_ERROR(logger,logEvent) LOG4CPLUS_ERROR(logger, logEvent)
#define LOG_FATAL(logger,logEvent) LOG4CPLUS_FATAL(logger, logEvent)

class ClogMgr
{
public:
	ClogMgr();
	static bool ReadCfg();
	static bool WriteCfg();
public:
	static void WriteLog(LPCTSTR LogMessage,LPCTSTR strFile = _T(""),int LineNum = 0,int LogLevel = INFO_LOG_LEVEL);
private:
	static int m_Level;
	char* itoa(int nNumber);
};