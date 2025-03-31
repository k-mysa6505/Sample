
// SampleDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "Sample.h"
#include "SampleDlg.h"
#include "afxdialogex.h"
#include "CHistDlg.h"
#include <vector>
#include <time.H>
#include <string>
#include <opencv2/opencv.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	カメラの解像度
#define HEIGHT	240//縦幅
#define WIDTH	320//横幅

#define ICEIL(dividend, divisor) (((dividend) + ((divisor) - 1)) / (divisor))
#define BPP 24

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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

// CSampleDlg ダイアログ
//	コンストラクタ
CSampleDlg::CSampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SAMPLE_DIALOG, pParent)
	, m_eth(0)
	, m_sth(0)
	, m_rate(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//メモリエラー対策
	m_BmpImage = NULL;
	m_BmpInfo = NULL;
	m_BmpFileHdr = {};
	m_Image = NULL;
	tmpBmpImage = NULL;
	tmpBmpInfo = NULL;
	tmpBmpFileHdr = {};
	tmpImage = NULL;

	m_OrgImage = NULL;
	m_OrgBmpFileHdr = {};
	m_OrgBmpInfo = {};
	tmpOrgImage = NULL;
	tmpOrgBmpFileHdr = {};
	tmpOrgBmpInfo = {};

	radio1 = {};
	radio2 = {};
	radio3 = {};
	radio4 = {};

	check1 = {};

	m_HistDlg = NULL;

	max_label = 1;
	m_LabelMap = NULL;
	m_LookupTable = NULL;

	capture = NULL;
	input = NULL;
	savedata = NULL;
	I1 = NULL;
	I2 = NULL;
	I3 = NULL;
	img_sp = 0;
}

//デストラクタ
CSampleDlg::~CSampleDlg()
{
	//メモリ解放
	if (m_BmpImage) {
		delete[] m_BmpImage;
	}

	if (m_Image) {
		delete[] m_Image;
	}

	if (m_OrgImage) {
		delete[] m_OrgImage;
	}
}

void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_eth);
	DDX_Slider(pDX, IDC_SLIDER_THRESHOLD, m_sth);
	DDX_Text(pDX, IDC_EDIT_RATE, m_rate);
}

BEGIN_MESSAGE_MAP(CSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &CSampleDlg::OnOpen)
	ON_BN_CLICKED(IDC_SAVE, &CSampleDlg::OnSave)
	ON_BN_CLICKED(IDC_KIDOKA, &CSampleDlg::OnKidoka)
	ON_BN_CLICKED(IDC_2CHIKA, &CSampleDlg::On2chika)
	ON_EN_CHANGE(IDC_EDIT_THRESHOLD, &CSampleDlg::OnEnChangeEditThreshold)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_THRESHOLD, &CSampleDlg::OnNMReleasedcaptureSliderThreshold)
	ON_BN_CLICKED(IDC_BACK, &CSampleDlg::OnBack)
	ON_BN_CLICKED(IDC_MEDIAN, &CSampleDlg::OnMedian)
	ON_BN_CLICKED(IDC_AVERAGE, &CSampleDlg::OnMoveAverage)
	ON_BN_CLICKED(IDC_EDGE, &CSampleDlg::OnEdgeDetection)
	ON_BN_CLICKED(IDC_HIST, &CSampleDlg::OnShowNewDialog)
	ON_BN_CLICKED(IDC_LINEAR, &CSampleDlg::OnLinearDensity)
	ON_BN_CLICKED(IDC_FLATTEN, &CSampleDlg::OnFlatten)
	ON_BN_CLICKED(IDC_COMP, &CSampleDlg::OnCompletelyFlatten)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CALLTIMER, &CSampleDlg::OnCallTimer)
	ON_BN_CLICKED(IDC_TEMPLATE_MATCHING, &CSampleDlg::OnTemplateMatching)
	ON_BN_CLICKED(IDC_DISSIMIRALITY, &CSampleDlg::OnDissimirality)
	ON_BN_CLICKED(IDC_LABELING, &CSampleDlg::OnLabeling)
	ON_BN_CLICKED(IDC_CAMERA, &CSampleDlg::OnCamera)
	ON_BN_CLICKED(IDC_VIDEO, &CSampleDlg::OnVideoCamera)
	ON_BN_CLICKED(IDC_FACE, &CSampleDlg::OnDitectFaces)
	ON_BN_CLICKED(IDC_NEAREST, &CSampleDlg::OnNearestNeighbor)
	ON_BN_CLICKED(IDC_LINEAR_INTER, &CSampleDlg::OnLinearInter)
	ON_BN_CLICKED(IDC_NEW_TEMPLATE_MATCHING, &CSampleDlg::OnNewTemplateMatching)
	ON_BN_CLICKED(IDC_BACKDIFF, &CSampleDlg::OnBackgroundDifference)
	ON_BN_CLICKED(IDC_FRAMEDIFF, &CSampleDlg::OnFrameDifference)
	ON_BN_CLICKED(IDC_EXPENSION, &CSampleDlg::OnExpension)
	ON_BN_CLICKED(IDC_CONTRACTION, &CSampleDlg::OnContraction)
	ON_BN_CLICKED(IDC_SKIN, &CSampleDlg::OnSkinExtraction)
	ON_BN_CLICKED(IDC_CONTOUR_TRACING, &CSampleDlg::OnContourTracing)
	ON_BN_CLICKED(IDC_HOUGH, &CSampleDlg::OnHoughTramsform)
	ON_BN_CLICKED(IDC_AFFINE, &CSampleDlg::OnAffine)
END_MESSAGE_MAP()

// CSampleDlg メッセージ ハンドラー
BOOL CSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_THRESHOLD))->SetRange(0, 255); //スライダーの範囲(0-255)

	//ラジオボタンのコントロール変数の設定
	radio1 = (CButton*)GetDlgItem(IDC_RADIO1);
	radio1->SetCheck(1); //初期化状態「ON」
	radio2 = (CButton*)GetDlgItem(IDC_RADIO2);
	radio3 = (CButton*)GetDlgItem(IDC_RADIO3);
	radio4 = (CButton*)GetDlgItem(IDC_RADIO4);

	//チェックボックスのコントロール変数設定
	check1 = (CButton*)GetDlgItem(IDC_CHECK1);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。
void CSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	//	画像描画関数
	DrawImage();
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSampleDlg::OnOpen()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	// MessageBox("ボタンテスト", "確認", MB_OK);
		//BMPファイルの読み込み
	//変数の宣言
	int i, j;
	size_t image_size;
	CString filename;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;

	//----------------------------------------------------------ファイル操作ダイアログの呼び出し処理

	//ファイルダイアログを表示してファイル指定する
	CFileDialog myDLG(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ﾋﾞｯﾄﾏｯﾌﾟ(*.BMP)|*.BMP||");
	if (myDLG.DoModal() != IDOK) return;

	//取得したファイルをオープンする
	filename = myDLG.GetPathName();
	if (!file.Open(filename, CFile::modeRead | CFile::typeBinary)) {
		return;
	}

	//タイトルバーにファイル名を表示する
	SetWindowText(filename);

	//----------------------------------------------------------ファイル内容の読み込み処理

	//ファイルヘッダ部とインフォヘッダ部を読み込む
	file.Read(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));

	// 前回の画像イメージをいったん解放
	if (m_BmpImage) delete[] m_BmpImage;

	// 前回使用していれば、いったんメモリ解放する
	if (m_BmpInfo) delete[] m_BmpInfo;

	// 前回の画像処理用メモリをいったん解放
	if (m_Image) delete[] m_Image;

	//	いったんメモリの開放
	if (m_OrgImage) delete[] m_OrgImage;

	//色情報を取得する
	//biBitCountは1ピクセルあたりのカラー表現ビット数
	//1,4,8,16,32がある。数字が大きいほど精細表現が可能
	//16ビット以上と未満でカラーデータ格納が異なる
	if (myBmpInfoHdr.biBitCount >= 16) {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	}
	else {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
		file.Read(m_BmpInfo->bmiColors,
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	}

	//m_BmpInfo(LPBITMAPINFO型)のmyBmpInfoHdrメンバに設定
	m_BmpInfo->bmiHeader = myBmpInfoHdr;

	//ファイル内のビットマップ実データ位置に合わせる
	file.Seek(m_BmpFileHdr.bfOffBits, CFile::begin);

	// 実画像データ分のバイト数を確保
	// bfsize	:	ビットマップファイル全サイズ
	// bfffBits	:	先頭にあるヘッダ情報サイズ
	image_size = m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits;
	m_BmpImage = new unsigned char[image_size];

	// ビットマップ実データを読み込み格納
	file.Read(m_BmpImage, (UINT)image_size);

	file.Close();

	//画像処理用のメモリを動的に確保
	m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	//RGBに分けて格納する
	if (myBmpInfoHdr.biBitCount == 24) {	//フルカラーの場合
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->B[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
				m_Image->G[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
				m_Image->R[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
			}
		}
	}
	else if (myBmpInfoHdr.biBitCount == 32) {
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->B[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 4];
				m_Image->G[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 4 + 1];
				m_Image->R[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 4 + 2];
			}
		}
	}
	else {
	}

	//メモリの確保
	m_OrgImage = new unsigned char[image_size];
	//メモリのコピー
	memcpy(m_OrgImage, m_BmpImage, image_size);
	//ファイルヘッダ・インフォの保存
	m_OrgBmpFileHdr = m_BmpFileHdr;
	m_OrgBmpInfo = *m_BmpInfo;

	//----------------------------------------------------------再描写指示

	//再描写指示
	Invalidate();
}

void CSampleDlg::DrawImage()
{
	// TODO: ここに実装コードを追加します.
		//画像描写処理
	HBITMAP myDIB;	//ビットマップのハンドル
	HBITMAP oldDIB;
	HDC hDC;		//デバイスコンテキストのハンドル
	HDC hCompatiDC;	//メモリデバイスコンテキスト
	PAINTSTRUCT ps;	//クライアント領域の描写に使う情報を保持

	CBitmap myBMP;

	//画像の準備が出来ていなければリターン
	if (!m_BmpInfo)	return;

	//描画先のデバイスコンテキストを得る
	//hDC = ::GetDC(m_hWnd);
	hDC = ((CStatic *)GetDlgItem(/*13-②で指定したピクチャボックスのID*/IDC_PICTBOX))->GetDC()->GetSafeHdc();

	//描写の準備をする
	//描写が終了したら必ずEndPaintを実行する必要がある
	::BeginPaint(m_hWnd, &ps);

	//hDCと互換性のあるメモリデバイスコンテキストを作成する
	hCompatiDC = ::CreateCompatibleDC(hDC);

	//DIB形式ビットマップからDDB(デバイス依存)形式ビットマップを作成する
	myDIB = ::CreateDIBitmap(hDC, &m_BmpInfo->bmiHeader,
		CBM_INIT, m_BmpImage, m_BmpInfo, DIB_RGB_COLORS);

	//hCompatiDCでmyDIBの示すオブジェクトを使用できるようにする
	//オリジナルビットマップをoldDIBに保存しておき後で復活させる
	oldDIB = (HBITMAP)::SelectObject(hCompatiDC, myDIB);

	//hCompatiDCにある画像をhDCに表示する
	BitBlt(hDC, 0, 0, m_BmpInfo->bmiHeader.biWidth, m_BmpInfo->bmiHeader.biHeight,
		hCompatiDC, 0, 0, SRCCOPY);

	//BeginPaint対応処理
	::EndPaint(m_hWnd, &ps);

	//オブジェクトの開放など
	::SelectObject(hCompatiDC, oldDIB);
	::DeleteObject(myDIB);
	::DeleteObject(hCompatiDC);
	::ReleaseDC(m_hWnd, hDC);
}

