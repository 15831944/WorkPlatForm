// DlgParamEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgParamEdit.h"
#include "afxdialogex.h"
#include "DlgInfoEdit.h"
#include "DlgParamSet.h"
#include <UUID.h>
#include "DlgParamEditFile.h"
#include <CStringChecker.h>
#include "DlgParamEditArray.h"
#include <sstream>

extern std::string GetLengthString(std::string str); //长度字符串"1,2"

extern std::string GetDimString(std::string str); //维度字符串"2"

extern CRect CalculateRect(const ZRectF& zrectf, CRect& rect);

extern void string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst );

const ZRectF CDlgParamEdit::m_conRectName(0.1448f, 0.2000f, 0.9696f, 0.3300f);
const ZRectF CDlgParamEdit::m_conRectType(0.1448f, 0.3467f, 0.5858f, 0.4800f); 
const ZRectF CDlgParamEdit::m_conRectValue(0.1448f, 0.5067f, 0.8505f, 0.6300f); 
const ZRectF CDlgParamEdit::m_conRectUnits(0.1448f, 0.6467f, 0.9696f, 0.7800f); 
const ZRectF CDlgParamEdit::m_conRectDesc(0.1448f, 0.8100f, 0.8505f, 0.9367f); 
const ZRectF CDlgParamEdit::m_conRectDirection(0.6046f, 0.3467f, 0.8005f, 0.4800f); 
const ZRectF CDlgParamEdit::m_conRectDynamic(0.8215f, 0.3467f, 0.9696f, 0.4800f); 
const ZRectF CDlgParamEdit::m_conRectBtnValue(0.8715f, 0.5067f, 0.9696f, 0.6300f); 
const ZRectF CDlgParamEdit::m_conRectBtnDesc(0.8715f, 0.8100f, 0.9696f, 0.9367f);
const ZRectF CDlgParamEdit::m_conRectInModel(0.5600f, 0.5067f, 0.8715f, 0.6300f);  


// CDlgParamEdit 对话框

IMPLEMENT_DYNAMIC(CDlgParamEdit, CDialogEx)

CDlgParamEdit::CDlgParamEdit(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgParamEdit::IDD, pParent)
{
	m_zTreeParam.Init();
	m_eParamType = TypeNULL;
}

CDlgParamEdit::~CDlgParamEdit()
{
}

void CDlgParamEdit::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgParamEdit, CBCGPDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_DESCRIPTION, &CDlgParamEdit::OnBnClickedButtonDescription)
	ON_CBN_SELCHANGE(IDC_COMBO_DIRECTION, &CDlgParamEdit::OnCbnSelchangeComboDirection)
	ON_BN_CLICKED(IDC_BUTTON_VALUE, &CDlgParamEdit::OnBnClickedButtonValue)
	ON_CBN_SELCHANGE(IDC_COMBO_UNIT, &CDlgParamEdit::OnCbnSelchangeComboUnit)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CDlgParamEdit::OnCbnSelchangeComboType)
	ON_EN_CHANGE(IDC_EDIT_VALUE, &CDlgParamEdit::OnEnChangeEditValue)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE, &CDlgParamEdit::OnEnKillfocusEditValue)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CHECK_DYNAMIC, &CDlgParamEdit::OnBnClickedCheckDynamic)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, &CDlgParamEdit::OnEnKillfocusEditDescription)
	ON_EN_KILLFOCUS(IDC_EDIT_CNNAME, &CDlgParamEdit::OnEnKillfocusEditCnname)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgParamEdit 消息处理程序


int CDlgParamEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	EnableVisualManagerStyle (TRUE, TRUE);

	return 0;
}
BOOL CDlgParamEdit::ParamEditIsEnable()
{
	return m_bEnabled;
}
void CDlgParamEdit::SetParamEditIsEnable(BOOL bEnable)
{
	 m_bEnabled=bEnable;
}
void CDlgParamEdit::Enable(BOOL bEnable)
{
	m_bEnabled = bEnable;

	if(!m_bEnabled)
	{//清空
		GetDlgItem(IDC_STATIC_GROUP)->SetWindowText("参数属性设置");
		GetParent()->GetDlgItem(IDC_STATIC_PARAM_NAME)->SetWindowText("参数属性设置");

		((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(-1);

		GetDlgItem(IDC_EDIT_VALUE)->SetWindowText("");

		((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->SetCurSel(-1);

		GetDlgItem(IDC_EDIT_DESCRIPTION)->SetWindowText("");

		GetDlgItem(IDC_EDIT_CNNAME)->SetWindowText("");
		//GetDlgItem(IDC_EDIT_FORMAT)->SetWindowText("");

		((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->SetCurSel(-1);

		((CButton*)GetDlgItem(IDC_CHECK_DYNAMIC))->SetCheck(BST_UNCHECKED);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->EnableWindow(m_bEnabled);
	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->RedrawWindow();

	GetDlgItem(IDC_EDIT_VALUE)->EnableWindow(m_bEnabled);

	GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(m_bEnabled);
	((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->RedrawWindow();

	GetDlgItem(IDC_EDIT_DESCRIPTION)->EnableWindow(m_bEnabled);

	GetDlgItem(IDC_EDIT_CNNAME)->EnableWindow(m_bEnabled);
	//GetDlgItem(IDC_EDIT_FORMAT)->EnableWindow(m_bEnabled);

	((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->EnableWindow(m_bEnabled);
	((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->RedrawWindow();

	((CButton*)GetDlgItem(IDC_CHECK_DYNAMIC))->EnableWindow(m_bEnabled);

	((CButton*)GetDlgItem(IDC_BUTTON_VALUE))->EnableWindow(m_bEnabled);

	((CButton*)GetDlgItem(IDC_CHECK_INMODEL))->EnableWindow(m_bEnabled);

	((CButton*)GetDlgItem(IDC_BUTTON_DESCRIPTION))->EnableWindow(m_bEnabled);

	EnableWindow(m_bEnabled);

}

std::string CDlgParamEdit::TypeString2ComboBoxString(LPCTSTR str)
{
	std::string strRet = str;

	if(strRet.find("Array") != std::string::npos)
	{
		m_nDim = 1;
	}
	else if(strRet.find("2Darray") != std::string::npos)
	{
		m_nDim = 2;
	}
	else if(strRet.find("3Darray") != std::string::npos)
	{
		m_nDim = 3;
	}
	else
	{
		m_nDim = 0;
	}

	if(strRet == "Int")
	{
		strRet = "Integer";
	}
	else if(strRet == "IntArray" || strRet == "Int2Darray" || strRet == "Int3Darray")
	{
		strRet = "Integer[]";
	}
	else if(strRet == "DoubleArray" || strRet == "Double2Darray" || strRet == "Double3Darray")
	{
		strRet = "Double[]";
	}
	else if(strRet == "StringArray" || strRet == "String2Darray" || strRet == "String3Darray")
	{
		strRet = "String[]";
	}
	else if(strRet == "BooleanArray" || strRet == "Boolean2Darray" || strRet == "Boolean3Darray")
	{
		strRet = "Boolean[]";
	}


	return strRet;
}

std::string CDlgParamEdit::ComboBoxString2TypeString(LPCTSTR str)
{
	std::string strRet = str;

	std::string strT = "";
	switch(m_nDim)
	{
	case 1:
		strT = "A";
		break;
	case 2:
		strT = "2Da";
		break;
	case 3:
		strT = "3Da";
		break;
	}

	if(strRet == "Integer")
	{
		strRet = "Int";
	}

	else if(strRet =="Integer[]")
	{
		strRet = "Int" + strT + "rray";
	}
	else if(strRet =="Double[]")
	{
		strRet = "Double" + strT + "rray";
	}
	else if(strRet =="String[]")
	{
		strRet = "String" + strT + "rray";
	}
	else if(strRet =="Boolean[]")
	{
		strRet = "Boolean" + strT + "rray";
	}

	return strRet;
}

CDlgParamEdit::EParamType CDlgParamEdit::StringToType(LPCTSTR str)
{
//	EParamType eType;
	//modify by zhaoyw
	EParamType eType=TypeNULL;
	CString strT(str);

	if(strT == "File")
	{
		eType = TypeFile;
	}
	else if(strT == "String")
	{
		eType = TypeString;
	}
	else if(strT == "String[]")
	{
		eType = TypeStringArray;
	}
	else if(strT == "Integer")
	{
		eType = TypeInteger;
	}
	else if(strT == "Integer[]")
	{
		eType = TypeIntegerArray;
	}
	else if(strT == "Double")
	{
		eType = TypeDouble;
	}
	else if(strT == "Double[]")
	{
		eType = TypeDoubleArray;
	}
	else if(strT == "Boolean")
	{
		eType = TypeBool;
	}
	else if(strT == "Boolean[]")
	{
		eType = TypeBoolArray;
	}

	return eType;
}

void CDlgParamEdit::SetParamInfo( ZTreeParam& zTreeParam )
{
	CStringW str = CStringW(zTreeParam.m_strName.c_str());
	if(str.GetLength() > 15)
	{
		str = str.Left(12) + L"...";
	}
	GetDlgItem(IDC_STATIC_GROUP)->SetWindowText("参数[" + CString(str) + "]属性设置");
	GetParent()->GetDlgItem(IDC_STATIC_PARAM_NAME)->SetWindowText(("参数（" + zTreeParam.m_strName + "）属性设置").c_str());

	int n=-1;
	n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(zTreeParam.m_strType.c_str()).c_str());
	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->RedrawWindow();

	GetDlgItem(IDC_EDIT_CNNAME)->SetWindowText(zTreeParam.m_strCnName.c_str());

	//GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(zTreeParam.m_strValue.c_str());

	if(zTreeParam.m_strUnit.empty())
	{
		n = ((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->FindStringExact(-1, " ");
	}
	else
	{
		n = ((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->FindStringExact(-1, zTreeParam.m_strUnit.c_str());
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->SetCurSel(n);
	((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->RedrawWindow();

	GetDlgItem(IDC_EDIT_DESCRIPTION)->SetWindowText(zTreeParam.m_strDesrciption.c_str());

	//GetDlgItem(IDC_EDIT_FORMAT)->SetWindowText(zTreeParam.m_strFormat.c_str());

	n = ((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->FindStringExact(-1, zTreeParam.m_strDirection.c_str());
	((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->SetCurSel(n);
	((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->RedrawWindow();

	n = BST_UNCHECKED;
	if(zTreeParam.m_strIsDynamic == "true")
	{
		n = BST_CHECKED;
	}
	((CButton*)GetDlgItem(IDC_CHECK_DYNAMIC))->SetCheck(n);

	n = BST_CHECKED;
	if(zTreeParam.m_strIsSaveInModel == "false")
	{
		n = BST_UNCHECKED;
	}
	((CButton*)GetDlgItem(IDC_CHECK_INMODEL))->SetCheck(n);

	m_zTreeParam = zTreeParam;

	m_eParamType = StringToType(TypeString2ComboBoxString(zTreeParam.m_strType.c_str()).c_str());

 	if(m_zTreeParam.m_strValue.length() > 3000 && m_zTreeParam.m_strType.find("rray") != std::string::npos)
 	{//截断过长字符串（不能显示）
 		GetDlgItem(IDC_EDIT_VALUE)->SetWindowText((zTreeParam.m_strValue.substr(0, 200) + "...").c_str());
 	}
	else
	{
		GetDlgItem(IDC_EDIT_VALUE)->SetWindowText((zTreeParam.m_strValue.c_str()));
	}

	ChangeEditControl(m_eParamType, m_eParamType);
}

ZTreeParam CDlgParamEdit::GetParamInfo()
{
	ZTreeParam zTreeParam = m_zTreeParam;
	CString str;

	int n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
	///if(n!=-1)
	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetLBText(n, str);
	zTreeParam.m_strType = ComboBoxString2TypeString(str);

	GetDlgItem(IDC_EDIT_CNNAME)->GetWindowText(str);
	zTreeParam.m_strCnName = str;

	GetDlgItem(IDC_EDIT_VALUE)->GetWindowText(str);
	if(zTreeParam.m_strType.find("rray") == std::string::npos)
	{
		zTreeParam.m_strValue = str;
	}

	n = ((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->GetCurSel();
	//if(n!=-1)
	((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->GetLBText(n, str);
	if(str == " ") str = "";
	zTreeParam.m_strUnit = str;

	GetDlgItem(IDC_EDIT_DESCRIPTION)->GetWindowText(str);
	zTreeParam.m_strDesrciption = str;

// 	GetDlgItem(IDC_EDIT_FORMAT)->GetWindowText(str);
// 	zTreeParam.m_strFormat = str;

	n = ((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->GetCurSel();
//	if(n!=-1)
	((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->GetLBText(n, str);
	zTreeParam.m_strDirection = str;

	n = ((CButton*)GetDlgItem(IDC_CHECK_DYNAMIC))->GetCheck();
	if(n == BST_CHECKED)
	{
		str = "true";
	}
	else
	{
		str = "false";
	}

	zTreeParam.m_strIsDynamic = str;

	n = ((CButton*)GetDlgItem(IDC_CHECK_INMODEL))->GetCheck();
	if(n == BST_CHECKED)
	{
		str = "true";
	}
	else
	{
		str = "false";
	}

	zTreeParam.m_strIsSaveInModel = str;

	m_zTreeParam = zTreeParam;

	return zTreeParam;
}


void CDlgParamEdit::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CBCGPDialog::OnOK();
}


void CDlgParamEdit::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CBCGPDialog::OnCancel();
}


void CDlgParamEdit::OnBnClickedButtonDescription()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItem(IDC_EDIT_DESCRIPTION)->GetWindowText(str);
	CDlgInfoEdit dlg(str.GetBuffer(), "备注编辑", 200);
	if(dlg.DoModal() == IDOK)
	{
		if(CStringChecker::CheckLength(dlg.m_strText.c_str(), 200) == FALSE)
		{
			MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
		}		
		else 
		{
			GetDlgItem(IDC_EDIT_DESCRIPTION)->SetWindowText(dlg.m_strText.c_str());
			m_zTreeParam.m_strDesrciption = dlg.m_strText;
		}
	}
}

void CDlgParamEdit::OnCbnSelchangeComboDirection()
{
	// TODO: 在此添加控件通知处理程序代码

	CString str;
	//ZTreeParam paramChanage;
	//memset(&paramChanage, 0, sizeof(ZTreeParam));
	int n = ((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION))->GetLBText(n, str);
	
	CZGridRow* pRow = ((CDlgParamSet*)GetParent())->m_wndGridTree.m_pRowCur;

	if(pRow)
	{
		//paramChanage = GetParamInfo();
		((CDlgParamSet*)GetParent())->m_wndGridTree.SetRowImage(pRow, &GetParamInfo(), TRUE);
		((CDlgParamSet*)GetParent())->m_wndGridTree.UpdateParamFromEditControl();

		vector<ParamEventListener*>& vListeners = ((CDlgParamSet*)GetParent())->m_wndGridTree.m_vListeners;
		for(int i = 0; i < (int)vListeners.size(); i++)
		{
			vListeners[i]->ParamChanged(&m_zTreeParam);
		}
		
		GetParent()->SendMessage(WM_USER_SORTMSG,(WPARAM)&m_zTreeParam,(WPARAM)pRow);
	}
	

}

void ZTreeParam::Init()
{
	m_strName = "新建参数";
	m_strType = "String";
	m_strIsDynamic = "false";
	m_strIsSaveInModel = "true";
	m_strID = ReslibMgr::GuidToString(ReslibMgr::CreateGuid());
	transform(m_strID.begin(), m_strID.end(), m_strID.begin(), ::tolower);
	m_strDirection = "In";
}


void CDlgParamEdit::OnBnClickedButtonValue()
{
	// TODO: 在此添加控件通知处理程序代码
	switch (m_eParamType)
	{
	case TypeFile: //文件
		{
// 			CDlgParamEditFile dlg;
// 			dlg.m_strFilePath = m_zTreeParam.m_strValue;
// 			if(dlg.DoModal() == IDOK)
// 			{
// 				m_zTreeParam.m_strValue = dlg.m_strFilePath;
// 				SetParamInfo(m_zTreeParam);
// 			}

			{
				GetParamInfo();

				CFileDialog dlgFile(TRUE
					, NULL
					, NULL
					, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
					, "All Files (*.*)|*.*||"
					//, AfxGetMainWnd()
					);

				const int MAX_FILE_NUMBER = 100;                                                           //至少允许选择100个文件
				dlgFile.m_ofn.lpstrFile = new TCHAR[_MAX_PATH * MAX_FILE_NUMBER]; //重新定义缓冲区大小          
				memset(dlgFile.m_ofn.lpstrFile, 0, _MAX_PATH * MAX_FILE_NUMBER);  //初始化定义的缓冲区
				dlgFile.m_ofn.nMaxFile = _MAX_PATH*MAX_FILE_NUMBER; // 指定缓冲区大小

				CString strFilePath;

				if (dlgFile.DoModal() == IDOK)
				{
					strFilePath = dlgFile.GetPathName();
					if (!strFilePath.IsEmpty() && !PathFileExists(strFilePath))
					{
						CString sMsg;
						sMsg.Format("文件 %s 不存在", strFilePath);
						MessageBox(sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
					}
					else
					{
						m_zTreeParam.m_strValue = strFilePath;
						SetParamInfo(m_zTreeParam);
						//GetDlgItem(IDC_EDIT)->SetWindowText(strFilePath);
					}
				}
				delete[] dlgFile.m_ofn.lpstrFile;     //最后别忘了释放内存
			}

			break;
		}
	case TypeString: //字符串
		{
			CDlgInfoEdit dlg(m_zTreeParam.m_strValue, "String参数编辑");
			if(dlg.DoModal() == IDOK)
			{
// 				if(CStringChecker::CheckLength(dlg.m_strText.c_str(), 3000) == FALSE)
// 				{
// 					MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
// 				}		
// 				else 
				{
					m_zTreeParam.m_strValue = dlg.m_strText;
					SetParamInfo(m_zTreeParam);
				}
			}
			break;
		}
	case TypeStringArray: //
	case TypeIntegerArray: //
	case TypeDoubleArray: //
	case TypeBoolArray: //
		{
			CDlgParamEditArray dlg;
			dlg.m_strArray = m_zTreeParam.m_strValue;
			dlg.m_strName = m_zTreeParam.m_strName;
			switch (m_eParamType)
			{
			case TypeStringArray:
				dlg.m_dataType = CDlgParamEditArray::TypeString;
				break;
			case TypeIntegerArray:
				dlg.m_dataType = CDlgParamEditArray::TypeInteger;
				break;
			case TypeDoubleArray:
				dlg.m_dataType = CDlgParamEditArray::TypeDouble;
				break;
			case TypeBoolArray:
				dlg.m_dataType = CDlgParamEditArray::TypeBool;
				break;
			}
			if(dlg.DoModal() == IDOK)
			{
				m_zTreeParam.m_strValue = dlg.m_strArray;
				//((CEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetLimitText(1024 * 1024);
				m_nDim = dlg.m_vDim.size();
				CString str;
				int n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
				((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetLBText(n, str);
				m_zTreeParam.m_strType = ComboBoxString2TypeString(str);
				SetParamInfo(m_zTreeParam);
			}
			break;
		}
	}
}


void CDlgParamEdit::OnCbnSelchangeComboUnit()
{
	// TODO: 在此添加控件通知处理程序代码
	
}

void CDlgParamEdit::RefreshLayout()
{
	int n = 6;

	BOOL b;
	b = GetDlgItem(IDC_CHECK_DYNAMIC)->IsWindowEnabled();
	if(b)
	{//动态
		GetDlgItem(IDC_CHECK_DYNAMIC)->ShowWindow(SW_SHOW);
		CRect rectDynamic;
		GetDlgItem(IDC_CHECK_DYNAMIC)->GetWindowRect(rectDynamic);
		CRect rectDirection;
		GetDlgItem(IDC_COMBO_DIRECTION)->GetWindowRect(rectDirection);
		int nOffset = rectDynamic.left - n - rectDirection.right;
		rectDirection.OffsetRect(nOffset, 0);
		ScreenToClient(rectDirection);
		GetDlgItem(IDC_COMBO_DIRECTION)->MoveWindow(rectDirection);
		CRect rectType;
		GetDlgItem(IDC_COMBO_TYPE)->GetWindowRect(rectType);
		ScreenToClient(rectType);
		rectType.right = rectDirection.left - n;
		GetDlgItem(IDC_COMBO_TYPE)->MoveWindow(rectType);
	}
	else
	{
		GetDlgItem(IDC_CHECK_DYNAMIC)->ShowWindow(SW_HIDE);
		CRect rectDynamic;
		GetDlgItem(IDC_CHECK_DYNAMIC)->GetWindowRect(rectDynamic);
		CRect rectDirection;
		GetDlgItem(IDC_COMBO_DIRECTION)->GetWindowRect(rectDirection);
		int nOffset = rectDynamic.right - rectDirection.right;
		rectDirection.OffsetRect(nOffset, 0);
		ScreenToClient(rectDirection);
		GetDlgItem(IDC_COMBO_DIRECTION)->MoveWindow(rectDirection);
		CRect rectType;
		GetDlgItem(IDC_COMBO_TYPE)->GetWindowRect(rectType);
		ScreenToClient(rectType);
		rectType.right = rectDirection.left - n;
		GetDlgItem(IDC_COMBO_TYPE)->MoveWindow(rectType);
	}

	b = GetDlgItem(IDC_BUTTON_VALUE)->IsWindowEnabled();
	if(b)
	{//值按钮
		GetDlgItem(IDC_BUTTON_VALUE)->ShowWindow(SW_SHOW);
		CRect rectBtnValue;
		GetDlgItem(IDC_BUTTON_VALUE)->GetWindowRect(rectBtnValue);
		CRect rectValue;
		GetDlgItem(IDC_EDIT_VALUE)->GetWindowRect(rectValue);
		int nOffset = rectBtnValue.left - n - rectValue.right;
		rectValue.right += nOffset;
		ScreenToClient(rectValue);
		GetDlgItem(IDC_EDIT_VALUE)->MoveWindow(rectValue);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_VALUE)->ShowWindow(SW_HIDE);
		CRect rectBtnValue;
		GetDlgItem(IDC_BUTTON_VALUE)->GetWindowRect(rectBtnValue);
		CRect rectValue;
		GetDlgItem(IDC_EDIT_VALUE)->GetWindowRect(rectValue);
		int nOffset = rectBtnValue.right - rectValue.right;
		rectValue.right += nOffset;
		ScreenToClient(rectValue);
		GetDlgItem(IDC_EDIT_VALUE)->MoveWindow(rectValue);
	}

	b = GetDlgItem(IDC_CHECK_INMODEL)->IsWindowEnabled();
	if(b)
	{//文件是否保存在模型中
		GetDlgItem(IDC_CHECK_INMODEL)->ShowWindow(SW_SHOW);
		CRect rectCheckInModel;
		GetDlgItem(IDC_CHECK_INMODEL)->GetWindowRect(rectCheckInModel);
		CRect rectValue;
		GetDlgItem(IDC_EDIT_VALUE)->GetWindowRect(rectValue);
		rectValue.right -= rectCheckInModel.Width();
		ScreenToClient(rectValue);
		GetDlgItem(IDC_EDIT_VALUE)->MoveWindow(rectValue);
	}
	else
	{
		GetDlgItem(IDC_CHECK_INMODEL)->ShowWindow(SW_HIDE);
	}
}

void CDlgParamEdit::MoveItem(int nID, int nLeft, int nTop, int nRight, int nBottom)
{
	CRect rect;
	GetDlgItem(nID)->GetWindowRect(rect);
	ScreenToClient(rect);
	rect.left += nLeft;
	rect.right += nRight;
	rect.top += nTop;
	rect.bottom += nBottom;
	GetDlgItem(nID)->MoveWindow(rect);
}

void CDlgParamEdit::StretchX(int nX)
{
	MoveItem(IDC_STATIC_GROUP, 0, 0, nX, 0);
	MoveItem(IDC_EDIT_CNNAME, 0, 0, nX, 0);
	MoveItem(IDC_COMBO_TYPE, 0, 0, nX, 0);
	MoveItem(IDC_EDIT_VALUE, 0, 0, nX, 0);
	MoveItem(IDC_COMBO_UNIT, 0, 0, nX, 0);
	MoveItem(IDC_EDIT_DESCRIPTION, 0, 0, nX, 0);
	MoveItem(IDC_COMBO_DIRECTION, nX, 0, nX, 0);
	MoveItem(IDC_CHECK_DYNAMIC, nX, 0, nX, 0);
	MoveItem(IDC_BUTTON_VALUE, nX, 0, nX, 0);
	MoveItem(IDC_BUTTON_DESCRIPTION, nX, 0, nX, 0);
	MoveItem(IDC_CHECK_INMODEL, nX, 0, nX, 0);

}

//调整控件布局
void CDlgParamEdit::AdjustLayout()
{

	CRect rectFrame;
	GetWindowRect(rectFrame);
	CRect rectGroup;
	GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(rectGroup);
	//POINT pointOffset = {rectGroup.left - rectFrame.left, rectGroup.top - rectFrame.top};
	POINT pointOffset = {rectGroup.left - rectFrame.left, 6};
	
	rectGroup.MoveToXY(0, 0);
	CRect rect;

	rect = CalculateRect(m_conRectName, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_EDIT_CNNAME)->MoveWindow(rect);

	rect = CalculateRect(m_conRectType, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_COMBO_TYPE)->MoveWindow(rect);

	rect = CalculateRect(m_conRectValue, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_EDIT_VALUE)->MoveWindow(rect);

	rect = CalculateRect(m_conRectUnits, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_COMBO_UNIT)->MoveWindow(rect);

	rect = CalculateRect(m_conRectDesc, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(rect);

	rect = CalculateRect(m_conRectDirection, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_COMBO_DIRECTION)->MoveWindow(rect);

	rect = CalculateRect(m_conRectDynamic, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_CHECK_DYNAMIC)->MoveWindow(rect);

	rect = CalculateRect(m_conRectInModel, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_CHECK_INMODEL)->MoveWindow(rect);

	rect = CalculateRect(m_conRectBtnValue, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_BUTTON_VALUE)->MoveWindow(rect);

	rect = CalculateRect(m_conRectBtnDesc, rectGroup);
	rect.OffsetRect(pointOffset);
	GetDlgItem(IDC_BUTTON_DESCRIPTION)->MoveWindow(rect);

	BOOL b;
	b = GetDlgItem(IDC_CHECK_DYNAMIC)->IsWindowEnabled();
	if(b)
	{//动态
		GetDlgItem(IDC_CHECK_DYNAMIC)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_CHECK_DYNAMIC)->ShowWindow(SW_HIDE);
		CRect rectDynamic;
		GetDlgItem(IDC_CHECK_DYNAMIC)->GetWindowRect(rectDynamic);
		CRect rectDirection;
		GetDlgItem(IDC_COMBO_DIRECTION)->GetWindowRect(rectDirection);
		int nOffset = rectDynamic.right - rectDirection.right;
		rectDirection.OffsetRect(nOffset, 0);
		ScreenToClient(rectDirection);
		GetDlgItem(IDC_COMBO_DIRECTION)->MoveWindow(rectDirection);
		CRect rectType;
		GetDlgItem(IDC_COMBO_TYPE)->GetWindowRect(rectType);
		rectType.right += nOffset;
		ScreenToClient(rectType);
		GetDlgItem(IDC_COMBO_TYPE)->MoveWindow(rectType);
	}

	b = GetDlgItem(IDC_BUTTON_VALUE)->IsWindowEnabled();
	if(b)
	{//值按钮
		GetDlgItem(IDC_BUTTON_VALUE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_VALUE)->ShowWindow(SW_HIDE);
		CRect rectBtnValue;
		GetDlgItem(IDC_BUTTON_VALUE)->GetWindowRect(rectBtnValue);
		CRect rectValue;
		GetDlgItem(IDC_EDIT_VALUE)->GetWindowRect(rectValue);
		int nOffset = rectBtnValue.right - rectValue.right;
		rectValue.right += nOffset;
		ScreenToClient(rectValue);
		GetDlgItem(IDC_EDIT_VALUE)->MoveWindow(rectValue);
	}

	b = GetDlgItem(IDC_CHECK_INMODEL)->IsWindowEnabled();
	if(b)
	{//文件是否保存在模型中
		GetDlgItem(IDC_CHECK_INMODEL)->ShowWindow(SW_SHOW);
		CRect rectCheckInModel;
		GetDlgItem(IDC_CHECK_INMODEL)->GetWindowRect(rectCheckInModel);
		CRect rectValue;
		GetDlgItem(IDC_EDIT_VALUE)->GetWindowRect(rectValue);
		rectValue.right -= rectCheckInModel.Width();
		ScreenToClient(rectValue);
		GetDlgItem(IDC_EDIT_VALUE)->MoveWindow(rectValue);
	}
	else
	{
		GetDlgItem(IDC_CHECK_INMODEL)->ShowWindow(SW_HIDE);
	}
}

void  CDlgParamEdit::ChangeEditControl(EParamType oldType, EParamType newType)
{
	switch (newType)
	{
	case TypeFile: //文件
		{
			if(oldType == newType)
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(FALSE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(TRUE);
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(TRUE);
			}
			else
			{
				int nRet = IDOK;
				if(!m_zTreeParam.m_strValue.empty()) nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
				switch (nRet)
				{
				case IDOK:
					{
						m_nDim = 0;
						ZTreeParam param = GetParamInfo();
						param.m_strValue = "";
						//param.m_strIsDynamic = "false";
						param.m_strUnit = "";
						param.m_strIsSaveInModel = "true";

						SetParamInfo(param);

					}
					break;
				case IDCANCEL:
					{
						int n;
						n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
						((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
					}
				default:
					break;
				}
			}
			break;
		}
	case TypeString: //String
		{
			if(oldType != newType)
			{
				if(oldType != TypeBool && oldType != TypeDouble && oldType != TypeFile && oldType != TypeInteger)
				{
					int nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
					switch (nRet)
					{
					case IDOK:
						{
							m_nDim = 0;
							ZTreeParam param = GetParamInfo();
							param.m_strValue = "";
							param.m_strIsDynamic = "false";
							param.m_strUnit = "";
							SetParamInfo(param);
						}
						break;
					case IDCANCEL:
						{
							int n;
							n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
							((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
						}
					default:
						break;
					}
				}
				else 
				{
					m_nDim = 0;
					ZTreeParam param = GetParamInfo();
					param.m_strIsDynamic = "false";
					param.m_strUnit = "";
					//param.m_strValue = "";
					SetParamInfo(param);
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(FALSE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(FALSE);	
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	case TypeStringArray: //StringArray
		{
			if(oldType != newType)
			{
				if(oldType != TypeIntegerArray && oldType != TypeDoubleArray && oldType != TypeStringArray && oldType != TypeBoolArray)
				{//其他类型变为String数组
					m_nDim = 1;
					ZTreeParam param = GetParamInfo();
					std::string& str = param.m_strValue;
					string_replace(str, "\\", "\\\\");
					string_replace(str, "\"", "\\\"");

					param.m_strIsDynamic = "false";
					param.m_strUnit = "";
					param.m_strValue = "[\"" + str + "\"](1)";
					SetParamInfo(param);
					break;
				}
				else
				{//其他数组变为String数组
					ZTreeParam param = GetParamInfo();
					std::string str = param.m_strValue;

					int nStart = str.find('[');
					int nEnd = str.find(']');

					if(nEnd <= nStart + 1)
					{
						break;
					}

					str = str.substr(nStart + 1, nEnd - nStart - 1);
					string_replace(str, ",", "\",\"");

					param.m_strUnit = "";
					param.m_strValue = "[\"" + str + "\"]" + "(" + GetLengthString(param.m_strValue) + ")";
					SetParamInfo(param);

					break;
				}

				int nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
				switch (nRet)
				{
				case IDOK:
					{
						m_nDim = 1;
						ZTreeParam param = GetParamInfo();
						param.m_strIsDynamic = "false";
						param.m_strUnit = "";
						param.m_strValue = "[\"\"](1)";
						SetParamInfo(param);
					}
					break;
				case IDCANCEL:
					{
						int n;
						n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
						((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
					}
				default:
					break;
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(TRUE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(TRUE);		
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	case TypeInteger: //Integer
		{
			if(oldType != newType)
			{
				if(CStringChecker::CheckInt(m_zTreeParam.m_strValue.c_str())/* || CStringChecker::CheckDouble(m_zTreeParam.m_strValue.c_str())*/ ) 
				{
					m_nDim = 0;
					ZTreeParam param = GetParamInfo();
					param.m_strIsDynamic = "false";
					param.m_strUnit = "";
					//param.m_strValue = "";
					std::stringstream ss;
					ss << m_zTreeParam.m_strValue;
					double n = 0;
					ss >> n;
					ss.str("");
					ss.clear();
					ss << (int)n;
					param.m_strValue = ss.str();
					SetParamInfo(param);
				}
				else
				{
					int nRet = IDOK;
					if(!m_zTreeParam.m_strValue.empty()) nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
					switch (nRet)
					{
					case IDOK:
						{
							m_nDim = 0;
							ZTreeParam param = GetParamInfo();
							param.m_strValue = "0";
							param.m_strIsDynamic = "false";
							param.m_strUnit = "";
							SetParamInfo(param);
						}
						break;
					case IDCANCEL:
						{
							int n;
							n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
							((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
						}
					default:
						break;
					}
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(FALSE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(FALSE);
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	case TypeIntegerArray: //IntegerArray
		{
			if(oldType != newType)
			{
				switch(oldType)
				{
				case TypeInteger:
				case TypeDouble:
				case TypeString:
					{
						if(!CStringChecker::CheckInt(m_zTreeParam.m_strValue.c_str())) goto end;
						m_nDim = 1;
						ZTreeParam param = GetParamInfo();
						param.m_strIsDynamic = "false";
						param.m_strUnit = "";
						std::stringstream ss;
						ss << m_zTreeParam.m_strValue;
						double n = 0;
						ss >> n;
						ss.str("");
						ss.clear();
						ss << (int)n;
						param.m_strValue = "[" + ss.str() + "](1)";
						SetParamInfo(param);
						break;
					}
				case TypeDoubleArray:
				case TypeStringArray:
					{
						ZTreeParam param = m_zTreeParam;
						param.m_strUnit = "";
						Json::Value jv;
						Json::Reader jReader;
						if(!jReader.parse(param.m_strValue.substr(0, param.m_strValue.find_last_of('(')), jv) || !jv.isArray())break;
						std::stringstream ss;
						ss << '[';
						BOOL bFlagMsg = FALSE;
						BOOL bNeedShowMsg = FALSE;
						for(int i = 0; i < (int)jv.size(); i++)
						{
							int n = 0;
							if(jv[i].isNumeric())
							{
								double d = jv[i].asDouble(); // 浮点型到整形转换截断提示
								 n = (int)d;
								 if (abs(d - n*1.0) >0.0)
									bNeedShowMsg = TRUE;
							}
							else if(jv[i].isString())
							{
								std::stringstream ssTemp;
								ssTemp << jv[i].asString();
								ssTemp >> n;
								if (!CStringChecker::CheckInt(jv[i].asString().c_str()))
								{
									bNeedShowMsg = TRUE;
									n = 0;
								}
							}

							if(!bFlagMsg && bNeedShowMsg)
							{
								int nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
								if(nRet == IDOK)
								{
									bFlagMsg = TRUE;
								}
								else
								{
									int n;
									n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
									((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
									return;
								}
							}

							ss << n;
							if(i != jv.size() - 1)
							{
								ss << ',';
							}
						}
						ss << ']' << '(' << GetLengthString(param.m_strValue) << ')';
						param.m_strValue = ss.str();
						GetParamInfo();
						param.m_strType = m_zTreeParam.m_strType;
						SetParamInfo(param);
						break;
					}
				default:
					end:
					{
						int nRet = IDOK;
						if(!m_zTreeParam.m_strValue.empty()) nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
						switch (nRet)
						{
						case IDOK:
							{
								m_nDim = 1;
								ZTreeParam param = GetParamInfo();
								param.m_strValue = "[0](1)";
								//param.m_strIsDynamic = "false";
								param.m_strUnit = "";
								SetParamInfo(param);
							}
							break;
						case IDCANCEL:
							{
								int n;
								n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
								((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
							}
						default:
							break;
						}
						break;
					}
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(TRUE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(TRUE);	
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	case TypeDouble: //Double
		{
			if(oldType != newType)
			{
				if(/*oldType != TypeInteger && */!CStringChecker::CheckDouble(m_zTreeParam.m_strValue.c_str()))
				{
					int nRet = IDOK;
					if(!m_zTreeParam.m_strValue.empty()) nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
					switch (nRet)
					{
					case IDOK:
						{
							m_nDim = 0;
							ZTreeParam param = GetParamInfo();
							param.m_strValue = "0.0";
							param.m_strIsDynamic = "false";
							param.m_strUnit = "";
							SetParamInfo(param);
						}
						break;
					case IDCANCEL:
						{
							int n;
							n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
							((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
						}
					default:
						break;
					}
				}
				else 
				{
					m_nDim = 0;
					ZTreeParam param = GetParamInfo();
					param.m_strIsDynamic = "false";
					param.m_strUnit = "";
					std::string str = m_zTreeParam.m_strValue;
					if(str.find('.', 0) == std::string::npos && str.find('e', 0) == std::string::npos && str.find('E', 0) == std::string::npos)  param.m_strValue = m_zTreeParam.m_strValue + ".0";
					SetParamInfo(param);
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(FALSE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(FALSE);	
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	case TypeDoubleArray: //DoubleArray
		{
			if(oldType != newType)
			{
				switch(oldType)
				{
				case TypeInteger:
				case TypeDouble:
				case TypeString:
					{
						if(!CStringChecker::CheckDouble(m_zTreeParam.m_strValue.c_str())) goto end_double;
						m_nDim = 1;
						ZTreeParam param = GetParamInfo();
						param.m_strIsDynamic = "false";
						param.m_strUnit = "";
						std::stringstream ss;
						ss << m_zTreeParam.m_strValue;
						double n = 0;
						ss >> n;
						ss.str("");
						ss.clear();
						ss << n;
						std::string str = ss.str();
						if(str.find('.', 0) == std::string::npos && str.find('e', 0) == std::string::npos && str.find('E', 0) == std::string::npos) str += ".0";
						param.m_strValue = "[" + str + "](1)";
						SetParamInfo(param);
						break;
					}
				case TypeIntegerArray:
				case TypeStringArray:
					{
						ZTreeParam param = m_zTreeParam;
						param.m_strUnit = "";
						Json::Value jv;
						Json::Reader jReader;
						if(!jReader.parse(param.m_strValue.substr(0, param.m_strValue.find_last_of('(')), jv) || !jv.isArray())break;
						std::stringstream ss;
						ss << '[';
						BOOL bFlagMsg = FALSE;
						for(int i = 0; i < (int)jv.size(); i++)
						{
							double n = 0;
							std::stringstream ssTemp;
							if(jv[i].isNumeric())
							{
								n = jv[i].asDouble();
							}
							else if(jv[i].isString())
							{
								ssTemp << jv[i].asString();
								ssTemp >> n;
								if (!CStringChecker::CheckDouble(jv[i].asString().c_str()))
								{
									n = 0;
									if(!bFlagMsg)
									{
										int nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
										if(nRet == IDOK)
										{
											bFlagMsg = TRUE;
										}
										else
										{
											int n;
											n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
											((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
											return;
										}
									}
								}
							}
							ssTemp.str("");
							ssTemp.clear();
							ssTemp << n;
							std::string str = ssTemp.str();
							if(str.find('.', 0) == std::string::npos && str.find('e', 0) == std::string::npos && str.find('E', 0) == std::string::npos) str += ".0";
							ss << str;
							if(i != jv.size() - 1)
							{
								ss << ',';
							}
						}
						ss << ']' << '(' << GetLengthString(param.m_strValue) << ')';
						param.m_strValue = ss.str();
						GetParamInfo();
						param.m_strType = m_zTreeParam.m_strType;
						SetParamInfo(param);
						break;
					}
				default:
					end_double:
					{
						int nRet = IDOK;
						if(!m_zTreeParam.m_strValue.empty()) nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
						switch (nRet)
						{
						case IDOK:
							{
								m_nDim = 1;
								ZTreeParam param = GetParamInfo();
								param.m_strValue = "[0.0](1)";
								//param.m_strIsDynamic = "false";
								param.m_strUnit = "";
								SetParamInfo(param);
							}
							break;
						case IDCANCEL:
							{
								int n;
								n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
								((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
							}
						default:
							break;
						}
						break;
					}
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(TRUE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(TRUE);	
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	case TypeBool: //Bool
		{
			if(oldType != newType)
			{
				switch(oldType)
				{
				case TypeString:
					{
						std::string str = m_zTreeParam.m_strValue;
						if(str == "false" || str == "true" )
						{
							m_nDim = 0;
							ZTreeParam param = GetParamInfo();
							param.m_strIsDynamic = "false";
							param.m_strUnit = "";
							param.m_strValue = str;
							SetParamInfo(param);
							return;
						}
					}
				}
				int nRet = IDOK;
				if(!m_zTreeParam.m_strValue.empty()) nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
				switch (nRet)
				{
				case IDOK:
					{
						m_nDim = 0;
						ZTreeParam param = GetParamInfo();
						param.m_strIsDynamic = "false";
						param.m_strUnit = "";
						param.m_strValue = "false";
						SetParamInfo(param);
					}
					break;
				case IDCANCEL:
					{
						int n;
						n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
						((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
					}
				default:
					break;
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(FALSE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(TRUE);	
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	case TypeBoolArray: //BoolArray
		{
			if(oldType != newType)
			{
				switch (oldType)
				{
				case TypeBool:
					{
						m_nDim = 1;
						ZTreeParam param = GetParamInfo();
						param.m_strIsDynamic = "false";
						param.m_strValue = "[" + m_zTreeParam.m_strValue + "](1)";
						SetParamInfo(param);
						break;
					}
				case TypeStringArray:
					{
						ZTreeParam param = m_zTreeParam;
						param.m_strUnit = "";
						Json::Value jv;
						Json::Reader jReader;
						if(!jReader.parse(param.m_strValue.substr(0, param.m_strValue.find_last_of('(')), jv) || !jv.isArray())break;
						std::stringstream ss;
						ss << '[';
						BOOL bFlagMsg = FALSE;
						for(int i = 0; i < (int)jv.size(); i++)
						{
							std::string str = "false";
							std::stringstream ssTemp;
							if(jv[i].isString())
							{
								std::string strTemp = jv[i].asString();
								if(strTemp == "true") str = strTemp;
								else if(strTemp != "false")
								{
									if(!bFlagMsg)
									{
										int nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
										if(nRet == IDOK)
										{
											bFlagMsg = TRUE;
										}
										else
										{
											int n;
											n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
											((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
											return;
										}
									}
								}
							}
							ss << str;
							if(i != jv.size() - 1)
							{
								ss << ',';
							}
						}
						ss << ']' << '(' << GetLengthString(param.m_strValue) << ')';
						param.m_strValue = ss.str();
						GetParamInfo();
						param.m_strType = m_zTreeParam.m_strType;
						SetParamInfo(param);
						break;
					}
				default:
					//end_bool_array:
					{
						int nRet = IDOK;
						if(!m_zTreeParam.m_strValue.empty()) nRet = MessageBox( "参数类型与之前的不一致，可能会丢失数据，是否继续？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
						switch (nRet)
						{
						case IDOK:
							{
								m_nDim = 1;
								ZTreeParam param = GetParamInfo();
								param.m_strValue = "[false](1)";
								//param.m_strIsDynamic = "false";
								param.m_strUnit = "";
								SetParamInfo(param);
							}
							break;
						case IDCANCEL:
							{
								int n;
								n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->FindStringExact(-1, TypeString2ComboBoxString(m_zTreeParam.m_strType.c_str()).c_str());
								((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(n);
							}
						default:
							break;
						}
						break;
					}
				}
			}
			else
			{
				GetDlgItem(IDC_COMBO_UNIT)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_DYNAMIC)->EnableWindow(TRUE);
				((CBCGPEdit*)GetDlgItem(IDC_EDIT_VALUE))->SetReadOnly(TRUE);		
				GetDlgItem(IDC_BUTTON_VALUE)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);
			}
			break;
		}
	}
	RefreshLayout();
}


void CDlgParamEdit::OnCbnSelchangeComboType()
{
	// TODO: 在此添加控件通知处理程序代码

	CString str;
	int n = ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetLBText(n, str);

	EParamType eOldType = m_eParamType;

	m_eParamType = StringToType(str);

	if(m_eParamType != eOldType)
	{
		ChangeEditControl(eOldType, m_eParamType);
	}

	m_zTreeParam = GetParamInfo();
	SetParamInfo(m_zTreeParam);
	((CDlgParamSet*)GetParent())->m_wndGridTree.UpdateParamFromEditControl();

	vector<ParamEventListener*>& vListeners = ((CDlgParamSet*)GetParent())->m_wndGridTree.m_vListeners;
	for(int i = 0; i < (int)vListeners.size(); i++)
	{
		vListeners[i]->ParamChanged(&m_zTreeParam);
	}

}


void CDlgParamEdit::OnEnChangeEditValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

}


void CDlgParamEdit::OnEnKillfocusEditValue()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;

	GetDlgItem(IDC_EDIT_VALUE)->GetWindowText(str);

	switch(m_eParamType)
	{
	case TypeInteger:
		{
			str.Trim(' ');
			if(CStringChecker::CheckInt(str) == FALSE)
			{
				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
				GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(m_zTreeParam.m_strValue.c_str());
				//GetDlgItem(IDC_EDIT_VALUE)->SetFocus();
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//松开左键
				return;
			}
// 			if(CStringChecker::CheckLength(str, 10) == FALSE)
// 			{
// 				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
// 				GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(m_zTreeParam.m_strValue.c_str());
// 				//GetDlgItem(IDC_EDIT_VALUE)->SetFocus();
// 				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//松开左键
// 				return;
// 			}
			break;
		}
	case TypeDouble:
		{
			str.Trim(' ');
			if(CStringChecker::CheckDouble(str) == FALSE)
			{
				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
				GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(m_zTreeParam.m_strValue.c_str());
				//GetDlgItem(IDC_EDIT_VALUE)->SetFocus();
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//松开左键
				return;
			}
// 			if(CStringChecker::CheckLength(str, 20) == FALSE)
// 			{
// 				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
// 				GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(m_zTreeParam.m_strValue.c_str());
// 				//GetDlgItem(IDC_EDIT_VALUE)->SetFocus();
// 				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//松开左键
// 				return;
// 			}
			str = CStringChecker::FormatDouble(str);
			GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(str);
			break;
		}
	case TypeString:
		{
// 			if(CStringChecker::CheckLength(str, 3000) == FALSE)
// 			{
// 				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
// 				GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(m_zTreeParam.m_strValue.c_str());
// 				//GetDlgItem(IDC_EDIT_VALUE)->SetFocus();
// 				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//松开左键
// 				return;
// 			}
			break;
		}
	}

	if(m_zTreeParam.m_strType.find("rray") == std::string::npos)
	{
		m_zTreeParam.m_strValue = str;
		//GetDlgItem(IDC_EDIT_VALUE)->SetWindowText(str);
	}

	return;
}


void CDlgParamEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	CBCGPDialog::OnLButtonDown(nFlags, point);
}


BOOL CDlgParamEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( pMsg->message == WM_LBUTTONDOWN && GetDlgItem(IDC_EDIT_VALUE)->GetSafeHwnd() == pMsg->hwnd)
	{        
		if(m_eParamType == TypeBool)
		{
			ZTreeParam param = GetParamInfo();

			if(param.m_strValue == "false")
			{
				param.m_strValue = "true";
			}
			else
			{
				param.m_strValue = "false";
			}

			m_zTreeParam = param;
			SetParamInfo(m_zTreeParam);
		}
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CDlgParamEdit::OnBnClickedCheckDynamic()
{
	// TODO: 在此添加控件通知处理程序代码
	GetParamInfo();
}


BOOL CDlgParamEdit::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();


	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_STATIC_GROUP)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_CHECK_INMODEL)->EnableWindow(FALSE);

	AdjustLayout();
	((CEdit*)GetDlgItem(IDC_EDIT_DESCRIPTION))->SetLimitText(200);
	((CEdit*)GetDlgItem(IDC_EDIT_CNNAME))->SetLimitText(20);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgParamEdit::OnEnKillfocusEditDescription()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;

	GetDlgItem(IDC_EDIT_DESCRIPTION)->GetWindowText(str);

	m_zTreeParam.m_strDesrciption = str;
}


void CDlgParamEdit::OnEnKillfocusEditCnname()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;

	GetDlgItem(IDC_EDIT_CNNAME)->GetWindowText(str);

	m_zTreeParam.m_strCnName = str;
}


BOOL CDlgParamEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(rect);
	CRect rectEdit;
	GetDlgItem(IDC_STATIC_CNNAME)->GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rect.left = rectEdit.left;
	GetDlgItem(IDC_EDIT_CNNAME)->GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rect.top = rectEdit.top;
	rect.right = rectEdit.right;
	GetDlgItem(IDC_EDIT_DESCRIPTION)->GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rect.bottom = rectEdit.bottom;
	//rect.DeflateRect(10, 10, 10, 10);
	CBrush brush(RGB(251, 251, 251));
	pDC->FillRect (rect, &brush);

	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_EDIT_CNNAME);
	pWnd->RedrawWindow();

	pWnd = GetDlgItem(IDC_EDIT_VALUE);
	pWnd->RedrawWindow();
	pWnd = GetDlgItem(IDC_EDIT_DESCRIPTION);
	pWnd->RedrawWindow();
	return TRUE;

	return CBCGPDialog::OnEraseBkgnd(pDC);
}
