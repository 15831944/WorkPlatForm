// VorxTime.cpp: implementation of the CVorxTime class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VorxTime.h"
#include <time.h>

namespace vfc
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	
	CVorxTime::CVorxTime()
	{
		memset(this,0,sizeof(CVorxTime));	
	}
	
	CVorxTime::~CVorxTime()
	{
		
	}
	
	bool CVorxTime::operator>(const CVorxTime& tm) const
	{
		if(year != tm.year) return (year>tm.year);
		if(month != tm.month) return (month>tm.month);
		if(day != tm.day) return (day>tm.day);
		if(hour != tm.hour) return (hour>tm.hour);
		if(minute != tm.minute) return (minute>tm.minute);
		if(second != tm.second) return (second>tm.second);
		return false;
	}
	
	bool CVorxTime::operator>=(const CVorxTime& tm) const
	{
		if(year != tm.year) return (year>tm.year);
		if(month != tm.month) return (month>tm.month);
		if(day != tm.day) return (day>tm.day);
		if(hour != tm.hour) return (hour>tm.hour);
		if(minute != tm.minute) return (minute>tm.minute);
		if(second != tm.second) return (second>tm.second);
		return true;	
	}
	
	bool CVorxTime::operator<(const CVorxTime& tm) const
	{
		if(year != tm.year) return (year<tm.year);
		if(month != tm.month) return (month<tm.month);
		if(day != tm.day) return (day<tm.day);
		if(hour != tm.hour) return (hour<tm.hour);
		if(minute != tm.minute) return (minute<tm.minute);
		if(second != tm.second) return (second<tm.second);
		return false;
	}
	
	bool CVorxTime::operator<=(const CVorxTime& tm) const
	{
		if(year != tm.year) return (year<tm.year);
		if(month != tm.month) return (month<tm.month);
		if(day != tm.day) return (day<tm.day);
		if(hour != tm.hour) return (hour<tm.hour);
		if(minute != tm.minute) return (minute<tm.minute);
		if(second != tm.second) return (second<tm.second);
		return true;
	}
	
	bool CVorxTime::operator==(const CVorxTime& tm) const
	{
		if(year != tm.year) return false;
		if(month != tm.month) return false;
		if(day != tm.day) return false;
		if(hour != tm.hour) return false;
		if(minute != tm.minute) return false;
		if(second != tm.second) return false;
		return true;
	}

	bool CVorxTime::operator!=(const CVorxTime& tm) const
	{
		if(year != tm.year) return true;
		if(month != tm.month) return true;
		if(day != tm.day) return true;
		if(hour != tm.hour) return true;
		if(minute != tm.minute) return true;
		if(second != tm.second) return true;
		return false;
	}

	void CVorxTime::SetValue(const CVorxTime& tm)
	{
		year = tm.year;
		month = tm.month;
		day = tm.day;
		hour = tm.hour;
		minute = tm.minute;
		second = tm.second;
	}

	void CVorxTime::GetCurrentTime()
	{
		memset(this,0,sizeof(CVorxTime));
#ifndef WIN32
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep); /*取得当地时间*/
		year = 1900 + p->tm_year;
		month = 1+p->tm_mon;
		day = p->tm_mday;
		hour = p->tm_hour;
		minute = p->tm_min;
		second = p->tm_sec;
#else
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);
		year = curTime.wYear;
		month = curTime.wMonth;
		day = curTime.wDay;
		hour = curTime.wHour;
		minute = curTime.wMinute;
		second = curTime.wSecond;
#endif
		
	}
	
	bool CVorxTime::SetValue(LPCTSTR sTime)
	{
		int nYear,nMonth,nDay,nHour,nMinute,nSecond;
		if(!sTime) return false;
		int nLen = strlen(sTime);

		int nFields = 0;
		if(nLen == 19)
			nFields = sscanf(sTime,"%d-%d-%d %d:%d:%d",&nYear,&nMonth,&nDay,&nHour,&nMinute,&nSecond);
		else if(nLen == 14)
			nFields = sscanf(sTime,"%04d%02d%02d%02d%02d%02d",&nYear,&nMonth,&nDay,&nHour,&nMinute,&nSecond);

		return (nFields==6 && SetValue(nYear,nMonth,nDay,nHour,nMinute,nSecond));
	}

	bool CVorxTime::SetValue(int nYea,int nMon,int nDay,int nHou,int nMin,int nSec)
	{
		if(nMon>0 && nMon<13 && nDay>0 && nDay<32 && nHou>-1 && nHou<60 && nMin>-1 && nMin<60 && nSec>-1 && nSec<60)
		{
			year = nYea;
			month = nMon;
			day = nDay;
			hour = nHou;
			minute = nMin;
			second = nSec;
			return TRUE;
		}
		return FALSE;
	}

	int CVorxTime::operator-(const CVorxTime& tm) const
	{
		return (int)(GetTime()-tm.GetTime());
	}

	CVorxTime CVorxTime::operator-(const int tm) const
	{
		CVorxTime ret;
		ret.SetTime(GetTime()-tm);
		return ret;
	}

	CVorxTime CVorxTime::operator+(const int tm) const
	{
		CVorxTime ret;
		ret.SetTime(GetTime()+tm);
		return ret;
	}
	
	void CVorxTime::ToString(char* sRet,bool bSeparate)
	{
		if(bSeparate)
			sprintf(sRet,"%04d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second);
		else
			sprintf(sRet,"%04d%02d%02d%02d%02d%02d",year,month,day,hour,minute,second);
	}

	time_t CVorxTime::GetTime() const
	{
		struct tm val;
		val.tm_year = year-1900;
		val.tm_mon = month-1;
		val.tm_mday = day;
		val.tm_hour = hour;
		val.tm_min = minute;
		val.tm_sec = second;
		val.tm_isdst = -1;
		return mktime(&val);
	}

	void CVorxTime::SetTime(time_t time)
	{
		struct tm* pVal = localtime(&time);
		if(pVal)
		{
			year = pVal->tm_year+1900;
			month = pVal->tm_mon+1;
			day = pVal->tm_mday;
			hour = pVal->tm_hour;
			minute = pVal->tm_min;
			second = pVal->tm_sec;
		}
	}
}
