
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once

#include "CaptureScreen.h"
#include "rcprotocol.h"

#define ON_THREAD WM_USER+1
#define ON_HOOKMOUSE WM_USER+2
#define ON_HOOKKB WM_USER+3


class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	WSADATA m_wsaData;
	SOCKET m_udpSock;
	SOCKADDR_IN m_udpAddr;
	int m_clntLen;
	RC_Param m_param;
	
	static int st_uId;

	static CaptureScreen st_cap;
	CWinThread *m_pSend_thread;
	CWinThread *m_pMouse_thread;
	static bool st_sendThreadFlag;

// 작업입니다.
public:
	

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar        m_wndStatusBar;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnThread(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	// 스레드 함수
public:
	static UINT Send_Thread_Func(LPVOID param);
	static UINT Mouse_Thread_Func(LPVOID param);
};


