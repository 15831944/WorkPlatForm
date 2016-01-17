
#include "StdAfx.h"
#include "ProcessMgr.h"
#include "UUID.h"
#include <dbghelp.h>
#include "DlgSvg.h"
#pragma comment(lib, "dbghelp.lib")
#include "tools.h"
#include "ZipTool.h"
#include "PeraProcessDesigner.h"
#include "XmlHistoryModel.h"
#include "reader.h"
#include "MainFrm.h"
#include "PeraProcessDesignerUtility.h"
#include "InvokeWsFunc.h"
#include "writer.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "md5.h"
#include "filters.h"
#include "channels.h"
#include "files.h"
#include "hex.h"
#ifdef _DEBUG
#pragma comment(lib, "cryptlibd.lib")
#else
#pragma comment(lib, "cryptlib.lib")
#endif
#include "DlgLoginBk.h"
//#define TIXML_USE_STL
#include "tinyxml.h"
#include "DlgParamSet.h"
#include "ProcessFunc.h"
#include "DlgRunFlow.h"
#include "UtilityFunc.h"
#include "ConvertData.h"
#include "CxStructTreeNodeFlow.h"
#include "OperateThreadFunc.h"
#include "CustomXpdl.h"
#include "WebEventData.h"
#include "AttachWindow.h"

CProcessMgr::CProcessMgr(void)
{
	Clear();
	ClearRunVar();
}

void CProcessMgr::ClearRunVar()
{
	m_bProcessStarting = FALSE;
	m_sRunUrl.Empty();
	m_sFlowId.Empty();
	m_sXpdlStreamWithFileid.Empty();
	m_bIsModelRun = TRUE;
}

void CProcessMgr::SetRunVar(CString sRunUrl, CString sFlowId)
{
	m_sRunUrl = sRunUrl;
	m_sFlowId = sFlowId;
}

CProcessMgr::~CProcessMgr(void)
{
}

BOOL CProcessMgr::New()
{
	/*
建目录
存工作路径变量
打开建模页面
*/
	//创建临时目录
	CString sWorkPath = GetExecDir() + "\\Data\\" + ReslibMgr::GuidToString(ReslibMgr::CreateGuid()).c_str();

	if (!MakeSureDirectoryPathExists(sWorkPath + "\\"))
	{
		ZTools::WriteZToolsFormatLog("创建工作目录失败：%s", sWorkPath);
		return FALSE;
	}

	//打开建模页面
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (!pMainFrame)
	{
		return FALSE;
	}
	pMainFrame->OnMenuModuleOpenjianmo();

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	//设置当前设计过程的变量
	Clear();
	m_sWorkPath = sWorkPath;
	
	pSvg->Navigate( theGlobalConfig.m_strSvgUrl );

	return TRUE;
}

BOOL CProcessMgr::Open(CString sProcessFilePath)
{
	ZTools::WriteZToolsFormatLog("开始打开设计过程");
/*
建目录
解压
存工作路径变量
打开建模页面
*/
	if (sProcessFilePath.IsEmpty())
	{
		return FALSE;
	}

	//创建临时目录
	CString sWorkPath = GetExecDir() + "\\Data\\" + ReslibMgr::GuidToString(ReslibMgr::CreateGuid()).c_str();

	if (!MakeSureDirectoryPathExists(sWorkPath + "\\"))
	{
		ZTools::WriteZToolsFormatLog("创建工作目录失败：%s", sWorkPath);
		return FALSE;
	}

	//解压
	if (!ZipTool::Unzip((LPCTSTR)sProcessFilePath, (LPCTSTR)sWorkPath))
	{
		ZTools::WriteZToolsFormatLog("unzip file failed: %s", sProcessFilePath);
		return FALSE;
	}

	//打开建模页面
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (!pMainFrame)
	{
		return FALSE;
	}
	pMainFrame->OnMenuModuleOpenjianmo();

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	//设置当前设计过程的变量
	Clear();
	m_sProcessFilePath = sProcessFilePath;
	m_sWorkPath = sWorkPath;
	
	{//打开时同样添加历史记录
		CString strHistoryPath = m_sWorkPath + "\\main.xpdl";
		if ( !IsAccessibleFile( strHistoryPath ) )
		{
			return FALSE;
		}

		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.LoadFile(strHistoryPath, TIXML_ENCODING_UTF8);
		if (doc.Error())
		{
			ZTools::WriteZToolsFormatLog("解析main.xpdl出错");
			return FALSE;
		}
		TiXmlElement* rootElement = doc.RootElement();//Package
		if (rootElement)
		{
			std::string strName;
			std::string strValue;
			//rootElement->QueryStringAttribute("name", &strName);
			TiXmlAttribute* pAb;
			pAb = rootElement->FirstAttribute();
			while (pAb)
			{
				strName = pAb->Name();
				strValue = pAb->Value();
				ZTools::UTF8ToMB(strName);
				ZTools::UTF8ToMB(strValue);
				if(strName == "Name")
				{
					AddToHistory(strValue.c_str(), m_sProcessFilePath);//添加到历史
					theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_RECENTLYMENU, TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT, 0);
					break;
				}
				pAb = pAb->Next();
			}			
		}
	}

	ZTools::WriteZToolsFormatLog("工作路径准备完成，开始加载静态页面");
	pSvg->Navigate( theGlobalConfig.m_strSvgUrl );
	return TRUE;
}

