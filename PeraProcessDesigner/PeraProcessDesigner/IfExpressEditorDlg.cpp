// IfExpressEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "IfExpressEditorDlg.h"
#include "afxdialogex.h"
#include "CxStructTreeNodeParam.h"
#include "CStringChecker.h"
#include "Tools.h"
#include "UtilityFunc.h"
#include "ConvertData.h"
// CIfExpressEditorDlg 对话框
BOOL GetBoolValueFromString(CString str,BOOL & bOut)
{
	str.MakeLower();
	if ( str.Compare("t")==0||
		str.Compare("true")==0||
		str.Compare("y")==0||
		str.Compare("yes")==0||
		str.Compare("是")==0||
		str.Compare("1")==0)
	{
		bOut = TRUE;
		return TRUE;
	}

	else if (str.Compare("f")==0||
		str.Compare("false")==0||
		str.Compare("n")==0||
		str.Compare("no")==0||
		str.Compare("not")==0||
		str.Compare("否")==0||
		str.Compare("0")==0)
	{
		bOut = FALSE;
		return TRUE;
	}
	else
		return FALSE;
}
static inline BOOL IsValidNum(const CString & str,int iMax,int iMin,CString &strErrMsg)
{
	strErrMsg = "";
	CString strTmp = str;
	strTmp.Trim();
	if (strTmp.Compare("*")==0)
	{
		strErrMsg = "数组下标*不是整型数字";
		return FALSE;
	}

	if (!CStringChecker::CheckInt(strTmp))
	{
		strErrMsg = "数组下标不是整型数字";
		return FALSE;
	}
	int iNum  = atoi(strTmp);
	if (iNum > iMax || iNum < iMin)
	{
		strErrMsg = "数组下标越界";
		return FALSE;
	}

	return TRUE;
}
static bool IsValidArryMapParam(const CString & strText,vector<int> vArryDim,CString &strErrMsg)
{
	int iLen = strText.GetLength();
	if (iLen < 3)
	{
		strErrMsg = "下标字符串不完整";
		return false; //[*] 至少三个字符
	}

	if (strText.Left(1)!="[")
	{
		strErrMsg = "下标字符串首字符不是\"[\"";
		return false;
	}

	if (strText.Right(1)!="]")
	{
		strErrMsg = "下标字符串尾字符不是\"]\"";
		return false;
	}

	CString strCenterPart = strText.Mid(1,iLen-2); // 数字和分隔符部分
	strCenterPart.Trim();
	vector<string> vSplitNum;
	string strTmp = strCenterPart;
	string strDelim = ",";
	ZTools::split(strTmp,strDelim,&vSplitNum);
	if (vSplitNum.size()!=vArryDim.size())
	{
		strErrMsg = "下标维度定义与参数实际维度类型不一致";
		return false;
	}
	for (size_t i=0;i<vArryDim.size();i++)
	{
		if (!IsValidNum(vSplitNum[i].c_str(),vArryDim[i]-1,0,strErrMsg))
			return false;
	}
	return true;
}

IMPLEMENT_DYNAMIC(CIfExpressEditorDlg, CCxBCGPDialog)

CIfExpressEditorDlg::CIfExpressEditorDlg(const BranchExpress &Express,CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CIfExpressEditorDlg::IDD, pParent)
{
	m_Express.Clear();
	m_Express = Express;
	m_iCheckType= -1;
	m_pNodeLeft = NULL;
	m_pNodeRight = NULL;
	m_iLastComboTypeSel = TP_PARAM; 
	m_bDeleted = FALSE;
	m_bIfNotSubFlow = TRUE;
	m_PointTreeLeaveFocus = CPoint(-1,-1);
	m_iTickCountTreeLeaveFocus = 0;
}

CIfExpressEditorDlg::~CIfExpressEditorDlg()
{
}

void CIfExpressEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LEFT, m_EditLeft);
	DDX_Control(pDX, IDC_EDIT_LEFT_ARRY, m_EditLeftArry);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, m_ComboSymbol);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboRightType);
	DDX_Control(pDX, IDC_EDIT_RIGHT, m_EditRight);
	DDX_Control(pDX, IDC_EDIT_RIGHT_ARRY, m_EditRightArry);
}


BEGIN_MESSAGE_MAP(CIfExpressEditorDlg, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CIfExpressEditorDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CIfExpressEditorDlg::OnCbnSelchangeComboType)
	ON_MESSAGE(WM_STRUCTTREEE_LDLK,&CIfExpressEditorDlg::OnMsgStructSelectParam)
	ON_MESSAGE(WM_STRUCTTREEE_LEAVEFOCUS,&CIfExpressEditorDlg::OnMsgStructLeaveFocus)
	ON_MESSAGE(WM_EDIT_AUTOCHECK,&CIfExpressEditorDlg::OnCheckEidtInput)
	ON_BN_CLICKED(IDC_BTN_DEL_IF_EXPRESS, &CIfExpressEditorDlg::OnBnClickedBtnDelIfExpress)
