#pragma once

#include <afxwin.h>
#include "CPassWordDlg.h"

#define ON_INIT_BT WM_USER+1

class CModaless :
	public CDialog
{
public:
	CButton m_bt_cap;
	CEdit m_edit_ID;
	CEdit m_edit_pwd;
	CEdit m_edit_PID;
	CStatic m_stat_pwd;
	CStatic m_stat_ID;
	CStatic m_stat_PID;

	CPassWordDlg *pPassDlg;

public:
	CModaless();
	~CModaless();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg LRESULT OnInitControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStartBtClicked();
};

