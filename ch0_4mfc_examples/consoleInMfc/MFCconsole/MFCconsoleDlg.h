// MFCconsoleDlg.h : header file
//

#if !defined(AFX_MFCCONSOLEDLG_H__9A5D247C_4861_4299_8DB8_0FE2E4460F36__INCLUDED_)
#define AFX_MFCCONSOLEDLG_H__9A5D247C_4861_4299_8DB8_0FE2E4460F36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMFCconsoleDlg dialog

class CMFCconsoleDlg : public CDialog
{
// Construction
	//CString m_strTest;
public:
	CMFCconsoleDlg(CWnd* pParent = NULL);	// standard constructor
    CString m_strTest;
// Dialog Data
	//{{AFX_DATA(CMFCconsoleDlg)
	enum { IDD = IDD_MFCCONSOLE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
 
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCconsoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMFCconsoleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCCONSOLEDLG_H__9A5D247C_4861_4299_8DB8_0FE2E4460F36__INCLUDED_)