void CSampleDlg::OnSave()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
		//画像保存処理
	CString filename;
	CFile file;

	//フルカラー以外の場合は省略（減色処理が必要になるので）
	if (m_BmpInfo->bmiHeader.biBitCount != 24) {
		MessageBox("フルカラーでない画像は保存できません", "がんばれ", MB_OK);
		return;
	}

	//ファイルダイアログを表示してファイル指定する
	CFileDialog myDLG(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ﾋﾞｯﾄﾏｯﾌﾟ(*.BMP)|*.BMP||");

	if (myDLG.DoModal() != IDOK) return;

	//取得したファイルをオープンする
	filename = myDLG.GetPathName() + ".bmp";

	if (!file.Open(filename, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary)) {
		MessageBox((CString)"ファイル" + filename + "の書き込みに失敗しました", "エラー", MB_OK);
		return;
	}

	//ファイル内容の書き込み
	file.Write(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Write(&m_BmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
	file.Write(m_BmpImage, m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits);

	file.Close();
}

void CSampleDlg::AutoSave(CString filename) {
	//画像保存処理
	CFile file;

	//フルカラー以外の場合は省略（減色処理が必要になるので）
	if (m_BmpInfo->bmiHeader.biBitCount != 24) {
		MessageBox("フルカラーでない画像は保存できません", "がんばれ", MB_OK);
		return;
	}

	if (!file.Open(filename, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary)) {
		MessageBox((CString)"ファイル" + filename + "の書き込みに失敗しました", "エラー", MB_OK);
		return;
	}

	//ファイル内容の書き込み
	file.Write(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Write(&m_BmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
	file.Write(m_BmpImage, m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits);

	file.Close();
}

void CSampleDlg::OnKidoka()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int i, j;
	double dy;

	//輝度化処理
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			dy = 0.3 * m_Image->R[i][j] + 0.59 * m_Image->G[i][j] + 0.11 * m_Image->B[i][j];
			m_Image->R[i][j] = (unsigned char)dy;
			m_Image->G[i][j] = (unsigned char)dy;
			m_Image->B[i][j] = (unsigned char)dy;
		}
	}

	//編集した内容を表示用バッファに戻す
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

void CSampleDlg::On2chika()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int i, j;
	double dy;

	
	//	2値化処理
	//	30 70 120 <- 提出用の閾値
	/*
	int t = 30;
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			dy = m_Image->R[i][j];
			if (dy < t) {
				m_Image->R[i][j] = 0;
				m_Image->G[i][j] = 0;
				m_Image->B[i][j] = 0;
			} else {
				m_Image->R[i][j] = 255;
				m_Image->G[i][j] = 255;
				m_Image->B[i][j] = 255;
			}
		}
	}
	*/
	
	
	//	2値化処理（判別分析法）
	int	   t, bt = 0;	//	閾値, ベストな閾値（結果は74）
	double v_max = 0;	//	クラス間分散の最大値
	for (t = 1; t <= 255; t++) {
		double  v = 0;			//	クラス間分散
		int    nb = 0, nw = 0;	//	黒クラス・白クラスの画素数
		double mb = 0, mw = 0;	//	黒クラス・白クラスの平均輝度値
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				dy = m_Image->R[i][j];
				if (dy < t) {
					nb++;
					mb += dy;
				} else {
					nw++;
					mw += dy;
				}
			}
		}
		mb /= nb;
		mw /= nw;
		v = nb * nw * (mb - mw) * (mb - mw);
		if (v > v_max) {
			v_max = v;
			bt = t;
		}
	}

	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			dy = m_Image->R[i][j];
			if (dy < bt) {
				m_Image->R[i][j] = 0;
				m_Image->G[i][j] = 0;
				m_Image->B[i][j] = 0;
			} else {
				m_Image->R[i][j] = 255;
				m_Image->G[i][j] = 255;
				m_Image->B[i][j] = 255;
			}
		}
	}

	/*
	//	２値化処理（k-means法）
	srand(time(nullptr));
	int centers[2] = {64, 192};
	int *pixelClusters = new int[m_BmpInfo->bmiHeader.biHeight * m_BmpInfo->bmiHeader.biWidth];
	int iterCount = 0, maxIter = 100;
	bool changed;

	do {
		changed = false;

		//	各ピクセルを最も近いクラスタに割り当て
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				pixelClusters[i * m_BmpInfo->bmiHeader.biWidth + j] = 0;

				dy = m_Image->R[i][j];
				int bestCluster = 0;
				int minDist = dy - centers[0];
				minDist *= (minDist >= 0) ? 1 : -1;

				for (int c = 1; c < 2; c++) {
					int dist = dy - centers[c];
					dist *= (dist >= 0) ? 1 : -1;
					if (dist < minDist) {
						minDist = dist;
						bestCluster = c;
					}
				}

				int idx = i * m_BmpInfo->bmiHeader.biWidth + j;
				if (pixelClusters[idx] != bestCluster) {
					changed = true;
					pixelClusters[idx] = bestCluster;
				}
			}
		}

		//	クラスタ中心を更新
		long long sums[2] = { 2, 0 };
		int counts[2] = { 2, 0 };
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				int cluster = pixelClusters[i * m_BmpInfo->bmiHeader.biWidth + j];
				sums[cluster] += dy;
				counts[cluster]++;
			}
		}

		for (int c = 0; c < 2; c++) {
			if (counts[c] > 0) {
				centers[c] = sums[c] / counts[c];
			}
			else {
				centers[c] = (c == 0) ? 64 : 192;
			}
		}

		iterCount++;
	} while (changed && iterCount < maxIter);

	int darkCluster = (centers[0] < centers[1]) ? 0 : 1;

	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			int cluster = pixelClusters[i * m_BmpInfo->bmiHeader.biWidth + j];
			if (cluster == darkCluster) {
				m_Image->R[i][j] = 0;
				m_Image->G[i][j] = 0;
				m_Image->B[i][j] = 0;
			}
			else {
				m_Image->R[i][j] = 255;
				m_Image->G[i][j] = 255;
				m_Image->B[i][j] = 255;
			}
		}
	}

	free(pixelClusters);
	*/

	//	２値化処理（p－タイル法）
	/*
	long long all_pixels = m_BmpInfo->bmiHeader.biHeight * m_BmpInfo->bmiHeader.biWidth;
	double p = 0.50;	//	割合

	int sum_c = 0;
	int t = 0;
	for (int c = 0; c < 256; c++) {
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				dy = m_Image->R[i][j];
				if (dy == c) {
					sum_c++;
					if (sum_c < all_pixels * p) {
						t = c;
					}
				}
			}
		}
	}
	
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			dy = m_Image->R[i][j];
			if (dy < t) {
				m_Image->R[i][j] = 0;
				m_Image->G[i][j] = 0;
				m_Image->B[i][j] = 0;
			}
			else {
				m_Image->R[i][j] = 255;
				m_Image->G[i][j] = 255;
				m_Image->B[i][j] = 255;
			}
		}
	}
	*/

	//編集した内容を表示用バッファに戻す
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

void CSampleDlg::OnBack()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	//	処理後の画像情報を元画像情報に戻す
	int i, j;

	//	ビットマップファイルヘッダ・インフォ元画像に戻す
	m_BmpFileHdr = m_OrgBmpFileHdr;
	*m_BmpInfo = m_OrgBmpInfo;

	//	元画像メモリを表示用画像メモリにコピーする
	memcpy(m_BmpImage, m_OrgImage, m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits);

	for (i = 0; i < m_Image->Height; i++) {
		for (j = 0; j < m_Image->Width; j++) {
			m_Image->B[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
			m_Image->G[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
			m_Image->R[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
		}
	}

	//	再描画
	Invalidate();
}

void CSampleDlg::OnEnChangeEditThreshold()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialogEx::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を関数し呼び出します。
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
	/*
	OnBack(); //画像を元に戻す
	UpdateData(TRUE); //エディットボックスからメンバ変数（DOX変数）に代入
	On2chika(); //二値化
	//エディットボックスに何も入力していないときに，メッセージを表示しないようにする
	OnBack(); //画像を元に戻す
	*/
	if(GetDlgItem(IDC_EDIT_THRESHOLD)->GetWindowTextLength() == 0){
		m_eth = 0;
	}
	else{
		UpdateData(TRUE);
	}
	m_sth = m_eth;
	UpdateData(FALSE);
	On2chika(); //二値化
}

void CSampleDlg::OnNMReleasedcaptureSliderThreshold(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
	//第2回_05追加分：変数の書き換え(スライダー)
	OnBack(); //画像を元に戻す
	UpdateData(TRUE); //スライダーからメンバ変数（DOX変数）に代入

	m_eth = m_sth;
	UpdateData(FALSE);
	On2chika(); //二値化
}

int CSampleDlg::MedianFilter(int _x, int _y, int size) {
	int* pixel_vals = new int[size * size];
	int  pixel_cnt = 0;

	int w = m_BmpInfo->bmiHeader.biWidth;
	int h = m_BmpInfo->bmiHeader.biHeight;

	//	フィルタ内の画素値を集計
	for (int y = _y - (size - 1) / 2; y < _y + (size + 1) / 2; y++) {
		for (int x = _x - (size - 1) / 2; x < _x + (size + 1) / 2; x++) {
			if (x < 0 || x >= w || y < 0 || y >= h) {
				continue;
			}
			pixel_vals[pixel_cnt] = m_Image->R[y][x];
			pixel_cnt++;
		}
	}

	//	昇順にソート
	for (int i = 0; i < pixel_cnt - 1; i++) {
		for (int j = pixel_cnt - 1; j > i; j--) {
			if (pixel_vals[j - 1] > pixel_vals[j]) {
				int tmp = pixel_vals[j];
				pixel_vals[j] = pixel_vals[j - 1];
				pixel_vals[j - 1] = tmp;
			}
		}
	}

	//	中央値を返す
	int ret = pixel_vals[(pixel_cnt - 1) / 2];

	delete[] pixel_vals;

	return ret;
}

void CSampleDlg::OnMedian()
{
	//	メディアンフィルタ
	int filter_size = 7;	//	3*3, 5*5, 7*7
	int filter_cnt = m_eth;	//	1, 5, 10, 30, 100回

	Image* after_img = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	for (int i = 0; i < filter_cnt; i++) {
		for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
			for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
				int after_val = MedianFilter(x, y, filter_size);
				after_img->R[y][x] = after_val;
				after_img->G[y][x] = after_val;
				after_img->B[y][x] = after_val;
			}
		}
		//	これやるとなぜかうまくいかない
		//	m_Image = after_img;
		for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->R[i][j] = after_img->R[i][j];
				m_Image->G[i][j] = after_img->G[i][j];
				m_Image->B[i][j] = after_img->B[i][j];
			}
		}
  	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

int CSampleDlg::MoveAverageFilter(int _x, int _y, int size) {
	int* pixel_vals = new int[size * size];
	int  pixel_cnt = 0;
	int  pixel_sum = 0;

	int w = m_BmpInfo->bmiHeader.biWidth;
	int h = m_BmpInfo->bmiHeader.biHeight;

	//	フィルタ内の画素値を集計
	for (int y = _y - (size - 1) / 2; y < _y + (size + 1) / 2; y++) {
		for (int x = _x - (size - 1) / 2; x < _x + (size + 1) / 2; x++) {
			if (x < 0 || x >= w || y < 0 || y >= h) {
				continue;
			}
			pixel_vals[pixel_cnt] = m_Image->R[y][x];
			pixel_cnt++;
		}
	}

	for (int i = 0; i < pixel_cnt; i++) {
		pixel_sum += pixel_vals[i];
	}

	delete[] pixel_vals;

	//	平均値を返す
	return pixel_sum / pixel_cnt;
}

void CSampleDlg::OnMoveAverage()
{
	//	移動平均法
	int filter_size = 7;	//	3*3, 5*5, 7*7
	int filter_cnt = m_eth;	//	1, 5, 10, 30, 100回

	Image* after_img = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	for (int i = 0; i < filter_cnt; i++) {
		for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
			for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
				int after_val = MoveAverageFilter(x, y, filter_size);
				after_img->R[y][x] = after_val;
				after_img->G[y][x] = after_val;
				after_img->B[y][x] = after_val;
			}
		}
		for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->R[i][j] = after_img->R[i][j];
				m_Image->G[i][j] = after_img->G[i][j];
				m_Image->B[i][j] = after_img->B[i][j];
			}
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//	フィルタ計算
int CSampleDlg::Filter2D(int _y, int _x, int op[][3]) const {
	int w = m_BmpInfo->bmiHeader.biWidth;
	int h = m_BmpInfo->bmiHeader.biHeight;

	//	畳み込み演算
	int conv = 0;
	for (int y = _y - 1; y <= _y + 1; y++) {
		for (int x = _x - 1; x <= _x + 1; x++) {
			if (x < 0 || x >= w || y < 0 || y >= h) {
				continue;
			}
			conv += m_Image->R[y][x] * op[y - _y + 1][x - _x + 1];
		}
	}

	return conv;
}

