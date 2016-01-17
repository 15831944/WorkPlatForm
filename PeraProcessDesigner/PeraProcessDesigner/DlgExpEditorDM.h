#pragma once
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "dlgexpeditor.h"
class CDlgExpEditorDM :
	public CDlgExpEditor
{
public:
	CDlgExpEditorDM(const char* strExp, CWnd* pParent = NULL);
	virtual ~CDlgExpEditorDM(void);
	virtual void OnOK();
	void ParseJson(const char* strJson);
	void ProcessUpdate( const char* str );

	//std::string m_strJsonSrc; //JSON пео╒"{"newExp : "...", oldExp : "...", "Error : "..."}
	Json::Value m_Jv;
	virtual BOOL OnInitDialog();
	int MoveItemUp(CWnd* wndItem, CWnd* wndDst, int d);

	enum EDirection
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	void MoveItem(CWnd* wndItem, EDirection e, int d);
};

