
// RemoteControl_CustomerView.cpp: CRemoteControlCustomerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "RemoteControl_Customer.h"
#endif

#include "RemoteControl_CustomerView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRemoteControlCustomerView

IMPLEMENT_DYNCREATE(CRemoteControlCustomerView, CFormView)

BEGIN_MESSAGE_MAP(CRemoteControlCustomerView, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)

	ON_BN_CLICKED(IDC_BUTTON_ENGAGE, OnClickedEngageBt)

	ON_MESSAGE(ON_CONNECT, &OnConnect)
	ON_MESSAGE(ON_DISCONNECT, &OnDisConnect)
END_MESSAGE_MAP()

// CRemoteControlCustomerView 생성/소멸

CRemoteControlCustomerView::CRemoteControlCustomerView() noexcept
	: CFormView(IDD_REMOTECONTROL_CUSTOMER_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_stat_id = NULL;
	m_stat_pwd = NULL;
	m_stat_run = NULL;
	m_bt_engage = NULL;

}

CRemoteControlCustomerView::~CRemoteControlCustomerView()
{
	delCtrl();
}

void CRemoteControlCustomerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CRemoteControlCustomerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CRemoteControlCustomerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	
	CRect rc;
	int randNum;

	randNum = rand() % 999999999 + 100000000;
	m_id.Format(_T("%d"), randNum);

	randNum = rand() % 999999999 + 100000000;
	m_pw.Format(_T("%d"), randNum);

	setConnectView();

}
LRESULT CRemoteControlCustomerView::OnConnect(WPARAM wParam, LPARAM lParam)
{
	delCtrl();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	setRunningView();

	return 0;
}

LRESULT CRemoteControlCustomerView::OnDisConnect(WPARAM wParam, LPARAM lParam)
{
	delCtrl();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	setConnectView();

	return 0;
}

void CRemoteControlCustomerView::setRunningView()
{
	m_stat_run = new CStatic();
	CString str = _T("RemoteControl is running...");
	m_stat_run->Create(str, WS_CHILD | WS_VISIBLE,
		CRect(10, 140, 280, 160), this);
}

void CRemoteControlCustomerView::setConnectView()
{
	CRect deviceSize;
	CRect rc;
	int width, height;
	CString strID, strPW;

	::GetWindowRect(this->GetSafeHwnd(), &rc);
	width = rc.right-rc.left;
	height = rc.bottom-rc.top;

	strID = _T("Access ID : ");
	strPW = _T("Access PW : ");

	strID.Append(m_id);
	strPW.Append(m_pw);

	m_stat_id = new CStatic();
	m_stat_id->Create(strID, WS_CHILD | WS_VISIBLE,
		CRect((width / 2) - 90, (height / 2) - 80, (width / 2) + 90, (height / 2) - 50), this);
	m_stat_pwd = new CStatic();
	m_stat_pwd->Create(strPW, WS_CHILD | WS_VISIBLE,
		CRect((width / 2) - 90, (height / 2) - 55, (width / 2) + 100, (height / 2) - 25), this);
	m_bt_engage = new CButton();
	m_bt_engage->Create(_T("접속 대기"), WS_CHILD | WS_VISIBLE,
		CRect((width / 2) - 70, (height / 2) + 30, (width / 2) + 70, (height / 2) + 80), this, IDC_BUTTON_ENGAGE);

	m_bt_p2p = new CButton();
	m_bt_p2p->Create(_T("P2P"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP
		, CRect((width / 2) - 90, (height / 2) - 25, (width / 2), (height / 2)), this, IDC_Radio_p2p);
	m_bt_relay = new CButton();
	m_bt_relay->Create(_T("Relay"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON
		, CRect((width / 2) + 10, (height / 2) - 25, (width / 2) + 100, (height / 2)), this, IDC_Radio_relay);

	m_bt_p2p->SetCheck(true);
}

void CRemoteControlCustomerView::delCtrl()
{
	if(m_stat_id != NULL){
		delete(m_stat_id);
		m_stat_id = NULL;
	}
	if (m_stat_pwd != NULL) {
		delete(m_stat_pwd);
		m_stat_pwd = NULL;
	}
	if (m_stat_run != NULL) {
		delete(m_stat_run);
		m_stat_run = NULL;
	}
	if (m_bt_engage != NULL) {
		delete(m_bt_engage);
		m_bt_engage = NULL;
	}
	if (m_bt_p2p != NULL) {
		delete(m_bt_p2p);
		m_bt_p2p = NULL;
	}
	if (m_bt_relay != NULL) {
		delete(m_bt_relay);
		m_bt_relay = NULL;
	}
}



// CRemoteControlCustomerView 인쇄

BOOL CRemoteControlCustomerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CRemoteControlCustomerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CRemoteControlCustomerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CRemoteControlCustomerView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}


// CRemoteControlCustomerView 진단

#ifdef _DEBUG
void CRemoteControlCustomerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRemoteControlCustomerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRemoteControlCustomerDoc* CRemoteControlCustomerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRemoteControlCustomerDoc)));
	return (CRemoteControlCustomerDoc*)m_pDocument;
}
#endif //_DEBUG


// CRemoteControlCustomerView 메시지 처리기
void CRemoteControlCustomerView::OnClickedEngageBt()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	::SendMessage(pMain->GetSafeHwnd(), ON_THREAD, 0,0);

}
