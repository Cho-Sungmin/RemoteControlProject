
// RemoteControlView.h: CRemoteControlView 클래스의 인터페이스
//

#pragma once
#include "RemoteControlDoc.h"

#include "CPassWordDlg.h"


#define ON_DISPLAY_IMG WM_USER+2
#define ON_NEWDOC WM_USER+3


class CRemoteControlView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CRemoteControlView() noexcept;
	DECLARE_DYNCREATE(CRemoteControlView)

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW };
#endif
// 특성입니다.
public:
	CRemoteControlDoc* GetDocument() const;

// 작업입니다.
public:
	CRect viewRect;
	HDC hdc;
	CButton m_bt_cap;
	CButton m_bt_p2p;
	CButton m_bt_relay;
	CEdit m_edit_ID;
	CEdit m_edit_pwd;
	CEdit m_edit_PID;
	CStatic m_stat_pwd;
	CStatic m_stat_ID;
	CStatic m_stat_PID;

	CPassWordDlg *pPassDlg;

// 재정의입니다.
public:
	//virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
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
	virtual ~CRemoteControlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnDisplayImg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStartBtClicked();

};

#ifndef _DEBUG  // RemoteControlView.cpp의 디버그 버전
inline CRemoteControlDoc* CRemoteControlView::GetDocument() const
   { return reinterpret_cast<CRemoteControlDoc*>(m_pDocument); }
#endif

