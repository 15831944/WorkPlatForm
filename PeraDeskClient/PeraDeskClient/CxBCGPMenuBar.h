#pragma once
#include <BCGPMenuBar.h>


// CCxBCGPMenuBar

class CCxBCGPMenuBar : public CBCGPMenuBar
{
	DECLARE_SERIAL(CCxBCGPMenuBar)

public:
	CCxBCGPMenuBar();
	virtual ~CCxBCGPMenuBar();
public:

	enum {
		//IMG_TOOLBAR_MESSAGE,
		IMG_TOOLBAR_FLEXWARE,
		IMG_TOOLBAR_PORTAL,
		IMG_TOOLBAR_SETTING,
		IMG_TOOLBAR_ABOUT
		//IMG_DATALINKER,
		////------------
		//IMG_RUN_PROCESS,
		////------------
		//IMG_HISTORY_MONITOR,
		//IMG_PREVIEW,
		////---------------
		////IMG_ABOUT,
		//IMG_HELP,
		//IMG_COMPONENT,
		//IMG_SAVEAS,
		//IMG_CLOSE,
		//IMG_INFO,
		//IMG_PUBLISH,
		//IMG_EXIT
	};

	void LoadData();
	void ClearData();

	CBCGPToolBarImages m_BCGToolbarImages;
	vector< Bitmap * > m_vImages;
	vector< HBITMAP > m_vHBitmaps;

	static LPCTSTR m_strHotKey;

protected:
	DECLARE_MESSAGE_MAP()

};


