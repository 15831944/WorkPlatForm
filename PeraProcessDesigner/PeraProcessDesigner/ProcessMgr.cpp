
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
��Ŀ¼
�湤��·������
�򿪽�ģҳ��
*/
	//������ʱĿ¼
	CString sWorkPath = GetExecDir() + "\\Data\\" + ReslibMgr::GuidToString(ReslibMgr::CreateGuid()).c_str();

	if (!MakeSureDirectoryPathExists(sWorkPath + "\\"))
	{
		ZTools::WriteZToolsFormatLog("��������Ŀ¼ʧ�ܣ�%s", sWorkPath);
		return FALSE;
	}

	//�򿪽�ģҳ��
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (!pMainFrame)
	{
		return FALSE;
	}
	pMainFrame->OnMenuModuleOpenjianmo();

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	//���õ�ǰ��ƹ��̵ı���
	Clear();
	m_sWorkPath = sWorkPath;
	
	pSvg->Navigate( theGlobalConfig.m_strSvgUrl );

	return TRUE;
}

BOOL CProcessMgr::Open(CString sProcessFilePath)
{
	ZTools::WriteZToolsFormatLog("��ʼ����ƹ���");
/*
��Ŀ¼
��ѹ
�湤��·������
�򿪽�ģҳ��
*/
	if (sProcessFilePath.IsEmpty())
	{
		return FALSE;
	}

	//������ʱĿ¼
	CString sWorkPath = GetExecDir() + "\\Data\\" + ReslibMgr::GuidToString(ReslibMgr::CreateGuid()).c_str();

	if (!MakeSureDirectoryPathExists(sWorkPath + "\\"))
	{
		ZTools::WriteZToolsFormatLog("��������Ŀ¼ʧ�ܣ�%s", sWorkPath);
		return FALSE;
	}

	//��ѹ
	if (!ZipTool::Unzip((LPCTSTR)sProcessFilePath, (LPCTSTR)sWorkPath))
	{
		ZTools::WriteZToolsFormatLog("unzip file failed: %s", sProcessFilePath);
		return FALSE;
	}

	//�򿪽�ģҳ��
	CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
	if (!pMainFrame)
	{
		return FALSE;
	}
	pMainFrame->OnMenuModuleOpenjianmo();

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	//���õ�ǰ��ƹ��̵ı���
	Clear();
	m_sProcessFilePath = sProcessFilePath;
	m_sWorkPath = sWorkPath;
	
	{//��ʱͬ�������ʷ��¼
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
			ZTools::WriteZToolsFormatLog("����main.xpdl����");
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
					AddToHistory(strValue.c_str(), m_sProcessFilePath);//��ӵ���ʷ
					theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_RECENTLYMENU, TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT, 0);
					break;
				}
				pAb = pAb->Next();
			}			
		}
	}

	ZTools::WriteZToolsFormatLog("����·��׼����ɣ���ʼ���ؾ�̬ҳ��");
	pSvg->Navigate( theGlobalConfig.m_strSvgUrl );
	return TRUE;
}

BOOL CProcessMgr::Save()
{
	ZTools::WriteZToolsFormatLog("������ƹ��̿�ʼ");
	/*
	SaveProcess
	*/
	if (m_sProcessFilePath.IsEmpty())
	{
		return FALSE;
	}

	//���ý�ģҳ�汣��
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
	ZTools::WriteZToolsFormatLog("������ƹ������");
	return TRUE;
}

