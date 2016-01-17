// DlgNDArrayResize.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgNDArrayResize.h"
#include "afxdialogex.h"
#include "CStringChecker.h"
#include "CxArrayND.h"
#include "PeraProcessDesignerUtility.h"

static const int MAX_ELEMENT_COUNT = 10 * 1000;

static CString g_strOriDim1;
static CString g_strOriDim2;
static CString g_strOriDim3;

// CDlgNDArrayResize 对话框

BEGIN_MESSAGE_MAP(CDlgNDArrayResize, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgNDArrayResize::OnBnClickedOk)
	ON_BN_CLICKED(IDC_NDAER_OK, &CDlgNDArrayResize::OnBnClickedNdaerOk)
	ON_BN_CLICKED(IDC_NDAER_CANCEL, &CDlgNDArrayResize::OnBnClickedNdaerCancel)
END_MESSAGE_MAP()

void CDlgNDArrayResize::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NDAER_DIM1, m_wndDim1);
	DDX_Control(pDX, IDC_NDAER_DIM2, m_wndDim2);
	DDX_Control(pDX, IDC_NDAER_DIM3, m_wndDim3);
	DDX_Control(pDX, IDC_NDAER_OK, m_wndOk);
	DDX_Control(pDX, IDC_NDAER_CANCEL, m_wndCancel);
}

IMPLEMENT_DYNAMIC(CDlgNDArrayResize, CDialogEx)

CDlgNDArrayResize::CDlgNDArrayResize(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNDArrayResize::IDD, pParent)
{
	m_pArrayData = NULL;
}

CDlgNDArrayResize::~CDlgNDArrayResize()
{
}

void CDlgNDArrayResize::OnBnClickedOk()
{
	//按 Enter 不关闭对话框
}

BOOL CDlgNDArrayResize::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);

	CString str;

	if ( m_pArrayData != NULL )
	{
		CxArrayND * pNDArray = ( CxArrayND* )m_pArrayData;
		int nDimCount = pNDArray->GetDimCount();
		
		switch ( nDimCount )
		{
		case 3:
			str.Format("%d", pNDArray->GetDataCount(0) );
			m_wndDim1.SetWindowText( str );
			g_strOriDim1 = str;
			str.Format("%d", pNDArray->GetDataCount(1) );
			g_strOriDim2 = str;
			m_wndDim2.SetWindowText( str );
			str.Format("%d", pNDArray->GetDataCount(2) );
			g_strOriDim3 = str;
			m_wndDim3.SetWindowText( str );
			break;

		case 2:
			str.Format("%d", pNDArray->GetDataCount(0) );
			g_strOriDim1 = str;
			m_wndDim1.SetWindowText( str );
			str.Format("%d", pNDArray->GetDataCount(1) );
			g_strOriDim2 = str;
			m_wndDim2.SetWindowText( str );
			
			m_wndDim3.EnableWindow(FALSE);
			break;

		case 1:
			str.Format("%d", pNDArray->GetDataCount(0) );
			g_strOriDim1 = str;
			m_wndDim1.SetWindowText( str );

			m_wndDim3.EnableWindow(FALSE);
			m_wndDim2.EnableWindow(FALSE);
			break;

		default:
			m_wndDim3.EnableWindow(FALSE);
			m_wndDim2.EnableWindow(FALSE);
			m_wndDim1.EnableWindow(FALSE);
			break;
		}
	}

	return TRUE;
}

void CDlgNDArrayResize::OnBnClickedNdaerOk()
{
	BOOL bModified = FALSE;

	if ( m_pArrayData == NULL )
	{
		EndDialog(FALSE); return;
	}

	vector<CString> vDimLen;
	int nDimCount = m_pArrayData->GetDimCount();
	CString str;

	switch ( nDimCount )
	{
	case 3:
		m_wndDim1.GetWindowText(str);
		if (!CStringChecker::CheckInt(str))
		{
			MessageBox("请输入整型数据", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
			return;
		}
		if ( g_strOriDim1 != str ) bModified = TRUE;
		vDimLen.push_back(str);
		m_wndDim2.GetWindowText(str);
		if (!CStringChecker::CheckInt(str))
		{
			MessageBox("请输入整型数据", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
			return;
		}
		if ( g_strOriDim2 != str ) bModified = TRUE;
		vDimLen.push_back(str);
		m_wndDim3.GetWindowText(str);
		if (!CStringChecker::CheckInt(str))
		{
			MessageBox("请输入整型数据", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
			return;
		}
		if ( g_strOriDim3 != str ) bModified = TRUE;
		vDimLen.push_back(str);
		break;

	case 2:
		m_wndDim1.GetWindowText(str);
		if (!CStringChecker::CheckInt(str))
		{
			MessageBox("请输入整型数据", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
			return;
		}
		if ( g_strOriDim1 != str ) bModified = TRUE;
		vDimLen.push_back(str);
		m_wndDim2.GetWindowText(str);
		if (!CStringChecker::CheckInt(str))
		{
			MessageBox("请输入整型数据", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
			return;
		}
		if ( g_strOriDim2 != str ) bModified = TRUE;
		vDimLen.push_back(str);
		break;

	case 1:
		m_wndDim1.GetWindowText(str);
		if (!CStringChecker::CheckInt(str))
		{
			MessageBox("请输入整型数据", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
			return;
		}
		if ( g_strOriDim1 != str ) bModified = TRUE;
		vDimLen.push_back(str);
		break;

	default:
		EndDialog(FALSE); return;
	}

	if ( !bModified )
	{
		EndDialog(FALSE); return;
	}

	vector<int> vNewSize;
	for (int i=0; i<(int)vDimLen.size(); i++)
	{
		int nLen = strtol((char *)(LPCTSTR)vDimLen[i], NULL, 10);
		vNewSize.push_back(nLen);
	}

	//检查最大值
	int nNewCount = 1;
	for (int i=0;i<(int)vNewSize.size();i++)
	{
		if ( nNewCount > MAX_ELEMENT_COUNT )
		{
			nNewCount = -1; break;
		}
		else if ( vNewSize[i] < 0 )
		{
			nNewCount = -1; break;
		}
		else
		{
			nNewCount *= (int)vNewSize[i];
			if ( nNewCount > MAX_ELEMENT_COUNT )
			{
				nNewCount = -1; break;
			}
		}
	}
	if ( nNewCount > MAX_ELEMENT_COUNT || nNewCount < 0 )
	{

		CString strMsg;
		strMsg.Format("元素个数超过最大值(%d个)，请重新设置", MAX_ELEMENT_COUNT);
		MessageBox(strMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION); return;
	}

	CxArrayND * pNDArray = reinterpret_cast< CxArrayND * > ( m_pArrayData );
	pNDArray->Resize( vNewSize );

	EndDialog( bModified ); return;
}


void CDlgNDArrayResize::OnBnClickedNdaerCancel()
{
	EndDialog( FALSE ); return;
}