BOOL CProcessMgr::Save()
{
	ZTools::WriteZToolsFormatLog("保存设计过程开始");
	/*
	SaveProcess
	*/
	if (m_sProcessFilePath.IsEmpty())
	{
		return FALSE;
	}

	//调用建模页面保存
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	wmd.m_sMethodName = "saveProcess";
	pParam = new CWebMethodParamData();
	pParam->m_sName = "filePath";
	pParam->m_sValue = m_sProcessFilePath;
	wmd.m_vParams.push_back(pParam);
	pSvg->InvokeJsMethod( &wmd );

	CString sResult = wmd.m_sResult;
	if (sResult.CompareNoCase("false") == 0)
	{
		return FALSE;
	}
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame)
	{
		pMainFrame->PostMessage(WM_UPDATE_MAIN_TITLE);
	}
	ZTools::WriteZToolsFormatLog("保存设计过程完成");
	return TRUE;
}

void CProcessMgr::Close()
{
	/*
	回收清空目录
	相关所有变量置空
	*/
	//建模页面到空白页面
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg != NULL )
	{
		CString str = GetSvgDefaultPage();
		if ( !IsAccessibleFile(str) ) str = "about:blank";
		pSvg->Navigate( str );
	}

	//结构树同步显示为空白树
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (pRoot)
	{
		pRoot->Clear();
		theApp.m_pBarJianmoTree->m_wndGridTree.Refresh();
		theApp.m_pBarJianmoTree->m_wndGridTree.RedrawWindow();
	}

	//临时目录清空
	if (!m_sWorkPath.IsEmpty())
	{
		CString sDeletePath = m_sWorkPath + "\\*";
		int nDir = sDeletePath.GetLength() + 2;
		char* psDir = new char[nDir];
		memset(psDir, 0, nDir);
		memcpy(psDir, (LPCTSTR)sDeletePath, sDeletePath.GetLength());

		SHFILEOPSTRUCT FileOp; 
		ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

		FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR ; 
		FileOp.hNameMappings = NULL; 
		FileOp.hwnd = NULL; 
		FileOp.lpszProgressTitle = NULL; 
		FileOp.pFrom = psDir;
		FileOp.pTo = NULL;
		FileOp.wFunc = FO_DELETE; 

		int nRet = SHFileOperation(&FileOp);

		delete[] psDir;

		::RemoveDirectory(m_sWorkPath);
	}
	//重置变量
	Clear();
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (pMainFrame)
	{
		pMainFrame->PostMessage(WM_UPDATE_MAIN_TITLE);
	}
}

BOOL CProcessMgr::OnNew()
{
	/*
	查询是否需要保存
	提示是、否、取消
	new
	*/
	if (!AskIsSave())
	{
		return FALSE;
	}

	Close();

	return New();
}

BOOL CProcessMgr::OnOpen()
{
	/*
	查询是否需要保存
	提示是、否、取消
	弹文件选择
	open
	*/

	//打开选择对话框
	CFileDialog dlgFile(TRUE
		, "ws"
		, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "Process File (*.ws)|*.ws||"
		, theApp.m_pMainWnd
		);

	int nRet = dlgFile.DoModal();

	if (nRet == IDCANCEL)
	{
		return FALSE;
	}
	else if (nRet == IDOK)
	{
		CString sFilePath = dlgFile.GetPathName();
		//打开前先关闭当前设计过程
		if (!AskIsSave())
		{
			return FALSE;
		}
		Close();
		//打开设计过程
		if (!Open(sFilePath))
		{
			CString sMsg;
			sMsg.Format("fail to open %s", sFilePath);
			MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}
	}

	SetModified(FALSE);
	return TRUE;
}

