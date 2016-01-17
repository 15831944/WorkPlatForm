#include "StdAfx.h"
#include "XpdlNodeComponent.h"


CXpdlNodeComponent::CXpdlNodeComponent(void)
{
	ZeroMemory( m_cRobotType, MAX_PATH*2 );
	ZeroMemory( m_cManifestPath, MAX_PATH*2 );
}


CXpdlNodeComponent::~CXpdlNodeComponent(void)
{
}
