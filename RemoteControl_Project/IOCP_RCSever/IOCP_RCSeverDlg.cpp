
// IOCP_RCSeverDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "IOCP_RCSever.h"
#include "IOCP_RCSeverDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IOCP_IO CIOCPRCSeverDlg::g_iocp;
HANDLE CIOCPRCSeverDlg::g_hIocp;
Customer* CIOCPRCSeverDlg::custTable;
int CIOCPRCSeverDlg::sizeOfTable;
int CIOCPRCSeverDlg::numOfUser;
bool CIOCPRCSeverDlg::flag;
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIOCPRCSeverDlg 대화 상자



CIOCPRCSeverDlg::CIOCPRCSeverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IOCP_RCSEVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIOCPRCSeverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btStart);
	DDX_Control(pDX, IDCANCEL, m_btEnd);
}

BEGIN_MESSAGE_MAP(CIOCPRCSeverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CIOCPRCSeverDlg::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CIOCPRCSeverDlg 메시지 처리기

BOOL CIOCPRCSeverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	numOfUser = 0;
	ZeroMemory(m_iocpThread, sizeof(CWinThread*)*CORE);
	pSocket = NULL;
	custTable = (Customer*)malloc(sizeof(Customer)*MAX_USER);
	ZeroMemory(custTable, sizeof(Customer)*MAX_USER);
	sizeOfTable = MAX_USER;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		::AfxMessageBox(_T("WSAStartup failed !!!"));

	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_sockAddr.sin_port = htons(RELAY_PORT);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CIOCPRCSeverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CIOCPRCSeverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CIOCPRCSeverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CIOCPRCSeverDlg::IOCPThread(LPVOID param)
{
	SOCKET_CONTEXT* pSocket = NULL;
	IO_CONTEXT* pIO = NULL;
	BOOL bRet;
	DWORD dwTransfer=0;
	PACKET_INFO* head;
	int error = -1;
	while (flag)
	{
		bRet = GetQueuedCompletionStatus(g_hIocp, &dwTransfer,
			(PULONG_PTR)&pSocket, (LPOVERLAPPED*)&pIO, INFINITE);

		if (!bRet)
			error = GetLastError();
		
		if (dwTransfer == 0)	//////socket disconnected
		{
			if (pIO != NULL)
			{
				delete pIO;
			}
			break;
		}
		if (pIO)
		{
			if (pIO->type == IO_TYPE_RECV)				////received data
			{
				head = (PACKET_INFO*)pIO->wsaBuf[0].buf;

				if (head->type == PACKET_TYPE_SEND_IMG)
				{
					if (custTable[head->uId].getHost() != nullptr)
					{
						pSocket->hAddr = custTable[head->uId].getHost()->getPubAddr();
						g_iocp.IOCP_SendTo(pSocket, pIO->wsaBuf);
					}
				}
				else if (head->type == PACKET_TYPE_SEND_MP)
				{
					if (custTable[head->uId].getHost() != nullptr)
					{
						pSocket->hAddr = custTable[head->uId].getCust().getPubAddr();
						g_iocp.IOCP_SendTo(pSocket, pIO->wsaBuf);
						if (((Mouse_Point*)&pIO->buf)->msg == DISCONNECT)
						{
							ZeroMemory(&custTable[head->uId], sizeof(Customer));
							numOfUser--;
						}
					}

				}
				else if (head->type == PACKET_TYPE_SEND_KB)
				{
					if (custTable[head->uId].getHost() != nullptr)
					{
						pSocket->hAddr = custTable[head->uId].getCust().getPubAddr();
						g_iocp.IOCP_SendTo(pSocket, pIO->wsaBuf);
					}
				}
				else if (head->type == PACKET_TYPE_CON_CUST)			////Connection Request
				{
				
					Log user;
					memcpy(&user, pIO->wsaBuf[1].buf, sizeof(Log));
					user.setPubAddr(pSocket->hAddr);
					SOCKADDR_IN tmp = user.getPrvAddr();
					tmp.sin_port = user.getPubAddr().sin_port;
					user.setPrvAddr(tmp);
					custTable[numOfUser++].addCust(user);				/////customer register
					if (numOfUser > sizeOfTable){
						custTable = (Customer*)realloc(custTable, sizeof(Customer)*(numOfUser+MAX_USER));
						sizeOfTable = numOfUser + MAX_USER;
					}
				}
				else if (head->type == PACKET_TYPE_CON_HOST)
				{
					Log* user = new Log();
					WSABUF wsaBufCust[2];
					WSABUF wsaBufHost[2];
					int index = -2;

					ZeroMemory(wsaBufCust, sizeof(wsaBufCust));
					ZeroMemory(wsaBufHost, sizeof(wsaBufHost));
					ZeroMemory(user, sizeof(Log));

					wsaBufCust[0].buf = (char*)&pIO->head;
					wsaBufCust[0].len = sizeof(pIO->head);
					wsaBufHost[0].buf = (char*)&pIO->head;
					wsaBufHost[0].len = sizeof(pIO->head);

					memcpy(user, pIO->wsaBuf[1].buf, sizeof(Log));

					index = searchCustomer(*user, custTable, numOfUser);
					
					if (index == -1) {
						
						pIO->head.type = CON_FAILED;
						g_iocp.IOCP_SendTo(pSocket, wsaBufHost);
					}	
					else
					{
						pIO->head.type = CON_SUCCESS;
						pIO->head.uId = index;

						user->setPubAddr(pSocket->hAddr);
						SOCKADDR_IN tmp = user->getPrvAddr();
						tmp.sin_port = user->getPubAddr().sin_port;
						user->setPrvAddr(tmp);
						custTable[index].setHost(user);

						wsaBufHost[1].buf = (char*)&custTable[index].getCust();
						wsaBufHost[1].len = sizeof(Log);
						wsaBufCust[1].buf = (char*)custTable[index].getHost();
						wsaBufCust[1].len = sizeof(Log);
						pSocket->hAddr = custTable[index].getHost()->getPubAddr();

						g_iocp.IOCP_SendTo(pSocket, wsaBufHost);					////send each infomation
						pSocket->hAddr = custTable[index].getCust().getPubAddr();
						g_iocp.IOCP_SendTo(pSocket, wsaBufCust);
						if (custTable[index].getCust().getMode() == MODE_P2P)		
						{
							ZeroMemory(&custTable[index], sizeof(Customer));	////clean the table
							numOfUser--;
							delete user;
						}
					}
					
				}
				/////////////////////////////////////////////////////////////////
				else
					AfxMessageBox(_T("IO Type Error..."));

				g_iocp.IOCP_RecvFrom(pSocket);
			}
			if (pIO != NULL)
				delete pIO;
		}
	}
	return 0;
}

void CIOCPRCSeverDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_udpSock = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_udpSock == INVALID_SOCKET)
	{
		MessageBox(_T("소켓 생성 실패!!!"));
		return;
	}
	
	if (bind(m_udpSock, (SOCKADDR*)&m_sockAddr, sizeof(m_sockAddr)) == SOCKET_ERROR)
	{
		MessageBox(_T("소켓 생성 실패!!!"));
		closesocket(m_udpSock);
		return;
	}
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (g_hIocp == NULL)
	{
		MessageBox(_T("CP 생성 실패!!"));
		closesocket(m_udpSock);
		return;
	}

	m_btStart.EnableWindow(false);
	flag = true;

	for (int i = 0; i < CORE; i++)
	{
		m_iocpThread[i] = ::AfxBeginThread(IOCPThread, NULL);
		m_iocpThread[i]->m_bAutoDelete = false;
		m_handles[i] = m_iocpThread[i]->m_hThread;
	}
	pSocket = new SOCKET_CONTEXT;
	pSocket->hSocket = m_udpSock;
	CreateIoCompletionPort((HANDLE)m_udpSock, g_hIocp, (ULONG_PTR)pSocket, CORE);
	//SOCKADDR_IN addr;

	g_iocp.IOCP_RecvFrom(pSocket);

}

int CIOCPRCSeverDlg::searchCustomer(const Log& user, Customer* table, int entity)
{
	for (int i = 0; i < entity; i++)
	{
		if (table[i].checkAuthority(user))
			return i;
	}

	return -1;
}


void CIOCPRCSeverDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//// Free memories
	flag = false;
	CloseHandle(g_hIocp);
	closesocket(m_udpSock);
	WSACleanup();

	if (pSocket != NULL)
		delete pSocket;
	if (custTable != NULL) {
		free(custTable);
	}
	
	

	DWORD dwExitCode=0;

	if (m_iocpThread != NULL) {
		for (int i = 0; i < CORE; i++) {
			::GetExitCodeThread(m_handles[i], &dwExitCode);
			if (dwExitCode != STILL_ACTIVE)
				delete m_iocpThread[i];
			else {
				::WaitForSingleObject(m_handles[i], INFINITE);
				delete m_iocpThread[i];
			}
		}
	}
	
	CDialogEx::OnClose();
}
