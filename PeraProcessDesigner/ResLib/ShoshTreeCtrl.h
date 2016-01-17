#if !defined(AFX_SHOSHTREECTRL_H__8DDD98E1_2748_4CA9_A0DF_B1772B54EACC__INCLUDED_)
#define AFX_SHOSHTREECTRL_H__8DDD98E1_2748_4CA9_A0DF_B1772B54EACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "CxImage\ximage.h"
#include "..\CxImage\ximage.h"
#include <map>

// ShoshTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShoshTreeCtrl window
class MagicDB;
class CReadRobotDB;
class CResLibProcessData;
class CResLibRobotData;
class CResLibData;



struct PublishInfo
{
	string sName;
	string sDesc;
	string sVersion;
	string sAuthor;
	string sTime;
	string sFilePath;
};

class CShoshTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CShoshTreeCtrl();

// Attributes
public:

// Operations
public:
	CImageList m_ImageList;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShoshTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShoshTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CShoshTreeCtrl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	VOID CreateShoshIL(COLORREF clrColor);	
	BOOL bStyles[5];
	list<CBitmap*> m_pBitMapList;
	list<Bitmap*> m_pBitMapList2;
//	CxImage* m_xImage;
private:
	
	//首先定义一个字体、颜色的结构体：
protected:
	struct Color_Font
	{
		COLORREF color;
		LOGFONT logfont;
	}; 
	//使用CMap来保存各个Item对应的字体、颜色信息
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont ;
	
	HTREEITEM m_hRoot;

	CxImage* m_xImage;
	int m_nFileImageNo;
	int m_nWSImageNo;
	int m_nRobotImageNo;
	int m_nPCImageNo;
	bool m_bIsMenuShow;
	private:
	int AddImageList(const char* stfilePath);
	bool DelAllImageList();
	void InitTree();
	void InsertRootTree();
	void InsertRobotTree();
	void InsertProcessTree();
	void SetItemIcons(HTREEITEM Treeitm,int imageID,int selImageID);	//设置图标
	CString GetFullPath(HTREEITEM hCurrent);	
	void AddSubDir(HTREEITEM hParent);//添加子目录
	HTREEITEM  TreeRecursion(CResLibProcessData* pProcessBase,HTREEITEM hTree);
	HTREEITEM  TreeRecursion(CResLibRobotData* pRobotBase,HTREEITEM hTree);
	bool SendRobotTreeDB(CResLibData* pResLibDataBase);
	bool SendProcessTreeDB(CResLibData* pResLibDataBase);
	void  TreeVisit(HTREEITEM hItem,map<string,HTREEITEM>& IDTreeMap);
	void DelTreeDate(HTREEITEM hSelect);

//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnNotifyDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownload(WPARAM wParam, LPARAM lParam);
	void UpdateDataSelChanged();
protected:
	afx_msg void OnTreeMenuAdd();
	afx_msg void OnTreeMenuDel();
	afx_msg void OnTreeMenuEdit();
	afx_msg void OnTreeMenuSend();
	void CleanSelchanged();
public:
	
	void Load();
	void Load(int nResType);
	void Release();
	bool RemoveDir(string sDirID);
	bool RemoveItem(string sItemID);
	bool EditRobot(CResLibData* pResLibDataItem);
	bool GetBaseComponentRoot(list<CResLibData*>& pBaseList);
	void SetMenuShow(bool bMenuShow);
	bool SendRobotTreeDB(PublishInfo ResLibDataInfo);
	bool SendProcessTreeDB(PublishInfo ResLibDataInfo);
	HTREEITEM GetTreeRoot(){return m_hRoot;}

protected:
	void Myreplace(string &str,string oldstr,string newstr);
	void SetItemFont(HTREEITEM hItem, LOGFONT& logfont);
	void SetItemBold(HTREEITEM hItem, BOOL bBold);
	void SetItemColor(HTREEITEM hItem, COLORREF color);
	BOOL GetItemFont(HTREEITEM hItem, LOGFONT * plogfont);
	BOOL GetItemBold(HTREEITEM hItem);
	COLORREF GetItemColor(HTREEITEM hItem);
public:
//	afx_msg void OnPaint();
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOSHTREECTRL_H__8DDD98E1_2748_4CA9_A0DF_B1772B54EACC__INCLUDED_)
