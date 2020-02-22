
// MainFrm.cpp: CMainFrame 클래스의 구현
//


#include "stdafx.h"
#include "RemoteControl.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection CMainFrame::st_cs;
int CMainFrame::st_ackNum = BIT_RATE;

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

DataQueue<Image_Packet> CMainFrame::st_que;
bool CMainFrame::st_ImgThreadFlag = false;
bool CMainFrame::st_recvThreadFlag = false;
HANDLE CMainFrame::st_hMutex;

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_hDll = NULL;
	m_pScView = NULL;
	m_pRecvThread = NULL;
	m_pImgThread = NULL;
}

CMainFrame::~CMainFrame()
{
	/* Thread exit */
	st_recvThreadFlag = false;
	st_ImgThreadFlag = false;

	/* socket close */
	closesocket(m_udpSock);
	WSACleanup();

	/* Threads exit sync */
	DWORD dwExitCode;

	if (m_pRecvThread != NULL) {
		::GetExitCodeThread(m_pRecvThread->m_hThread, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
			delete m_pRecvThread;
		else {
			::WaitForSingleObject(m_pRecvThread->m_hThread, INFINITE);
			delete m_pRecvThread;
		}
	}
	if (m_pImgThread != NULL) {
		::GetExitCodeThread(m_pImgThread->m_hThread, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
			delete(m_pImgThread);
		else {
			::WaitForSingleObject(m_pImgThread->m_hThread, INFINITE);
			delete(m_pImgThread);
		}
	}
	if(m_hDll != NULL){
		FreeLibrary(m_hDll);
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

	/////////////////////* Ready a valid socket */////////////////////////////

	/* Initializing values */
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		::SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);

	m_udpServAddr.sin_family = AF_INET;
	m_udpServAddr.sin_port = htons(RELAY_PORT);
	m_udpServAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	m_udpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_udpSock == INVALID_SOCKET)
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
	/* recv image packets from client */

	WSABUF wsaBuf[2];
	PACKET_INFO head;
	Image_Packet img_pack;
	RC_Param *p = (RC_Param*)param;
	int addr_len = sizeof(SOCKADDR_IN);
	DWORD dwRecv, dwFlags;
	dwRecv = dwFlags = 0;

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);
	wsaBuf[1].buf = (char*)&img_pack;
	wsaBuf[1].len = sizeof(img_pack);

	while (st_recvThreadFlag)
	{
		WSARecvFrom(p->sock, wsaBuf, 2, &dwRecv, &dwFlags, (SOCKADDR*)&p->addr, &addr_len, NULL, NULL);
		st_cs.Lock();					
		st_que.enqueue(img_pack);		//enqueue right after receive packets
		st_cs.Unlock();
	}

	return 0;
}

UINT CMainFrame::Img_Thread_Func(LPVOID param)
{
	/* make image data with packets in the queue */

	int seq;
	char* buf;
	bool enter;
	int readSize = 0;
	Image_Packet data;
	IStream* pIstream;
	HWND hScView = (HWND) param;
	HGLOBAL h_buffer = ::GlobalAlloc(GMEM_MOVEABLE, GLOBAL_MEMORY_SIZE);

	if (::CreateStreamOnHGlobal(h_buffer, TRUE, &pIstream) == S_OK)
	{	
		buf = (char*)::GlobalLock(h_buffer);
		while (st_ImgThreadFlag)
		{
			int tmp = 0;
			readSize = 0;
			seq = 1;

			while (st_ImgThreadFlag)
			{
				st_cs.Lock();
				enter = st_que.dequeue(&data);
				st_cs.Unlock();

				if (enter)	// queue was not empty
				{
					if (data.seq >= seq)		// still have fragment
					{
						if (data.seq == seq)	// completely received
						{
							seq++;
						}
						else                    // missed packets
						{
							tmp++;
							seq = data.seq+1;
						}
						memcpy(buf + readSize, data.data, data.size);
						readSize += data.size;
						if (data.flag == false)		// the last packet is arrived
						{
							if (tmp < 1) {	// broken img skip
								st_ackNum++;	// flow control
								ZeroMemory(buf + readSize, GLOBAL_MEMORY_SIZE - readSize);
								::SendMessage(hScView, ON_DISPLAY_IMG, (WPARAM)pIstream, 0);
							}
							else
								st_ackNum -= 3;	// flow control

							break;
						}
					}
					else 		// missed the last packet
					{
						st_ackNum -= 3;	// flow control
						break;
					}
				}
				else
					Sleep(0);					// switch to another thread
			}//while
			if (st_ackNum < 1)
				st_ackNum = 1;
			::SendNotifyMessage(hScView, ON_SEND_ACK, 0, (LPARAM)&st_ackNum);
		}//while
		::GlobalUnlock(h_buffer);
		pIstream->Release();
	}	
	
	return 0;
}

