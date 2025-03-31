#include "pch.h"
#include "CHistDlg.h"
#include "SampleDlg.h"

//�R���X�g���N�^
CHistDlg::CHistDlg(CWnd* pParent) : CDialog(CHistDlg::IDD, pParent) {
	m_pParent = (CSampleDlg*)pParent;
	hDC = NULL;
}

//�f�X�g���N�^
CHistDlg::~CHistDlg() {
	m_pParent = NULL;
}BEGIN_MESSAGE_MAP(CHistDlg, CDialog)
ON_WM_PAINT()
ON_BN_CLICKED(IDC_SAVE, &CHistDlg::OnSave)
END_MESSAGE_MAP()


void CHistDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����܂��B
	// �`�惁�b�Z�[�W�� CDialog::OnPaint() ���Ăяo���Ȃ��ł��������B

	//�`�揈��
	//�f�o�C�X�R���e�L�X�g�̃n���h��
	hDC = ((CStatic*)GetDlgItem(IDC_HIST_PICTBOX))->GetDC()->GetSafeHdc();

	//�y���̍쐬
	CPen hWhitePen;
	hWhitePen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen hBlackPen;
	hBlackPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	//���y��������
	::SelectObject(hDC, hWhitePen);
	//�`��G���A��^�����ɓh��Ԃ�
	Rectangle(hDC, 0, 0, 256, 256);

	//�ԃy��������
	::SelectObject(hDC, hBlackPen);
	//��������
	//::MoveToEx(hDC, 5, 5, NULL);
	//::LineTo(hDC, 50, 50);

	//�q�X�g�O�����쐬
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

	//�㏈��
	hWhitePen.DeleteObject();
	hBlackPen.DeleteObject();
}

void CHistDlg::OnSave()
{
	//�ۑ�����
	CString filename;

	RECT rc;
	HDC hCompatibleDC;
	HBITMAP hBitmap, hBitmapPrev;
	BITMAPINFO* pbmi;
	void* pbits;
	HANDLE hfile;
	DWORD tmp;
	BITMAPFILEHEADER bmfh;

	//�r�b�g�}�b�v�C���t�H���쐬�B
	HWND hWnd = CHistDlg::m_hWnd;

	//�r�b�g�}�b�v���Z�[�u����͈�(Client�̈�)���擾����
	::GetClientRect(hWnd, &rc);
	GetDlgItem(IDC_HIST_PICTBOX)->GetClientRect(&rc);

	hDC = ::GetDC(hWnd);
	pbmi = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)];

	//BITMAPFILEHEADER�̍쐬
	ZeroMemory(&pbmi->bmiHeader, sizeof(pbmi->bmiHeader));
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = 256; //��
	pbmi->bmiHeader.biHeight = 256; //���� 
	pbmi->bmiHeader.biPlanes = 1;

	//�P�s�N�Z��������r�b�g�����i�[
	pbmi->bmiHeader.biBitCount = GetDeviceCaps(hDC, BITSPIXEL);

	//�r�b�g�}�b�v�̃o�C�g�����i�[
	//(�����̊��ł́A�F�͂P�s�N�Z��=�S�o�C�g�\���Ȃ̂ŁA�P����*4�Ƃ����B)
	pbmi->bmiHeader.biSizeImage = pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biHeight * 4;

	//DIB ���쐬���ăE�C���h�E����r�b�g�}�b�v���R�s�[
	hBitmap = CreateDIBSection(hDC, pbmi, DIB_RGB_COLORS, &pbits, NULL, 0);
	hCompatibleDC = CreateCompatibleDC(hDC);
	hBitmapPrev = (HBITMAP)SelectObject(hCompatibleDC, hBitmap);
	BitBlt(hCompatibleDC, 0, 0, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight, hDC, rc.left, rc.top, SRCCOPY);
	SelectObject(hCompatibleDC, hBitmapPrev);
	::ReleaseDC(hWnd, hDC);

	//�r�b�g�}�b�v�t�@�C���w�b�_�[���쐬�B
	bmfh.bfType = 0x4D42;   //BitMap�t�@�C����\���R�[�h
	//("BM"��WORD�^�Ɋi�[�������̂ŁA���܂育�Ƃł��B)
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfh.bfSize = bmfh.bfOffBits + pbmi->bmiHeader.biSizeImage;

	//�t�@�C���_�C�A���O��\�����ăt�@�C���w�肷��
	CFileDialog myDLG(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "�ޯ�ϯ��(*.BMP)|*.BMP||");
	if (myDLG.DoModal() != IDOK) return;

	//�擾�����t�@�C�����I�[�v������
	filename = myDLG.GetPathName() + ".bmp";

	//DIB ���t�@�C���փZ�[�u
	hfile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	WriteFile(hfile, &bmfh, sizeof(bmfh), &tmp, NULL);
	WriteFile(hfile, &pbmi->bmiHeader, sizeof(pbmi->bmiHeader), &tmp, NULL);
	SetFilePointer(hfile, bmfh.bfOffBits, NULL, FILE_BEGIN);
	WriteFile(hfile, pbits, pbmi->bmiHeader.biSizeImage, &tmp, NULL);
	CloseHandle(hfile);

	//BITMAPINFO�̉��
	delete[](BYTE*)pbmi;
}
