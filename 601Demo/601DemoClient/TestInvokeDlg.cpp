// TestInvokeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "601DemoClient.h"
#include "TestInvokeDlg.h"
#include "afxdialogex.h"
#include <string>
using namespace  std;

struct CThreadDataInvoke
{
	string m_strFuncName;
	string m_strFunParam;
	InvokeServerMethod * m_pInvokeObj;
};

DWORD WINAPI ThreadFunInvokeUsrDefine(LPVOID pData)
{
	CThreadDataInvoke * pInvokeData = (CThreadDataInvoke*)pData;
	if (pInvokeData==NULL)
		return 1;
	if (pInvokeData->m_pInvokeObj == NULL)
	{
		delete pInvokeData;
		return 2;
	}

	theApp.GetMainWnd()->EnableWindow(FALSE);

	pInvokeData->m_pInvokeObj->InvokeMethod(pInvokeData->m_strFuncName,pInvokeData->m_strFunParam);
	theApp.TraceRunLog("-------------------��ɲ���--------------");
	theApp.GetMainWnd()->EnableWindow(TRUE);
	delete pInvokeData;
	return 0;
}

// CTestInvokeDlg �Ի���

IMPLEMENT_DYNAMIC(CTestInvokeDlg, CDialogEx)

CTestInvokeDlg::CTestInvokeDlg(InvokeServerMethod * pInvokeObj,CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestInvokeDlg::IDD, pParent)
	, m_strFunName(_T(""))
	, m_strParam(_T(""))
	,m_pInvokeObj(pInvokeObj)
{

}

CTestInvokeDlg::~CTestInvokeDlg()
{
}

void CTestInvokeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FUNC_NAME, m_strFunName);
	DDV_MaxChars(pDX, m_strFunName, 30);
	DDX_Text(pDX, IDC_EDIT_FUCN_PARAM, m_strParam);
}


BEGIN_MESSAGE_MAP(CTestInvokeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTestInvokeDlg::OnBnClickedOk)
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CTestInvokeDlg ��Ϣ�������


void CTestInvokeDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_pInvokeObj == NULL)
		return;
	if (m_strFunName.IsEmpty())
	{
		MessageBox("Զ�̷���������Ϊ�գ�","��ʾ",MB_OK|MB_ICONINFORMATION);
		return;
	}
	CThreadDataInvoke *pDataInvoke = new CThreadDataInvoke;
	pDataInvoke->m_pInvokeObj  = m_pInvokeObj;
	pDataInvoke->m_strFuncName = m_strFunName;
	pDataInvoke->m_strFunParam = m_strParam;

	//GetDlgItem(IDOK)->EnableWindow(FALSE);
	HANDLE h =  CreateThread(NULL,0,ThreadFunInvokeUsrDefine,(LPVOID)pDataInvoke,0,NULL);
	//WaitForSingleObject(h,INFINITE);
	//GetDlgItem(IDOK)->EnableWindow(TRUE);
	//delete pDataInvoke;
	//CDialogEx::OnOK();
}


void CTestInvokeDlg::OnNcDestroy()
{
	CDialogEx::OnNcDestroy();

	// TODO: �ڴ˴������Ϣ����������
	delete this;
}
