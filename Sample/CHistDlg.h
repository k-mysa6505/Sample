#pragma once
#include <afxwin.h>

#include "resource.h"
#include "Sample.h"
class CSampleDlg;

class CHistDlg : public CDialog
{
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CHistDlg(CWnd* pParent = NULL);
    virtual ~CHistDlg();

    //�����o�ϐ�
    CSampleDlg* m_pParent;
    HDC hDC;
    enum { IDD = IDD_HIST_DIALOG };
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnSave();
};

