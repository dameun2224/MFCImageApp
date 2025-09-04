
// MFCImageAppDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCImageApp.h"
#include "MFCImageAppDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCImageAppDlg 대화 상자



CMFCImageAppDlg::CMFCImageAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCIMAGEAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCImageAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_pictureControl1);
}

BEGIN_MESSAGE_MAP(CMFCImageAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_UPLOAD, &CMFCImageAppDlg::OnClickedBtnUpload)
END_MESSAGE_MAP()


// CMFCImageAppDlg 메시지 처리기

BOOL CMFCImageAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCImageAppDlg::OnPaint()
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
HCURSOR CMFCImageAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCImageAppDlg::OnClickedBtnUpload()
{
	// 파일 열기 대화 상자
	CFileDialog dlg(TRUE, _T("*.png"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Image Files (*.bmp;*.jpg;*.gif;*.png)|*.bmp;*.jpg;*.gif;*.png|All Files (*.*)|*.*||"), this);

	// 사용자가 파일을 선택하고 [확인]을 누르면
	if (dlg.DoModal() == IDOK)
	{
		CString strFilePath = dlg.GetPathName();

		// GDI+를 사용해 이미지 로드
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(strFilePath);

		if (pBitmap->GetLastStatus() == Gdiplus::Ok)
		{
			CImage image;
			HBITMAP hBmp = NULL;
			pBitmap->GetHBITMAP(NULL, &hBmp);
			image.Attach(hBmp);

			CRect rect;
			m_pictureControl1.GetClientRect(&rect);

			// Picture Control에 이미지 그리기
			CClientDC dc(&m_pictureControl1);
			image.StretchBlt(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), SRCCOPY);

			image.Detach(); // CImage에서 HBITMAP을 분리하여 직접 관리
		}

		delete pBitmap;
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
}
