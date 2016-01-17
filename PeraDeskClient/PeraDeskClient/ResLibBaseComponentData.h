#pragma once
#include "ResLibData.h"
class CResLibBaseComponentData :
	public CResLibData
{
public:
	CResLibBaseComponentData(void);
	virtual ~CResLibBaseComponentData(void);

public:
	string m_RobotType;
	virtual string GetSvgJsonStr();
};