LRESULT CMainFrame::OnThread(WPARAM wParam, LPARAM lParam)
{
	/* try connect to server */

	Log buf;
	WSABUF wsaBuf[2];
	PACKET_INFO head;
	wchar_t *wcStr;
	int addr_len = sizeof(m_udpServAddr);
	DWORD dwSent, dwRecv, dwFlags;
	dwSent = dwRecv = dwFlags = 0;
	CRemoteControlView* pView = (CRemoteControlView*)GetActiveView();
	
	ZeroMemory((void*)&buf, sizeof(buf));
	head.type = PACKET_TYPE_CON_HOST;

	buf.setMode(pView->m_bt_p2p.GetCheck());	//mode : Relay or P2P

	/* get information of connection */
	CString str, w_char_str;

	pView->m_edit_PID.GetWindowTextW(str);
	w_char_str = str.GetBuffer(str.GetLength());
	char tmp_id[ID_LENGTH];
	WideCharToMultiByte(CP_ACP, 0, w_char_str, -1, tmp_id, ID_LENGTH, 0, 0);
	str.ReleaseBuffer();
	buf.setID(tmp_id);

	str.SetString(*(CString*)lParam);
	w_char_str = str.GetBuffer(str.GetLength());
	char tmp_pw[PW_LENGTH];
	WideCharToMultiByte(CP_ACP, 0, w_char_str, -1, tmp_pw, PW_LENGTH, 0, 0);
	str.ReleaseBuffer();
	buf.setPW(tmp_pw);

	/* get private ip */
	SOCKADDR_IN myIP;
	//getsockname(udpSock, (SOCKADDR*)&buf.getPrvAddr(), &addr_len);
	myIP.sin_addr = GetInIpAddress();
	buf.setPrvAddr(myIP);

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);
	wsaBuf[1].buf = (char*)&buf;
	wsaBuf[1].len = sizeof(buf);

	/* send information id, pw, etc as a packet to server */
	WSASendTo(m_udpSock, wsaBuf, 2, &dwSent, 0, (SOCKADDR*)&m_udpServAddr, addr_len, NULL, NULL);
	/* receive information of the other client from server */
	WSARecvFrom(m_udpSock, wsaBuf, 2, &dwRecv, &dwFlags, (SOCKADDR*)&m_udpClntAddr, &addr_len, NULL, NULL);
	if (head.type == CON_FAILED)
	{
		::MessageBox(this->GetSafeHwnd(), _T("Wrong Access"), _T("Login Error"), 0);

		return 0;
	}
	else
	{
		m_pScView->st_uId = head.uId;

		if(buf.getMode() == MODE_P2P)
			m_udpClntAddr = buf.getPubAddr();		////p2p mode
	
		/////Initializing hooking//////

		m_hDll = LoadLibrary(_T("mouseHook.dll"));
		mouseHookStart = (PFN_MOUSE_HOOKSTART)GetProcAddress(m_hDll, "mouseHookStart");
		mouseHookStop = (PFN_MOUSE_HOOKSTOP)GetProcAddress(m_hDll, "mouseHookStop");
		kbHookStart = (PFN_KB_HOOKSTART)GetProcAddress(m_hDll, "kbHookStart");
		kbHookStop = (PFN_KB_HOOKSTOP)GetProcAddress(m_hDll, "kbHookStop");

		param.sock = m_udpSock;
		param.addr = m_udpClntAddr;

		return 1;
	}
}

LRESULT CMainFrame::OnScreen(WPARAM wParam, LPARAM lParam)
{
	int addr_len = sizeof(SOCKADDR_IN);
	char dummy[2] = { 0 };

	MoveWindow(0, 0, 1500, 950, 0);
	m_pRcView = (CRemoteControlView*)GetActiveView();

	if (m_pScView == NULL)
	{
		CCreateContext context;
		context.m_pCurrentDoc = GetActiveDocument();

		//p_ScView = (CScreenView*)CScreenView::CreateObject();
		m_pScView = new CScreenView();

		m_pScView->Create(NULL, NULL, 0, CFrameWnd::rectDefault, this, 2, &context);
		m_pScView->OnInitialUpdate();
	}

	/* change veiw */
	SetActiveView(m_pScView, 1);
	m_pScView->ShowWindow(SW_SHOW);
	m_pRcView->ShowWindow(SW_HIDE);
	m_pScView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
	RecalcLayout();

	if(m_pRcView->m_bt_p2p.GetCheck() == MODE_P2P)
		sendto(m_udpSock, dummy, sizeof(dummy), 0, (SOCKADDR*)&m_udpClntAddr, addr_len);	//// for hole punching

	/* create threads */
	st_recvThreadFlag = true;
	st_ImgThreadFlag = true;
	m_pRecvThread = AfxBeginThread(Recv_Thread_Func, (LPVOID)&param);
	m_pImgThread = AfxBeginThread(Img_Thread_Func, (LPVOID)m_pScView->GetSafeHwnd());
	m_pRecvThread->m_bAutoDelete = FALSE;
	m_pImgThread->m_bAutoDelete = FALSE;

	/* hook dll dynamic call */
	mouseHookStart(GetCurrentThreadId());
	kbHookStart(GetCurrentThreadId());

	return 0;
}

BOOL CMainFrame::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (m_pScView != NULL) {
		Mouse_Point mp;
		mp.msg = -1;	// disconnection announcement
		m_pScView->SendMessage(ON_MOUSEHOOK, 0, (LPARAM)&mp);	//// for announce disconnection
	}

	st_ImgThreadFlag = false;
	st_recvThreadFlag = false;

	return CFrameWnd::DestroyWindow();
}
