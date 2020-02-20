#include "stdafx.h"
#include "CModaless.h"
#include "resource.h"
#include "MainFrm.h"


CModaless::CModaless()
{

}


CModaless::~CModaless()
{
	delete(m_bt_cap);
}

BEGIN_MESSAGE_MAP(CModaless, CDialog)
	ON_WM_CLOSE()
	ON_MESSAGE(ON_INIT_BT, &OnInitControls)
	ON_BN_CLICKED(IDC_ButtonSwitch, OnStartBtClicked)
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CModaless::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	::SendMessage(pMain->GetSafeHwnd(), WM_CLOSE, 0, 0);

	CDialog::OnClose();
}

LRESULT CModaless::OnInitControls(WPARAM wParam, LPARAM lParam)
{
	CRect rc;
	CRemoteControlView *pView = (CRemoteControlView *)lParam;

	this->GetClientRect(&rc);

	m_stat_ID.Create(_T("�� ID : "), WS_CHILD | WS_VISIBLE
		, CRect(rc.left + 60, rc.top + 50, rc.left + 120, rc.top + 90), this, IDC_Stat_ID);
	m_stat_pwd.Create(_T("�� PW : "), WS_CHILD | WS_VISIBLE
		, CRect(rc.left + 50, rc.top + 100, rc.left + 120, rc.top + 140), this, IDC_Stat_pwd);
	m_stat_PID.Create(_T("�� ��Ʈ��ID : "), WS_CHILD | WS_VISIBLE
		, CRect(rc.left + 5, rc.top + 200, rc.left + 120, rc.top + 240), this, IDC_Stat_PID);

	m_edit_ID.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		, CRect(rc.left + 120, rc.top + 50, rc.left + 320, rc.top + 90), this, IDC_Edit_ID);
	m_edit_pwd.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		, CRect(rc.left + 120, rc.top + 100, rc.left + 320, rc.top + 140), this, IDC_Edit_pwd);
	m_edit_PID.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		, CRect(rc.left + 120, rc.top + 200, rc.left + 320, rc.top + 240), this, IDC_Edit_PID);

	m_bt_cap.Create(_T("��Ʈ�� ����"), WS_CHILD | WS_VISIBLE
		, CRect(rc.left+70, rc.bottom-100, rc.left + 290, rc.bottom-50), this, IDC_ButtonSwitch);


	CRemoteControlDoc *pDoc = (CRemoteControlDoc *)pView->GetDocument();
	CString str;

	str.Format(_T("%d"), pDoc->getRandID());
	m_edit_ID.SetWindowTextW(str);
	str.Format(_T("%d"), pDoc->getRandPW());
	m_edit_pwd.SetWindowTextW(str);

	return 0;
}

void CModaless::OnStartBtClicked()
{
	CString str;

	m_edit_PID.GetWindowTextW(str);

	int id = _ttoi(str);

	pPassDlg = new CPassWordDlg(id);

	pPassDlg->DoModal();

}

