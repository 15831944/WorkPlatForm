//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
// 
// BCGPChartLegend.h: declaration of the CBCGPChartLegend and related classes.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_BCGPCHARTLEGEND_H__1875ABCF_5225_490C_BBD3_9B6CFB20ADC0__INCLUDED_)
#define AFX_BCGPCHARTLEGEND_H__1875ABCF_5225_490C_BBD3_9B6CFB20ADC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"
#include "BCGPChartFormat.h"

class CBCGPChartSeries;

//****************************************************************************************
// Legend Cell
//****************************************************************************************

class BCGCBPRODLLEXPORT CBCGPChartLegendCell : public CObject
{
	friend class CBCGPChartLegend;
	DECLARE_DYNCREATE(CBCGPChartLegendCell)
public:
	enum LegendCellType
	{
		LCT_KEY,
		LCT_LABEL,
		LCT_CUSTOM
	};

	CBCGPChartLegendCell();
	CBCGPChartLegendCell(CBCGPChartSeries* pSeries, int	nDataPointIndex, 
		LegendCellType type);

	CBCGPChartLegendCell(const CString& strContent, int nID, CObject* pCustomData = NULL);

	virtual ~CBCGPChartLegendCell();

	void PropogateLegendID(int nID);

	BOOL ContainsSeries(CBCGPChartSeries* pSeries, int nDataPointIndex) const
	{
		return (m_pSeries == pSeries && m_nDataPointIndex == nDataPointIndex);
	}

	int GetCellID() const {return m_nID;}

public:	
	BCGPChartFormatLabel		m_entryStyle;

	// ignore series format and use m_entryStyle to draw label
	// uses m_entryStyle for custom content always  
	// also uses m_entryStyle for content padding
	BOOL					m_bIgnoreSeriesFormat; 

protected:
	LegendCellType			m_cellType;
	CBCGPChartSeries*		m_pSeries;
	int						m_nDataPointIndex;

	CBCGPSize				m_szContentSize;
	CString					m_strCustomContent;
	CObject*				m_pCustomData;

	int						m_nID;
};

//****************************************************************************************
// Legend Entry
//****************************************************************************************

class BCGCBPRODLLEXPORT CBCGPChartLegendEntry : public CObject
{
	DECLARE_DYNCREATE(CBCGPChartLegendEntry)

public:
	CBCGPChartLegendEntry()
	{

	}

	virtual ~CBCGPChartLegendEntry();

	void PropogateLegendID(int nID);
	void AddLegendCell(CBCGPChartLegendCell* pCell);

	BOOL ContainsCell(CBCGPChartSeries* pSeries, int nDataPointIndex) const;
	BOOL ContainsCell(int nID) const;

protected:
	CObList m_lstLegendCells;
};

//****************************************************************************************
// Chart Legend
//****************************************************************************************

class BCGCBPRODLLEXPORT CBCGPChartLegend : public CObject
{
	DECLARE_DYNCREATE(CBCGPChartLegend)

public:
	CBCGPChartLegend();
	CBCGPChartLegend(int nLegendID, CWnd* pWndHost);
	virtual ~CBCGPChartLegend();

	void SetHostControl(CWnd* pWndHost)
	{
		ASSERT_VALID(this);
		m_pWndHost = pWndHost;

		if (m_pWndHost != NULL)
		{
			ASSERT_VALID(m_pWndHost);
		}
	}

	virtual CBCGPSize CalcSize();

	virtual void OnDraw(CBCGPGraphicsManager* pGM);
	virtual void OnDrawLegendEntry(CBCGPGraphicsManager* pGM);

	void AddLegendEntry(CBCGPChartLegendEntry* pEntry);

	CBCGPChartLegendEntry* AddLegendEntry(CBCGPChartSeries* pSeries, int nDataPointIndex = -1);
	CBCGPChartLegendEntry* FindLegendEntry(CBCGPChartSeries* pSeries, int nDataPointIndex = -1) const;
	CBCGPChartLegendEntry* FindLegendEntry(int nCellID) const;
	BOOL RemoveLegendEntry(CBCGPChartSeries* pSeries, int nDataPointIndex = -1);
	void RemoveAllEntries();

	void SetLegendID(int nID);
	int GetLegendID() const{return m_nLegendID;}

	void SetLegendCellRTC(CRuntimeClass* pRTC);
	CRuntimeClass* GetLegendCellRTC() const {return m_pLegendCellRTC;}

public:
	CString					m_strLegendTitle;
	BCGPChartFormatLegendTable	m_legendStyle;

	// used for several legends with the same alignment
	int						m_nLegendToLegendDistance;

protected:
	CObList					m_lstLegendEntries;
	CBCGPSize				m_szLegendSize;
	CWnd*					m_pWndHost;

	CRuntimeClass*			m_pLegendCellRTC;
	int						m_nLegendID;

	virtual CBCGPChartLegendCell* OnCreateLegendCell(CBCGPChartSeries* pSeries, int nDataPointIndex, 
		CBCGPChartLegendCell::LegendCellType type);
	virtual CBCGPChartLegendEntry* OnCreateLegendEntry();
};

#endif // !defined(AFX_BCGPCHARTLEGEND_H__1875ABCF_5225_490C_BBD3_9B6CFB20ADC0__INCLUDED_)
