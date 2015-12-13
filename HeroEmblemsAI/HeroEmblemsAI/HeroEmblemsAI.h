
// HeroEmblemsAI.h : main header file for the HeroEmblemsAI application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CHeroEmblemsAIApp:
// See HeroEmblemsAI.cpp for the implementation of this class
//

class CHeroEmblemsAIApp : public CWinApp
{
public:
	CHeroEmblemsAIApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CHeroEmblemsAIApp theApp;
