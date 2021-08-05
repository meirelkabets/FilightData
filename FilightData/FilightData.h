// FilightData.h : main header file for the FilightData application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CFilightDataApp:
// See FilightData.cpp for the implementation of this class
//

class CFilightDataApp : public CBCGPWinApp
{
public:
	CFilightDataApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFilightDataApp theApp;