//	エッジ検出
void CSampleDlg::OnEdgeDetection()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	//	輝度化
	OnKidoka();

	//	プリューウィットフィルタ
	int prewitt_operator_x[3][3] = {
		{-1, 0, 1},
		{-1, 0, 1},
		{-1, 0, 1}
	};
	int prewitt_operator_y[3][3] = {
		{-1, -1, -1},
		{ 0,  0,  0},
		{ 1,  1,  1}
	};
	//	ソーベルフィルタ
	int sobel_operator_x[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};
	int sobel_operator_y[3][3] = {
		{-1, -2, -1},
		{ 0,  0,  0},
		{ 1,  2,  1}
	};
	//	ラプラシアンフィルタ
	int laplacian_operator[3][3] = {
		{ 0,  1, 0},
		{ 1, -4, 1},
		{ 0,  1, 0}
	};
	//	Kirschマスク
	int kirsch_mask[8][3][3] = {
		{
			{-3, -3,  5},
			{-3,  0,  5},
			{-3, -3,  5}
		}, {
			{-3, -3, -3},
			{-3,  0,  5},
			{-3,  5,  5}
		}, {
			{-3, -3, -3},
			{-3,  0, -3},
			{ 5,  5,  5}
		}, {
			{-3, -3, -3},
			{ 5,  0, -3},
			{ 5,  5, -3}
		}, {
			{ 5, -3, -3},
			{ 5,  0, -3},
			{ 5, -3, -3}
		}, {
			{ 5,  5, -3},
			{ 5,  0, -3},
			{-3, -3, -3}
		}, {
			{ 5,  5,  5},
			{-3,  0, -3},
			{-3, -3, -3}
		}, {
			{-3,  5,  5},
			{-3,  0,  5},
			{-3, -3, -3}
		}
	};

	//	エッジ検出用の配列
	int** img_edge = NULL;
	int** img_edge_x = NULL;
	int** img_edge_y = NULL;
	img_edge = new int* [m_BmpInfo->bmiHeader.biHeight];
	img_edge_x = new int* [m_BmpInfo->bmiHeader.biHeight];
	img_edge_y = new int* [m_BmpInfo->bmiHeader.biHeight];
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		img_edge[i] = new int[m_BmpInfo->bmiHeader.biWidth];
		img_edge_x[i] = new int[m_BmpInfo->bmiHeader.biWidth];
		img_edge_y[i] = new int[m_BmpInfo->bmiHeader.biWidth];
	}

	//	フィルタの適用
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			if (radio1->GetCheck() == 1) {		//		プリューウィットフィルタ
				img_edge_x[i][j] = Filter2D(i, j, prewitt_operator_x);
				img_edge_y[i][j] = Filter2D(i, j, prewitt_operator_y);
			}
			if (radio2->GetCheck() == 1) {		//		ソーベルフィルタ
				img_edge_x[i][j] = Filter2D(i, j, sobel_operator_x);
				img_edge_y[i][j] = Filter2D(i, j, sobel_operator_y);
			}
			if (radio3->GetCheck() == 1) {		//		ラプラシアンフィルタ
				img_edge[i][j] = Filter2D(i, j, laplacian_operator);
			}
			if (radio4->GetCheck() == 1) {		//		Kirschマスク
				int edge_max = 0;
				for (int k = 0; k < 8; k++) {
					int edge = Filter2D(i, j, kirsch_mask[k]);
					if (edge > edge_max) {
						edge_max = edge;
					}
				}
				img_edge[i][j] = edge_max;
			}
		}
	}

	//	エッジ検出結果を代入
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			int edge_st;	//	エッジ強度
			if (radio1->GetCheck() == 1 || radio2->GetCheck() == 1) {
				edge_st = (int)sqrt(pow(img_edge_x[i][j], 2) + pow(img_edge_y[i][j], 2));
			}
			else {
				edge_st = img_edge[i][j];
			}
			if (edge_st > 255) {
				edge_st = 255;
			}
			if (edge_st < 0) {
				edge_st = 0;
			}
			m_Image->B[i][j] = edge_st;
			m_Image->R[i][j] = edge_st;
			m_Image->G[i][j] = edge_st;
		}
	}

	//	色反転
	if (check1->GetCheck() == 1) {
		for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->R[i][j] = 255 - m_Image->R[i][j];
				m_Image->G[i][j] = 255 - m_Image->G[i][j];
				m_Image->B[i][j] = 255 - m_Image->B[i][j];
			}
		}
	}

	delete[] img_edge;
	delete[] img_edge_x;
	delete[] img_edge_y;

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//	新規ダイアログ表示
void CSampleDlg::OnShowNewDialog() {
	//新規ダイアログ表示ボタン関数
	//CHistDlg dlg(this); //モーダルダイアログ作成
	//dlg.DoModal(); //モーダルダイアログ表示
	//モードレスダイアログの表示
	if (!m_HistDlg) {		//まだ作成されていないならダイアログを新規作成		
		m_HistDlg = new CHistDlg(this);									//メモリの確保
		if (m_HistDlg == NULL) return;									//メモリの確保に失敗したなら終了

		if (!m_HistDlg->GetSafeHwnd()) {
			m_HistDlg->Create(IDD_HIST_DIALOG);						    //モードレスダイアログの作成（メインダイアログの裏側へは行けない）
			//m_HistDlg->Create(IDD_HIST_DIALOG, GetDesktopWindow());	//モードレスダイアログの作成（メインダイアログの裏側へ行けるようにする）
			m_HistDlg->ShowWindow(TRUE);								//ダイアログを見えるようにする
		}
		else {
			delete m_HistDlg;											//メモリ解放
			m_HistDlg = NULL;											//ポインタ初期化
		}
	}
	else {				//すでに作成済みならダイアログを閉じる
		if (m_HistDlg->GetSafeHwnd()) m_HistDlg->DestroyWindow();		//ウィンドウの破棄
		delete m_HistDlg;												//メモリ解放
		m_HistDlg = NULL;												//ポインタ初期化
		Invalidate();													//メインダイアログのピクチャボックスに画像を描画する
	}
}

//	線形濃度変換
void CSampleDlg::OnLinearDensity()
{
	//	最大最小の輝度値を求める
	int low = 255, high = 0;
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			int lm = m_Image->R[i][j];
			low  = lm < low  ? lm : low;
			high = lm > high ? lm : high;
		}
	}

	//	変換
   	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			int in = m_Image->R[i][j];
			int out;
			if (in < low) {
				out = 0;
			}
			else if (low <= in && in < high) {
				out = (int)(((float)255 / (high - low)) * (in - low));
			}
			else {
				out = 255;
			}
			m_Image->B[i][j] = out;
			m_Image->R[i][j] = out;
			m_Image->G[i][j] = out;
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//	ヒストグラム平坦化
void CSampleDlg::OnFlatten()
{
	//	ヒストグラムを配列で表す
	int lum_list[256] = { 0 };
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			lum_list[m_Image->R[i][j]]++;
		}
	}

	//	平坦化
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			int in = m_Image->R[i][j];   
			int out;
			int sum = 0;
			for (int k = 0; k <= in; k++) {
				sum += lum_list[k];
			}
			out = (int)(sum * ((float)255 / (m_BmpInfo->bmiHeader.biHeight * m_BmpInfo->bmiHeader.biWidth)));
			m_Image->B[i][j] = out;
			m_Image->R[i][j] = out;
			m_Image->G[i][j] = out;
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//	完全平坦化
void CSampleDlg::OnCompletelyFlatten()
{
	const int after_sum_px = m_BmpInfo->bmiHeader.biHeight * m_BmpInfo->bmiHeader.biWidth / 256;	//	平坦化後の輝度値の度数
	int after_lm = 0;		//	平坦化後の輝度値
	int before_lm = 0;		//	平坦化前の輝度値
	int sum_px = 0;			//	変換した画素数

	//	処理後の画像用
	int** after_img = NULL;
	after_img = new int*[m_BmpInfo->bmiHeader.biHeight];
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		after_img[i] = new int[m_BmpInfo->bmiHeader.biWidth];
	}

	//	平坦化
	while(true) {
		//	輝度値0->255の順に探して変換
		for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				if (m_Image->R[i][j] != before_lm) {	//	注目の輝度値以外は除外
					continue;
				}
				after_img[i][j] = after_lm;
				sum_px++;
				if (sum_px == after_sum_px) {			//	指定の度数に達したら
					after_lm++;
					sum_px = 0;
				}
			}
		}
		before_lm++;
		if (before_lm > 255) {
			break;
		}
	}

	//	画像の更新
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_Image->R[i][j] = after_img[i][j];
			m_Image->G[i][j] = after_img[i][j];
			m_Image->B[i][j] = after_img[i][j];
		}
	}

	delete[] after_img;

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

void CSampleDlg::Animation(int count) {
	//BMPファイルの読み込み
			//変数の宣言
	int i, j;
	size_t image_size;
	std::string filenum = std::to_string(count);
	std::string filepath = "..\\..\\5\\anime\\anime" + filenum + ".bmp"; //アニメーションに用いる画像があるパスを記入
	//std::string filepath = "..\\..\\AnimationFrames\\original\\" + filenum.insert(0, 3 - filenum.length(), '0') + ".bmp";
	CString filename;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;

	///////////////////参照するディレクトリ(追加)////////////////////////(プロパティ等で確認する)
	filename.Format((LPCSTR)filepath.c_str());

	if (!file.Open(filename, CFile::modeRead | CFile::typeBinary)) {
		return;
	}

	//タイトルバーにファイル名を表示する
	SetWindowText("VCtest " + filename);

	//----------------------------------------------------------ファイル内容の読み込み処理

	//ファイルヘッダ部とインフォヘッダ部を読み込む
	file.Read(&m_BmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));

	// 前回の画像イメージをいったん解放
	if (m_BmpImage) delete[] m_BmpImage;

	if (m_BmpInfo) delete[] m_BmpInfo;

	// 前回の画像処理用メモリをいったん解放
	if (m_Image) delete[] m_Image;

	//色情報を取得する
	//biBitCountは1ピクセルあたりのカラー表現ビット数
	//1,4,8,16,32がある。数字が大きいほど精細表現が可能
	//16ビット以上と未満でカラーデータ格納が異なる
	if (myBmpInfoHdr.biBitCount >= 16) {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	}
	else {
		m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
		file.Read(m_BmpInfo->bmiColors,
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	}

	//m_BmpInfo(LPBITMAPINFO型)のmyBmpInfoHdrメンバに設定
	m_BmpInfo->bmiHeader = myBmpInfoHdr;

	//ファイル内のビットマップ実データ位置に合わせる
	file.Seek(m_BmpFileHdr.bfOffBits, CFile::begin);

	// 実画像データ分のバイト数を確保
	// bfsize	:	ビットマップファイル全サイズ
	// bfffBits	:	先頭にあるヘッダ情報サイズ
	image_size = m_BmpFileHdr.bfSize - m_BmpFileHdr.bfOffBits;
	m_BmpImage = new unsigned char[image_size];

	// ビットマップ実データを読み込み格納
	file.Read(m_BmpImage, (UINT)image_size);

	file.Close();

	//画像処理用のメモリを動的に確保
	m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);


	//RGBに分けて格納する
	if (myBmpInfoHdr.biBitCount == 24) {	//フルカラーの場合
		for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
				m_Image->B[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3];
				m_Image->G[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1];
				m_Image->R[i][j] = m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2];
			}
		}
	}
	else {
	}

	//再描写指示
	Invalidate();
}

//	タイマー関数
void CSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);

	//タイマー割り込み処理
	static int count = 0;	//カウント用

	if (nIDEvent == 1) {
		Animation(count); //追加したアニメーション用関数

		count++;

		//x(作成した画像の枚数)になったらタイマー終了
		if (count > 60) {
			KillTimer(1);
			count = 0; //リセット
		}
	}
	else if (nIDEvent == 2) {
		Capture(); //カメラから画像取得
		if (cv::waitKey(1) == 0x1B) { //ESCキーで終了
			KillTimer(1);
		}
	}
	else if (nIDEvent == 3) {
		Capture();
		BackDiff();
		if (cv::waitKey(1) == 0x1B) { //ESCキーで終了
			KillTimer(1);
		}

	}
	else if (nIDEvent == 4) {
		Capture();
		FrameDiff();
		if (cv::waitKey(1) == 0x1B) { //ESCキーで終了
			KillTimer(1);
		}
	}
}

void CSampleDlg::OnCallTimer()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	SetTimer(1, 200, NULL);
}

void CSampleDlg::OnOpenTemplate()
{
	int i, j;
	size_t image_size;
	CString filename;
	CFile file;
	BITMAPINFOHEADER myBmpInfoHdr;

	//----------------------------------------------------------ファイル操作ダイアログの呼び出し処理

	//ファイルダイアログを表示してファイル指定する
	CFileDialog myDLG(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"ﾋﾞｯﾄﾏｯﾌﾟ(*.bmP)|*.BMP||");
	if (myDLG.DoModal() != IDOK) return;

	//取得したファイルをオープンする
	filename = myDLG.GetPathName();
	if (!file.Open(filename, CFile::modeRead | CFile::typeBinary)) {
		return;
	}

	//タイトルバーにファイル名を表示する
	SetWindowText(filename);

	//----------------------------------------------------------ファイル内容の読み込み処理

	//ファイルヘッダ部とインフォヘッダ部を読み込む
	file.Read(&tmpBmpFileHdr, sizeof(BITMAPFILEHEADER));
	file.Read(&myBmpInfoHdr, sizeof(BITMAPINFOHEADER));

	// 前回の画像イメージをいったん解放
	if (tmpBmpImage) delete[] tmpBmpImage;

	// 前回使用していれば、いったんメモリ解放する
	if (tmpBmpInfo) delete[] tmpBmpInfo;

	// 前回の画像処理用メモリをいったん解放
	if (tmpImage) delete[] tmpImage;

	//	いったんメモリの開放
	if (tmpOrgImage) delete[] tmpOrgImage;

	//色情報を取得する
	//biBitCountは1ピクセルあたりのカラー表現ビット数
	//1,4,8,16,32がある。数字が大きいほど精細表現が可能
	//16ビット以上と未満でカラーデータ格納が異なる
	if (myBmpInfoHdr.biBitCount >= 16) {
		tmpBmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];
	}
	else {
		tmpBmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) +
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD)];
		file.Read(tmpBmpInfo->bmiColors,
			(1i64 << myBmpInfoHdr.biBitCount) * sizeof(RGBQUAD));
	}

	//tmpBmpInfo(LPBITMAPINFO型)のmyBmpInfoHdrメンバに設定
	tmpBmpInfo->bmiHeader = myBmpInfoHdr;

	//ファイル内のビットマップ実データ位置に合わせる
	file.Seek(tmpBmpFileHdr.bfOffBits, CFile::begin);

	// 実画像データ分のバイト数を確保
	// bfsize	:	ビットマップファイル全サイズ
	// bfffBits	:	先頭にあるヘッダ情報サイズ
	image_size = tmpBmpFileHdr.bfSize - tmpBmpFileHdr.bfOffBits;
	tmpBmpImage = new unsigned char[image_size];

	// ビットマップ実データを読み込み格納
	file.Read(tmpBmpImage, (UINT)image_size);

	file.Close();

	//画像処理用のメモリを動的に確保
	tmpImage = new Image(tmpBmpInfo->bmiHeader.biHeight, tmpBmpInfo->bmiHeader.biWidth);

	//RGBに分けて格納する
	if (myBmpInfoHdr.biBitCount == 24) {	//フルカラーの場合
		for (i = 0; i < tmpBmpInfo->bmiHeader.biHeight; i++) {
			for (j = 0; j < tmpBmpInfo->bmiHeader.biWidth; j++) {
				tmpImage->B[i][j] = tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - 1 - i) * tmpBmpInfo->bmiHeader.biWidth + j) * 3];
				tmpImage->G[i][j] = tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - 1 - i) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 1];
				tmpImage->R[i][j] = tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - 1 - i) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 2];
			}
		}
	}
	else {
	}

	//メモリの確保
	tmpOrgImage = new unsigned char[image_size];
	//メモリのコピー
	memcpy(tmpOrgImage, tmpBmpImage, image_size);
	//ファイルヘッダ・インフォの保存
	tmpOrgBmpFileHdr = tmpBmpFileHdr;
	tmpOrgBmpInfo = *tmpBmpInfo;
}

