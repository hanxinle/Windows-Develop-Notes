
// CFileDlg.h : 头文件
//

#pragma once


// CCFileDlg 对话框
class CCFileDlg : public CDialogEx
{
// 构造
public:
	CCFileDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CFILE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedRead();
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	

	CString m_strName;
	CString m_strWrite;
	CString m_strRead;

public:

};