void CProcessMgr::Close()
{
	/*
	�������Ŀ¼
	������б����ÿ�
	*/
	//��ģҳ�浽�հ�ҳ��
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg != NULL )
	{
		CString str = GetSvgDefaultPage();
		if ( !IsAccessibleFile(str) ) str = "about:blank";
		pSvg->Navigate( str );
	}

	//�ṹ��ͬ����ʾΪ�հ���
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (pRoot)
	{
		pRoot->Clear();
		theApp.m_pBarJianmoTree->m_wndGridTree.Refresh();
		theApp.m_pBarJianmoTree->m_wndGridTree.RedrawWindow();
	}

	//��ʱĿ¼���
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
	//���ñ���
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
	��ѯ�Ƿ���Ҫ����
	��ʾ�ǡ���ȡ��
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
	��ѯ�Ƿ���Ҫ����
	��ʾ�ǡ���ȡ��
	���ļ�ѡ��
	open
	*/

	//��ѡ��Ի���
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
		//��ǰ�ȹرյ�ǰ��ƹ���
		if (!AskIsSave())
		{
			return FALSE;
		}
		Close();
		//����ƹ���
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
	//δ������ȵ�ѡ���ļ�
	//save

	//���û����ʱ����·����û�д򿪽�ģҳ��
	if (m_sWorkPath.IsEmpty())
	{
		return TRUE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return TRUE;

	//��ѯ�Ƿ���Ա���
	if (!GetProcessStateCanSave())
	{
		return FALSE;
	}

	//��δ�������ѡ����ƹ����ļ�
	if (m_sProcessFilePath.IsEmpty())
	{
		//Ĭ������ƹ������ƴ洢
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

	//����
	if (Save())
	{
		SetModified(FALSE);
		//����ɹ�ʱ���뵽��ʷ
		AddToHistory();
		theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_RECENTLYMENU, TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT, 0);
		//MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "��ƹ��̱���ɹ�", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	else
	{
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "��ƹ��̱���ʧ��", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	return Save();
}

BOOL CProcessMgr::OnSaveAs()
{
	//ѡ���ļ�
	//save
	//����������Ϊ�µ�

	//���û����ʱ����·����û�д򿪽�ģҳ��
	if (m_sWorkPath.IsEmpty())
	{
		return TRUE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return TRUE;

	//��ѯ�Ƿ���Ա���
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

	//����
	if (Save())
	{
		SetModified(FALSE);
		//����ɹ�ʱ���뵽��ʷ
		AddToHistory();
		theApp.GetMainWnd()->SetTimer( TIMER_MAINFRAME_RECENTLYMENU, TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT, 0);
	//	MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "��ƹ������Ϊ�ɹ�", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	else
	{
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "��ƹ������Ϊʧ��", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
}

BOOL CProcessMgr::OnClose()
{
	//��ѯ�Ƿ���Ҫ����
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
	//��ҳ�淽����ѯ�Ƿ���Ҫ����
	//return TRUE;
}

BOOL CProcessMgr::CanRunSubProcess()
{
	//��ҳ�淽����ѯ��ǰ�����ϲ�ڵ��Ƿ���Ч

	CCxStructTreeNode * pNodeCur = theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData();
	if ( pNodeCur == NULL ) return FALSE;
	if ( !pNodeCur->IsFlowNode() ) return FALSE;
	if ( !((CCxStructTreeNodeFlow*)pNodeCur)->CanRun() )
	{
		::MessageBox(GetMainWnd(), "����δ�ɹ����е����νڵ㣬�޷�����", g_lpszAppTitle, MB_OK|MB_ICONERROR);
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
	//���û����ʱ����·����û�д򿪽�ģҳ�棬����Ҫѯ��
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
					int nRet = MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "�Ƿ񱣴浱ǰ���ڱ༭����ƹ���", g_lpszAppTitle, MB_YESNOCANCEL|MB_ICONQUESTION);
					switch (nRet)
					{
					case IDYES:
						if (!OnSave())
						{
							//���治�ɹ�ʱ���жϺ�������
							return FALSE;
						}
						break;
					case IDNO:
						break;
					case IDCANCEL:
						//�û�ȡ����ǰ����ʱ������Ҳ������
						return FALSE;
						break;
					default:
						break;
					}
				}
			}
		}
	}

	//����ɹ����߲�����ʱ�������������������½����
	return TRUE;
}

void CProcessMgr::AddToHistory()
{
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (!pRoot) return;

	//���ӵ���ʷ�б���ʾ��ƹ������Ƽ�WS�ļ�ȫ·��
	CXmlHistoryModel xhm;
	xhm.Load();

	CHistoryModelData * pHMD = new CHistoryModelData();
	pHMD->m_strName = pRoot->m_strOriName;
	pHMD->m_strPath = m_sProcessFilePath;

	xhm.ClearNode(pHMD); //�������ʷ�ڵ�
	xhm.InsertFront( pHMD ); //��ӵ�ͷ
	

	xhm.Save();
}

void CProcessMgr::AddToHistory(LPCTSTR strName, LPCTSTR strPath)
{
	//���ӵ���ʷ�б���ʾ��ƹ������Ƽ�WS�ļ�ȫ·��
	CXmlHistoryModel xhm;
	xhm.Load();

	CHistoryModelData * pHMD = new CHistoryModelData();
	pHMD->m_strName = strName;
	pHMD->m_strPath = strPath;

	xhm.ClearNode(pHMD); //�������ʷ�ڵ�
	xhm.InsertFront( pHMD ); //��ӵ�ͷ

	xhm.Save();
}

void CProcessMgr::RefreshHistory()
{
	CXmlHistoryModel xhm;
	xhm.Load();
	xhm.ClearNode(); //����
	xhm.Save();
}

BOOL CProcessMgr::Publish()
{
	//��������ı����Ƿ���Ч
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (!pRoot)
	{
		ZTools::WriteZToolsFormatLog("δȡ�����ڵ�");
		return FALSE;
	}
	if (!theApp.m_pDlgResLibTree)
	{
		return FALSE;
	}

	//��ѯ�Ƿ���Ա���
	if (!GetProcessStateCanSave())
	{
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("û�н�ģҳ�棬�޷�����");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;
	
	CString sProcessPath;
	sProcessPath.Format("%s\\publish\\%s\\%s.ws", m_sWorkPath, ZTools::GetCurrentTimeString16().c_str(), pRoot->m_strOriName);
	MakeSureDirectoryPathExists(sProcessPath);

	//���ý�ģҳ�汣��
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
		ZTools::WriteZToolsFormatLog("��������ʱ��������ʧ��%s", sProcessPath);
		return FALSE;
	}

	if (!PathFileExists(sProcessPath))
	{
		ZTools::WriteZToolsFormatLog("����������ʱ����������ʧ��%s", sProcessPath);
		return FALSE;
	}

	//��������Ϣ�ӽṹ����ȡ��·���ӵ�ǰ����ȡ��������Դ������
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
	//���ý�ģҳ���ѯ����
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
	//���ý�ģҳ���ѯ����
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
	//�ȱ���
	//Ȼ���ϴ�
	//Ȼ���ICE
	//Ȼ��ƴ��������

	//��������ı����Ƿ���Ч
	CCxStructTreeNodeFlow* pRoot = GetTreeRoot();
	if (!pRoot)
	{
		return FALSE;
	}

	//��ѯ�Ƿ���������
	//if (!GetProcessStateCanRun())
	//{
	//	return FALSE;
	//}
	//��ѯ�Ƿ���Ա���
	if (!GetProcessStateCanSave())
	{
		return FALSE;
	}

	//���ticketΪ�գ��ҵ����氲װ��־ΪTRUEʱ���ߵ������߼���������������߼�
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
			//ʹ���������棬����Ҫ��¼�ſ�������
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
	//��ICE
	CString sJsonResult = InvokeWsMethod("startLocalModelByByteXpdl", strJsonParams.c_str());
	Json::Value jsonResult;
	Json::Reader reader;
	if (!reader.parse((LPCTSTR)sJsonResult, jsonResult))
	{
		HideLoading();
		CString sMsg;
		sMsg.Format("���÷�����ƹ��̷���ֵ�쳣,%s", sJsonResult);
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
		MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "����ʵ��IDΪ��", g_lpszAppTitle, MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	pMgr->SetRunVar(sUrl, sFlowId);

	return TRUE;
}

BOOL CProcessMgr::OpenRunInNewTab(LPVOID lpParameter)
{
	CProcessMgr* pMgr = (CProcessMgr*)lpParameter;

	//������ִ��ҳ��
	CString sExeDir = GetExecDir();
	CString sExePath = sExeDir + "\\PeraWebContainer.exe";

	//-json {"url" : "www.baidu.com", "user" : "fei", "realusername" : "�Է�", "ticket": "1", "password": "999999", "role": "admin"}
	CString sParam;
	////������û��ƴ��Ʊ��
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
		ZTools::WriteZToolsLog( "CProcessMgr::OpenRunInNewTab g_pawRunProcess ֵ��Ϊ��" );
		return FALSE;
	}
	g_pawRunProcess = new CAttachWindow();
	CString sWebContainerPath = GetExecDir() + "\\PeraPDWebContainer.exe";
	ZTools::WriteZToolsFormatLog( "CProcessMgr::OpenRunInNewTab webcontainer.exe ·��[%s]", sWebContainerPath );
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
		ZTools::WriteZToolsLog( "CProcessMgr::OpenRunInNewTab ����WebContainerʧ��" );
		delete g_pawRunProcess; g_pawRunProcess = NULL;
		return FALSE;
	}
	ZTools::WriteZToolsLog( "CProcessMgr::OpenRunInNewTab ����WebContainer���" );

	return TRUE;
}
BOOL CProcessMgr::OpenRunInModal(LPVOID lpParameter)
{
	//return OpenRunInNewTab(lpParameter);

	CProcessMgr* pMgr = (CProcessMgr*)lpParameter;
	//////////////////////////////////////////////////////////////////////////
	//�������ڷ���Ϣ����ģ̬����	
	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SHOW_RUNFLOW_DIALOG, (WPARAM)(LPCTSTR)pMgr->m_sRunUrl
		, (LPARAM)(LPCTSTR)pMgr->m_sFlowId);

	return TRUE;
}

