#pragma once
#include "AttachWindow.h"
#include "WebContainerCmdLine.h"

/*
�ͻ��˽��̴������ʱ������ ResponseAttachProcessWnd
������������������������������ WM_PROCESS_WND_ATTACHED ����Ӧ����
*/
extern void ResponseAttachProcessWnd( HWND hClientWnd );

extern CAttachWindow g_aw;