int CSampleDlg::calcDissimirality(int _y, int _x, int th, int tw) {
	int sum_diss = 0;
	for (int y = 0; y < th; y++) {
		for (int x = 0; x < tw; x++) {
			if (_x + x >= m_BmpInfo->bmiHeader.biWidth ||
				_y + y >= m_BmpInfo->bmiHeader.biHeight) {
				continue;
			}
			sum_diss += abs(m_Image->R[_y + y][_x + x] - tmpImage->R[y][x]);
		}
	}
	return sum_diss;
}

//	テンプレートマッチング
void CSampleDlg::OnTemplateMatching()
{
	int i, j;

	//	テンプレートを開く
	OnOpenTemplate();
	int tHeight = tmpBmpInfo->bmiHeader.biHeight;
	int tWidth = tmpBmpInfo->bmiHeader.biWidth;

	//	テンプレートじゃないほうを輝度化
	Image* colorImg = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			colorImg->R[i][j] = m_Image->R[i][j];
			colorImg->G[i][j] = m_Image->G[i][j];
			colorImg->B[i][j] = m_Image->B[i][j];
		}
	}
	OnKidoka();

	//	テンプレートを輝度化
	for (i = 0; i < tHeight; i++) {
		for (j = 0; j < tWidth; j++) {
			double dy = 0.3 * tmpImage->R[i][j] + 0.59 * tmpImage->G[i][j] + 0.11 * tmpImage->B[i][j];
			tmpImage->R[i][j] = (unsigned char)dy;
			tmpImage->G[i][j] = (unsigned char)dy;
			tmpImage->B[i][j] = (unsigned char)dy;
		}
	}
	for (i = 0; i < tHeight; i++) {
		for (j = 0; j < tWidth; j++) {
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3] = tmpImage->B[i][j];
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 1] = tmpImage->G[i][j];
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 2] = tmpImage->R[i][j];
		}
	}

	int my, mx;	//	テンプレートじゃないほうの基準点
	int matched_x = 0, matched_y = 0;	//	マッチした基準点
	for (my = 0; my < m_BmpInfo->bmiHeader.biHeight; my++) {
		for (mx = 0; mx < m_BmpInfo->bmiHeader.biWidth; mx++) {
			//	相違度の算出
			if (my + tHeight >= m_BmpInfo->bmiHeader.biHeight ||
				mx + tWidth >= m_BmpInfo->bmiHeader.biWidth) {
				continue;
			}

			int diss = calcDissimirality(my, mx, tHeight, tWidth);
			if (diss == 0) {
				matched_x = mx;
				matched_y = my;
			}
		}
	}

	//	カラーに戻す
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_Image->R[i][j] = colorImg->R[i][j];
			m_Image->G[i][j] = colorImg->G[i][j];
			m_Image->B[i][j] = colorImg->B[i][j];
		}
	}

	//	マークする
	int ty, tx;	//	テンプレート内の点
	for (ty = 0; ty < tHeight; ty++) {
		for (tx = 0; tx < tWidth; tx++) {
			if (0 < ty && ty < tHeight - 1 &&
				0 < tx && tx < tWidth - 1) {
				continue;
			}
			m_Image->R[matched_y + ty][matched_x + tx] = 255;
			m_Image->G[matched_y + ty][matched_x + tx] = 0;
			m_Image->B[matched_y + ty][matched_x + tx] = 0;
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//	相違度画像
void CSampleDlg::OnDissimirality()
{
	int i, j;

	//	テンプレートを開く
	OnOpenTemplate();
	int tHeight = tmpBmpInfo->bmiHeader.biHeight;
	int tWidth = tmpBmpInfo->bmiHeader.biWidth;

	//	テンプレートじゃないほうを輝度化
	OnKidoka();

	//	テンプレートを輝度化
	for (i = 0; i < tHeight; i++) {
		for (j = 0; j < tWidth; j++) {
			double dy = 0.3 * tmpImage->R[i][j] + 0.59 * tmpImage->G[i][j] + 0.11 * tmpImage->B[i][j];
			tmpImage->R[i][j] = (unsigned char)dy;
			tmpImage->G[i][j] = (unsigned char)dy;
			tmpImage->B[i][j] = (unsigned char)dy;
		}
	}
	for (i = 0; i < tHeight; i++) {
		for (j = 0; j < tWidth; j++) {
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3] = tmpImage->B[i][j];
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 1] = tmpImage->G[i][j];
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//	各位置の相違度を保存
	int** diss2D = new int* [m_BmpInfo->bmiHeader.biHeight];
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		diss2D[i] = new int[m_BmpInfo->bmiHeader.biWidth];
	}
	int my, mx;	//	テンプレートじゃないほうの基準点
	int max_diss = 0;							//	最大相違度
	int min_diss = 255 * tHeight * tWidth;		//	最小相違度
	for (my = 0; my < m_BmpInfo->bmiHeader.biHeight; my++) {
		for (mx = 0; mx < m_BmpInfo->bmiHeader.biWidth; mx++) {
			//	相違度の算出
			if (my + tHeight >= m_BmpInfo->bmiHeader.biHeight ||
				mx + tWidth >= m_BmpInfo->bmiHeader.biWidth) {
				continue;
			}
			diss2D[my][mx] = calcDissimirality(my, mx, tHeight, tWidth);
			if (diss2D[my][mx] > max_diss) {
				max_diss = diss2D[my][mx];
			}
			if (diss2D[my][mx] < min_diss) {
				min_diss = diss2D[my][mx];
			}
		}
	}

	//	相違度を濃度値にする
	for (my = 0; my < m_BmpInfo->bmiHeader.biHeight; my++) {
		for (mx = 0; mx < m_BmpInfo->bmiHeader.biWidth; mx++) {
			m_Image->R[my][mx] = 0;
			m_Image->G[my][mx] = 0;
			m_Image->B[my][mx] = 0;
			if (my + tHeight >= m_BmpInfo->bmiHeader.biHeight ||
				mx + tWidth >= m_BmpInfo->bmiHeader.biWidth) {
				continue;
			}
			//	mapみたいにやる方法
			m_Image->R[my][mx] = (unsigned char)((float)diss2D[my][mx] / max_diss) * 255;
			m_Image->G[my][mx] = (unsigned char)((float)diss2D[my][mx] / max_diss) * 255;
			m_Image->B[my][mx] = (unsigned char)((float)diss2D[my][mx] / max_diss) * 255;
			//	最悪の場合で割る方法
			//m_Image->R[my][mx] = ((float)diss2D[my][mx] / (255 * tHeight * tWidth)) * 255;
			//m_Image->G[my][mx] = ((float)diss2D[my][mx] / (255 * tHeight * tWidth)) * 255;
			//m_Image->B[my][mx] = ((float)diss2D[my][mx] / (255 * tHeight * tWidth)) * 255;
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//	配列内の1以上の最小値を取得
int GetValidMinFromArray(int array[]) {
	int min_val = INT_MAX;
	for (int i = 0; i < 9; i++) {
		int val = array[i];
		if (val != 0) {
			min_val = (val < min_val) ? val : min_val;
		}
	}
	
	if (min_val == INT_MAX) {
		return 0;
	}
	return min_val;
}

//	近傍のラベルを取得
void CSampleDlg::GetNeighborLabels(int _x, int _y, int (&label_list)[9], int* neighbor_list)
{
	int x, y;	//	近傍の座標

	for (int i = 0; i < 9; i++) {
		if (neighbor_list[i] == 0) {
			label_list[i] = 0;
			continue;
		}
		x = _x + (i % 3 - 1);
		y = _y + (i / 3 - 1);
		if (x < 0 || x >= m_BmpInfo->bmiHeader.biWidth ||
			y < 0 || y >= m_BmpInfo->bmiHeader.biHeight) {
			label_list[i] = 0;
		}
		else if (m_Image->R[_y][_x] == m_Image->R[y][x]) {
			label_list[i] = m_LabelMap[y][x];
		}
		else {
			label_list[i] = 0;
		}
	}
}

//	ルックアップテーブルの更新
int CSampleDlg::UpdateLookupTable(int index)
{
	if (index == m_LookupTable[index]) {
		return index;
	}
	m_LookupTable[index] = UpdateLookupTable(m_LookupTable[index]);
	return m_LookupTable[index];
}

//	テーブルのラベル値を更新
void CSampleDlg::UpdateLookupLabel(int _x, int _y, int min_label, int* neighbor_list)
{
	int x, y;	//	近傍の座標

	for (int i = 0; i < 9; i++) {
		if (neighbor_list[i] == 0) {
			continue;
		}
		x = _x + (i % 3 - 1);
		y = _y + (i / 3 - 1);
		if (x < 0 || x >= m_BmpInfo->bmiHeader.biWidth ||
			y < 0 || y >= m_BmpInfo->bmiHeader.biHeight) {
			continue;
		}
		if (m_Image->R[_y][_x] != m_Image->R[y][x]) {
			continue;
		}
		//	いったん更新してから参照
		if (UpdateLookupTable(m_LabelMap[y][x]) > min_label) {
			m_LookupTable[m_LabelMap[y][x]] = min_label;
		}
	}
}

//	ラスタ走査
void CSampleDlg::FowardScanning(int* neighbor_list, int* max_label)
{
	int nb_labels[9] = { 0 };	//	近傍のラベル
	int min_nb_label = 0;

	for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
		for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
			if (m_Image->R[y][x] == 0) {
				continue;
			}
			GetNeighborLabels(x, y, nb_labels, neighbor_list);
			min_nb_label = GetValidMinFromArray(nb_labels);
			min_nb_label = UpdateLookupTable(min_nb_label);	//	ルックアップテーブルをたどって最小の値を取得
			if (min_nb_label == 0) {
				//	近傍が背景or余白のみ
				if (m_LabelMap[y][x] == 0) {
					//	まだラベルがついてないなら
					m_LabelMap[y][x] = *max_label;
					(*max_label)++;
				}
			} else {
				//	最小ラベルを格納し，ルックアップラベルを更新
				m_LabelMap[y][x] = min_nb_label;
				UpdateLookupLabel(x, y, min_nb_label, neighbor_list);
			}
		}
	}
}

//	逆ラスタ走査
void CSampleDlg::ReverseScanning(int* neighbor_list, int* max_label)
{
	int nb_labels[9] = { 0 };	//	近傍のラベル
	int min_nb_label = 0;

	for (int y = m_BmpInfo->bmiHeader.biHeight - 1; y >= 0; y--) {
		for (int x = m_BmpInfo->bmiHeader.biWidth - 1; x >= 0; x--) {
			if (m_Image->R[y][x] == 0) {
				continue;
			}
			GetNeighborLabels(x, y, nb_labels, neighbor_list);
			min_nb_label = GetValidMinFromArray(nb_labels);
			min_nb_label = UpdateLookupTable(min_nb_label);	//	ルックアップテーブルをたどって最小の値を取得
			if (min_nb_label == 0) {
				//	近傍が背景or余白のみ
				if (m_LabelMap[y][x] == 0) {
					//	まだラベルがついてないなら
					m_LabelMap[y][x] = *max_label;
					(*max_label)++;
				}
			} else {
				//	最小ラベルを格納し，ルックアップラベルを更新
				m_LabelMap[y][x] = min_nb_label;
				UpdateLookupLabel(x, y, min_nb_label, neighbor_list);
			}
		}
	}
}

//	衝突処理
void CSampleDlg::ResoluteConflict(int max_label)
{
	for (int i = max_label; i > 0; i--) {
		for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
			for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
				if (i == m_LabelMap[y][x]) {
					m_LabelMap[y][x] = m_LookupTable[i];
				}
			}
		}
	}	
}

