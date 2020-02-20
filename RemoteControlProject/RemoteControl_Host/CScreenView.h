#pragma once


// CScreenView 보기

class CScreenView : public CView
{
	DECLARE_DYNCREATE(CScreenView)
public:
	HDC m_hdc;
	static int st_uId;
	CScreenView();           // 동적 만들기에 사용되는 protected 생성자입니다.
protected:
	
	virtual ~CScreenView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDisplayImg(WPARAM wParam, LPARAM lParam);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnMouseHook(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKBHook(WPARAM wParam, LPARAM lParam);
};


