#pragma once
#include "CxWebPageMgr.h"

/*
使用说明：
添加一个模块的步骤：
1、在 CPeraWebPageMgr 中，添加 FW_ 开头的模块标识。添加 FE_ 开头的事件标识。
2、在 PeraWebPageEventExData 文件中添加数据类。
3、在 _OnNewEventExData 中 new 事件对应的数据类对象。
4、CPeraWebPageMgrHost::Require 中添加【客户端】给【WebPage进程】发起的事件。
5、CPeraWebPageMgrHost::OnWebPageEvent 中添加【WebPage进程】给【客户端】发起或响应的事件。
6、CPeraWebPageMgrClient::Require 中添加【WebPage进程】给【客户端】发起的事件。
7、CPeraWebPageMgrClient::OnWebPageEvent 中添加【客户端】给【WebPage进程】发起或响应的事件。
*/
class CPeraWebPageMgr :
	public CCxWebPageMgr
{
public:
	//WebPage标志---------------------
	enum{
		FW_TABVIEW_TASK_MODEL = 1          //
	};

	//事件代码---------------------
	//第一个从 FE_CUSTOM + 1 开始
	enum{
		//运行子流程WebPage
//		FE_RUNFLOW_CANCEL = FE_CUSTOM + 1,    //客户端通知svg，取消、结束运行
//		FE_RUNFLOW_FINISHED, //svg通知客户端，后台运行结束


		FE_END
	};

public:
	CPeraWebPageMgr(void);
	virtual ~CPeraWebPageMgr(void);
	virtual CString GetEventDisplay( CCxWebPageEvent * pEvent );
//	BOOL NavigateInNewTab( LPCTSTR lpszUrl, int nWebPageFlag );
protected:
	virtual CCxWebPageEventExData* _OnNewEventExData( int nModuleFlag, int nEventFlag );
	
};
