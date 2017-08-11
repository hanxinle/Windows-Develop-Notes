#if !defined(AFX_LISTDLG_H__DDBA1610_B4B1_4DDA_9755_E22F4EA57456__INCLUDED_)
#define AFX_LISTDLG_H__DDBA1610_B4B1_4DDA_9755_E22F4EA57456__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListDlg dialog

class CListDlg : public CDialog
{
// Construction
public:
	CListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CListDlg)
	enum { IDD = IDD_LIST_DIALOG };
	CStatic	m_ListT;
	CListBox	m_knmlist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CListDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTDLG_H__DDBA1610_B4B1_4DDA_9755_E22F4EA57456__INCLUDED_)