END_MESSAGE_MAP()


// CIfExpressEditorDlg 消息处理程序

BOOL CIfExpressEditorDlg::InitDlgCtrl()
{
	if (!m_Express.m_LeftParam.strID.IsEmpty() && m_Express.m_LeftParam.strConst.IsEmpty())
	{
		CString strText = m_Express.m_LeftParam.strParamFullPath;
		if (!m_Express.m_LeftParam.strArry.IsEmpty()) // 存在数组下标
		{
			strText.TrimRight(m_Express.m_LeftParam.strArry); //将数组下标显示到专门的编辑框
			m_EditLeftArry.ShowWindow(SW_SHOW);
			m_EditLeftArry.SetWindowText(m_Express.m_LeftParam.strArry);
		}
		else
		{
			m_EditLeftArry.ShowWindow(SW_HIDE);
			AdjustLeftEditSize();
		}
		m_EditLeft.SetWindowText(strText); // 变参
		GetDataFullPathByID(m_Express.m_LeftParam.strID,&m_pNodeLeft); //用于后面取数组维度，判断是否下标越界和常量类型的检测
		GetCheckDataType(m_pNodeLeft);
	}
	else
	{
		m_EditLeft.SetWindowText(m_Express.m_LeftParam.strConst); // 常参
	}
	
	if (!m_Express.m_RightParam.strID.IsEmpty() && m_Express.m_RightParam.strConst.IsEmpty())
	{
		CString strText = m_Express.m_RightParam.strParamFullPath;
		if (!m_Express.m_RightParam.strArry.IsEmpty()) // 存在数组下标
		{
			strText.TrimRight(m_Express.m_RightParam.strArry); //将数组下标显示到专门的编辑框
			m_EditRightArry.ShowWindow(SW_SHOW);
			m_EditRightArry.SetWindowText(m_Express.m_RightParam.strArry);
		}
		else
		{
			m_EditRightArry.ShowWindow(SW_HIDE);
			AdjustRightEditSize();
		}
		m_EditRight.SetWindowText(strText); // 变参
		m_strLastRightEditParam = strText;
		m_ComboRightType.SetCurSel(TP_PARAM);
		m_iLastComboTypeSel = TP_PARAM;
		GetDataFullPathByID(m_Express.m_RightParam.strID,&m_pNodeRight); //用于后面取数组维度，判断是否下标越界
	}
	else
	{
		if (m_iCheckType==DT_STRING)
		{
			ASSERT(m_Express.m_RightParam.strConst.GetLength()>=2);
			ASSERT(m_Express.m_RightParam.strConst.Left(1)=="\""); 
			ASSERT(m_Express.m_RightParam.strConst.Right(1)=="\"");
			// 显示字符串类型的表达式时，左右引号不显示
			m_Express.m_RightParam.strConst = m_Express.m_RightParam.strConst.Mid(1,m_Express.m_RightParam.strConst.GetLength()-2);
			/*m_Express.m_RightParam.strConst.Replace("\\\"","\"");*/
		}
		m_EditRight.SetWindowText(m_Express.m_RightParam.strConst); // 常参
		m_strLastRightEditConst = m_Express.m_RightParam.strConst;
		m_ComboRightType.SetCurSel(TP_CONST);
		m_iLastComboTypeSel = TP_CONST;
		m_EditRightArry.ShowWindow(SW_HIDE);
		AdjustRightEditSize();
		m_EditRight.SetReadOnly(FALSE);
		GetCheckDataType(m_pNodeLeft);
	}
	if (m_Express.m_strIdExpress.IsEmpty())
	{
		DeleteExpress();
	}
	UpdateComboBoxSymbol();
	int iCurSel = -1;
	int iCount = m_ComboSymbol.GetCount();
	for (int i=0;i<iCount;i++)
	{
		CString strTextCombo;
		m_ComboSymbol.GetLBText(i,strTextCombo);
		if (strTextCombo.Compare(m_Express.m_strExpressSymbol)==0)
		{
			iCurSel = i;
			break;
		}
	}
	m_ComboSymbol.SetCurSel(iCurSel);
	m_EditLeftArry.SetMsgProWnd(GetSafeHwnd()); // 设计错误检测处理窗口为本对话框
	m_EditRightArry.SetMsgProWnd(GetSafeHwnd());
	m_EditRight.SetMsgProWnd(GetSafeHwnd());
	return TRUE;
}
void  CIfExpressEditorDlg::AdjustRightEditSize()
{
	CRect RectEdit;
	m_EditRight.GetWindowRect(RectEdit);
	BOOL bIsHide = (m_EditRightArry.GetStyle() & SW_HIDE);
	int iWidth = m_EditRightArry.IsWindowVisible() ? 210:276;
	m_EditRight.SetWindowPos(NULL,0,0,iWidth,RectEdit.Height(),SWP_NOZORDER|SWP_NOMOVE); // 调整控件的宽度
}
void CIfExpressEditorDlg:: AdjustLeftEditSize()
{
	CRect RectEdit;
	m_EditLeft.GetWindowRect(RectEdit);
	BOOL bIsHide = (m_EditLeftArry.GetStyle() & SW_HIDE);
	int iWidth = m_EditLeftArry.IsWindowVisible() ? 210:276;
	m_EditLeft.SetWindowPos(NULL,0,0,iWidth,RectEdit.Height(),SWP_NOZORDER|SWP_NOMOVE);  // 调整控件的宽度
}
BOOL CIfExpressEditorDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	ModifyStyle( WS_THICKFRAME, 0, SWP_FRAMECHANGED|SWP_DRAWFRAME );
	// TODO:  在此添加额外的初始化
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE); // Set big icon
	if (!InitDlgCtrl())
	{
		MessageBox("解析输入条件表达式失败！",g_lpszAppTitle,MB_OK|MB_ICONERROR);
		EndDialog(0);
	}

	InitTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void  CIfExpressEditorDlg::InitTree()
{
	m_wndTreeDropDown.Create(m_wndTreeDropDown.IDD,GetDesktopWindow()); // 下拉显示树的对话框父窗口设置为桌面，保证在树外任何地方点击都能使树失去焦点
	m_wndTreeDropDown.m_IfEditTree.SetMessReveiveWnd(GetSafeHwnd()); // 设置消息树双击消息接收窗口
}

