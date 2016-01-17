#pragma once

//控件ID
enum {
	//ID_ = ,
};



//窗口消息
enum {
	/*
	*/
	//WM_ = (WM_USER + 1),
};



//注册消息
/*
*/
//extern UINT WM_



//CopyData 消息标识
enum{
	/*
	功能：
	*/
	//CPMSG_ = 1
};



//其他全局变量
#define  OperationStatus_Waiting "waiting"
#define  OperationStatus_Normal  "normal"
#define  OperationStatus_Finish  "finish"
#define  OperationStatus_Pause   "pause"
#define  OperationStatus_Stop    "stop"
#define  OperationStatus_Resume  "resume"
#define  OperationStatus_Error   "error"
#define  DefaultReturnJson "{\n\"flag\":\"true\",\n\"msg\":\"\",\n\"errorCode\":0\n}"

//应用程序标题
extern const LPCTSTR g_lpszAppTitle;

#define DBG_LOG_OUTPUTSTRING_DWONLOAD	0
#define TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME	5000
