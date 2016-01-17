
// DlgParamSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgParamSet.h"
#include "afxdialogex.h"
#include "DlgParamEdit.h"
#include <sstream>
#include "CxStructTreeNodeArrayElement.h"
#include "CxStructTreeNodeParam.h"
#include "CxStructTreeNodeFlow.h"
#include "InvokeWsFunc.h"
#include "PeraProcessDesignerUtility.h"
#include "ZTreeParamSet.h"

// CDlgParamSet 对话框

IMPLEMENT_DYNAMIC(CDlgParamSet, CDlgParamSetBase)

CDlgParamSet::CDlgParamSet(LPCTSTR str, CWnd* pParent /*=NULL*/)
	: CDlgParamSetBase(str, pParent)
{
}

CDlgParamSet::~CDlgParamSet()
{

}

void CDlgParamSet::DoDataExchange(CDataExchange* pDX)
{
	CDlgParamSetBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgParamSet, CDlgParamSetBase)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgParamSet::OnBnClickedButtonDelete)
	//ON_BN_CLICKED(IDOK, &CDlgParamSet::OnBnClickedOk)
	ON_BN_CLICKED(IDOK1, &CDlgParamSet::OnBnClickedOk1)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgParamSet 消息处理程序

BOOL CDlgParamSet::OnInitDialog()
{
	CDlgParamSetBase::OnInitDialog();

	{
		CRect rectStatic;
		GetDlgItem(IDC_STATIC)->GetWindowRect(rectStatic);
		CRect rectWeb;
		GetDlgItem(IDC_STATIC_WEB)->GetWindowRect(rectWeb);
		int nOffset = rectWeb.right - rectStatic.right;
		GetDlgItem(IDC_STATIC_WEB)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_RELATED_GRID)->ShowWindow(SW_HIDE);
		//m_relatedGrid.ShowWindow(SW_HIDE);

		CRect rect;
		GetDlgItem(IDOK1)->GetWindowRect(rect);
		ScreenToClient(rect);
		rect.OffsetRect(- nOffset + 4, 0);
		GetDlgItem(IDOK1)->MoveWindow(rect);

		GetDlgItem(IDCANCEL)->GetWindowRect(rect);
		ScreenToClient(rect);
		rect.OffsetRect(- nOffset + 4, 0);
		GetDlgItem(IDCANCEL)->MoveWindow(rect);

		GetWindowRect(rect);
		rect.right -= nOffset - 2;
		MoveWindow(rect);

		//m_dlgParamEdit.ShowWindow(SW_HIDE);
		{//拉伸
			int nStretchX = 300;
			GetWindowRect(rect);
			rect.right += nStretchX;
			MoveWindow(rect);

			GetDlgItem(IDC_STATIC_PARAM_NAME)->GetWindowRect(rect);
			ScreenToClient(rect);
			rect.right += nStretchX;
			GetDlgItem(IDC_STATIC_PARAM_NAME)->MoveWindow(rect);

			CWnd& m_wndTreeLocation = *GetDlgItem(IDC_STATIC_GRID);
			m_wndTreeLocation.GetWindowRect(rect);
			ScreenToClient(rect);
			rect.right += nStretchX;
			m_wndTreeLocation.MoveWindow(rect);
			//GetDlgItem(IDC_STATIC)->MoveWindow(rect);
			m_wndGridTree.GetWindowRect(rect);
			ScreenToClient(rect);
			rect.right += nStretchX;
			m_wndGridTree.MoveWindow(rect);
			m_wndGridTree.SetColumnWidth(ZTreeParamSet::COL_NAME, 200 + nStretchX);
			m_wndGridTree.AdjustLayout();

			m_dlgParamEdit.GetWindowRect(rect);
			ScreenToClient(rect);
			rect.right += nStretchX;
			m_dlgParamEdit.MoveWindow(rect);
			m_dlgParamEdit.StretchX(nStretchX);

			GetDlgItem(IDOK1)->GetWindowRect(rect);
			ScreenToClient(rect);
			rect.OffsetRect(nStretchX, 0);
			GetDlgItem(IDOK1)->MoveWindow(rect);

			GetDlgItem(IDCANCEL)->GetWindowRect(rect);
			ScreenToClient(rect);
			rect.OffsetRect(nStretchX, 0);
			GetDlgItem(IDCANCEL)->MoveWindow(rect);
		}
	}

	CRect rect;
	m_wndGridTree.GetClientRect(rect);
	m_wndGridTree.SetColumnWidth(ZTreeParamSet::COL_NAME, rect.Width() - 40);
	m_wndGridTree.AdjustLayout();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgParamSet::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

//	CDlgParamSetBase::OnOK();
}

void CDlgParamSet::DeleteRowItem()
{
	CDlgParamSet::OnBnClickedButtonDelete();
}

