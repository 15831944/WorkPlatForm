#include "StdAfx.h"
#include "KeepTicketFunc.h"
#include "LoginFunc.h"
#include "PeraProcessDesigner.h"
#include "value.h"
#include "reader.h"
#include "MainFrm.h"

static int s_nNextCallTime = 600000;
static bool s_bKeepRun = true;

CString CallKeepTicket(CString sTicket)
{
	CString sReturn;
	CString sTicketValue;
	int nPos = sTicket.Find('=');
	if (nPos == -1)
	{
		return sReturn;
	}
	else
	{
		sTicketValue = sTicket.Right(sTicket.GetLength() - sTicket.Find('=') - 1);
	}
	CString sResult = KeepTicket(sTicketValue);

	Json::Value jRoot;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sResult, jRoot))
	{
		return sReturn;
	}
	if (!jRoot.isObject()) return sReturn;

	CString sMsg;
	CString sTime;

	sMsg = ( jRoot["result"].isString() ) ? jRoot["result"].asCString() : _T("");
	sTime = ( jRoot["maxInactiveInterval"].isString() ) ? jRoot["maxInactiveInterval"].asCString() : _T("");

	int nTime = atoi(sTime);
	if (nTime > 0)
	{
		nTime *= 40000;
	}
	if (nTime > 0)
	{
		s_nNextCallTime = nTime;
	}

	sReturn = sMsg;
	return sReturn;
}

static DWORD WINAPI KeepTicketThread(LPVOID lpParameter)
{
	while (s_bKeepRun)
	{
		CPeraProcessDesignerApp* pTheApp = (CPeraProcessDesignerApp*)lpParameter;
		if (pTheApp)
		{
			CString sMsg = CallKeepTicket(pTheApp->m_LoginData.m_strTicket);
			if (sMsg.CompareNoCase("timeout") == 0)
			{
				//MessageBox("用户已超时，需重新登录！");
				//Logout(pTheApp->m_LoginData.m_strTicket);
				pTheApp->m_LoginData.Clear();
				CMainFrame* pMainFrame = (CMainFrame*)(pTheApp->m_pMainWnd);
				if (pMainFrame)
				{
					pMainFrame->m_wndLoginBar.RedrawWindow();
				}
			}
		}
		Sleep(s_nNextCallTime);
	}
	return 0;
}

void StartKeepTicketThread()
{
	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, KeepTicketThread
		, (LPVOID)&theApp
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		return;	
	}
	ResumeThread(hThread);

	::CloseHandle(hThread);
}

void StopKeepTicketThread()
{
	s_bKeepRun = false;
}