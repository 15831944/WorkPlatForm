
// ICEClientDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CICEClientDemoApp:
// �йش����ʵ�֣������ ICEClientDemo.cpp
//
#include "GDIPlusInit.h"

class CICEClientDemoApp : public CWinApp ,public CGDIPlusInit
{
public:
	CICEClientDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CICEClientDemoApp theApp;