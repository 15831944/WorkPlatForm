#pragma once

//�ؼ�ID
enum {
	//ID_ = ,
};



//������Ϣ
enum {
	/*
	*/
	//WM_ = (WM_USER + 1),
};



//ע����Ϣ
/*
*/
//extern UINT WM_



//CopyData ��Ϣ��ʶ
enum{
	/*
	���ܣ�
	*/
	//CPMSG_ = 1
};



//����ȫ�ֱ���
#define  OperationStatus_Waiting "waiting"
#define  OperationStatus_Normal  "normal"
#define  OperationStatus_Finish  "finish"
#define  OperationStatus_Pause   "pause"
#define  OperationStatus_Stop    "stop"
#define  OperationStatus_Resume  "resume"
#define  OperationStatus_Error   "error"
#define  DefaultReturnJson "{\n\"flag\":\"true\",\n\"msg\":\"\",\n\"errorCode\":0\n}"

//Ӧ�ó������
extern const LPCTSTR g_lpszAppTitle;

#define DBG_LOG_OUTPUTSTRING_DWONLOAD	0
#define TIMEOUT_PERSIST_DOWNLOAD_THREAD_INIT_TIME	5000