void CIfExpressEditorDlg::OnBnClickedOk()
{
	// 从控件获取新的数组下标值
	int iComboSymbolSel = m_ComboSymbol.GetCurSel();
	if (iComboSymbolSel==-1 && m_bDeleted && m_Express.m_LeftParam.strID.IsEmpty()
		 && m_ComboRightType.GetCurSel()==TP_PARAM && m_Express.m_RightParam.strID.IsEmpty())
		return CCxBCGPDialog::OnOK();

	if (iComboSymbolSel==-1)
	{
		MessageBox("运算符未指定!",g_lpszAppTitle,MB_OK|MB_ICONERROR);
		return;
	}
	m_ComboSymbol.GetLBText(iComboSymbolSel,m_Express.m_strExpressSymbol);
	if ((ComboParamType)m_ComboRightType.GetCurSel() == TP_CONST)
	{
		if (!CheckGetConst(FALSE))
			return;
	}
	CString strErrMsg;
	if (!m_Express.m_LeftParam.strArry.IsEmpty()) //有数组下标
	{
		if (!CheckMapArry(m_pNodeLeft,TRUE,strErrMsg))
		{
			MessageBox(strErrMsg,g_lpszAppTitle,MB_OK|MB_ICONERROR);
			m_EditLeftArry.ChangeBkColor(true);
			m_EditLeftArry.SetFocus();
			return;
		}
		// 从控件获取新的数组下标值
		m_EditLeftArry.GetWindowText(m_Express.m_LeftParam.strArry);
		m_Express.m_LeftParam.strArry.Replace(" ",""); // 去掉数组里面的空格[ 0, 0 ] -> [0,0]
	}

	if (!m_Express.m_RightParam.strArry.IsEmpty())
	{
		if (!CheckMapArry(m_pNodeRight,FALSE,strErrMsg))
		{
			MessageBox(strErrMsg,g_lpszAppTitle,MB_OK|MB_ICONERROR);
			m_EditRightArry.ChangeBkColor(true);
			m_EditRightArry.SetFocus();
			return;
		}
		// 从控件获取新的数组下标值
		m_EditRightArry.GetWindowText(m_Express.m_RightParam.strArry);
		m_Express.m_RightParam.strArry.Replace(" ",""); // 去掉数组里面的空格[ 0, 0 ] -> [0,0]
	}

	m_Express.m_LeftParam.strParamFullPath = GetDataFullPathByID(m_Express.m_LeftParam.strID,NULL) + m_Express.m_LeftParam.strArry;
	if (m_Express.m_LeftParam.strParamFullPath.IsEmpty())
	{
		MessageBox("运算符左侧参数未指定!",g_lpszAppTitle,MB_OK|MB_ICONERROR);
		m_EditLeft.ChangeBkColor(true);
		m_EditLeft.SetFocus();
		return;
	}

	CString strRightPart;
	if (m_ComboRightType.GetCurSel()==TP_PARAM)
	{
		m_Express.m_RightParam.strParamFullPath = GetDataFullPathByID(m_Express.m_RightParam.strID,NULL) + m_Express.m_RightParam.strArry;
		strRightPart = m_Express.m_RightParam.strParamFullPath;
	}
	else if (m_ComboRightType.GetCurSel()==TP_CONST)
	{
		strRightPart = m_Express.m_RightParam.strConst;

		if (m_iCheckType== DT_BOOLEAN && !strRightPart.IsEmpty())
		{
			BOOL bValueUserInPut;
			if (!GetBoolValueFromString(strRightPart,bValueUserInPut))
			{
				MessageBox("浮点常量错误!",g_lpszAppTitle,MB_OK|MB_ICONERROR);
				m_EditRight.ChangeBkColor(true);
				m_EditRight.SetFocus();
				return;
			}
			strRightPart = bValueUserInPut ? "true":"false";
			m_Express.m_RightParam.strConst = strRightPart;
		}
	}

	if (strRightPart.IsEmpty())
	{
		if (m_iCheckType!=DT_STRING || (m_iCheckType==DT_STRING && m_ComboRightType.GetCurSel()==TP_PARAM) )
		{
			MessageBox("运算符右侧参数未指定!",g_lpszAppTitle,MB_OK|MB_ICONERROR);
			m_EditRight.ChangeBkColor(true);
			m_EditRight.SetFocus();
			return;
		}
	}
	if (m_ComboRightType.GetCurSel()==TP_PARAM) // 校验左右类型是否一致
	{ 
		int iLeftType = m_iCheckType; // 保存左侧类型
		GetCheckDataType(m_pNodeRight);
		int iRightType = m_iCheckType;
		m_iCheckType = iLeftType;
		if (iRightType!=iLeftType) // 运算符左右类型不一致
		{
			MessageBox("运算符两侧参数类型不一致!",g_lpszAppTitle,MB_OK|MB_ICONINFORMATION);
			return;
		}
	}

	if (m_iCheckType==DT_STRING && m_ComboRightType.GetCurSel()==TP_CONST) // 字符串类型常量需加引号
	{
		strRightPart = CString("\"") + strRightPart +  CString("\"");
	}
	if (m_iCheckType==DT_DOUBLE && m_ComboRightType.GetCurSel()==TP_CONST) // double型格式化
	{
		strRightPart = CStringChecker::FormatDouble(strRightPart);
	}
	if (m_iCheckType==DT_INT && m_ComboRightType.GetCurSel()==TP_CONST) // int型格式化
	{
		int iTmp = atoi(strRightPart);
		strRightPart.Format("%d",iTmp);
	}
	m_Express.m_strFullPathExpress.Format("%s%s%s",m_Express.m_LeftParam.strParamFullPath,m_Express.m_strExpressSymbol,strRightPart);

	CString strLeftPart;
	strLeftPart.Format("#{%s%s}",m_Express.m_LeftParam.strID,m_Express.m_LeftParam.strArry);
	if (m_ComboRightType.GetCurSel()==TP_CONST)
		m_Express.m_RightParam.strConst = strRightPart;
	else
		strRightPart.Format("#{%s%s}",m_Express.m_RightParam.strID,m_Express.m_RightParam.strArry);

	m_Express.m_strIdExpress.Format("%s%s%s",strLeftPart,m_Express.m_strExpressSymbol,strRightPart);
	//去掉多余的属性
	if (m_ComboRightType.GetCurSel()==TP_PARAM)
	{
		m_Express.m_RightParam.strConst.Empty();
	}
	else if (m_ComboRightType.GetCurSel()==TP_CONST)
	{
		m_Express.m_RightParam.strID.Empty();
		m_Express.m_RightParam.strArry.Empty();
		m_Express.m_RightParam.strParamFullPath.Empty();
	}

	CCxBCGPDialog::OnOK();
}

