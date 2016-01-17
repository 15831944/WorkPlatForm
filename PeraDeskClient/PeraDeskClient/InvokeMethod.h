#pragma once
#include "StlHeader.h"

extern BOOL CanInvokeMethod( void );

extern BOOL InvokeMethod( string& sResult
	, LPCTSTR lpszMethodName, LPCTSTR lpszParam );