BOOL CProcessMgr::OnSave()
{
	//未保存过先弹选择文件
	//save

	//如果没有临时工作路径或没有打开建模页面
	if (m_sWorkPath.IsEmpty())
	{
		return TRUE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return TRUE;

	//查询是否可以保存
	if (!GetProcessStateCanSave())
	{
		return FALSE;
	}

	//如未保存过，选择设计过程文件
	if (m_sProcessFilePath.IsEmpty())
	{
		//默认以设计过程名称存储
		CString sProcessName("");

		CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
		if (pRoot)
		{
			sProcessName = pRoot->m_strOriName;
		}
		CFileDialog dlgFile(FALSE
			, "ws"
			, sProcessName
			, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
			, "Process File (*.ws)|*.ws||"
			, theApp.m_pMainWnd
			);

		int nRet = dlgFile.DoModal();

		if (nRet == IDCANCEL || nRet == IDNO)
		{
			return FALSE;
		}
		else if (nRet == IDOK || nRet == IDYES)
		{
			CString sFilePath = dlgFile.GetPathName();
			CString sExt = sFilePath.Right(3);
			if (sExt.CompareNoCase(".ws") != 0)
			{
				m_sProcessFilePath = sFilePath + ".ws";
			}
			else
			{
				m_sProcessFilePath = sFilePath;
			}
		}
	}

	//保存
	if (Save())
	{
		SetModified(FALSE);
		//保存成功时加入到历史
		AddToHistory();
		theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_RECENTLYMENU, TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT, 0);
		//MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "设计过程保存成功", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	else
	{
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "设计过程保存失败", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	return Save();
}

BOOL CProcessMgr::OnSaveAs()
{
	//选择文件
	//save
	//将变量都置为新的

	//如果没有临时工作路径或没有打开建模页面
	if (m_sWorkPath.IsEmpty())
	{
		return TRUE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return TRUE;

	//查询是否可以保存
	if (!GetProcessStateCanSave())
	{
		return FALSE;
	}

	CFileDialog dlgFile(FALSE
		, "ws"
		, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "Process File (*.ws)|*.ws||"
		, theApp.m_pMainWnd
		);

	int nRet = dlgFile.DoModal();

	if (nRet == IDCANCEL || nRet == IDNO)
	{
		return FALSE;
	}
	else if (nRet == IDOK || nRet == IDYES)
	{
		CString sFilePath = dlgFile.GetPathName();
		CString sExt = sFilePath.Right(3);
		if (sExt.CompareNoCase(".ws") != 0)
		{
			m_sProcessFilePath = sFilePath + ".ws";
		}
		else
		{
			m_sProcessFilePath = sFilePath;
		}
	}

	//保存
	if (Save())
	{
		SetModified(FALSE);
		//保存成功时加入到历史
		AddToHistory();
		theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_RECENTLYMENU, TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT, 0);
	//	MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "设计过程另存为成功", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	else
	{
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "设计过程另存为失败", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
}

BOOL CProcessMgr::OnClose()
{
	//查询是否需要保存
	//close
	if (!AskIsSave())
	{
		return FALSE;
	}
	Close();
	SetModified(FALSE);
	return TRUE;
}

BOOL CProcessMgr::GetProcessStateNeedSave()
{
	if (theApp.m_pBarJianmoTree)
	{
		return theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified;
	}
	return FALSE;
	//调页面方法查询是否需要保存
	//return TRUE;
}

BOOL CProcessMgr::CanRunSubProcess()
{
	//调页面方法查询当前流程上层节点是否有效

	CCxStructTreeNode * pNodeCur = theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData();
	if ( pNodeCur == NULL ) return FALSE;
	if ( !pNodeCur->IsFlowNode() ) return FALSE;
	if ( !((CCxStructTreeNodeFlow*)pNodeCur)->CanRun() )
	{
		::MessageBox(GetMainWnd(), "存在未成功运行的上游节点，无法运行", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

void CProcessMgr::Clear()
{
	m_sWorkPath.Empty();
	m_sProcessFilePath.Empty();
	SetModified(FALSE);
}

BOOL CProcessMgr::AskIsSave()
{
	//如果没有临时工作路径或没有打开建模页面，不需要询问
	if (!m_sWorkPath.IsEmpty())
	{
		VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleModel );
		if ( pView != NULL )
		{
			CDlgSvg * pSvg = reinterpret_cast< CDlgSvg * >( pView->m_dwExData );
			if ( pSvg != NULL )
			{
				if (GetProcessStateNeedSave())
				{
					int nRet = MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "是否保存当前正在编辑的设计过程", g_lpszAppTitle, MB_YESNOCANCEL|MB_ICONQUESTION);
					switch (nRet)
					{
					case IDYES:
						if (!OnSave())
						{
							//保存不成功时，中断后续操作
							return FALSE;
						}
						break;
					case IDNO:
						break;
					case IDCANCEL:
						//用户取消当前操作时，后续也不进行
						return FALSE;
						break;
					default:
						break;
					}
				}
			}
		}
	}

	//保存成功或者不保存时，都继续后续操作，新建或打开
	return TRUE;
}

void CProcessMgr::AddToHistory()
{
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (!pRoot) return;

	//增加到历史列表，显示设计过程名称及WS文件全路径
	CXmlHistoryModel xhm;
	xhm.Load();

	CHistoryModelData * pHMD = new CHistoryModelData();
	pHMD->m_strName = pRoot->m_strOriName;
	pHMD->m_strPath = m_sProcessFilePath;

	xhm.ClearNode(pHMD); //清理该历史节点
	xhm.InsertFront( pHMD ); //添加到头
	

	xhm.Save();
}

void CProcessMgr::AddToHistory(LPCTSTR strName, LPCTSTR strPath)
{
	//增加到历史列表，显示设计过程名称及WS文件全路径
	CXmlHistoryModel xhm;
	xhm.Load();

	CHistoryModelData * pHMD = new CHistoryModelData();
	pHMD->m_strName = strName;
	pHMD->m_strPath = strPath;

	xhm.ClearNode(pHMD); //清理该历史节点
	xhm.InsertFront( pHMD ); //添加到头

	xhm.Save();
}

void CProcessMgr::RefreshHistory()
{
	CXmlHistoryModel xhm;
	xhm.Load();
	xhm.ClearNode(); //清理
	xhm.Save();
}

BOOL CProcessMgr::Publish()
{
	//发布所需的变量是否有效
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (!pRoot)
	{
		ZTools::WriteZToolsFormatLog("未取到根节点");
		return FALSE;
	}
	if (!theApp.m_pDlgResLibTree)
	{
		return FALSE;
	}

	//查询是否可以保存
	if (!GetProcessStateCanSave())
	{
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有建模页面，无法保存");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;
	
	CString sProcessPath;
	sProcessPath.Format("%s\\publish\\%s\\%s.ws", m_sWorkPath, ZTools::GetCurrentTimeString16().c_str(), pRoot->m_strOriName);
	MakeSureDirectoryPathExists(sProcessPath);

	//调用建模页面保存
	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	wmd.m_sMethodName = "saveProcess";
	pParam = new CWebMethodParamData();
	pParam->m_sName = "filePath";
	pParam->m_sValue = sProcessPath;
	wmd.m_vParams.push_back(pParam);
	pSvg->InvokeJsMethod( &wmd );

	CString sResult = wmd.m_sResult;
	if (sResult.CompareNoCase("false") == 0)
	{
		ZTools::WriteZToolsFormatLog("发布流程时保存流程失败%s", sProcessPath);
		return FALSE;
	}

	if (!PathFileExists(sProcessPath))
	{
		ZTools::WriteZToolsFormatLog("发布子流程时保存子流程失败%s", sProcessPath);
		return FALSE;
	}

	//各属性信息从结构树上取，路径从当前类中取，调用资源树发布
	PublishInfo pi;
	pi.sAuthor = (char *)(LPCTSTR)pRoot->m_strAuthor;
	pi.sDesc = (char *)(LPCTSTR)pRoot->m_strDescription;
	pi.sFilePath = (char *)(LPCTSTR)sProcessPath;
	pi.sName = (char *)(LPCTSTR)pRoot->m_strOriName;
	pi.sTime = (char *)(LPCTSTR)pRoot->m_strCreated;
	pi.sVersion = (char *)(LPCTSTR)pRoot->m_strVersion;

	theApp.m_pDlgResLibTree->m_tree.SendProcessTreeDB(pi);
	return TRUE;
}

void CProcessMgr::SetModified( BOOL bModified)
{
	if (theApp.m_pBarJianmoTree)
	{
		theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = bModified;
	}
}

BOOL CProcessMgr::GetProcessStateCanSave()
{
	//调用建模页面查询保存
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	CWebMethodData wmd;
	wmd.m_sMethodName = "getProcessStateCanSave";
	pSvg->InvokeJsMethod( &wmd );

	CString sResult = wmd.m_sResult;

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sResult, root))
	{
		return FALSE;
	}

	CString sFlag = root["flag"].asString().c_str();	
	if (sFlag.CompareNoCase("false") == 0)
	{
		CString sMsg = root["msg"].asString().c_str();
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CProcessMgr::GetSubProcessStateCanSave()
{
	//调用建模页面查询保存
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	CWebMethodData wmd;
	wmd.m_sMethodName = "getSubProcessStateCanSave";
	pSvg->InvokeJsMethod( &wmd );

	CString sResult = wmd.m_sResult;

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sResult, root))
	{
		return FALSE;
	}

	CString sFlag = root["flag"].asString().c_str();	
	if (sFlag.CompareNoCase("false") == 0)
	{
		CString sMsg = root["msg"].asString().c_str();
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CProcessMgr::OnRunModel()
{
	//先保存
	//然后上传
	//然后调ICE
	//然后拼参数启动

	//运行所需的变量是否有效
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (!pRoot)
	{
		return FALSE;
	}

	//查询是否允许运行
	//if (!GetProcessStateCanRun())
	//{
	//	return FALSE;
	//}
	//查询是否可以保存
	if (!GetProcessStateCanSave())
	{
		return FALSE;
	}

	//如果ticket为空，且单机版安装标志为TRUE时，走单机版逻辑；否则走网络版逻辑
	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		if ( theApp.m_bAloneVersionWs )
		{
			ShowLoading();
			if ( !RunAloneVersion( TRUE ) )
			{
				HideLoading();
				return FALSE;
			}
			return TRUE;
		}
		else
		{
			//使用网络引擎，必须要登录才可以运行
			CDlgLoginBk dlgLogin;
			dlgLogin.DoModal();
			if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
			{
				return FALSE;
			}
		}
	}

	if (m_bProcessStarting)
	{
		return FALSE;
	}
	
	ShowLoading();

	if ( !RunInServerVersion( TRUE ) )
	{
		ClearRunVar();
		HideLoading();
		return FALSE;
	}

	return TRUE;	
}

BOOL CProcessMgr::InvokeRunProcess(LPVOID lpParameter, std::string strJsonParams)
{
	CProcessMgr* pMgr = (CProcessMgr*)lpParameter;
	//调ICE
	CString sJsonResult = InvokeWsMethod("startLocalModelByByteXpdl", strJsonParams.c_str());
	Json::Value jsonResult;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sJsonResult, jsonResult))
	{
		HideLoading();
		CString sMsg;
		sMsg.Format("调用发起设计过程返回值异常,%s", sJsonResult);
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	std::string strOptMsg = jsonResult["optMsg"].asString();
	//ZTools::UTF8ToMB(strOptMsg);
	if (!strOptMsg.empty())
	{
		HideLoading();
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), strOptMsg.c_str(), g_lpszAppTitle, MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	CString sUrl = jsonResult["optUrl"].asString().c_str();
	CString sFlowId = jsonResult["flowId"].asString().c_str();
	if (sFlowId.IsEmpty())
	{
		HideLoading();
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "运行实例ID为空", g_lpszAppTitle, MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	pMgr->SetRunVar(sUrl, sFlowId);

	return TRUE;
}

BOOL CProcessMgr::OpenRunInNewTab(LPVOID lpParameter)
{
	CProcessMgr* pMgr = (CProcessMgr*)lpParameter;

	//启动打开执行页面
	CString sExeDir = GetExecDir();
	CString sExePath = sExeDir + "\\PeraWebContainer.exe";

	//-json {"url" : "www.baidu.com", "user" : "fei", "realusername" : "赵飞", "ticket": "1", "password": "999999", "role": "admin"}
	CString sParam;
	////联调先没有拼入票据
	sParam.Format("-json {\"url\" : \"%s\", \"ticket\":\"%s\"}", pMgr->m_sRunUrl, theApp.m_LoginData.m_strTicket);
	//sParam.Format("-json {\"url\" : \"%s\", \"ticket\":\"%s\"}", sUrl, "ticket-proxy=ZzxSJrzPhvKJCElqycem");

	//CCxWPENavigate * pData = new CCxWPENavigate();
	//pData->m_strUrl = pMgr->m_sRunUrl;
	//AttachTicket( pData->m_strUrl, theApp.m_LoginData.m_strTicket );
	/*CCxWebPageEvent * pEvent = theWebPageMgr.NewEvent( CPeraWebPageMgr::FW_TAB_VIEW
		, CPeraWebPageMgr::FE_START, pData, TRUE );
	delete pData; pData = NULL;
	if ( pEvent == NULL )
	{
		return FALSE;
	}*/
	//if ( !theWebPageMgr.Require( CPeraWebPageMgr::FW_TAB_VIEW
	//	, CPeraWebPageMgr::FE_START, pData ) )
	//{
	//	//theWebPageMgr.DeleteEvent( pEvent ); return FALSE;
	//}
	//delete pData; pData = NULL;

	if ( g_pawRunProcess != NULL )
	{
		ZTools::WriteZToolsLog( "CProcessMgr::OpenRunInNewTab g_pawRunProcess 值不为空" );
		return FALSE;
	}
	g_pawRunProcess = new CAttachWindow();
	CString sWebContainerPath = GetExecDir() + "\\PeraPDWebContainer.exe";
	ZTools::WriteZToolsFormatLog( "CProcessMgr::OpenRunInNewTab webcontainer.exe 路径[%s]", sWebContainerPath );
	CAttachWindowExData awExData;
	awExData.m_sUrl = pMgr->m_sRunUrl;
	AttachTicket( awExData.m_sUrl, theApp.m_LoginData.m_strTicket );
	CString sId = GetNewEventNameWin7();
	if ( !g_pawRunProcess->CreateClient( sId, 
										WEBTYPE_RUNPROCESS, 
										CPMSG_AW_PROCESS_ATTACHING, 
										GetCurrentProcessId(), 
										GetMainWnd(), 
										sWebContainerPath, 
										awExData.GetJsonStr() ))
	{
		ZTools::WriteZToolsLog( "CProcessMgr::OpenRunInNewTab 启动WebContainer失败" );
		delete g_pawRunProcess; g_pawRunProcess = NULL;
		return FALSE;
	}
	ZTools::WriteZToolsLog( "CProcessMgr::OpenRunInNewTab 启动WebContainer完成" );

	return TRUE;
}
BOOL CProcessMgr::OpenRunInModal(LPVOID lpParameter)
{
	//return OpenRunInNewTab(lpParameter);

	CProcessMgr* pMgr = (CProcessMgr*)lpParameter;
	//////////////////////////////////////////////////////////////////////////
	//给主窗口发消息，打开模态窗口	
	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SHOW_RUNFLOW_DIALOG, (WPARAM)(LPCTSTR)pMgr->m_sRunUrl
		, (LPARAM)(LPCTSTR)pMgr->m_sFlowId);

	return TRUE;
}

//获取文件的MD5码
CString CProcessMgr::GetFileMD5(CString sFilePath)
{
	std::string strFilePath = (LPCTSTR)sFilePath;
	CryptoPP::Weak::MD5 md5;
	CryptoPP::HashFilter filter(md5);

	std::auto_ptr<CryptoPP::ChannelSwitch> channelSwitch(new CryptoPP::ChannelSwitch);
	channelSwitch->AddDefaultRoute(filter);
	std::wstring wstr = s2ws_1(strFilePath);
	CryptoPP::FileSource(wstr.c_str(), true, channelSwitch.release());

	std::stringstream result;
	CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(result), false);
	filter.TransferTo(encoder);
	std::string strResult = result.str();
	CString sResult = strResult.c_str();
	return sResult;
}

CString CProcessMgr::GetReplacedXpdlByUploadFile()
{
	//调用建模页面查询保存
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return "";

	CWebMethodData wmd;
	wmd.m_sMethodName = "getReplacedXpdlByUploadFile";
	pSvg->InvokeJsMethod( &wmd );

	return wmd.m_sResult;
}

BOOL CProcessMgr::OnSaveSubProcess(void* p)
{
	CCxStructTreeNodeFlow * pNode = reinterpret_cast< CCxStructTreeNodeFlow * >(p);
	if ( pNode == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有取到当前节点");
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有建模页面，无法保存");
		return FALSE;
	}
	
	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;

	//查询是否可以保存
	//后续改为取当前节点是否可以保存
	if (!GetSubProcessStateCanSave())
	{
		return FALSE;
	}

	CString sSubProcessPath;

	CFileDialog dlgFile(FALSE
		, "ws"
		, pNode->m_strOriName
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "Process File (*.ws)|*.ws||"
		, theApp.m_pMainWnd
		);

	int nRet = dlgFile.DoModal();

	if (nRet == IDCANCEL || nRet == IDNO)
	{
		return FALSE;
	}
	else if (nRet == IDOK || nRet == IDYES)
	{
		CString sFilePath = dlgFile.GetPathName();
		CString sExt = sFilePath.Right(3);
		if (sExt.CompareNoCase(".ws") != 0)
		{
			sSubProcessPath = sFilePath + ".ws";
		}
		else
		{
			sSubProcessPath = sFilePath;
		}
	}

	//调用建模页面保存
	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	wmd.m_sMethodName = "saveSubprocess";
	pParam = new CWebMethodParamData();
	pParam->m_sName = "filePath";
	pParam->m_sValue = sSubProcessPath;
	wmd.m_vParams.push_back(pParam);
	pSvg->InvokeJsMethod( &wmd );

	CString sResult = wmd.m_sResult;
	if (sResult.CompareNoCase("false") == 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CProcessMgr::OnPublishSubProcess( void* p )
{
	CCxStructTreeNodeFlow * pNode = reinterpret_cast< CCxStructTreeNodeFlow * >(p);
	if ( pNode == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有取到当前节点");
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有建模页面，无法保存");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;

	//查询是否可以保存
	//后续改为取当前节点是否可以保存
	if (!GetSubProcessStateCanSave())
	{
		return FALSE;
	}

	CString sSubProcessPath;
	sSubProcessPath.Format("%s\\publish\\%s\\%s.ws", m_sWorkPath, ZTools::GetCurrentTimeString16().c_str(), pNode->m_strOriName);
	MakeSureDirectoryPathExists(sSubProcessPath);

	//调用建模页面保存
	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	wmd.m_sMethodName = "saveSubprocess";
	pParam = new CWebMethodParamData();
	pParam->m_sName = "filePath";
	pParam->m_sValue = sSubProcessPath;
	wmd.m_vParams.push_back(pParam);
	pSvg->InvokeJsMethod( &wmd );

	CString sResult = wmd.m_sResult;
	if (sResult.CompareNoCase("false") == 0)
	{
		ZTools::WriteZToolsFormatLog("发布子流程时保存子流程失败%s", sSubProcessPath);
		return FALSE;
	}

	if (!PathFileExists(sSubProcessPath))
	{
		ZTools::WriteZToolsFormatLog("发布子流程时保存子流程失败%s", sSubProcessPath);
		return FALSE;
	}

	//各属性信息从结构树上取
	PublishInfo pi;
	pi.sAuthor = (char *)(LPCTSTR)pNode->m_strAuthor;
	pi.sDesc = (char *)(LPCTSTR)pNode->m_strDescription;
	pi.sFilePath = (char *)(LPCTSTR)sSubProcessPath;
	pi.sName = (char *)(LPCTSTR)pNode->m_strOriName;
	pi.sTime = (char *)(LPCTSTR)pNode->m_strCreated;
	pi.sVersion = (char *)(LPCTSTR)pNode->m_strVersion;

	theApp.m_pDlgResLibTree->m_tree.SendProcessTreeDB(pi);

	return TRUE;
}

BOOL CProcessMgr::OnRunSubProcess()
{
	if (!theApp.m_pBarJianmoTree)
	{
		return FALSE;
	}
	CCxStructTreeNode * pNodeCur = theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData();
	if ( pNodeCur == NULL ) return FALSE;

	CCxStructTreeNodeFlow * pNode = reinterpret_cast< CCxStructTreeNodeFlow * >(pNodeCur);
	if ( pNode == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有取到当前节点");
		return FALSE;
	}
	if ( pNode->m_pTree == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有取到当前节点的m_pTree");
		return FALSE;
	}
	//pNode->m_pTree->BeforeRunning( pNode );

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有建模页面，无法运行");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;
	
	//查询是否允许运行
	if (!CanRunSubProcess())
	{
		return FALSE;
	}
	//查询是否可以保存
	if (!GetSubProcessStateCanSave())
	{
		return FALSE;
	}

	//如果ticket为空，且单机版安装标志为TRUE时，走单机版逻辑；否则走网络版逻辑
	if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
	{
		if ( theApp.m_bAloneVersionWs )
		{
			ShowLoading();
			if ( !RunAloneVersion( FALSE ) )
			{
				HideLoading();
				return FALSE;
			}
			return TRUE;
		}
		else
		{
			//使用网络引擎，必须要登录才可以运行
			CDlgLoginBk dlgLogin;
			dlgLogin.DoModal();
			if ( theApp.m_LoginData.m_strTicket.IsEmpty() )
			{
				return FALSE;
			}
		}
	}

	ShowLoading();

	if ( !RunInServerVersion( FALSE ) )
	{
		HideLoading();
		return FALSE;
	}

	return TRUE;	
}

BOOL CProcessMgr::OnEditSubProcess( void* p )
{
	CCxStructTreeNodeFlow * pNode = reinterpret_cast< CCxStructTreeNodeFlow * >(p);
	if ( pNode == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有取到当前节点");
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("没有建模页面，无法保存");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;
	
	if ( !pNode->IsFlowNode() && !pNode->IsRootNode() ) 
	{
		ZTools::WriteZToolsFormatLog("当前编辑的节点不是根节点或子流程节点");
		return FALSE;
	}

	//调用建模页面编辑
// 	CWebMethodData wmd;
// 	CWebMethodParamData * pParam = NULL;
// 	wmd.m_strMethodName = "editProcessParameters";
// 	pSvg->InvokeJsMethod( &wmd );
// 	return TRUE;

	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_STRUCTTREE_EDIT_ROOTPARAMS, NULL, NULL);
	return TRUE;
}

CCxStructTreeNodeFlow* CProcessMgr::GetTreeRoot()
{
	if (!theApp.m_pBarJianmoTree)
	{
		return NULL;
	}

	return (CCxStructTreeNodeFlow*)theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot();
}
BOOL CProcessMgr::GetComponentStateCanSave()
{
	//调用建模页面查询保存
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	CWebMethodData wmd;
	wmd.m_sMethodName = "getComponentStateCanSave";
	pSvg->InvokeJsMethod( &wmd );

	CString sResult = wmd.m_sResult;

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sResult, root))
	{
		return FALSE;
	}

	CString sFlag = root["flag"].asString().c_str();	
	if (sFlag.CompareNoCase("false") == 0)
	{
		CString sMsg = root["msg"].asString().c_str();
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CProcessMgr::RunInServerVersion(BOOL bIsModelRun)
{
	ClearRunVar();

	m_bIsModelRun = bIsModelRun;
	if (m_bIsModelRun)
	{
		m_bProcessStarting = TRUE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		std::string strError("RunInServerVersion 获取Svg失败");
		ZTools::WriteZToolsLog(strError);
		return FALSE;	
	}

	CWebMethodData wmd;
	if ( m_bIsModelRun )
	{
		wmd.m_sMethodName = "getReplacedAbsolutePathXpdl";
	}
	else
	{
		wmd.m_sMethodName = "getReplacedAbsolutePathSubXpdl";
	}
	pSvg->InvokeJsMethod( &wmd );
	m_sXpdlStreamWithFileid = wmd.m_sResult;
	if ( m_sXpdlStreamWithFileid.IsEmpty() )
	{
		std::string strError("RunInServerVersion 获取xpdl失败");
		ZTools::WriteZToolsLog(strError);
		return FALSE;
	}

	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(
		NULL
		, 0
		, RunInServerThread
		, (LPVOID)this
		, CREATE_SUSPENDED
		, &dwThreadId);

	if (hThread == NULL || hThread == 0)
	{
		std::string strError("执行设计过程时创建线程失败");
		ZTools::WriteZToolsLog(strError);
		return FALSE;	
	}
	ResumeThread(hThread);
	::CloseHandle(hThread);

	return TRUE;
}

DWORD WINAPI CProcessMgr::RunInServerThread( LPVOID lpParameter )
{
	CProcessMgr* pMgr = (CProcessMgr*)lpParameter;

	std::string strXpdl = (LPCTSTR)pMgr->m_sXpdlStreamWithFileid;
	ZTools::MBToUTF8(strXpdl);
	CCustomXpdl xpdlObj;
	xpdlObj.LoadStream(strXpdl.c_str(), pMgr->m_sWorkPath + "\\temp.xml");
	xpdlObj.Parse();
	xpdlObj.UploadFileAndReplacePath();
	strXpdl.clear();
	xpdlObj.GetXpdlStream(strXpdl);

	// 	//pkgid
	// 	//xpdlFileContent
	// 	//ip
	// 	//userid
	// 	//username
	Json::Value jsonParams(Json::arrayValue);

	if (pMgr->m_bIsModelRun)
	{
		CString sPkgId;
		if (theApp.m_pBarJianmoTree)
		{
			CCxStructTreeNodeFlow* pRoot = (CCxStructTreeNodeFlow*)theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot();
			if (pRoot)
			{
				sPkgId = pRoot->m_strPkgId;
			}
		}
		jsonParams.append(Json::Value(sPkgId));
	}
	else
	{
		jsonParams.append(Json::Value(""));
	}
	jsonParams.append(Json::Value(strXpdl));

	jsonParams.append(Json::Value(GetLocalIpFromIce()));
	jsonParams.append(Json::Value(theApp.m_LoginData.m_strUser));
	jsonParams.append(Json::Value(theApp.m_LoginData.m_strRealName));
	if (pMgr->m_bIsModelRun)
	{
		jsonParams.append(Json::Value("Process"));
	}
	else
	{
		jsonParams.append(Json::Value("SubFlow"));
	}

	{
		int nPos = theApp.m_LoginData.m_strTicket.Find('=');
		if (nPos == -1)
		{
			jsonParams.append(Json::Value(""));
		}
		else
		{
			CString strTicket;
			strTicket = theApp.m_LoginData.m_strTicket.Right(theApp.m_LoginData.m_strTicket.GetLength() - theApp.m_LoginData.m_strTicket.Find('=') - 1);
			jsonParams.append(Json::Value(strTicket));
		}
	}
	Json::FastWriter writer;
	std::string strJsonParams = writer.write(jsonParams);

	if (!InvokeRunProcess(lpParameter, strJsonParams))
	{
		pMgr->ClearRunVar();
		HideLoading();
		return FALSE;
	}
	
	if (pMgr->m_bIsModelRun)
	{
		HideLoading();
		OpenRunInNewTab(lpParameter);
	}
	else
	{
		HideLoading();
		OpenRunInModal(lpParameter);
	}

	return TRUE;
}