void CIfExpressEditorDlg::OnCbnSelchangeComboType()
{
	// TODO: 在此添加控件通知处理程序代码
	int iSelIndex = m_ComboRightType.GetCurSel();
	if (iSelIndex==m_iLastComboTypeSel)
		return; // 选择的未变化

	if (TP_PARAM== iSelIndex) // 节点参数
	{
		if (TP_CONST==m_iLastComboTypeSel)
			m_EditRight.GetWindowText(m_strLastRightEditConst);

		m_EditRight.SetReadOnly(TRUE);
		m_EditRight.SetWindowText(m_strLastRightEditParam);

		if (m_Express.m_RightParam.strArry.IsEmpty())
		{
			if (m_strLastRightEditParam.IsEmpty()) //如果编辑框为空，认为有错误
			{
				m_EditRight.ChangeBkColor(true);
				m_EditRight.SetFocus();
			}
			else
				m_EditRight.ChangeBkColor(false);

			m_EditRightArry.ShowWindow(SW_HIDE);
		}
		else
		{
			bool bErr = OnCheckEidtInput(0,(LPARAM)&m_EditRightArry)==S_OK ? false:true;
			m_EditRightArry.ChangeBkColor(bErr);
			m_EditRightArry.ShowWindow(SW_SHOW);
			if (!m_Express.m_RightParam.strID.IsEmpty())
				m_EditRight.ChangeBkColor(false);
		}
		AdjustRightEditSize();
	}
	else if(TP_CONST==iSelIndex)// 常量参数
	{
		if (TP_PARAM==m_iLastComboTypeSel)
			m_EditRight.GetWindowText(m_strLastRightEditParam);

		m_EditRight.SetReadOnly(FALSE);
		m_EditRight.SetWindowText(m_strLastRightEditConst);
		m_EditRightArry.ShowWindow(SW_HIDE);
		AdjustRightEditSize();
		GetCheckDataType(m_pNodeLeft); // 常数的时候，需要根据左边类型进行合法性检测
		CheckGetConst(TRUE);
	}
	else
	{
		return;
	}
	m_iLastComboTypeSel = (ComboParamType)iSelIndex;
}
HRESULT CIfExpressEditorDlg::OnMsgStructLeaveFocus(WPARAM,LPARAM lp)
{
	m_wndTreeDropDown.ShowWindow(SW_HIDE);
	GetCursorPos(&m_PointTreeLeaveFocus);
	m_iTickCountTreeLeaveFocus = GetTickCount();
	return S_OK;
}
HRESULT CIfExpressEditorDlg::OnMsgStructSelectParam(WPARAM,LPARAM lp)
{
	CCxStructTreeNode *pNode = reinterpret_cast< CCxStructTreeNode * >(lp);

	CCxStructTreeNode *pNodeBackUpTmp = NULL;
	if (IDC_EDIT_LEFT == m_iFocusEidtPCtrlID)
	{
		pNodeBackUpTmp = m_pNodeLeft;
		m_pNodeLeft = pNode;
		GetCheckDataType(m_pNodeLeft); // 左侧节点重新选择后，需更新校验类型
		if (m_ComboRightType.GetCurSel()==TP_CONST)
			CheckGetConst(TRUE); // 切换类型后就检查
	}
	else if (IDC_EDIT_RIGHT == m_iFocusEidtPCtrlID)
	{
		pNodeBackUpTmp = m_pNodeRight;
		m_pNodeRight = pNode;
	}
	CString strText;
	BOOL bHaveArry = FALSE;
	if (pNode->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY) // 数组参数ID需要去掉后面的元素名称，否则全路径找不到
	{
		bHaveArry = TRUE;
		strText = pNode->m_strText; //获取数组下标
		while(pNode && pNode->m_NodeType==CCxStructTreeNode::NT_PARAM_ARRAY)
				pNode = pNode->m_pParent;
	}
	CString strParamID = pNode->m_strID;
	CString strFullPath = GetDataFullPathByID(strParamID,NULL); // 获取参数全路径
	CString strArry = "";
	if (bHaveArry)
	{
		// 以下代码从带下标的数组参数中获取下标部分，例如doubel-数组.test[10]得到[10]
		strArry = strText;
		CString strParamName = pNode->m_strOriName;
		CStringW strWArry		= LPCWSTR(_bstr_t(strArry));
		CStringW strWParamName  = LPCWSTR(_bstr_t(strParamName));
		strWArry.TrimLeft(strWParamName); //得到只剩数组下标的部分
		strArry = LPCTSTR(_bstr_t(strWArry));
	}
	if (IDC_EDIT_LEFT == m_iFocusEidtPCtrlID) //给左编辑框赋值
	{
		UpdateComboBoxSymbol(); // 根据节点类型调整运算符
		m_Express.m_LeftParam.Clear();
		m_Express.m_LeftParam.strID = strParamID; // 记录参数ID
		m_Express.m_LeftParam.strParamFullPath = strFullPath;	// 记录参数全路径
		m_EditLeft.SetWindowText(strFullPath);
		if (bHaveArry)
		{
			m_Express.m_LeftParam.strArry = strArry; //记录选择的数组下标
			m_EditLeftArry.SetWindowText(strArry);
			m_EditLeftArry.ShowWindow(SW_SHOW);
			m_EditLeftArry.ChangeBkColor(false);
		}
		else
			m_EditLeftArry.ShowWindow(SW_HIDE);

		AdjustLeftEditSize();
		if (bHaveArry)
			m_EditLeftArry.SetFocus();
		else
			m_EditLeft.SetFocus();

		m_EditLeft.ChangeBkColor(false);
	}
	else if (IDC_EDIT_RIGHT == m_iFocusEidtPCtrlID)
	{
		int iTempCheck = m_iCheckType;
		GetCheckDataType(pNode);
		int iCurType = m_iCheckType;
		m_iCheckType = iTempCheck;
		if (iCurType!=m_iCheckType && m_iCheckType!=-1) // 运算符左右类型不一致
		{
			MessageBox("所选节点与运算符左侧参数类型不一致!",g_lpszAppTitle,MB_OK|MB_ICONINFORMATION);
			m_pNodeRight = pNodeBackUpTmp;
			return S_OK;
		}
		m_Express.m_RightParam.Clear();
		m_Express.m_RightParam.strID = strParamID; // 记录参数ID
		m_Express.m_RightParam.strParamFullPath = strFullPath;	// 记录参数全路径
		m_EditRight.SetWindowText(strFullPath);
		if (bHaveArry)
		{
			m_Express.m_RightParam.strArry = strArry; //记录选择的数组下标
			m_EditRightArry.SetWindowText(strArry);
			m_EditRightArry.ShowWindow(SW_SHOW);
			m_EditRightArry.ChangeBkColor(false);
		}
		else
			m_EditRightArry.ShowWindow(SW_HIDE);

		AdjustRightEditSize();
		if (bHaveArry)
			m_EditRightArry.SetFocus();
		else
			m_EditRight.SetFocus();

		m_EditRight.ChangeBkColor(false); // 重置错误状态
		m_EditRightArry.ChangeBkColor(false);
	}
	m_wndTreeDropDown.ShowWindow(SW_HIDE);
	return S_OK;
}
LRESULT CIfExpressEditorDlg::OnCheckEidtInput(WPARAM,LPARAM lp)
{
	CPeraAutoCheckEdit *pEdit = (CPeraAutoCheckEdit *)lp;
	CString strText;
	pEdit->GetWindowText(strText);
	CString strErrMsg;
	if (pEdit== &m_EditLeftArry)
	{
		// 需检查数组下标
		if (CheckMapArry(m_pNodeLeft,TRUE,strErrMsg))
			return S_OK;
	}
	else if (pEdit==&m_EditRightArry)
	{
		// 需检查数组下标
		if (CheckMapArry(m_pNodeRight,FALSE,strErrMsg))
			return S_OK;
	}
	else if (pEdit==&m_EditRight)
	{
		if (m_ComboRightType.GetCurSel() == TP_PARAM)
			return S_FALSE;

		if (m_iCheckType==DT_DOUBLE)
		{
			if (CStringChecker::CheckDouble(strText))
				return S_OK;

			strErrMsg = "常量不是浮点型数据！";
		}
		else if (m_iCheckType==DT_INT)
		{
			if (CStringChecker::CheckInt(strText))
				return S_OK;
			strErrMsg = "常量不是整型数据！";
		}
		else if (m_iCheckType==DT_BOOLEAN)
		{
			BOOL bTmp;
			if (GetBoolValueFromString(strText,bTmp))
				return S_OK;

			strErrMsg = "错误的Bool类型表达式！";
		}
		else
			return S_OK; // 其他类型不做检查
	}

	CStringW wstrErrMsg = CStrA2CStrW(strErrMsg);
	pEdit->ShowBalloonTip(L"提示",(LPCWSTR)wstrErrMsg,TTI_INFO);
	return S_FALSE;
}
BOOL CIfExpressEditorDlg::CheckMapArry(CCxStructTreeNode * pNodeParam ,BOOL bIsLeft,CString &strErrMsg)
{
	strErrMsg =  "";
	CString strTxt = "";
	if (bIsLeft)
		m_EditLeftArry.GetWindowText(strTxt);
	else
		m_EditRightArry.GetWindowText(strTxt);

	if (pNodeParam->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY) // 数组参数ID需要去掉后面的元素名称，否则全路径找不到
	{
		while(pNodeParam && pNodeParam->m_NodeType==CCxStructTreeNode::NT_PARAM_ARRAY)
			pNodeParam = pNodeParam->m_pParent;
	}

	CCxStructTreeNodeParam *pParamNode = (CCxStructTreeNodeParam*)pNodeParam;
	if (!pParamNode->IsArray())
	{
		strErrMsg = "节点类型不一致";
		return FALSE;
	}

	CxArrayND * pArrayND = reinterpret_cast < CxArrayND * > ( pParamNode->m_lpValueData );
	if ( pArrayND == NULL )
	{
		strErrMsg = "类型为数组的参数节点，不存在数组对象";
		return FALSE;
	}
	int iDimCount = 0;
	vector<int> vDim;
	for (int i=0;i<pArrayND->GetDimCount();i++)
	{
		vDim.push_back(pArrayND->GetDataCount(i));
	}

	strTxt.Trim();
	if (!IsValidArryMapParam(strTxt,vDim,strErrMsg))
		return FALSE;

	return TRUE;
}
void CIfExpressEditorDlg::GetCheckDataType(CCxStructTreeNode *pNode)
{
	if (pNode==NULL)
		return;

	if (pNode->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY) // 数组参数ID需要去掉后面的元素名称，否则全路径找不到
	{
		while(pNode && pNode->m_NodeType==CCxStructTreeNode::NT_PARAM_ARRAY)
			pNode = pNode->m_pParent;
	}

	CCxStructTreeNodeParam *pParamNode = (CCxStructTreeNodeParam*)pNode;
	if (pParamNode->IsArray())
	{
		CxArrayND * pArrayND = reinterpret_cast < CxArrayND * > ( pParamNode->m_lpValueData );
		m_iCheckType = pArrayND->GetRealDataType(); // 获取真实数据类型
	}
	else 
	{
		m_iCheckType = pParamNode->GetElementDataType();
	}
}

