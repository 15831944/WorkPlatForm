// DataMapExpressEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DataMapExpressEditDlg.h"
#include "afxdialogex.h"
#include "Tools.h"
#include "CxStructTreeNodeParam.h"
#include "PeraProcessDesignerUtility.h"
#include "CStringChecker.h"
#include "MethodForDataMapIfExpress.h"
#include "ConvertData.h"
static CString g_strArryHelp = "数组下标需用\"[m,n]\"形式指定,映射单个元素必须指定下标,同维数组映射,可用\"*\"映射某一维的所有元素, 不填写下标时表示数组整体映射";

inline BOOL IsValidNum(const CString & str,int iMax,int iMin,CString &strErrMsg)
{
	strErrMsg = "";
	CString strTmp = str;
	strTmp.Trim();
	if (strTmp.Compare("*")==0)
		return TRUE;

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
// CDataMapExpressEditDlg 对话框
IMPLEMENT_DYNAMIC(CDataMapExpressEditDlg, CCxBCGPDialog)

	CDataMapExpressEditDlg::CDataMapExpressEditDlg(CString strExp,CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDataMapExpressEditDlg::IDD, pParent)
	,m_strInJsonStr(strExp)
{
	m_pNodeLeftParam = NULL;
	m_pNodeRightParam = NULL;
	m_iCheckType = -1;
	m_iLeftStarSymbolCount  = 0;
	m_iRightStarSymbolCount = 0;
	m_bIsArryFullMap = FALSE;
	ZTools::WriteZToolsFormatLog("数据映射传入Json串为:%s",strExp);
}

CDataMapExpressEditDlg::~CDataMapExpressEditDlg()
{
}

void CDataMapExpressEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MAP_EXPREESS_CONDITION, m_ComboSymbol);
	DDX_Control(pDX, IDC_EDIT_MAP_LEFT_PARAM, m_LeftEdit);
	DDX_Control(pDX, IDC_EDIT_MAP_RIGHT_PARAM, m_RightEdit);
	DDX_Control(pDX, IDC_EDIT_MAP_OTHER_PARAM, m_OtherEdit);
	DDX_Control(pDX, IDC_EDIT_LEFTARRY, m_EditLeftArry);
	DDX_Control(pDX, IDC_EDIT_RIGHTARRY, m_EditRightArry);
}


BEGIN_MESSAGE_MAP(CDataMapExpressEditDlg, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDataMapExpressEditDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_MAP_EXPREESS_CONDITION, &CDataMapExpressEditDlg::OnCbnSelchangeComboMapExpreessCondition)
	ON_MESSAGE(WM_EDIT_AUTOCHECK,&CDataMapExpressEditDlg::OnCheckEidtInput)
END_MESSAGE_MAP()


// CDataMapExpressEditDlg 消息处理程序