void CSampleDlg::RemoveNoises(int area, int max_label)
{
	std::vector<int> labels;

	//	area以下の面積のラベル値を調べる
	for (int label = 1; label < max_label; label++) {
		int sum_px = 0;
		for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
			for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
				if (m_LabelMap[y][x] == label) {
					sum_px++;
				};
				if (sum_px > area) {
					//	超過したらスキップ
					y = m_BmpInfo->bmiHeader.biHeight;
					break;
				}
			}
		}
		if (1 <= sum_px && sum_px <= area) {
			labels.push_back(label);
		}
	}

	//	area以下の領域を背景とする
	for (int label : labels) {
		for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
			for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
				if (m_LabelMap[y][x] != label) {
					continue;
				}
				m_LabelMap[y][x] = 0;
				m_Image->R[y][x] = 0;
				m_Image->G[y][x] = 0;
				m_Image->B[y][x] = 0;
			};
		}
	}
}

//	初期化
void CSampleDlg::InitLabels() {
	m_LabelMap = new int* [m_BmpInfo->bmiHeader.biHeight];
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		m_LabelMap[i] = new int[m_BmpInfo->bmiHeader.biWidth]();
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_LabelMap[i][j] = 0;
		}
	}

	m_LookupTable = new int[65536];	//	適当な数
	for (int i = 0; i < 65536; i++) {
		m_LookupTable[i] = i;
	}
}

//	終了処理
void CSampleDlg::DestroyLabels()
{
	delete[] m_LabelMap;
	delete[] m_LookupTable;
}

//	ラベリング
void CSampleDlg::OnLabeling()
{
	max_label = 1;	//	ラベルの最大値
	
	//	着目する近傍の画素
	int neighbor4[] = { 0, 1, 0, 1, 0, 0, 0, 0, 0 };
	int neighbor8[] = { 1, 1, 1, 1, 0, 0, 0, 0, 0 };
	int neighbor4_reversed[] = { 0, 0, 0, 0, 0, 1, 0, 1, 0 };
	int neighbor8_reversed[] = { 0, 0, 0, 0, 0, 1, 1, 1, 1 };

	InitLabels();

	//	ラスタ走査
	FowardScanning(neighbor8, &max_label);
	ResoluteConflict(max_label);

	//	逆ラスタ走査
	ReverseScanning(neighbor8_reversed, &max_label);
	ResoluteConflict(max_label);
	
	//	ノイズ除去
	RemoveNoises(100, max_label);

	LabelPaint(m_LabelMap, max_label);

	// DestroyLabels();
}

void CSampleDlg::LabelPaint(int** m_Img_cp, int label_max) {
	std::vector<int> clr_table;
	// サイズを確実に確保（各ラベルに対してRGB3色分）
	clr_table.resize(label_max * 3); 
	// カラーテーブルの生成
	for (int i = 0; i < label_max; i++) {
		clr_table[i * 3] = rand() % 256;     // R
		clr_table[i * 3 + 1] = rand() % 256; // G
		clr_table[i * 3 + 2] = rand() % 256; // B
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			if (m_Img_cp[i][j] > 0 && m_Img_cp[i][j] < label_max) {  // 範囲チェックを追加
				int idx = m_Img_cp[i][j] - 1;  // インデックスを0ベースに調整
				m_Image->R[i][j] = clr_table[idx * 3];
				m_Image->G[i][j] = clr_table[idx * 3 + 1];
				m_Image->B[i][j] = clr_table[idx * 3 + 2];
			}
			else {
				/*
				m_Image->R[i][j] = 0;
				m_Image->G[i][j] = 0;
				m_Image->B[i][j] = 0;
				*/
			}
		}
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//ビットマップインフォの設定
int CSampleDlg::GetBITMAPINFO()
{
	DWORD bytesPerPixel = 24 / 8; // １画素当たりのバイト数

	// まず，走査線の正味のバイト数を計算する．
	DWORD lineSizeDW = bytesPerPixel * WIDTH;

	// lineSizeDW を実際のサイズ (DWORD 数) にするため，
	// sizeof(DWORD) で割る (小数部切り上げ)．
	lineSizeDW = (int)lineSizeDW / sizeof(DWORD);

	DWORD lineSize = lineSizeDW * sizeof(DWORD);
	DWORD imageSize = lineSize * HEIGHT;

	m_BmpImage = new unsigned char[imageSize];

	//ビットマップインフォの設定
	m_BmpInfo->bmiHeader.biHeight = HEIGHT;
	m_BmpInfo->bmiHeader.biWidth = WIDTH;
	m_BmpInfo->bmiHeader.biBitCount = 24;
	m_BmpInfo->bmiHeader.biSizeImage = imageSize;
	m_BmpInfo->bmiHeader.biClrImportant = 0;
	m_BmpInfo->bmiHeader.biClrUsed = 0;
	m_BmpInfo->bmiHeader.biCompression = 0;
	m_BmpInfo->bmiHeader.biPlanes = 1;
	m_BmpInfo->bmiHeader.biSize = 40;
	m_BmpInfo->bmiHeader.biXPelsPerMeter = 0;
	m_BmpInfo->bmiHeader.biYPelsPerMeter = 0;

	return 0;
}

//ビットマップファイルヘッダの設定
int CSampleDlg::GetBITMAPFILEHEADER()
{
	//ビットマップファイルヘッダの設定
	m_BmpFileHdr.bfType = 0x4d42;
	m_BmpFileHdr.bfReserved1 = 0;
	m_BmpFileHdr.bfReserved2 = 0;

	m_BmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	m_BmpFileHdr.bfSize = m_BmpInfo->bmiHeader.biSizeImage + m_BmpFileHdr.bfOffBits;
	return 0;
}

//カメラから1フレーム取得
void CSampleDlg::Capture()
{
	int i, j;

	//カメラから取得
	capture.read(input);

	//RGB格納
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			m_Image->B[i][j] = input.ptr(i)[j * 3];
			m_Image->G[i][j] = input.ptr(i)[j * 3 + 1];
			m_Image->R[i][j] = input.ptr(i)[j * 3 + 2];
		}
	}

	//表示用
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描写指示
	//Invalidate(NULL);
}

//カメラ初期化
void CSampleDlg::InitCamera()
{
	//タイトルバーにファイル名を表示する
	SetWindowText("カメラ");

	// 前回の画像イメージをいったん開放
	if (m_BmpImage) delete[] m_BmpImage;

	if (m_Image) delete[] m_Image;

	//前回使用していれば、いったんメモリ開放する
	if (m_BmpInfo) delete[] m_BmpInfo;
	m_BmpInfo = (LPBITMAPINFO)new char[sizeof(BITMAPINFO)];

	GetBITMAPINFO();//ビットマップファイルヘッダ

	GetBITMAPFILEHEADER();//ビットマップインフォの設定

	//画像処理用のメモリを動的に確保
	m_Image = new Image(HEIGHT, WIDTH);

	//Mat型の初期化
	input = Mat::zeros(Size(WIDTH, HEIGHT), CV_8UC3);

	// カメラからのビデオキャプチャを初期化する
	capture.open(0);
}

void CSampleDlg::OnCamera()
{
	//初期化
	InitCamera();

	//静止画
	Capture();
}

void CSampleDlg::OnVideoCamera()
{
	//初期化
	InitCamera();

	//動画
	SetTimer(2, 100, NULL);
}

//	背景差分
void CSampleDlg::OnBackgroundDifference()
{
	//初期化
	InitCamera();

	savedata = new int*[m_BmpInfo->bmiHeader.biHeight];
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		savedata[i] = new int[m_BmpInfo->bmiHeader.biWidth];
	}

	//	背景を撮影
	Capture();
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			savedata[i][j] = 0.3 * m_Image->R[i][j] + 0.59 * m_Image->G[i][j] + 0.11 * m_Image->B[i][j];
		}
	}

	//動画
	SetTimer(3, 100, NULL);
}
void CSampleDlg::BackDiff() {
	//	元画像の保存
	static int filenum = 0;
	CString filename;
	filename.Format(_T("C:\\Users\\MYS\\source\\repos\\7\\背景差分\\元画像\\%.3d.bmp"), filenum);
	AutoSave(filename);

	double dy;

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			dy = 0.3 * m_Image->R[i][j] + 0.59 * m_Image->G[i][j] + 0.11 * m_Image->B[i][j];
			if (abs(dy - savedata[i][j]) < 30) {
				m_Image->R[i][j] = 0;
				m_Image->G[i][j] = 0;
				m_Image->B[i][j] = 0;
			}
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();

	filename.Format(_T("C:\\Users\\MYS\\source\\repos\\7\\背景差分\\結果画像\\%.3d.bmp"), filenum);
	AutoSave(filename);
	filenum++;
}

//	フレーム間差分
void CSampleDlg::OnFrameDifference()
{
	//初期化
	InitCamera();
	img_sp = 0;
	I1 = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	I2 = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	I3 = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	//動画
	SetTimer(4, 100, NULL);
}
void CSampleDlg::FrameDiff()
{
	//	元画像の保存
	static int filenum = 0;
	CString filename;
	filename.Format(_T("C:\\Users\\MYS\\source\\repos\\7\\フレーム間差分\\元画像\\%.3d.bmp"), filenum);
	AutoSave(filename);

	double dy, dy2;

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			dy = 0.3 * m_Image->R[i][j] + 0.59 * m_Image->G[i][j] + 0.11 * m_Image->B[i][j];
			if (img_sp == 0) {
				I1->R[i][j] = (unsigned char)dy;
			}
			if (img_sp == 1) {
				I2->R[i][j] = (unsigned char)dy;
			}
			if (img_sp == 2) {
				I3->R[i][j] = (unsigned char)dy;
			}
		}
	}

	img_sp++;
	if (img_sp >= 3) {
		img_sp = 0;
	}

	if (img_sp != 2) {
		return;
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			dy = abs(I3->R[i][j] - I2->R[i][j]);
			dy2 = abs(I2->R[i][j] - I1->R[i][j]);
			dy = (dy < 10) ? 0 : 1;
			dy2 = (dy2 < 10) ? 0 : 1;
			if (dy * dy2 == 0) {
				m_Image->R[i][j] = 0;
				m_Image->G[i][j] = 0;
				m_Image->B[i][j] = 0;
			}
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();

	filename.Format(_T("C:\\Users\\MYS\\source\\repos\\7\\フレーム間差分\\結果画像\\%.3d.bmp"), filenum);
	AutoSave(filename);
	filenum++;
}

static void ConvertRGB2HSV(float R, float G, float B, float* H, float* S, float* V) {
	int max = std::max(std::max(R, G), B);
	int min = std::min(std::min(R, G), B);
	if (max == 0) {
		*H = *S = 0;
	}
	else if (max == min) {
		*H = 0;
	}
	else {
		*S = (float)(max - min) / max;
		*V = (float)max / 255;
		if (max == R) {
			*H = 60 * (float)(G - B) / (max - min);
		}
		else if (max == G) {
			*H = 60 * (float)(B - R) / (max - min) + 120;
		}
		else {
			*H = 60 * (float)(R - G) / (max - min) + 240;
		}
	}
}