BOOL CIfExpressEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	ASSERT(pMsg != NULL);
	if (pMsg->message==WM_LBUTTONDOWN)
	{
		ZTools::WriteZToolsFormatLog("WM_LBUTTONDOWN");
		if (m_wndTreeDropDown.IsWindowVisible())
		{
			m_wndTreeDropDown.ShowWindow(SW_HIDE);
			ZTools::WriteZToolsFormatLog("鼠标点击隐藏树，失去焦点后");
			return TRUE; // 本次点击只隐藏树
		}
		int iTickTime = GetTickCount();
		int iGapTime  = iTickTime - m_iTickCountTreeLeaveFocus;
		if (CPoint(pMsg->pt) == m_PointTreeLeaveFocus && iGapTime < 100)
		{
			ZTools::WriteZToolsFormatLog("鼠标点击使得树失去焦点而隐藏，失去焦点前");
			return TRUE; //屏蔽隐藏树前的点击事件
		}

		CRect rect;
		bool bShowTree = FALSE;
		CCxStructTreeNode *pNodeCur = NULL;
		CString strArrID;
		CString strTextArry;
		if ( pMsg->hwnd == m_EditLeft.GetSafeHwnd())
		{
			bShowTree = TRUE;
			m_EditLeft.GetWindowRect(&rect);
			pNodeCur = m_pNodeLeft;
			if (!m_Express.m_LeftParam.strArry.IsEmpty())
			{
				strArrID = m_Express.m_LeftParam.strID;
				m_EditLeftArry.GetWindowText(strTextArry);
			}
			m_iFocusEidtPCtrlID = IDC_EDIT_LEFT;
		}
		else if (pMsg->hwnd == m_EditRight.GetSafeHwnd())
		{
			if (pMsg->hwnd == m_EditRight.GetSafeHwnd() && m_ComboRightType.GetCurSel()==TP_CONST)
				bShowTree = FALSE;
			else
			{
				bShowTree = TRUE;
				m_EditRight.GetWindowRect(&rect);
				pNodeCur = m_pNodeRight;
				if (!m_Express.m_RightParam.strArry.IsEmpty())
				{
					strArrID = m_Express.m_RightParam.strID;
					m_EditRightArry.GetWindowText(strTextArry);
				}
			}
			m_iFocusEidtPCtrlID = IDC_EDIT_RIGHT;
		}
	
		if (bShowTree)
		{
			if (pNodeCur)
			{
				if (!strTextArry.IsEmpty())
				{
					CCxStructTreeNode *pDefaultSel = FindArryElementNode(strArrID,strTextArry);
					if (pDefaultSel)
					 pNodeCur = pDefaultSel;
				}

				CBCGPGridRow* pSel = m_wndTreeDropDown.m_IfEditTree.FindRowByData((DWORD_PTR)pNodeCur);
				m_wndTreeDropDown.m_IfEditTree.SetCurSel(pSel);
				m_wndTreeDropDown.m_IfEditTree.UpdateSelToParamName(); //选中树的第一列
				if (pSel)
				{
					ASSERT_VALID (pSel);
					CBCGPGridRow* pParent = pSel;
					while ((pParent = pParent->GetParent ())!= NULL &&pParent->GetSubItemsCount()>0)
					{
						ASSERT_VALID (pParent);
						pParent->Expand (TRUE);
					}
					m_wndTreeDropDown.m_IfEditTree.EnsureVisible (pSel, TRUE); // 保证当前选中行可见
				}
			}
			else
			{
				m_wndTreeDropDown.m_IfEditTree.SelectRow(-1);
				m_wndTreeDropDown.m_IfEditTree.ExpandAll(FALSE);
			}
			m_wndTreeDropDown.SetWindowPos(&wndTopMost,rect.left,rect.bottom,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);
			m_wndTreeDropDown.m_IfEditTree.SetFocus();
			return TRUE;
		}
	}
	return CCxBCGPDialog::PreTranslateMessage(pMsg);
}
CCxStructTreeNode * CIfExpressEditorDlg::FindArryElementNode(const CString & strArrID,const CString & strArryIndex)
{
	CString strTmp = strArryIndex;
	strTmp.Trim();
	if (strTmp.Left(1)!="[")
		return NULL;

	if (strTmp.Right(1)!="]")
		return NULL;

	CCxStructTreeNode *pNodeArryRoot = NULL;
	pNodeArryRoot = theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.Find(strArrID);
	if (pNodeArryRoot==NULL)
		return NULL;

	CString strCenterPart = strTmp.Mid(1,strTmp.GetLength()-2); // 数字和分隔符部分
	vector<string> vSplitNum;
	string strTmpStr = strCenterPart;
	string strDelim = ",";
	ZTools::split(strTmpStr,strDelim,&vSplitNum);

	CCxStructTreeNodeParam *pParamNode = reinterpret_cast<CCxStructTreeNodeParam*>(pNodeArryRoot);
	CxArrayND * pArrayND = reinterpret_cast < CxArrayND * > ( pParamNode->m_lpValueData );
	if (pArrayND==NULL)
		return NULL;

	int iDimCount = pArrayND->GetDimCount();
	if (iDimCount!=vSplitNum.size())
		return NULL;

	vector<int> vDim;
	for (int i=0;i<iDimCount;i++)
		vDim.push_back(pArrayND->GetDataCount(i));

	CCxStructTreeNode *pNodeFind = pNodeArryRoot;
	for (size_t i=0;i<vSplitNum.size();i++)
	{
		int iIndex = atoi(vSplitNum[i].c_str());
		if (iIndex >= vDim[i])
			return NULL;

		pNodeFind = pNodeFind->m_vChildren[iIndex];
	}

	return pNodeFind;
}
BOOL CIfExpressEditorDlg::CheckGetConst(BOOL bSlient)
{
	m_EditRight.GetWindowText(m_Express.m_RightParam.strConst);
	if (m_Express.m_RightParam.strConst.IsEmpty() && m_iCheckType!=DT_STRING)
	{
		if (!bSlient)
			MessageBox("常量值不能为空!",g_lpszAppTitle,MB_OK|MB_ICONERROR);

		m_EditRight.SetFocus();
		m_EditRight.ChangeBkColor(true);
		return FALSE;
	}
	if (m_iCheckType!=DT_STRING)
		m_Express.m_RightParam.strConst.Trim(); // 字符串类型的左右空格不去掉

	CString srtErr;
	if (m_iCheckType==DT_DOUBLE)
	{
		if (CStringChecker::CheckDouble(m_Express.m_RightParam.strConst))
		{
			m_EditRight.ChangeBkColor(false);
			return TRUE;
		}

		srtErr = "常量不是浮点型!";
	}
	else if (m_iCheckType==DT_INT)
	{
		if (CStringChecker::CheckInt(m_Express.m_RightParam.strConst))
		{
			m_EditRight.ChangeBkColor(false);
			return TRUE;
		}

		srtErr = "常量不是整型!";
	}
	else if (m_iCheckType==DT_BOOLEAN)
	{
		BOOL b;
		if (GetBoolValueFromString(m_Express.m_RightParam.strConst,b))
		{
			m_EditRight.ChangeBkColor(false);
			return TRUE;
		}
		srtErr = "常量不是可识别的bool类型!";
	}
	else
	{
		m_EditRight.ChangeBkColor(false);
		return TRUE; // 其他类型不做检查
	}

	if (!bSlient)
		MessageBox(srtErr,g_lpszAppTitle,MB_OK|MB_ICONERROR);

	m_EditRight.ChangeBkColor(true);
	m_EditRight.SetFocus();
	return FALSE;
}
void  CIfExpressEditorDlg::DeleteExpress()
{
	m_Express.Clear();
	m_EditLeft.SetWindowText("");
	m_EditLeftArry.ShowWindow(SW_HIDE);
	AdjustLeftEditSize();
	m_EditRight.SetWindowText("");
	m_EditRightArry.ShowWindow(SW_HIDE);
	AdjustRightEditSize();
	m_ComboRightType.SetCurSel(TP_PARAM);
	m_ComboSymbol.SetCurSel(-1);
	m_bDeleted = TRUE;
	m_strLastRightEditParam = "";
	m_strLastRightEditConst = "";
	m_iLastComboTypeSel = TP_PARAM;
	m_EditLeft.ChangeBkColor(false);
	m_EditRight.ChangeBkColor(false);
}

