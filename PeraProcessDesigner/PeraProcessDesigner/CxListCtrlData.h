#pragma once

typedef struct _COL_DATA
{
	enum
	{
		TYPE_TEXT = 1
		, TYPE_IMAGE
		, TYPE_EDIT
		, TYPE_CHECKBOX
		, TYPE_BUTTON
		, TYPE_RADIO
	};

	_COL_DATA();
	~_COL_DATA();
	void Clear(void);

	long nId;
	CString strText;
	Color crBg;
	Color crText;
	UINT nType;
	int nSortId;
	BOOL bFloat;
	BOOL bShow;
	int nWidth;
	int nFormat;
	
} COL_DATA;


