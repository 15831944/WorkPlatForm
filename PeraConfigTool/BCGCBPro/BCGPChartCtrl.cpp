//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
// BCGPChartCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPChartCtrl.h"

// CBCGPChartCtrl

IMPLEMENT_DYNAMIC(CBCGPChartCtrl, CBCGPVisualCtrl)

CBCGPChartCtrl::CBCGPChartCtrl()
{
	m_pChart = NULL;
}

CBCGPChartCtrl::~CBCGPChartCtrl()
{
	if (m_pChart != NULL)
	{
		delete m_pChart;
	}
}

BEGIN_MESSAGE_MAP(CBCGPChartCtrl, CBCGPVisualCtrl)
	//{{AFX_MSG_MAP(CBCGPChartCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBCGPChartCtrl::CreateCustomChart(CRuntimeClass* pChartRTC)
{
	if (pChartRTC != NULL && !pChartRTC->IsDerivedFrom(RUNTIME_CLASS(CBCGPChartVisualObject)))
	{
		ASSERT(FALSE);
		TRACE0("CBCGPChartCtrl::CreateCustomChart: custom chart container must be derived from CBCGPChartVisualContainer.");
		return FALSE;
	}

	if (m_pChart != NULL)
	{
		delete m_pChart;
	}

	if (pChartRTC != NULL)
	{
		m_pChart = DYNAMIC_DOWNCAST(CBCGPChartVisualObject, pChartRTC->CreateObject());
	}
	else
	{
		m_pChart = new CBCGPChartVisualObject();	
	}	

	ASSERT_VALID(m_pChart);
	return TRUE;
}
