#pragma once
#include "DlgParamEdit.h"

//�����¼�������
class ParamEventListener
{
public:
	// ��Ӳ���
	virtual void AddParam(LPCTSTR str)
	{
	}

	// ɾ������
	virtual void DeleteParam(LPCTSTR str)
	{
	}

	// ����������
	virtual void RenameParam(LPCTSTR strOld, LPCTSTR strNew)
	{
	}

	// �������Ըı�
	virtual void ParamChanged(ZTreeParam* pParam)
	{
	}

};

