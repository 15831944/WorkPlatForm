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
// BCGPVisualCollector.h: interface for the CBCGPRibbonCollector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPCONTAINERCOLLECTOR_H__304B8633_B530_459B_B1ED_BA17DF48C6B2__INCLUDED_)
#define AFX_BCGPCONTAINERCOLLECTOR_H__304B8633_B530_459B_B1ED_BA17DF48C6B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGPVisualInfo.h"

class CBCGPVisualContainer;
class CBCGPGaugeImpl;

class CBCGPVisualCollector  
{
public:
	CBCGPVisualCollector(CBCGPVisualInfo& info);
	virtual ~CBCGPVisualCollector();

public:
	virtual void Collect (const CBCGPVisualContainer& container);

protected:
	virtual CBCGPVisualInfo::XElement* CollectElement(const CBCGPBaseVisualObject& element);

	virtual CBCGPVisualInfo::XGaugeElement* CollectGaugeElement(const CBCGPGaugeImpl& element);
	virtual void CollectBaseGaugeElement (const CBCGPGaugeImpl& element, CBCGPVisualInfo::XGaugeElement& info);

	virtual void CollectContainer (const CBCGPVisualContainer& container, CBCGPVisualInfo::XContainer& info);

	CBCGPVisualInfo& GetInfo ()
	{
		return m_Info;
	}

private:
	CBCGPVisualInfo&	m_Info;
};

#endif // !defined(AFX_BCGPCONTAINERCOLLECTOR_H__304B8633_B530_459B_B1ED_BA17DF48C6B2__INCLUDED_)