int CDlgParamSet::DelSelNodes(vector<ZTreeParam*> &vecParams)
{
	vector<ZTreeParam*>::iterator iterTemp;
	vector<ZTreeParam*>allParams;
	vector<int> vecIndex;
	ZTreeParam*pParam = NULL;
	int nIndex = -1;
	CZGridRow* pRow = NULL;
	BOOL bSortFlag = FALSE;

	for(int i=0;i<m_wndGridTree.GetRowCount();i++)
	{
		pRow =(CZGridRow*) m_wndGridTree.GetRow(i);
 		pParam = (ZTreeParam*)pRow->GetData();
		allParams.push_back(pParam);
	}

	for (iterTemp=allParams.begin();iterTemp!=allParams.end();++iterTemp)
	{
		pParam=*iterTemp;
		ZTreeParam*tmppParam = *iterTemp;

		int i = m_wndGridTree.GetRowIDByParam(pParam);
		pRow = (CZGridRow*)m_wndGridTree.GetRow(i);
		if (pParam!=NULL)
		{
			vector<ZTreeParam*>::iterator tmpiter;
			tmpiter =find(vecParams.begin(),vecParams.end(), pParam);
			
			//在vecParams中没有找到说明不是要删除的节点
			if (tmpiter==vecParams.end())
				continue;

			if (pParam->m_bIsGroup)
			{
				int nSubItems = pRow->GetSubItemsCount();
				for (int j=i+1;j<=i+nSubItems;j++)
				{
					pRow =(CZGridRow*) m_wndGridTree.GetRow(j);
					if (pRow==NULL) continue;

					pParam = (ZTreeParam*)pRow->GetData();
					if (pParam==NULL)	continue;

					tmpiter =find(vecParams.begin(),vecParams.end(), pParam);
					int ndelIndex = m_wndGridTree.GetRowIDByParam(pParam);
					if (tmpiter==vecParams.end())//需要移动到外边的参数节点
					{
						nIndex = m_wndGridTree.GetRowCount();
						pRow = (CZGridRow*)m_wndGridTree.GetRow(--nIndex);
						pParam->m_strGroup="";
						pRow->SetData(reinterpret_cast<DWORD_PTR>(pParam));
						m_wndGridTree.UpdateRowByParam(pRow,pParam);
						m_wndGridTree.AddLast();
						bSortFlag = TRUE;
						//m_wndGridTree.RemoveRow(ndelIndex);
						//m_wndGridTree.RebuildIndexes();
						//m_wndGridTree.AdjustLayout();
					}
				}
			}

			pParam=tmppParam;
			m_wndGridTree.m_pRowCur = NULL;
			m_wndGridTree.m_pParamCur = NULL;
			RemoveParam(pParam);
			m_wndGridTree.RemoveRow(i);
			m_wndGridTree.RebuildIndexes();
			m_wndGridTree.AdjustLayout();
		}
	}

	if (bSortFlag)
	{
		m_wndGridTree.UpdateTreeParam();
	}
	
	return 1;
}
int CDlgParamSet::GetDelParamlst(vector<ZTreeParam*> &vecParams,int &nGroupCount)
{
	int  ncount  =0;
	ZTreeParam* pParam  = NULL;
	CBCGPGridItem*pItem = NULL;
	CZGridRow  *pRow =  NULL;
	CList <CBCGPGridItem*, CBCGPGridItem*> lstSelected;
	m_wndGridTree.GetSelectedItems (lstSelected);

	for (POSITION pos = lstSelected.GetHeadPosition(); pos != NULL; )
	{
		pItem = lstSelected.GetAt(pos);
		lstSelected.GetNext(pos);
		pRow = (CZGridRow*)pItem->GetParentRow();
		pParam = (ZTreeParam*)pRow->GetData();

		if (pParam==NULL)	
			continue;

		if (pParam->m_bIsGroup)	
			nGroupCount++;

		ncount++;
		vecParams.push_back(pParam);
	}
	return ncount;
}
void CDlgParamSet::OnBnClickedButtonDelete()
{
	vector<ZTreeParam*>  vecParamslst;
	int nDelCount=0;
	int nGroupCount=0;
	int nParamCount=0;
	nDelCount = GetDelParamlst(vecParamslst,nGroupCount);
	nParamCount = nDelCount - nGroupCount;

	int nRet =-1;
	//选中的节点数
	CZGridRow* pRow = NULL;
	ZTreeParam* pParam =NULL;
	
	CString strTip;
	strTip.Empty();	

	if (nDelCount<=0)return;

	ZTreeParam* pfirstParam =NULL;

	if(nParamCount==1&&nDelCount==1)
	{
		pfirstParam= vecParamslst.at(0);
		strTip.Format("确认要删除参数【%s】吗？", pfirstParam->m_strName.c_str	());
	}
	if(nParamCount>1&&(!nGroupCount))
	{
		strTip.Format("您选中了【%d】个参数，确认都要删除吗?",nParamCount);
	}
	if (nParamCount>=1&&nGroupCount>=1)
	{
		strTip.Format("您选中了【%d】个组，【%d】个参数，确认都要删除吗?",nGroupCount,nParamCount);
	}
	if (nDelCount>=1&&(!nParamCount))
	{
		strTip.Format("您选中了【%d】个组，确认都要删除吗?", nGroupCount);
	}
	
	nRet = MessageBox( strTip, g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
	switch (nRet)
	{
		case IDOK:
		{
			DelSelNodes(vecParamslst);
			m_bParamsChanged = TRUE;
			m_wndGridTree.RebuildIndexes();
			m_wndGridTree.AdjustLayout();
			SelectCurRow(NULL);
			m_dlgParamEdit.Enable(FALSE);
			return;
		}
		break;
	case IDCANCEL:
		default:
		break;
	}
}
CZGridRow* CDlgParamSet::GetNextRowAfterDel( CZGridRow*pCurItem)
{
	CZGridRow* pParaentItem=NULL;
	CZGridRow* pNextItem=NULL;

	CList<CBCGPGridRow*, CBCGPGridRow*> lst;

	//要删除的pRow
	//pCurItem = (CZGridRow*)m_wndGridTree.GetRow(nIndex);

	//if (pCurItem==NULL)		return n;

	pParaentItem = (CZGridRow*)pCurItem->GetParent();
	pParaentItem->GetSubItems(lst);
	POSITION pos =lst.Find(pCurItem);
	lst.GetNext (pos);
	 
	if (pos==NULL)
		pNextItem=pParaentItem;
	else
		pNextItem=(CZGridRow*)lst.GetAt(pos);
	
	return pNextItem;

}

std::string GetLengthString(std::string str) //长度字符串"1,2"
{
	int nStart = str.find_last_of('(');
	int nEnd = str.find_last_of(')');

	if(nEnd <= nStart + 1)
	{
		return "";
	}

	str = str.substr(nStart + 1, nEnd - nStart - 1);

	return str;
}

std::string GetDimString(std::string str) //维度字符串"2"
{
	str = GetLengthString(str);
	int nPos = 0;
	std::string strT;
	for(int i = 0; i < (int)str.length(); i++)
	{
		if(str[i] != ' ' && str[i] != ',')
		{
		}
		else if(str[i] == ',')
		{
			nPos ++;
		}
	}
	{//最后一个
		nPos++;
	}
	stringstream stream;
	stream << nPos;
	return stream.str();
}

void CDlgParamSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int ncount =GetSelMoveParamCount();
	if (ncount==SINGLE_MOVE_NODE)
	m_wndGridTree.UpdateParamFromEditControl();

	UpdateJsonFromTree(); //更新Json

	CCxStructTreeNodeFlow* pRoot = (CCxStructTreeNodeFlow*)theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot();
		
	theApp.m_pBarJianmoTree->m_wndGridTree.m_bModified = TRUE;

	CCxBCGPDialog::OnOK();
}


