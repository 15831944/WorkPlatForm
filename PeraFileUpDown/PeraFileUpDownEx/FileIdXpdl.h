#pragma once
#include "Xpdl\Xpdl.h"

class CFileIdXpdl :
	public CXpdl
{
public:
	CFileIdXpdl(void);
	~CFileIdXpdl(void);
	
	BOOL ReplacePathToFileId(Json::Value jsonResult);
};

