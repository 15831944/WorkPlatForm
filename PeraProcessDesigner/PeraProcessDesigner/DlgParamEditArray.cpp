// DlgParamEditArray.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgParamEditArray.h"
#include "afxdialogex.h"
#include <sstream>
#include"reader.h"
#include "Tools.h"

static const LPCTSTR arrColName[] = {
	"��������", "����ֵ"
};
static const int arrColWidth[] = {
	130, 360 
};

LPCTSTR CDlgParamEditArray::m_strDefaultDatas[] = {
	"0",
	"",
	"0.0",
	"false"
};

// CDlgParamEditArray �Ի���

IMPLEMENT_DYNAMIC(CDlgParamEditArray, CCxBCGPDialog)

CDlgParamEditArray::CDlgParamEditArray(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgParamEditArray::IDD, pParent)
{
	SetEdited(FALSE);
	m_bInit = FALSE;
}

CDlgParamEditArray::~CDlgParamEditArray()
{
}

void CDlgParamEditArray::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgParamEditArray, CCxBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CDlgParamEditArray::OnBnClickedButtonEdit)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgParamEditArray::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CDlgParamEditArray::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CDlgParamEditArray::OnEnChangeEdit3)
END_MESSAGE_MAP()


// CDlgParamEditArray ��Ϣ�������


void CDlgParamEditArray::OnBnClickedButtonEdit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(IsEdited())
	{
		int nCount = 0;
		for(int i = 0; i < (int)m_vDim.size(); i++)
		{
			if(i == 0) nCount = m_vDim[i];
			else
			{
				nCount = m_vDim[i] * nCount;
			}
		}
		if(nCount > m_maxNum)
		{
			MessageBox( "���鳤�ȳ������ƣ�����������", g_lpszAppTitle, MB_ICONWARNING);
			return;
		}
		m_vArray.assign(nCount, m_strDefaultDatas[m_dataType]);

		LoadArrayList();

		if(m_vArray.size() > 0)
		{
			SetEdited(FALSE);
		}
	}
	else
	{
		return;
	}
}

BOOL CDlgParamEditArray::StringToArray()
{
	if(m_strArray.empty())
	{
		return FALSE;
	}

	if(m_strArray.find('[') == std::string::npos || m_strArray.find(']') == std::string::npos || m_strArray.find('(') == std::string::npos || m_strArray.find(')') == std::string::npos)
	{
		return FALSE;
	}

	int nStart = 0;
	int nEnd = 0;
	std::string str;
////////////����
	nStart = m_strArray.find('[');
	nEnd = m_strArray.rfind(']');

	if(nEnd <= nStart + 1)
	{
		return FALSE;
	}
	
	m_vArray.clear();
	m_vDim.clear();

	str = m_strArray.substr(nStart + 1, nEnd - nStart - 1);

	switch (m_dataType)
	{
	case TypeInteger:
	case TypeDouble:
	case TypeBool:
		{
			int nPos = 0;
			std::string strT;
			for(int i = 0; i < (int)str.length(); i++)
			{
				if(str[i] != ' ' && str[i] != ',')
				{
					strT.append(1, str[i]);
				}
				else if(str[i] == ',')
				{
					m_vArray.push_back(strT);
					nPos ++;
					strT = "";
				}
			}
			m_vArray.push_back(strT);
			break;
		}
	case TypeString:
		{
			Json::Reader jsRead;
			Json::Value obj;
			str = string("[") + str + "]";
			if (!jsRead.parse(str,obj,false))
				break;
			if (!obj.isArray())
				break;
			int iSize = obj.size();
			int i=0;
			while(i< iSize)
			{
				string strValueI =obj[i++].asCString();
				m_vArray.push_back(strValueI);
			}
			break;
		}
	}
////////////////ά��
	nStart = m_strArray.find_last_of('(');
	nEnd = m_strArray.find_last_of(')');

	if(nEnd <= nStart + 1)
	{
		return FALSE;
	}

	str = m_strArray.substr(nStart + 1, nEnd - nStart - 1);
	vector<string> vDimNumArry;
	string strDelim(",");
	ZTools::split(str,strDelim,&vDimNumArry);
	for (size_t i=0;i<vDimNumArry.size();i++)
	{
		stringstream stream;
		int n = 0;
		stream << vDimNumArry[i];
		stream >> n;
		m_vDim.push_back(n);
	}
	return TRUE;
}

void string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst )
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
	{
		strBig.replace( pos, srclen, strdst );
		pos += dstlen;
	}
} 



