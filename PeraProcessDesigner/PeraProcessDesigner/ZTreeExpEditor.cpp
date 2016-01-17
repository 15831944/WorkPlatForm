#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "ZTreeExpEditor.h"
#include "DlgExpEditor.h"
#include "CxStructTreeNodeParam.h"


ZTreeExpEditor::ZTreeExpEditor(void)
{
}


ZTreeExpEditor::~ZTreeExpEditor(void)
{
}
BEGIN_MESSAGE_MAP(ZTreeExpEditor, CZGridCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


void ZTreeExpEditor::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//CBCGPGridCtrl::OnLButtonDblClk(nFlags, point);


	CBCGPGridRow * pRow = GetCurSel();
	if ( pRow != NULL )
	{
		CCxStructTreeNode * pData = reinterpret_cast< CCxStructTreeNode * > ( pRow->GetData() );
		if ( pData != NULL )
		{
			switch ( pData->m_NodeType )
			{
			case CCxStructTreeNode::NT_COMPONENT:
				return;
			case CCxStructTreeNode::NT_ROOT:
				return;
			case CCxStructTreeNode::NT_FOR:
				return;
			case CCxStructTreeNode::NT_IF:
				return;
			case CCxStructTreeNode::NT_PARALLEL:
				return;
			case CCxStructTreeNode::NT_SEQUENCE:
				return;
			case CCxStructTreeNode::NT_SUBFLOW:
				return;
			case CCxStructTreeNode::NT_PARAM:
			case CCxStructTreeNode::NT_PARAM_ARRAY:
				{
					{//�Ƿ�̬����
						CCxStructTreeNode * p;
						if(!pData->m_vChildren.empty()) return;//����ײ�ڵ���Ч
						for(p = pData; p->m_NodeType != CCxStructTreeNode::NT_PARAM; p = p->m_pParent);
						if(((CCxStructTreeNodeParam*)p)->m_bIsDynamicArray || ((CCxStructTreeNodeParam*)p)->IsFile()) return;//���ڶ�̬������ļ���������Ч
					}
					CString strName;
					CString strComponentName;
					{//������ȷ��				
						if(pData->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY)
						{//�������
							//							if(!pData->m_vChildren.empty()) return; //����ײ�ڵ�����Ч
							strName = ((CCxStructTreeNodeParam*)pData)->m_strText;
						}
						else
						{
							strName = pData->m_strOriName;
						}
					}
					{//�����ȷ��
						CCxStructTreeNode * p;
						for(p = pData; p->m_NodeType == CCxStructTreeNode::NT_PARAM || p->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY; p = p->m_pParent);
						strComponentName = p->m_strOriName;
					}

					CDlgExpEditor& dlgExpEditor = *(CDlgExpEditor*)GetParent();

					CEdit& edit = dlgExpEditor.m_editExp;

					int& nPos = dlgExpEditor.m_nEditPosStart; //���
					int& nPosEnd = dlgExpEditor.m_nEditPosEnd; //���

					CString strAdd = strComponentName + '.' + strName; //Ҫ��ӵ�
					CString strEdit;  //ԭʼ��
					edit.GetWindowText(strEdit);

					CStringW wstrEdit(strEdit);

					int nLen = wstrEdit.GetLength();

					CString strLeft(wstrEdit.Left(nPos));
					CString strRight(wstrEdit.Right(wstrEdit.GetLength() - nPosEnd));

					//if(nPos > 0) strAdd = " " + strAdd;
					//if(nPos < nLen) strAdd += " ";
					CString str = strLeft + strAdd + strRight;	

					edit.SetSel(nPos, nPosEnd);
					edit.ReplaceSel(strAdd);
					//edit.SetWindowText(str);

					CStringW wstrAdd(strAdd);
					nPos += wstrAdd.GetLength();
					nPosEnd = nPos; //�������

					edit.SetFocus();
					return;
				}				
			default:
				break;
			}
		}
	}

}