void CDlgParamSet::OnBnClickedOk1()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOk();
	//CDlgParamSetBase::OnOK();
}


void CDlgParamSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDlgParamSetBase::OnPaint()
	CRect rect;
	
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rect);
	ScreenToClient(rect);
	CRect rectBtn;
	m_btnDelete.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);
	//m_imageBarParam.Draw2(dc.GetSafeHdc(), rect.left + 1, rectBtn.top - 4, rect.Width() - 2, rectBtn.Height() + 7);

	CRect rectWeb;
	GetDlgItem(IDC_STATIC_WEB)->GetWindowRect(rectWeb);
	ScreenToClient(rectWeb);
	CRect rectDraw;
	rectDraw.left = rect.left + 1;
	rectDraw.right = rect.right - 1;
	rectDraw.top = rectWeb.bottom + 38;
	rectDraw.bottom = rect.bottom - 1;
	CBrush brush(RGB(251, 251, 251));
	dc.FillRect (rectDraw, &brush);

	CPen pen(0, 1, RGB(151,151,151));
	dc.SelectObject(pen);
	CRect rectParamName;
	GetDlgItem(IDC_STATIC_PARAM_NAME)->GetWindowRect(rectParamName);
	ScreenToClient(rectParamName);
	dc.MoveTo(rectDraw.left, rectParamName.top - 7);
	dc.LineTo(rectDraw.right, rectParamName.top - 7);

	//dc.SetTextColor( RGB(0, 0, 0) );         //画文字
	CFont* pFont = GetFont();
	dc.SelectObject(pFont);
	dc.SetBkMode( TRANSPARENT );
	CRect rectStrDelete, rectStrAdd;
	GetDlgItem(IDC_STATIC_DELETE)->GetWindowRect(rectStrDelete);
	ScreenToClient(rectStrDelete);
	GetDlgItem(IDC_STATIC_ADD)->GetWindowRect(rectStrAdd);
	ScreenToClient(rectStrAdd);
	SetFont(GetParent()->GetFont());
// 	dc.DrawText( "删除", &rectStrDelete, DT_SINGLELINE | DT_CENTER
// 		| DT_VCENTER | DT_END_ELLIPSIS);
// 	dc.DrawText( "增加", &rectStrAdd, DT_SINGLELINE | DT_CENTER
// 		| DT_VCENTER | DT_END_ELLIPSIS);	
}



void CDlgParamSet::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDlgParamSetBase::OnLButtonDown(nFlags, point);
}