void CDlgParamEditArray::ArrayToString()
{
	m_strArray = "";

	//����
	switch (m_dataType)
	{
	case TypeInteger:
	case TypeDouble:
	case TypeBool:
		{
			m_strArray.append(1, '[');
			for(int i = 0; i < (int)m_vArray.size(); i++)
			{
				m_strArray += m_vArray[i];
				if(i < (int)m_vArray.size() - 1)
				{
					m_strArray.append(1, ',');
				}
			}
			m_strArray.append(1, ']');
			break;
		}
	case TypeString:
		{
			m_strArray.append(1, '[');
			for(int i = 0; i < (int)m_vArray.size(); i++)
			{
				std::string str = m_vArray[i];
				string_replace(str, "\\", "\\\\");
				string_replace(str, "\"", "\\\"");
				m_strArray += '"' + str + '"';
				if(i < (int)m_vArray.size() - 1)
				{
					m_strArray.append(1, ',');
				}
			}
			m_strArray.append(1, ']');
			break;
		}
	}

	//ά��
	m_strArray.append(1, '(');
	for(int i = 0; i < (int)m_vDim.size(); i++)
	{
		stringstream ss;
		ss << m_vDim[i];
		m_strArray += ss.str();

		if(i < (int)m_vDim.size() - 1)
		{
			m_strArray.append(1, ',');
		}
	}
	m_strArray.append(1, ')');
		
}

void CDlgParamEditArray::RefreshEdit()
{
	std::vector<int> vDim = m_vDim;
	for(int i = 0; i < (int)vDim.size(); i++)
	{
		CString str;
		str.Format("%d", vDim[i]);
		int nID = IDC_EDIT1;
		switch(i)
		{
		case 0:
			nID = IDC_EDIT1;
			break;
		case 1:
			nID = IDC_EDIT2;
			break;
		case 2:
			nID = IDC_EDIT3;
			break;
		}
		GetDlgItem(nID)->SetWindowText(str);						
	}
	SetEdited(FALSE);
}

BOOL CDlgParamEditArray::OnInitDialog()
{
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);

	CCxBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	StringToArray();

	RefreshEdit(); //��������������Ҫ�ٵ���һ��ToArray
	
	//StringToArray();

	InitGrid(); //��ʼ��������

	m_bInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgParamEditArray::UpdateArrayFromGrid()
{

}

void CDlgParamEditArray::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	if(IsEdited())
	{
		MessageBox( "�����ȱ༭����Ĭ��ֵ��", g_lpszAppTitle, MB_ICONWARNING);
		return;
	}

	UpdateArrayFromGrid();
	ArrayToString();
	CCxBCGPDialog::OnOK();
}

void CDlgParamEditArray::InitGrid()
{
	CWnd* wndGrid = GetDlgItem(IDC_STATIC_GRID);
	CRect rc;
	wndGrid->GetClientRect (&rc);
	wndGrid->MapWindowPoints (this, &rc);
	//wndGrid->ShowWindow(SW_HIDE);
	rc.DeflateRect(1, 1, 1, 1);

	m_arrayGrid.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, this, (UINT)-1);
	m_arrayGrid.EnableColumnAutoSize (TRUE);
	m_arrayGrid.SetSingleSel (TRUE);
	m_arrayGrid.EnableGroupByBox (FALSE);
	//m_arrayGrid.SetReadOnly (TRUE);
	m_arrayGrid.SetWholeRowSel (TRUE);
	m_arrayGrid.EnableHeader (TRUE, 0);

	m_arrayGrid.SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	//m_arrayGrid.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);

	//������ɫ
	CBCGPGridColors theme;
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme (&m_arrayGrid, theme);
	theme.m_clrBackground = RGB(249, 249, 249);
	theme.m_HeaderSelColors.InitColors();

	m_arrayGrid.SetColorTheme (theme);

	//��ʼ����
	int nColCount = sizeof( arrColName ) / sizeof( LPCTSTR );
	for (int i=0; i<nColCount; i++)
	{
		m_arrayGrid.InsertColumn ( i, arrColName[i], arrColWidth[i] );
		m_arrayGrid.SetHeaderAlign(i, HDF_CENTER);
		m_arrayGrid.SetColumnAlign(i, HDF_CENTER);
	}
	//ȡ��������
	LoadArrayList();
	//����
	m_arrayGrid.AdjustLayout ();
	//return TRUE;
}

