#pragma once
#include "up_window.h"

class use_window : public up_window
{

public:

	use_window(void);
	~use_window(void);


public:

	HBRUSH			m_hBrushCtrl_1;
	HBRUSH			m_hBrushCtrl_2;

public:

	virtual void InitWindow();
	virtual void InitListControl();
	virtual void ChnPosition(int cx, int cy);
	virtual HBRUSH DrawCtrl(CDC* pDC, UINT nCtrlID);
	virtual bool Skin_ButtonPressed(CString m_ButtonName);


protected:

	virtual BOOL OnInitDialog();	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnQueryOpen();
};
