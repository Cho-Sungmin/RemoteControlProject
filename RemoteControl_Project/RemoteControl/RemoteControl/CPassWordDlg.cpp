// CPassWordDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "RemoteControl.h"
#include "CPassWordDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "RemoteControlView.h"
#include "RemoteControlDoc.h"


// CPassWordDlg 대화 상자

IMPLEMENT_DYNAMIC(CPassWordDlg, CDialogEx)

CPassWordDlg::CPassWordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PASSWORD, pParent)
{
}

CPassWordDlg::CPassWordDlg(int id, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PASSWORD, pParent)
{
	m_id = id;
}

CPassWordDlg::~CPassWordDlg()
{
}

void CPassWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}


BEGIN_MESSAGE_MAP(CPassWordDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPassWordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPassWordDlg 메시지 처리기


void CPassWordDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
	CString str;

	m_edit.SetWindowText(_T("100006334"));	/* This should be changed. Now just for test */
	m_edit.GetWindowTextW(str);

	if(pMain->SendMessageW(ON_THREAD, 0, (LPARAM)&str))
		pMain->SendMessageW(ON_SCREEN, 0, 0);

	CDialogEx::OnOK();
}
