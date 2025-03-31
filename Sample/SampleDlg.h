// SampleDlg.h : ヘッダー ファイル
//
#pragma once
#include "Image.h"
#include "CHistDlg.h"
class CHistDlg;
#include <opencv2/opencv.hpp>
using namespace cv;


// CSampleDlg ダイアログ
class CSampleDlg : public CDialogEx
{
public:
	//	コンストラクション
	CSampleDlg(CWnd* pParent = nullptr);
	//	デストラクター
	~CSampleDlg();

	//画像読み込みのためのメンバ変数の宣言
	unsigned char* m_BmpImage;      //イメージデータを格納する変数
	LPBITMAPINFO m_BmpInfo;		//BMPヘッダ情報を格納する変数
	BITMAPFILEHEADER m_BmpFileHdr;  //BMPファイル情報を格納する変数
	Image* m_Image; 		//画像データを格納する変数

	//	テンプレート画像読みこみのためのメンバ変数の宣言
	unsigned char* tmpBmpImage;      //イメージデータを格納する変数
	LPBITMAPINFO tmpBmpInfo;		//BMPヘッダ情報を格納する変数
	BITMAPFILEHEADER tmpBmpFileHdr;  //BMPファイル情報を格納する変数
	Image* tmpImage; 		//画像データを格納する変数

	//	元画像保存用
	unsigned char* m_OrgImage;				//		イメージデータを格納する変数
	BITMAPFILEHEADER m_OrgBmpFileHdr;		//		BMPヘッダ情報を格納する変数
	BITMAPINFO m_OrgBmpInfo;				//		BMPファイル情報を格納する変数
	unsigned char* tmpOrgImage;
	BITMAPFILEHEADER tmpOrgBmpFileHdr;
	BITMAPINFO tmpOrgBmpInfo;

	//ラジオボタン用メンバ変数宣言
	CButton* radio1;
	CButton* radio2;
	CButton* radio3;
	CButton* radio4;

	//チェックボックス
	CButton* check1;

	//ヒストグラム用ダイアログ
	CHistDlg* m_HistDlg;

	//ラベリング
	int max_label;
	int** m_LabelMap;	//	画素ごとのラベル
	int* m_LookupTable;	//	ルックアップテーブル

	//	USBカメラ1
	VideoCapture capture; //USBカメラ
	Mat input; //カメラから画像を取得
	//	フレーム間差分
	Image* I1, * I2, * I3;
	int img_sp;
	//	背景差分
	int** savedata;

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAMPLE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	//	ラベリング用関数群
	void GetNeighborLabels(int, int, int(&)[9], int*);
	int  UpdateLookupTable(int);
	void UpdateLookupLabel(int, int, int, int*);
	void FowardScanning(int*, int*);
	void ReverseScanning(int*, int*);
	void ResoluteConflict(int);
	void RemoveNoises(int, int);
	void InitLabels(void);
	void DestroyLabels(void);
	//	拡大縮小
	void ImageScaling();
	//	ハフ変換
	void DispParamSpace(int** param, int width, int height);

public:
	afx_msg void OnOpen();
	void DrawImage();
	afx_msg void OnSave();
	void AutoSave(CString);
	afx_msg void OnKidoka();
	afx_msg void On2chika();
	int m_eth;
	afx_msg void OnEnChangeEditThreshold();
	int m_sth;
	afx_msg void OnNMReleasedcaptureSliderThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBack();
	//	メディアンフィルタ
	int MedianFilter(int, int, int);
	afx_msg void OnMedian();
	//	移動平均法
	int MoveAverageFilter(int, int, int);
	afx_msg void OnMoveAverage();
	//	各種フィルタ
	int Filter2D(int, int, int[][3]) const;
	afx_msg void OnEdgeDetection();
	//	新規ダイアログ
	afx_msg void OnShowNewDialog();
	//	コントラスト強調
	afx_msg void OnLinearDensity();
	afx_msg void OnFlatten();
	afx_msg void OnCompletelyFlatten();
	//	アニメーション
	afx_msg void Animation(int);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCallTimer();
	//	テンプレートマッチング
	afx_msg void OnOpenTemplate();
	afx_msg void OnTemplateMatching();
	//	相違度画像
	int calcDissimirality(int, int, int, int);
	afx_msg void OnDissimirality();
	//	ラベリング
	afx_msg void OnLabeling();
	void LabelPaint(int** m_Img_cp, int label_max);
	//	Webカメラ
	int GetBITMAPINFO(void);
	int GetBITMAPFILEHEADER(void);
	void Capture(void);
	void InitCamera(void);
	afx_msg void OnCamera();
	afx_msg void OnVideoCamera();
	afx_msg void OnDitectFaces();
	afx_msg void OnBackgroundDifference();
	afx_msg void OnFrameDifference();
	void BackDiff();
	void FrameDiff();
	//	拡大・縮小
	double m_rate;	//	倍率
	afx_msg void OnNearestNeighbor();
	afx_msg void OnLinearInter();
	afx_msg void OnNewTemplateMatching();
	//	膨張・収縮
	afx_msg void OnExpension();
	afx_msg void OnContraction();
	afx_msg void OnSkinExtraction();
	//	輪郭追跡
	afx_msg void OnContourTracing();
	//	ハフ変換
	afx_msg void OnHoughTramsform();
	//	アフィン変換
	afx_msg void OnAffine();
};

