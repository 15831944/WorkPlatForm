#include "StdAfx.h"
#include "GlobalConfig.h"
#include "UtilityFunc.h"


CGlobalConfig::CGlobalConfig(void)
{
}


CGlobalConfig::~CGlobalConfig(void)
{
}

void CGlobalConfig::Load( void )
{
	m_strSvgUrl = GetExecDir() + "\\modelEditor\\html\\modelEditor.html";
	//GetChannelUrl(m_strSvgUrl, g_lpszViewTitleModel);
}

CGlobalConfig theGlobalConfig;