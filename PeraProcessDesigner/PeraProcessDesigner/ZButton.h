#pragma once
#include <BCGProMigrate.h>


// CZButton

class CZButton : public CBCGButton
{
	DECLARE_DYNAMIC(CZButton)

public:
	CZButton();
	virtual ~CZButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked();

	int m_index; //Ë÷Òý

	static int m_staCount;
};


