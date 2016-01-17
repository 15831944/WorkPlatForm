#pragma once
#include "ximage.h"


// CZStatic

class CZStatic : public CStatic
{
	DECLARE_DYNAMIC(CZStatic)

public:
	CZStatic();
	virtual ~CZStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	CxImage m_cxImage; //ืสิด
};


