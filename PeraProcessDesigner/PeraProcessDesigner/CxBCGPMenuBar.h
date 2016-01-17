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
		IMG_NEW_PROCESS,
		IMG_OPEN_PROCESS,
		IMG_SAVE,
		IMG_DATALINKER,
		//------------
		IMG_RUN_PROCESS,
		//------------
		IMG_JOBMGR,
		IMG_PREVIEW,
		//---------------
		//IMG_ABOUT,
		IMG_HELP,
		IMG_COMPONENT,
		IMG_SAVEAS,
		IMG_CLOSE,
		IMG_INFO,
		IMG_PUBLISH,
		IMG_EXIT
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


