
// MainFrm.cpp: CMainFrame 클래스의 구현
//


#include "stdafx.h"
#include "RemoteControl.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection CMainFrame::cs;
int CMainFrame::g_uId;

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(ON_THREAD, &OnThread)
	ON_MESSAGE(ON_SCREEN, &OnScreen)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

DataQueue<Image_Packet> CMainFrame::que;
bool CMainFrame::ImgThreadFlag, CMainFrame::recvThreadFlag;
HANDLE CMainFrame::h_mutex;

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	hDll = NULL;
	p_ScView = NULL;
	h_recv_thread = NULL;
	h_img_thread = NULL;
	recvThreadFlag = false;
	ImgThreadFlag = false;
}

CMainFrame::~CMainFrame()
{
	recvThreadFlag = false;
	ImgThreadFlag = false;
	closesocket(udpSock);
	WSACleanup();
	DWORD dwExitCode;
	if (h_recv_thread != NULL) {
		::GetExitCodeThread(h_recv_thread->m_hThread, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
			delete h_recv_thread;
		else {
			::WaitForSingleObject(h_recv_thread->m_hThread, INFINITE);
			delete h_recv_thread;
		}
	}
	if (h_img_thread != NULL) {
		::GetExitCodeThread(h_img_thread->m_hThread, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
			delete(h_img_thread);
		else {
			::WaitForSingleObject(h_img_thread->m_hThread, INFINITE);
			delete(h_img_thread);
		}
	}
	if(hDll != NULL){
		FreeLibrary(hDll);
		mouseHookStop();
		kbHookStop();
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		::SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);

	udpServAddr.sin_family = AF_INET;
	udpServAddr.sin_port = htons(RELAY_PORT);
	udpServAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	udpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (udpSock == INVALID_SOCKET)
	{
		MessageBox(_T("Socket creation Error"));
		WSACleanup();
		::SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);
		return 0;
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

UINT CMainFrame::Recv_Thread_Func(LPVOID param)
{
	WSABUF wsaBuf[2];
	PACKET_INFO head;
	DWORD dwRecv, dwFlags = 0;
	Image_Packet img_pack;
	RC_Param *p = (RC_Param*)param;
	int addr_len = sizeof(SOCKADDR_IN);

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);
	wsaBuf[1].buf = (char*)&img_pack;
	wsaBuf[1].len = sizeof(img_pack);

	while (recvThreadFlag)
	{
		////receive img data
		WSARecvFrom(p->sock, wsaBuf, 2, &dwRecv, &dwFlags, (SOCKADDR*)&p->addr, &addr_len, NULL, NULL);
		cs.Lock();					
		que.enqueue(img_pack);
		cs.Unlock();
	}

	return 0;
}

UINT CMainFrame::Img_Thread_Func(LPVOID param)
{
	FILE *fp;
	Image_Packet data;
	HWND h_view = (HWND) param;
	bool enter;
	int seq;

	while (ImgThreadFlag)
	{
		seq = 0;
		fopen_s(&fp, "img.jpg", "wb");
		fseek(fp, 0, SEEK_SET);
		while(ImgThreadFlag)
		{
			cs.Lock();
			enter = que.dequeue(&data);
			cs.Unlock();

			if (enter)						//// queue was not empty
			{
				if(data.seq > seq){			//// still have fragment
					fwrite(data.data, 1, data.size, fp);
					seq = data.seq;

					if (data.flag == false)	//// last data is arrived
						break;
				}
				else break;
			}
			else{
				Sleep(0);					//// switch to another thread
			}

		}
		fclose(fp);
		::SendMessage(h_view, ON_DISPLAY_IMG, 0, 0);
	}
	
	return 0;
}

LRESULT CMainFrame::OnThread(WPARAM wParam, LPARAM lParam)
{
	WSABUF wsaBuf[2];
	Log buf;
	PACKET_INFO head;
	DWORD dwSent, dwRecv, dwFlags = 0;
	CString str;
	int addr_len = sizeof(udpServAddr);
	wchar_t *wcStr;
	CRemoteControlView* pView = (CRemoteControlView*)GetActiveView();
	
	ZeroMemory((void*)&buf, sizeof(buf));
	head.type = PACKET_TYPE_CON_HOST;
	head.uId = -1;

	buf.setMode(pView->m_bt_p2p.GetCheck());		//// check mode

	pView->m_edit_PID.GetWindowTextW(str);
	wcStr = str.GetBuffer(str.GetLength());
	str.ReleaseBuffer();
	WideCharToMultiByte(CP_ACP, 0, wcStr, -1, buf.getID(), sizeof(buf.getID())+1, 0, 0);
	str.SetString(*(CString*)lParam);
	wcStr = str.GetBuffer(str.GetLength());
	str.ReleaseBuffer();
	WideCharToMultiByte(CP_ACP, 0, wcStr, -1, buf.getPW(), sizeof(buf.getPW())+1, 0, 0);

	getsockname(udpSock, (SOCKADDR*)&buf.getPrvAddr(), &addr_len);
	SOCKADDR_IN myIP = buf.getPrvAddr();
	myIP.sin_addr = GetInIpAddress();
	buf.setPrvAddr(myIP);

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);
	wsaBuf[1].buf = (char*)&buf;
	wsaBuf[1].len = sizeof(buf);

	//// Connection Request
	WSASendTo(udpSock, wsaBuf, 2, &dwSent, 0, (SOCKADDR*)&udpServAddr, addr_len, NULL, NULL);
	//// Receive Ack
	WSARecvFrom(udpSock, wsaBuf, 2, &dwRecv, &dwFlags, (SOCKADDR*)&udpClntAddr, &addr_len, NULL, NULL);
	if (head.type == CON_FAILED)
	{
		::MessageBox(this->GetSafeHwnd(), _T("Wrong Access"), _T("Login Error"), 0);

		return 0;
	}
	else
	{
		CRemoteControlDoc* pDoc = (CRemoteControlDoc*)GetActiveDocument();

		pDoc->setUid(head.uId);

		if(buf.getMode())
			udpClntAddr = buf.getPubAddr();		////p2p mode
	

		/////Initializing hooking//////

		hDll = LoadLibrary(_T("mouseHook.dll"));
		mouseHookStart = (PFN_MOUSE_HOOKSTART)GetProcAddress(hDll, "mouseHookStart");
		mouseHookStop = (PFN_MOUSE_HOOKSTOP)GetProcAddress(hDll, "mouseHookStop");
		kbHookStart = (PFN_KB_HOOKSTART)GetProcAddress(hDll, "kbHookStart");
		kbHookStop = (PFN_KB_HOOKSTOP)GetProcAddress(hDll, "kbHookStop");

		param.sock = udpSock;
		param.addr = udpClntAddr;

		return 1;
	}
}

LRESULT CMainFrame::OnScreen(WPARAM wParam, LPARAM lParam)
{
	int addr_len = sizeof(SOCKADDR_IN);
	char dummy[2] = { 0 };

	MoveWindow(0, 0, 1500, 950, 0);

	p_RcView = (CRemoteControlView*)GetActiveView();
	if (p_ScView == NULL)
	{
		CCreateContext context;
		context.m_pCurrentDoc = GetActiveDocument();

		//p_ScView = (CScreenView*)CScreenView::CreateObject();
		p_ScView = new CScreenView();

		p_ScView->Create(NULL, NULL, 0, CFrameWnd::rectDefault, this, 2, &context);
		p_ScView->OnInitialUpdate();
	}
	SetActiveView(p_ScView, 1);
	p_ScView->ShowWindow(SW_SHOW);
	p_RcView->ShowWindow(SW_HIDE);
	p_ScView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
	RecalcLayout();

	if(p_RcView->m_bt_p2p.GetCheck())
		sendto(udpSock, dummy, sizeof(dummy), 0, (SOCKADDR*)&udpClntAddr, addr_len);	//// for hole punching

	recvThreadFlag = true;
	ImgThreadFlag = true;
	h_recv_thread = AfxBeginThread(Recv_Thread_Func, (LPVOID)&param);
	h_img_thread = AfxBeginThread(Img_Thread_Func, (LPVOID)p_ScView->GetSafeHwnd());
	h_recv_thread->m_bAutoDelete = FALSE;
	h_img_thread->m_bAutoDelete = FALSE;

	mouseHookStart(GetCurrentThreadId());
	kbHookStart(GetCurrentThreadId());

	return 0;
}

BOOL CMainFrame::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (p_ScView != NULL) {
		Mouse_Point mp;
		mp.msg = -1;
		p_ScView->SendMessage(ON_MOUSEHOOK, 0, (LPARAM)&mp);	//// for announce disconnection
	}

	ImgThreadFlag = false;
	recvThreadFlag = false;

	return CFrameWnd::DestroyWindow();
}
