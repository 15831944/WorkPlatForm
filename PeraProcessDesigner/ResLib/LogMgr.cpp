#include "StdAfx.h"
#include "LogMgr.h"
#include <locale.h>
#include "tinyxml.h"

int ClogMgr::m_Level = OFF_LOG_LEVEL;

ClogMgr::ClogMgr()
{

}

char* ClogMgr::itoa(int nNumber)
{
	char *strRtn = (char*)malloc(10);
	if (strRtn) sprintf(strRtn,"%d",nNumber);
	return strRtn;
}

bool ClogMgr::ReadCfg()
{
	WIN32_FIND_DATA findFileData;

	//////////////////////////////////////
	char szFileName [MAX_PATH];

	::GetModuleFileName(NULL,szFileName,MAX_PATH);

	CString strXmlPath;

	strXmlPath.Format("%s",szFileName);

	strXmlPath = strXmlPath.Left(strXmlPath.ReverseFind('\\') + 1);
	//////////////////////////////////////
	
	strXmlPath += _T("Config.xml");

	HANDLE hHandle = FindFirstFile(strXmlPath, &findFileData); 

	if (hHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	TiXmlDocument m_XmlDoc;
	const char* strLevel;
	m_XmlDoc.LoadFile(strXmlPath);
	m_XmlDoc.Parse(strXmlPath);
	TiXmlElement* rootElement = m_XmlDoc.RootElement(); //Config
	TiXmlElement* LogManageElement = rootElement->FirstChildElement(); //LogManager
	TiXmlElement* LogLevelElement = LogManageElement->FirstChildElement(); //LogLevel

	while ( LogLevelElement ) {
		TiXmlAttribute* attributeOfLogLevel = LogLevelElement->FirstAttribute(); //获得LogLevel的属性
		while ( attributeOfLogLevel ) {
			//std::cout << attributeOfStudent->Name() << " : " << attributeOfStudent->Value() << std::endl;
			attributeOfLogLevel = attributeOfLogLevel->Next();
		}
		
		strLevel = LogLevelElement->GetText();
		LogLevelElement = LogLevelElement->NextSiblingElement();
	}

	m_Level = atoi(strLevel);
	return true;
}

bool ClogMgr::WriteCfg()
{
	return true;
}

void ClogMgr::WriteLog(LPCTSTR LogMessage,LPCTSTR strFile,int LineNum,int LogLevel)
{
	setlocale(LC_ALL,"");

	if(!ReadCfg())
	{
		return;
	}

	if(m_Level == OFF_LOG_LEVEL)
	{
		return;
	}

	char szFileName [MAX_PATH];

	::GetModuleFileName(NULL,szFileName,MAX_PATH);

	CString strPath;

	strPath.Format("%s",szFileName);

	strPath = strPath.Left(strPath.ReverseFind('\\') + 1);

	strPath += _T("ReslibMgr.log");

	char buffer [MAX_PATH]; 

	SYSTEMTIME s; 
	GetLocalTime(&s); 

	sprintf(buffer,"[%d-%d-%d %d:%d:%d]",s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond); 

	CString strTm = buffer;

	CString strFileName,strNum,strPattern;

	if(strFile == _T("") || LineNum == 0)
	{
		strNum.Format("%d",LineNum);

		strFileName.Format("[%s:%s]",strFile,strNum);

		strPattern = strTm + " [%p] [PID:%t] - %m%n";
	}
	else
	{
		strNum.Format("%d",LineNum);

		strFileName.Format("[%s:%s]",strFile,strNum);

		strPattern = strTm + " [%p] [PID:%t] - %m " + strFileName + "%n";
	}
	helpers::LogLog::getLogLog()->setInternalDebugging(true);

	SharedAppenderPtr _appendPtr(new RollingFileAppender(LOG4CPLUS_TEXT(strPath.GetBuffer(0)), 200*1024, 5));

	_appendPtr->setName(LOG4CPLUS_TEXT("First"));

	_appendPtr->setLayout( std::auto_ptr<Layout>(new PatternLayout(strPattern.GetBuffer(0))));

	Logger::getRoot().addAppender(_appendPtr);

	Logger root = Logger::getRoot();

	Logger test = Logger::getInstance(LOG4CPLUS_TEXT("test"));

	//test.setLogLevel(m_Level);

	Logger subTest = Logger::getInstance(LOG4CPLUS_TEXT("test.subtest"));

	subTest.setLogLevel(m_Level);

	switch(LogLevel)
	{
	case NOT_SET_LOG_LEVEL:
		break;
	case TRACE_LOG_LEVEL:
		LOG_TRACE(subTest,LogMessage);
		break;
	case DEBUG_LOG_LEVEL:
		LOG_DEBUG(subTest,LogMessage);
		break;
	case INFO_LOG_LEVEL:
		LOG_INFO(subTest,LogMessage);
		break;
	case WARN_LOG_LEVEL:
		LOG_WARN(subTest,LogMessage);
		break;
	case ERROR_LOG_LEVEL:
		LOG_ERROR(subTest,LogMessage);
		break;
	case FATAL_LOG_LEVEL:
		LOG_FATAL(subTest,LogMessage);
		break;
	default:
		break;
	}

	Logger::getRoot().shutdown();
}