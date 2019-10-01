
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "RemoteControl_Customer.h"

#include "RemoteControl_CustomerView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

int CMainFrame::uId;
bool CMainFrame::sm_sendThreadFlag;
CaptureScreen CMainFrame::m_cap;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(ON_THREAD, &OnThread)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
	closesocket(m_udpSock);
	WSACleanup();
	sm_sendThreadFlag = false;
	DWORD dwExitCode;
	if(m_pSend_thread != NULL){
		::GetExitCodeThread(m_pSend_thread->m_hThread, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
			delete m_pSend_thread;
		else {
			::WaitForSingleObject(m_pSend_thread->m_hThread, INFINITE);
			delete m_pSend_thread;
		}
	}
	if (m_pMouse_thread != NULL) {
		::GetExitCodeThread(m_pMouse_thread->m_hThread, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
			delete m_pMouse_thread;
		else {
			::WaitForSingleObject(m_pMouse_thread->m_hThread, INFINITE);
			delete m_pMouse_thread;
		}
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

	HDC h_screen_bg = ::GetDC(NULL);
	int x = (::GetDeviceCaps(h_screen_bg, HORZRES) / 2) - 290;
	int y = (::GetDeviceCaps(h_screen_bg, VERTRES) / 2) - 200;

	SetWindowPos(NULL, x, y, 480, 400, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_MINIMIZEBOX | WS_SYSMENU;

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

LRESULT CMainFrame::OnThread(WPARAM wParam, LPARAM lParam)
{
	CRemoteControlCustomerView *pView = (CRemoteControlCustomerView *)GetActiveView();
	WSABUF wsaBuf[2];
	Log buf;
	PACKET_INFO head;
	DWORD dwSent=0, dwRecv=0, dwFlags = 0;
	wchar_t *tmp;
	CString str;
	int addr_len = sizeof(m_udpAddr);
	HDC h_screen_bg = ::GetDC(NULL);

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		::SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);

	m_udpAddr.sin_family = AF_INET;
	m_udpAddr.sin_port = htons(RELAY_PORT);
	m_udpAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	m_udpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (m_udpSock == INVALID_SOCKET)
	{
		MessageBox(_T("Socket creation Error"));
		WSACleanup();
		::SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);
	}
	ZeroMemory((void*)&buf, sizeof(buf));
	head.type = PACKET_TYPE_CON_CUST;
	head.uId = -1;

	buf.setMode(pView->m_bt_p2p->GetCheck());
	str = pView->m_id;
	tmp = str.GetBuffer(str.GetLength());
	WideCharToMultiByte(CP_ACP, 0, tmp, -1, buf.getID(), sizeof(buf.getID())+1, 0, 0);

	str = pView->m_pw;
	tmp = str.GetBuffer(str.GetLength());
	WideCharToMultiByte(CP_ACP, 0, tmp, -1, buf.getPW(), sizeof(buf.getPW())+1, 0, 0);

	getsockname(m_udpSock, (SOCKADDR*)&buf.getPrvAddr(), &addr_len);
	SOCKADDR_IN myIP = buf.getPrvAddr();
	myIP.sin_addr = GetInIpAddress();
	buf.setPrvAddr(myIP);

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);
	wsaBuf[1].buf = (char*)&buf;
	wsaBuf[1].len = sizeof(buf);

	WSASendTo(m_udpSock, wsaBuf, 2, &dwSent, 0, (SOCKADDR*)&m_udpAddr, addr_len, NULL, NULL);
	pView->m_bt_engage->SetWindowTextW(_T("접속 대기중"));
	int size = WSARecvFrom(m_udpSock, wsaBuf, 2, &dwRecv, &dwFlags, (SOCKADDR*)&m_udpAddr, &addr_len, NULL, NULL);
	if (head.type == CON_FAILED || size == -1)
	{
		AfxMessageBox(_T("접속 실패!!"));
		pView->m_bt_engage->SetWindowTextW(_T("접속 대기중"));

		return 0;
	}
	else {
		if(buf.getMode())
			m_udpAddr = buf.getPubAddr();		//// p2p mode
		sm_sendThreadFlag = true;
		

		int x = (::GetDeviceCaps(h_screen_bg, HORZRES)) - 450;
		int y = (::GetDeviceCaps(h_screen_bg, VERTRES)) - 500;
		
		SetWindowPos(NULL, x, y, 400, 450, NULL);
		pView->SendMessage(ON_CONNECT, 0, 0);

		this->UpdateWindow();

		m_param.sock = m_udpSock;
		m_param.addr = m_udpAddr;
		m_pSend_thread = ::AfxBeginThread(Send_Thread_Func, (void*)&m_param);
		m_pMouse_thread = ::AfxBeginThread(Mouse_Thread_Func, (void*)&m_param);
		m_pSend_thread->m_bAutoDelete = FALSE;
		m_pMouse_thread->m_bAutoDelete = FALSE;
		//int x = (::GetDeviceCaps(h_screen_bg, HORZRES) / 2) - 290;
		//int y = (::GetDeviceCaps(h_screen_bg, VERTRES) / 2) - 200;

		//SetWindowPos(NULL, x, y, 480, 400, NULL);
		//pView->SendMessage(ON_DISCONNECT, 0, 0);

		return 1;

	}

}

UINT CMainFrame::Send_Thread_Func(LPVOID param)
{
	FILE* fp;
	WSABUF wsaBuf[2];
	DWORD dwSent = 0;
	Image_Packet imgBuf; 
	RC_Param *p = (RC_Param*)param;
	int addr_len = sizeof(SOCKADDR_IN);
	PACKET_INFO head;

	head.type = PACKET_TYPE_SEND_IMG;
	head.uId = uId;

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);

	wsaBuf[1].buf = (char*)&imgBuf;
	wsaBuf[1].len = sizeof(imgBuf);

	while (sm_sendThreadFlag)
	{
		imgBuf.seq = 0;
		m_cap.capture();
		fopen_s(&fp, "image.jpg", "rb");
		fseek(fp, 0, SEEK_SET);
		while (1)
		{
			imgBuf.flag = true;
			imgBuf.size = fread(imgBuf.data, 1, sizeof(imgBuf.data), fp);
			imgBuf.seq++;
			
			if(imgBuf.seq%10 == 0)		//// Flow Control
				Sleep(1);			
			if (feof(fp) == 1)
			{
				imgBuf.flag = false;
				WSASendTo(p->sock, wsaBuf, 2, &dwSent, 0, (SOCKADDR*)&p->addr, addr_len, NULL, NULL);
				
				break;
			}
			else {
				WSASendTo(p->sock, wsaBuf, 2, &dwSent, 0, (SOCKADDR*)&p->addr, addr_len, NULL, NULL);
			}
		}
		fclose(fp);
	}
	return 0;
}
UINT CMainFrame::Mouse_Thread_Func(LPVOID param)
{
	Mouse_Point mp;
	RC_Param *p = (RC_Param*)param;
	SOCKADDR_IN udpDesAddr;
	MPoint point;
	int nCode;
	PACKET_INFO head;
	WSABUF wsaBuf[2];
	DWORD dwRecv, dwFlag = 0;
	int addr_len = sizeof(SOCKADDR_IN);
	int width = 0, height = 0;

	wsaBuf[0].buf = (char*)&head;
	wsaBuf[0].len = sizeof(head);

	wsaBuf[1].buf = (char*)&mp;
	wsaBuf[1].len = sizeof(mp);
	

	width = ::GetSystemMetrics(SM_CXSCREEN);//::GetDeviceCaps(h_screen_dc, HORZRES);
	height = ::GetSystemMetrics(SM_CYSCREEN);//::GetDeviceCaps(h_screen_dc, VERTRES);

	while(sm_sendThreadFlag)
	{
		dwRecv = 0;
		WSARecvFrom(p->sock, wsaBuf, 2, &dwRecv, &dwFlag, (SOCKADDR*)&udpDesAddr, &addr_len, NULL,NULL);
		if(dwRecv > 0)
		{
			if (head.type == PACKET_TYPE_SEND_MP) {
				if (mp.msg == WM_LBUTTONDBLCLK)
				{
					point.x = mp.point.x * width;
					point.y = mp.point.y * height;
					SetCursorPos(point.x, point.y);
					Sleep(150);
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					Sleep(150);
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				}
				/*else if (mp.msg == WM_MOUSEMOVE)
				{
					point.x = mp.point.x * width;
					point.y = mp.point.y * height;
					SetCursorPos(point.x, point.y);
				}*/

				else if (mp.msg == WM_LBUTTONDOWN)
				{
					point.x = mp.point.x * width;
					point.y = mp.point.y * height;

					SetCursorPos(point.x, point.y);
					Sleep(150);
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				}

				else if (mp.msg == WM_LBUTTONUP)
				{
					point.x = mp.point.x * width;
					point.y = mp.point.y * height;

					SetCursorPos(point.x, point.y);
					Sleep(150);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				}
				else if (mp.msg == WM_RBUTTONUP)
				{
					point.x = mp.point.x * width;
					point.y = mp.point.y * height;

					SetCursorPos(point.x, point.y);
					Sleep(150);
					mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
				}
				else if (mp.msg == WM_RBUTTONDOWN)
				{
					point.x = mp.point.x * width;
					point.y = mp.point.y * height;

					SetCursorPos(point.x, point.y);
					Sleep(150);
					mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
				}
				else if (mp.msg == DISCONNECT) {
					sm_sendThreadFlag = false;
					CMainFrame* pMain = (CMainFrame*)AfxGetApp()->GetMainWnd();

					int x = (::GetDeviceCaps(::GetDC(NULL), HORZRES) / 2) - 290;
					int y = (::GetDeviceCaps(::GetDC(NULL), VERTRES) / 2) - 200;

					::SetWindowPos(pMain->GetSafeHwnd(), NULL, x, y, 480, 400, NULL);
					CRemoteControlCustomerView* pView = (CRemoteControlCustomerView*)pMain->GetActiveView();

					::SendMessage(pView->GetSafeHwnd(), ON_DISCONNECT, 0, 0);

				}
			}
			else if (head.type == PACKET_TYPE_SEND_KB)
			{
				memcpy(&nCode, wsaBuf[1].buf, sizeof(nCode));

				keybd_event(nCode, 0, 0, 0);
				keybd_event(nCode, 0, KEYEVENTF_KEYUP, 0);
			}
		}
		else {
			FILE *fp;
			DWORD errNo = GetLastError();

			fopen_s(&fp, "recv.txt", "a");

			fprintf(fp, "Error : %d\n", errNo);
			fclose(fp);

			sm_sendThreadFlag = false;
			
		}
	}
	return 0;
}
