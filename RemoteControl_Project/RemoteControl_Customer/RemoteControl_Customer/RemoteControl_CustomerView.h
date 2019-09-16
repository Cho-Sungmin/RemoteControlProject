
// RemoteControl_CustomerView.h: CRemoteControlCustomerView 클래스의 인터페이스
//

#pragma once

#define ON_CONNECT WM_USER+1
#define ON_DISCONNECT WM_USER+2

#include "RemoteControl_CustomerDoc.h"
#include "CaptureScreen.h"
#include "rcprotocol.h"

class CRemoteControlCustomerView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CRemoteControlCustomerView() noexcept;
	DECLARE_DYNCREATE(CRemoteControlCustomerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_REMOTECONTROL_CUSTOMER_FORM };
#endif

// 특성입니다.
public:
	CRemoteControlCustomerDoc* GetDocument() const;

	CStatic *m_stat_id;
	CStatic *m_stat_pwd;
	CStatic *m_stat_run;
	CButton *m_bt_engage;
	CButton *m_bt_p2p;
	CButton *m_bt_relay;

	CString m_id, m_pw;
// 작업입니다.

public:
	void delCtrl();
	void setConnectView();
	void setRunningView();

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);


// 구현입니다.
public:
	virtual ~CRemoteControlCustomerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnClickedEngageBt();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisConnect(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // RemoteControl_CustomerView.cpp의 디버그 버전
inline CRemoteControlCustomerDoc* CRemoteControlCustomerView::GetDocument() const
   { return reinterpret_cast<CRemoteControlCustomerDoc*>(m_pDocument); }
#endif

