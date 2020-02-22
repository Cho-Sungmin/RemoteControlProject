// CScreenView.cpp: 구현 파일
//

#include "stdafx.h"
#include "RemoteControl.h"
#include "CScreenView.h"
#include "resource.h"
#include "MainFrm.h"



// CScreenView

IMPLEMENT_DYNCREATE(CScreenView, CView)

int CScreenView::st_uId = 0;

CScreenView::CScreenView()
{
}

CScreenView::~CScreenView()
{
}

BEGIN_MESSAGE_MAP(CScreenView, CView)
	ON_MESSAGE(ON_MOUSEHOOK, &OnMouseHook)
	ON_MESSAGE(ON_KBHOOK, &OnKBHook)
	ON_MESSAGE(ON_DISPLAY_IMG, &OnDisplayImg)
	ON_MESSAGE(ON_SEND_ACK, &OnSendACK)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CScreenView 그리기

void CScreenView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CScreenView 진단

#ifdef _DEBUG
void CScreenView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CScreenView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CScreenView 메시지 처리기

int CScreenView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_hdc = ::GetDC(this->GetSafeHwnd());

	return 0;
}

LRESULT CScreenView::OnDisplayImg(WPARAM wParam, LPARAM lParam)
{
	CRect rc;
	CImage img;
	int result;
	IStream* pIstream = (IStream*)wParam;

	this->GetClientRect(&rc);
	result = img.Load(pIstream);

	if (!FAILED(result))
	{
		img.Draw(m_hdc, 0, 0, rc.right, rc.bottom);
	}

	return 0;
}

LRESULT CScreenView::OnMouseHook(WPARAM wParam, LPARAM lParam)
{
	/* send packets about mouse events */

	PACKET_INFO head;
	WSABUF wsaBuf[2];
	Mouse_Point mp = *(Mouse_Point*)lParam;
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CRemoteControlDoc* pDoc = (CRemoteControlDoc*)pMain->GetActiveDocument();
	DWORD dwSent, dwFlag;
	dwSent = dwFlag = 0;

	head.type = PACKET_TYPE_SEND_MP;
	head.uId = st_uId;

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);

	wsaBuf[1].buf = (char*)&mp;
	wsaBuf[1].len = sizeof(mp);


	int addr_len = sizeof(pMain->param.addr);

	WSASendTo(pMain->m_udpSock, wsaBuf, 2, &dwSent, dwFlag, (SOCKADDR*)&pMain->param.addr, addr_len, NULL, NULL);

	return 0;
}

LRESULT CScreenView::OnKBHook(WPARAM wParam, LPARAM lParam)
{
	/* send packets about keyboard events */

	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();

	int nCode = *(int*)lParam;
	WSABUF wsaBuf[2];
	DWORD dwSent = 0, dwFlag = 0;
	PACKET_INFO head;

	head.type = PACKET_TYPE_SEND_KB;
	head.uId = st_uId;

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);
	wsaBuf[1].buf = (char*)&nCode;
	wsaBuf[1].len = sizeof(nCode);


	int addr_len = sizeof(pMain->param.addr);

	WSASendTo(pMain->m_udpSock, wsaBuf, 2, &dwSent, dwFlag, (SOCKADDR*)&pMain->m_udpClntAddr, addr_len, NULL, NULL);

	return 0;
}
LRESULT CScreenView::OnSendACK(WPARAM wParam, LPARAM lParam)
{
	/* send ack packet with flow control information */
	DWORD dwSent = 0, dwFlag = 0;
	PACKET_INFO head;
	head.type = PACKET_TYPE_SEND_ACK;
	head.uId = st_uId;

	WSABUF wsaBuf[2];
	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);
	int packetSize = *(int*)lParam;
	wsaBuf[1].buf = (char*)&packetSize;
	wsaBuf[1].len = sizeof(packetSize);

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	int addr_len = sizeof(pMain->param.addr);
	WSASendTo(pMain->m_udpSock, wsaBuf, 2, &dwSent, dwFlag, (SOCKADDR*)&pMain->m_udpClntAddr, addr_len, NULL, NULL);

	return 0;
}
