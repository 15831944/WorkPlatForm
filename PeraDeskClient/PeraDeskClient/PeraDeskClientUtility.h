#pragma once
//#include "DlgSvg.h"
//#include "DlgHistoryMonitor.h"
//#include "CxStructTreeGridT.h"
//#include "DlgDistribution.h"

//extern CDlgSvg * GetSvgDilogPtr (void);
//extern CDlgHistoryMonitor * GetHistoryMonitorDilogPtr (void);
//extern CDlgDistribution * GetDistributionDilogPtr (void);
//extern CString GetSvgDefaultPage( void );
//extern BOOL GetStructTreeCurSel( CCxStructTreeRowT ** ppSelRow, CCxStructTreeNode ** ppSelNode );
extern void SetMouseClick( CPoint ptDest, HWND hwndDest
	, CPoint ptSrc = CPoint(-1, -1), HWND hwndSrc = NULL );
//extern CString ConvertRunFlowStatus( LPCTSTR lpszCode );
//将double值统一转化为精度为2的浮点型数据。
//extern CString ConvertDoubleValue( LPCTSTR lpszSrc );
//extern BOOL ConvertArrayDim( vector<int>& vPos, LPCTSTR lpszDimStr );

extern HWND FindPeraDeskClientMainWindow( void );
extern HWND FindPeraDeskClientLonginWindow( void );
