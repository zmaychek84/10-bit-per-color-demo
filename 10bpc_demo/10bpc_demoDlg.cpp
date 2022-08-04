//********************************************************* 
// 
// Copyright (c) Intel. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************
// 10bpc_demoDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "10bpc_demo.h"
#include "10bpc_demoDlg.h"
#include "afxdialogex.h"
#include "PopupWindow.h"

// C10bpcdemoDlg dialog
const PWSTR g_appTitle = L"10bpc demo";
#if 1
namespace
{
	std::unique_ptr<PopupWindow> g_popupwindow;
};
#endif

C10bpcdemoDlg::C10bpcdemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY10BPC_DEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void C10bpcdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C10bpcdemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOPENFILE, &C10bpcdemoDlg::OnBnClickedOpenfile)
	ON_BN_CLICKED(IDCREATEPATTERN, &C10bpcdemoDlg::OnBnClickedCreatepattern)
END_MESSAGE_MAP()


// C10bpcdemoDlg message handlers

BOOL C10bpcdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void C10bpcdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	if (wcslen(mMediaFilePath) > 3 && mFirstTime)
	{
		DisplayImage();
		mFirstTime = FALSE;
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR C10bpcdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void C10bpcdemoDlg::OnBnClickedOpenfile()
{
	HRESULT hr = S_OK;

	CString fileName;
	wchar_t* p = fileName.GetBuffer(MAX_PATH + 1);
	CFileDialog dlgFile(TRUE);
	
	OPENFILENAME& ofn = dlgFile.GetOFN();
	ofn.Flags |= OFN_ALLOWMULTISELECT;
	ofn.lpstrFile = p;
	ofn.nMaxFile = MAX_PATH + 1;		

	ofn.lpstrFilter = L"Image Files(*.bmp, *.png, *.jpg, *.tif)\0*.bmp;*.png;*.jpg;*.tif;\0";

	dlgFile.DoModal();
    
    if (5 > fileName.GetLength())
	{
		mMediaFilePath = fileName;
        LPWSTR pExtn = PathFindExtension(mMediaFilePath);

        DisplayImage();
        
	}
}

void C10bpcdemoDlg::DisplayPattern()
{
	g_popupwindow = std::make_unique<PopupWindow>();

	HRESULT ret = g_popupwindow->CreatePopupWindow(true, NULL);
	g_popupwindow.reset();
}
void C10bpcdemoDlg::DisplayImage()
{
	g_popupwindow = std::make_unique<PopupWindow>();

	HRESULT ret= g_popupwindow->CreatePopupWindow(false, (LPWSTR)mMediaFilePath.GetString());
	g_popupwindow.reset();
}
void C10bpcdemoDlg::OnBnClickedCreatepattern()
{
	
	DisplayPattern();
}