//	顔領域検出
void CSampleDlg::OnDitectFaces()
{
	//	肌色抽出
	std::vector<Point> skin_points;
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			//	HSV変換
			float H, S, V;
			ConvertRGB2HSV(
				m_Image->R[i][j], m_Image->G[i][j], m_Image->B[i][j], &H, &S, &V
			);
			//	肌色かどうかチェック
			if ((88 <= H && H <= 98) && (0.75 <= S && S <= 0.85) && (0.6 <= V && V <= 0.8)) {		//	ダミー
				continue;
			}
			if (((11 <= H && H <= 31) || (325 <= H && H <= 340)) && (0.10 <= S && S <= 0.40) && (0.30 <= V && V <= 0.60)) {
				continue;
			}
			if ((23 <= H && H <= 30) && (0.10 <= S && S <= 0.25) && (0.67 <= V && V <= 0.87)) {
				continue;
			}
			if (H == 0 || V == 0) {	//	灰色
				continue;
			}
			if ((10 <= H && H <= 30) &&	(0.2 <= S && S <= 1.0) && (0.3 <= V && V <= 1.0)) {	//	肌色
				skin_points.push_back({ j, i });
			} else
			if (H <= 15 && (0.4 <= S && S <= 0.5) && (0.2 <= V && V <= 0.70)) {
				skin_points.push_back({ j, i });
			} else 
			if ((7 <= H && H <= 13) && (0.32 <= S && S <= 0.4) && (0.75 <= V)) {
				skin_points.push_back({ j, i });
			}
		}
	}

	//	肌色領域の保存
	Image *skin_map = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			skin_map->R[i][j] = skin_map->G[i][j] = skin_map->B[i][j] = 0;
		}
	}
	for (Point p : skin_points) {
		skin_map->R[p.y][p.x] = skin_map->G[p.y][p.x] = skin_map->B[p.y][p.x] = 255;
	}

	//	ラベリング
	Image m_Image_copy = *m_Image;
	*m_Image = *skin_map;
	OnLabeling();

	//	ラベルごとに領域を囲む矩形を抽出
	std::vector<std::vector<int>> rect_list;
	int minx = m_BmpInfo->bmiHeader.biWidth;
	int miny = m_BmpInfo->bmiHeader.biHeight;
	int maxx = -1;
	int maxy = -1;
	for (int label = 1; label < max_label; label++) {
		for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight - 0; i++) {
			for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth - 0; j++) {
				if (m_LabelMap[i][j] != label) {
					continue;
				}
				minx = (j < minx) ? j : minx;
				miny = (i < miny) ? i : miny;
				maxx = (j > maxx) ? j : maxx;
				maxy = (i > maxy) ? i : maxy;
			}
		}
		rect_list.push_back({ minx, miny, maxx, maxy });
		minx = m_BmpInfo->bmiHeader.biWidth;
		miny = m_BmpInfo->bmiHeader.biHeight;
		maxx = maxy = -1;
	}

	//	抽出した矩形で枠を囲む
	for (std::vector<int> rect : rect_list) {
		for (int j = rect[0]; j <= rect[2]; j++) {
			m_Image->R[rect[1]][j] = m_Image->R[rect[3]][j] = 255;
			m_Image->G[rect[1]][j] = m_Image->G[rect[3]][j] = 0;
			m_Image->B[rect[1]][j] = m_Image->B[rect[3]][j] = 0;
		}
		for (int i = rect[1]; i <= rect[3]; i++) {
			m_Image->R[i][rect[0]] = m_Image->R[i][rect[2]] = 255;
			m_Image->G[i][rect[0]] = m_Image->G[i][rect[2]] = 0;
			m_Image->B[i][rect[0]] = m_Image->B[i][rect[2]] = 0;
		}
	}

	//	枠線以外を元に戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			if (m_Image->R[i][j] == 255 && m_Image->G[i][j] == 0 && m_Image->B[i][j] == 0) {
				continue;
			}
			m_Image->R[i][j] = m_Image_copy.R[i][j];
			m_Image->G[i][j] = m_Image_copy.G[i][j];
			m_Image->B[i][j] = m_Image_copy.B[i][j];
		}
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	拡大縮小
void CSampleDlg::ImageScaling()
{
	//拡大・縮小後のサイズに変更
	int exWidth, exHeight;
	size_t ExImageSize;

	exWidth = (int)(m_OrgBmpInfo.bmiHeader.biWidth * m_rate);   //拡大・縮小後の幅
	exHeight = (int)(m_OrgBmpInfo.bmiHeader.biHeight * m_rate); //拡大・縮小後の高さ

	//BMPの場合幅を4の倍数にする必要があるため4の倍数になるようにする
	exWidth = exWidth - exWidth % 4;
	exHeight = exHeight - exHeight % 4;

	DWORD bytesPerPixel = BPP / 8; //1画素当たりのバイト数

	//走査線の正味のバイト数を計算する
	DWORD lineSizeDW = bytesPerPixel * exWidth;

	//"lineSizeDW"を実際のサイズ(DWORD 数)にするため，sizeof(DWORD)で割る(小数部切り上げ)
	lineSizeDW = ICEIL(lineSizeDW, sizeof(DWORD));

	DWORD lineSize = lineSizeDW * sizeof(DWORD);
	ExImageSize = lineSize * exHeight;

	//拡大・縮小後の値を格納
	m_BmpInfo->bmiHeader.biWidth = (long)exWidth;
	m_BmpInfo->bmiHeader.biHeight = (long)exHeight;
	m_BmpInfo->bmiHeader.biSizeImage = (DWORD)ExImageSize;
	m_BmpFileHdr.bfSize = m_BmpInfo->bmiHeader.biSizeImage + m_BmpFileHdr.bfOffBits;

	// 前回の画像イメージをいったん開放
	delete[] m_BmpImage;
	delete[] m_Image;

	//拡大後の画像のメモリ確保
	m_BmpImage = new unsigned char[m_BmpFileHdr.bfSize];
	m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
}

//	最近傍法
void CSampleDlg::OnNearestNeighbor()
{
	if (GetDlgItem(IDC_EDIT_RATE)->GetWindowTextLength() == 0) {
		m_rate = 1;
	}
	else {
		UpdateData(TRUE);
	}

	//	拡大・縮小前の画像を記憶
	Image* src = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			src->R[i][j] = m_Image->R[i][j];
			src->G[i][j] = m_Image->G[i][j];
			src->B[i][j] = m_Image->B[i][j];
		}
	}

	//	画像の拡大・縮小
	ImageScaling();

	//	補間
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			int si, sj;	//	逆変換後の座標
			si = int(i / m_rate);
			sj = int(j / m_rate);
			m_Image->R[i][j] = src->R[si][sj];
			m_Image->G[i][j] = src->G[si][sj];
			m_Image->B[i][j] = src->B[si][sj];
		}
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	線形補間法
void CSampleDlg::OnLinearInter()
{
	if (GetDlgItem(IDC_EDIT_RATE)->GetWindowTextLength() == 0) {
		m_rate = 1;
	}
	else {
		UpdateData(TRUE);
	}

	//	拡大・縮小前の画像を記憶
	Image* src = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			src->R[i][j] = m_Image->R[i][j];
			src->G[i][j] = m_Image->G[i][j];
			src->B[i][j] = m_Image->B[i][j];
		}
	}

	//	画像の拡大・縮小
	ImageScaling();

	//	補間
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			int si, sj;	//	逆変換後の座標
			double di, dj;	//	si,sjの小数部分
			si = int(i / m_rate);
			sj = int(j / m_rate);
			di = i / m_rate - si;
			dj = j / m_rate - sj;

			if (si >= src->Height - 1 && sj >= src->Width - 1) {	//	端処理
				m_Image->R[i][j] = src->R[si][sj];
				m_Image->G[i][j] = src->G[si][sj];
				m_Image->B[i][j] = src->B[si][sj];
			} else if (si >= src->Height - 1) {
				m_Image->R[i][j] = (1 - dj) * src->R[si][sj] + dj * src->R[si][sj + 1];
				m_Image->G[i][j] = (1 - dj) * src->G[si][sj] + dj * src->G[si][sj + 1];
				m_Image->B[i][j] = (1 - dj) * src->B[si][sj] + dj * src->B[si][sj + 1];
			}
			else if (sj >= src->Width - 1) {
				m_Image->R[i][j] = (1 - di) * src->R[si][sj] + di * src->R[si + 1][sj];
				m_Image->G[i][j] = (1 - di) * src->G[si][sj] + di * src->G[si + 1][sj];
				m_Image->B[i][j] = (1 - di) * src->B[si][sj] + di * src->B[si + 1][sj];
			}
			else {
				m_Image->R[i][j] = (1 - dj) * (1 - di) * src->R[si][sj] + dj * (1 - di) * src->R[si][sj + 1] + (1 - dj) * di * src->R[si + 1][sj] + dj * di * src->R[si + 1][sj + 1];
				m_Image->G[i][j] = (1 - dj) * (1 - di) * src->G[si][sj] + dj * (1 - di) * src->G[si][sj + 1] + (1 - dj) * di * src->G[si + 1][sj] + dj * di * src->G[si + 1][sj + 1];
				m_Image->B[i][j] = (1 - dj) * (1 - di) * src->B[si][sj] + dj * (1 - di) * src->B[si][sj + 1] + (1 - dj) * di * src->B[si + 1][sj] + dj * di * src->B[si + 1][sj + 1];
			}
		}
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	新・テンプレートマッチング
void CSampleDlg::OnNewTemplateMatching()
{
	/* 2枚の画像を下処理 */
	//	テンプレートを開く
	OnOpenTemplate();
	int tHeight = tmpBmpInfo->bmiHeader.biHeight;
	int tWidth = tmpBmpInfo->bmiHeader.biWidth;
	//	テンプレートじゃないほうを輝度化
	Image* colorImg = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			colorImg->R[i][j] = m_Image->R[i][j];
			colorImg->G[i][j] = m_Image->G[i][j];
			colorImg->B[i][j] = m_Image->B[i][j];
		}
	}
	OnKidoka();
	//	テンプレートを輝度化
	for (int i = 0; i < tHeight; i++) {
		for (int j = 0; j < tWidth; j++) {
			double dy = 0.3 * tmpImage->R[i][j] + 0.59 * tmpImage->G[i][j] + 0.11 * tmpImage->B[i][j];
			tmpImage->R[i][j] = (unsigned char)dy;
			tmpImage->G[i][j] = (unsigned char)dy;
			tmpImage->B[i][j] = (unsigned char)dy;
		}
	}
	for (int i = 0; i < tHeight; i++) {
		for (int j = 0; j < tWidth; j++) {
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3] = tmpImage->B[i][j];
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 1] = tmpImage->G[i][j];
			tmpBmpImage[((tmpBmpInfo->bmiHeader.biHeight - i - 1) * tmpBmpInfo->bmiHeader.biWidth + j) * 3 + 2] = tmpImage->R[i][j];
		}
	}

	/* テンプレートマッチング */
	int my, mx;		//	テンプレートじゃないほうの基準点
	std::vector<Point> matched_pt;
	std::vector<double> matched_rate;
	for (double rate = 0.5; rate <= 2.0 + 0.001; rate += 0.05) {
		/* テンプレートをリサイズ */
		int resize_height = int(tmpImage->Height * rate);
		int resize_width = int(tmpImage->Width * rate);
		int** resized_img = nullptr;	//	リサイズ後の輝度化テンプレート
		resized_img = new int* [resize_height];
		for (int i = 0; i < resize_width; i++) {
			resized_img[i] = new int[resize_width];
		}
		//	補間
		for (int i = 0; i < resize_height; i++) {
			for (int j = 0; j < resize_width; j++) {
				int si, sj;	//	逆変換後の座標
				double di, dj;	//	si,sjの小数部分
				si = int(i / rate);
				sj = int(j / rate);
				di = i / rate - si;
				dj = j / rate - sj;
				if (si >= tmpImage->Height - 1 && sj >= tmpImage->Width - 1) {	//	端処理
					resized_img[i][j] = tmpImage->R[si][sj];
				} else if (si >= tmpImage->Height - 1) {
					resized_img[i][j] = (1 - dj) * tmpImage->R[si][sj] + dj * tmpImage->R[si][sj + 1];
				}
				else if (sj >= tmpImage->Width - 1) {
					resized_img[i][j] = (1 - di) * tmpImage->R[si][sj] + di * tmpImage->R[si + 1][sj];
				}
				else {
					resized_img[i][j] = (1 - dj) * (1 - di) * tmpImage->R[si][sj] + dj * (1 - di) * tmpImage->R[si][sj + 1] + (1 - dj) * di * tmpImage->R[si + 1][sj] + dj * di * tmpImage->R[si + 1][sj + 1];
				}
			}
		}
		/* 相違度の計算 */
		int max_diss = 255 * tHeight * tWidth * rate * rate;
		for (my = 0; my < m_BmpInfo->bmiHeader.biHeight; my++) {
			for (mx = 0; mx < m_BmpInfo->bmiHeader.biWidth; mx++) {
				if (my + tHeight * rate >= m_BmpInfo->bmiHeader.biHeight ||
					mx + tWidth * rate >= m_BmpInfo->bmiHeader.biWidth) {
					continue;
				}
				float diss = 0;
				for (int i = 0; i < resize_height; i++) {
					for (int j = 0; j < resize_height; j++) {
						diss += abs(m_Image->R[my + i][mx + j] - resized_img[i][j]);
						if (diss / max_diss > 0.005) {
							i = resize_height;
							break;
						}
					}
				}
				if (diss / max_diss <= 0.005) {
					matched_pt.push_back({ mx, my });
					matched_rate.push_back(rate);
				}
			}
		}

		delete[] resized_img;
	}

	//	カラーに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_Image->R[i][j] = colorImg->R[i][j];
			m_Image->G[i][j] = colorImg->G[i][j];
			m_Image->B[i][j] = colorImg->B[i][j];
		}
	}

	//	マークする
	for (int i = 0; i < matched_pt.size(); i++) {
		int mark_size = int(tHeight * matched_rate[i]);
		for (int y = matched_pt[i].y; y <= matched_pt[i].y + mark_size; y++) {
			int x1 = matched_pt[i].x;
			int x2 = x1 + mark_size;
			m_Image->R[y][x1] = m_Image->R[y][x2] = 255;
			m_Image->G[y][x1] = m_Image->G[y][x2] = 0;
			m_Image->B[y][x1] = m_Image->B[y][x2] = 0;
		}
		for (int x = matched_pt[i].x; x <= matched_pt[i].x + mark_size; x++) {
			int y1 = matched_pt[i].y;
			int y2 = y1 + mark_size;
			m_Image->R[y1][x] = m_Image->R[y2][x] = 255;
			m_Image->G[y1][x] = m_Image->G[y2][x] = 0;
			m_Image->B[y1][x] = m_Image->B[y2][x] = 0;
		}
	}

	//編集した内容を表示用バッファに戻す
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - i - 1) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}

	//再描画
	Invalidate();
}