BOOL CDataMapExpressEditDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	ModifyStyle( WS_THICKFRAME, 0, SWP_FRAMECHANGED|SWP_DRAWFRAME );
	// TODO:  在此添加额外的初始化
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE); // Set big icon
	if (!ParseInPut())
	{
		EndDialog(0);
		return FALSE;
	}
	InitUI();
	InitToolTipCtrl();
	m_OtherEdit.SetMsgProWnd(GetSafeHwnd());
	m_EditLeftArry.SetMsgProWnd(GetSafeHwnd());
	m_EditRightArry.SetMsgProWnd(GetSafeHwnd());

	CString str;
	GetWindowText(str);
	SetWindowText(CString(g_lpszAppTitle) + "--" + str);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CDataMapExpressEditDlg::GetCheckDataType()
{
	CCxStructTreeNodeParam *pParamNode = (CCxStructTreeNodeParam*)m_pNodeRightParam;
	if (pParamNode->IsDouble())
		m_iCheckType = DT_DOUBLE;
	else if (pParamNode->IsInt())
		m_iCheckType = DT_INT;
	else if (pParamNode->IsBoolean())
		m_iCheckType = DT_BOOLEAN;
	else if (pParamNode->IsString())
		m_iCheckType = DT_STRING;
	else if (pParamNode->IsArray())
	{
		CxArrayND * pArrayND = reinterpret_cast < CxArrayND * > ( pParamNode->m_lpValueData );
		m_iCheckType= pArrayND->GetRealDataType();
	}
	else
		m_iCheckType  = -1;
}
BOOL CDataMapExpressEditDlg::ParseInPut()
{
	ZTools::WriteZToolsFormatLog("传入Json串：%s",m_strInJsonStr);
	if (m_strInJsonStr.IsEmpty())
		return FALSE;

	Json::Reader jR;
	if(!jR.parse(std::string(m_strInJsonStr), m_Jv))
	{//解析失败
		ZTools::WriteZToolsFormatLog("解析Json失败");
		return FALSE;
	}
	// 得到旧的表达式JSON串
	m_strOldExp = m_Jv["OldExp"].isString() ?  m_Jv["OldExp"].asString().c_str():"";

	BOOL bOK = ParseDataMapParam(m_strOldExp,m_MapData.m_LeftParam,m_MapData.m_RightParam,
		m_MapData.m_strOtherSymbol,m_MapData.m_strOtherParam);

	if (!bOK)
	{
		CString strErr;
		strErr.Format("映射表达式【%s】解析失败！",m_strOldExp);
		MessageBox(strErr,g_lpszAppTitle,MB_OK|MB_ICONERROR);
		return FALSE;
	}

	m_MapData.m_LeftParam.strParamFullPath  = GetDataFullPathByID(m_MapData.m_LeftParam.strID ,&m_pNodeLeftParam);
	m_MapData.m_RightParam.strParamFullPath = GetDataFullPathByID(m_MapData.m_RightParam.strID ,&m_pNodeRightParam);
	return TRUE;
}
void CDataMapExpressEditDlg::InitComBox()
{
	//设置Combox初始选择值
	int iComboItemCount = m_ComboSymbol.GetCount();
	CString strSysbol = "";
	int iSelIndex = -1;
	for (int i=0;i<iComboItemCount;i++)
	{
		m_ComboSymbol.GetLBText(i,strSysbol);
		if (m_MapData.m_strOtherSymbol.Compare(strSysbol)==0)
		{
			iSelIndex = i;
			break;
		}
	}
	if (iSelIndex==-1)
		iSelIndex = 0;// 选中空运算符

	m_ComboSymbol.SetCurSel(iSelIndex);
	if (iSelIndex==0)
		m_OtherEdit.EnableWindow(FALSE);
	else
		m_OtherEdit.EnableWindow(TRUE);
}
void CDataMapExpressEditDlg::InitUI()
{
	CWnd *pWndHelp = GetDlgItem(IDC_STATIC_HELP);
	if (IsArry(m_pNodeLeftParam))
	{
		m_EditLeftArry.ShowWindow(SW_SHOW);
		m_EditLeftArry.SetWindowText(m_MapData.m_LeftParam.strArry);
		pWndHelp->SetWindowText(g_strArryHelp);
	}
	else
	{
		CRect rect;
		m_EditLeftArry.GetWindowRect(&rect);
		m_EditLeftArry.ShowWindow(SW_HIDE);
		CRect rectOwn;
		m_LeftEdit.GetWindowRect(rectOwn);
		// 当左侧没有数组映射的时候，扩大窗口的宽度，填充影藏数组映射编辑框后的空白位置
		m_LeftEdit.SetWindowPos(NULL,0,0,rectOwn.Width() + rect.Width(),rectOwn.Height(),SWP_NOMOVE|SWP_NOZORDER); 
		pWndHelp->ShowWindow(SW_HIDE);
	}

	if ( IsArry(m_pNodeRightParam))
	{
		m_EditRightArry.ShowWindow(SW_SHOW);
		m_EditRightArry.SetWindowText(m_MapData.m_RightParam.strArry);
	}
	else
	{
		CRect rect;
		m_EditRightArry.GetWindowRect(&rect);
		m_EditRightArry.ShowWindow(SW_HIDE);
		CRect rectOwn;
		m_RightEdit.GetWindowRect(rectOwn);
		// 当右侧没有数组映射的时候，扩大窗口的宽度，填充影藏数组映射编辑框后的空白位置
		m_RightEdit.SetWindowPos(NULL,0,0,rectOwn.Width() + rect.Width(),rectOwn.Height(),SWP_NOMOVE|SWP_NOZORDER);
	}

	// 设置左编辑框的文本并滚动到行结尾
	m_LeftEdit.SetWindowText(m_MapData.m_LeftParam.strParamFullPath);
	int iLen = m_LeftEdit.GetWindowTextLength();
	m_LeftEdit.SetSel(iLen-1,iLen-1,FALSE);

	// 设置右编辑框的文本并滚动到行结尾
	m_RightEdit.SetWindowText(m_MapData.m_RightParam.strParamFullPath);
	iLen = m_RightEdit.GetWindowTextLength();
	m_RightEdit.SetSel(iLen-1,iLen-1,FALSE);

	m_OtherEdit.SetWindowText(m_MapData.m_strOtherParam);

	InitComBox();
	OnCbnSelchangeComboMapExpreessCondition();
	GetCheckDataType();
	OnCheckEidtInput(0,LPARAM(&m_EditRightArry));
	if (m_ComboSymbol.GetCurSel()==0)
		m_OtherEdit.EnableWindow(FALSE);
	else
		m_OtherEdit.EnableWindow(TRUE);

	UpdateData(FALSE);// 更新到界面
}
bool CDataMapExpressEditDlg::IsValidArryMapParam(const CString & strText,vector<int> vArryDim,CString &strErrMsg,BOOL bIsLeft)
{
	int iLen = strText.GetLength();
	if (iLen < 3)
	{
		strErrMsg = "映射下标字符串不完整";
		return false; //[*] 至少三个字符
	}

	if (strText.Left(1)!="[")
	{
		strErrMsg = "映射下标字符串首字符不是\"[\"";
		return false;
	}

	if (strText.Right(1)!="]")
	{
		strErrMsg = "映射下标字符串尾字符不是\"]\"";
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
		strErrMsg = "映射维度格式错误";
		return false;
	}
	int iXingCount = 0;
	for (size_t i=0;i<vArryDim.size();i++)
	{
		if (!IsValidNum(vSplitNum[i].c_str(),vArryDim[i]-1,0,strErrMsg))
			return false;
		if (vSplitNum[i]=="*")
			iXingCount++;
	}
	if (bIsLeft)
		m_iLeftStarSymbolCount = iXingCount; // 记录*号个数
	else
		m_iRightStarSymbolCount = iXingCount;

	if (iXingCount>1)
	{
		strErrMsg = "数组映射中出现不止一个*";
		return false;
	}
	return true;
}
BOOL CDataMapExpressEditDlg::CheckMapArry(CCxStructTreeNode * pNodeParam ,BOOL bIsLeft,CString &strErrMsg)
{
	strErrMsg.Empty();
	CString strTxt = "";
	if (bIsLeft)
		m_EditLeftArry.GetWindowText(strTxt);
	else
		m_EditRightArry.GetWindowText(strTxt);

	if (strTxt.IsEmpty())
		return TRUE; // 数组映射下标为空的时候认为合法,点击确定的时候再联合=左右的数组维度详细检查

	CCxStructTreeNodeParam *pParamNode = reinterpret_cast<CCxStructTreeNodeParam*>(pNodeParam);
	ASSERT(pParamNode);
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
	if (!IsValidArryMapParam(strTxt,vDim,strErrMsg,bIsLeft))
		return FALSE;

	return TRUE;
}
BOOL CDataMapExpressEditDlg::CheckOtherParam(CString & strErrMsg)
{
	CString strText  = "";
	strErrMsg = "";
	m_OtherEdit.GetWindowText(strText);
	if (m_ComboSymbol.GetCurSel()==0) // 选择空，即不需要附加表达式
		return TRUE;

	if (m_iCheckType == DT_DOUBLE)
	{
		if (CStringChecker::CheckDouble(strText))
			return TRUE;

		strErrMsg = "附加表达式不是浮点类型!";
	}
	else if (m_iCheckType == DT_INT)
	{
		if (CStringChecker::CheckInt(strText))
			return TRUE;

		strErrMsg = "附加表达式不是整数类型!";
	}
	else
	{
		return TRUE; // 其他类型不做校验
	}

	return FALSE;
}
BOOL CDataMapExpressEditDlg::ProcessUpdate( const char* str )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;

	wmd.m_sMethodName = "dataMappingExpUpdate";

	pParam = new CWebMethodParamData();
	pParam->m_sName = "json";
	pParam->m_sValue = str;
	wmd.m_vParams.push_back(pParam);
	BOOL b = pSvg->InvokeJsMethod( &wmd );

	if (!b || wmd.m_sResult.IsEmpty()) 
	{
		MessageBox("调用页面方法dataMappingExpUpdate失败！", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	else
	{
		return ParseResultJson(wmd.m_sResult);
	}

}
BOOL CDataMapExpressEditDlg::ParseResultJson(const char* strJson)
{
	Json::Reader Jreader;
	Json::Value Jv;
	Json::FastWriter Jwriter;
	Json::Value JvRet;

	if(!Jreader.parse(strJson, m_Jv))
	{//解析失败
		return FALSE;
	}
	std::string strErr = m_Jv["Error"].asString();

	if(strErr.empty())
		return TRUE;

	MessageBox(strErr.c_str(), g_lpszAppTitle, MB_OK|MB_ICONERROR);
	return FALSE;
}
void CDataMapExpressEditDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// 从控件上获取新的数据值
	if (IsArry(m_pNodeLeftParam))
		m_EditLeftArry.GetWindowText(m_MapData.m_LeftParam.strArry);

	if (IsArry(m_pNodeRightParam))
		m_EditRightArry.GetWindowText(m_MapData.m_RightParam.strArry);

	m_OtherEdit.GetWindowText(m_MapData.m_strOtherParam);
	m_MapData.m_LeftParam.strArry.Trim();
	m_MapData.m_RightParam.strArry.Trim();

	CString strErrMsgLeft,strErrMsgRight;
	int iErrType = 0;
	if (!CheckArryMapOK(FALSE,strErrMsgLeft,strErrMsgRight,TRUE,iErrType))
		return;

	// 映射下标正确的时候才可以去掉中间的空格，否则检查会漏掉用户输入的错误
	m_MapData.m_LeftParam.strArry.Replace(" ",""); // 数组下标里面的空格去掉 bug PERAS-3065 解决方案
	m_MapData.m_RightParam.strArry.Replace(" ","");// 数组下标里面的空格去掉 bug PERAS-3065 解决方案

	if (m_iLeftStarSymbolCount!=m_iRightStarSymbolCount) // 左右*个数一致校验
	{
		MessageBox("等号两侧数组下标中*个数不同！",g_lpszAppTitle,MB_OK|MB_ICONERROR);
		if (m_iLeftStarSymbolCount>0)
		{
			m_EditLeftArry.ChangeBkColor(true);
			m_EditLeftArry.SetFocus();
		}
		else
		{
			m_EditRightArry.ChangeBkColor(true);
			m_EditRightArry.SetFocus();
		}
		
		return;
	}
	if (m_iLeftStarSymbolCount == 0 && !m_bIsArryFullMap) 
	{
		if (m_ComboSymbol.GetCurSel()>0) // 只有选择了有效的附加表达式运算符时，才对附加参数进行校验
		{
			if (m_MapData.m_strOtherParam.IsEmpty())
			{
				MessageBox("附加表达式数据未指定!",g_lpszAppTitle,MB_OK|MB_ICONERROR);
				m_OtherEdit.SetFocus();
				return;
			}
			CString strErr;
			if (!CheckOtherParam(strErr))
			{
				MessageBox(strErr, g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_OtherEdit.ChangeBkColor(true);
				m_OtherEdit.SetFocus();
				return;
			}
		}

		int iComboSymbolSel = m_ComboSymbol.GetCurSel();
		if (iComboSymbolSel <=0)
		{
			m_MapData.m_strOtherSymbol = "";
			m_MapData.m_strOtherParam  = ""; //选择无效的运算符时，附加参数无效
		}
		else
		{
			m_ComboSymbol.GetLBText(iComboSymbolSel,m_MapData.m_strOtherSymbol);
			m_MapData.m_strOtherParam.Trim();
		}
	}
	else // 有数组映射时，不允许单位换算（不允许附件运算）
	{
		m_MapData.m_strOtherSymbol = "";
		m_MapData.m_strOtherParam  = "";
	}


	CString strNewExp;
	strNewExp.Format("#{%s%s}=#{%s%s}%s%s",m_MapData.m_LeftParam.strID,m_MapData.m_LeftParam.strArry,
		m_MapData.m_RightParam.strID,m_MapData.m_RightParam.strArry,
		m_MapData.m_strOtherSymbol,m_MapData.m_strOtherParam);

	Json::Reader Jreader;
	Json::Value Jv;
	Json::FastWriter Jwriter;
	Json::Value JvRet;

	m_Jv["NewExp"] = (LPCTSTR)strNewExp;
	std::string str = Jwriter.write(m_Jv);
	if (!ProcessUpdate(str.c_str()))
		return;

	CCxBCGPDialog::OnOK();
}
void CDataMapExpressEditDlg::OnCbnSelchangeComboMapExpreessCondition()
{
	// TODO: 在此添加控件通知处理程序代码
	int iSelIndex = m_ComboSymbol.GetCurSel();
	if (iSelIndex<=0)
	{
		m_OtherEdit.ChangeBkColor(false);
		m_OtherEdit.EnableWindow(FALSE);
	}
	else
	{
		m_OtherEdit.EnableWindow(TRUE);
		//切换到有效的符号时，重新检查附加表达式数据是否合法
		bool bErr = OnCheckEidtInput(0,(LPARAM)&m_OtherEdit)==S_OK ? false:true;
		m_OtherEdit.ChangeBkColor(bErr);
	}
}
HRESULT CDataMapExpressEditDlg::OnCheckEidtInput(WPARAM wp,LPARAM lp)
{
	CPeraAutoCheckEdit *pEdit = (CPeraAutoCheckEdit *)lp;
	CString strText;
	pEdit->GetWindowText(strText);

	CString strErrMsg;
	if (pEdit== &m_OtherEdit)
	{
		if (m_iCheckType == DT_DOUBLE)
		{
			if (CStringChecker::CheckDouble(strText))
				return S_OK;

			strErrMsg = "不是浮点型数据!";
		}
		else if (m_iCheckType == DT_INT)
		{
			if (CStringChecker::CheckInt(strText))
				return S_OK;

			strErrMsg = "不是整型数据!";
		}
		else
		{
			return S_OK; // 其他类型不做校验
		}

		CStringW wstrErrMsg = CStrA2CStrW(strErrMsg);
		m_OtherEdit.ShowBalloonTip(L"提示",(LPCWSTR)wstrErrMsg,TTI_INFO);
		return S_FALSE;
	}
	else if (pEdit== &m_EditLeftArry || pEdit== & m_EditRightArry)
	{
		if (IsArry(m_pNodeLeftParam))
			m_EditLeftArry.GetWindowText(m_MapData.m_LeftParam.strArry);

		if (IsArry(m_pNodeRightParam))
			m_EditRightArry.GetWindowText(m_MapData.m_RightParam.strArry);

		int iEditCtrl = 0;
		if (pEdit== &m_EditLeftArry) iEditCtrl = 1;
		else if (pEdit== &m_EditRightArry) iEditCtrl = 2;

		int iErrType = 0;
		CString strErrMsgLeft,strErrMsgRight;
		if (CheckArryMapOK(TRUE,strErrMsgLeft,strErrMsgRight,FALSE,iErrType))
		{

			if (m_iLeftStarSymbolCount >0 || m_iRightStarSymbolCount > 0 || m_bIsArryFullMap
				||m_iCheckType == DT_BOOLEAN
				||m_iCheckType == DT_STRING) // 有数组维度映射时,禁用单位换算
			{
				m_ComboSymbol.EnableWindow(FALSE);
				m_OtherEdit.EnableWindow(FALSE);
			}
			else
			{
				m_ComboSymbol.EnableWindow(TRUE);
				if (m_ComboSymbol.GetCurSel()==0)
					m_OtherEdit.EnableWindow(FALSE);
				else
					m_OtherEdit.EnableWindow(TRUE);
			}
			return S_OK;
		}

		if (iErrType==1 && pEdit== &m_EditLeftArry)
		{
			CStringW wstrErrMsg = CStrA2CStrW(strErrMsgLeft);
			m_EditLeftArry.ShowBalloonTip(L"提示",(LPCWSTR)wstrErrMsg,TTI_INFO);
		}
		else if (iErrType==2 && pEdit== &m_EditRightArry)
		{
			CStringW wstrErrMsg = CStrA2CStrW(strErrMsgRight);
			m_EditRightArry.ShowBalloonTip(L"提示",(LPCWSTR)wstrErrMsg,TTI_INFO);
		}
		else if (iErrType==3)
		{
			CStringW wstrErrMsg;
			if (iEditCtrl==1) // 在当前编辑的控件上显示错误气泡
			{
				wstrErrMsg = CStrA2CStrW(strErrMsgLeft);
				m_EditLeftArry.ShowBalloonTip(L"提示",(LPCWSTR)wstrErrMsg,TTI_INFO);
			}
			else
			{
				wstrErrMsg = CStrA2CStrW(strErrMsgRight);
				m_EditRightArry.ShowBalloonTip(L"提示",(LPCWSTR)wstrErrMsg,TTI_INFO);
			}
		}
		
		if (iEditCtrl == iErrType || iErrType == 3)
			return S_FALSE; // 两遍都错了
		else
			return S_OK; // 出错的不是当前控件，不应该标黄
	}

	return S_OK;
}
BOOL CDataMapExpressEditDlg::IsArry(CCxStructTreeNode *pNode)
{
	if (pNode==NULL)
		return FALSE;

	if (!pNode->IsParamNode())
		return FALSE;

	CCxStructTreeNodeParam *pParamNode = reinterpret_cast< CCxStructTreeNodeParam * >(pNode);
	if (pParamNode==NULL)
		return FALSE;

	if (pParamNode->IsArray())
		return TRUE;

	return FALSE;
}
BOOL CDataMapExpressEditDlg::CheckArryMapOK(BOOL bSlient,CString & strErrMsgLeft,CString & strErrMsgRight,BOOL bOnOk,int & bErrType)
{
	BOOL bLeftIsArry = FALSE;
	BOOL bRightIsArry = FALSE;
	BOOL bErrOccur = FALSE;
	bErrType = 0;
	if (IsArry(m_pNodeLeftParam))
	{
		bLeftIsArry = TRUE;
		if (!CheckMapArry(m_pNodeLeftParam,TRUE,strErrMsgLeft))
		{
			if (!bSlient)
			{
				MessageBox(strErrMsgLeft, g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditLeftArry.SetFocus();
			}

			m_EditLeftArry.ChangeBkColor(true);
			bErrOccur = TRUE;
			bErrType = 1; // 左边不对
			if (bOnOk)
				return FALSE; // 点击确定时，出错则立即返回
		}
		else
			m_EditLeftArry.ChangeBkColor(false);
	}

	if (IsArry(m_pNodeRightParam))
	{
		bRightIsArry = TRUE;
		if(!CheckMapArry(m_pNodeRightParam,FALSE,strErrMsgRight))
		{
			if (!bSlient)
			{
				MessageBox(strErrMsgRight, g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditRightArry.SetFocus();
			}

			m_EditRightArry.ChangeBkColor(true);
			if (bErrOccur)
				bErrType = 3; // 左右都不对
			else
				bErrType = 2; // 右边不对

			bErrOccur = TRUE;
			if (bOnOk)
				return FALSE; // 点击确定时，出错则立即返回
		}
		else
			m_EditRightArry.ChangeBkColor(false);
	}

	if (bErrOccur)
		return FALSE;

	// 都是数组
	if (bLeftIsArry && bRightIsArry) 
	{
		CCxStructTreeNodeParam *pLeftParam = reinterpret_cast < CCxStructTreeNodeParam * >(m_pNodeLeftParam); 
		CxArrayND * pArrayLeftND = reinterpret_cast < CxArrayND * > ( pLeftParam->m_lpValueData );
		ASSERT(pArrayLeftND);

		CCxStructTreeNodeParam *pRightParam = reinterpret_cast < CCxStructTreeNodeParam * >(m_pNodeRightParam); 
		CxArrayND * pArrayRightND = reinterpret_cast < CxArrayND * > ( pRightParam->m_lpValueData );
		ASSERT(pArrayRightND);

		m_bIsArryFullMap = FALSE;
		if (m_MapData.m_LeftParam.strArry.IsEmpty() && m_MapData.m_RightParam.strArry.IsEmpty()) //且映射下标都为空的情况检查
		{
			CString strDimLeft  = pArrayLeftND->GetDimStr();
			CString strDimRight = pArrayRightND->GetDimStr();
			m_bIsArryFullMap = TRUE; // 需判断是否是整体映射
			if (!bOnOk)
				return TRUE; // 左右都是数组，编辑时不校验，点击确定的时候才校验

			if (strDimLeft.Compare(strDimRight)!=0)
			{
				if (!bSlient)
				{
					MessageBox("数组整体映射时,数组维度和长度都必须相同！", g_lpszAppTitle, MB_OK|MB_ICONERROR);
					m_EditLeftArry.ChangeBkColor(true);
					m_EditRightArry.ChangeBkColor(true);
					m_EditLeftArry.SetFocus();
				}
				strErrMsgLeft = "数组整体映射时,数组维度和长度都必须相同！";
				strErrMsgRight = strErrMsgLeft;
				return FALSE;
			}
		}
		else if (m_MapData.m_LeftParam.strArry.IsEmpty()) //“=”左侧映射下标为空
		{
			if (!bOnOk)
				return TRUE; // 左右都是数组，编辑时不校验，点击确定的时候才校验

			if (!bSlient)
			{
				MessageBox("映射数组下标未指定!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditLeftArry.ChangeBkColor(true);
				m_EditLeftArry.SetFocus();
			}
			strErrMsgLeft = "映射数组下标未指定!";
			strErrMsgRight = strErrMsgLeft;
			return FALSE;
		}
		else if(m_MapData.m_RightParam.strArry.IsEmpty()) //“=”右侧映射下标为空
		{
			if (!bOnOk)
				return TRUE; // 左右都是数组，编辑时不校验，点击确定的时候才校验

			if (!bSlient)
			{
				MessageBox("映射数组下标未指定!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditRightArry.ChangeBkColor(true);
				m_EditRightArry.SetFocus();
			}
			strErrMsgLeft = "映射数组下标未指定!";
			strErrMsgRight = strErrMsgLeft;
			return FALSE;
		}

		m_EditLeftArry.ChangeBkColor(false);
		m_EditRightArry.ChangeBkColor(false);
		return TRUE;
	}
	else if (bLeftIsArry) // 只有左边是数组
	{
		if (m_MapData.m_LeftParam.strArry.IsEmpty())
		{
			if (!bSlient)
			{
				MessageBox("映射数组下标未指定!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditLeftArry.ChangeBkColor(true);
				m_EditLeftArry.SetFocus();
			}
			strErrMsgLeft = "映射数组下标未指定!";
			strErrMsgRight = strErrMsgLeft;
			return FALSE;
		}
		 
	}
	else if (bRightIsArry) // 只有右边是数组
	{
		 if(m_MapData.m_RightParam.strArry.IsEmpty()) //“=”右侧映射下标为空
		 {
			 if (!bSlient)
			 {
				 MessageBox("映射数组下标未指定!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				 m_EditRightArry.ChangeBkColor(true);
				 m_EditRightArry.SetFocus();
			 }
			strErrMsgLeft = "映射数组下标未指定!";
			strErrMsgRight = strErrMsgLeft;
			return FALSE;
		}
	}
	m_EditLeftArry.ChangeBkColor(false);
	m_EditRightArry.ChangeBkColor(false);
	return TRUE;
}

BOOL CDataMapExpressEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	ASSERT(pMsg != NULL);
	if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP)
	{
		if (IsWindow(m_ToolTipCtrl.m_hWnd))
			m_ToolTipCtrl.RelayEvent(pMsg);
	}
	return CCxBCGPDialog::PreTranslateMessage(pMsg);
}
void CDataMapExpressEditDlg::InitToolTipCtrl()
{
	m_ToolTipCtrl.Create(this);
	CString strText;
	m_LeftEdit.GetWindowText(strText);
	m_ToolTipCtrl.AddTool(&m_LeftEdit,strText);

	m_RightEdit.GetWindowText(strText);
	m_ToolTipCtrl.AddTool(&m_RightEdit,strText);

	/*m_ToolTipCtrl.AddTool(&m_EditLeftArry, "在此填写数组映射下标");
	m_ToolTipCtrl.AddTool(&m_EditRightArry,"在此填写数组映射下标");*/

	m_ToolTipCtrl.SetDelayTime(TTDT_AUTOPOP,3000);
	m_ToolTipCtrl.Activate(TRUE);
}