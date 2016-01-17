// VorxTime.h: interface for the CVorxTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VORXTIME_H__44F12EB7_C892_404A_BF68_253E20B7D575__INCLUDED_)
#define AFX_VORXTIME_H__44F12EB7_C892_404A_BF68_253E20B7D575__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MacroDefine.h"

namespace vfc
{
	class CVorxTime 
	{
	public:
#ifdef BIG_EDIAN
		UINT32 year:12;								//年
		UINT32 month:4;								//月
		UINT32 day:5;								//日
		UINT32 hour:5;								//时
		UINT32 minute:6;							//分
		
		UINT32 second:6;							//秒
		UINT32 enable:1;							//是否有效
		UINT32 used:1;								//是否使用
		UINT32 cover:1;								//是否覆盖
		UINT32 reserved:23;							//保留
#else
		UINT32 minute:6;							//分
		UINT32 hour:5;								//时
		UINT32 day:5;								//日
		UINT32 month:4;								//月
		UINT32 year:12;								//年
		
		UINT32 reserved:23;							//保留
		UINT32 cover:1;								//是否覆盖
		UINT32 used:1;								//是否使用
		UINT32 enable:1;							//是否有效
		UINT32 second:6;							//秒
#endif
		
	public:
		bool operator>(const CVorxTime& tm) const;	//时间比较运算符重载
		bool operator>=(const CVorxTime& tm) const;
		bool operator<(const CVorxTime& tm) const;
		bool operator<=(const CVorxTime& tm) const;
		bool operator==(const CVorxTime& tm) const;
		bool operator!=(const CVorxTime& tm) const;
		int operator-(const CVorxTime& tm) const;
		CVorxTime operator-(const int tm) const;
		CVorxTime operator+(const int tm) const;

		void SetValue(const CVorxTime& tm);
		bool SetValue(LPCTSTR sTime);
		bool SetValue(int nYear,int nMon,int nDay,int nHour,int nMin,int nSec);

		time_t GetTime() const;
		void SetTime(time_t tm);

		void GetCurrentTime();						//取当前时间
		void ToString(char* sRet,bool bSeparate);	//转换成字符串
		bool IsValid(){return year>0;}
		
		CVorxTime();
		~CVorxTime();
	};
	typedef CVorxTime CVorxIndex;
}
#endif // !defined(AFX_VORXTIME_H__44F12EB7_C892_404A_BF68_253E20B7D575__INCLUDED_)
