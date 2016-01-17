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
// BCGPChartLegend.cpp : implementation file
//

#include "stdafx.h"
#include "BCGPChartLegend.h"
#include "BCGPChartSeries.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CBCGPChartLegendCell, CObject)
IMPLEMENT_DYNCREATE(CBCGPChartLegendEntry, CObject)
IMPLEMENT_DYNCREATE(CBCGPChartLegend, CObject)

//****************************************************************************************
// Legend Cell
//****************************************************************************************
CBCGPChartLegendCell::CBCGPChartLegendCell()
{
	m_pSeries = NULL;
	m_nDataPointIndex = -1;
	m_bIgnoreSeriesFormat = FALSE;
	m_cellType = CBCGPChartLegendCell::LCT_CUSTOM;
	m_pCustomData = NULL;
	m_nID = -1;
}
//****************************************************************************************
CBCGPChartLegendCell::CBCGPChartLegendCell(CBCGPChartSeries* pSeries, int	nDataPointIndex, 
					 CBCGPChartLegendCell::LegendCellType type)
{
	m_pSeries = pSeries;
	m_nDataPointIndex = nDataPointIndex;
	m_cellType = type;
	m_bIgnoreSeriesFormat = FALSE;
	m_pCustomData = NULL;
	m_nID = -1;
}
//****************************************************************************************
CBCGPChartLegendCell::CBCGPChartLegendCell(const CString& strContent, int nID, CObject* pCustomData)
{
	m_pSeries = NULL;
	m_nDataPointIndex = -1;
	m_bIgnoreSeriesFormat = FALSE;
	m_cellType = CBCGPChartLegendCell::LCT_CUSTOM;
	m_strCustomContent = strContent;
	m_pCustomData = pCustomData;
	m_nID = nID;
}
//****************************************************************************************
CBCGPChartLegendCell::~CBCGPChartLegendCell()
{

}
//****************************************************************************************
void CBCGPChartLegendCell::PropogateLegendID(int nID)
{
	ASSERT_VALID(this);

	if (m_pSeries != NULL)
	{
		m_pSeries->m_nLegendID = nID;
		if (m_nDataPointIndex != -1)
		{
			CBCGPChartDataPoint* pDp = (CBCGPChartDataPoint*) m_pSeries->GetDataPointAt(m_nDataPointIndex);
			if (pDp != NULL)
			{
				pDp->m_nLegendID = nID;
			}
		}
	}
}
//****************************************************************************************
// LEGEND ENTRY
//****************************************************************************************
CBCGPChartLegendEntry::~CBCGPChartLegendEntry()
{
	while (!m_lstLegendCells.IsEmpty())
	{
		delete m_lstLegendCells.RemoveTail();
	}
}
//****************************************************************************************
void CBCGPChartLegendEntry::PropogateLegendID(int nID)
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstLegendCells.GetHeadPosition(); pos != NULL;)
	{
		CBCGPChartLegendCell* pCell = DYNAMIC_DOWNCAST(CBCGPChartLegendCell, m_lstLegendCells.GetNext(pos));

		if (pCell != NULL)
		{
			ASSERT_VALID(pCell);
			pCell->PropogateLegendID(nID);
		}
	}
}
//****************************************************************************************
void CBCGPChartLegendEntry::AddLegendCell(CBCGPChartLegendCell* pCell)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pCell);
	m_lstLegendCells.AddTail(pCell);
}
//****************************************************************************************
BOOL CBCGPChartLegendEntry::ContainsCell(CBCGPChartSeries* pSeries, int nDataPointIndex) const
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstLegendCells.GetHeadPosition(); pos != NULL;)
	{
		CBCGPChartLegendCell* pCell = DYNAMIC_DOWNCAST(CBCGPChartLegendCell, m_lstLegendCells.GetNext(pos));
		ASSERT_VALID(pCell);

		if (pCell->ContainsSeries(pSeries, nDataPointIndex))
		{
			return TRUE;
		}
	}

	return FALSE;
}
//****************************************************************************************
BOOL CBCGPChartLegendEntry::ContainsCell(int nID) const
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstLegendCells.GetHeadPosition(); pos != NULL;)
	{
		CBCGPChartLegendCell* pCell = DYNAMIC_DOWNCAST(CBCGPChartLegendCell, m_lstLegendCells.GetNext(pos));
		ASSERT_VALID(pCell);

		if (pCell->GetCellID() == nID)
		{
			return TRUE;
		}
	}

	return FALSE;
}
//****************************************************************************************
// CHART LEGEND
//****************************************************************************************
CBCGPChartLegend::CBCGPChartLegend()
{
	m_nLegendID = 0;
	m_szLegendSize.SetSizeEmpty();
	m_pWndHost = NULL;
	m_nLegendToLegendDistance = 20;
	m_pLegendCellRTC = NULL;
}
//****************************************************************************************
CBCGPChartLegend::CBCGPChartLegend(int nLegendID, CWnd* pWndHost)
{
	m_nLegendID = nLegendID;
	m_szLegendSize.SetSizeEmpty();
	m_pWndHost = pWndHost;
	m_nLegendToLegendDistance = 20;
	m_pLegendCellRTC = NULL;
}
//****************************************************************************************
CBCGPChartLegend::~CBCGPChartLegend()
{
	void RemoveAllEntries();
}
//****************************************************************************************
CBCGPSize CBCGPChartLegend::CalcSize()
{
	ASSERT_VALID(this);
	CBCGPSize sz(0, 0);

	return sz;
}
//****************************************************************************************
void CBCGPChartLegend::OnDraw(CBCGPGraphicsManager* /*pGM*/)
{
	ASSERT_VALID(this);
}
//****************************************************************************************
void CBCGPChartLegend::OnDrawLegendEntry(CBCGPGraphicsManager* /*pGM*/)
{
	ASSERT_VALID(this);
}
//****************************************************************************************
void CBCGPChartLegend::AddLegendEntry(CBCGPChartLegendEntry* pEntry)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pEntry);

	m_lstLegendEntries.AddTail(pEntry);
}
//****************************************************************************************
CBCGPChartLegendEntry* CBCGPChartLegend::AddLegendEntry(CBCGPChartSeries* pSeries, int nDataPointIndex)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pSeries);

	CBCGPChartLegendCell* pCellKey = OnCreateLegendCell(pSeries, nDataPointIndex, CBCGPChartLegendCell::LCT_KEY);
	CBCGPChartLegendCell* pCellLabel = OnCreateLegendCell(pSeries, nDataPointIndex, CBCGPChartLegendCell::LCT_LABEL);

	ASSERT_VALID(pCellKey);
	ASSERT_VALID(pCellLabel);

	CBCGPChartLegendEntry* pEntry = OnCreateLegendEntry();
	ASSERT_VALID(pEntry);
	pEntry->AddLegendCell(pCellKey);
	pEntry->AddLegendCell(pCellLabel);
	m_lstLegendEntries.AddTail(pEntry);

	return pEntry;
}
//****************************************************************************************
CBCGPChartLegendEntry* CBCGPChartLegend::FindLegendEntry(CBCGPChartSeries* pSeries, int nDataPointIndex) const
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstLegendEntries.GetHeadPosition(); pos != NULL;)
	{
		CBCGPChartLegendEntry* pEntry = DYNAMIC_DOWNCAST(CBCGPChartLegendEntry, m_lstLegendEntries.GetNext(pos));
		ASSERT_VALID(pEntry);

		if (pEntry->ContainsCell(pSeries, nDataPointIndex))
		{
			return pEntry;
		}
	}

	return NULL;
}
//****************************************************************************************
CBCGPChartLegendEntry* CBCGPChartLegend::FindLegendEntry(int nCellID) const
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstLegendEntries.GetHeadPosition(); pos != NULL;)
	{
		CBCGPChartLegendEntry* pEntry = DYNAMIC_DOWNCAST(CBCGPChartLegendEntry, m_lstLegendEntries.GetNext(pos));
		ASSERT_VALID(pEntry);

		if (pEntry->ContainsCell(nCellID))
		{
			return pEntry;
		}
	}

	return NULL;
}
//****************************************************************************************
BOOL CBCGPChartLegend::RemoveLegendEntry(CBCGPChartSeries* pSeries, int nDataPointIndex)
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstLegendEntries.GetHeadPosition(); pos != NULL;)
	{
		POSITION posCurr = pos;
		CBCGPChartLegendEntry* pEntry = DYNAMIC_DOWNCAST(CBCGPChartLegendEntry, m_lstLegendEntries.GetNext(pos));

		if (pEntry->ContainsCell(pSeries, nDataPointIndex))
		{
			m_lstLegendEntries.RemoveAt(posCurr);
			delete pEntry;
			return TRUE;
		}
	}

	return FALSE;
}
//****************************************************************************************
void CBCGPChartLegend::RemoveAllEntries()
{
	ASSERT_VALID(this);

	while (!m_lstLegendEntries.IsEmpty())
	{
		delete m_lstLegendEntries.RemoveTail();
	}
}
//****************************************************************************************
void CBCGPChartLegend::SetLegendID(int nID)
{
	ASSERT_VALID(this);

	if (m_nLegendID != nID)
	{
		for (POSITION pos = m_lstLegendEntries.GetHeadPosition(); pos != NULL;)
		{
			CBCGPChartLegendEntry* pEntry = DYNAMIC_DOWNCAST(CBCGPChartLegendEntry, m_lstLegendEntries.GetNext(pos));
			pEntry->PropogateLegendID(nID);
		}
	}
}
//****************************************************************************************
void CBCGPChartLegend::SetLegendCellRTC(CRuntimeClass* pRTC)
{
	ASSERT_VALID(this);

	if (pRTC != NULL && !pRTC->IsDerivedFrom(RUNTIME_CLASS(CBCGPChartLegendCell)))
	{
		ASSERT(FALSE);
		TRACE0("CBCGPChartLegend::SetLegendCellRTC: Invalid runtine class specified for CBCGPChartLegendCell.");
		return;
	}

	m_pLegendCellRTC = pRTC;
}
//****************************************************************************************
CBCGPChartLegendCell* CBCGPChartLegend::OnCreateLegendCell(CBCGPChartSeries* pSeries, int nDataPointIndex, 
												 CBCGPChartLegendCell::LegendCellType type)
{
	ASSERT_VALID(this);

	CBCGPChartLegendCell* pLegendCell = NULL;

	if (m_pLegendCellRTC != NULL)
	{
		pLegendCell = DYNAMIC_DOWNCAST(CBCGPChartLegendCell, m_pLegendCellRTC->CreateObject());

		ASSERT_VALID(pLegendCell);

		pLegendCell->m_pSeries = pSeries;
		pLegendCell->m_nDataPointIndex = nDataPointIndex;
		pLegendCell->m_cellType = type;
	}
	else
	{
		pLegendCell = new CBCGPChartLegendCell(pSeries, nDataPointIndex, type);
	}

	return pLegendCell;
}
//****************************************************************************************
CBCGPChartLegendEntry* CBCGPChartLegend::OnCreateLegendEntry()
{
	ASSERT_VALID(this);
	return new CBCGPChartLegendEntry();
}
