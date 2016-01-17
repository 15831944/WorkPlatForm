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
// BCGPVisualConstructor.h: interface for the CBCGPRibbonConstrucor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPVISUALCONSTRUCTOR_H__856CE5A1_AC4D_4029_8A23_77ECDC8EEB77__INCLUDED_)
#define AFX_BCGPVISUALCONSTRUCTOR_H__856CE5A1_AC4D_4029_8A23_77ECDC8EEB77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGPVisualInfo.h"

class CBCGPVisual;
class CBCGPGaugeImpl;

class CBCGPVisualConstructor  
{
public:
	CBCGPVisualConstructor(const CBCGPVisualInfo& info);
	virtual ~CBCGPVisualConstructor();

public:
	virtual void Construct (CBCGPVisualContainer& container) const;

protected:
	virtual CBCGPBaseVisualObject* CreateElement (const CBCGPVisualInfo::XElement& info) const;

	virtual CBCGPGaugeImpl* CreateGaugeElement (const CBCGPVisualInfo::XGaugeElement& info) const;
	virtual void ConstructGaugeElement (CBCGPGaugeImpl& element, const CBCGPVisualInfo::XGaugeElement& info) const;

	const CBCGPVisualInfo& GetInfo () const
	{
		return m_Info;
	}

private:
	const CBCGPVisualInfo&	m_Info;
};

#endif // !defined(AFX_BCGPVISUALCONSTRUCTOR_H__856CE5A1_AC4D_4029_8A23_77ECDC8EEB77__INCLUDED_)
