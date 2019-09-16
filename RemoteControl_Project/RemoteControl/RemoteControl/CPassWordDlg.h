#pragma once

#include "CPassWordDlg.h"
// CPassWordDlg 대화 상자

class CPassWordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPassWordDlg)

public:
	CPassWordDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	CPassWordDlg(int id, CWnd* pParent = nullptr);
	virtual ~CPassWordDlg();

public:
	int m_id;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PASSWORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_edit;
	
};
