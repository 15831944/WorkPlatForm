#pragma once
#include "AttachWindow.h"
#include "WebContainerCmdLine.h"

/*
客户端进程创建完成时，调用 ResponseAttachProcessWnd
吸附后，如需后续处理，在主窗口中添加 WM_PROCESS_WND_ATTACHED 的响应函数
*/
extern void ResponseAttachProcessWnd( HWND hClientWnd );

extern CAttachWindow g_aw;