// CxBCGPToolbarButton.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraDeskClient.h"
#include "CxBCGPToolbarButton.h"


// CCxBCGPToolbarButton

IMPLEMENT_SERIAL(CCxBCGPToolbarButton, CBCGPToolbarButton, VERSIONABLE_SCHEMA | 1)

CCxBCGPToolbarButton::CCxBCGPToolbarButton()
{
}

CCxBCGPToolbarButton::~CCxBCGPToolbarButton()
{
}

CCxBCGPToolbarButton::CCxBCGPToolbarButton(UINT uiID, int iImage
	, LPCTSTR lpszText, BOOL bUserButton,
	BOOL bLocked)
	: CBCGPToolbarButton(uiID, iImage, lpszText, bUserButton, bLocked)
{

}
