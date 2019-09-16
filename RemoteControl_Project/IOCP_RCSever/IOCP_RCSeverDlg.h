
// IOCP_RCSeverDlg.h: 헤더 파일
//

#pragma once

#define CORE 4
// CIOCPRCSeverDlg 대화 상자
class CIOCPRCSeverDlg : public CDialogEx
{
// 생성입니다.
public:
	CIOCPRCSeverDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IOCP_RCSEVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btStart;
	CButton m_btEnd;
	SOCKET m_udpSock;
	SOCKADDR_IN m_sockAddr;
	WSADATA wsaData;
	SOCKET_CONTEXT* pSocket;
	CWinThread* m_iocpThread[CORE];
	HANDLE m_handles[CORE];

	static IOCP_IO g_iocp;
	static HANDLE g_hIocp;
	static Customer* custTable;
	static int numOfUser;
	static int sizeOfTable;
	static bool flag;

	static int searchCustomer(const Log& user, Customer* table, int entity);
	static UINT IOCPThread(LPVOID param);

	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};