void CIfExpressEditorDlg::OnBnClickedBtnDelIfExpress()
{
	// TODO: 在此添加控件通知处理程序代码
	int iComboSymbolSel = m_ComboSymbol.GetCurSel();
	if (iComboSymbolSel==-1 && m_bDeleted && m_Express.m_LeftParam.strID.IsEmpty()
		&& m_ComboRightType.GetCurSel()==TP_PARAM && m_Express.m_RightParam.strID.IsEmpty())
		return; // 已经是空表达式了

	if(IDNO == MessageBox("确定删除当前表达式？",g_lpszAppTitle,MB_YESNO|MB_ICONQUESTION))
		return;

	DeleteExpress();
}
void  CIfExpressEditorDlg::UpdateComboBoxSymbol()
{
	if (m_iCheckType== DT_STRING || m_iCheckType== DT_BOOLEAN)
	{
		int iSel = m_ComboSymbol.GetCurSel();
		m_ComboSymbol.ResetContent();
		m_ComboSymbol.AddString("==");
		m_ComboSymbol.AddString("!=");
		if (iSel > 1) iSel = 0;
		m_ComboSymbol.SetCurSel(iSel);
	}
	else
	{
		int iSel = m_ComboSymbol.GetCurSel();
		m_ComboSymbol.ResetContent();
		m_ComboSymbol.AddString("==");
		m_ComboSymbol.AddString("!=");
		m_ComboSymbol.AddString("<");
		m_ComboSymbol.AddString("<=");
		m_ComboSymbol.AddString(">");
		m_ComboSymbol.AddString(">=");
		m_ComboSymbol.SetCurSel(iSel);
	}
}