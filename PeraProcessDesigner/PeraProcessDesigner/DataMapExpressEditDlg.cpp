// DataMapExpressEditDlg.cpp : ʵ���ļ�
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
static CString g_strArryHelp = "�����±�����\"[m,n]\"��ʽָ��,ӳ�䵥��Ԫ�ر���ָ���±�,ͬά����ӳ��,����\"*\"ӳ��ĳһά������Ԫ��, ����д�±�ʱ��ʾ��������ӳ��";

inline BOOL IsValidNum(const CString & str,int iMax,int iMin,CString &strErrMsg)
{
	strErrMsg = "";
	CString strTmp = str;
	strTmp.Trim();
	if (strTmp.Compare("*")==0)
		return TRUE;

	if (!CStringChecker::CheckInt(strTmp))
	{
		strErrMsg = "�����±겻����������";
		return FALSE;
	}
	int iNum  = atoi(strTmp);
	if (iNum > iMax || iNum < iMin)
	{
		strErrMsg = "�����±�Խ��";
		return FALSE;
	}

	return TRUE;
}
// CDataMapExpressEditDlg �Ի���
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
	ZTools::WriteZToolsFormatLog("����ӳ�䴫��Json��Ϊ:%s",strExp);
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


// CDataMapExpressEditDlg ��Ϣ�������


