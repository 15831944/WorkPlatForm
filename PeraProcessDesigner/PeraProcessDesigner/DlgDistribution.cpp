#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "DlgDistribution.h"
#include "UtilityFunc.h"
#include "ConfigFunc.h"


CDlgDistribution::CDlgDistribution(void)
{
}


CDlgDistribution::~CDlgDistribution(void)
{
}

CString CDlgDistribution::LoadUrl( void )
{
	CString strUrl;
	strUrl = GetConfig("PeraProcessDesigner", "DrmConfigManager");
	if (
		strUrl.IsEmpty()
		|| theApp.m_LoginData.m_strTicket.IsEmpty()
		)
	{
		this->Navigate("about:blank");
		return "";
	}

	AttachTicket(strUrl, theApp.m_LoginData.m_strTicket);
	this->Navigate((LPCTSTR)strUrl);
	return strUrl;
}
