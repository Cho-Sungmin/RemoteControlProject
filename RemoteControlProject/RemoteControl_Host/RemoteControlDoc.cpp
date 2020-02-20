﻿
// RemoteControlDoc.cpp: CRemoteControlDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "RemoteControl.h"
#endif

#include "RemoteControlDoc.h"
#include "MainFrm.h"


#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRemoteControlDoc

IMPLEMENT_DYNCREATE(CRemoteControlDoc, CDocument)

BEGIN_MESSAGE_MAP(CRemoteControlDoc, CDocument)
END_MESSAGE_MAP()


// CRemoteControlDoc 생성/소멸

CRemoteControlDoc::CRemoteControlDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_randID = rand()%999999999 + 100000000;
	m_randPW = rand()%999999999 + 100000000;

}

CRemoteControlDoc::~CRemoteControlDoc()
{
}

BOOL CRemoteControlDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	//CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
	//CRemoteControlView *pView = (CRemoteControlView *)pMain->GetActiveView();

	//::SendMessage(pView->GetSafeHwnd(), ON_NEWDOC, 0, 0);

	return TRUE;
}


// CRemoteControlDoc serialization

void CRemoteControlDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.

	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CRemoteControlDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CRemoteControlDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CRemoteControlDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CRemoteControlDoc 진단

#ifdef _DEBUG
void CRemoteControlDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRemoteControlDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRemoteControlDoc 명령

int CRemoteControlDoc::getRandID()
{
	return m_randID;
}
int CRemoteControlDoc::getRandPW()
{
	return m_randPW;
}