//	膨張
void CSampleDlg::OnExpension()
{
	std::vector<Point> ep;		//	膨張部分の位置
	int neighbor4[] = { 0, 1, 0, 1, 0, 1, 0, 1, 0 };	//	膨張させる近傍
	int neighbor8[] = { 1, 1, 1, 1, 0, 1, 1, 1, 1 };
	
	/* 膨張させる位置を見つける */
	for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
		for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
			if (m_Image->R[y][x] == 255) {
				continue;
			}
			for (int i = 0; i < 9; i++) {
				if (neighbor8[i] == 0) {
					continue;
				}
				int ex = x + (i % 3 - 1);
				int ey = y + (i / 3 - 1);
				if (ex < 0 || m_BmpInfo->bmiHeader.biWidth <= ex ||
					ey < 0 || m_BmpInfo->bmiHeader.biHeight <= ey) {
					continue;
				}
				ep.push_back({ ex, ey });
			}
		}
	}

	/* 膨張させる */
	for (Point p : ep) {
		m_Image->R[p.y][p.x] = 0;
		m_Image->G[p.y][p.x] = 0;
		m_Image->B[p.y][p.x] = 0;
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	収縮
void CSampleDlg::OnContraction() {
	std::vector<Point> ep;		//	収縮部分の位置
	int neighbor4[] = { 0, 1, 0, 1, 0, 1, 0, 1, 0 };	//	収縮させる近傍
	int neighbor8[] = { 1, 1, 1, 1, 0, 1, 1, 1, 1 };
	
	/* 収縮させる位置を見つける */
	for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
		for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
			if (m_Image->R[y][x] != 255) {
				continue;
			}
			for (int i = 0; i < 9; i++) {
				if (neighbor8[i] == 0) {
					continue;
				}
				int ex = x + (i % 3 - 1);
				int ey = y + (i / 3 - 1);
				if (ex < 0 || m_BmpInfo->bmiHeader.biWidth <= ex ||
					ey < 0 || m_BmpInfo->bmiHeader.biHeight <= ey) {
					continue;
				}
				ep.push_back({ ex, ey });
			}
		}
	}
	/* 収縮させる */
	for (Point p : ep) {
		m_Image->R[p.y][p.x] = 255;
		m_Image->G[p.y][p.x] = 255;
		m_Image->B[p.y][p.x] = 255;
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	肌色抽出
void CSampleDlg::OnSkinExtraction()
{
	std::vector<Point> skin_points;		//	肌色の画素

	//	肌色検出
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			//	HSV変換
			float H, S, V;
			ConvertRGB2HSV(
				m_Image->R[i][j], m_Image->G[i][j], m_Image->B[i][j], &H, &S, &V
			);
			//	肌色かどうかチェック
			if ((88 <= H && H <= 98) && (0.75 <= S && S <= 0.85) && (0.6 <= V && V <= 0.8)) {		//	ダミー
				continue;
			}
			if (((11 <= H && H <= 31) || (325 <= H && H <= 340)) && (0.10 <= S && S <= 0.40) && (0.30 <= V && V <= 0.60)) {
				continue;
			}
			if ((23 <= H && H <= 30) && (0.10 <= S && S <= 0.25) && (0.67 <= V && V <= 0.87)) {
				continue;
			}
			if (H == 0 || V == 0) {	//	灰色
				continue;
			}
			if ((10 <= H && H <= 30) &&	(0.2 <= S && S <= 1.0) && (0.3 <= V && V <= 1.0)) {	//	肌色
				skin_points.push_back({ j, i });
			} else
			if (H <= 15 && (0.4 <= S && S <= 0.5) && (0.2 <= V && V <= 0.70)) {
				skin_points.push_back({ j, i });
			} else 
			if ((7 <= H && H <= 13) && (0.32 <= S && S <= 0.4) && (0.75 <= V)) {
				skin_points.push_back({ j, i });
			}
		}
	}
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_Image->R[i][j] = m_Image->G[i][j] = m_Image->B[i][j] = 0;
		}
	}
	for (Point p : skin_points) {
		m_Image->R[p.y][p.x] = m_Image->G[p.y][p.x] = m_Image->B[p.y][p.x] = 255;
	}

	//	結果を描画
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	境界追跡
void CSampleDlg::OnContourTracing()
{
	const int BACKGROUND_VALUE = 255;

	int i, j, k;
	int y, x;

	int labels = 0;
	int search_direction;
	int search_order[8][8] = {
		{5, 6, 7, 0, 1, 2, 3, 4},
		{6, 7, 0, 1, 2, 3, 4, 5},
		{7, 0, 1, 2, 3, 4, 5, 6},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{1, 2, 3, 4, 5, 6, 7, 0},
		{2, 3, 4, 5, 6, 7, 0, 1},
		{3, 4, 5, 6, 7, 0, 1, 2},
		{4, 5, 6, 7, 0, 1, 2, 3},
	};
	int search_order_reversed[8][8] = {
		{1, 0, 7, 6, 5, 4, 3, 2},
		{0, 7, 6, 5, 4, 3, 2, 1},
		{7, 6, 5, 4, 3, 2, 1, 0},
		{6, 5, 4, 3, 2, 1, 0, 7},
		{5, 4, 3, 2, 1, 0, 7, 6},
		{4, 3, 2, 1, 0, 7, 6, 5},
		{3, 2, 1, 0, 7, 6, 5, 4},
		{2, 1, 0, 7, 6, 5, 4, 3},
	};
	int move_y[] = { 0, -1, -1, -1,  0,  1, 1, 1 };
	int move_x[] = { 1,  1,  0, -1, -1, -1, 0, 1 };

	typedef struct {
		int x, y;
		int label;
		double length;
		std::vector<int> chain;
	} Shape;
	std::vector<Shape> shapes;

	Image* colorImage = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			colorImage->R[i][j] = m_Image->R[i][j];
			colorImage->G[i][j] = m_Image->G[i][j];
			colorImage->B[i][j] = m_Image->B[i][j];
		}
	}

	InitLabels();

	OnKidoka();

	//	背景から物体の輪郭追跡
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			if (j == 0) {
				continue;
			}
			if (m_Image->R[i][j] == BACKGROUND_VALUE || m_Image->R[i][j - 1] != BACKGROUND_VALUE) {
				continue;
			}
			if (m_Image->R[i][j - 1] == m_Image->R[i][j]) {
				continue;
			}
			if (m_LabelMap[i][j - 1] != 0 || m_LabelMap[i][j] != 0) {
				continue;
			}
			labels++;
			search_direction = 0;
			y = i;
			x = j;

			Shape shape = { x, y, 0, {0} };
			while (true) {
				int start_y = y;
				int start_x = x;
				int start_val = m_Image->R[start_y][start_x];
				for (k = 0; k < 8; k++) {
					int direction = search_order[search_direction][k];
					y = start_y + move_y[direction];
					x = start_x + move_x[direction];
					if (y < 0 || y >= m_BmpInfo->bmiHeader.biHeight ||
						x < 0 || x >= m_BmpInfo->bmiHeader.biWidth) {
						continue;
					}
					int around_val = m_Image->R[y][x];
					if (abs(around_val - start_val) > 50) {
						continue;
					}
					if (m_LabelMap[y][x] == 0) {
						m_LabelMap[y][x] = labels;
					}
					search_direction = direction;
					shape.chain.push_back(direction);
					if (direction % 2 == 0) {
						shape.length += 1.0;
					}
					else {
						shape.length += sqrt(2);
					}
					break;
				}
				if (y == i && x == j) {
					if (search_direction != 1) {
						break;
					}
					continue;
				}
			}
			shape.label = labels;
			shapes.push_back(shape);
		}
	}

	//	物体から物体（孔）の輪郭追跡
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			if (j == m_BmpInfo->bmiHeader.biWidth - 1) {
				continue;
			}
			if (m_Image->R[i][j] == BACKGROUND_VALUE || m_Image->R[i][j + 1] == BACKGROUND_VALUE) {
				continue;
			}
			if (m_Image->R[i][j] == m_Image->R[i][j + 1]) {
				continue;
			}
			if (m_LabelMap[i][j] != 0 || m_LabelMap[i][j + 1] != 0) {
				continue;
			}
			labels++;
			search_direction = 4;
			y = i;
			x = j;

			Shape shape = { x, y, 0, {0} };
			while (true) {
				int start_y = y;
				int start_x = x;
				int start_val = m_Image->R[start_y][start_x];
				for (k = 0; k < 8; k++) {
					int direction = search_order[search_direction][k];
					y = start_y + move_y[direction];
					x = start_x + move_x[direction];
					if (y < 0 || y >= m_BmpInfo->bmiHeader.biHeight ||
						x < 0 || x >= m_BmpInfo->bmiHeader.biWidth) {
						continue;
					}
					int around_val = m_Image->R[y][x];
					if (abs(around_val - start_val) > 50) {
						continue;
					}
					if (m_LabelMap[y][x] == 0) {
						m_LabelMap[y][x] = labels;
					}
					search_direction = direction;
					shape.chain.push_back(direction);
					if (direction % 2 == 0) {
						shape.length += 1.0;
					}
					else {
						shape.length += sqrt(2);
					}
					break;
				}
				if (y == i && x == j) {
					break;
				}
			}
			shape.label = labels;
			shapes.push_back(shape);
		}
	}

	//	ラベルの統一
	std::vector<int> right_label, left_label;
	std::vector<int> up_label, down_label;
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			if (m_LabelMap[i][j] == 0) {
				continue;
			}
			int order = 0;	//	探索方向
			int dist = 1;
			right_label.push_back(m_LabelMap[i][j]);
			left_label.push_back(m_LabelMap[i][j]);
			up_label.push_back(m_LabelMap[i][j]);
			down_label.push_back(m_LabelMap[i][j]);
			while (true) {
				int y = i + move_y[order * 2] * dist;
				int x = j + move_x[order * 2] * dist;
				dist++;
				if (x < 0 || x >= m_BmpInfo->bmiHeader.biWidth ||
					y < 0 || y >= m_BmpInfo->bmiHeader.biHeight) {
					order++;
					dist = 1;
					if (order >= 4) {
						break;
					}
					continue;
				}
				if (m_LabelMap[y][x] == 0) {
					continue;
				}
				switch (order) {
				case 0:
				{
					int& back = right_label.back();
					if (m_LabelMap[y][x] < back) {
						right_label.push_back(m_LabelMap[y][x]);
					}
					break;
				}
				case 1:
				{
					int& back = up_label.back();
					if (m_LabelMap[y][x] < back) {
						up_label.push_back(m_LabelMap[y][x]);
					}
					break;
				}
				case 2:
				{
					int& back = left_label.back();
					if (m_LabelMap[y][x] < back) {
						left_label.push_back(m_LabelMap[y][x]);
					}
					break;
				}
				case 3:
				{
					int& back = down_label.back();
					if (m_LabelMap[y][x] < back) {
						down_label.push_back(m_LabelMap[y][x]);
					}
					break;
				}
				default:
					break;
				}
			}
			int min_label = labels;
			for (int right : right_label) {
				for (int left : left_label) {
					for (int up : up_label) {
						for (int down : down_label) {
							if (right == left && left == up && up == down && right < min_label) {
								min_label = right;
							}
						}
					}
				}
			}
			m_LookupTable[m_LabelMap[i][j]] = min_label;
			right_label.clear();
			left_label.clear();
			up_label.clear();
			down_label.clear();
		}
	}
	ResoluteConflict(labels);

	//	"T"の検出
	int t_label = -1;
	std::vector<int> t_shape = { 6, 0, 7, 6, 0, 2, 1, 0, 2, 4 };
	for (Shape shape : shapes) {
		int shape_index = 0;
		int prev_chain_code = -1;
		for (int chain_code : shape.chain) {
			if (chain_code == prev_chain_code) {
				continue;
			}
			prev_chain_code = chain_code;
			if (chain_code == t_shape[shape_index]) {
				shape_index++;
			}
			else {
				break;
			}
			if (shape_index >= t_shape.size()) {
				break;
			}
		}
		if (shape_index == t_shape.size()) {
			t_label = shape.label;
		}
	}

	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_Image->R[i][j] = colorImage->R[i][j];
			m_Image->G[i][j] = colorImage->G[i][j];
			m_Image->B[i][j] = colorImage->B[i][j];
		}
	}

	LabelPaint(m_LabelMap, labels + 1);

	//	T専用の色
	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			if (m_LabelMap[i][j] == t_label) {
				m_Image->R[i][j] = 245;
				m_Image->G[i][j] = 130;
				m_Image->B[i][j] = 32;
			}
		}
	}

	for (i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();

}

