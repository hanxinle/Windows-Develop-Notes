// MFCconsole.h : main header file for the MFCCONSOLE application
//

#if !defined(AFX_MFCCONSOLE_H__4391F1B6_F3C4_48DD_BBB3_E5B0595F0BB9__INCLUDED_)
#define AFX_MFCCONSOLE_H__4391F1B6_F3C4_48DD_BBB3_E5B0595F0BB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFCconsoleApp:
// See MFCconsole.cpp for the implementation of this class
//

class CMFCconsoleApp : public CWinApp
{
public:
	CMFCconsoleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCconsoleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMFCconsoleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCCONSOLE_H__4391F1B6_F3C4_48DD_BBB3_E5B0595F0BB9__INCLUDED_)
