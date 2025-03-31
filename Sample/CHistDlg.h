#pragma once
#include <afxwin.h>

#include "resource.h"
#include "Sample.h"
class CSampleDlg;

class CHistDlg : public CDialog
{
public:
    //コンストラクタ・デストラクタ
    CHistDlg(CWnd* pParent = NULL);
    virtual ~CHistDlg();

    //メンバ変数
    CSampleDlg* m_pParent;
    HDC hDC;
    enum { IDD = IDD_HIST_DIALOG };
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnSave();
};

