// CScreenView.cpp: 구현 파일
//

#include "stdafx.h"
#include "RemoteControl.h"
#include "CScreenView.h"
#include "resource.h"
#include "MainFrm.h"

#define ON_DISPLAY_IMG WM_USER+2

// CScreenView

IMPLEMENT_DYNCREATE(CScreenView, CView)



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
	hdc = ::GetDC(this->GetSafeHwnd());

	return 0;
}

LRESULT CScreenView::OnDisplayImg(WPARAM wParam, LPARAM lParam)
{
	CRect rc;
	CImage img;
	int result;
	IStream* p_istream = (IStream*)wParam;

	this->GetClientRect(&rc);
	result = img.Load(p_istream);

	if (result == 0)
	{
		img.Draw(hdc, 0, 0, rc.right, rc.bottom);
	}

	return 0;
}

LRESULT CScreenView::OnMouseHook(WPARAM wParam, LPARAM lParam)
{
	Mouse_Point mp = *(Mouse_Point*)lParam;
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CRemoteControlDoc* pDoc = (CRemoteControlDoc*)pMain->GetActiveDocument();
	PACKET_INFO head;
	WSABUF wsaBuf[2];
	DWORD dwSent, dwFlag = 0;

	head.type = PACKET_TYPE_SEND_MP;
	head.uId = pDoc->getUid();

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);

	wsaBuf[1].buf = (char*)&mp;
	wsaBuf[1].len = sizeof(mp);


	int addr_len = sizeof(pMain->param.addr);

	WSASendTo(pMain->udpSock, wsaBuf, 2, &dwSent, dwFlag, (SOCKADDR*)&pMain->param.addr, addr_len, NULL, NULL);

	return 0;
}

LRESULT CScreenView::OnKBHook(WPARAM wParam, LPARAM lParam)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CRemoteControlDoc* pDoc = (CRemoteControlDoc*)pMain->GetActiveDocument();

	int nCode = *(int*)lParam;
	DWORD dwSent = 0, dwFlag = 0;
	WSABUF wsaBuf[2];
	PACKET_INFO head;

	head.type = PACKET_TYPE_SEND_KB;
	head.uId = pDoc->getUid();

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);

	wsaBuf[1].buf = (char*)&nCode;
	wsaBuf[1].len = sizeof(nCode);


	int addr_len = sizeof(pMain->param.addr);

	WSASendTo(pMain->udpSock, wsaBuf, 2, &dwSent, dwFlag, (SOCKADDR*)&pMain->param.addr, addr_len, NULL, NULL);


	return 0;
}
