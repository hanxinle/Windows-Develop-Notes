// ChatRoomDlg.h : header file
//

#pragma once
#include "resource.h"
#include "afxwin.h"
#include "Inc.h"


// CChatRoomDlg dialog
class CChatRoomDlg : public CDialog {
    // Construction
public:
    CChatRoomDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    enum { IDD = IDD_CHATROOM_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    void DlgAllInit();
    CButton m_StopClientBtn;
    BOOL EnableWindow(UINT uID, BOOL bEnable = TRUE);
    void ExtendDiaog(BOOL bShow);
    afx_msg void OnBnClickedNetset();
    BOOL bShowAll;
    afx_msg void OnBnClickedStartServer();
    SOCKET m_ListenSock;
    SOCKET m_ConnectSock;
    HANDLE m_hListenThread;
    HANDLE m_hConnectThred;
    CArray <CClientItem, CClientItem> m_ClientArray;
    CEdit m_MsgEdit;
    void ShowMsg(CString strMsg);
    void RemoveClientFromArray(CClientItem in_Item);
    afx_msg void OnBnClickedSendmsg();
    afx_msg void OnBnClickedStartClient();
    void SendClientsMsg(CString strMsg, CClientItem *pNotSend = NULL);
    int m_bIsServer;
    afx_msg void OnEnChangeInputMsg();
    void StopClient();
    void StopServer();
    BOOL bShutDown;
    afx_msg void OnBnClickedStopClient();
    afx_msg void OnBnClickedStopServer();
    afx_msg void OnBnClickedRadioClient();
    afx_msg void OnBnClickedRadioServer();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOther();
    BOOL TrayMyIcon(BOOL bAdd = TRUE);
    afx_msg void OnMenuTrayinco();
    LRESULT OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam);
    afx_msg void OnMenuShow();
    afx_msg void OnMenuExit();
};
