#pragma once
#include "DlgParamEdit.h"

//参数事件监听器
class ParamEventListener
{
public:
	// 添加参数
	virtual void AddParam(LPCTSTR str)
	{
	}

	// 删除参数
	virtual void DeleteParam(LPCTSTR str)
	{
	}

	// 重命名参数
	virtual void RenameParam(LPCTSTR strOld, LPCTSTR strNew)
	{
	}

	// 参数属性改变
	virtual void ParamChanged(ZTreeParam* pParam)
	{
	}

};

