#pragma once
#include "xpdlnode.h"

class CXpdlNodeComponent :
	public CXpdlNode
{
public:
	CXpdlNodeComponent(void);

	virtual ~CXpdlNodeComponent(void);

	char	m_cRobotType[MAX_PATH*2];

	char	m_cManifestPath[ MAX_PATH*2 ];
};

