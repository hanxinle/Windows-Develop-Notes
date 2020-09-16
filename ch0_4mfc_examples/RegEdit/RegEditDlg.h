// RegEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegEditDlg dialog

class CRegEditDlg : public CDialog
{
// Construction
public:
	CRegEditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRegEditDlg)
	enum { IDD = IDD_REGEDIT_DIALOG };
	int		m_iRootSelected;
	CString	m_strSubKey;
	CString	m_strValueName;
	CString	m_strValue;
	CString	m_strSubKeyName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRegEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCreateSubkey();
	afx_msg void OnDeleteSubkey();
	afx_msg void OnDeleteValuename();
	afx_msg void OnQueryInfokey();
	afx_msg void OnRadioLocalMachine();
	afx_msg void OnRadioCurrentUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	char  m_cstSubKeyName[200];
	HKEY  hMyKey;
public:
	afx_msg void OnBnClickedCancel();
};
