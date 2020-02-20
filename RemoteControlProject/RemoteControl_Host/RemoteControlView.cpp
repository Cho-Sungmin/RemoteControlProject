
// RemoteControlView.cpp: CRemoteControlView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "RemoteControl.h"
#endif
#include "RemoteControlView.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRemoteControlView

IMPLEMENT_DYNCREATE(CRemoteControlView, CFormView)

BEGIN_MESSAGE_MAP(CRemoteControlView, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ButtonSwitch, OnStartBtClicked)
END_MESSAGE_MAP()

// CRemoteControlView 생성/소멸

CRemoteControlView::CRemoteControlView() noexcept
	: CFormView(IDD_FORMVIEW)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pPassDlg = NULL;
	
}

CRemoteControlView::~CRemoteControlView()
{
	if(m_hdc!=NULL)
		::ReleaseDC(NULL, m_hdc);
	if(m_pPassDlg != NULL)
		delete m_pPassDlg;
}

BOOL CRemoteControlView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	return CView::PreCreateWindow(cs);
}
void CRemoteControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CRect rc;
	this->GetClientRect(&rc);

	m_stat_ID.Create(_T("내 ID : "), WS_CHILD | WS_VISIBLE
		, CRect(rc.left + 60, rc.top + 50, rc.left + 120, rc.top + 90), this, IDC_Stat_ID);
	m_stat_pwd.Create(_T("내 PW : "), WS_CHILD | WS_VISIBLE
		, CRect(rc.left + 50, rc.top + 100, rc.left + 120, rc.top + 140), this, IDC_Stat_pwd);
	m_stat_PID.Create(_T("내 파트너ID : "), WS_CHILD | WS_VISIBLE
		, CRect(rc.left + 5, rc.top + 200, rc.left + 120, rc.top + 240), this, IDC_Stat_PID);

	m_edit_ID.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		, CRect(rc.left + 120, rc.top + 50, rc.left + 320, rc.top + 90), this, IDC_Edit_ID);
	m_edit_pwd.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		, CRect(rc.left + 120, rc.top + 100, rc.left + 320, rc.top + 140), this, IDC_Edit_pwd);
	m_edit_PID.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		, CRect(rc.left + 120, rc.top + 200, rc.left + 320, rc.top + 240), this, IDC_Edit_PID);

	m_bt_cap.Create(_T("파트너 연결"), WS_CHILD | WS_VISIBLE
		, CRect(rc.left + 70, rc.bottom - 100, rc.left + 290, rc.bottom - 50), this, IDC_ButtonSwitch);

	m_bt_p2p.Create(_T("P2P"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP
		, CRect(rc.left + 350, rc.top + 50, rc.left + 420, rc.top + 90), this, IDC_Radio_p2p);

	m_bt_relay.Create(_T("Relay"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON
		, CRect(rc.left + 350, rc.top + 100, rc.left + 420, rc.top + 140), this, IDC_Radio_relay);

	CRemoteControlDoc* pDoc = (CRemoteControlDoc*)GetDocument();
	CString str;

	/* fixed id, pw for tests */
	str.SetString(_T("100018467"));
	//str.Format(_T("%d"), pDoc->getRandID());
	m_edit_ID.SetWindowTextW(str);
	str.SetString(_T("100006334"));
	//str.Format(_T("%d"), pDoc->getRandPW());
	m_edit_pwd.SetWindowTextW(str);

	m_edit_PID.SetWindowTextW(_T("100018467"));

	m_bt_p2p.SetCheck(true);
}

void CRemoteControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

// CRemoteControlView 인쇄

BOOL CRemoteControlView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CRemoteControlView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CRemoteControlView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}
void CRemoteControlView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}

// CRemoteControlView 진단

#ifdef _DEBUG
void CRemoteControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRemoteControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRemoteControlDoc* CRemoteControlView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRemoteControlDoc)));
	return (CRemoteControlDoc*)m_pDocument;
}
#endif //_DEBUG


// CRemoteControlView 메시지 처리기
void CRemoteControlView::OnStartBtClicked()
{
	CString str;

	m_edit_PID.GetWindowTextW(str);

	int id = _ttoi(str);

	m_pPassDlg = new CPassWordDlg(id);

	m_pPassDlg->DoModal();

}
