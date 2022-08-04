
// 10bpc_demoDlg.h : header file
//

#pragma once


// C10bpcdemoDlg dialog
class C10bpcdemoDlg : public CDialogEx
{
// Construction
public:
	C10bpcdemoDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY10BPC_DEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL mFirstTime;
	CString mMediaFilePath;


	void DisplayImage();
	void DisplayPattern();
	afx_msg void OnBnClickedOpenfile();
	afx_msg void OnBnClickedCreatepattern();
};
