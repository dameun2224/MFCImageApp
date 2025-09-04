
// MFCImageAppDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCImageApp.h"
#include "MFCImageAppDlg.h"
#include "afxdialogex.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCImageAppDlg 대화 상자


/* 생성자 */
// CDialogEx 초기화 및 기본 아이콘 로드
CMFCImageAppDlg::CMFCImageAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCIMAGEAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/* DDX(Dialog Data Exchange, Control) */
void CMFCImageAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_pictureControl1);
}

/* 메시지 맵 */ 
BEGIN_MESSAGE_MAP(CMFCImageAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_UPLOAD, &CMFCImageAppDlg::OnClickedBtnUpload)
END_MESSAGE_MAP()


/* OnInitDialog: 다이얼로그 초기화 함수 */
BOOL CMFCImageAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


/* OnPaint: 그리기 처리 함수 */ 
// 운영 체제가 창을 다시 그려야 할 때 호출하는 함수, 사용자는 호출하지 않음
void CMFCImageAppDlg::OnPaint()
{
	CDialogEx::OnPaint();
}

// HCURSOR CMFCImageAppDlg::OnQueryDragIcon() - 관련 내용 모두 삭제함
// Windows(95, 98, XP 시절)에서는 프로그램 창을 최소화하면 바탕화면 안쪽에 작은 아이콘 모양으로 표시됨.
// Windows 10, 11 같은 최신 버전에서는 최소화된 창이 바탕화면에 아이콘으로 남지 않고, 작업 표시줄 버튼으로만 남아서 사용되지 않음

#include <atlimage.h> // ← 추가


void CMFCImageAppDlg::OnClickedBtnUpload()
{
    // PNG/JPG/BMP 모두 선택 가능
	// CFileDialog: Common File Dialog, 파일 열기/저장 대화상자
	// TRUE: 열기, FALSE: 저장 / 기본 확장자 / 초기 파일명 / 옵션 / 필터 / 부모 윈도우
    CFileDialog dlg(TRUE, _T("png"), nullptr, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Image Files (*.bmp;*.jpg;*.jpeg;*.png)|*.bmp;*.jpg;*.jpeg;*.png|All Files (*.*)|*.*||"), this);

    if (dlg.DoModal() == IDOK) {
        CString path = dlg.GetPathName();

        // 1) PNG/JPG/BMP 로드
        CImage img;
        HRESULT hr = img.Load(path);    // 함수 실행 결과 (HRESULT, an error-handling data type)
        if (FAILED(hr)) {
            AfxMessageBox(_T("이미지 로드에 실패했습니다."));
            return;
        }

        // 2) HBITMAP로 변환하여 GDI로 처리
        HBITMAP hBmp = (HBITMAP)img.Detach();
        if (!hBmp) {
            AfxMessageBox(_T("HBITMAP 변환에 실패했습니다."));
            return;
        }

        // Picture Control DC
        CClientDC dc(&m_pictureControl1);
        CRect rc;
        m_pictureControl1.GetClientRect(&rc);

        // 메모리 DC 만들고 비트맵 선택
        CDC memDC;
        memDC.CreateCompatibleDC(&dc);
        HBITMAP hOld = (HBITMAP)memDC.SelectObject(hBmp);

        // 원본 크기 얻기
        BITMAP bm = {};
		::GetObject(hBmp, sizeof(bm), &bm); // 전역 namespace에 있는 GetObject 호출 (MFC에도 GetObject 함수가 있어서)

        // 화질 개선 + 컨트롤 크기에 맞게 스트레치 출력
        SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);
        SetBrushOrgEx(dc.GetSafeHdc(), 0, 0, NULL);
        StretchBlt(dc.GetSafeHdc(), 0, 0, rc.Width(), rc.Height(), memDC.GetSafeHdc(), 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        // 정리
        // GDI 규칙: DC에 뭔가(SelectObject)를 붙였으면, 나중에 원래 걸려있던 걸 다시 돌려놔야 함
        memDC.SelectObject(hOld);
        DeleteObject(hBmp); 
    }
}
