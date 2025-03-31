#include "pch.h"
#include "CHistDlg.h"
#include "SampleDlg.h"

//コンストラクタ
CHistDlg::CHistDlg(CWnd* pParent) : CDialog(CHistDlg::IDD, pParent) {
	m_pParent = (CSampleDlg*)pParent;
	hDC = NULL;
}

//デストラクタ
CHistDlg::~CHistDlg() {
	m_pParent = NULL;
}BEGIN_MESSAGE_MAP(CHistDlg, CDialog)
ON_WM_PAINT()
ON_BN_CLICKED(IDC_SAVE, &CHistDlg::OnSave)
END_MESSAGE_MAP()


void CHistDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ここにメッセージ ハンドラー コードを追加します。
	// 描画メッセージで CDialog::OnPaint() を呼び出さないでください。

	//描画処理
	//デバイスコンテキストのハンドル
	hDC = ((CStatic*)GetDlgItem(IDC_HIST_PICTBOX))->GetDC()->GetSafeHdc();

	//ペンの作成
	CPen hWhitePen;
	hWhitePen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen hBlackPen;
	hBlackPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	//白ペンを持つ
	::SelectObject(hDC, hWhitePen);
	//描画エリアを真っ白に塗りつぶす
	Rectangle(hDC, 0, 0, 256, 256);

	//赤ペンを持つ
	::SelectObject(hDC, hBlackPen);
	//線を引く
	//::MoveToEx(hDC, 5, 5, NULL);
	//::LineTo(hDC, 50, 50);

	//ヒストグラム作成
	int lum_list[256] = { 0 };
	int lum_max = 0;
	for (int i = 0; i < m_pParent->m_BmpInfo->bmiHeader.biHeight; i++) {
		for (int j = 0; j < m_pParent->m_BmpInfo->bmiHeader.biWidth; j++) {
			int val = m_pParent->m_Image->R[i][j];
			lum_list[val]++;
			if (lum_list[val] > lum_max) {
				lum_max = lum_list[val];
			}
		}
	}
	for (int i = 0; i < 256; i++) {
		::MoveToEx(hDC, i, 255, NULL);  
		::LineTo(hDC, i, 255 - ((float)(lum_list[i]) / lum_max * 256 / 2));
	}

	//後処理
	hWhitePen.DeleteObject();
	hBlackPen.DeleteObject();
}

void CHistDlg::OnSave()
{
	//保存処理
	CString filename;

	RECT rc;
	HDC hCompatibleDC;
	HBITMAP hBitmap, hBitmapPrev;
	BITMAPINFO* pbmi;
	void* pbits;
	HANDLE hfile;
	DWORD tmp;
	BITMAPFILEHEADER bmfh;

	//ビットマップインフォを作成。
	HWND hWnd = CHistDlg::m_hWnd;

	//ビットマップをセーブする範囲(Client領域)を取得する
	::GetClientRect(hWnd, &rc);
	GetDlgItem(IDC_HIST_PICTBOX)->GetClientRect(&rc);

	hDC = ::GetDC(hWnd);
	pbmi = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)];

	//BITMAPFILEHEADERの作成
	ZeroMemory(&pbmi->bmiHeader, sizeof(pbmi->bmiHeader));
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = 256; //幅
	pbmi->bmiHeader.biHeight = 256; //高さ 
	pbmi->bmiHeader.biPlanes = 1;

	//１ピクセルあたりビット数を格納
	pbmi->bmiHeader.biBitCount = GetDeviceCaps(hDC, BITSPIXEL);

	//ビットマップのバイト数を格納
	//(今時の環境では、色は１ピクセル=４バイト表現なので、単純に*4とした。)
	pbmi->bmiHeader.biSizeImage = pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biHeight * 4;

	//DIB を作成してウインドウからビットマップをコピー
	hBitmap = CreateDIBSection(hDC, pbmi, DIB_RGB_COLORS, &pbits, NULL, 0);
	hCompatibleDC = CreateCompatibleDC(hDC);
	hBitmapPrev = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);
	BitBlt(hCompatibleDC, 0, 0, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight, hDC, rc.left, rc.top, SRCCOPY);
	SelectObject(hCompatibleDC, hBitmapPrev);
	::ReleaseDC(hWnd, hDC);

	//ビットマップファイルヘッダーを作成。
	bmfh.bfType = 0x4D42;   //BitMapファイルを表すコード
	//("BM"をWORD型に格納したもので、決まりごとです。)
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfh.bfSize = bmfh.bfOffBits + pbmi->bmiHeader.biSizeImage;

	//ファイルダイアログを表示してファイル指定する
	CFileDialog myDLG(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "ﾋﾞｯﾄﾏｯﾌﾟ(*.BMP)|*.BMP||");
	if (myDLG.DoModal() != IDOK) return;

	//取得したファイルをオープンする
	filename = myDLG.GetPathName() + ".bmp";

	//DIB をファイルへセーブ
	hfile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	WriteFile(hfile, &bmfh, sizeof(bmfh), &tmp, NULL);
	WriteFile(hfile, &pbmi->bmiHeader, sizeof(pbmi->bmiHeader), &tmp, NULL);
	SetFilePointer(hfile, bmfh.bfOffBits, NULL, FILE_BEGIN);
	WriteFile(hfile, pbits, pbmi->bmiHeader.biSizeImage, &tmp, NULL);
	CloseHandle(hfile);

	//BITMAPINFOの解放
	delete[](BYTE*)pbmi;
}
