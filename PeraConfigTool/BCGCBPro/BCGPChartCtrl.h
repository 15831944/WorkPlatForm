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
//
// BCGPChartCtrl.h: declaration of the CBCGPChartCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPCHARTCTRL_H__8E3DB7C8_BD56_4654_89A6_3F0B67AD7B4B__INCLUDED_)
#define AFX_BCGPCHARTCTRL_H__8E3DB7C8_BD56_4654_89A6_3F0B67AD7B4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"
#include "BCGPChartVisualObject.h"
#include "BCGPVisualCtrl.h"

// CBCGPChartCtrl

class CBCGPChartVisualObject;

class BCGCBPRODLLEXPORT CBCGPChartCtrl : public CBCGPVisualCtrl
{
	DECLARE_DYNAMIC(CBCGPChartCtrl)

public:
	CBCGPChartCtrl();
	virtual ~CBCGPChartCtrl();

	BOOL CreateCustomChart(CRuntimeClass* pChartRTC);

	virtual CBCGPChartVisualObject*	GetChart()
	{
		if (m_pChart == NULL)
		{
			m_pChart = new CBCGPChartVisualObject();
		}

		return m_pChart;
	}

protected:
	//{{AFX_MSG(CBCGPChartVisualContainer)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual CBCGPBaseVisualObject* GetVisualObject()
	{
		return GetChart();
	}

protected:
	CBCGPChartVisualObject*	m_pChart;
};

#endif // !defined(AFX_BCGPCHARTCTRL_H__8E3DB7C8_BD56_4654_89A6_3F0B67AD7B4B__INCLUDED_)
