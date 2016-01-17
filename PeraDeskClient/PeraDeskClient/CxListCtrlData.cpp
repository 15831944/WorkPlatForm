#include "StdAfx.h"
#include "CxListCtrlData.h"

_COL_DATA::_COL_DATA()
{
	nId = -1;
	crBg = Color::White;
	crText = Color::Black;
	nType = TYPE_TEXT;
	nSortId = -1;
	bFloat = FALSE;
	bShow = TRUE;
	nWidth = 64;
	nFormat = LVCFMT_LEFT;
}

_COL_DATA::~_COL_DATA()
{

}

void _COL_DATA::Clear( void )
{
	nId = -1;
	strText = "";
	crBg = Color::White;
	crText = Color::Black;
	nType = TYPE_TEXT;
	nSortId = -1;
	bFloat = FALSE;
	bShow = TRUE;
	nWidth = 64;
	nFormat = LVCFMT_LEFT;
}