//	ハフ変換（Wed:3h,5h）
void CSampleDlg::OnHoughTramsform()
{
	const double PI = acos(-1);	//	円周率
	struct Polar { double r, theta; };	//	極座標

	//	パラメータ空間を生成
	//	高さは画像の斜めの長さ（最大値が画像の角から角になるので）の２倍
	//	幅は0からpi（小数第2位まで）
	int param_space_height = int(sqrt(pow(m_Image->Height, 2) + pow(m_Image->Width, 2))) * 2;
	int param_space_width = int(PI * 100);
	int** param_space = new int* [param_space_height];
	for (int i = 0; i < param_space_height; i++) {
		param_space[i] = new int[param_space_width];
		for (int j = 0; j < param_space_width; j++) {
			param_space[i][j] = 0;
		}
	}

	//	画像からパラメータ空間へ写像
	for (int y = 0; y < m_BmpInfo->bmiHeader.biHeight; y++) {
		for (int x = 0; x < m_BmpInfo->bmiHeader.biWidth; x++) {
			if (m_Image->R[y][x] == 255) {
				continue;
			}
			double r, theta;	//	パラメータ
			for (theta = 0.00; theta < PI; theta += 0.01) {
				r = x * cos(theta) + y * sin(theta);
				param_space[int(r + param_space_height / 2.0 + 0.50)][int(theta * 100)]++;
			}
		}
	}

	//	パラメータ空間を描画
	//	DispParamSpace(param_space, param_space_width, param_space_height);
	//	return;

	//	パラメータ空間の曲線の重なりを調べる
	std::vector<Polar> overlap_param;
	std::vector<int> overlap_count;
	for (int r = 0; r < param_space_height; r++) {
		for (double theta = 0.00; theta < PI; theta += 0.01) {
			int count = param_space[r][int(theta * 100)];
			if (count > 60) {	//	この数を超えたら直線とみなす
				overlap_param.push_back({ double(r), theta });
				overlap_count.push_back(count);
			}
		}
	}

	//	類似する周辺の直線を削除
	for (int i = 0; i < overlap_count.size(); i++) {
		for (int j = i + 1; j < overlap_count.size(); j++) {
			if (abs(overlap_param[i].r - overlap_param[j].r) < 10 &&
				abs(overlap_param[i].theta - overlap_param[j].theta) < PI * 10 / 180) {
				if (overlap_count[i] < overlap_count[j]) {
					overlap_param.erase(overlap_param.begin() + i);
					overlap_count.erase(overlap_count.begin() + i);
				}
				else {
					overlap_param.erase(overlap_param.begin() + j);
					overlap_count.erase(overlap_count.begin() + j);
				}
				j--;
			}
		}
	}

	//	検出した直線を描画
	for (Polar p : overlap_param) {
		if (p.theta == 0) {
			continue;
		}
		//	y=ax+b
		double a = -1 / tan(p.theta);
		double b = (p.r - param_space_height / 2.0) / sin(p.theta) + 0.5;	//	四捨五入のための1/2
		double x, y;
		for (x = 0; x < m_BmpInfo->bmiHeader.biWidth; x += 0.1) {
			y = a * x + b;
			if (y < 0 || y >= m_Image->Height) {
				continue;
			}
			m_Image->R[int(y)][int(x)] = 255;
			m_Image->G[int(y)][int(x)] = 0;
			m_Image->B[int(y)][int(x)] = 0;
		}
	}

	//	結果を描画
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	パラメータ空間を描画
void CSampleDlg::DispParamSpace(int** param, int width, int height)
{
	//	処理内容は拡大縮小ImageScaling()を参照
	size_t ExImageSize;

	int radian_width = width;
	width = 180;
	height = height - height % 4;

	DWORD bytesPerPixel = BPP / 8;

	DWORD lineSizeDW = bytesPerPixel * width;

	lineSizeDW = ICEIL(lineSizeDW, sizeof(DWORD));

	DWORD lineSize = lineSizeDW * sizeof(DWORD);
	ExImageSize = lineSize * height;

	m_BmpInfo->bmiHeader.biWidth = (long)width;
	m_BmpInfo->bmiHeader.biHeight = (long)height;
	m_BmpInfo->bmiHeader.biSizeImage = (DWORD)ExImageSize;
	m_BmpFileHdr.bfSize = m_BmpInfo->bmiHeader.biSizeImage + m_BmpFileHdr.bfOffBits;

	delete[] m_BmpImage;
	delete[] m_Image;

	m_BmpImage = new unsigned char[m_BmpFileHdr.bfSize];
	m_Image = new Image(m_BmpInfo->bmiHeader.biHeight, m_BmpInfo->bmiHeader.biWidth);

	for (int j = 0; j < radian_width; j++) {
		int deg = int(double(j) / radian_width * 180);
		for (int i = 0; i < height; i++) {
			int dy = param[i][j] * 2;
			if (dy > 255)	dy = 255;
			m_Image->R[i][deg] = dy;
			m_Image->G[i][deg] = dy;
			m_Image->B[i][deg] = dy;
		}
	}

	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

//	逆行列を計算
template <int N = 3>
std::array<std::array<double, N>, N>
CalcInverseMatrix(std::array<std::array<double, N>, N> mat) {
    const double epsilon = 1e-10;  // 許容誤差
    int i, j, k;
    std::array<std::array<double, N>, N> inv = { 0 };         // 逆行列
    std::array<std::array<double, N * 2>, N> sweep = { 0 };   // 拡大行列

    // 拡大行列の初期化
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            sweep[i][j] = mat[i][j];
            sweep[i][j + N] = (i == j) ? 1 : 0;
        }
    }

    // 掃き出し法による逆行列の計算
    for (k = 0; k < N; k++) {
        // ピボット選択（k列目で最大の絶対値を持つ行を探す）
        int pivot_row = k;
        double max_val = std::abs(sweep[k][k]);
        for (i = k + 1; i < N; i++) {
            if (std::abs(sweep[i][k]) > max_val) {
                max_val = std::abs(sweep[i][k]);
                pivot_row = i;
            }
        }

        // 必要な場合は行を交換
        if (pivot_row != k) {
            for (j = 0; j < N * 2; j++) {
                std::swap(sweep[k][j], sweep[pivot_row][j]);
            }
        }

        // 行列が特異に近い場合のチェック
        if (std::abs(sweep[k][k]) < epsilon) {
            throw std::runtime_error("Matrix is nearly singular");
        }

        // k行目の正規化
        double a = 1.0 / sweep[k][k];
        for (j = 0; j < N * 2; j++) {
            sweep[k][j] *= a;
        }

        // 他の行の掃き出し
        for (i = 0; i < N; i++) {
            if (i == k) {
                continue;
            }
            a = -sweep[i][k];
            for (j = 0; j < N * 2; j++) {
                sweep[i][j] += sweep[k][j] * a;
            }
        }
    }

    // 結果を返す
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            inv[i][j] = sweep[i][j + N];
        }
    }

    // 結果の検証（オプション）
    // 元の行列と計算した逆行列の積が単位行列に近いかチェック
    double max_error = 0.0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            double sum = 0.0;
            for (k = 0; k < N; k++) {
                sum += mat[i][k] * inv[k][j];
            }
            if (i == j) {
                max_error = std::max(max_error, std::abs(sum - 1.0));
            } else {
                max_error = std::max(max_error, std::abs(sum));
            }
        }
    }

    if (max_error > epsilon) {
        throw std::runtime_error("Inverse matrix verification failed");
    }

    return inv;
}

//	アフィン変換
void CSampleDlg::OnAffine()
{
	const double PI = acos(-1);	//	円周率
	const int tsize = 148;		//	元画像の幅

	std::array<std::array<std::array<double, 3>, 3>, 7> A;	//	変換行列のあつまり
	std::array<std::array<double, 3>, 3> invA;	//	逆行列
	double rate;	//	拡大・縮小率
	double theta;	//	回転角[rad]
	double phi;		//	剪断角[rad]
	int x1, y1;		//	平行移動
	int xp, yp;		//	変換後の座標
	int xr, yr;		//	逆変換後の座標
	int xp_min, xp_max, yp_min, yp_max;	//	変換後の端

	//	拡大
	rate = 1.3;
	x1 = 150;
	y1 = 0;
	A[0] = { {
		{rate, 0, (double)x1},
		{0, rate, 0},
		{0, 0, 1}
	} };

	//	縮小
	rate = 0.7;
	x1 = 350;
	y1 = 0;
	A[1] = { {
		{0.7, 0, 350.0},
		{0, 0.7, 0},
		{0, 0, 1}
	} };

	//	回転
	theta = 11 * PI / 12;	//	165°
	x1 = int(tsize / 2);
	y1 = 250;
	A[2] = { {
		{cos(theta), -sin(theta), -(tsize / 2) * cos(theta) + (tsize / 2) * sin(theta) + x1},
		{sin(theta), cos(theta), -(tsize / 2) * sin(theta) - (tsize / 2) * cos(theta) + y1},
		{0, 0, 1}
	} };

	//	剪断
	phi = 23 * PI / 180;		//	23°
	x1 = 150;
	y1 = 150;
	A[3] = { {
		{1, 0, (double)x1},
		{tan(phi), 1, (double)y1},
		{0, 0, 1}
	} };

	//	組み合わせ１（回転・縮小）
	theta = 4 * PI / 3;
	rate = 0.9;
	x1 = int(tsize / 2);
	y1 = 500;
	A[4] = { {
		{rate * cos(theta),-rate * sin(theta), rate * (-(tsize / 2) * cos(theta) + (tsize / 2) * sin(theta)) + x1},
		{rate * sin(theta), rate * cos(theta), rate * ((tsize / 2) * sin(theta) - (tsize / 2) * sin(theta)) + y1},
		{0, 0, 1}
	} };

	//	組み合わせ２（拡大・剪断）
	rate = 1.1;
	phi = 5 * PI / 6;
	x1 = 170;
	y1 = 380;
	A[5] = {{
		{rate, 0, (double)x1},
		{rate * tan(phi), rate, (double)y1},
		{0, 0, 1}
	}};

	//	組み合わせ３（剪断・回転）
	theta = PI / 4;
	phi = PI / 4;
	x1 = 470;
	y1 = 280;
	A[6] = { {
		{cos(theta) - sin(theta) * tan(phi), -sin(theta), -(tsize / 2) * cos(theta) + (tsize / 2) * sin(theta) + x1},
		{sin(theta) + cos(theta) * tan(phi),  cos(theta), -(tsize / 2) * sin(theta) - (tsize / 2) * cos(theta) + y1},
		{0, 0, 1}
	} };

	//	アフィン変換
	for (int i = 0; i < A.size(); i++) {
		xp_min = yp_min = INT_MAX;
		xp_max = yp_max = INT_MIN;

		invA = CalcInverseMatrix(A[i]);

		//	変換後の領域の端を調べる
		for (int y = 0; y < tsize; y++) { 
			for (int x = 0; x < tsize; x++) {
				xp = A[i][0][0] * x + A[i][0][1] * y + A[i][0][2];
				yp = A[i][1][0] * x + A[i][1][1] * y + A[i][1][2];
				xp_min = xp < xp_min ? xp : xp_min;
				yp_min = yp < yp_min ? yp : yp_min;
				xp_max = xp > xp_max ? xp : xp_max;
				yp_max = yp > yp_max ? yp : yp_max;
			}
		}

		//	変換後の領域を調整
		xp_min = xp_min < 0 ? 0 : xp_min;
		yp_min = yp_min < 0 ? 0 : yp_min;
		xp_max = xp_max >= m_BmpInfo->bmiHeader.biWidth  ? m_BmpInfo->bmiHeader.biWidth  - 1 : xp_max;
		yp_max = yp_max >= m_BmpInfo->bmiHeader.biHeight ? m_BmpInfo->bmiHeader.biHeight - 1 : yp_max;

		//	補間
		for (int y = yp_min; y <= yp_max; y++) {
			for (int x = xp_min; x <= xp_max; x++) {
				double dx, dy;	//	xr,yrの小数部分
				xr = int(invA[0][0] * x + invA[0][1] * y + invA[0][2]);
				yr = int(invA[1][0] * x + invA[1][1] * y + invA[1][2]);
				if (xr < 0 || xr >= tsize || yr < 0 || yr >= tsize) {	//	逆変換後の座標が元画像の領域外
					continue;
				}
				dx = (invA[0][0] * x + invA[0][1] * y + invA[0][2]) - xr;
				dy = (invA[1][0] * x + invA[1][1] * y + invA[1][2]) - yr;
				m_Image->R[y][x] = (1 - dx) * (1 - dy) * m_Image->R[yr][xr] + dx * (1 - dy) * m_Image->R[yr][xr + 1] + (1 - dx) * dy * m_Image->R[yr + 1][xr] + dx * dy * m_Image->R[yr + 1][xr + 1];
				m_Image->G[y][x] = (1 - dx) * (1 - dy) * m_Image->G[yr][xr] + dx * (1 - dy) * m_Image->G[yr][xr + 1] + (1 - dx) * dy * m_Image->G[yr + 1][xr] + dx * dy * m_Image->G[yr + 1][xr + 1];
				m_Image->B[y][x] = (1 - dx) * (1 - dy) * m_Image->B[yr][xr] + dx * (1 - dy) * m_Image->B[yr][xr + 1] + (1 - dx) * dy * m_Image->B[yr + 1][xr] + dx * dy * m_Image->B[yr + 1][xr + 1];
			}
		}
	}

	//	描画
	for (int i = 0; i < m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_BmpInfo->bmiHeader.biWidth; j++) {
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3] = m_Image->B[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 1] = m_Image->G[i][j];
			m_BmpImage[((m_BmpInfo->bmiHeader.biHeight - 1 - i) * m_BmpInfo->bmiHeader.biWidth + j) * 3 + 2] = m_Image->R[i][j];
		}
	}
	Invalidate();
}

