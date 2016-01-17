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
		TYPE_START,    //开头  if
		TYPE_MIDDLE, //中间  else if
		TYPE_END        //结尾  else下拉框
	};

	enum MODE
	{
		MODE_EXCLUSIVE,    //单一
		MODE_INCLUSIVE,    //并行
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

	static const ZRectF m_conRectCondition; //位置百分比
	static const ZRectF m_conRectConditionList; //位置百分比
	static const ZRectF m_conRectExp; //位置百分比
	static const ZRectF m_conRectBtnEditor; //按钮
	static const ZRectF m_conRectBranch; //位置百分比
	static const ZRectF m_conRectArrow; //位置百分比
	static const char* const m_str[MODE_INCLUSIVE + 1][TYPE_END + 1];
	static const char* const m_strCBSecond;

	CBCGPStatic m_staCondition;  //if	
	CZComboBox m_listCondition;  //if	
	CZEdit m_editExp;        //表达式
	CZButton m_btnEditor; //编辑按钮
	CEditBranch  m_editBranch;  //分支1
	CZStatic m_staArrow;    //箭头
	

	CRect m_rect;    //位置
	int m_nSizeIncX; //拉伸X
	int m_nSIzeIncY; //拉伸Y

	TYPE m_type;          //分支类型
	BranchExpress    m_BranchExpress;
};