BOOL CDataMapExpressEditDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	ModifyStyle( WS_THICKFRAME, 0, SWP_FRAMECHANGED|SWP_DRAWFRAME );
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
	// �쳣: OCX ����ҳӦ���� FALSE
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
	ZTools::WriteZToolsFormatLog("����Json����%s",m_strInJsonStr);
	if (m_strInJsonStr.IsEmpty())
		return FALSE;

	Json::Reader jR;
	if(!jR.parse(std::string(m_strInJsonStr), m_Jv))
	{//����ʧ��
		ZTools::WriteZToolsFormatLog("����Jsonʧ��");
		return FALSE;
	}
	// �õ��ɵı��ʽJSON��
	m_strOldExp = m_Jv["OldExp"].isString() ?  m_Jv["OldExp"].asString().c_str():"";

	BOOL bOK = ParseDataMapParam(m_strOldExp,m_MapData.m_LeftParam,m_MapData.m_RightParam,
		m_MapData.m_strOtherSymbol,m_MapData.m_strOtherParam);

	if (!bOK)
	{
		CString strErr;
		strErr.Format("ӳ����ʽ��%s������ʧ�ܣ�",m_strOldExp);
		MessageBox(strErr,g_lpszAppTitle,MB_OK|MB_ICONERROR);
		return FALSE;
	}

	m_MapData.m_LeftParam.strParamFullPath  = GetDataFullPathByID(m_MapData.m_LeftParam.strID ,&m_pNodeLeftParam);
	m_MapData.m_RightParam.strParamFullPath = GetDataFullPathByID(m_MapData.m_RightParam.strID ,&m_pNodeRightParam);
	return TRUE;
}
void CDataMapExpressEditDlg::InitComBox()
{
	//����Combox��ʼѡ��ֵ
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
		iSelIndex = 0;// ѡ�п������

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
		// �����û������ӳ���ʱ�����󴰿ڵĿ�ȣ����Ӱ������ӳ��༭���Ŀհ�λ��
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
		// ���Ҳ�û������ӳ���ʱ�����󴰿ڵĿ�ȣ����Ӱ������ӳ��༭���Ŀհ�λ��
		m_RightEdit.SetWindowPos(NULL,0,0,rectOwn.Width() + rect.Width(),rectOwn.Height(),SWP_NOMOVE|SWP_NOZORDER);
	}

	// ������༭����ı����������н�β
	m_LeftEdit.SetWindowText(m_MapData.m_LeftParam.strParamFullPath);
	int iLen = m_LeftEdit.GetWindowTextLength();
	m_LeftEdit.SetSel(iLen-1,iLen-1,FALSE);

	// �����ұ༭����ı����������н�β
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

	UpdateData(FALSE);// ���µ�����
}
bool CDataMapExpressEditDlg::IsValidArryMapParam(const CString & strText,vector<int> vArryDim,CString &strErrMsg,BOOL bIsLeft)
{
	int iLen = strText.GetLength();
	if (iLen < 3)
	{
		strErrMsg = "ӳ���±��ַ���������";
		return false; //[*] ���������ַ�
	}

	if (strText.Left(1)!="[")
	{
		strErrMsg = "ӳ���±��ַ������ַ�����\"[\"";
		return false;
	}

	if (strText.Right(1)!="]")
	{
		strErrMsg = "ӳ���±��ַ���β�ַ�����\"]\"";
		return false;
	}

	CString strCenterPart = strText.Mid(1,iLen-2); // ���ֺͷָ�������
	strCenterPart.Trim();
	vector<string> vSplitNum;
	string strTmp = strCenterPart;
	string strDelim = ",";
	ZTools::split(strTmp,strDelim,&vSplitNum);
	if (vSplitNum.size()!=vArryDim.size())
	{
		strErrMsg = "ӳ��ά�ȸ�ʽ����";
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
		m_iLeftStarSymbolCount = iXingCount; // ��¼*�Ÿ���
	else
		m_iRightStarSymbolCount = iXingCount;

	if (iXingCount>1)
	{
		strErrMsg = "����ӳ���г��ֲ�ֹһ��*";
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
		return TRUE; // ����ӳ���±�Ϊ�յ�ʱ����Ϊ�Ϸ�,���ȷ����ʱ��������=���ҵ�����ά����ϸ���

	CCxStructTreeNodeParam *pParamNode = reinterpret_cast<CCxStructTreeNodeParam*>(pNodeParam);
	ASSERT(pParamNode);
	if (!pParamNode->IsArray())
	{
		strErrMsg = "�ڵ����Ͳ�һ��";
		return FALSE;
	}

	CxArrayND * pArrayND = reinterpret_cast < CxArrayND * > ( pParamNode->m_lpValueData );
	if ( pArrayND == NULL )
	{
		strErrMsg = "����Ϊ����Ĳ����ڵ㣬�������������";
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
	if (m_ComboSymbol.GetCurSel()==0) // ѡ��գ�������Ҫ���ӱ��ʽ
		return TRUE;

	if (m_iCheckType == DT_DOUBLE)
	{
		if (CStringChecker::CheckDouble(strText))
			return TRUE;

		strErrMsg = "���ӱ��ʽ���Ǹ�������!";
	}
	else if (m_iCheckType == DT_INT)
	{
		if (CStringChecker::CheckInt(strText))
			return TRUE;

		strErrMsg = "���ӱ��ʽ������������!";
	}
	else
	{
		return TRUE; // �������Ͳ���У��
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
		MessageBox("����ҳ�淽��dataMappingExpUpdateʧ�ܣ�", g_lpszAppTitle, MB_OK|MB_ICONERROR);
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
	{//����ʧ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// �ӿؼ��ϻ�ȡ�µ�����ֵ
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

	// ӳ���±���ȷ��ʱ��ſ���ȥ���м�Ŀո񣬷������©���û�����Ĵ���
	m_MapData.m_LeftParam.strArry.Replace(" ",""); // �����±�����Ŀո�ȥ�� bug PERAS-3065 �������
	m_MapData.m_RightParam.strArry.Replace(" ","");// �����±�����Ŀո�ȥ�� bug PERAS-3065 �������

	if (m_iLeftStarSymbolCount!=m_iRightStarSymbolCount) // ����*����һ��У��
	{
		MessageBox("�Ⱥ����������±���*������ͬ��",g_lpszAppTitle,MB_OK|MB_ICONERROR);
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
		if (m_ComboSymbol.GetCurSel()>0) // ֻ��ѡ������Ч�ĸ��ӱ��ʽ�����ʱ���ŶԸ��Ӳ�������У��
		{
			if (m_MapData.m_strOtherParam.IsEmpty())
			{
				MessageBox("���ӱ��ʽ����δָ��!",g_lpszAppTitle,MB_OK|MB_ICONERROR);
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
			m_MapData.m_strOtherParam  = ""; //ѡ����Ч�������ʱ�����Ӳ�����Ч
		}
		else
		{
			m_ComboSymbol.GetLBText(iComboSymbolSel,m_MapData.m_strOtherSymbol);
			m_MapData.m_strOtherParam.Trim();
		}
	}
	else // ������ӳ��ʱ��������λ���㣨�����������㣩
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iSelIndex = m_ComboSymbol.GetCurSel();
	if (iSelIndex<=0)
	{
		m_OtherEdit.ChangeBkColor(false);
		m_OtherEdit.EnableWindow(FALSE);
	}
	else
	{
		m_OtherEdit.EnableWindow(TRUE);
		//�л�����Ч�ķ���ʱ�����¼�鸽�ӱ��ʽ�����Ƿ�Ϸ�
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

			strErrMsg = "���Ǹ���������!";
		}
		else if (m_iCheckType == DT_INT)
		{
			if (CStringChecker::CheckInt(strText))
				return S_OK;

			strErrMsg = "������������!";
		}
		else
		{
			return S_OK; // �������Ͳ���У��
		}

		CStringW wstrErrMsg = CStrA2CStrW(strErrMsg);
		m_OtherEdit.ShowBalloonTip(L"��ʾ",(LPCWSTR)wstrErrMsg,TTI_INFO);
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
				||m_iCheckType == DT_STRING) // ������ά��ӳ��ʱ,���õ�λ����
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
			m_EditLeftArry.ShowBalloonTip(L"��ʾ",(LPCWSTR)wstrErrMsg,TTI_INFO);
		}
		else if (iErrType==2 && pEdit== &m_EditRightArry)
		{
			CStringW wstrErrMsg = CStrA2CStrW(strErrMsgRight);
			m_EditRightArry.ShowBalloonTip(L"��ʾ",(LPCWSTR)wstrErrMsg,TTI_INFO);
		}
		else if (iErrType==3)
		{
			CStringW wstrErrMsg;
			if (iEditCtrl==1) // �ڵ�ǰ�༭�Ŀؼ�����ʾ��������
			{
				wstrErrMsg = CStrA2CStrW(strErrMsgLeft);
				m_EditLeftArry.ShowBalloonTip(L"��ʾ",(LPCWSTR)wstrErrMsg,TTI_INFO);
			}
			else
			{
				wstrErrMsg = CStrA2CStrW(strErrMsgRight);
				m_EditRightArry.ShowBalloonTip(L"��ʾ",(LPCWSTR)wstrErrMsg,TTI_INFO);
			}
		}
		
		if (iEditCtrl == iErrType || iErrType == 3)
			return S_FALSE; // ���鶼����
		else
			return S_OK; // ����Ĳ��ǵ�ǰ�ؼ�����Ӧ�ñ��
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
			bErrType = 1; // ��߲���
			if (bOnOk)
				return FALSE; // ���ȷ��ʱ����������������
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
				bErrType = 3; // ���Ҷ�����
			else
				bErrType = 2; // �ұ߲���

			bErrOccur = TRUE;
			if (bOnOk)
				return FALSE; // ���ȷ��ʱ����������������
		}
		else
			m_EditRightArry.ChangeBkColor(false);
	}

	if (bErrOccur)
		return FALSE;

	// ��������
	if (bLeftIsArry && bRightIsArry) 
	{
		CCxStructTreeNodeParam *pLeftParam = reinterpret_cast < CCxStructTreeNodeParam * >(m_pNodeLeftParam); 
		CxArrayND * pArrayLeftND = reinterpret_cast < CxArrayND * > ( pLeftParam->m_lpValueData );
		ASSERT(pArrayLeftND);

		CCxStructTreeNodeParam *pRightParam = reinterpret_cast < CCxStructTreeNodeParam * >(m_pNodeRightParam); 
		CxArrayND * pArrayRightND = reinterpret_cast < CxArrayND * > ( pRightParam->m_lpValueData );
		ASSERT(pArrayRightND);

		m_bIsArryFullMap = FALSE;
		if (m_MapData.m_LeftParam.strArry.IsEmpty() && m_MapData.m_RightParam.strArry.IsEmpty()) //��ӳ���±궼Ϊ�յ�������
		{
			CString strDimLeft  = pArrayLeftND->GetDimStr();
			CString strDimRight = pArrayRightND->GetDimStr();
			m_bIsArryFullMap = TRUE; // ���ж��Ƿ�������ӳ��
			if (!bOnOk)
				return TRUE; // ���Ҷ������飬�༭ʱ��У�飬���ȷ����ʱ���У��

			if (strDimLeft.Compare(strDimRight)!=0)
			{
				if (!bSlient)
				{
					MessageBox("��������ӳ��ʱ,����ά�Ⱥͳ��ȶ�������ͬ��", g_lpszAppTitle, MB_OK|MB_ICONERROR);
					m_EditLeftArry.ChangeBkColor(true);
					m_EditRightArry.ChangeBkColor(true);
					m_EditLeftArry.SetFocus();
				}
				strErrMsgLeft = "��������ӳ��ʱ,����ά�Ⱥͳ��ȶ�������ͬ��";
				strErrMsgRight = strErrMsgLeft;
				return FALSE;
			}
		}
		else if (m_MapData.m_LeftParam.strArry.IsEmpty()) //��=�����ӳ���±�Ϊ��
		{
			if (!bOnOk)
				return TRUE; // ���Ҷ������飬�༭ʱ��У�飬���ȷ����ʱ���У��

			if (!bSlient)
			{
				MessageBox("ӳ�������±�δָ��!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditLeftArry.ChangeBkColor(true);
				m_EditLeftArry.SetFocus();
			}
			strErrMsgLeft = "ӳ�������±�δָ��!";
			strErrMsgRight = strErrMsgLeft;
			return FALSE;
		}
		else if(m_MapData.m_RightParam.strArry.IsEmpty()) //��=���Ҳ�ӳ���±�Ϊ��
		{
			if (!bOnOk)
				return TRUE; // ���Ҷ������飬�༭ʱ��У�飬���ȷ����ʱ���У��

			if (!bSlient)
			{
				MessageBox("ӳ�������±�δָ��!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditRightArry.ChangeBkColor(true);
				m_EditRightArry.SetFocus();
			}
			strErrMsgLeft = "ӳ�������±�δָ��!";
			strErrMsgRight = strErrMsgLeft;
			return FALSE;
		}

		m_EditLeftArry.ChangeBkColor(false);
		m_EditRightArry.ChangeBkColor(false);
		return TRUE;
	}
	else if (bLeftIsArry) // ֻ�����������
	{
		if (m_MapData.m_LeftParam.strArry.IsEmpty())
		{
			if (!bSlient)
			{
				MessageBox("ӳ�������±�δָ��!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				m_EditLeftArry.ChangeBkColor(true);
				m_EditLeftArry.SetFocus();
			}
			strErrMsgLeft = "ӳ�������±�δָ��!";
			strErrMsgRight = strErrMsgLeft;
			return FALSE;
		}
		 
	}
	else if (bRightIsArry) // ֻ���ұ�������
	{
		 if(m_MapData.m_RightParam.strArry.IsEmpty()) //��=���Ҳ�ӳ���±�Ϊ��
		 {
			 if (!bSlient)
			 {
				 MessageBox("ӳ�������±�δָ��!", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				 m_EditRightArry.ChangeBkColor(true);
				 m_EditRightArry.SetFocus();
			 }
			strErrMsgLeft = "ӳ�������±�δָ��!";
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
	// TODO: �ڴ����ר�ô����/����û���
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

	/*m_ToolTipCtrl.AddTool(&m_EditLeftArry, "�ڴ���д����ӳ���±�");
	m_ToolTipCtrl.AddTool(&m_EditRightArry,"�ڴ���д����ӳ���±�");*/

	m_ToolTipCtrl.SetDelayTime(TTDT_AUTOPOP,3000);
	m_ToolTipCtrl.Activate(TRUE);
}