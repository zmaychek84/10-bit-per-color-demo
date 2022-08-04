
// 10bpc_demo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// C10bpcdemoApp:
// See 10bpc_demo.cpp for the implementation of this class
//

class C10bpcdemoApp : public CWinApp
{
public:
	C10bpcdemoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern C10bpcdemoApp theApp;