//��ȡ�ļ���MD5��
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
	//���ý�ģҳ���ѯ����
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
		ZTools::WriteZToolsFormatLog("û��ȡ����ǰ�ڵ�");
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("û�н�ģҳ�棬�޷�����");
		return FALSE;
	}
	
	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;

	//��ѯ�Ƿ���Ա���
	//������Ϊȡ��ǰ�ڵ��Ƿ���Ա���
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

	//���ý�ģҳ�汣��
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
		ZTools::WriteZToolsFormatLog("û��ȡ����ǰ�ڵ�");
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("û�н�ģҳ�棬�޷�����");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;

	//��ѯ�Ƿ���Ա���
	//������Ϊȡ��ǰ�ڵ��Ƿ���Ա���
	if (!GetSubProcessStateCanSave())
	{
		return FALSE;
	}

	CString sSubProcessPath;
	sSubProcessPath.Format("%s\\publish\\%s\\%s.ws", m_sWorkPath, ZTools::GetCurrentTimeString16().c_str(), pNode->m_strOriName);
	MakeSureDirectoryPathExists(sSubProcessPath);

	//���ý�ģҳ�汣��
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
		ZTools::WriteZToolsFormatLog("����������ʱ����������ʧ��%s", sSubProcessPath);
		return FALSE;
	}

	if (!PathFileExists(sSubProcessPath))
	{
		ZTools::WriteZToolsFormatLog("����������ʱ����������ʧ��%s", sSubProcessPath);
		return FALSE;
	}

	//��������Ϣ�ӽṹ����ȡ
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
		ZTools::WriteZToolsFormatLog("û��ȡ����ǰ�ڵ�");
		return FALSE;
	}
	if ( pNode->m_pTree == NULL )
	{
		ZTools::WriteZToolsFormatLog("û��ȡ����ǰ�ڵ��m_pTree");
		return FALSE;
	}
	//pNode->m_pTree->BeforeRunning( pNode );

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("û�н�ģҳ�棬�޷�����");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;
	
	//��ѯ�Ƿ���������
	if (!CanRunSubProcess())
	{
		return FALSE;
	}
	//��ѯ�Ƿ���Ա���
	if (!GetSubProcessStateCanSave())
	{
		return FALSE;
	}

	//���ticketΪ�գ��ҵ����氲װ��־ΪTRUEʱ���ߵ������߼���������������߼�
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
			//ʹ���������棬����Ҫ��¼�ſ�������
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
		ZTools::WriteZToolsFormatLog("û��ȡ����ǰ�ڵ�");
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		ZTools::WriteZToolsFormatLog("û�н�ģҳ�棬�޷�����");
		return FALSE;
	}

	if ( theApp.m_pMainWnd == NULL ) return FALSE;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return FALSE;
	
	if ( !pNode->IsFlowNode() && !pNode->IsRootNode() ) 
	{
		ZTools::WriteZToolsFormatLog("��ǰ�༭�Ľڵ㲻�Ǹ��ڵ�������̽ڵ�");
		return FALSE;
	}

	//���ý�ģҳ��༭
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
	//���ý�ģҳ���ѯ����
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
		std::string strError("RunInServerVersion ��ȡSvgʧ��");
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
		std::string strError("RunInServerVersion ��ȡxpdlʧ��");
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
		std::string strError("ִ����ƹ���ʱ�����߳�ʧ��");
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