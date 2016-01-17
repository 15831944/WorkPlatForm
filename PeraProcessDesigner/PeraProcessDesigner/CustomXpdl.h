#pragma once
#include "Xpdl\Xpdl.h"

class CCustomXpdl :
	public CXpdl
{
public:
	CCustomXpdl(void);
	~CCustomXpdl(void);
	
	BOOL UploadFileAndReplacePath();
};

