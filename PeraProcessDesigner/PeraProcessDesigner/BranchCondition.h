#pragma once
#include <afxwin.h>
#include <ZEdit.h>
#include "ZComboBox.h"
#include "ZButton.h"
#include "ZStatic.h"
#include "EditBranch.h"
#include "MethodForDataMapIfExpress.h"

struct ZRectF
{
	ZRectF::ZRectF(
		float l,
		float t,
		float r,
		float b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	float  left;
	float top;
	float right;
	float bottom;
};

class BranchCondition
{
public:
	enum TYPE
	{
		TYPE_START,    //��ͷ  if
		TYPE_MIDDLE, //�м�  else if
		TYPE_END        //��β  else������
	};

	enum MODE
	{
		MODE_EXCLUSIVE,    //��һ
		MODE_INCLUSIVE,    //����
	};

	BranchCondition(void);
	virtual ~BranchCondition(void);

	BOOL Create(CRect rect, CWnd* wndParent = NULL, const char* strIdExp = NULL, const char* strbranch = NULL, TYPE type = TYPE_MIDDLE, MODE mode = MODE_EXCLUSIVE);
	void Resize(int nOffsetX, int nOffsetY);
	void Move(CWnd& wnd, int nOffsetX, int nOffsetY);
	void SetPos( CWnd& wnd, int nX, int nY );
	void SetPos(int nX, int nY );
	void SetPosY( CWnd& wnd, int nY );
	void SetPosY(int nY );
	void SetMode( MODE mode );
	void Swap(BranchCondition* pBC);

	static const ZRectF m_conRectCondition; //λ�ðٷֱ�
	static const ZRectF m_conRectConditionList; //λ�ðٷֱ�
	static const ZRectF m_conRectExp; //λ�ðٷֱ�
	static const ZRectF m_conRectBtnEditor; //��ť
	static const ZRectF m_conRectBranch; //λ�ðٷֱ�
	static const ZRectF m_conRectArrow; //λ�ðٷֱ�
	static const char* const m_str[MODE_INCLUSIVE + 1][TYPE_END + 1];
	static const char* const m_strCBSecond;

	CBCGPStatic m_staCondition;  //if	
	CZComboBox m_listCondition;  //if	
	CZEdit m_editExp;        //���ʽ
	CZButton m_btnEditor; //�༭��ť
	CEditBranch  m_editBranch;  //��֧1
	CZStatic m_staArrow;    //��ͷ
	

	CRect m_rect;    //λ��
	int m_nSizeIncX; //����X
	int m_nSIzeIncY; //����Y

	TYPE m_type;          //��֧����
	BranchExpress    m_BranchExpress;
};