BOOL CDlgParamEditArray::LoadArrayList( void )
{
	if ( !IsWindow( m_arrayGrid.GetSafeHwnd() ) ) return FALSE;
	if ( m_arrayGrid.GetColumnCount() <= 0 ) return FALSE;

	const int nColumns = m_arrayGrid.GetColumnCount ();
	m_arrayGrid.RemoveAll();

	CBCGPGridRow * pRow = NULL;

	std::vector<int> vDim = m_vDim;

	int nDim = vDim.size();

	if(nDim < 1) return FALSE;

	for ( int i=0; i<(int)m_vArray.size(); i++ )
	{
		pRow = m_arrayGrid.CreateRow (nColumns);
		//pRow->AllowSubItems ();
		std::string str = m_strName;
		str.append(1, '[');

		std::string strT;
		int nRemain = i;
		for(int j = nDim; j > 0; j--)
		{
			std::stringstream ss;
			int nCount = vDim[j - 1];
			int n = nRemain % nCount;
			nRemain = nRemain / nCount;

			ss << n;
			strT = ss.str() + strT;
			if(j != 1)
			{
				strT = ',' + strT;
			}
		}

		str += strT;
		str.append(1, ']');

		pRow->GetItem (COL_NAME)->SetValue ( (LPCTSTR)str.c_str() );
		pRow->GetItem (COL_VALUE)->SetValue ( (LPCTSTR)m_vArray[i].c_str());
		pRow->GetItem (COL_NAME)->AllowEdit(FALSE);
		if (m_dataType== CDlgParamEditArray::TypeBool)
			pRow->GetItem (COL_VALUE)->AllowEdit(FALSE);
		else
			pRow->GetItem (COL_VALUE)->AllowEdit(TRUE);

		m_arrayGrid.AddRow (pRow, FALSE);
	}

	m_arrayGrid.AdjustLayout ();

	return TRUE;
}

void CDlgParamEditArray::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CCxBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	OnChangeEdit();
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CDlgParamEditArray::OnEnChangeEdit2()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CCxBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	OnChangeEdit();
}


void CDlgParamEditArray::OnEnChangeEdit3()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CCxBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	OnChangeEdit();
}

void CDlgParamEditArray::OnChangeEdit()
{
	if(!m_bInit) return;

	std::stringstream ss;
	int nDim1 = 0, nDim2 = 0, nDim3 = 0;

	CString str;

    GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	ss << str;
	ss >> nDim1;
	ss.str("");
	ss.clear();
	if (str.GetLength()>5 && nDim1 >m_maxNum)
	{
		 MessageBox( "���鳤�ȳ������ƣ�", g_lpszAppTitle, MB_ICONINFORMATION);
		 str = m_StrEdit1Last;
		 GetDlgItem(IDC_EDIT1)->SetWindowText(m_StrEdit1Last);
	}
	m_StrEdit1Last =  str;

	GetDlgItem(IDC_EDIT2)->GetWindowText(str);
	ss << str;
	ss >> nDim2;
	ss.str("");
	ss.clear();
	if (str.GetLength()>5 && nDim2 >m_maxNum)
	{
		MessageBox( "���鳤�ȳ������ƣ�", g_lpszAppTitle, MB_ICONINFORMATION);
		str = m_StrEdit2Last;
		GetDlgItem(IDC_EDIT2)->SetWindowText(m_StrEdit2Last);
	}
	m_StrEdit2Last =  str;

	GetDlgItem(IDC_EDIT3)->GetWindowText(str);
	ss << str;
	ss >> nDim3;
	ss.str("");
	ss.clear();
	if (str.GetLength()>5 && nDim3 >m_maxNum)
	{
		MessageBox( "���鳤�ȳ������ƣ�", g_lpszAppTitle, MB_ICONINFORMATION);
		str = m_StrEdit3Last;
		GetDlgItem(IDC_EDIT3)->SetWindowText(m_StrEdit3Last);
	}
	m_StrEdit3Last =  str;

	if(nDim1 * nDim2 * nDim3 > m_maxNum)
	{
	}

 	m_vArray.clear();
 	m_vDim.clear();

	if(nDim1 > 0)
	{
		m_vDim.push_back(nDim1);
	}

	if(nDim2 > 0)
	{
		m_vDim.push_back(nDim2);
	}

	if(nDim3 > 0)
	{
		m_vDim.push_back(nDim3);
	}

	LoadArrayList();

	SetEdited(TRUE);
}