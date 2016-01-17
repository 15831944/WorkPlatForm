// CxBCGPMenuBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CxBCGPMenuBar.h"
#include "PtrVectorHeader.h"
#include "UserDefine.h"

LPCTSTR CCxBCGPMenuBar::m_strHotKey = "N"; //菜单热键集合
// CCxBCGPMenuBar

BEGIN_MESSAGE_MAP(CCxBCGPMenuBar, CBCGPMenuBar)
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CCxBCGPMenuBar, CBCGPMenuBar, VERSIONABLE_SCHEMA | 1)

CCxBCGPMenuBar::CCxBCGPMenuBar()
{
}

CCxBCGPMenuBar::~CCxBCGPMenuBar()
{
	ClearData();
}


void CCxBCGPMenuBar::ClearData()
{
	m_BCGToolbarImages.Clear();
	for (int i=0; i<(int)m_vHBitmaps.size(); i++)
	{
		DeleteObject ( m_vHBitmaps[i] );
	}
	m_vHBitmaps.clear();
	PtrVectorClear( m_vImages );
}

void CCxBCGPMenuBar::LoadData()
{
	// 	static const LPCTSTR arrImageName[] = {
	// 		"NewProcess.png", "OpenProcess.png", "RunProcess.png",
	// 		"Preview.png", "Save.png", "DataLinker.png", "Monitor.png", 
	// 		"Help.png", "About.png" 
	// 	};

	static const LPCTSTR arrImageName[] = {
		"NewProcess.png", "OpenProcess.png", "Save.png", "DataLinker.png", 
		"RunProcess.png",
		"Monitor.png", "Preview.png", 
		//"About.png" ,
		"Help.png" ,
		"OpenComponent.png",
		"SaveAs.png",
		"Close.png",
		"Info.png",
		"Publish.png",
		"Exit.png"
	};

	CString strDir = GetExecDir() + "\\Res\\MainToolbar\\";
	Bitmap * pBitmap = NULL;
	HBITMAP hbmp = NULL;

	//ResetAll();

	ClearData();

	m_BCGToolbarImages.SetImageSize( CSize(16, 16) );

	//Color crImgBg;
	//crImgBg.SetFromCOLORREF(g_crToolbarBg);
	//Color crImgBg = Color::White;
	//m_BCGToolbarImages.SetTransparentColor (g_crToolbarBg);

	int nCount = sizeof( arrImageName ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pBitmap = GetNewBitmap( strDir + arrImageName[i] );
		if ( pBitmap == NULL )
		{
			CString strLog;
			strLog.Format( "图片资源丢失，%s", arrImageName[i] );
			MessageBox( strLog, g_lpszAppTitle );

			ClearData();
			return;
		}
		m_vImages.push_back( pBitmap );
		//Color color(255,255,255,255);
		pBitmap->GetHBITMAP( NULL, &hbmp);
		if ( hbmp == NULL )
		{
			ClearData();
			return;
		}
		m_vHBitmaps.push_back( hbmp );
		m_BCGToolbarImages.AddImage( hbmp );
	}

	if ( m_BCGToolbarImages.GetCount() == 0 ) return;

	//SetSizes( CSize(32, 30), CSize(16, 16) );
	//SetMenuSizes( CSize(32, 22), CSize(13, 13));
	SetMenuSizes( CSize(32, 28), CSize(16, 16));
	//SetUserImages( &m_BCGToolbarImages );

	int nImgPos = m_Images.GetCount() > 0 ? m_Images.GetCount() : 0;

	for (int i=0; i<m_BCGToolbarImages.GetCount(); i++)
	{
		m_Images.AddImage( m_vHBitmaps[i] );
		m_ImagesLocked.AddImage( m_vHBitmaps[i] );
		m_LargeImages.AddImage( m_vHBitmaps[i] );
		m_LargeImagesLocked.AddImage( m_vHBitmaps[i] );
	}

	m_BCGToolbarImages.ConvertTo32Bits();
	m_Images.ConvertTo32Bits();
	m_ImagesLocked.ConvertTo32Bits();
	m_LargeImages.ConvertTo32Bits();
	m_LargeImagesLocked.ConvertTo32Bits();

	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_NEW, 1, nImgPos + IMG_NEW_PROCESS);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_OPEN, 1, nImgPos + IMG_OPEN_PROCESS);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_SAVE, 1, nImgPos + IMG_SAVE);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_RUN, 1, nImgPos + IMG_RUN_PROCESS);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TOOL_JOBMGR, 1, nImgPos + IMG_JOBMGR);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_DISTRIBUTION, 1, nImgPos + IMG_PREVIEW);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_HELP, 1, nImgPos + IMG_HELP);
	//BCGPCMD_MGR.EnableMenuItemImage(IDM_HELP_ABOUT, 1, nImgPos + IMG_ABOUT);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TOOL_FLEXWARE, 1, nImgPos + IMG_COMPONENT);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_SAVEAS, 1, nImgPos + IMG_SAVEAS);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_INFO, 1, nImgPos + IMG_INFO);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_CLOSEALL, 1, nImgPos + IMG_CLOSE);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_PUBLISH, 1, nImgPos + IMG_PUBLISH);
	BCGPCMD_MGR.EnableMenuItemImage(IDM_TASK_EXIT, 1, nImgPos + IMG_EXIT);


}


