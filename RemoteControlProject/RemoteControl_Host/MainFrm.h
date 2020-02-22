
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include <rcprotocol.h>
#include <DataQueue.h>
#include "RemoteControlView.h"
#include "CModaless.h"
#include "CScreenView.h"

#define GLOBAL_MEMORY_SIZE 1024 * 1024

typedef VOID(CALLBACK* PFN_MOUSE_HOOKSTART)(DWORD);
typedef VOID(CALLBACK* PFN_MOUSE_HOOKSTOP)(VOID);

typedef VOID(CALLBACK* PFN_KB_HOOKSTART)(DWORD);
typedef VOID(CALLBACK* PFN_KB_HOOKSTOP)(VOID);

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
private:
	
public:
// 작업입니다.
	CRemoteControlView* m_pRcView;
	CScreenView* m_pScView;

	WSADATA m_wsaData;
	SOCKADDR_IN m_udpServAddr, m_udpClntAddr;
	SOCKET m_udpSock;
	RC_Param param;
	static CCriticalSection st_cs;
	static int st_ackNum;

	static DataQueue<Image_Packet> st_que;
	CWinThread *m_pRecvThread, *m_pImgThread;
	static bool st_ImgThreadFlag, st_recvThreadFlag;
	static HANDLE st_hMutex;

	HMODULE m_hDll;
	PFN_MOUSE_HOOKSTART mouseHookStart;
	PFN_MOUSE_HOOKSTOP mouseHookStop;
	PFN_KB_HOOKSTART kbHookStart;
	PFN_KB_HOOKSTOP kbHookStop;

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
	static UINT Recv_Thread_Func(LPVOID param);
	static UINT Img_Thread_Func(LPVOID param);
	

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar        m_wndStatusBar;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnThread(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScreen(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

public:

	virtual BOOL DestroyWindow();
};


